#ifndef ORLACO_H
#define ORLACO_H

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <stdint.h>
#include <stdlib.h>

// https://stackoverflow.com/questions/28027937/cross-platform-sockets

#ifdef _WIN32
    /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x0501  /* Windows XP. */
    #endif
    #include <winsock2.h>
    #include <Ws2tcpip.h>
#else
    /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
    #include <unistd.h> /* Needed for close() */
#endif

#include "common.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define ORLACO_DEFAULT_PORT             17215
#define ORLACO_MAX_CAMERAS              10
#define ORLACO_MAX_REGISTERS            100
#define ORLACO_MAX_SD_SERVICES          10
#define ORLACO_MAX_SD_OPTIONS           2
#define ORLACO_MAX_RESPONSE_TIME_MS     5000

#define ORLACO_SD_OPTION_LENGTH         0x10

#define ORLACO_BUFFER_LENGTH            1500
#define ORLACO_NUM_REGIONS_OF_INTEREST  11

#ifndef TRUE
#define TRUE                            (1)
#endif

#ifndef FALSE
#define FALSE                           (0)
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum {
    E_ORLACO_VERBOSITY_ERRORS_ONLY = 0,
    E_ORLACO_VERBOSITY_INFO,
    E_ORLACO_VERBOSITY_DEBUG,
} ORLACO_eVerbosityLevel;

typedef enum {
    E_ORLACO_RETURN_CODE_OK                            = 0x00, // No error occurred
    E_ORLACO_RETURN_CODE_NOT_OK                        = 0x01, // An unspecified error occurred
    E_ORLACO_RETURN_CODE_UNKNOWN_SERVICE               = 0x02, // The requested Service ID is unknown.
    E_ORLACO_RETURN_CODE_UNKNOWN_METHOD                = 0x03, // The requested Method ID is unknown. Service ID is known.
    E_ORLACO_RETURN_CODE_NOT_READY                     = 0x04, // Service ID and Method ID are known. Application not running.
    E_ORLACO_RETURN_CODE_NOT_REACHABLE                 = 0x05, // System running the service is not reachable (internal error code only).
    E_ORLACO_RETURN_CODE_TIMEOUT                       = 0x06, // A timeout occurred (internal error code only).
    E_ORLACO_RETURN_CODE_WRONG_PROTOCOL_VERSION        = 0x07, // Version of SOME/IP protocol not supported
    E_ORLACO_RETURN_CODE_WRONG_INTERFACE_VERSION       = 0x08, // Interface version mismatch
    E_ORLACO_RETURN_CODE_MALFORMED_MESSAGE             = 0x09, // Deserialization error, so that payload cannot be deserialized.
    E_ORLACO_RETURN_CODE_WRONG_MESSAGE_TYPE            = 0x0a, // An unexpected message type was received (e.g. REQUEST_NO_RETURN for a method defined as REQUEST.)

    E_ORLACO_RETURN_CODE_LOCKED_BY_FOREIGN_INSTANCE    = 0x20, // Camera service is already locked by another client
    E_ORLACO_RETURN_CODE_LOCK_EXPIRED                  = 0x21, // The camera lock has expired
    E_ORLACO_RETURN_CODE_NOT_LOCKED                    = 0x22, // Camera is not locked
    E_ORLACO_RETURN_CODE_INVALID_PS_ENTRY              = 0x24, // The requested PSE ID is unknown
    E_ORLACO_RETURN_CODE_INVALID_PS_OPERATION          = 0x25, // The requested PSE operation is not allowed, e.g. store on a RO PSE
    E_ORLACO_RETURN_CODE_INVALID_PS_DATA               = 0x26, // The PSE contains a CRC16 error
    E_ORLACO_RETURN_CODE_NO_MORE_SPACE                 = 0x27, // No more space available to store the PSE
    E_ORLACO_RETURN_CODE_INVALID_ROI_INDEX             = 0x30, // The requested ROI is out of range
    E_ORLACO_RETURN_CODE_INVALID_ROI_NUMBER            = 0x31, // The requested number of ROIs is out of range, defined by sDatasheet.numOfRegionOfInterest
    E_ORLACO_RETURN_CODE_INVALID_VIDEO_FORMAT          = 0x32, // Invalid value in video format
    E_ORLACO_RETURN_CODE_INVALID_HISTOGRAM_FORMAT      = 0x33, // Invalid value in histogram format
    E_ORLACO_RETURN_CODE_INVALID_CONTROL_INDEX         = 0x35, // The requested camControlIndex is out of range or not supported by the camera
    E_ORLACO_RETURN_CODE_INVALID_CONTROL_MODE          = 0x36, // The requested control mode is not supported by the camera
    E_ORLACO_RETURN_CODE_INVALID_CONTROL_VALUE         = 0x37, // The requested control value is out of the range, defined in sCamControl
    E_ORLACO_RETURN_CODE_INVALID_REGISTER_ADDRESS      = 0x38, // The register address is not supported by the imager
    E_ORLACO_RETURN_CODE_INVALID_REGISTER_VALUE        = 0x39, // The value for the given register address is not supported by the imager
    E_ORLACO_RETURN_CODE_INVALID_REGISTER_OPERATION    = 0x3A
} ORLACO_teReturnCode;


