/*!
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2012, Ingenico.
 28-32 boulevard de Grenelle 75015 Paris, France.
 All rights reserved.

 This source program is the property of INGENICO Company and may not be copied
 in any form or by any means, whether in part or in whole, except under license
 expressly granted by INGENICO company

 All copies of this program, whether in part or in whole, and
 whether modified or not, must display this and all other
 embedded copyright and ownership notices in full.
 ------------------------------------------------------------------------------

 Project : Wifi  Sample
 Module  : FUN - Demo Features Unit

 @file      fun_wifi.c
 @brief     Contains all definitions of functions utilized for Wifi.
 @date      04/16/2013

 ------------------------------------------------------------------------------
 */

#include "sdk.h"
#include <GL_GraphicLib.h>
#include "TlvTree.h"
#include "LinkLayer.h"
#include "GTL_Traces.h"

#include "globals.h"
//#include "par.h"
//#include "flow.h"
//#include "Utilities.h"
//#include "err_process.h"

#define CHECK(CND,LBL) {if(!(CND)){goto LBL;}}

static int comStartWifi(void);
static int comSetWifi(void);
static int comDialIp(word tlsSSL);
static int comSendIp(const byte * req, word usLen);
static int comRecvIp(byte * response, word usLen);
static int comHangStartIp(void);
static int comStopWifi(void);

typedef struct {
	char* m_szTest;
	char* m_szPinCode;
	char* m_szAPN;
	char* m_szHostName;
	unsigned long m_nPort;
} T_Wifi_TEST_SETTINGS;

typedef struct sComChn {
	struct {
		LL_HANDLE hdl;
		unsigned long int conn_delay;   // 4 Second delay
		unsigned long int ll_conn_delay;
		unsigned int send_timeout;
		int ifconn;
		struct {
			unsigned int cTimeout;
			unsigned int cValue;
		} Wifi;
	} prm;
	TLV_TREE_NODE hCfg;
	TLV_TREE_NODE hPhyCfg;
	TLV_TREE_NODE hDLinkCfg;
	TLV_TREE_NODE hTransCfg;
} tComChn;

static tComChn com;



/** Start a wifi connection using Link Layer
 * \return 1 if started succesfully, negative on error
 */
static int comStartWifi(void) {
	int ret=1;

	com.hCfg = NULL;
	com.hDLinkCfg = NULL;
	com.hPhyCfg = NULL;
	com.hTransCfg = NULL;

	//ret = Wifi_IsDevicePresent();
	return ret;
}

/** Set wifi settings using Link Layer
 * \return 1 if wifi is set succesfully, negative on error
 */
static int comSetWifi(void) {
	int ret;
	int err;
	int status;

	com.prm.conn_delay = 1 * 100;   //4 Second delay
	com.prm.send_timeout = 100 * 100;
	com.prm.ifconn = FALSE;
	com.prm.Wifi.cTimeout = 20 * 100;
	com.prm.Wifi.cValue = 100 * 100;

	err = LL_Network_GetStatus(LL_PHYSICAL_V_WIFI, &status);
	switch (status) {
	case LL_STATUS_WIFI_CONNECTED:
		break;

	case LL_STATUS_WIFI_NOT_CONNECTED:
	default:
		break;
	}

	/*
    ret = Wifi_IsConnected();
    if(ret == 1)
    {
        com.prm.ifconn = TRUE;
        goto lblOK;
    }

    Wifi_SetBootproto(DLL_WIFI_BOOT_PROTO_DHCP);
	 */

	//lblOK:

	if(com.hPhyCfg)
		TlvTree_Release(com.hPhyCfg);

	if(com.hCfg)
		TlvTree_Release(com.hCfg);

	com.hCfg = TlvTree_New(LL_TAG_LINK_LAYER_CONFIG);
	com.hPhyCfg = TlvTree_AddChild(com.hCfg, LL_TAG_PHYSICAL_LAYER_CONFIG, NULL, 0);
	TlvTree_AddChildInteger(com.hPhyCfg, LL_PHYSICAL_T_LINK, (unsigned char) LL_PHYSICAL_V_WIFI, LL_PHYSICAL_L_LINK);
	ret = 1;

	(void)err;
	return ret;
}

/** Connect to host using Link Layer
 * \param[in] adr ip address of the host
 * \param[in] dPort port used by the host
 *
 * \return 1 if connected succesfully, negative on error
 */
