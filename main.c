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
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "orlaco.h"

#ifdef _WIN32
#include <windows.h>
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum
{
	E_STATUS_OK,
	E_STATUS_AGAIN,
	E_STATUS_ERROR_TIMEOUT,
	E_STATUS_ERROR_WRITING,
	E_STATUS_OUT_OF_RANGE,
	E_STATUS_NULL_PARAMETER,
	E_STATUS_FAIL
} teStatus;

typedef enum{
	E_VERBOSITY_ERRORS_ONLY = -1,
	E_VERBOSITY_LOW,
	E_VERBOSITY_MEDIUM,
	E_VERBOSITY_HIGH
} teVerbosity;

typedef struct
{
	volatile bool_t		bExitRequest;
	volatile bool_t		bExit;
	bool_t				bDiscoverCameras;
	bool_t				bReadRegisters;
	bool_t				bWriteRegisters;
	bool_t				bReadRegionsOfInterest;
	bool_t				bWriteRegionsOfInterest;
	bool_t				bSetCameraMode;
	teVerbosity			eVerbosity;
	char				*pstrIpAddress;
	int					iPort;
	ORLACO_tsInstance	sOrlaco;
} tsInstance;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

static void vParseCommandLineOptions(tsInstance *psInstance, int argc, char *argv[]);

#ifdef _WIN32
static BOOL WINAPI bCtrlHandler(DWORD dwCtrlType);
#endif

static void vPrintRegisterDefinitions(ORLACO_tsInstance *psInstance);
static bool_t bIsPrintable(char c);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

static tsInstance sInstance;

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: main
 *
 * DESCRIPTION:
 * Application entry point
 *
 * RETURNS:
 * int
 *
 ****************************************************************************/
