/****************************************************************************
 *
 * Copyright 2021 Lee Mitchell <lee@indigopepper.com>
 * This file is part of OCC (Orlaco Camera Configurator)
 *
 * OCC (Orlaco Camera Configurator) is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * OCC (Orlaco Camera Configurator) is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OCC (Orlaco Camera Configurator).  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "orlaco.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ORLACO_SOCKET_READ_TIMEOUT_MS   (100)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

// SOME/IP Methods
typedef enum {
    E_ORLACO_METHOD_ID_GET_DATA_SHEET                  = 0x0001,
    E_ORLACO_METHOD_ID_GET_CAM_STATUS                  = 0x0002,
    E_ORLACO_METHOD_ID_SET_CAM_MODE                    = 0x0003,    // Start, Stop, Restart the camera application
    E_ORLACO_METHOD_ID_SET_CAM_EXCLUSIVE               = 0x0011,
    E_ORLACO_METHOD_ID_ERASE_CAM_EXCLUSIVE             = 0x0019,
    E_ORLACO_METHOD_ID_SET_HOST_PARAMETERS             = 0x0022,
    E_ORLACO_METHOD_ID_GET_HOST_PARAMETERS             = 0x0024,
    E_ORLACO_METHOD_ID_ERASE_HOST_PARAMETERS           = 0x0029,
    E_ORLACO_METHOD_ID_SET_REGION_OF_INTEREST          = 0x0101,
    E_ORLACO_METHOD_ID_SET_REGIONS_OF_INTEREST         = 0x0102,
    E_ORLACO_METHOD_ID_GET_REGION_OF_INTEREST          = 0x0103,
    E_ORLACO_METHOD_ID_GET_REGIONS_OF_INTEREST         = 0x0104,
    E_ORLACO_METHOD_ID_ERASE_REGION_OF_INTEREST        = 0x0109,
    E_ORLACO_METHOD_ID_SET_VIDEO_FORMAT                = 0x0111,
    E_ORLACO_METHOD_ID_GET_VIDEO_FORMAT                = 0x0113,
    E_ORLACO_METHOD_ID_ERASE_VIDEO_FORMAT              = 0x0119,
    E_ORLACO_METHOD_ID_SET_HISTOGRAMM_FORMAT           = 0x0121,
    E_ORLACO_METHOD_ID_GET_HISTOGRAMM_FORMAT           = 0x0123,
    E_ORLACO_METHOD_ID_ERASE_HISTOGRAMM_FORMAT         = 0x0129,
    E_ORLACO_METHOD_ID_SUBSCRIBE_ROI_VIDEO             = 0x0131,
    E_ORLACO_METHOD_ID_UNSUBSCRIBE_ROI_VIDEO           = 0x0132,
    E_ORLACO_METHOD_ID_SUBSCRIBE_ROI_HISTOGRAMM        = 0x0133,
    E_ORLACO_METHOD_ID_UNSUBSCRIBE_ROI_HISTOGRAMM      = 0x0134,
    E_ORLACO_METHOD_ID_SET_CAM_CONTROL                 = 0x0201,
    E_ORLACO_METHOD_ID_SET_CAM_CONTROLS                = 0x0202,
    E_ORLACO_METHOD_ID_GET_CAM_CONTROL                 = 0x0203,
    E_ORLACO_METHOD_ID_GET_CAM_CONTROLS                = 0x0204,
    E_ORLACO_METHOD_ID_SET_CAM_REGISTER                = 0x0301,
    E_ORLACO_METHOD_ID_SET_CAM_REGISTERS               = 0x0302,
    E_ORLACO_METHOD_ID_GET_CAM_REGISTER                = 0x0303,
    E_ORLACO_METHOD_ID_GET_CAM_REGISTERS               = 0x0304,
    E_ORLACO_METHOD_ID_SET_USED_REGISTER_SET           = 0x0305,
    E_ORLACO_METHOD_ID_SERVICE_DISCOVERY               = 0x8100,
} ORLACO_teMethodID;


// SOME/IP Message Type
typedef enum {
    E_ORLACO_MESSAGE_TYPE_REQUEST                      = 0x00,
    E_ORLACO_MESSAGE_TYPE_REQUEST_NO_RETURN            = 0x01,
    E_ORLACO_MESSAGE_TYPE_NOTIFICATION                 = 0x02,
    E_ORLACO_MESSAGE_TYPE_RESPONSE                     = 0x80,
    E_ORLACO_MESSAGE_TYPE_ERROR                        = 0x81,
    E_ORLACO_MESSAGE_TYPE_TP_REQUEST                   = 0x20,
    E_ORLACO_MESSAGE_TYPE_TP_REQUEST_NO_RETURN         = 0x21,
    E_ORLACO_MESSAGE_TYPE_TP_NOTIFICATION              = 0x22,
    E_ORLACO_MESSAGE_TYPE_TP_RESPONSE                  = 0x23,
    E_ORLACO_MESSAGE_TYPE_TP_ERROR                     = 0x24,
} ORLACO_teMessageType;


// Make sure these remain in the same order as the register indexes
typedef enum {
    E_ORLACO_REGISTER_ADDRESS_LED_MODE                             = 0xb00c,
    E_ORLACO_REGISTER_ADDRESS_STREAM_PROTOCOL                      = 0xb041,
    E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_0                  = 0xb042,
    E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_1                  = 0xb043,
    E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_2                  = 0xb044,
    E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_3                  = 0xb045,
    E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_0                = 0xb046,
    E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_1                = 0xb047,
    E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_2                = 0xb048,
    E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_3                = 0xb049,
    E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_0                        = 0xb04a,
    E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_1                        = 0xb04b,
    E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_2                        = 0xb04c,
    E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_3                        = 0xb04d,
    E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_4                        = 0xb04e,
    E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_5                        = 0xb04f,
    E_ORLACO_REGISTER_ADDRESS_VLAN_ID_0                            = 0xb055,
    E_ORLACO_REGISTER_ADDRESS_VLAN_ID_1                            = 0xb056,
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_0                          = 0xb057,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_1                          = 0xb058,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_2                          = 0xb059,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_3                          = 0xb05a,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_4                          = 0xb05b,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_5                          = 0xb05c,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_6                          = 0xb05d,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_STREAM_ID_7                          = 0xb05e,  // 0xb057 - 0xb05e inclusive
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_0  = 0xb05f,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_1  = 0xb060,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_2  = 0xb061,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_3  = 0xb062,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_0 = 0xb063,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_1 = 0xb064,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_2 = 0xb065,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_3 = 0xb066,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_4 = 0xb067,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_5 = 0xb068,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_PORT_0        = 0xb069,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_PORT_1        = 0xb06a,
    E_ORLACO_REGISTER_ADDRESS_SELECTED_ROI                         = 0xb06b,
    E_ORLACO_REGISTER_ADDRESS_NO_STREAM_AT_BOOT                    = 0xb06c,
    E_ORLACO_REGISTER_ADDRESS_UDP_COMMUNICATION_PORT_0             = 0xb06d,
    E_ORLACO_REGISTER_ADDRESS_UDP_COMMUNICATION_PORT_1             = 0xb06e,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_SOURCE_PORT_0             = 0xb06f,
    E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_SOURCE_PORT_1             = 0xb070,
    E_ORLACO_REGISTER_ADDRESS_HDR                                  = 0xb071,
    E_ORLACO_REGISTER_ADDRESS_OVERLAY                              = 0xb072,
    E_ORLACO_REGISTER_ADDRESS_DHCP                                 = 0xb073,
    E_ORLACO_REGISTER_ADDRESS_WAIT_FOR_MAC                         = 0xb078,
    E_ORLACO_REGISTER_ADDRESS_WAIT_FOR_PTP_SYNC                    = 0xb079,
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_0                      = 0xb171,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_1                      = 0xb172,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_2                      = 0xb173,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_3                      = 0xb174,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_4                      = 0xb175,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_5                      = 0xb176,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_6                      = 0xb177,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_7                      = 0xb178,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_8                      = 0xb179,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_9                      = 0xb17a,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_10                     = 0xb17b,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_11                     = 0xb17c,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_12                     = 0xb17d,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_13                     = 0xb17e,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_14                     = 0xb17f,   // 0xb171 - 0xb180 inclusive
    E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_15                     = 0xb180,   // 0xb171 - 0xb180 inclusive
} ORLACO_teRegisterAddress;


typedef struct {
    uint16_t    u16Address;
    char        *pcDescription;
    char        *pcHelp;
} ORLACO_tsRegisterDefinition;

typedef struct {
    uint16_t u16Qtty;
    uint16_t au16RegisterAddress[ORLACO_MAX_REGISTERS];
} ORLACO_tsRegisterRequestsPayload;

typedef struct {
    uint16_t u16Qtty;
    ORLACO_tsRegisterValue asRegisterValues[ORLACO_MAX_REGISTERS];
} ORLACO_tsGetRegistersResponsePayload;

typedef struct {
    uint16_t u16Qtty;
    ORLACO_tsRegisterValue asRegisterValues[ORLACO_MAX_REGISTERS];
} ORLACO_tsSetRegistersRequestPayload;

typedef struct {
    uint32_t u32ExclusiveTime;
} ORLACO_tsSetCamExclusivePayload;

typedef struct {
    uint32_t u32Mode;
} ORLACO_tsSetCamModePayload;

typedef struct {
    uint32_t u32RegionOfInterestIndex;
    uint16_t u16P1X;
    uint16_t u16P1Y;
    uint16_t u16P2X;
    uint16_t u16P2Y;
    uint8_t u8Unknown1SetTo0x01;
    uint8_t u8Unknown2SetTo0x00;
    uint16_t u16Unknown3SetTo0x0000;

    uint16_t u16OutputWidth;
    uint16_t u16OutputHeight;

    uint8_t u8Unknown4SetTo0x00;
    uint8_t u8FrameRate;
    uint16_t u16Unknown4bSetTo0x0000;

    uint8_t u8Unknown5SetTo0x00;
    uint8_t u8Unknown6SetTo0x02;
    uint32_t u32MaxBitrate; // set to 0x00000032 = 50Mb/s
    uint8_t u8VideoCompressionMode; // 0x00 = none, 0x01 = JPEG, 0x02 = H.264

    uint8_t u8Unknown7SetTo0x00;
    uint8_t u8Unknown8SetTo0x00;
    uint8_t u8Unknown9SetTo0x00;

    uint8_t u8Unknown10SetTo0x04;
    uint8_t u8Unknown11SetTo0x01;

    uint16_t u16Unknown12SetTo0x00ff;

} ORLACO_tsSetRegionOfInterestPayload;

// New camera, payload from get roi 0 request
// 0000   00 00 00 00 05 00 03 c0 01 00 00 00 05 00 03 c0
// 0010   00 1e 00 00 00 02 00 00 00 32 01 00 00 00 04 01
// 0020   00 ff

typedef struct {
    uint32_t u32RegionOfInterest;
} ORLACO_tsGetRegionOfInterestRequestPayload;


typedef struct {
    uint32_t u32RegionOfInterest;
    uint16_t u16P1X;
    uint16_t u16P1Y;
    uint16_t u16P2X;
    uint16_t u16P2Y;
    uint8_t u8Unknown1SetTo0x01;
    uint8_t u8Unknown2SetTo0x00;
    uint16_t u16Unknown3SetTo0x0000;

    uint16_t u16OutputWidth;
    uint16_t u16OutputHeight;

    uint8_t u8Unknown4SetTo0x00;
    uint8_t u8FrameRate;
    uint16_t u16Unknown4bSetTo0x0000;

    uint8_t u8Unknown5SetTo0x00;
    uint8_t u8Unknown6SetTo0x02;
    uint32_t u32MaxBitrate; // set to 0x00000032 = 50Mb/s
    uint8_t u8VideoCompressionMode; // 0x00 = none, 0x01 = MJPEG, 0x02 = H.264

    uint8_t u8Unknown7SetTo0x00;
    uint8_t u8Unknown8SetTo0x00;
    uint8_t u8Unknown9SetTo0x00;

    uint8_t u8Unknown10SetTo0x04;
    uint8_t u8Unknown11SetTo0x01;

    uint16_t u16Unknown12SetTo0x00ff;
} ORLACO_tsGetRegionOfInterestResponsePayload;


typedef struct {
    uint32_t u32RegionOfInterest;
} ORLACO_tsSubscribeRegionOfInterestPayload;


typedef struct {
    uint16_t u16Length;
    uint8_t u8Type;
    uint8_t u8Reserved;
    union {
        uint32_t au32Data[2];
    } uData;
} ORLACO_tsServiceDiscoveryServiceOptions;


typedef struct {
    uint8_t u8Flags;
    uint32_t u24Reserved;
    uint32_t u32LengthOfEntriesArrayInBytes;
    ORLACO_tsServiceDiscoveryServiceEntry asServiceEntry[ORLACO_MAX_SD_SERVICES];
    uint32_t u32LengthOfOptionsArrayInBytes;
    ORLACO_tsServiceDiscoveryServiceOptions asServiceOptions[ORLACO_MAX_SD_OPTIONS];
} ORLACO_tsServiceDiscoveryPayload;


typedef struct {
    ORLACO_tuIP uSrcAddr;

    uint16_t u16ServiceID;
    uint16_t u16MethodID;
    uint32_t u32Length;
    uint16_t u16ClientID;
    uint16_t u16SessionID;
    uint8_t u8SomeIPVersion;
    uint8_t u8InterfaceVersion;
    uint8_t u8MessageType;
    uint8_t u8ReturnCode;

    union {
        ORLACO_tsSetCamExclusivePayload                sSetCamExclusivePayload;
        ORLACO_tsSetCamModePayload                     sSetCamModePayload;
        ORLACO_tsRegisterRequestsPayload               sRegisterRequestsPayload;
        ORLACO_tsGetRegistersResponsePayload           sGetRegistersResponsePayload;
        ORLACO_tsSetRegistersRequestPayload            sSetRegistersRequestPayload;
        ORLACO_tsGetRegionOfInterestRequestPayload     sGetRegionOfInterestRequestPayload;
        ORLACO_tsGetRegionOfInterestResponsePayload    sGetRegionOfInterestResponsePayload;
        ORLACO_tsSetRegionOfInterestPayload            sSetRegionOfInterestPayload;
        ORLACO_tsSubscribeRegionOfInterestPayload      sSubscribeRegionOfInterestPayload;
        ORLACO_tsServiceDiscoveryPayload               sServiceDiscoveryPayload;
    } uPayload;

} ORLACO_tsMsg ;


typedef struct {
    uint32_t u32Length;
    uint32_t u32Offset;
    uint8_t *pu8Data;
} ORLACO_tsBuffer;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

static uint16_t ORLACO_u16GetSessionID(ORLACO_tsInstance *psInstance);

static ORLACO_tsBuffer *ORLACO_psBufferCreate(uint32_t u32DataLength);
static void ORLACO_vBufferDestroy(ORLACO_tsBuffer *psBuffer);

static bool_t ORLACO_bWriteU8(ORLACO_tsBuffer *psBuffer, uint8_t u8Data);
static bool_t ORLACO_bWriteU16(ORLACO_tsBuffer *psBuffer, uint16_t u16Data);
static bool_t ORLACO_bWriteU24(ORLACO_tsBuffer *psBuffer, uint32_t u24Data);
static bool_t ORLACO_bWriteU32(ORLACO_tsBuffer *psBuffer, uint32_t u32Data);

static bool_t ORLACO_bReadU8(ORLACO_tsBuffer *psBuffer, uint8_t *pu8Data);
static bool_t ORLACO_bReadU16(ORLACO_tsBuffer *psBuffer, uint16_t *pu16Data);
static bool_t ORLACO_bReadU24(ORLACO_tsBuffer *psBuffer, uint32_t *pu24Data);
static bool_t ORLACO_bReadU32(ORLACO_tsBuffer *psBuffer, uint32_t *pu32Data);

static bool_t ORLACO_bWriteMessageHeaderIntoBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsMsg *psMsg);
static bool_t ORLACO_bReadMessageHeaderFromBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsMsg *psMsg);
static void ORLACO_vPrintBuffer(ORLACO_tsBuffer *psBuffer);
static bool_t ORLACO_bWriteServiceDiscoveryServiceEntryIntoBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsServiceDiscoveryServiceEntry *psServiceEntry);
static bool_t ORLACO_bReadServiceDiscoveryServiceEntryFromBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsServiceDiscoveryServiceEntry *psServiceEntry);
static bool_t ORLACO_bSendDatagram(UDPSOCKET sktTx, struct sockaddr_in *psDstAddr, ORLACO_tsBuffer *psBuffer);
static bool_t ORLACO_bReceiveDatagram(ORLACO_tsInstance *psInstance, ORLACO_tsMsg *psRxMsg, uint16_t u16MethodID);
static char *ORLACO_pcGetReturnCodeAsString(ORLACO_teReturnCode eReturnCode);
bool_t ORLACO_bIPAlreadyInArray(ORLACO_tsInstance *psInstance, ORLACO_tuIP IP);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

// A list of all the registers known so far in the Orlaco camera
// Make sure these stay in the same order that they are defined in, otherwise the indexes will be useless.
const ORLACO_tsRegisterDefinition ORLACO_asRegisterDefinitions[] = {
    {E_ORLACO_REGISTER_ADDRESS_LED_MODE,                            "LED Mode", "0=Off, 1=Auto, 2=On"},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_PROTOCOL,                     "Stream Protocol", "0=RTP, 1=AVB"},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_0,                 "IP Address 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_1,                 "IP Address 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_2,                 "IP Address 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_IP_ADDRESS_3,                 "IP Address 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_0,               "Network Mask 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_1,               "Network Mask 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_2,               "Network Mask 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_STATIC_NETWORK_MASK_3,               "Network Mask 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_0 ,                      "MAC Address 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_1 ,                      "MAC Address 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_2 ,                      "MAC Address 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_3 ,                      "MAC Address 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_4 ,                      "MAC Address 4", ""},
    {E_ORLACO_REGISTER_ADDRESS_MAC_ADDRESS_5 ,                      "MAC Address 5", ""},
    {E_ORLACO_REGISTER_ADDRESS_VLAN_ID_0,                           "VLAN ID 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_VLAN_ID_1,                           "VLAN ID 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_0,                         "Stream ID 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_1,                         "Stream ID 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_2,                         "Stream ID 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_3,                         "Stream ID 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_4,                         "Stream ID 4", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_5,                         "Stream ID 5", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_6,                         "Stream ID 6", ""},
    {E_ORLACO_REGISTER_ADDRESS_STREAM_ID_7,                         "Stream ID 7", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_0, "Destination IP Address 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_1, "Destination IP Address 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_2, "Destination IP Address 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_IP_ADDRESS_3, "Destination IP Address 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_0,"Destination MAC Address 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_1,"Destination MAC Address 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_2,"Destination MAC Address 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_3,"Destination MAC Address 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_4,"Destination MAC Address 4", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_MAC_ADDRESS_5,"Destination MAC Address 5", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_PORT_0,       "Destination Port 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_DESTINATION_PORT_1,       "Destination Port 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_SELECTED_ROI,                        "Selected ROI", "1 to 10"},
    {E_ORLACO_REGISTER_ADDRESS_NO_STREAM_AT_BOOT,                   "No Stream At Boot ?", "0=Stream, 1=No stream"},
    {E_ORLACO_REGISTER_ADDRESS_UDP_COMMUNICATION_PORT_0,            "UDP Communication Port 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_UDP_COMMUNICATION_PORT_1,            "UDP Communication Port 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_SOURCE_PORT_0,            "RTP Stream Source Port 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_RTP_STREAM_SOURCE_PORT_1,            "RTP Stream Source Port 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_HDR,                                 "HDR ?", "0=Disabled, 1=Enabled"},
    {E_ORLACO_REGISTER_ADDRESS_OVERLAY,                             "Overlay ?", "0=Disabled, 1=Enabled"},
    {E_ORLACO_REGISTER_ADDRESS_DHCP,                                "DHCP Enabled ?", "0=Disabled, 1=Enabled"},
    {E_ORLACO_REGISTER_ADDRESS_WAIT_FOR_MAC,                        "Wait For MAC ?", "0=Don't wait, 1=Wait"},
    {E_ORLACO_REGISTER_ADDRESS_WAIT_FOR_PTP_SYNC,                   "Wait For PTP Sync ?", "0=Don't wait, 1=Wait"},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_0,                     "DHCP Hostname 0", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_1,                     "DHCP Hostname 1", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_2,                     "DHCP Hostname 2", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_3,                     "DHCP Hostname 3", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_4,                     "DHCP Hostname 4", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_5,                     "DHCP Hostname 5", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_6,                     "DHCP Hostname 6", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_7,                     "DHCP Hostname 7", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_8,                     "DHCP Hostname 8", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_9,                     "DHCP Hostname 9", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_10,                    "DHCP Hostname 10", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_11,                    "DHCP Hostname 11", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_12,                    "DHCP Hostname 12", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_13,                    "DHCP Hostname 13", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_14,                    "DHCP Hostname 14", ""},
    {E_ORLACO_REGISTER_ADDRESS_DHCP_HOSTNAME_15,                    "DHCP Hostname 15", ""},
};

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: ORLACO_bInit
 *
 * DESCRIPTION:
 * Initialises the Orlaco functions
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bInit(ORLACO_tsInstance *psInstance, char *pcUnicastIP, char *pcBroadcastIP, uint16_t u16DstPort)
{
    int n;
    int iEnable;
    psInstance->u16DstPort = u16DstPort;

    psInstance->u16NumCameras = 0;
    psInstance->psCameras = NULL;

    // Initialise the socket
    psInstance->Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

#ifdef _WIN32
	if (psInstance->Socket == INVALID_SOCKET)
#else
	if (psInstance->Socket < 0)
#endif
    {
		printf("Error: Can't create UDP socket in %s\n", __FUNCTION__);
        return FALSE;
	}

    // Make sure socket supports broadcasts
    iEnable = 1;
    if(setsockopt(psInstance->Socket, SOL_SOCKET, SO_BROADCAST, (const char*)&iEnable, sizeof(iEnable)) != 0)
    {
		printf("Error: Can't set socket options in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Make sure socket doesn't receive its own broadcasts
    // iEnable = 0;
    // if(setsockopt(psInstance->Socket, IPPROTO_IP, IP_MULTICAST_LOOP, &iEnable, sizeof(iEnable)) != 0)
    // {
	// 	printf("Error: Can't set socket options\n");
    //     return FALSE;
    // }

#ifdef _WIN32
    DWORD timeout = ORLACO_SOCKET_READ_TIMEOUT_MS;
    if(setsockopt(psInstance->Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) != 0)
    {
		printf("Error: Can't set socket options in %s\n", __FUNCTION__);
        return FALSE;
    }
#else
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = ORLACO_SOCKET_READ_TIMEOUT_MS * 1000;
    if(setsockopt(psInstance->Socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0)
    {
		printf("Error: Can't set socket options in %s\n", __FUNCTION__);
        return FALSE;
    }
#endif

	/* Set the socket address for the server */
	memset((char *) &psInstance->fdServer, 0, sizeof(psInstance->fdServer));
	psInstance->fdServer.sin_family = AF_INET;
	psInstance->fdServer.sin_port = htons(u16DstPort);
	psInstance->fdServer.sin_addr.s_addr = INADDR_ANY;

	/* Set the socket address for unicast packets */
	memset((char *) &psInstance->fdUnicast, 0, sizeof(psInstance->fdUnicast));
	psInstance->fdUnicast.sin_family = AF_INET;
	psInstance->fdUnicast.sin_port = htons(u16DstPort);
	psInstance->fdUnicast.sin_addr.s_addr = inet_addr(pcUnicastIP);

	/* Set the socket address for broadcast packets */
	memset((char *) &psInstance->fdBroadcast, 0, sizeof(psInstance->fdBroadcast));
	psInstance->fdBroadcast.sin_family = AF_INET;
	psInstance->fdBroadcast.sin_port = htons(u16DstPort);
	psInstance->fdBroadcast.sin_addr.s_addr = inet_addr(pcBroadcastIP);

    if(bind(psInstance->Socket, (const struct sockaddr*)&psInstance->fdServer, sizeof(psInstance->fdServer)) < 0)
    {
        printf("Error: Bind failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Set how many registers we have
    psInstance->u16NumRegisters = sizeof(ORLACO_asRegisterDefinitions) / sizeof(ORLACO_tsRegisterDefinition);

    // Allocate the memory for the registers
    psInstance->psRegisters = (ORLACO_tsRegisterValue*)malloc(psInstance->u16NumRegisters * sizeof(ORLACO_tsRegisterValue));
    if(psInstance->psRegisters == NULL)
    {
        printf("Error: Failed to allocate memory for registers in %s\n", __FUNCTION__);
        return FALSE;
    }
 
    // Initialise the registers with the correct addresses etc
    memset(psInstance->psRegisters, 0, psInstance->u16NumRegisters * sizeof(ORLACO_tsRegisterValue));
    for(n = 0; n < psInstance->u16NumRegisters; n++)
    {
        psInstance->psRegisters[n].u16Address = ORLACO_asRegisterDefinitions[n].u16Address;
        psInstance->psRegisters[n].pcDescription = ORLACO_asRegisterDefinitions[n].pcDescription;
        psInstance->psRegisters[n].pcHelp = ORLACO_asRegisterDefinitions[n].pcHelp;
    }

    // Set how many regions of interest there are
    psInstance->u16NumRegionsOfInterest = ORLACO_NUM_REGIONS_OF_INTEREST;

    // Allocate the memory for the regions of interest
    psInstance->psRegionsOfInterest = (ORLACO_tsRegionOfInterest*)malloc(psInstance->u16NumRegionsOfInterest * sizeof(ORLACO_tsRegionOfInterest));
    if(psInstance->psRegionsOfInterest == NULL)
    {
        printf("Error: Failed to allocate memory for regions of interest in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Initialise the regions of interest
    memset(psInstance->psRegionsOfInterest, 0, psInstance->u16NumRegionsOfInterest * sizeof(ORLACO_tsRegionOfInterest));

    return TRUE;
}


/****************************************************************************
 *
 * NAME: ORLACO_vDeInit
 *
 * DESCRIPTION:
 * De-initialises the Orlaco functions
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void ORLACO_vDeInit(ORLACO_tsInstance *psInstance)
{

#ifdef _WIN32
    closesocket(psInstance->Socket);
#else
    close(psInstance->Socket);
#endif

    // Free memory allocated for the registers
    if(psInstance->psRegisters != NULL)
    {
        free(psInstance->psRegisters);
    }

    // Free memory allocated for the regions of interest
    if(psInstance->psRegionsOfInterest != NULL)
    {
        free(psInstance->psRegionsOfInterest);
    }

    // Free memory allocated for discovered cameras
    if(psInstance->psCameras != NULL)
    {
        free(psInstance->psCameras);
    }

}


/****************************************************************************
 *
 * NAME: ORLACO_vSetVerbosity
 *
 * DESCRIPTION:
 * Set the logging verbosity level
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void ORLACO_vSetVerbosity(ORLACO_tsInstance *psInstance, ORLACO_eVerbosityLevel eVerbosityLevel)
{
    psInstance->eVerbosity = eVerbosityLevel;
}


/****************************************************************************
 *
 * NAME: ORLACO_bSetBroadcastIP
 *
 * DESCRIPTION:
 * Sets the broadcast IP address
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetBroadcastIP(ORLACO_tsInstance *psInstance, char *pcIpAddress, uint16_t u16DstPort)
{
	/* Set the socket address for broadcast packets */
	memset((char *) &psInstance->fdBroadcast, 0, sizeof(psInstance->fdBroadcast));
	psInstance->fdBroadcast.sin_family = AF_INET;
	psInstance->fdBroadcast.sin_port = htons(u16DstPort);
	psInstance->fdBroadcast.sin_addr.s_addr = inet_addr(pcIpAddress);

    return TRUE;
}


/****************************************************************************
 *
 * NAME: ORLACO_bSetUnicastIP
 *
 * DESCRIPTION:
 * Sets the unicast IP address
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetUnicastIP(ORLACO_tsInstance *psInstance, char *pcIpAddress, uint16_t u16DstPort)
{
	/* Set the socket address for unicast packets */
	memset((char *) &psInstance->fdUnicast, 0, sizeof(psInstance->fdUnicast));
	psInstance->fdUnicast.sin_family = AF_INET;
	psInstance->fdUnicast.sin_port = htons(u16DstPort);
	psInstance->fdUnicast.sin_addr.s_addr = inet_addr(pcIpAddress);

    return TRUE;
}


/****************************************************************************
 *
 * NAME: ORLACO_bBufferTest
 *
 * DESCRIPTION:
 * Function for testing the buffer read/write functions
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bBufferTest(ORLACO_tsInstance *psInstance)
{

    uint8_t u8;
    uint16_t u16;
    uint32_t u24;
    uint32_t u32;

    bool_t bOk = TRUE;

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    bOk &= ORLACO_bWriteU8(psBuffer, 0x11);
    bOk &= ORLACO_bWriteU16(psBuffer, 0x2233);
    bOk &= ORLACO_bWriteU24(psBuffer, 0x445566);
    bOk &= ORLACO_bWriteU32(psBuffer, 0x778899aa);
    if(!bOk){
        ORLACO_vBufferDestroy(psBuffer);
        return bOk;
    }

    printf("Buffer contains %d bytes after writing:", psBuffer->u32Offset);
    for(int n = 0; n < psBuffer->u32Offset; n++)
    {
        printf(" %02x", psBuffer->pu8Data[n]);
    }
    printf("\n");

    psBuffer->u32Offset = 0;

    bOk &= ORLACO_bReadU8(psBuffer, &u8);
    bOk &= ORLACO_bReadU16(psBuffer, &u16);
    bOk &= ORLACO_bReadU24(psBuffer, &u24);
    bOk &= ORLACO_bReadU32(psBuffer, &u32);

    printf("u8=%02x u16=%04x u24=%06x, u32=%08x\n", u8, u16, u24, u32);

    // Free the buffer
    ORLACO_vBufferDestroy(psBuffer);

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bDiscover
 *
 * DESCRIPTION:
 * Sends a broadcast discovery message to identify cameras
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bDiscover(ORLACO_tsInstance *psInstance)
{

    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Clear any previous discovery results
    psInstance->u16NumCameras = 0;
    if(psInstance->psCameras != NULL)
    {
        free(psInstance->psCameras);
        psInstance->psCameras = NULL;
    }

    psInstance->u16ServiceID = 0xffff;

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_SERVICE_DISCOVERY;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_NOTIFICATION;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;


    sMsg.uPayload.sServiceDiscoveryPayload.u8Flags = 0;
    sMsg.uPayload.sServiceDiscoveryPayload.u24Reserved = 0;
    sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes = ORLACO_SD_OPTION_LENGTH * 1;
    sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfOptionsArrayInBytes = 0;

    // Service entry 0
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u8Type = 0; // Find
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u8Index1stOptions = 0;
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u8Index2ndOptions = 0;
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u8NumberOfOptions = 0;
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u16ServiceID = psInstance->u16ServiceID;
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u16InstanceID = 0xffff;
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u8MajorVersion = 0xff;
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u24TTL = 3600; // 1 hour
    sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u32MinorVersion = 0xffffffff; // Any

    sMsg.u32Length += 12 + sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes + sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfOptionsArrayInBytes;


    // Write the message header into the byte buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sServiceDiscoveryPayload.u8Flags);
    bOk &= ORLACO_bWriteU24(psBuffer, sMsg.uPayload.sServiceDiscoveryPayload.u24Reserved);
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes);

    bOk &= ORLACO_bWriteServiceDiscoveryServiceEntryIntoBuffer(psBuffer, &sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0]);

    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfOptionsArrayInBytes);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdBroadcast, psBuffer))
    {
        return FALSE;
    }

    while(ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_MESSAGE_TYPE_NOTIFICATION))
    {
        // If we got some options but the service id is 0xffff, probably our own broadcast so drop it
        if((sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes >= ORLACO_SD_OPTION_LENGTH) && (sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0].u16ServiceID == 0xffff))
        {
            if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("Skipping message since its probably our own broadcast in %s\n", __FUNCTION__);
            continue;
        }

        // If we've already seen a message from this camera, skip it
        if(ORLACO_bIPAlreadyInArray(psInstance, sMsg.uSrcAddr))
        {
            if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("Skipping message since we've already seen this camera before in %s\n", __FUNCTION__);
            continue;
        }

        psInstance->u16NumCameras++;
        psInstance->psCameras = realloc(psInstance->psCameras, sizeof(ORLACO_tsCamera) * psInstance->u16NumCameras);
        psInstance->psCameras[psInstance->u16NumCameras-1].uIP.u32IP = sMsg.uSrcAddr.u32IP;
        if(sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes >= ORLACO_SD_OPTION_LENGTH)
        {
            memcpy(&psInstance->psCameras[psInstance->u16NumCameras-1].sDiscoveryServiceEntry, &sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[0], sizeof(ORLACO_tsServiceDiscoveryServiceEntry));
        }
        else
        {
            memset(&psInstance->psCameras[psInstance->u16NumCameras-1].sDiscoveryServiceEntry, 0, sizeof(ORLACO_tsServiceDiscoveryServiceEntry));
        }

        if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_INFO) printf("Got response from IP %d.%d.%d.%d Len=%d\n", sMsg.uSrcAddr.au8IP[3], sMsg.uSrcAddr.au8IP[2], sMsg.uSrcAddr.au8IP[1], sMsg.uSrcAddr.au8IP[0], sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes);

        for(int n = 0; n < sMsg.uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes; n += ORLACO_SD_OPTION_LENGTH)
        {
            if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_INFO) printf("Option %d Type=%02x ServiceID=%04x InstanceID=%04x V=%d.%d 1stOptionsIdx=%d 2ndOptionsIdx=%d OptionsNum=%02x\n\n",
                                          n,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u8Type,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u16ServiceID,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u16InstanceID,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u8MajorVersion,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u32MinorVersion,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u8Index1stOptions,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u8Index2ndOptions,
                                          sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u8NumberOfOptions
                                          );
            psInstance->u16ServiceID = sMsg.uPayload.sServiceDiscoveryPayload.asServiceEntry[n].u16ServiceID;
        }
    }

    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bSetCamExclusive
 *
 * DESCRIPTION:
 * Sends a "Set Camera Exclusive" message containing the specified exclusive time
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetCamExclusive(ORLACO_tsInstance *psInstance, uint32_t u32ExclusiveTime)
{
    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_SET_CAM_EXCLUSIVE;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Add the message payload and adjust the length field to include it
    sMsg.u32Length += sizeof(sMsg.uPayload.sSetCamExclusivePayload);
    sMsg.uPayload.sSetCamExclusivePayload.u32ExclusiveTime = u32ExclusiveTime;

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the payload into the buffer
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sSetCamExclusivePayload.u32ExclusiveTime);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response
    bOk &= ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_SET_CAM_EXCLUSIVE);

    return bOk;
 
}


/****************************************************************************
 *
 * NAME: ORLACO_bEraseCamExclusive
 *
 * DESCRIPTION:
 * Sends a "Erase Camera Exclusive" message
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bEraseCamExclusive(ORLACO_tsInstance *psInstance)
{
    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_ERASE_CAM_EXCLUSIVE;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response
    bOk &= ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_ERASE_CAM_EXCLUSIVE);

    return bOk;
 
}


/****************************************************************************
 *
 * NAME: ORLACO_bSetCamMode
 *
 * DESCRIPTION:
 * Sends a "Set Camera Mode" message containing the specified mode
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetCamMode(ORLACO_tsInstance *psInstance, ORLACO_teCameraMode eMode)
{
    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Construct the message header
    // sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16ServiceID = 0xffff;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_SET_CAM_MODE;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Add the message payload and adjust the length field to include it
    sMsg.u32Length += sizeof(sMsg.uPayload.sSetCamModePayload);
    // sMsg.u32Length += 1;
    sMsg.uPayload.sSetCamModePayload.u32Mode = eMode;

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the payload into the buffer
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sSetCamModePayload.u32Mode);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response
    bOk &= ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_SET_CAM_MODE);

    return bOk;
 
}


/****************************************************************************
 *
 * NAME: ORLACO_bGetRegisters
 *
 * DESCRIPTION:
 * Reads registers from the camera
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bGetRegisters(ORLACO_tsInstance *psInstance)
{
    bool_t bOk = TRUE;
    int n;
    ORLACO_tsMsg sMsg;
    uint16_t u16Qtty = 0;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // See how many registers we will be reading
    for(n = 0; n < psInstance->u16NumRegisters; n++)
    {
        if(psInstance->psRegisters[n].bRead) u16Qtty++;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_GET_CAM_REGISTERS;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Adjust the length field to include the quantity of registers and list of register adresses
    sMsg.u32Length += sizeof(uint16_t) + (u16Qtty * sizeof(uint16_t));

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the number of registers into the buffer
    bOk &= ORLACO_bWriteU16(psBuffer, u16Qtty);

    // Write the register addresses into the buffer
    for(n = 0; n < psInstance->u16NumRegisters; n++)
    {
        // If the register is marked for reading, add its address to the payload
        if(psInstance->psRegisters[n].bRead)
        {
            bOk &= ORLACO_bWriteU16(psBuffer, psInstance->psRegisters[n].u16Address);
        }
    }

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response, exit if not
    if(!ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_GET_CAM_REGISTERS))
    {
        return FALSE;
    }

    // Check we got back the same number of registers that we requested
    if(sMsg.uPayload.sGetRegistersResponsePayload.u16Qtty != u16Qtty)
    {
        return FALSE;
    }

    // Copy register values into original request ensuring the correct value goes with the correct address in case the camera doesn't send things back in the same order
    for(int n = 0; n < psInstance->u16NumRegisters; n++)
    {
        for(int x = 0; x < sMsg.uPayload.sGetRegistersResponsePayload.u16Qtty; x++)
        {
            if(sMsg.uPayload.sGetRegistersResponsePayload.asRegisterValues[x].u16Address == psInstance->psRegisters[n].u16Address)
            {
                psInstance->psRegisters[n].u8Value = sMsg.uPayload.sGetRegistersResponsePayload.asRegisterValues[x].u8Value;
            }
        }
    }

    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bSetRegisters
 *
 * DESCRIPTION:
 * Writes registers on the camera
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetRegisters(ORLACO_tsInstance *psInstance)
{
    bool_t bOk = TRUE;
    int n;
    ORLACO_tsMsg sMsg;
    uint16_t u16Qtty = 0;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // See how many registers we will be reading
    for(n = 0; n < psInstance->u16NumRegisters; n++)
    {
        if(psInstance->psRegisters[n].bWrite) u16Qtty++;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_SET_CAM_REGISTERS;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Adjust the length field to include the quantity of registers and list of register adresses
    sMsg.u32Length += sizeof(uint16_t) + (u16Qtty * 4);

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the number of registers into the buffer
    bOk &= ORLACO_bWriteU16(psBuffer, u16Qtty);

    // Write the register addresses and their values into the buffer
    for(n = 0; n < psInstance->u16NumRegisters; n++)
    {
        // If the register is marked for writing, add its address and value to the payload
        if(psInstance->psRegisters[n].bWrite)
        {
            bOk &= ORLACO_bWriteU16(psBuffer, psInstance->psRegisters[n].u16Address);
            bOk &= ORLACO_bWriteU8(psBuffer, psInstance->psRegisters[n].u8Padding);
            bOk &= ORLACO_bWriteU8(psBuffer, psInstance->psRegisters[n].u8Value);
        }
    }

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response
    bOk &= ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_SET_CAM_REGISTERS);

    return bOk;

}


/****************************************************************************
 *
 * NAME: ORLACO_bGetAllRegisters
 *
 * DESCRIPTION:
 * Reads all the registers on the camera
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bGetAllRegisters(ORLACO_tsInstance *psInstance)
{

    bool_t bOk = TRUE;
    int n;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    for(n = 0; n < psInstance->u16NumRegisters; n++)
    {
        psInstance->psRegisters[n].bRead = TRUE;
    }


    bOk &= ORLACO_bGetRegisters(psInstance);

    for(int n = 0; n < psInstance->u16NumRegisters; n++)
    {
        printf("%02d) Addr=%04x Value=%02x - %3d - %c\t%s\n", n, psInstance->psRegisters[n].u16Address, psInstance->psRegisters[n].u8Value, psInstance->psRegisters[n].u8Value, psInstance->psRegisters[n].u8Value, psInstance->psRegisters[n].pcDescription);
    }

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bGetRegionOfInterest
 *
 * DESCRIPTION:
 * Gets the specified region of interest from the camera
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bGetRegionOfInterest(ORLACO_tsInstance *psInstance, uint32_t u32RegionOfInterest, ORLACO_tsRegionOfInterest *psRegionOfInterest)
{

    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_GET_REGION_OF_INTEREST;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Add the message payload and adjust the length field to include it
    sMsg.u32Length += sizeof(sMsg.uPayload.sGetRegionOfInterestRequestPayload);
    sMsg.uPayload.sGetRegionOfInterestRequestPayload.u32RegionOfInterest = u32RegionOfInterest;

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the payload into the buffer
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sGetRegionOfInterestRequestPayload.u32RegionOfInterest);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response, exit if not
    if(!ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_GET_REGION_OF_INTEREST))
    {
        return FALSE;
    }

    psRegionOfInterest->u16P1X = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P1X;
    psRegionOfInterest->u16P1Y = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P1Y;
    psRegionOfInterest->u16P2X = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P2X;
    psRegionOfInterest->u16P2Y = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P2Y;
    psRegionOfInterest->u16OutputWidth = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16OutputWidth;
    psRegionOfInterest->u16OutputHeight = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16OutputHeight;
    psRegionOfInterest->eCompressionMode = (ORLACO_teVideoCompressionMode)sMsg.uPayload.sGetRegionOfInterestResponsePayload.u8VideoCompressionMode;
    psRegionOfInterest->u32MaxBitrate = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u32MaxBitrate;
    psRegionOfInterest->u8FrameRate = sMsg.uPayload.sGetRegionOfInterestResponsePayload.u8FrameRate;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("P1X=%d P1Y=%d P2X=%d P2Y=%d OutputWidth=%d OutputHeight=%d MaxBitRate=%d FrameRate=%d CompressionMode=%d LastWord=%04x\n",
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P1X,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P1Y,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P2X,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16P2Y,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16OutputWidth,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16OutputHeight,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u32MaxBitrate,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u8FrameRate,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u8VideoCompressionMode,
                                  sMsg.uPayload.sGetRegionOfInterestResponsePayload.u16Unknown12SetTo0x00ff
                                  );

    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bGetRegionsOfInterest
 *
 * DESCRIPTION:
 * Get the regions of interest marked for reading
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bGetRegionsOfInterest(ORLACO_tsInstance *psInstance)
{

    bool_t bOk = TRUE;
    int n;

    for(n = 1; n < psInstance->u16NumRegionsOfInterest; n++)
    {
        if(psInstance->psRegionsOfInterest[n].bRead)
        {
            bOk &= ORLACO_bGetRegionOfInterest(psInstance, n, &psInstance->psRegionsOfInterest[n]);
        }
    }

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bSetRegionsOfInterest
 *
 * DESCRIPTION:
 * Set the regions of interest marked for writing
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetRegionsOfInterest(ORLACO_tsInstance *psInstance)
{

    bool_t bOk = TRUE;
    int n;

    for(n = 1; n < psInstance->u16NumRegionsOfInterest; n++)
    {
        if(psInstance->psRegionsOfInterest[n].bWrite)
        {
            bOk &= ORLACO_bSetRegionOfInterest(psInstance, n, &psInstance->psRegionsOfInterest[n]);
        }
    }

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bSubscribeRoiVideo
 *
 * DESCRIPTION:
 * Subscribe the selected region of interest
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSubscribeRoiVideo(ORLACO_tsInstance *psInstance, uint32_t u32RegionOfInterest)
{
    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_SUBSCRIBE_ROI_VIDEO;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    // Add the message payload and adjust the length field to include it
    sMsg.u32Length += sizeof(sMsg.uPayload.sSubscribeRegionOfInterestPayload);
    sMsg.uPayload.sSubscribeRegionOfInterestPayload.u32RegionOfInterest = u32RegionOfInterest;

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the payload into the buffer
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sSubscribeRegionOfInterestPayload.u32RegionOfInterest);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response
    bOk &= !ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_SUBSCRIBE_ROI_VIDEO);

    return bOk;

}


/****************************************************************************
 *
 * NAME: ORLACO_bGetRegionOfInterest
 *
 * DESCRIPTION:
 * Writes the specified region of interest to the camera
 *
 * RETURNS:
 * bool_t TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
bool_t ORLACO_bSetRegionOfInterest(ORLACO_tsInstance *psInstance, uint32_t u32RegionOfInterestIndex, ORLACO_tsRegionOfInterest *psRegionOfInterest)
{
    bool_t bOk = TRUE;
    ORLACO_tsMsg sMsg;

    if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("%s()\n", __FUNCTION__);

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    // Construct the message header
    sMsg.u16ServiceID = psInstance->u16ServiceID;
    sMsg.u16MethodID = E_ORLACO_METHOD_ID_SET_REGION_OF_INTEREST;

    sMsg.u32Length = 8;

    sMsg.u16ClientID = psInstance->u16ClientID;
    sMsg.u16SessionID = ORLACO_u16GetSessionID(psInstance);

    sMsg.u8SomeIPVersion = 1;
    sMsg.u8InterfaceVersion = 1;
    sMsg.u8MessageType = E_ORLACO_MESSAGE_TYPE_REQUEST;
    sMsg.u8ReturnCode = E_ORLACO_RETURN_CODE_OK;

    sMsg.uPayload.sSetRegionOfInterestPayload.u32RegionOfInterestIndex = u32RegionOfInterestIndex;
    sMsg.uPayload.sSetRegionOfInterestPayload.u16P1X = psRegionOfInterest->u16P1X;
    sMsg.uPayload.sSetRegionOfInterestPayload.u16P1Y = psRegionOfInterest->u16P1Y;
    sMsg.uPayload.sSetRegionOfInterestPayload.u16P2X = psRegionOfInterest->u16P2X;
    sMsg.uPayload.sSetRegionOfInterestPayload.u16P2Y = psRegionOfInterest->u16P2Y;
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown1SetTo0x01 = 0x01; // no idea what this does yet, if set to 0, return code is 0x32 - invalid value in video format
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown2SetTo0x00 = 0x00; // no idea what this does yet, if set to 1, return code is 0x32 - invalid value in video format
    sMsg.uPayload.sSetRegionOfInterestPayload.u16Unknown3SetTo0x0000 = 0x0000; // no idea what this does yet, if set to 1, return code is 0x32 - invalid value in video format
    sMsg.uPayload.sSetRegionOfInterestPayload.u16OutputWidth = psRegionOfInterest->u16OutputWidth;
    sMsg.uPayload.sSetRegionOfInterestPayload.u16OutputHeight = psRegionOfInterest->u16OutputHeight;
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown4SetTo0x00 = 0x00; // no idea what this does yet
    sMsg.uPayload.sSetRegionOfInterestPayload.u8FrameRate = psRegionOfInterest->u8FrameRate;
    sMsg.uPayload.sSetRegionOfInterestPayload.u16Unknown4bSetTo0x0000 = 0x0000; // no idea what this does yet
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown5SetTo0x00 = 0x00; // no idea what this does yet
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown6SetTo0x02 = 0x02; // no idea what this does yet, if set to 1, return code is 0x32 - invalid value in video format
    sMsg.uPayload.sSetRegionOfInterestPayload.u32MaxBitrate = psRegionOfInterest->u32MaxBitrate;
    sMsg.uPayload.sSetRegionOfInterestPayload.u8VideoCompressionMode = (uint8_t)psRegionOfInterest->eCompressionMode;
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown7SetTo0x00 = 0x00; // no idea what this does yet, setting to 1 returns ok
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown8SetTo0x00 = 0x00; // no idea what this does yet, setting to 1 returns ok
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown9SetTo0x00 = 0x00; // no idea what this does yet, setting to 1 returns ok
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown10SetTo0x04 = 0x04; // no idea what this does yet
    sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown11SetTo0x01 = 0x01; // no idea what this does yet
    sMsg.uPayload.sSetRegionOfInterestPayload.u16Unknown12SetTo0x00ff = 0x00ff; // no idea what this does yet


    // Adjust the length field to include the payload
    sMsg.u32Length += 38;

    // printf("Len=%d %d\n", sMsg.u32Length, sizeof(ORLACO_tsSetRegionOfInterestPayload));

    // Write the message header into the byte array buffer
    bOk &= ORLACO_bWriteMessageHeaderIntoBuffer(psBuffer, &sMsg);

    // Write the payload into the buffer
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u32RegionOfInterestIndex);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16P1X);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16P1Y);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16P2X);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16P2Y);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown1SetTo0x01);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown2SetTo0x00);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16Unknown3SetTo0x0000);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16OutputWidth);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16OutputHeight);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown4SetTo0x00);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8FrameRate);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16Unknown4bSetTo0x0000);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown5SetTo0x00);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown6SetTo0x02);
    bOk &= ORLACO_bWriteU32(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u32MaxBitrate);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8VideoCompressionMode);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown7SetTo0x00);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown8SetTo0x00);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown9SetTo0x00);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown10SetTo0x04);
    bOk &= ORLACO_bWriteU8(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u8Unknown11SetTo0x01);
    bOk &= ORLACO_bWriteU16(psBuffer, sMsg.uPayload.sSetRegionOfInterestPayload.u16Unknown12SetTo0x00ff);

    // If we couldn't write the message to the buffer for some reason, free the buffer and then exit
    if(!bOk)
    {
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // Send the message
    if(!ORLACO_bSendDatagram(psInstance->Socket, &psInstance->fdUnicast, psBuffer))
    {
        return FALSE;
    }

    // See if we get a response
    bOk &= ORLACO_bReceiveDatagram(psInstance, &sMsg, E_ORLACO_METHOD_ID_SET_REGION_OF_INTEREST);

    return bOk;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: ORLACO_u16GetSessionID
 *
 * DESCRIPTION:
 * Gets a session ID
 *
 * RETURNS:
 * uint16_t
 *
 ****************************************************************************/
static uint16_t ORLACO_u16GetSessionID(ORLACO_tsInstance *psInstance)
{
    // increment the session ID. Add extra 1 if that rolls us over to zero since 0 = no session ID
    psInstance->u16SessionID++;
    if(psInstance->u16SessionID == 0)
    {
        psInstance->u16SessionID = 1;
    }
    return psInstance->u16SessionID;
}


/****************************************************************************
 *
 * NAME: ORLACO_psBufferCreate
 *
 * DESCRIPTION:
 * Creates a buffer
 *
 * RETURNS:
 * ORLACO_tsBuffer* A pointer to a buffer, or NULL if something failed
 *
 ****************************************************************************/
static ORLACO_tsBuffer *ORLACO_psBufferCreate(uint32_t u32DataLength)
{
    // Try and allocate some memory for a buffer. Exit if this fails
    ORLACO_tsBuffer *psBuffer = (ORLACO_tsBuffer*)malloc(sizeof(ORLACO_tsBuffer));
    if(psBuffer == NULL)
    {
        return NULL;
    }

    // Initialise to 0
    memset(psBuffer, 0, sizeof(ORLACO_tsBuffer));

    // Try and allocate the memory for the data. If this fails, free the buffer and return
    psBuffer->pu8Data = (uint8_t*)malloc(u32DataLength);
    if(psBuffer->pu8Data == NULL)
    {
        free(psBuffer);
        return NULL;
    }

    psBuffer->u32Length = u32DataLength;

    return psBuffer;
}


/****************************************************************************
 *
 * NAME: ORLACO_vBufferDestroy
 *
 * DESCRIPTION:
 * Destroys a buffer
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void ORLACO_vBufferDestroy(ORLACO_tsBuffer *psBuffer)
{
    // Don't try and free a buffer that hasn't been allocated!
    if(psBuffer == NULL)
    {
        return;
    }

    // Only free the data buffer if it was successfully allocated in the first place
    if(psBuffer->pu8Data != NULL)
    {
        free(psBuffer->pu8Data);
    }

    // Now we can free the buffer
    free(psBuffer);
}


/****************************************************************************
 *
 * NAME: ORLACO_bWriteU8
 *
 * DESCRIPTION:
 * Writes an 8 bit value into a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was written, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bWriteU8(ORLACO_tsBuffer *psBuffer, uint8_t u8Data)
{

    uint8_t *pu8WritePtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to write past the end of the buffer
    if((psBuffer->u32Offset + 1) >= psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Write the data into the buffer and adjust the offset value
    *pu8WritePtr = u8Data;
    psBuffer->u32Offset += 1;

    // Return success
    return TRUE;
}


/****************************************************************************
 *
 * NAME: ORLACO_bWriteU16
 *
 * DESCRIPTION:
 * Writes a 16 bit value into a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was written, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bWriteU16(ORLACO_tsBuffer *psBuffer, uint16_t u16Data)
{
    uint8_t *pu8WritePtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to write past the end of the buffer
    if((psBuffer->u32Offset + 1) >= psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Write the data into the buffer and adjust the offset value
    *pu8WritePtr = (u16Data >> 8) & 0xff;
    pu8WritePtr++;
    *pu8WritePtr = (u16Data >> 0) & 0xff;
    psBuffer->u32Offset += 2;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bWriteU24
 *
 * DESCRIPTION:
 * Writes a 24 bit value into a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was written, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bWriteU24(ORLACO_tsBuffer *psBuffer, uint32_t u24Data)
{
    uint8_t *pu8WritePtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to write past the end of the buffer
    if((psBuffer->u32Offset + 1) >= psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Write the data into the buffer and adjust the offset value
    *pu8WritePtr = (u24Data >> 16) & 0xff;
    pu8WritePtr++;
    *pu8WritePtr = (u24Data >> 8) & 0xff;
    pu8WritePtr++;
    *pu8WritePtr = (u24Data >> 0) & 0xff;
    psBuffer->u32Offset += 3;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bWriteU32
 *
 * DESCRIPTION:
 * Writes a 32 bit value into a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was written, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bWriteU32(ORLACO_tsBuffer *psBuffer, uint32_t u32Data)
{
    uint8_t *pu8WritePtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to write past the end of the buffer
    if((psBuffer->u32Offset + 1) >= psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Write the data into the buffer and adjust the offset value
    *pu8WritePtr = (u32Data >> 24) & 0xff;
    pu8WritePtr++;
    *pu8WritePtr = (u32Data >> 16) & 0xff;
    pu8WritePtr++;
    *pu8WritePtr = (u32Data >> 8) & 0xff;
    pu8WritePtr++;
    *pu8WritePtr = (u32Data >> 0) & 0xff;
    psBuffer->u32Offset += 4;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bReadU8
 *
 * DESCRIPTION:
 * Reads an 8 bit value from a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was read, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReadU8(ORLACO_tsBuffer *psBuffer, uint8_t *pu8Data)
{
    uint8_t *pu8ReadPtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to read past the end of the buffer
    if((psBuffer->u32Offset + 1) > psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Read the data from the buffer
    *pu8Data = *pu8ReadPtr;
    psBuffer->u32Offset += 1;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bReadU16
 *
 * DESCRIPTION:
 * Reads a 16 bit value from a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was read, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReadU16(ORLACO_tsBuffer *psBuffer, uint16_t *pu16Data)
{
    uint16_t u16Data = 0;
    uint8_t *pu8ReadPtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to read past the end of the buffer
    if((psBuffer->u32Offset + 2) > psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Read the data from the buffer
    u16Data = (*(pu8ReadPtr++)) << 8;
    u16Data |= (*(pu8ReadPtr++)) << 0;

    *pu16Data = u16Data;
    psBuffer->u32Offset += 2;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bReadU24
 *
 * DESCRIPTION:
 * Reads a 24 bit value from a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was read, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReadU24(ORLACO_tsBuffer *psBuffer, uint32_t *pu24Data)
{
    uint32_t u24Data = 0;
    uint8_t *pu8ReadPtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to read past the end of the buffer
    if((psBuffer->u32Offset + 3) > psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Read the data from the buffer
    u24Data  = (*(pu8ReadPtr++)) << 16;
    u24Data |= (*(pu8ReadPtr++)) << 8;
    u24Data |= (*(pu8ReadPtr++)) << 0;

    *pu24Data = u24Data;
    psBuffer->u32Offset += 3;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bReadU32
 *
 * DESCRIPTION:
 * Reads a 32 bit value from a buffer
 *
 * RETURNS:
 * bool_t - TRUE if the data was read, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReadU32(ORLACO_tsBuffer *psBuffer, uint32_t *pu32Data)
{
    uint32_t u32Data = 0;
    uint8_t *pu8ReadPtr = psBuffer->pu8Data + psBuffer->u32Offset;

    // Check we're not about to read past the end of the buffer
    if((psBuffer->u32Offset + 4) > psBuffer->u32Length)
    {
        // Return failure
        return FALSE;
    }

    // Read the data from the buffer
    u32Data  = (*(pu8ReadPtr++)) << 24;
    u32Data |= (*(pu8ReadPtr++)) << 16;
    u32Data |= (*(pu8ReadPtr++)) << 8;
    u32Data |= (*(pu8ReadPtr++)) << 0;

    *pu32Data = u32Data;
    psBuffer->u32Offset += 4;

    // Return success
    return TRUE;

}


/****************************************************************************
 *
 * NAME: ORLACO_bWriteMessageHeaderIntoBuffer
 *
 * DESCRIPTION:
 * Writes a message header into a buffer
 *
 * RETURNS:
 * bool_t - TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bWriteMessageHeaderIntoBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsMsg *psMsg)
{

    bool_t bOk = TRUE;

    // First part of the message so ensure the buffer offset is set to zero
    psBuffer->u32Offset = 0;


    // Service ID
    bOk &= ORLACO_bWriteU16(psBuffer, psMsg->u16ServiceID);

    // Method ID
    bOk &= ORLACO_bWriteU16(psBuffer, psMsg->u16MethodID);

    // Length
    bOk &= ORLACO_bWriteU32(psBuffer, psMsg->u32Length);

    // Client ID
    bOk &= ORLACO_bWriteU16(psBuffer, psMsg->u16ClientID);

    // Session ID
    bOk &= ORLACO_bWriteU16(psBuffer, psMsg->u16SessionID);

    // SOME/IP Version
    bOk &= ORLACO_bWriteU8(psBuffer, psMsg->u8SomeIPVersion);

    // Interface Version
    bOk &= ORLACO_bWriteU8(psBuffer, psMsg->u8InterfaceVersion);

    // Message type
    bOk &= ORLACO_bWriteU8(psBuffer, psMsg->u8MessageType);

    // Return code
    bOk &= ORLACO_bWriteU8(psBuffer, psMsg->u8ReturnCode);
  
    return bOk;

}


/****************************************************************************
 *
 * NAME: ORLACO_vPrintBuffer
 *
 * DESCRIPTION:
 * Prints the contents of a buffer to the console
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void ORLACO_vPrintBuffer(ORLACO_tsBuffer *psBuffer)
{
    for(int n = 0; n < psBuffer->u32Offset; n++)
    {
        printf(" %02x", psBuffer->pu8Data[n]);
    }
}


/****************************************************************************
 *
 * NAME: ORLACO_bReadMessageHeaderFromBuffer
 *
 * DESCRIPTION:
 * Reads a message header from a buffer
 *
 * RETURNS:
 * bool_t - TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReadMessageHeaderFromBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsMsg *psMsg)
{

    bool_t bOk = TRUE;

    // Service ID
    bOk &= ORLACO_bReadU16(psBuffer, &psMsg->u16ServiceID);

    // Method ID
    bOk &= ORLACO_bReadU16(psBuffer, &psMsg->u16MethodID);

    // Length
    bOk &= ORLACO_bReadU32(psBuffer, &psMsg->u32Length);

    // There should be at least 8 bytes in the rest of the header, exit if not
    if(psMsg->u32Length < 8)
    {
        printf("Not enough length (%d)!\n", psMsg->u32Length);
        return FALSE;
    }

    // Client ID
    bOk &= ORLACO_bReadU16(psBuffer, &psMsg->u16ClientID);

    // Session ID
    bOk &= ORLACO_bReadU16(psBuffer, &psMsg->u16SessionID);

    // SOME/IP Version
    bOk &= ORLACO_bReadU8(psBuffer, &psMsg->u8SomeIPVersion);

    // Interface Version
    bOk &= ORLACO_bReadU8(psBuffer, &psMsg->u8InterfaceVersion);

    // Message type
    bOk &= ORLACO_bReadU8(psBuffer, &psMsg->u8MessageType);

    // Return code
    bOk &= ORLACO_bReadU8(psBuffer, &psMsg->u8ReturnCode);
  
    return bOk;

}


/****************************************************************************
 *
 * NAME: ORLACO_bWriteServiceDiscoveryServiceEntryIntoBuffer
 *
 * DESCRIPTION:
 * Writes a service discovery service entry into a buffer
 *
 * RETURNS:
 * bool_t - TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bWriteServiceDiscoveryServiceEntryIntoBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsServiceDiscoveryServiceEntry *psServiceEntry)
{

    bool_t bOk = TRUE;

    bOk &= ORLACO_bWriteU8(psBuffer, psServiceEntry->u8Type);
    bOk &= ORLACO_bWriteU8(psBuffer, psServiceEntry->u8Index1stOptions);
    bOk &= ORLACO_bWriteU8(psBuffer, psServiceEntry->u8Index2ndOptions);
    bOk &= ORLACO_bWriteU8(psBuffer, psServiceEntry->u8NumberOfOptions);
    bOk &= ORLACO_bWriteU16(psBuffer, psServiceEntry->u16ServiceID);
    bOk &= ORLACO_bWriteU16(psBuffer, psServiceEntry->u16InstanceID);
    bOk &= ORLACO_bWriteU8(psBuffer, psServiceEntry->u8MajorVersion);
    bOk &= ORLACO_bWriteU24(psBuffer, psServiceEntry->u24TTL);
    bOk &= ORLACO_bWriteU32(psBuffer, psServiceEntry->u32MinorVersion);

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bReadServiceDiscoveryServiceEntryFromBuffer
 *
 * DESCRIPTION:
 * Reads a service discovery service entry from a buffer
 *
 * RETURNS:
 * bool_t - TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReadServiceDiscoveryServiceEntryFromBuffer(ORLACO_tsBuffer *psBuffer, ORLACO_tsServiceDiscoveryServiceEntry *psServiceEntry)
{

    bool_t bOk = TRUE;

    bOk &= ORLACO_bReadU8(psBuffer, &psServiceEntry->u8Type);
    bOk &= ORLACO_bReadU8(psBuffer, &psServiceEntry->u8Index1stOptions);
    bOk &= ORLACO_bReadU8(psBuffer, &psServiceEntry->u8Index2ndOptions);
    bOk &= ORLACO_bReadU8(psBuffer, &psServiceEntry->u8NumberOfOptions);
    bOk &= ORLACO_bReadU16(psBuffer, &psServiceEntry->u16ServiceID);
    bOk &= ORLACO_bReadU16(psBuffer, &psServiceEntry->u16InstanceID);
    bOk &= ORLACO_bReadU8(psBuffer, &psServiceEntry->u8MajorVersion);
    bOk &= ORLACO_bReadU24(psBuffer, &psServiceEntry->u24TTL);
    bOk &= ORLACO_bReadU32(psBuffer, &psServiceEntry->u32MinorVersion);

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bSendDatagram
 *
 * DESCRIPTION:
 * Sends the contents of the specified buffer as a UDP datagram to the specified IP address
 *
 * RETURNS:
 * bool_t - TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bSendDatagram(UDPSOCKET sktTx, struct sockaddr_in *psDstAddr, ORLACO_tsBuffer *psBuffer)
{
    bool_t bOk = TRUE;
    int iLen;
    // printf("Tx: ");
    // ORLACO_vPrintBuffer(psBuffer);
    // printf("\n");

    iLen = sendto(sktTx, (const char *)psBuffer->pu8Data, psBuffer->u32Offset, 0, (const struct sockaddr*)psDstAddr, sizeof(struct sockaddr_in));
    if(iLen != psBuffer->u32Offset)
    {
        printf("Sendto returned %d\n", iLen);
        bOk &= FALSE;
    }

    // Buffer no longer required so free it here
    ORLACO_vBufferDestroy(psBuffer);

    return bOk;
}


/****************************************************************************
 *
 * NAME: ORLACO_bReceiveDatagram
 *
 * DESCRIPTION:
 * Receives a message
 *
 * RETURNS:
 * bool_t - TRUE if successful, FALSE otherwise
 *
 ****************************************************************************/
static bool_t ORLACO_bReceiveDatagram(ORLACO_tsInstance *psInstance, ORLACO_tsMsg *psRxMsg, uint16_t u16MethodID)
{
    bool_t bOk = TRUE;
    int iTimeout = ORLACO_MAX_RESPONSE_TIME_MS;
    int iLen = 0;

    uint16_t u16SenderPort;
    int64_t i64BytesReceived = 0;

    const struct sockaddr_in sRxAddr = {};
    int iRxAddrLen = sizeof(sRxAddr);

    memset(psRxMsg, 0, sizeof(ORLACO_tsMsg));

    // Allocate a buffer
    ORLACO_tsBuffer *psBuffer = ORLACO_psBufferCreate(ORLACO_BUFFER_LENGTH);
    if(psBuffer == NULL)
    {
        printf("Error: Buffer allocation failed in %s\n", __FUNCTION__);
        return FALSE;
    }

    do
    {

        while((iLen = recvfrom(psInstance->Socket, (char*)psBuffer->pu8Data, psBuffer->u32Length, 0, (struct sockaddr*)&sRxAddr, &iRxAddrLen)) <= 0)
        {
            // if(iLen == SOCKET_ERROR)
            // {
            //     printf("Last error = %d, ptr = %p\n",WSAGetLastError(), sRxBuffer.pu8Data);
            //     // return FALSE;
            // }
            iTimeout -= ORLACO_SOCKET_READ_TIMEOUT_MS;
            if(iTimeout <= 0)
            {
                if(psInstance->eVerbosity >= E_ORLACO_VERBOSITY_DEBUG) printf("Rx Timeout\n");
                ORLACO_vBufferDestroy(psBuffer);
                return FALSE;
            }
#ifdef _WIN32
            Sleep(1);
#else
            usleep(1000);
#endif
        }

        // printf("Addr: %s\n", inet_ntoa(sRxAddr.sin_addr));

        psBuffer->u32Length = (uint32_t)iLen;

        // psBuffer->u32Offset = (uint32_t)iLen;
        // printf("Rx: ");
        // ORLACO_vPrintBuffer(psBuffer);
        // printf("\n");

        // Get the IP address of the sender
#ifdef _WIN32
        psRxMsg->uSrcAddr.au8IP[3] = sRxAddr.sin_addr.S_un.S_un_b.s_b1;
        psRxMsg->uSrcAddr.au8IP[2] = sRxAddr.sin_addr.S_un.S_un_b.s_b2;
        psRxMsg->uSrcAddr.au8IP[1] = sRxAddr.sin_addr.S_un.S_un_b.s_b3;
        psRxMsg->uSrcAddr.au8IP[0] = sRxAddr.sin_addr.S_un.S_un_b.s_b4;
#else
        psRxMsg->uSrcAddr.au8IP[3] = (uint8_t)((sRxAddr.sin_addr.s_addr >> 0) & 0xff);
        psRxMsg->uSrcAddr.au8IP[2] = (uint8_t)((sRxAddr.sin_addr.s_addr >> 8) & 0xff);
        psRxMsg->uSrcAddr.au8IP[1] = (uint8_t)((sRxAddr.sin_addr.s_addr >> 16) & 0xff);
        psRxMsg->uSrcAddr.au8IP[0] = (uint8_t)((sRxAddr.sin_addr.s_addr >> 24) & 0xff);
#endif

        psBuffer->u32Offset = 0;

        bOk = ORLACO_bReadMessageHeaderFromBuffer(psBuffer, psRxMsg);
        if(!bOk){
            printf("Error reading message header\n");
            continue;
        }

        // printf("RX: ServiceID=%04x MethodID=%04x Length=%08x ClientID=%04x SessionID=%04x SOME/IP Version=%d Interface Version=%d MessageType=%02x ReturnCode=%02x\n",
        //             psRxMsg->u16ServiceID,
        //             psRxMsg->u16MethodID,
        //             psRxMsg->u32Length,
        //             psRxMsg->u16ClientID,
        //             psRxMsg->u16SessionID,
        //             psRxMsg->u8SomeIPVersion,
        //             psRxMsg->u8InterfaceVersion,
        //             psRxMsg->u8MessageType,
        //             psRxMsg->u8ReturnCode);
    }
    while((psRxMsg->u16ServiceID != psInstance->u16ServiceID)&&(psRxMsg->u16MethodID != u16MethodID)&&(!bOk));

    // Check the response code
    if(psRxMsg->u8ReturnCode != E_ORLACO_RETURN_CODE_OK)
    {
        printf("Error: Response code = %d: %s\n", psRxMsg->u8ReturnCode, ORLACO_pcGetReturnCodeAsString((ORLACO_teReturnCode)psRxMsg->u8ReturnCode));
        ORLACO_vBufferDestroy(psBuffer);
        return FALSE;
    }

    // printf("Rx Success\n");

    switch(psRxMsg->u16MethodID)
    {

    case E_ORLACO_METHOD_ID_GET_DATA_SHEET:
        break;

    case E_ORLACO_METHOD_ID_GET_CAM_STATUS:
        break;

    case E_ORLACO_METHOD_ID_GET_HOST_PARAMETERS:
        break;

    case E_ORLACO_METHOD_ID_GET_REGION_OF_INTEREST:
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16P1X);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16P1Y);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16P2X);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16P2Y);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown1SetTo0x01);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown2SetTo0x00);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16Unknown3SetTo0x0000);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16OutputWidth);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16OutputHeight);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown4SetTo0x00);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8FrameRate);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16Unknown4bSetTo0x0000);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown5SetTo0x00);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown6SetTo0x02);
        bOk &= ORLACO_bReadU32(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u32MaxBitrate);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8VideoCompressionMode);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown7SetTo0x00);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown8SetTo0x00);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown9SetTo0x00);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown10SetTo0x04);
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u8Unknown11SetTo0x01);
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegionOfInterestResponsePayload.u16Unknown12SetTo0x00ff);
        break;

    case E_ORLACO_METHOD_ID_GET_REGIONS_OF_INTEREST:
        break;

    case E_ORLACO_METHOD_ID_GET_VIDEO_FORMAT:
        break;

    case E_ORLACO_METHOD_ID_GET_HISTOGRAMM_FORMAT:
        break;

    case E_ORLACO_METHOD_ID_GET_CAM_CONTROL:
        break;

    case E_ORLACO_METHOD_ID_GET_CAM_CONTROLS:
        break;

    case E_ORLACO_METHOD_ID_GET_CAM_REGISTER:
        break;

    case E_ORLACO_METHOD_ID_GET_CAM_REGISTERS:
        bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegistersResponsePayload.u16Qtty);
        for(int n = 0; n < psRxMsg->uPayload.sGetRegistersResponsePayload.u16Qtty; n++)
        {
            bOk &= ORLACO_bReadU16(psBuffer, &psRxMsg->uPayload.sGetRegistersResponsePayload.asRegisterValues[n].u16Address);
            bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegistersResponsePayload.asRegisterValues[n].u8Padding);
            bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sGetRegistersResponsePayload.asRegisterValues[n].u8Value);
        }
        break;

    case E_ORLACO_METHOD_ID_SERVICE_DISCOVERY:
        bOk &= ORLACO_bReadU8(psBuffer, &psRxMsg->uPayload.sServiceDiscoveryPayload.u8Flags);
        bOk &= ORLACO_bReadU24(psBuffer, &psRxMsg->uPayload.sServiceDiscoveryPayload.u24Reserved);
        bOk &= ORLACO_bReadU32(psBuffer, &psRxMsg->uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes);
        for(int n = 0; n < psRxMsg->uPayload.sServiceDiscoveryPayload.u32LengthOfEntriesArrayInBytes; n += ORLACO_SD_OPTION_LENGTH)
        {
            bOk &= ORLACO_bReadServiceDiscoveryServiceEntryFromBuffer(psBuffer, &psRxMsg->uPayload.sServiceDiscoveryPayload.asServiceEntry[n]);
        }
        break;

    // Either the response doesn't contain any data or no idea what it is
    default:
        break;

    }

    ORLACO_vBufferDestroy(psBuffer);

    return bOk;

}


