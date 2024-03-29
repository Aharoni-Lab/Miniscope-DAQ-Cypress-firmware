/*
 * definitions.h
 *
 *  Created on: Jan 20, 2020
 *      Author: DBAharoni
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "uvc.h"

/* ========= Pick your image sensor for the list below ====== */
#define MINISCOPE_V4_606X606
//#define SIMINISCOPE_2592_1944
//#define SIMINISCOPE_2XBIN_1296_972
//#define MINISCOPE_V3_752X480

#define SS_FC_UYVY_TOTAL_NO_OF_RES 9 /*  No. of resolutions supported */

#define FRAME_INDEX_608X608   1 // Used for standard Miniscope V4
#define FRAME_INDEX_2592X1944 2 // Used for full res MT9P031 (MiniLFM, Minicam, LFOV)
#define FRAME_INDEX_1296X972  3 // Used for 2xbinned MT9P031 (Minicam, LFOV)
#define FRAME_INDEX_752X480   4 // Used for Miniscope V3
#define FRAME_INDEX_1024X768  5 // Used for Minicam
#define FRAME_INDEX_800X800   6 // Used for LFOV scope
#define FRAME_INDEX_1000X1000 7 // Used for LFOV Miniscope / MiniCAM
#define FRAME_INDEX_1500X1500 8 // Used for LFOV Miniscope / MiniCAM
#define FRAME_INDEX_1800X1800 9 // Used for LFOV Miniscope / MiniCAM
/* ========================================================== */

// Sets specs of image sensor being use sensors is being used.
#ifdef MINISCOPE_V4_606X606
#define X_ASPECT_RATIO 1
#define Y_ASPECT_RATIO 1
#define WIDTH          608
#define HEIGHT         608
#endif
#ifdef SIMINISCOPE_2592_1944
#define X_ASPECT_RATIO 0x04
#define Y_ASPECT_RATIO 0x03
#define WIDTH          2592
#define HEIGHT         1944
#endif
#ifdef SIMINISCOPE_2XBIN_1296_972
#define X_ASPECT_RATIO 0x04
#define Y_ASPECT_RATIO 0x03
#define WIDTH          1296
#define HEIGHT         972
#endif
#ifdef MINISCOPE_V3_752X480
#define X_ASPECT_RATIO 0x2F
#define Y_ASPECT_RATIO 0x1E
#define WIDTH          752
#define HEIGHT         480
#endif

#define WIDTH_L  (uint8_t) (WIDTH & 0xFF)
#define WIDTH_H  (uint8_t) ((WIDTH >> 8) & 0xFF)
#define HEIGHT_L (uint8_t) (HEIGHT & 0xFF)
#define HEIGHT_H (uint8_t) ((HEIGHT >> 8) & 0xFF)

// Communication over saturation channel
#define RECORD_STATUS_MASK 0x01
#define CMD_QUERY_FW_ABI   0x02

#define MODE_V4_MINISCOPE 0x20
#define MODE_DEMO_2_COLOR 0x21
#define MODE_2_COLOR      0x22

#define CONFIG_BNO055_TO_NDOF 0xF0

// DAQ GPIO
#define FRAME_OUT       20
#define TRIG_RECORD_EXT 21

#define GPIO_SHIFT 20
#define GPIO_MASK  0b00000111

#define MCU_SET_MODE 0x04

// --------- Used for calculating descriptor sizes -----------
#define SS_YUY2_STILL_DESCR_SIZE      0 // We aren't using still image descriptors (???)
#define SS_FC_TOTAL_SIZE_CLASS_DSCR_L CY_U3P_GET_LSB (SS_FC_TOTAL_SIZE_CLASS_DSCR)
#define SS_FC_TOTAL_SIZE_CLASS_DSCR_H CY_U3P_GET_MSB (SS_FC_TOTAL_SIZE_CLASS_DSCR)

#define SS_FC_TOTAL_SIZE_CLASS_DSCR                                                                          \
    (0x0E + 0x1B + SS_YUY2_STILL_DESCR_SIZE +                                                                \
     (SS_FC_UYVY_TOTAL_NO_OF_RES * 0x1E)) // removed "+ 0x06" which is likely color matching descriptor

/* Low byte - Total size of the device descriptor */
#define SS_TOTAL_SIZE_CONFIG_DSCR_L CY_U3P_GET_LSB (SS_TOTAL_SIZE_CONFIG_DSCR)
/* High byte - Total size of the device descriptor */
#define SS_TOTAL_SIZE_CONFIG_DSCR_H CY_U3P_GET_MSB (SS_TOTAL_SIZE_CONFIG_DSCR)

/* Total size of the device descriptor.
 * Note: The hex values indicates size of the device descriptor along with the sub-descriptors */
/* Length of this descriptor and all sub descriptors *
 * 0x09 - configuration descriptor size
 * 0x8A- sum of all descriptors for a UVC camera other than video streaming class specific desc
 * TOTAL_SIZE_CLASS_DSCR - Video Streaming class specific descriptor
 */
#ifdef USB_DEBUG_INTERFACE
#define SS_TOTAL_SIZE_CONFIG_DSCR                                                                            \
    (0x09 + 0x8A + SS_FC_TOTAL_SIZE_CLASS_DSCR + 35) // SS_TOTAL_SIZE_CLASS_DSCR)
#else
#define SS_TOTAL_SIZE_CONFIG_DSCR (0x09 + 0x8A + SS_FC_TOTAL_SIZE_CLASS_DSCR) // SS_TOTAL_SIZE_CLASS_DSCR)
#endif
/* Total descriptor size (SS_TOTAL_SIZE_CONFIG_DSCR) = Configuration Descriptor size +
Interface Association Descriptor size +
Standard Video Control Interface Descriptor size +
Class specific VC Interface Header Descriptor size +
Input (Camera) Terminal Descriptor size +
Processing Unit Descriptor size +Extension Unit Descriptor size +
Output Terminal Descriptor size+
 Video Control Status Interrupt Endpoint Descriptor size +
 Super Speed Endpoint Companion Descriptor size+
 Class Specific Interrupt Endpoint Descriptor size +
 Standard Video Streaming Interface Descriptor size +
 SS_FC_TOTAL_SIZE_CLASS_DSCR+ Endpoint Descriptor for BULK Streaming Video Data size +
 Super Speed Endpoint Companion Descriptor size */
// ---------------------------------------------------------------------------------------

#endif /* DEFINITIONS_H_ */
