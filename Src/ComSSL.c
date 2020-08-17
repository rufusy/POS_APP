
#include <globals.h>
#include <SSL_.h>
#include <X509_.h>
#include "TlvTree.h"
#include "LinkLayer.h"
#include "ExtraGPRS.h"


enum eSta {
	autBeg,                     //engine states: authorisation
	autSendReq,
	autRecvRsp,
	autEnd
};

typedef struct sTleParam {
	byte tmrG;                  //global timeout
	byte tmrF;                  //frame timeout
	byte tmrC;                  //character timeout
	byte tryS;                  //number of send trial
	byte tryR;                  //number of receive trial
} tTleParam;


typedef struct sComChn {
	byte chn;                   // communication channel used
	byte chnRole;
	struct {
		LL_HANDLE hdl;
		char separator;
		unsigned long int conn_delay;   // 4 Second delay
		unsigned long int ll_conn_delay;
		unsigned int send_timeout;
		int ifconn;
		SSL_HANDLE hdlSsl;
		SSL_PROFILE_HANDLE hdlProfile;
		char SslProfName[PROFILE_NAME_SIZE + 1];
	} prm;
	TLV_TREE_NODE hCfg;
	TLV_TREE_NODE hPhyCfg;
	TLV_TREE_NODE hDLinkCfg;
	TLV_TREE_NODE hTransCfg;
} tComChn;
static tComChn com;



const char *parseStr_Local(char *dst, const char *src, int dim, char separator) {
	VERIFY(dst);
	VERIFY(src);

	while(*src) {
		if(*src == separator) {
			src++;              // skip separator
			break;
		}
		VERIFY(dim > 1);        // dim includes ending zero
		*dst++ = *src++;
		dim--;
	}

	*dst = 0;
	return src;
}


static int comStartSsl(void) {
	com.prm.hdlSsl = 0;
	com.prm.hdlProfile = 0;
	ssllib_open();
	return 1;
}


static int comHangStartSsl(void) {
	CHECK(com.prm.hdlSsl, lblKO);

	SSL_Disconnect(com.prm.hdlSsl);
	//CHECK(ret == 0, lblKO);

	SSL_Free(com.prm.hdlSsl);
	//CHECK(ret == 0, lblKO);

	SSL_UnloadProfile(com.prm.SslProfName);
	//CHECK(ret == SSL_PROFILE_EOK, lblKO);

	SSL_DeleteProfile(com.prm.SslProfName);
	//CHECK(ret == SSL_PROFILE_EOK, lblKO);

	com.prm.hdlSsl = 0;
	memset(com.prm.SslProfName, 0, sizeof(com.prm.SslProfName));
	return 1;

	lblKO:
	return -1;
}

static int comHangWaitSsl(void) {

	if(com.prm.hdlSsl) {
		SSL_Disconnect(com.prm.hdlSsl);
		//CHECK(ret == 0, lblKO);

		com.prm.hdlSsl = 0;
	}

	if(!strcmp(com.prm.SslProfName, "")) {
		SSL_UnloadProfile(com.prm.SslProfName);
		//CHECK(ret == SSL_PROFILE_EOK, lblKO);

		SSL_DeleteProfile(com.prm.SslProfName);
		//CHECK(ret == SSL_PROFILE_EOK, lblKO);
		memset(com.prm.SslProfName, 0, sizeof(com.prm.SslProfName));
	}
	return 1;
}



static int comStopSsl(void) {

	if(com.prm.hdlSsl) {
		SSL_Disconnect(com.prm.hdlSsl);
		//CHECK(ret == 0, lblKO);
		SSL_Free(com.prm.hdlSsl);
		//CHECK(ret == 0, lblKO);
		com.prm.hdlSsl = 0;
	}

	if(!strcmp(com.prm.SslProfName, "")) {
		SSL_UnloadProfile(com.prm.SslProfName);
		//CHECK(ret == SSL_PROFILE_EOK, lblKO);
		SSL_DeleteProfile(com.prm.SslProfName);
		//CHECK(ret == SSL_PROFILE_EOK, lblKO);
		memset(com.prm.SslProfName, 0, sizeof(com.prm.SslProfName));
	}
	return 1;
}