/****************************************************************************
 *
 * NAME: ORLACO_pcGetReturnCodeAsString
 *
 * DESCRIPTION:
 * Receives a string describing the given response code
 *
 * RETURNS:
 * char * - A pointer to the text representation of the response code
 *
 ****************************************************************************/
static char *ORLACO_pcGetReturnCodeAsString(ORLACO_teReturnCode eReturnCode)
{
    typedef struct {
        ORLACO_teReturnCode eCode;
        char *pcText;
    } tsLookup;

    int n;

    const tsLookup asLookup[] = {
        {E_ORLACO_RETURN_CODE_OK                        , "Ok"},
        {E_ORLACO_RETURN_CODE_NOT_OK                    , "An unspecified error occurred"},
        {E_ORLACO_RETURN_CODE_UNKNOWN_SERVICE           , "The requested Service ID is unknown."},
        {E_ORLACO_RETURN_CODE_UNKNOWN_METHOD            , "The requested Method ID is unknown. Service ID is known."},
        {E_ORLACO_RETURN_CODE_NOT_READY                 , "Service ID and Method ID are known. Application not running."},
        {E_ORLACO_RETURN_CODE_NOT_REACHABLE             , "System running the service is not reachable (internal error code only)."},
        {E_ORLACO_RETURN_CODE_TIMEOUT                   , "A timeout occurred (internal error code only)."},
        {E_ORLACO_RETURN_CODE_WRONG_PROTOCOL_VERSION    , "Version of SOME/IP protocol not supported"},
        {E_ORLACO_RETURN_CODE_WRONG_INTERFACE_VERSION   , "Interface version mismatch"},
        {E_ORLACO_RETURN_CODE_MALFORMED_MESSAGE         , "Deserialization error, so that payload cannot be deserialized."},
        {E_ORLACO_RETURN_CODE_WRONG_MESSAGE_TYPE        , "An unexpected message type was received (e.g. REQUEST_NO_RETURN for a method defined as REQUEST.)"},
        {E_ORLACO_RETURN_CODE_LOCKED_BY_FOREIGN_INSTANCE, "Camera service is already locked by another client"},
        {E_ORLACO_RETURN_CODE_LOCK_EXPIRED              , "The camera lock has expired"},
        {E_ORLACO_RETURN_CODE_NOT_LOCKED                , "Camera is not locked"},
        {E_ORLACO_RETURN_CODE_INVALID_PS_ENTRY          , "The requested PSE ID is unknown"},
        {E_ORLACO_RETURN_CODE_INVALID_PS_OPERATION      , "The requested PSE operation is not allowed, e.g. store on a RO PSE"},
        {E_ORLACO_RETURN_CODE_INVALID_PS_DATA           , "The PSE contains a CRC16 error"},
        {E_ORLACO_RETURN_CODE_NO_MORE_SPACE             , "No more space available to store the PSE"},
        {E_ORLACO_RETURN_CODE_INVALID_ROI_INDEX         , "The requested ROI is out of range"},
        {E_ORLACO_RETURN_CODE_INVALID_ROI_NUMBER        , "The requested number of ROIs is out of range, defined by sDatasheet.numOfRegionOfInterest"},
        {E_ORLACO_RETURN_CODE_INVALID_VIDEO_FORMAT      , "Invalid value in video format"},
        {E_ORLACO_RETURN_CODE_INVALID_HISTOGRAM_FORMAT  , "Invalid value in histogram format"},
        {E_ORLACO_RETURN_CODE_INVALID_CONTROL_INDEX     , "The requested camControlIndex is out of range or not supported by the camera"},
        {E_ORLACO_RETURN_CODE_INVALID_CONTROL_MODE      , "The requested control mode is not supported by the camera"},
        {E_ORLACO_RETURN_CODE_INVALID_CONTROL_VALUE     , "The requested control value is out of the range, defined in sCamControl"},
        {E_ORLACO_RETURN_CODE_INVALID_REGISTER_ADDRESS  , "The register address is not supported by the imager"},
        {E_ORLACO_RETURN_CODE_INVALID_REGISTER_VALUE    , "The value for the given register address is not supported by the imager"},
        {E_ORLACO_RETURN_CODE_INVALID_REGISTER_OPERATION, "The operation requested is invalid for this register"},
    };
    for(n = 0; n < sizeof(asLookup) / sizeof(tsLookup); n++)
    {
        if(asLookup[n].eCode == eReturnCode) return asLookup[n].pcText;
    }

    return "Unknown return code";
}

bool_t ORLACO_bIPAlreadyInArray(ORLACO_tsInstance *psInstance, ORLACO_tuIP IP)
{
    int n;
    for(n = 0; n < psInstance->u16NumCameras; n++)
    {
        if(psInstance->psCameras[n].uIP.u32IP == IP.u32IP)
        {
            return TRUE;
        }
    }
    return FALSE;
}



/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