int main(int argc, char *argv[])
{
	// int udp_socket;
	// struct sockaddr_in si_other;
	// int slen = sizeof(si_other);

#ifdef _WIN32
    WSADATA wsaData;
#endif

	int n;

	bool_t bOk = TRUE;
	ORLACO_tuIP auIP[10];
	ORLACO_tsRegionOfInterest sROI;

	int iNumCameras = 0;

	/* Initialise application state and set some defaults */
	sInstance.bReadRegisters = FALSE;
	sInstance.bWriteRegisters = FALSE;
	sInstance.bReadRegionsOfInterest = FALSE;
	sInstance.bWriteRegionsOfInterest = FALSE;

	sInstance.bExit = FALSE;
	sInstance.eVerbosity = E_VERBOSITY_MEDIUM;
	sInstance.pstrIpAddress = "127.0.0.1";

#ifdef _WIN32
   	/* Create the socket */
    WSAStartup(MAKEWORD(2, 2), &wsaData);

	SetConsoleCtrlHandler(bCtrlHandler, TRUE);
#endif



	// Need to do this before parsing command line options
	ORLACO_bInit(&sInstance.sOrlaco, "192.168.2.10", "192.168.2.255", ORLACO_DEFAULT_PORT);

	// Set the default service ID
	sInstance.sOrlaco.u16ServiceID = 0x433f;

    /* Parse the command line options */
    vParseCommandLineOptions(&sInstance, argc, argv);

	if(bOk && sInstance.bDiscoverCameras)
	{
		bOk &= ORLACO_bDiscover(&sInstance.sOrlaco);
	}

	if(bOk && (sInstance.bWriteRegisters || sInstance.bWriteRegionsOfInterest))
	{
		bOk &= ORLACO_bSetCamExclusive(&sInstance.sOrlaco, 100);
	}

	if(bOk && sInstance.bWriteRegisters)
	{
		bOk &= ORLACO_bSetRegisters(&sInstance.sOrlaco);
	}

	if(bOk && sInstance.bReadRegisters)
	{
		bOk &= ORLACO_bGetRegisters(&sInstance.sOrlaco);
	}

	if(bOk && sInstance.bWriteRegionsOfInterest)
	{
		bOk &= ORLACO_bSetRegionsOfInterest(&sInstance.sOrlaco);
	}

	if(bOk && sInstance.bReadRegionsOfInterest)
	{
		bOk &= ORLACO_bGetRegionsOfInterest(&sInstance.sOrlaco);
	}

	if(bOk && (sInstance.bWriteRegisters || sInstance.bWriteRegionsOfInterest))
	{
		bOk &= ORLACO_bEraseCamExclusive(&sInstance.sOrlaco);
	}

	if(bOk && sInstance.bDiscoverCameras)
	{
		printf("Found %d devices\n", sInstance.sOrlaco.u16NumCameras);
		for(n = 0; n < sInstance.sOrlaco.u16NumCameras; n++)
		{
			printf("%d: IP=%d.%d.%d.%d Type=%02x ServiceID=%04x InstanceID=%04x V=%d.%d\n",
				n,
				sInstance.sOrlaco.psCameras[n].uIP.au8IP[3],
				sInstance.sOrlaco.psCameras[n].uIP.au8IP[2],
				sInstance.sOrlaco.psCameras[n].uIP.au8IP[1],
				sInstance.sOrlaco.psCameras[n].uIP.au8IP[0],
				sInstance.sOrlaco.psCameras[n].sDiscoveryServiceEntry.u8Type,
				sInstance.sOrlaco.psCameras[n].sDiscoveryServiceEntry.u16ServiceID,
				sInstance.sOrlaco.psCameras[n].sDiscoveryServiceEntry.u16InstanceID,
				sInstance.sOrlaco.psCameras[n].sDiscoveryServiceEntry.u8MajorVersion,
				sInstance.sOrlaco.psCameras[n].sDiscoveryServiceEntry.u32MinorVersion);
		}
	}

	if(bOk && (sInstance.bReadRegisters || sInstance.bWriteRegisters))
	{
		if(sInstance.eVerbosity >= E_VERBOSITY_MEDIUM) printf("\nRegisters\nIndex\tAddress\tHex\tDecimal\tAscii\tName\n");
		for(n = 0; n < sInstance.sOrlaco.u16NumRegisters; n++)
		{
			if(sInstance.sOrlaco.psRegisters[n].bRead || sInstance.sOrlaco.psRegisters[n].bWrite)
			{
				printf("%02d\t0x%04x\t0x%02x\t%3d\t%c\t%s\n", n, sInstance.sOrlaco.psRegisters[n].u16Address, sInstance.sOrlaco.psRegisters[n].u8Value, sInstance.sOrlaco.psRegisters[n].u8Value, bIsPrintable(sInstance.sOrlaco.psRegisters[n].u8Value) ? sInstance.sOrlaco.psRegisters[n].u8Value : ' ', sInstance.sOrlaco.psRegisters[n].pcDescription);
			}
		}
	}

	if(bOk && (sInstance.bReadRegionsOfInterest || sInstance.bWriteRegionsOfInterest))
	{
		if(sInstance.eVerbosity >= E_VERBOSITY_MEDIUM) printf("\nRegions Of Interest\nROI\tP1X\tP1Y\tP2X\tP2Y\tWidth\tHeight\tMbps\tFps\tMode\n");

		for(n = 1; n < sInstance.sOrlaco.u16NumRegionsOfInterest; n++)
		{
			if(sInstance.sOrlaco.psRegionsOfInterest[n].bRead || sInstance.sOrlaco.psRegionsOfInterest[n].bWrite)
			{
				printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
						n,
						sInstance.sOrlaco.psRegionsOfInterest[n].u16P1X,
						sInstance.sOrlaco.psRegionsOfInterest[n].u16P1Y,
						sInstance.sOrlaco.psRegionsOfInterest[n].u16P2X,
						sInstance.sOrlaco.psRegionsOfInterest[n].u16P2Y,
						sInstance.sOrlaco.psRegionsOfInterest[n].u16OutputWidth,
						sInstance.sOrlaco.psRegionsOfInterest[n].u16OutputHeight,
						sInstance.sOrlaco.psRegionsOfInterest[n].u32MaxBitrate,
						sInstance.sOrlaco.psRegionsOfInterest[n].u8FrameRate,
						sInstance.sOrlaco.psRegionsOfInterest[n].eCompressionMode);
			}

		}
	}

	if(bOk && sInstance.bSetCameraMode)
	{
		bOk &= ORLACO_bSetCamMode(&sInstance.sOrlaco, sInstance.sOrlaco.eCameraMode);
	}

	ORLACO_vDeInit(&sInstance.sOrlaco);





    /* Main program loop, execute until we get a signal requesting to exit */
    // while(!sInstance.bExit)
    // {

    // 	if(sInstance.bExitRequest)
    // 	{
	// 		sInstance.bExitRequest = FALSE;
    // 	}

    // }