static int comDialIp(word tlsSSL) {
	int ret = -1;
	char adr[100 + 1];
	char port[100 + 1];
	card dPort;
	//	char TLS_Enabled[10];
	//
	//	memset(TLS_Enabled, 0, sizeof(TLS_Enabled));
	memset(adr, 0, sizeof(adr));
	memset(port, 0, sizeof(port));
	dPort = 0;

	ret = appGet(appGprsIpRemote, adr, lenGprsIpRemote+1);   // Retrieve remote IP
	CHECK(ret>=0, lblKO);

	ret = appGet(appGprsPort, port, lenGprsPort+1);                // Retrieve port number
	CHECK(ret>=0, lblKO);

	if(*port) {
		ret = dec2num(&dPort, port, 0);
	}

	if(com.hTransCfg)
		TlvTree_Release(com.hTransCfg);

	com.hTransCfg = TlvTree_AddChild(com.hCfg, LL_TAG_TRANSPORT_LAYER_CONFIG,   // TAG Transport layer parameters
			NULL,  // VALUE (Null)
			0);    // LENGTH 0

	TlvTree_AddChildInteger(com.hTransCfg, LL_TRANSPORT_T_PROTOCOL, // TAG
			LL_TRANSPORT_V_TCPIP,   // VALUE
			LL_TRANSPORT_L_PROTOCOL);   // LENGTH 1 byte

	////-----------------  SSL  ------------------

	///Get the communication cipher

	switch (tlsSSL) {
	case 1:

		// SSL for Ethernet
		TlvTree_AddChildString(com.hTransCfg,
				LL_TCPIP_T_SSL_PROFILE,	                // TAG
				SSL_PROFILE_NAME);	                    // VALUE

		TlvTree_AddChildInteger(com.hTransCfg,
				LL_TCPIP_T_SSL_TCP_CONNECT_TIMEOUT,  	// TAG
				com.prm.Wifi.cTimeout/2,                            // VALUE
				LL_TCPIP_L_SSL_TCP_CONNECT_TIMEOUT);	// LENGTH 4

		break;
	default:
		break;
	}

	////------------------------------------------


	TlvTree_AddChildString(com.hTransCfg, LL_TCPIP_T_HOST_NAME, adr);
	TlvTree_AddChildInteger(com.hTransCfg, LL_TCPIP_T_PORT, dPort,
			LL_TCPIP_L_PORT);

	TlvTree_AddChildInteger(com.hTransCfg, LL_TCPIP_T_CONNECT_TIMEOUT,
			(unsigned int) com.prm.Wifi.cTimeout,
			LL_TCPIP_L_CONNECT_TIMEOUT);

	ret = LL_Configure(&com.prm.hdl, com.hCfg);
	CHECK(ret == LL_ERROR_OK, lblKO);

	ret = LL_Connect(com.prm.hdl);
	CHECK(ret == LL_ERROR_OK || ret == LL_ERROR_NETWORK_ALREADY_CONNECTED
			|| ret == LL_ERROR_ALREADY_CONNECTED, lblKO);

	com.prm.ll_conn_delay = TMT_Retrieve_Clock();

	ret = 1;
	goto lblEnd;
	lblKO:
	ret = -1;
	lblEnd:
	return (ret);
}

/** Send an HTTP Request using link layer
 * \return number of bytes sent to the host
 */
static int comSendIp(const byte * req, word usLen) {
	int ret = 0;

	ret = LL_Send(com.prm.hdl, usLen, req, LL_INFINITE);
	return ret;
}

/** Receive a response from host using link layer
 * \return number of bytes received from the host
 */
static int comRecvIp(byte * response, word usLen) {
	// Local variables
	// ***************
	int iRet, iLength=0, iNbrBytes;
	int loops = 0;

	// Timeout setting
	// ***************

	// Receiving first block
	// *********************
	do {
		iNbrBytes = LL_Receive(com.prm.hdl, usLen, response+iLength, 100);     // Check reception
		if (iNbrBytes != 0)
			break;
		if (loops > 20)
			break;
		loops++;                                                     // Bytes received
	} while (iNbrBytes == 0);

	iLength += iNbrBytes;
	iRet = LL_GetLastError(com.prm.hdl);
	//	CHECK (iRet==LL_ERROR_OK, lblEnd);
	if ((iNbrBytes != 0) && (iNbrBytes < usLen)){
		// Receiving next block until timeout (Inter block 500ms)
		// ======================================================
		while(1) {
			iNbrBytes = LL_Receive(com.prm.hdl, usLen-iLength, response+iLength, 600);
			iLength += iNbrBytes;
			iRet = LL_GetLastError(com.prm.hdl);
			//			CHECK ((iRet==LL_ERROR_OK) || (iRet==LL_ERROR_TIMEOUT), lblEnd);
			if (iNbrBytes==0)
				break;
		}
	}

	iRet = iLength;
	goto lblEnd;

	// Errors treatment
	// ****************

	lblEnd:
	return iRet;
}

/** Start IP Disconnection
 * \return 1 if disconnected succesfully, negative on error
 */
static int comHangStartIp(void) {
	int ret = 1;

	ret = LL_Disconnect(com.prm.hdl);
	ret = LL_Configure(&com.prm.hdl, NULL);
	CHECK(ret == LL_ERROR_OK, lblKO);
	com.prm.hdl = 0;
	return ret;

	lblKO:
	return -1;
}

/** Release all link layer configuration
 * \return 1 if stopped succesfully, negative on error
 */
static int comStopWifi(void) {
	int ret = 1;

	TlvTree_Release(com.hCfg);
	return ret;
}


void comWifi_Prepare(void){

	//	ret = comStartWifi();
	//	if (ret!=1){
	//		return;
	//	}
	//
	//	ret = comSetWifi();
	//	if (ret!=1){
	//		return;
	//	}
	//
	//	ret = comDialIp();
}

int comWifiConnect(tBuffer * req,tBuffer * rsp, word SSL) {
	int ret = 0, rspLen = 0;
	byte RespBuffer[4096];

	memset(RespBuffer, 0, sizeof(RespBuffer));

	fncDisplayData_Goal("","","Connecting...",500,0);
	ret = comStartWifi();
	if (ret!=1){
		return 0;
	}

	ret = comSetWifi();
	if (ret!=1){
		return 0;
	}

	ret = comDialIp(SSL);
	if (ret!=1){
		fncDisplayData_Goal("","","Connection FAILED!!!",500,0);
		return 0;
	}

	ret = LL_ClearSendBuffer(com.prm.hdl);
	CHECK(ret==LL_ERROR_OK, lblEnd);

	ret = LL_ClearReceiveBuffer(com.prm.hdl);
	CHECK(ret==LL_ERROR_OK, lblEnd);

	ret = comSendIp(bufPtr(req), bufLen(req));
	if (ret==0){
		return 0;
	}
	rspLen = comRecvIp(RespBuffer, bufDim(rsp));
	bufApp(rsp, RespBuffer, rspLen);

	lblEnd:

	comHangStartIp();
	comStopWifi();
	return rspLen;
}