// Must make sure these are in the same order as the register addresses
typedef enum {
    E_ORLACO_REGISTER_INDEX_LED_MODE                   = 0,
    E_ORLACO_REGISTER_INDEX_STREAM_PROTOCOL,
    E_ORLACO_REGISTER_INDEX_STATIC_IP_ADDRESS_0,
    E_ORLACO_REGISTER_INDEX_STATIC_IP_ADDRESS_1,
    E_ORLACO_REGISTER_INDEX_STATIC_IP_ADDRESS_2,
    E_ORLACO_REGISTER_INDEX_STATIC_IP_ADDRESS_3,
    E_ORLACO_REGISTER_INDEX_STATIC_NETWORK_MASK_0,
    E_ORLACO_REGISTER_INDEX_STATIC_NETWORK_MASK_1,
    E_ORLACO_REGISTER_INDEX_STATIC_NETWORK_MASK_2,
    E_ORLACO_REGISTER_INDEX_STATIC_NETWORK_MASK_3,
    E_ORLACO_REGISTER_INDEX_MAC_ADDRESS_0,
    E_ORLACO_REGISTER_INDEX_MAC_ADDRESS_1,
    E_ORLACO_REGISTER_INDEX_MAC_ADDRESS_2,
    E_ORLACO_REGISTER_INDEX_MAC_ADDRESS_3,
    E_ORLACO_REGISTER_INDEX_MAC_ADDRESS_4,
    E_ORLACO_REGISTER_INDEX_MAC_ADDRESS_5,
    E_ORLACO_REGISTER_INDEX_VLAN_ID_0,
    E_ORLACO_REGISTER_INDEX_VLAN_ID_1,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_0,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_1,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_2,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_3,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_4,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_5,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_6,
    E_ORLACO_REGISTER_INDEX_STREAM_ID_7,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_IP_ADDRESS_0,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_IP_ADDRESS_1,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_IP_ADDRESS_2,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_IP_ADDRESS_3,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_MAC_ADDRESS_0,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_MAC_ADDRESS_1,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_MAC_ADDRESS_2,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_MAC_ADDRESS_3,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_MAC_ADDRESS_4,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_MAC_ADDRESS_5,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_PORT_0,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_DESTINATION_PORT_1,
    E_ORLACO_REGISTER_INDEX_SELECTED_ROI,
    E_ORLACO_REGISTER_INDEX_NO_STREAM_AT_BOOT,
    E_ORLACO_REGISTER_INDEX_UDP_COMMUNICATION_PORT_0,
    E_ORLACO_REGISTER_INDEX_UDP_COMMUNICATION_PORT_1,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_SOURCE_PORT_0,
    E_ORLACO_REGISTER_INDEX_RTP_STREAM_SOURCE_PORT_1,
    E_ORLACO_REGISTER_INDEX_HDR,
    E_ORLACO_REGISTER_INDEX_OVERLAY,
    E_ORLACO_REGISTER_INDEX_DHCP,
    E_ORLACO_REGISTER_INDEX_WAIT_FOR_MAC,
    E_ORLACO_REGISTER_INDEX_WAIT_FOR_PTP_SYNC,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_0,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_1,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_2,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_3,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_4,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_5,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_6,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_7,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_8,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_9,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_10,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_11,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_12,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_13,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_14,
    E_ORLACO_REGISTER_INDEX_DHCP_HOSTNAME_15,
} ORLACO_teRegisterIndex;

typedef enum {
    E_ORLACO_LED_MODE_OFF                                          = 0x0000,
    E_ORLACO_LED_MODE_AUTO                                         = 0x0001,
    E_ORLACO_LED_MODE_ON                                           = 0x0002,
} ORLACO_teLedMode;

typedef enum {
    E_ORLACO_DHCP_MODE_DISABLED                                    = 0x0000,
    E_ORLACO_DHCP_MODE_ENABLED                                     = 0x0001,
} ORLACO_teDHCPMode;

typedef enum {
    E_ORLACO_VIDEO_COMPRESSION_MODE_NONE                           = 0x00,
    E_ORLACO_VIDEO_COMPRESSION_MODE_JPEG                           = 0x01,
    E_ORLACO_VIDEO_COMPRESSION_MODE_H264                           = 0x02,
} ORLACO_teVideoCompressionMode;


