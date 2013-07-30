/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef C_WIFI_H_
#define C_WIFI_H_
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <errno.h>
#include <bytecodes.h>

#include "wpa_ctrl.h"

#include "common.h"

#include "lms2012.h"

#define WIFI_PERSISTENT_PATH    vmSETTINGS_DIR          // FileSys guidance ;-)
#define WIFI_PERSISTENT_FILENAME  "WiFiConnections.dat" // Persistent storage for KNOWN connections

#define MAC_ADDRESS_LENGTH    18  // xx:xx:xx:xx:xx:xx + /0x00
#define FREQUENCY_LENGTH      5
#define SIGNAL_LEVEL_LENGTH   4
#define SECURITY_LENGTH       129 //33
#define FRIENDLY_NAME_LENGTH  33

#define PSK_LENGTH            65  // 32 bytes = 256 bit + /0x00
#define KEY_MGMT_LENGTH       33
#define PAIRWISE_LENGTH       33
#define GROUP_LENGTH          33
#define PROTO_LENGTH          33

#define WIFI_INIT_TIMEOUT     10  //60
#define WIFI_INIT_DELAY       10

#define BROADCAST_IP_LOW  "255"   // "192.168.0.255"
#define BROADCAST_PORT  	3015	  // UDP
#define TCP_PORT 5555
#define BEACON_TIME 5             // 5 sec's between BEACONs

#define TIME_FOR_WIFI_DONGLE_CHECK 10

#define BLUETOOTH_SER_LENGTH  13  // No "white" space separators
#define BRICK_HOSTNAME_LENGTH     (NAME_LENGTH + 1)

enum                    // WiFiStates
{
  WIFI_NOT_INITIATED,   // NOTHING INIT'ed
  WIFI_INIT,            // Do the time consumption stuff
  WIFI_INITIATED,       // Initiated and ready for AP:
                        // We have the H/W stack up and running
  READY_FOR_AP_SEARCH,  // Stack ready for further AP search
  SEARCH_APS,           // As it says
  SEARCH_PENDING,       // Time consumption search
  AP_LIST_UPDATED,      // User have forced a new search
  AP_CONNECTING,
  WIFI_CONNECTED_TO_AP, // User has connected to an AP
  UDP_NOT_INITIATED,    // Ready for starting Beacons
  INIT_UDP_CONNECTION,  // Do the init via UDP
  UDP_FIRST_TX,         // Temporary state after start Beacon TX
  UDP_VISIBLE,          // We TX'es a lot of Beacon
  UDP_CONNECTED,        // We have an active negotiation connection
  TCP_NOT_CONNECTED,    // Waiting for the PC to connect via TCP
  TCP_CONNECTED,        // We have a TCP connection established
  CLOSED                // UDP/TCP closed
};

enum                    // States the STACK goes through
{
  NOT_INIT          = 0x00,
  LOAD_SUPPLICANT   = 0x01,
  WAIT_ON_INTERFACE = 0x02,
  DONE              = 0x80
};

enum                    // States the TCP connection can be in (READ)
{
  TCP_IDLE                = 0x00,
  TCP_WAIT_ON_START       = 0x01,
  TCP_WAIT_ON_LENGTH      = 0x02,
  TCP_WAIT_ON_FIRST_CHUNK = 0x04,
  TCP_WAIT_ON_ONLY_CHUNK  = 0x08,
  TCP_WAIT_COLLECT_BYTES  = 0x10
};

typedef enum
{
    NO_TX,
    TX_BEACON
}
BEACON_MODE;

typedef enum
{
	HW_NOT_KNOWN,
	HW_KNOWN
}
KNOWN_HW;

typedef enum
{
  TCP_DOWN,
  TCP_UP
}
TCP_STATES;

enum                                        // WiFi AP flags "capabilities" and state
{
  VISIBLE = 0x1,
  CONNECTED = 0x02,
  WPA2 = 0x04,
  KNOWN = 0x08,
  UNKNOWN = 0x80
};