#ifdef _WIN32
    /* Tidy up and then exit */
	WSACleanup();
#endif

	return EXIT_SUCCESS;
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 *
 * NAME: vParseCommandLineOptions
 *
 * DESCRIPTION:
 * Parse command line options
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void vParseCommandLineOptions(tsInstance *psInstance, int argc, char *argv[])
{

	int n;
	int c;
	char *token, *fromStr, *toStr, *ipStr, *portStr;
	int index, value, from, to, port;

	static const struct option lopts[] = {
		{ "discover",		required_argument,	0, 	'd'	},
		{ "write-reg",		required_argument,	0, 	'w'	},
		{ "read-reg",		required_argument,	0, 	'r'	},
		{ "read-regs",		required_argument,	0, 	'R'	},

		{ "get-roi",		required_argument,	0, 	'g'	},
		{ "get-rois",		required_argument,	0, 	'G'	},
		{ "set-roi",		required_argument,	0, 	's'	},

		{ "ip", 			required_argument,	0, 	'i'	},
		{ "service-id",		required_argument,	0, 	'e'	},

		{ "set-mode", 		required_argument,	0, 	'm'	},

        { "verbosity",     	required_argument, 	0,  'v' },

        { "help",       	no_argument,		0,  'h' },
        { "help",          	no_argument, 		0,  '?' },

		{ NULL, 0, 0, 0 },
	};


	while(1)
	{

		c = getopt_long(argc, argv, "d:w:r:R:g:G:s:i:e:m:v:?h", lopts, NULL);

		if (c == -1)
			break;

		switch(c)
		{

		case 'd':
			port = ORLACO_DEFAULT_PORT;
			ipStr = strtok(optarg, ":");
			portStr = strtok(NULL, ":");
			if(portStr != NULL)
			{
				port = (uint16_t)atoi(portStr);
			}
			if(!ORLACO_bSetBroadcastIP(&psInstance->sOrlaco, ipStr, port))
			{
				printf("Error: Failed to set the broadcast IP to %s:%d\n", ipStr, port);
				exit(EXIT_FAILURE);
			}
			psInstance->bDiscoverCameras = TRUE;
			break;

		case 'i':
			port = ORLACO_DEFAULT_PORT;
			ipStr = strtok(optarg, ":");
			portStr = strtok(NULL, ":");
			if(portStr != NULL)
			{
				port = (uint16_t)atoi(portStr);
			}
			if(!ORLACO_bSetUnicastIP(&psInstance->sOrlaco, ipStr, port))
			{
				printf("Error: Failed to set the camera IP to %s:%d\n", ipStr, port);
				exit(EXIT_FAILURE);
			}
			break;

		case 'e':
			psInstance->sOrlaco.u16ServiceID = 0x433f;
			psInstance->sOrlaco.u16ServiceID = (uint16_t)atoi(optarg);
			if(psInstance->eVerbosity >= E_VERBOSITY_MEDIUM) printf("Service ID set to 0x%04x\n", psInstance->sOrlaco.u16ServiceID);
			break;

		case 'w':
			token = strtok(optarg, "=");
			index = (uint8_t)atoi(token);
			value = (uint8_t)atoi(strtok(NULL, "="));
			if(index < psInstance->sOrlaco.u16NumRegisters)
			{
				if(psInstance->eVerbosity >= E_VERBOSITY_MEDIUM) printf("Write register %d value %02x\n", index, value);
				psInstance->sOrlaco.psRegisters[index].u8Value = value;
				psInstance->sOrlaco.psRegisters[index].bWrite = TRUE;
			}
			else
			{
				printf("Error: Register index %d is out of range, max is %d\n", index, psInstance->sOrlaco.u16NumRegisters - 1);
				exit(EXIT_FAILURE);
			}
			psInstance->bWriteRegisters |= TRUE;
			break;

		case 'r':
			index = (uint8_t)atoi(optarg);
			if(index < psInstance->sOrlaco.u16NumRegisters)
			{
				if(psInstance->eVerbosity >= E_VERBOSITY_MEDIUM) printf("Read register %d\n", index);
				psInstance->sOrlaco.psRegisters[index].bRead = TRUE;
			}
			else
			{
				printf("Error: Register index %d is out of range, max is %d\n", index, psInstance->sOrlaco.u16NumRegisters - 1);
				exit(EXIT_FAILURE);
			}
			psInstance->bReadRegisters |= TRUE;
			break;

		case 'R':
			from = 0;
			to = psInstance->sOrlaco.u16NumRegisters - 1;
			fromStr = strtok(optarg, ":");
			if(fromStr != NULL)
			{
				from = atoi(fromStr);
			}
			toStr = strtok(NULL, ":");
			if(toStr != NULL)
			{
				to = atoi(toStr);
			}
			if(from >= psInstance->sOrlaco.u16NumRegisters)
			{
				printf("Error: From value %d is out of range, max is %d\n", from, psInstance->sOrlaco.u16NumRegisters - 1);
				exit(EXIT_FAILURE);
			}
			if(to >= psInstance->sOrlaco.u16NumRegisters)
			{
				printf("Error: To value %d is out of range, max is %d\n", to, psInstance->sOrlaco.u16NumRegisters - 1);
				exit(EXIT_FAILURE);
			}
			if(from >= to)
			{
				printf("Error: To value %d must be greater than from value %d\n", to, from);
				exit(EXIT_FAILURE);
			}
			if(psInstance->eVerbosity >= E_VERBOSITY_MEDIUM) printf("Read registers %d to %d\n", from, to);
			for(n = from; n <= to; n++)
			{
				psInstance->sOrlaco.psRegisters[n].bRead = TRUE;
			}
			psInstance->bReadRegisters |= TRUE;
			break;

		case 'g':
			index = (uint8_t)atoi(optarg);
			if((index > 0) && (index < psInstance->sOrlaco.u16NumRegisters))
			{
				if(psInstance->eVerbosity >= E_VERBOSITY_MEDIUM) printf("Get ROI %d\n", index);
				psInstance->sOrlaco.psRegionsOfInterest[index].bRead = TRUE;
			}
			else
			{
				printf("Error: ROI index %d is out of range, min is 1, max is %d\n", index, psInstance->sOrlaco.u16NumRegionsOfInterest - 1);
				exit(EXIT_FAILURE);
			}
			psInstance->bReadRegionsOfInterest |= TRUE;
			break;

		case 'G':
			from = 1;
			to = psInstance->sOrlaco.u16NumRegionsOfInterest - 1;
			fromStr = strtok(optarg, ":");
			if(fromStr != NULL)
			{
				from = atoi(fromStr);
			}
			toStr = strtok(NULL, ":");
			if(toStr != NULL)
			{
				to = atoi(toStr);
			}
			if((from < 1) || (from >= psInstance->sOrlaco.u16NumRegionsOfInterest))
			{
				printf("Error: From value %d is out of range, min is 1, max is %d\n", from, psInstance->sOrlaco.u16NumRegionsOfInterest - 1);
				exit(EXIT_FAILURE);
			}
			if((to < 1) || (to >= psInstance->sOrlaco.u16NumRegionsOfInterest))
			{
				printf("Error: To value %d is out of range, min is 1, max is %d\n", to, psInstance->sOrlaco.u16NumRegionsOfInterest - 1);
				exit(EXIT_FAILURE);
			}
			if(from >= to)
			{
				printf("Error: To value %d must be greater than from value %d\n", to, from);
				exit(EXIT_FAILURE);
			}
			if(psInstance->eVerbosity >= E_VERBOSITY_MEDIUM) printf("Read ROI's %d to %d\n", from, to);
			for(n = from; n <= to; n++)
			{
				psInstance->sOrlaco.psRegionsOfInterest[n].bRead = TRUE;
			}
			psInstance->bReadRegionsOfInterest |= TRUE;
			break;

		case 's':
			{
				index = (uint8_t)atoi(strtok(optarg, "="));

				if((index > 0) && (index < psInstance->sOrlaco.u16NumRegisters))
				{

					psInstance->sOrlaco.psRegionsOfInterest[index].u16P1X = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u16P1Y = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u16P2X = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u16P2Y = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u16OutputWidth = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u16OutputHeight = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u32MaxBitrate = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].u8FrameRate = atoi(strtok(NULL, ","));
					psInstance->sOrlaco.psRegionsOfInterest[index].eCompressionMode = (ORLACO_teVideoCompressionMode)atoi(strtok(NULL, ","));

					if(psInstance->eVerbosity >= E_VERBOSITY_HIGH) printf("Set ROI %d P1X=%d P1Y=%d P2X=%d P2Y=%d Width=%d Height=%d MaxBitRate=%dMb/s FrameRate=%dfps Mode=%d\n",
						index,
						psInstance->sOrlaco.psRegionsOfInterest[index].u16P1X,
						psInstance->sOrlaco.psRegionsOfInterest[index].u16P1Y,
						psInstance->sOrlaco.psRegionsOfInterest[index].u16P2X,
						psInstance->sOrlaco.psRegionsOfInterest[index].u16P2Y,
						psInstance->sOrlaco.psRegionsOfInterest[index].u16OutputWidth,
						psInstance->sOrlaco.psRegionsOfInterest[index].u16OutputHeight,
						psInstance->sOrlaco.psRegionsOfInterest[index].u32MaxBitrate,
						psInstance->sOrlaco.psRegionsOfInterest[index].u8FrameRate,
						psInstance->sOrlaco.psRegionsOfInterest[index].eCompressionMode);
					psInstance->sOrlaco.psRegionsOfInterest[index].bWrite = TRUE;
					psInstance->bWriteRegionsOfInterest |= TRUE;
				}
				else
				{
					printf("Error: ROI index %d is out of range, min is 1, max is %d\n", index, psInstance->sOrlaco.u16NumRegionsOfInterest - 1);
					exit(EXIT_FAILURE);
				}
			}
			break;

		case 'm':
			psInstance->sOrlaco.eCameraMode = (ORLACO_teCameraMode)atoi(optarg);
			psInstance->bSetCameraMode = TRUE;
			break;

		case 'v':
			switch(atoi(optarg))
			{
			case -1:
				psInstance->eVerbosity = E_VERBOSITY_ERRORS_ONLY;
				ORLACO_vSetVerbosity(&psInstance->sOrlaco, E_ORLACO_VERBOSITY_ERRORS_ONLY);
				break;

			case 0:
				psInstance->eVerbosity = E_VERBOSITY_LOW;
				ORLACO_vSetVerbosity(&psInstance->sOrlaco, E_ORLACO_VERBOSITY_ERRORS_ONLY);
				break;
			
			case 1:
				psInstance->eVerbosity = E_VERBOSITY_MEDIUM;
				ORLACO_vSetVerbosity(&psInstance->sOrlaco, E_ORLACO_VERBOSITY_INFO);
				break;
			
			case 2:
				psInstance->eVerbosity = E_VERBOSITY_HIGH;
				ORLACO_vSetVerbosity(&psInstance->sOrlaco, E_ORLACO_VERBOSITY_DEBUG);
				break;

			default:
				psInstance->eVerbosity = E_VERBOSITY_HIGH;
				ORLACO_vSetVerbosity(&psInstance->sOrlaco, E_ORLACO_VERBOSITY_DEBUG);
				break;

			}
			break;

        case '?':
		case 'h':
		default:
			printf("+----------------------------------------------------------------------+\n" \
		   "|               OCC (Orlaco Camera Configurator)                       |\n" \
		   "| Copyright (C) 2021 Lee Mitchell <lee@indigopepper.com>               |\n" \
		   "|                                                                      |\n" \
	       "| This program comes with ABSOLUTELY NO WARRANTY.                      |\n" \
		   "| This is free software, and you are welcome to redistribute it        |\n" \
		   "| under certain conditions; See the GNU General Public License         |\n" \
		   "| version 3 or later for more details. You should have received a copy |\n" \
		   "| of the GNU General Public License along with OCC (Orlaco Camera      |\n" \
		   "| Configurator) If not, see <http://www.gnu.org/licenses/>.            |\n" \
		   "+----------------------------------------------------------------------+\n\n");

			if((optarg != NULL) && (strcasecmp(optarg, "regs") == 0))
			{
				vPrintRegisterDefinitions(&psInstance->sOrlaco);
			}
			else
			{
				printf("\nUsage: %s <options>\n\n", argv[0]);
				puts("  -d --discover <IP>:<Port>        Discover cameras using broadcast IP:Port(17215 default)\n\n"
					"  -i --ip <IP>:<port>              Set the IP address and port of the camera, e.g. 192.168.2.1:17215\n\n"
					"  -e --service-id <service-id>     Set the service id to <service-id>\n\n"
					"  -r --read-reg  <index>           Read the value from register <index>\n\n"
					"  -R --read-regs <from>:<to>       Read the value from register at index <from> to index <to>\n\n"
					"  -w --write-reg <index>=<value>   Write <value> into register <index>\n\n"
					"  -g --get-roi <index>             Read the Region Of Interest at index <index>\n\n"
					"  -G --read-rois <from>:<to>       Read the Region Of Interest at index <from> to index <to>\n\n"
					"  -s --set-roi <index>=<p1x>,<p1y>,<p2x>,<p2y>,<width>,<height>,<maxBitRate>,<fps>,<compression mode>\n"
					"                                   Write region of interest at index <index>\n\n"
					"  -v --verbosity <level>           Set verbosity level -1, 0, 1 & 2 are valid\n\n"
					"  -q --quiet                       Enable quiet mode (no updates on console)\n\n"
					"  -d --debug                       Enable debugging mode (extra console messages)\n\n"
					"  -? --help                        Display help\n\n"
					"     --help regs                   Display the list of registers available\n\n"
					);
			}

			exit(EXIT_FAILURE);
			break;
		}

	}

}