static int comSetSsl(const char *init) {
	int ret = 0;
	char ProfNum[2 + 1];

	char keyFile[64 + 1];
	char crtFile[64 + 1];
	char caFile[64 + 1];

	memset(keyFile, 0, sizeof(keyFile));
	memset(crtFile, 0, sizeof(crtFile));
	memset(caFile, 0, sizeof(caFile));

	VERIFY(init);

	com.prm.separator = '|';    //common for all types of chn
	init = parseStr_Local(ProfNum, init, sizeof(ProfNum), com.prm.separator);
	init = parseStr_Local(keyFile, init, sizeof(keyFile), com.prm.separator);
	init = parseStr_Local(crtFile, init, sizeof(crtFile), com.prm.separator);
	init = parseStr_Local(caFile, init, sizeof(caFile), com.prm.separator);

	if(*ProfNum) {
		Telium_Sprintf(com.prm.SslProfName, "BSEAPPTLS4%s", ProfNum);
	} else {                    //default
		memcpy(com.prm.SslProfName, SSL_PROFILE_NAME, strlen(SSL_PROFILE_NAME));
	}
	com.prm.hdlProfile = SSL_NewProfile(com.prm.SslProfName, &ret);
	if(com.prm.hdlProfile == NULL) {
		SSL_UnloadProfile(com.prm.SslProfName);
		SSL_DeleteProfile(com.prm.SslProfName);
		com.prm.hdlProfile = SSL_NewProfile(com.prm.SslProfName, &ret);
	}
	CHECK(com.prm.hdlProfile != NULL, lblLoad);

	ret = SSL_ProfileSetProtocol(com.prm.hdlProfile, TLSv1_2);
	//	ret = SSL_ProfileSetProtocol(com.prm.hdlProfile, TLSv1);
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	ret = SSL_ProfileSetCipher(com.prm.hdlProfile, SSL_RSA | SSL_DSS| SSL_DES| SSL_3DES | SSL_RC4 | SSL_RC2 |SSL_MD5 | SSL_SHA1 | SSL_AES | SSL_SHA256| SSL_SHA384| SSL_aECDSA | SSL_kEDH | SSL_kECDHE | SSL_RC2 , SSL_HIGH | SSL_NOT_EXP );
	//	ret = SSL_ProfileSetCipher(com.prm.hdlProfile, SSL_RSA | SSL_DSS| SSL_DES| SSL_3DES | SSL_RC4 | SSL_RC2 | SSL_AES | SSL_SHA256| SSL_SHA384| SSL_aECDSA | SSL_kEDH | SSL_kECDHE | SSL_RC2 , SSL_HIGH | SSL_NOT_EXP );
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	/* The created profile will use the following certificates in the terminal */
	ret = SSL_ProfileSetKeyFile(com.prm.hdlProfile, keyFile, FALSE);
	ret = SSL_ProfileSetCertificateFile(com.prm.hdlProfile, crtFile);

	ret = SSL_ProfileAddCertificateCA(com.prm.hdlProfile, caFile);
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	//Add other CA certificates if any
	while(*init) {
		memset(caFile, 0, sizeof(caFile));
		init = parseStr_Local(caFile, init, sizeof(caFile), com.prm.separator);
		if(*caFile) {
			ret = SSL_ProfileAddCertificateCA(com.prm.hdlProfile, caFile);
			CHECK(ret == SSL_PROFILE_EOK, lblKO);
		}
	}

	/* Save the profile.*/
	ret = SSL_SaveProfile(com.prm.hdlProfile);
	CHECK(ret == SSL_PROFILE_EOK, lblKO);

	lblLoad:
	com.prm.hdlProfile = SSL_LoadProfile(com.prm.SslProfName);
	CHECK(com.prm.hdlProfile != NULL, lblKO);

	ret = 1;
	goto lblEnd;

	lblKO:
	ret = -1;

	lblEnd:
	return ret;
}


static int comDialSsl(void) {
	int ret;
	int nError;
	char adr[100 + 1];
	char port[100];
	card dPort;

	memset(adr, 0, sizeof(adr));
	memset(port, 0, sizeof(port));
	dPort = 0;
	com.prm.separator = '|';    //common for all types of chn

	ret = appGet(appGprsIpRemote, adr, lenGprsIpRemote+1);   // Retrieve remote IP
	CHECK(ret>=0, lblKO);

	ret = appGet(appGprsPort, port, lenGprsPort+1);                // Retrieve port number
	CHECK(ret>=0, lblKO);

	if(*port) {
		ret = dec2num(&dPort, port, 0);
	}

	ret = SSL_New(&com.prm.hdlSsl, com.prm.hdlProfile);
	CHECK(ret == 0, lblKO);

	ret = SSL_Connect(com.prm.hdlSsl, adr, dPort, 1600);
	CHECK(ret == 0, lblKO);

	ret = 1;
	goto lblEnd;
	lblKO:
	ret = SSL_ProfileGetLastError( com.prm.hdlProfile, &nError );

	ret = -1;
	lblEnd:
	return ret;
}

int comSendSsl(byte b) {
	int ret;

	CHECK(com.prm.hdlSsl, lblKO);
	ret = SSL_Write(com.prm.hdlSsl, &b, 1, 300);
	CHECK(ret == 1, lblKO);

	return 1;

	lblKO:
	return -1;
}

