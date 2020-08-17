//Subversion reference
//$HeadURL: https://svn.ingenico.com/UTAPROJECTS/sea-partner/5W/trunk/ISO5WPH/log/rsp.c $
//$Id: rsp.c 1917 2009-04-22 13:24:29Z abarantsev $

#include <globals.h>
#include "iso8583.h"
#include "EMV_Support.h"
#include "GTL_Assert.h"

#define CHK CHECK(ret>=0,lblKO)
#define MOV(N) if(len<N) return -1; len-= N; rsp+= N
#define PARSE(F) ret = F(rsp, length); CHK; MOV(ret);
#ifndef MIN
#define MIN(x,y)  ( ((x) < (y)) ? (x) : (y) )
#endif

static int putVal(const byte * val, word key, word len) {
	int ret;

	VERIFY(val);
	ret = mapPut(key, val, len);
	CHK;
	return ret;
	lblKO:
	return -1;
}

static int parseICCData(const byte * val, word lenVal) {
	int ret = 0;
	card ctl = 0,ptr;
	word key,beg;
	byte tag[2];
	byte len[1];
	byte dat[300],dd[600],ddBin[200],tgBin[8];
	card lenDat;
	char tg[16];
	char temp[600 + 1];

	while(ctl < lenVal) {
		memset(dat,0,sizeof dat);
		memset(dd,0,sizeof dd);

		memcpy(tgBin,val + ctl,8);
		bin2hex(tg,tgBin,8);
		if(strncmp(&tg[1],"F",1)==0){//4bytes
			if (strncmp(&tg[2],"8",1)==0) { //6bytes
				memcpy(tag, val + ctl, 3);  //Tag len = 2
				ctl += 3;
			}else{
				memcpy(tag, val + ctl, 2);  //Tag len = 2
				ctl += 2;
			}
		} else { //two bytes
			memcpy(tag, val + ctl, 1);  //Tag len = 1
			ctl += 1; //tag
		}

		memcpy(len, val + ctl, 1);
		bin2hex((char*)dd,len,1); hex2bin(len,(char*)dd,1);
		bin2num(&lenDat, val + ctl, 1);
		ctl += 1; //len

		memcpy(dat, val + ctl, lenDat);
		bin2hex((char*)&dd[2],dat,lenDat); hex2bin(dat,(char*)&dd[2],lenDat);

		ctl += lenDat;

		ptr = (card)tag[0];
		key = mapKeyTag(ptr);
		hex2bin(ddBin,(char*)dd,lenDat+1);

		beg = begKey(key);
		switch (beg) {
		case traBeg:
		case appBeg: //Local to the application
			bin2hex(temp, ddBin, lenDat+1);
			ret = mapPut(key, temp, (lenDat+1)*2);
			CHK;
			break;
		default:// All others come from Kernel
			if(key > 0){
				ret = mapPut(key, ddBin, lenDat+1);
				CHK;
			}
			break;
		}
	}
	lblKO:
	return 1;
}

/** This function parse the field working key from the response buffer
 *  (FID V).
 * \param    pucRsp (I-) Response Buffer.
 * \param    usLen (I-) Length of the response.
 * \return
 *  This function has return value.
 *    - >=0 : Field done.
 *    -  <0 : Field failed.
 * \header log\\log.h
 * \source log\\rsp.c
 */
static int rspWorkingKey(const byte * pucRsp, word usLen) {
	//    enum { SIZE = 16 };         // Field size
	//    char tcWKey[SIZE + 1];
	//    byte tucDoubleWKey[SIZE];   // Convert to a double key
	//    byte ucLocMK;               // Master Key location
	//    byte ucLocPK;               // Pin Key location
	//    char tcSep[] = { cFS, cETX, 0 };    // Separator
	//    int ret;
	//
	//    VERIFY(pucRsp);
	//
	//    ret = fmtTok(0, (char *) pucRsp, tcSep);    // Count the fid "V" length
	//    CHECK(usLen >= ret, lblKO);
	//    CHECK(SIZE >= ret, lblKO);  // Erroneous message
	//    ret = fmtTok(tcWKey, (char *) pucRsp, tcSep);   // Now extract it
	//    VERIFY(ret <= SIZE);
	//    VERIFY(ret == (int) strlen(tcWKey));
	//
	//    usLen = (byte) ret;
	//    MAPGETBYTE(appMKeyLoc, ucLocMK, lblKO); // Retrieve Master Key location from DBA (record "app" application stuff)
	//    MAPGETBYTE(appPinKeyLoc, ucLocPK, lblKO);   // Retrieve Pin Key location from DBA (record "app" application stuff)
	//
	//    ret = cryStart('m');
	//    CHECK(ret >= 0, lblCry);
	//    hex2bin(tucDoubleWKey, tcWKey, SIZE);   // Convert to double key, DED using duplicate key gives single DES-1
	//    hex2bin(&tucDoubleWKey[8], tcWKey, SIZE);
	//    ret = cryLoadDKuDK(ucLocPK, ucLocMK, tucDoubleWKey, 'p');
	//    CHECK(ret >= 0, lblCry);    // Download Pin Key into locPK using Master Key located at locMK
	//
	//    ret = usLen;                // Return field size
	//    goto lblEnd;
	//
	//  lblCry:
	//  lblKO:                       // Non-classified low-level error
	//    ret = -1;
	//    goto lblEnd;
	//  lblEnd:
	//    cryStop('m');               // Stop in case if it was started; the return code is not checked intentionnally
	//    return ret;
	return 1;
}