typedef enum {
    E_ORLACO_CAMERA_MODE_START_CAMERA_SERVICE                       = 0x01, // The camera application will be started
    E_ORLACO_CAMERA_MODE_STOP_CAMERA_SERVICE                        = 0x02, // The camera application will be stopped
    E_ORLACO_CAMERA_MODE_RESTART_CAMERA_SERVICE                     = 0x03, // The camera application will be restarted
    E_ORLACO_CAMERA_MODE_STOP_CAMERA                                = 0x04, // The camera will be stopped (standby mode). Requires a power cycle or wake on LAN to recover
} ORLACO_teCameraMode;

typedef struct {
    uint16_t u16Address;
    uint8_t u8Padding;
    uint8_t u8Value;
    char *pcDescription;
    char *pcHelp;
    bool_t bWrite;
    bool_t bRead;
} ORLACO_tsRegisterValue;


typedef union {
    uint8_t au8IP[4];
    uint32_t u32IP;
} ORLACO_tuIP;


typedef struct {
    uint16_t u16P1X;
    uint16_t u16P1Y;
    uint16_t u16P2X;
    uint16_t u16P2Y;
    uint16_t u16OutputWidth;
    uint16_t u16OutputHeight;
    uint32_t u32MaxBitrate;                         // Specified in Megabits per second
    uint8_t u8FrameRate;
    ORLACO_teVideoCompressionMode eCompressionMode;
    bool_t bWrite;
    bool_t bRead;
} ORLACO_tsRegionOfInterest;


typedef struct {
    uint8_t u8Type;
    uint8_t u8Index1stOptions;
    uint8_t u8Index2ndOptions;
    uint8_t u8NumberOfOptions;
    uint16_t u16ServiceID;
    uint16_t u16InstanceID;
    uint8_t u8MajorVersion;
    uint32_t u24TTL;
    uint32_t u32MinorVersion;
} ORLACO_tsServiceDiscoveryServiceEntry;


typedef struct {
    ORLACO_tuIP uIP;
    ORLACO_tsServiceDiscoveryServiceEntry sDiscoveryServiceEntry;
} ORLACO_tsCamera;

#ifdef _WIN32
    typedef unsigned int UDPSOCKET;
#else
    typedef int UDPSOCKET;
#endif

typedef struct {
    ORLACO_eVerbosityLevel eVerbosity;
    struct sockaddr_in fdServer;
    struct sockaddr_in fdUnicast;
    struct sockaddr_in fdBroadcast;
    UDPSOCKET Socket;
    uint16_t u16DstPort;
    uint16_t u16ServiceID;
    uint16_t u16ClientID;
    uint16_t u16SessionID;
    uint16_t u16ResponseTimer;
    ORLACO_teCameraMode eCameraMode;
    uint16_t u16NumRegisters;
    ORLACO_tsRegisterValue *psRegisters;
    uint16_t u16NumRegionsOfInterest;
    ORLACO_tsRegionOfInterest *psRegionsOfInterest;
    uint16_t u16NumCameras;
    ORLACO_tsCamera *psCameras;
} ORLACO_tsInstance;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

bool_t ORLACO_bInit(ORLACO_tsInstance *psInstance, char *pcUnicastIP, char *pcBroadcastIP, uint16_t u16DstPort);
void ORLACO_vDeInit(ORLACO_tsInstance *psInstance);
void ORLACO_vSetVerbosity(ORLACO_tsInstance *psInstance, ORLACO_eVerbosityLevel eVerbosityLevel);
bool_t ORLACO_bSetBroadcastIP(ORLACO_tsInstance *psInstance, char *pcIpAddress, uint16_t u16DstPort);
bool_t ORLACO_bSetUnicastIP(ORLACO_tsInstance *psInstance, char *pcIpAddress, uint16_t u16DstPort);

// bool_t ORLACO_bBufferTest(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bDiscover(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bSetCamExclusive(ORLACO_tsInstance *psInstance, uint32_t u32ExclusiveTime);
bool_t ORLACO_bEraseCamExclusive(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bSetCamMode(ORLACO_tsInstance *psInstance, ORLACO_teCameraMode eMode);
bool_t ORLACO_bGetRegisters(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bSetRegisters(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bGetAllRegisters(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bGetRegionOfInterest(ORLACO_tsInstance *psInstance, uint32_t u32RegionOfInterest, ORLACO_tsRegionOfInterest *psRegionOfInterest);
bool_t ORLACO_bGetRegionsOfInterest(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bSetRegionOfInterest(ORLACO_tsInstance *psInstance, uint32_t u32RegionOfInterestIndex, ORLACO_tsRegionOfInterest *psRegionOfInterest);
bool_t ORLACO_bSetRegionsOfInterest(ORLACO_tsInstance *psInstance);
bool_t ORLACO_bSubscribeRoiVideo(ORLACO_tsInstance *psInstance, uint32_t u32RegionOfInterest);


#endif // ORLACO_H

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