static int comSendBufSsl(const byte * msg, word len) {
	int ret = -1;

	ret = SSL_Write(com.prm.hdlSsl, msg, len, 300);
	CHECK(ret == len, lblKO);

	ret = len;
	goto lblEnd;

	lblKO:

	lblEnd:

	return ret;
}


int comRecvSsl(byte * b, int dly) {
	int ret = 0;

	VERIFY(b);

	CHECK(com.prm.hdlSsl, lblKO);
	ret = SSL_Read(com.prm.hdlSsl, &b, 1, dly * 100);
	CHECK(ret >= 0, lblKO);

	return (ret);

	lblKO:
	return -1;
}

static int comRecvBufSsl(tBuffer * msg, const byte * trm, byte dly) {
	int ret = -1;
	word dim, idx;
	byte b = 0x00;
	char response[5120 + 1];

	memset(response, 0, sizeof(response));

	CHECK(com.prm.hdlSsl, lblKO);
	tmrStart(0, dly * 100);
	dim = 0;
	if(trm) {
		while(tmrGet(0)) {
			if(dim >= bufDim(msg))
				break;
			ret = SSL_Read(com.prm.hdlSsl, &b, 1, dly);
			if(ret != 1)
				continue;
			if(msg) {
				ret = bufSet(msg, b, 1);
				CHECK(ret >= 0, lblKO);
			}
			dim++;
			if(!trm)
				continue;
			idx = 0;
			while(trm[idx]) {   //is it a terminator character
				if(b == trm[idx])
					break;
				idx++;
			}
			if(b == trm[idx])
				break;
		}
		ret = dim;
	} else {

		// ret = SSL_Read(com.prm.hdlSsl, (void *) bufPtr(msg), (int) bufDim(msg)-1, dly * 100);
		ret = SSL_Read(com.prm.hdlSsl,  response,  sizeof(response)-1, dly * 100);
		bufApp(msg, (byte *)response, ret);

		if(ret > 0)
			msg->pos = (word) ret;
	}

	goto lblEnd;
	lblKO:
	ret = -1;
	lblEnd:
	tmrStop(0);
	return ret;
}


static void tleInit(tTleParam * par) {

	par->tmrG = 180;            // Global timeout
	par->tmrF = 35;             // Frame timeout
	par->tmrC = 3;              // Character timeout
	par->tryS = 3;              // Max sending retries
	par->tryR = 3;              // Max receiving retries
}

static int SendReq(tTleParam * par, byte sta, tBuffer * req) {
	int ret;

	VERIFY(par);

	ret = comSendBufSsl(bufPtr(req), bufLen(req)); //send the request
	CHECK(ret == bufLen(req), lblKO);

	ret = sta += 1;
	goto lblEnd;

	lblKO:
	return -1;

	lblEnd:
	return ret;
}

static int recvRsp(tTleParam * par, byte sta,tBuffer * rsp) {
	int ret,counter = 0/*,cntr_Exit = 0;
	card totaltLength = 0;
	byte dRsp2[1024];
	tBuffer bRsp2*/;

	VERIFY(par);

	counter = counter + 2;

	ret = comRecvBufSsl(rsp, NULL, par->tmrF);
	CHECK(ret > 0, lblKO);
	//	totaltLength = ret;

	//	fncDisplayData_Goal("","","Please Wait...",500,0);
	//
	//	while(ret>=0){
	//		bufDel(&bRsp2, 0, 0);
	//		memset(dRsp2, 0, sizeof(dRsp2));
	//		bufInit(&bRsp2, dRsp2, sizeof(dRsp2));
	//
	//		ret = comRecvBufSsl(&bRsp2, NULL,3);
	//		totaltLength = ret + totaltLength;
	//		if(ret>6){
	//			bufApp(rsp,bufPtr(&bRsp2),bufLen(&bRsp2));
	//
	//			counter = counter + 1;
	//		}else if(ret == 0)
	//			cntr_Exit = cntr_Exit + 1;
	//
	//		if (cntr_Exit > 5)
	//			break;
	//	}

	ret = sta + 1;

	goto lblEnd;

	lblKO:
	ret = -1;
	goto lblEnd;

	lblEnd:
	return ret;
}


static int sslAut(tTleParam * par, byte sta,tBuffer * req,tBuffer * rsp) {  //tle automate
	int ret = 0;

	VERIFY(par);

	while(sta < autEnd) {
		switch (sta) {
		case autSendReq:
			fncDisplayData_Goal(" ","","Sending...",200,0);
			ret = SendReq(par, sta, req);
			break;
		case autRecvRsp:
			fncDisplayData_Goal(" ","","Receiving...",200,0);

			ret = recvRsp(par, sta, rsp);
			break;
		default:
			break;
		}
		if(kbdKey() == kbdANN)
			sta = autEnd;
		sta = ret;

	}
	ret = 1;

	return ret;
}