word rspGetLen_(byte bit,int fmt){
	word returnData=0;

	if(bit==37 || bit==38  || bit== 39 || bit==41 || bit==42 || bit==43 || bit==53)
		returnData=fmt;
	else {
		if(fmt % 2 != 0)
			fmt++;
		returnData= fmt / 2;
	}

	return returnData;
}

int rspGetLen_fmt(byte bit,int len){
	if(bit==35 || bit==2 || bit==32){
		if(len % 2 != 0)
			len++;
		len = len / 2; //special case where the host sends back fld-35 or fld-2
	}
	return len;
}


static int putValStan(const byte * val, word key, word len) {
	int ret;
	char testData[30];

	memset(testData,0,sizeof(testData));

	VERIFY(val);
	//	ret = mapPut(key, val, len);
	//	CHK;

	bin2hex(testData,val,len);

	ret = mapPut(traSTAN, testData, len*2);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int ManageFld54(const byte * rsp, word len){
	int ret = 1;

	MAPPUTSTR(traOtherAmt,rsp + 2,lblKO);

	ret=1;
	goto lblEnd;

	lblKO:
	ret=-1;
	goto lblEnd;

	lblEnd:
	return ret;
}


/**
 * Manages responses on the field 62
 * @param rsp
 * @param len
 * @return
 */
int ManageFld62(const byte * val, word lenVal){
	int ret = 1;
	word mnuItem, ctl = 0;
	byte tag[2];
	byte len[2];
	byte dat[300];
	char tagLen[2];
	card tagLenDat;

	while(ctl < lenVal) {
		memset(tagLen,0,sizeof tagLen);
		memset(len,0,sizeof len);
		memcpy(len, val + ctl, 2);
		bin2hex(tagLen, len, 2);
		tagLenDat = atoi(tagLen) - 2;
		ctl += 2; //--------------------------len

		memset(tag,0,sizeof tag);
		memcpy(tag, val + ctl, 2);
		ctl += 2; //--------------------------tag

		memset(dat,0,sizeof dat);
		memcpy(dat, val + ctl, tagLenDat);
		ctl += tagLenDat; //------------------data

		//////================ save data respectively ============
	}

	MAPGETWORD(traMnuItm,mnuItem,lblKO);

	switch (mnuItem) {
	case mnuLogOn:
		//admMasterKey_Silent(rsp, len);
		break;
	default:
		ret=1;
		break;
	}

	goto lblEnd;
	lblKO:
	ret=-1;
	goto lblEnd;

	lblEnd:
	return ret;
}

static int ManageFld63(const byte * rsp, word rspLen){
	int ret = 1;
	word mnuItem, ctl = 0;
	char tag[2];
	byte len[2];
	byte dat[1000];
	card tagLenDat, tagVal;
	char tagLen[2];

	MAPGETWORD(traMnuItm,mnuItem,lblKO); ctl = 0;

	while(ctl < rspLen) {
		memset(len,0,sizeof len);
		memcpy(len, rsp + ctl, 2);
		bin2hex(tagLen, len, 2);
		tagLenDat = atoi(tagLen) - 2;
		ctl += 2; //--------------------------len

		memset(tag,0,sizeof tag);
		memcpy(tag, rsp + ctl, 2);
		dec2num(&tagVal, tag, 0);
		ctl += 2; //--------------------------tag

		memset(dat,0,sizeof dat);
		memcpy(dat, rsp + ctl, tagLenDat);
		ctl += tagLenDat; //------------------data

		//////================ save data respectively ============
		switch (tagVal) {
		case 22: //Alternate Response
			MAPPUTSTR(traAlternateRsp, dat, lblKO);
			break;
		default:
			if (strncmp(tag, "C7", 2) == 0) { //(C7)Alternate Response
				// ApplicationCurrencyFill((char *)dat);
				ApplicationCurrencyFillAuto((char *)dat);
			}
			break;

		}
	}

	goto lblEnd;

	lblKO:
	ret=-1;
	goto lblEnd;

	lblEnd:
	return ret;
}

static int getFld(const byte * rsp, byte bit) {
	int ret;
	int fmt;
	char amt[lenAmt + 1];
	card len;                   // length of Data Element
	byte cnt;
	byte lenhex[5];             //mapp: to check
//	byte emvRespCode[3];
	char MENU[lenMnu + 1];
	card mnuitem = 0;

	VERIFY(rsp);

	fmt = isoFmt(bit);
	cnt = 0;

	if(fmt < 0) {               //LLVAR or LLLVAR
		switch (-fmt) {
		case 1: case 2:
			cnt = 1; break;
		case 3: case 4:
			cnt = 2; break;
		case 5:
			cnt = 3; break;
		default:
			cnt = 0;
			break;
		}
		VERIFY(cnt);

		memset(lenhex, 0x00, sizeof(lenhex));
		bin2hex((char *) lenhex, rsp, cnt);
		dec2num(&len, (char *) lenhex, sizeof(lenhex));

		len = rspGetLen_fmt(bit,len); // special case for fields eg 35 and 2
	} else {
		len=rspGetLen_(bit,fmt);
	}

	rsp += cnt;
	//rsp += len;
	switch (bit) {
	case isoAmt:
		MAPGET(traMnuItm, MENU, lblKO);
		dec2num(&mnuitem, MENU,0);
		if (mnuitem == mnuBalanceEnquiry) {
			memset(amt, 0, sizeof(amt));
			bin2hex(amt, rsp, len);
			ret = putVal((byte *)amt, traAmt, len*2);
			CHK;

			ComputeTotAmt();
		}
		break;
	case isoSTAN:
		ret = putValStan(rsp, traSTAN, len);
		break;
	case isoTim:
		//ret = putTime(rsp);
		//CHK;
		break;
	case isoDat:
		//ret = putDate(rsp);
		//CHK;
		break;
	case isoRrn:
		ret = putVal(rsp, traRrn, len);
		break;
	case isoAutCod:
		ret = putVal(rsp, traAutCod, len);
		CHK;
		break;
	case isoRspCod:
		if(len > 2)
			len = 2;
		ret = putVal(rsp, traRspCod, len);
		CHK;
//		memcpy(&emvRespCode[0], rsp, len);
//		ret = putVal(emvRespCode, emvRspCod, len);
//		CHK;
		break;
	case isoSecCtl:
		ret = rspWorkingKey(rsp, len);
		CHK;
		break;
	case isoAddAmt:		//054 Additional Amounts
		ret = ManageFld54(rsp, len);
		CHK;
		break;
	case isoEmvPds:
		ret = parseICCData(rsp, len);
		CHK;
		break;
	case isoRoc:	//062 Additional Data on host
		ret = ManageFld62(rsp, len);
		CHK;
		break;
	case isoAddDat:		//063 ROC-Trace No
		ManageFld63(rsp, len);
		break;
	default:
		ret = len;
		break;
	}

	return cnt + len;
	lblKO:
	return ret;
}

/** This function parses all the fields and saves the
 *  data in application data base.
 * \param    rsp (I-) Response Buffer.
 * \param    len (I-) Length of the response.
 * \header log\\log.h
 * \source log\\rsp.c
 */
int rspParse(const byte * rsp, word len) {
	int ret,bitSize;
	byte BitMap[16];
	byte bit;

	VERIFY(rsp);

	MOV(2);                     //skip MTI

	memset(BitMap, 0, 16);
	memcpy(BitMap, rsp, 16);

	bit=1;
	if(!bitTest(BitMap, bit)){
		bitSize=8;
	}else{
		bitSize=16;
	}
	MOV(bitSize);               //move bitmap

	for (bit = 2; bit <= (bitSize * 8); bit++) {
		if(!bitTest(BitMap, bit))
			continue;

		ret = getFld(rsp, bit);
		CHK;

		MOV(ret);
	}

	return len;                 //return the length of the non-parsed tail; non-negative is OK; normally 1 (' ')
	lblKO:
	return ret;
}