/****************************************************************************
 *
 * NAME: bCtrlHandler
 *
 * DESCRIPTION:
 * Handles Ctrl+C events
 *
 * RETURNS:
 * BOOL
 *
 ****************************************************************************/
#ifdef _WIN32
static BOOL WINAPI bCtrlHandler(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {

    case CTRL_C_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        printf("\nExit requested\n");
        sInstance.bExitRequest = TRUE;
        return TRUE;

    case CTRL_BREAK_EVENT:
        printf("\nImmediate exit requested\n");
        sInstance.bExit = TRUE;
    	return TRUE;

    default:
        return FALSE;
    }
}
#endif

/****************************************************************************
 *
 * NAME: vPrintRegisterDefinitions
 *
 * DESCRIPTION:
 * Prints out a list of the registers in the Orlaco Emos camera
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
static void vPrintRegisterDefinitions(ORLACO_tsInstance *psInstance)
{
	int n;

	printf("Orlaco EMOS Camera Registers:\n\n"\
		   "Index\tDescription\n"\
		   "-----\t-----------\n");
	for(n = 0; n < psInstance->u16NumRegisters; n++)
	{
		printf("%02d\t%s %s\n", n, psInstance->psRegisters[n].pcDescription, psInstance->psRegisters[n].pcHelp);
	}

}

static bool_t bIsPrintable(char c)
{
	if(c >=32 && c <=127)
	{
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

