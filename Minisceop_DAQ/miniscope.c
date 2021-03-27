/*
 * miniscope.c
 *
 *  Created on: Jan 20, 2020
 *      Author: DBAharoni
 */

#include <cyu3system.h>
#include <cyu3os.h>
#include <cyu3dma.h>
#include <cyu3error.h>
#include <cyu3uart.h>
#include <cyu3i2c.h>
#include <cyu3types.h>
#include <cyu3gpio.h>
#include <cyu3utils.h>

#include "miniscope.h"

// ------------- Initialize globals ----
uint32_t dFrameNumber = 0;
uint32_t currentTime = 0;

CyBool_t recording = CyFalse;

CyBool_t endOfFrame = CyFalse;

CyBool_t bnoEnabled = CyFalse;

uint8_t quatBNO[8] = {0,0,0,0,0,0,0,0};

// For EWL focal plane jumping
uint8_t ewlPlaneNumber = 0;
uint8_t ewlNumPlanes = 0;
uint8_t ewlPlaneValue[3] = {0,0,0};
// ----------

/**
 *  Initializes our I2C packet ringbuffer using a preallocated structure.
 *  @return 0 on success.
 */
int i2c_packet_queue_init (I2CPacketQueue *pq)
{
    pq->pendingCount = 0;
    pq->idxRD = 0;
    pq->idxWR = 0;
    pq->curPacketParts = I2C_PACKET_PART_NONE;

    if (CyU3PMutexCreate (&pq->lock, CYU3P_NO_INHERIT) != CY_U3P_SUCCESS)
        return -1;
    return 0;
}

void i2c_packet_queue_free (I2CPacketQueue *pq)
{
    CyU3PMutexPut(&pq->lock);
    CyU3PMutexDestroy(&pq->lock);
}

void i2c_packet_queue_lock (I2CPacketQueue *pq)
{
    CyU3PMutexGet (&(pq->lock), CYU3P_WAIT_FOREVER);
}

void i2c_packet_queue_unlock (I2CPacketQueue *pq)
{
    CyU3PMutexPut (&(pq->lock));
}

void i2c_packet_queue_wrnext_if_complete (I2CPacketQueue *pq, I2CPacketPartFlags flag_added)
{
    pq->curPacketParts = pq->curPacketParts | flag_added;
    if (pq->curPacketParts != (I2C_PACKET_PART_HEAD | I2C_PACKET_PART_BODY | I2C_PACKET_PART_TAIL))
        return; /* we don't have a complete packet yet */

    /* advance in the write buffer */
    pq->idxWR = (pq->idxWR + 1) % I2C_PACKET_BUFFER_SIZE;
    pq->pendingCount++;

    /* we have no parts registered yet */
    pq->curPacketParts = I2C_PACKET_PART_NONE;
}

//----------------------------------

void handleEWLPlaneJumping(void) {

	if (ewlNumPlanes > 0) {

		CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
		CyU3PI2cPreamble_t preamble;
		uint8_t dataBuff[2];

		ewlPlaneNumber = (ewlPlaneNumber + 1) % ewlNumPlanes;

		preamble.buffer[0] = 0b11101110; // I2C Address
		preamble.buffer[1] = 0x08; // usual reg byte
		preamble.length    = 2; //register length + 1 (for address)
		preamble.ctrlMask  = 0x0000;

		dataBuff[0] = ewlPlaneValue[ewlPlaneNumber];
		dataBuff[1] = 0x02;

		apiRetStatus = CyU3PI2cTransmitBytes (&preamble, dataBuff, 2, 0);
		if (apiRetStatus == CY_U3P_SUCCESS)
			CyU3PBusyWait (100);
		else
			CyU3PDebugPrint (2, "I2C DAC WRITE command failed\r\n");

	}
}

uint8_t getNumPlanes(void) {
	return ewlNumPlanes;
}

void I2CProcessAndSendPendingPacket (I2CPacketQueue *pq)
{
	CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
	CyU3PI2cPreamble_t preamble;
	uint8_t dataBuff[9];
	uint8_t packetLength;
	int i;

	if (pq->pendingCount == 0)
		return;
	i2c_packet_queue_lock (pq);

	while (pq->pendingCount > 0) {
		if (pq->buffer[pq->idxRD][0] == 0xFE) {
			if (pq->buffer[pq->idxRD][1] == 2) {
				// User for configuring DAQ and not as I2C pass through
				handleDAQConfigCommand(pq->buffer[pq->idxRD][2]);
			}
			else if (pq->buffer[pq->idxRD][1] > 2 && pq->buffer[pq->idxRD][2] == 0x01){
				// Used for ewl focal plane jumping settings
				ewlPlaneNumber = 0;
				ewlNumPlanes = pq->buffer[pq->idxRD][1] - 2;
				for (uint8_t i = 0; i < ewlNumPlanes; i++) {
					ewlPlaneValue[i] = pq->buffer[pq->idxRD][3 + i];
				}

			}
		} else {
			if (pq->buffer[pq->idxRD][0] & 0x01) {
				// This denotes a full packet of 6 bytes.
				packetLength = 6; // Number of bytes in packet
				preamble.buffer[0] = pq->buffer[pq->idxRD][0]&0xFE; // I2C Address
				preamble.buffer[1] = pq->buffer[pq->idxRD][1]; // usual reg byte
				preamble.length    = 2; //register length + 1 (for address)
				preamble.ctrlMask  = 0x0000;

				for (i=0; i< (packetLength-2);i++){
					dataBuff[i] = pq->buffer[pq->idxRD][i+2];
				}
			}
			else {
				// less than 6 bytes in packet
				packetLength = pq->buffer[pq->idxRD][1]; // Number of bytes in packet
				preamble.buffer[0] = pq->buffer[pq->idxRD][0]; // I2C Address
				preamble.buffer[1] = pq->buffer[pq->idxRD][2]; // usual reg byte
				preamble.length    = 2; //register length + 1 (for address)
				preamble.ctrlMask  = 0x0000;

				for (i=0; i< (packetLength-2);i++){
					dataBuff[i] = pq->buffer[pq->idxRD][i+3];
				}
			}

			apiRetStatus = CyU3PI2cTransmitBytes (&preamble, dataBuff, packetLength - 2, 0);
			if (apiRetStatus == CY_U3P_SUCCESS)
				CyU3PBusyWait (100);
			else
				CyU3PDebugPrint (2, "I2C DAC WRITE command failed\r\n");
		}
		pq->idxRD = (pq->idxRD + 1) % I2C_PACKET_BUFFER_SIZE;
		pq->pendingCount--;
	}

	/* release lock on the queue */
	i2c_packet_queue_unlock (pq);
}