#define AP_FLAG_ADJUST_FOR_STORAGE ((unsigned char)(~(VISIBLE + CONNECTED + UNKNOWN)))  // Set/reset for persistent storage

enum                                        // The Brick WiFi state
{
  NO_CONNECTION = 0x0,
  CONNECTION_MADE = 0x01,
  SEARCHING = 0x02,
};

typedef struct
{
  char mac_address[MAC_ADDRESS_LENGTH];     // as it tells
  char frequency[FREQUENCY_LENGTH];         // additional info - not used
  char signal_level[SIGNAL_LEVEL_LENGTH];   // -
  char security[SECURITY_LENGTH];           // Only WPA2 or NONE
  char friendly_name[FRIENDLY_NAME_LENGTH]; // The name, the user will see aka SSID
  char key_management[KEY_MGMT_LENGTH];
  char pre_shared_key[PSK_LENGTH];          // Preshared Key (Encryption)
  char pairwise_ciphers[PAIRWISE_LENGTH];
  char group_ciphers[GROUP_LENGTH];
  char proto[PROTO_LENGTH];
  unsigned char ap_flags;                   // Holds the capabilities etc. of the AP
}
aps;

// Common Network stuff
// --------------------

RESULT  cWiFiGetIpAddr(char* IpAddress);

RESULT  cWiFiGetMyMacAddr(char* MacAddress);

RESULT cWiFiKnownDongleAttached(void);      // Known H/W

//UDP functions
//-------------

RESULT  cWiFiTxingBeacons(void);        // Are we active in tx beacons??

void cWiFiUdpClientClose(void);

// TCP functions
// -------------

RESULT cWiFiTcpConnected(void);         // TCP connection established?

//RESULT cWiFiTcpTempClose(void);

UWORD cWiFiWriteTcp(UBYTE* Buffer, UWORD Length);

UWORD cWiFiReadTcp(UBYTE* Buffer, UWORD Length);

// WPA and AP stuff
// ----------------
void cWiFiMoveUpInList(int Index);      // Direct UI function

void cWiFiMoveDownInList(int Index);    // Direct UI function

void cWiFiDeleteInList(int Index);      // Direct UI function

RESULT cWiFiGetApMacAddr(char* MacAddr, int Index);

RESULT cWiFiGetHiddenMacAddr(char* MacAddr, int Index);

RESULT cWiFiGetName(char *ApName, int Index, char Length);  // Get the FriendlyName owned by ApTable[Index]

RESULT cWiFiSetName(char *ApName, int Index);  // Set the FriendlyName @ ApTable[Index]  // Hidden!?

RESULT cWiFiSetSsid(char *Ssid);

RESULT cWiFiSetKeyManagToWpa2(void);

RESULT cWiFiSetKeyManagToNone(void);

RESULT cWiFiGetIndexFromName(char *Name, UBYTE *Index);

void cWiFiSetEncryptToWpa2(int Index);

void cWiFiSetEncryptToNone(int Index);

void cWiFiSetKnown(int Index);

void cWiFiDeleteAsKnown(int LocalIndex);

unsigned char cWiFiGetFlags(int Index);  // Get Flags owned by ApTable[Index]

RESULT cWiFiConnectToAp(int Index);

RESULT cWiFiMakePsk(char *ApSsid, char *PassPhrase, int Index);

int cWiFiGetApListSize(void);

void cWiFiIncApListSize(void);

RESULT cWiFiAddHidden(char *HiddenApName, char *Security, char *PassWord);

RESULT cWiFiScanForAPs(void);

RESULT cWiFiGetOnStatus(void);

// Common Control
// --------------

RESULT    cWiFiGetStatus(void);

void      cWiFiSetBtSerialNo(void);

void      cWiFiSetBrickName(void);

void      cWiFiControl(void);

RESULT    cWiFiTurnOn(void);        // TURN ON

RESULT    cWiFiTurnOff(void);       // TURN OFF

RESULT    cWiFiExit(void);

RESULT    cWiFiInit(void);

#endif /* C_WIFI_H_ */