static int sslHostDial(void) {
	int ret = 0;
	//	const char *ptr = "|file://flash/HOST/CLIENT_KEY.PEM|file://flash/HOST/CLIENT.CRT|file://flash/HOST/SERVER.CRT";
	//	const char *ptr = "|/SYSTEM/CLIENT_KEY.PEM|/SYSTEM/CLIENT.CRT|/SYSTEM/SERVER.CRT";
	const char *ptr = "|/HOST/CLIENT_KEY.PEM|/HOST/CLIENT.CRT|/HOST/SERVER.CRT|/HOST/SERVER_INT.CRT";

	ret = comStartSsl();
	CHECK(ret >= 0, lblKO);

	ret = comSetSsl(ptr);

	ret = comDialSsl();
	if(ret < 0) {
		switch (-ret) {
		//		case 0:
		//			inf = infConnConnected;
		//			break;
		//		case comBusy:
		//			inf = infConnBusy;
		//			break;
		//		case comNoDialTone:
		//			inf = infConnNoDialTone;
		//			break;
		//		case comNoCarrier:
		//			inf = infConnNoCarrier;
		//			break;
		//		case comTimeout:
		//			//inf = infConnTimeout;
		//			inf = infBlank;
		//			break;
		//		default:
		//			inf = infBlank;
		//			break;
		}
		CHECK(ret >= 0, lblKO);
	}

	ret = 1;
	goto lblEnd;

	lblKO:
	comStopSsl();
	ret = -1;

	lblEnd:
	tmrStop(0);
	tmrStop(3);

	return ret;
}


static int sslProcessConnect(void) {
	int ret;

	ret = sslHostDial();
	if(ret >= 0) {
		return ret;
	}

	CHECK(ret >= 0, lblKO);

	return ret;

	lblKO:
	return -1;
}


static int sslConnect(void) {
	int ret;
	byte num = 0;

	for (num = 0; num < 3; num++) {
		ret = sslProcessConnect();
		if(ret >= 0)
			break;
	}
	CHECK(ret >= 0, lblKO);     //

	goto lblEnd;

	lblKO:
	return -1;

	lblEnd:

	MAPPUTSTR(traTLS_PositionMarker, "2",lblKO);//Make sure TLS knows this is postion creating Connection
	return ret;
}

int sslProcessConnect_Check(void) {
	int ret;

	_clrscr();
	fncDisplayData_Goal("","","Please Wait...",500,0);

	ret = sslConnect();
	CHECK(ret >= 0, lblKO);

	comHangStartSsl();
	comHangWaitSsl();

	comStopSsl();

	return ret;
	lblKO:
	comHangStartSsl();
	comHangWaitSsl();

	comStopSsl();
	return -1;
}


void ComSSL_Prepare(void) {
	int ret = 0;

	MAPPUTSTR(traTLS_PositionMarker, "1",lblKO);//Make sure TLS knows this is postion creating Connection

	const char *ptr = "|/HOST/CLIENT_KEY.PEM|/HOST/CLIENT.CRT|/HOST/SERVER.CRT|/HOST/SERVER_INT.CRT";

	ret = comStartSsl();
	CHECK(ret >= 0, lblKO);

	ret = comSetSsl(ptr);

	lblKO:;

	//	sslConnect();
}


int ComSSL(tBuffer * req,tBuffer * rsp){
	int retVal = 0;
	int ret;
	tTleParam par;
	char TLS_PositionMarker[5];

	tleInit(&par);

	fncDisplayData_Goal("","","Connecting...",500,0);

	ret = sslConnect();
	CHECK(ret >= 0, lblKO);

	do {
		memset(TLS_PositionMarker, 0, sizeof(TLS_PositionMarker));
		mapGet(traTLS_PositionMarker, TLS_PositionMarker, mapDatLen(traTLS_PositionMarker));//Make sure TLS knows this is postion creating Connection
	} while (strncmp(TLS_PositionMarker, "2", 1) != 0 );

	ret = sslAut(&par, autSendReq, req, rsp);
	CHECK(ret >= 0, lblKO);

	retVal = bufLen(rsp);

	ret = comHangStartSsl();
	CHECK(ret >= 0, lblKO);

	ret = comHangWaitSsl();
	CHECK(ret >= 0, lblKO);

	ret = retVal;
	goto lblEnd;

	lblKO:
	ret = -1;

	lblEnd:
	comStopSsl();
	return ret;
}