void handleDAQConfigCommand(uint8_t command) {
	switch(command) {
		case(0x00):
			bnoEnabled = CyTrue;
			break;

		default:
			break;
	}
}
CyU3PReturnStatus_t readBNO(void) {
	CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
	CyU3PI2cPreamble_t preamble;


	preamble.buffer[0] = BNO055_ADDR & 0xFE; /*  Mask out the transfer type bit. */
	preamble.buffer[1] = 0x20; // We can use 0x00 here for testing I2C communication stability
	preamble.buffer[2] = BNO055_ADDR | 0x01;
	preamble.length    = 3;
	preamble.ctrlMask  = 0x0002;                                /*  Send start bit after third byte of preamble. */

	apiRetStatus = CyU3PI2cReceiveBytes (&preamble, (uint8_t *)quatBNO, 8, 0);

	if (apiRetStatus == CY_U3P_SUCCESS) {
	        CyU3PBusyWait (200); //in us
	}

	return apiRetStatus;
}

void configurePin(uint8_t pinNum, CyBool_t inputEn, CyBool_t outValue, CyU3PGpioIntrMode_t intMode) {
	CyU3PGpioSimpleConfig_t     gpioConfig;
	CyU3PReturnStatus_t 		apiRetStatus;

	apiRetStatus = CyU3PDeviceGpioOverride (pinNum, CyTrue);
	if (apiRetStatus != 0)
	{
		CyU3PDebugPrint (4, "GPIO Override failed, Error Code = %d\n", apiRetStatus);
		CyFxAppErrorHandler (apiRetStatus);
	}

	gpioConfig.outValue    = outValue;
	gpioConfig.driveLowEn  = !inputEn;
	gpioConfig.driveHighEn = !inputEn;
	gpioConfig.inputEn     = inputEn;
	gpioConfig.intrMode    = intMode;
	apiRetStatus           = CyU3PGpioSetSimpleConfig (pinNum, &gpioConfig);
	if (apiRetStatus != CY_U3P_SUCCESS)
	{
		CyU3PDebugPrint (4, "GPIO Set Config Error, Error Code = %d\n", apiRetStatus);
		CyFxAppErrorHandler (apiRetStatus);
	}
//	if (inputEn) {
	apiRetStatus = CyU3PGpioSetIoMode (pinNum, CY_U3P_GPIO_IO_MODE_WPD);
	if (apiRetStatus != CY_U3P_SUCCESS)
	{
		CyU3PDebugPrint (4, "GPIO Set IO Mode Error, Error Code = %d\n", apiRetStatus);
		CyFxAppErrorHandler (apiRetStatus);
	}
//	}
}
void configureGPIOs(void) {

#ifdef FRAME_OUT
	//Output frame sync signal
	configurePin(FRAME_OUT,CyFalse,CyTrue);
#endif
#ifdef TRIG_RECORD_EXT
	// Input trigger
	configurePin(TRIG_RECORD_EXT,CyTrue,CyFalse);
#endif

#ifdef TIMER
	// Configure as a complex GPIO to be able to use timer as well for GPIO interrupt
	CyU3PGpioComplexConfig_t gpioConfig;
	CyU3PReturnStatus_t 		apiRetStatus;

	apiRetStatus = CyU3PDeviceGpioOverride (TIMER, CyFalse);
	gpioConfig.outValue    = CyFalse;
	gpioConfig.driveLowEn  = CyFalse;
	gpioConfig.driveHighEn = CyFalse;
	gpioConfig.inputEn     = CyFalse;
	gpioConfig.intrMode    = CY_U3P_GPIO_INTR_TIMER_THRES; // CY_U3P_GPIO_INTR_NEG_EDGE, CY_U3P_GPIO_INTR_BOTH_EDGE
	gpioConfig.timerMode   = CY_U3P_GPIO_TIMER_HIGH_FREQ; // CY_U3P_GPIO_TIMER_HIGH_FREQ
	gpioConfig.pinMode	   = CY_U3P_GPIO_MODE_STATIC;
	gpioConfig.timer       = 0;
	gpioConfig.period	   = 192000000;
	gpioConfig.threshold   = 192000000;

	CyU3PGpioSetComplexConfig(TIMER, &gpioConfig);
#endif

#ifdef LOCK_IN
	configurePin(LOCK_IN,CyTrue,CyFalse, CY_U3P_GPIO_INTR_NEG_EDGE);
#endif
#ifdef LED_RED
	// Input trigger
	configurePin(LED_RED,CyFalse,CyFalse, CY_U3P_GPIO_NO_INTR);
#endif
#ifdef LED_GREEN
	// Input trigger
	configurePin(LED_GREEN,CyFalse,CyFalse, CY_U3P_GPIO_NO_INTR);
#endif
}
