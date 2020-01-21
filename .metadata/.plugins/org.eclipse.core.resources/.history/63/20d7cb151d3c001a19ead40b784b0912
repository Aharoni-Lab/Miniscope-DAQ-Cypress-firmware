/*
 * miniscope.h
 *
 *  Created on: Jan 20, 2020
 *      Author: DBAharoni
 */

#ifndef MINISCOPE_H_
#define MINISCOPE_H_

#include "definitions.h"
#include <cyu3types.h>

// ------- Globals ---------------------
#define BNO055_ADDR		0b01010000
#define			I2C_PACKET_BUFFER_SIZE	64

extern CyBool_t recording;

extern CyBool_t endOfFrame;

extern CyBool_t bnoEnabled;// TODO: Enable this var when bnoEnable command gets sent

// Holds generic I2C commands waiting to be sent

extern uint8_t 	pendingI2CPacket[I2C_PACKET_BUFFER_SIZE][6]; //circular buffer
extern uint8_t			I2CNumPacketsPending;
extern uint8_t			I2CPacketNextReadIdx;
extern uint8_t			I2CPacketNextWriteIdx;
extern uint8_t			I2CPacketState;

// For BNO head orientation sensor
extern uint8_t quatBNO[8];

// Tracking frame number and timing
extern uint32_t dFrameNumber;
extern uint32_t currentTime;
//----------------------------------

// Handles the processing and sending of generic I2C packets that have built up since previous End of Frame event
extern void I2CProcessAndSendPendingPacket (void);

extern CyU3PReturnStatus_t readBNO(void);

#endif /* MINISCOPE_H_ */
