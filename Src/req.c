//Subversion reference
//$HeadURL: https://svn.ingenico.com/UTAPROJECTS/sea-partner/5W/trunk/ISO5WPH/log/req.c $
//$Id: req.c 1944 2009-04-28 10:40:17Z ajloreto $

#include <globals.h>
#include "iso8583.h"
#include "EMV_Support.h"
#include "GTL_Assert.h"

#define CHK CHECK(ret>=0,lblKO)
static char reqtyp = 'A';       //'A': authorization; 'S': settlement; 'P': parameters download(TMS)
static byte LocationType;       //location on where to get certain information?     // 'L' - log , 'T' = tra
static int thereis_F;
//static int disable_crdSeq;

static int getVal(tBuffer * val, word key) {
	int ret;
	byte buf[256];

	VERIFY(val);
	ret = mapGet(key, buf, 256);
	CHK;
	VERIFY(strlen((char *) buf) <= 256);
	bufReset(val);
	ret = bufApp(val, buf, bufLen(val));
	CHK;
	return ret;
	lblKO:
	return -1;
}

static int getExpDatVal(tBuffer * val, word key) {
	int ret/*,len,ctr*/;
	byte buf[256];
	byte bcd[2];
	//	char track2[lenTrk2 + 1];

	VERIFY(val);

	switch (LocationType) {
	//	case 'R':
	//		ret = mapGet(revTrk2, track2, sizeof(track2));
	//		CHK;
	//		memset(buf,0,sizeof(buf));
	//		len = strlen(track2);
	//		for (ctr = 0; ctr < len; ctr++) {
	//			if((track2[ctr] == '=') || (track2[ctr] == 'D')) {
	//				buf[0]=track2[ctr+1];
	//				buf[1]=track2[ctr+2];
	//				buf[2]=track2[ctr+3];
	//				buf[3]=track2[ctr+4];
	//				break;
	//			}
	//		}
	//		break;
	//	case 'L':
	//	case 'T':
	default:
		ret = mapGet(key, buf, 256);
		break;
	}
	CHK;

	VERIFY(strlen((char *) buf) <= 256);
	bufReset(val);

	hex2bin(bcd, (char *)buf, 0);

	ret = bufApp(val, bcd, sizeof(bcd));
	CHK;

	return ret;
	lblKO:
	return -1;
}


static int getPanVal(tBuffer * val) {
	int ret;
	byte buf[256];
	byte bcd[lenPan];

	VERIFY(val);
	ret = mapGet(traPan, buf, 256);
	CHK;
	VERIFY(strlen((char *) buf) <= 256);
	bufReset(val);

	ret = strlen((char *) buf);
	if(ret % 2 != 0)
		ret++;

	hex2bin(bcd, (char *)buf, ret/2);

	ret = bufApp(val, bcd, ret/2);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getPrcCod(tBuffer * val) {
	int ret;
	byte PrcCod[1 + lenPrcCod], bytetrnType[2 + 1];
	char PrcCodStr[1 + lenPrcCod*2];
	char trnType[2 + 1];

	VERIFY(val);

	memset(PrcCodStr, 0, sizeof(PrcCodStr));
	memset(trnType,0,sizeof(trnType));

	MAPGET(traRqsProcessingCode, PrcCodStr, lblKO);

	hex2bin(bytetrnType,PrcCodStr,1);
	strncpy(&trnType[0],"\x01",1);
	strncpy(&trnType[1],(char *)bytetrnType,1); //Set the transaction type from the processing code
	MAPPUTSTR(emvTrnTyp,trnType,lblKO);

	hex2bin(PrcCod, PrcCodStr, 0);

	ret = bufApp(val, PrcCod, lenPrcCod);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getAmt(tBuffer * val) {
	int ret;
	char amt[lenAmt + 1];
	char Tipamt[lenAmt + 1];
	char Cbamt[lenAmt + 1];
	byte bcd[lenAmtBcd];
	word Amtkey, TipAmtkey, CashBackAmtkey;

	VERIFY(val);

	memset(amt, 0, sizeof(amt));
	memset(Tipamt, 0, sizeof(Tipamt));
	memset(Cbamt, 0, sizeof(Cbamt));

	switch (LocationType) {
	//      case 'R':
	//          Amtkey = revAmt;
	//          TipAmtkey = revTipAmt;
	//          break;
	//      case 'L':
	//          Amtkey = logTraAmt;
	//          TipAmtkey = logTipAmt;
	//          break;
	//      case 'T':
	default:
		Amtkey = traAmt;
		TipAmtkey = traTipAmt;
		CashBackAmtkey = traCashbackAmt;
		break;
	}

	MAPGET(Amtkey, amt, lblKO);
	MAPGET(TipAmtkey, Tipamt, lblKO);
	MAPGET(CashBackAmtkey, Cbamt, lblKO);

	addStr(amt, amt, Tipamt);
	addStr(amt, amt, Cbamt);

	fmtPad(amt, -lenAmt, '0');
	hex2bin(bcd, amt, 0);

	ret = bufApp(val, bcd, lenAmtBcd);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getDatTim(tBuffer * val) {
	int ret;
	char datTim[lenDatTim + 1];
	byte bcd[(lenDatTim - 4) / 2];  //remove YY and cc of dateTime
	word key;

	VERIFY(val);

	switch (LocationType) {
	//      case 'L':
	//          key = logDatTim;
	//          break;
	case 'T':
	default:
		key = traDatTim;
		break;
	}
	ret = mapGet(key, datTim, sizeof(datTim));
	CHK;

	hex2bin(bcd, datTim + 2, 0);    //remove the first 2 and last 2 char in DateTime containing YY & cc

	ret = bufApp(val, bcd, sizeof(bcd));
	CHK;
	return ret;
	lblKO:
	return -1;
}

static int getTim(tBuffer * val) {
	int ret;
	char Tim[lenDatTim + 1];
	byte bcd[(lenDatTim - 8) / 2];  //remove YYMMDD and cc of dateTime
	word key;

	VERIFY(val);

	switch (LocationType) {
	//      case 'R':
	//          key = revDatTim;
	//          break;
	//      case 'L':
	//          key = logDatTim;
	//          break;
	case 'T':
	default:
		key = traDatTim;
		break;
	}
	ret = mapGet(key, Tim, sizeof(Tim));
	CHK;

	//	hex2bin(bcd, Tim + 6, 0);   //remove the first 6 and last 2 char in DateTime containing YYMMDD & cc
	hex2bin(bcd, Tim + 8, 0);   //remove the first 6 and last 2 char in DateTime containing YYMMDD & cc

	//ret = bufApp(val, "\x10\x56\x14", 3); CHK;
	ret = bufApp(val, bcd, sizeof(bcd));

	CHK;
	return ret;
	lblKO:
	return -1;
}

static int getDat(tBuffer * val) {
	int ret;
	char Dat[lenDatTim + 1];
	byte bcd[(lenDatTim - 10) / 2]; //remove YY and hhmmsscc of dateTime
	word key;

	VERIFY(val);

	switch (LocationType) {
	//      case 'R':
	//          key = revDatTim;
	//          break;
	//      case 'L':
	//          key = logDatTim;
	//          break;
	case 'T':
	default:
		key = traDatTim;
		break;
	}
	ret = mapGet(key, Dat, sizeof(Dat));
	CHK;

	hex2bin(bcd, Dat + 4, 2);   //remove the first 2 and last 8 char in DateTime containing YY & hhmmsscc

	//ret = bufApp(val, "\x12\x03", 3); CHK;
	ret = bufApp(val, bcd, sizeof(bcd));

	CHK;
	return ret;
	lblKO:
	return -1;
}

static int getAquiringInstitution(tBuffer * val) {
	int ret;
	char Dat[3+ 1];
	byte bcd[2];

	memset(Dat,0,sizeof(Dat));
	strcpy(Dat,"TZA"); //CU-Code="834"

	hex2bin(bcd, &Dat[0], 0);   //remove the first 2 and last 8 char in DateTime containing YY & hhmmsscc

	ret = bufApp(val, bcd, sizeof(bcd));
	CHK;

	CHK;
	return ret;
	lblKO:
	return -1;
}

static int getSTAN(tBuffer * val) {
	int ret;
	card STAN;
	char dec[lenSTAN + 1];
	byte bcd[lenSTAN/2];
	word key;

	VERIFY(val);

	switch (LocationType) {
	//      case 'R':
	//          key = revSTAN;
	//          break;
	//      case 'L':
	//          key = logSTAN;
	//          break;
	case 'T':
	default:
		key = appSTAN;
		break;
	}
	ret = mapGetCard(key, STAN);
	CHK;

	if (STAN == 0) {
		STAN = 1;
		mapPutCard(key, STAN);
	}

	num2dec(dec, STAN, lenSTAN);
	hex2bin(bcd, dec, 0);
	ret = bufApp(val, bcd, lenSTAN/2);
	CHK;

	//ret = bufApp(val, "\x00\x00\x01", 3); CHK;

	return ret;
	lblKO:
	return -1;
}

static int getCardSeq(tBuffer * val) {
	int ret;
	char crdSeq[lenCrdSeq + 1];
	byte bcd[lenCrdSeqBCD];

	ret = mapGet(traCrdSeq, crdSeq, sizeof(crdSeq));
	CHK;
	fmtPad(crdSeq, -lenCrdSeq, '0');
	hex2bin(bcd, crdSeq, 0);

	ret = bufApp(val, bcd, lenCrdSeqBCD);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getPosEntMod(tBuffer * val) {
	int ret;
	char entMod;
	char POSE[lenPOSE];
	byte PIN[1 + lenPinBlk];
	byte pinOpt = '2';
	char cardname[33];
	byte bcd[lenPOSEBCD], fallBack = 0, ClessMagMode = 0, ClessMagMode_Tra = 0;
	word key;

	VERIFY(val);
	memset(POSE, 0, sizeof(POSE));
	memset(cardname, 0, sizeof(cardname));

	switch (LocationType) {
	//      case 'R':
	//          key = revEntMod;
	//          break;
	//      case 'L':
	//          key = logEntMod;
	//          break;
	case 'T':
	default:
		key = traEntMod;
		break;
	}
	ret = mapGetByte(key, entMod);
	CHK;

	MAPGETBYTE(appFallback, fallBack, lblKO);

	switch (entMod) {
	case 'k':
	case 'K':
		strcat(POSE, "001");
		break;
	case 'c':
	case 'C':
		if (fallBack == 1) {
			strcat(POSE, "080");
		} else
			strcat(POSE, "005");
		break;
	case 'l':
	case 'L':
		MAPGETBYTE(traClessMagMode, ClessMagMode_Tra, lblKO);
		MAPGETBYTE(appClessMagMode, ClessMagMode, lblKO);
		if ((ClessMagMode == 1) || (ClessMagMode_Tra == 1)) {
			strcat(POSE, "091");
		} else {
			strcat(POSE, "007");
		}
		break;
	case 'm':
	case 'M':
		if (fallBack == 1) {
			strcat(POSE, "080");
		}else
			strcat(POSE, "090");
		break;
	default:
		strcat(POSE, "000");
		break;
	}

	//    ret = mapGetByte(issPinOpt, pinOpt);
	//    CHK;

	switch (entMod) {
	case 'k':
	case 'K':
		strcat(POSE, "2");
		break;
	default:

		MAPGET(appCardName, cardname, lblKO);
		if (strncmp(cardname,"UPI",3) == 0) {
			mapGet(traPinBlk, PIN, sizeof(PIN));
			if(PIN[0] != 0){ //Pin was entered
				strcat(POSE, "1");
			} else {//No PIN
				strcat(POSE, "2");
			}

		} else {
			switch (pinOpt) {
			case '0':                //No Pin capabiliy
				strcat(POSE, "2");
				break;
			case '2':                //PIN capable
				strcat(POSE, "1");
				break;
			case '1':                //Unspecified
			default:
				//strcat(POSE, "0");
				strcat(POSE, "1"); //PIN capable default
				break;
			}
		}
		break;
	}
	hex2bin(bcd, POSE, 0);

	ret = bufApp(val, bcd, lenPOSEBCD);
	//ret = bufApp(val, (byte *) "\x00\x20", 2);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getNII(tBuffer * val) {
	int ret;
	char Nii[lenNII + 5];
	byte bcdNii[lenNii];
	word key;

	VERIFY(val);

	memset(Nii, 0, sizeof(Nii));
	memset(bcdNii, 0, sizeof(bcdNii));

	key = appNII;

	ret = mapGet(key, Nii, sizeof(Nii));
	fmtPad(Nii, -(lenNII + 1), '0');
	hex2bin(bcdNii, Nii, 0);

	ret = bufApp(val, bcdNii, lenNii);
	CHK;

	//ret = bufApp(val, "\x02\x00\x64", 2); CHK;

	return ret;
	lblKO:
	return -1;
}

static int getPosCndCod(tBuffer * val) {
	int ret;
	byte POSC = 0;
	word txnType;
	char EntMod;
	char MENU[lenMnu + 1];
	card mnuItem = 0;

	memset(MENU, 0, sizeof(MENU));

	MAPGETBYTE(traEntMod, EntMod, lblKO);

	MAPGET(traMnuItm,MENU,lblKO);
	dec2num(&mnuItem, MENU, 0);

	switch (LocationType) {
	//      case 'L':
	//          MAPGETBYTE(logConCode, POSC, lblEnd);
	//          break;
	case 'T':                // Point of Service Condition Code as per ISO8583
	default:
		MAPGETBYTE(traTxnType, txnType, lblKO);
		switch (txnType) {
		case trtPreAut:
			POSC = 0x06;
			break;
		case trtRefund:
			POSC = 0x17;
			break;
		case trtVoid:
			POSC = 0x21;
			break;
		case trtSale:
		default:
			switch (EntMod) {
			case 'k':
			case 'K':
				POSC = 0x05;
				break;
			default:
				if (mnuItem == mnuPreaut) {
					POSC = 0x06;
				} else {
					POSC = 0;
				}
				break;
			}
			break;
		}
		MAPPUTBYTE(traConCode, POSC, lblEnd);
		break;
	}

	ret = bufApp(val, &POSC, 1);
	//ret = bufApp(val, (byte *) "\x00", 1);
	CHK;

	goto lblEnd;
	lblKO:
	return -1;
	lblEnd:
	return ret;
}

static int getTrack2(tBuffer * val) {
	int ret, ctr, len;
	char track2[lenTrk2 + 1];
	byte bcdTrack2[(lenTrk2 + 1) / 2];
	word key;
	int x = 0;

	memset(track2, 0, sizeof(track2));
	memset(bcdTrack2, 0, sizeof(bcdTrack2));

	thereis_F = 0;

	VERIFY(val);
	switch (LocationType) {
	//      case 'R':
	//          key = revTrk2;
	//          break;
	//      case 'L':
	//          key = logTrk2;
	//          break;
	default:
		key = traTrk2;
		break;
	}

	ret = mapGet(key, track2, sizeof(track2));
	CHK;

	len = strlen(track2);
	for (ctr = 0; ctr < len; ctr++) {
		if(track2[ctr] == '=') {
			track2[ctr] = 'D';
		}
		if(track2[ctr] == 'F') {
			thereis_F = 1;
			for (x = 1; x < (len-ctr); x++) {
				track2[ctr+x] = 0x0;
			}
			len = ctr + 1;
			break;
		} else if ((track2[ctr] >= 'A' && track2[ctr] <= 'Z') || (track2[ctr] >= 'a' && track2[ctr] <= 'z')){
			if(track2[ctr] != 'D') {
				for (x = 0; x < (len-ctr); x++) {
					track2[ctr+x] = 0x0;
				}
				len = ctr;
				break;
			}
		}
	}

	if((len % 2 != 0) && (!thereis_F)){
		thereis_F = 1;
		track2[len] = 'F'; //to remove error on MTIP transactions that dont need a zero at the end
		track2[len+1] = 0x0;
		len = len + 1;
	}


	//	ret = bufAppStr(val, track2);
	//	CHK;

	hex2bin(bcdTrack2,track2,0);
	len = len / 2;
	ret = bufApp(val, (byte *) bcdTrack2, len);
	CHK;

	ret = len;

	return ret;
	lblKO:
	return -1;
}

#ifdef __EMV__
static int isException(word key){
	int ret = 0;
	switch (key) {
	case emvTrnSeqCnt:
	case emvIFDSerNum:
	case emvTrnTyp:
		ret = 1;
		break;
	default:
		ret = 0;
		break;
	}
	return ret;
}

static int getICCData(tBuffer * val) {
	int ret = 0;
	word ctl[] = {
			tagTrnCurCod,
			tagPANSeq,
			tagAIP,
			tagDFNam,
			tagTVR,
			tagTrnDat,
			tagTrnTyp,
			tagAmtNum,
			tagAmtOthNum,
			tagTrmAvn,
			tagIAD,
			tagAccCntCod,
			tagIFDSerNum,
			tagIssTrnCrt,
			tagCID,
			tagTrmCap,
			tagCVMRes,
			tagTrmTyp,
			tagATC,
			tagUnpNum,
			tagTrnSeqCnt,
			tagTrmRskMng,
			tagTSI,

			// ----- VISA TAGS ----
			tagFormFactor,

			//New UPI Tag
			tagMrcPrcCry,

			0
	};
	word key,beg;
	word *ptr;
	byte tag[2 + 1];
	byte dat[999 + 1];
	byte temp[999 + 1];
	char EntMod;

	VERIFY(val);

	memset(dat,0,sizeof dat);
	PSQ_Give_Serial_Number((char *)(dat + 1));
	//	ret = mapGet(appTID, dat+1, 8);
	//	CHK;
	dat[0]='\x08';
	ret = mapPut(emvIFDSerNum, dat, 9);

	ptr = ctl;
	while(*ptr) {

		key = mapKeyTag(*ptr);

		//---------- Logic on cless and Contact -----------
		ret = mapGetByte(traEntMod, EntMod);
		switch (EntMod) {
		case 'C':
		case 'c':
			if (key == emvFormFactor) {
				key = 0;
			}
			break;
			//// Commented below since Paywave need this tag
			//		case 'L':
			//		case 'l':
			//			if (key == emvCVMRes) {
			//				key = 0;
			//			}
			//			break;
		}
		//--------------------------------------------------
		if(key == 0){
			ptr++;
			continue;
		}
		memset(tag, 0, sizeof(tag));
		memset(dat, 0, sizeof(dat));
		memset(temp, 0, sizeof(temp));

		beg = begKey(key);
		switch (beg) {
		case traBeg:
		case appBeg: //Local to the application
			if(isException(key)){
				ret = mapGet(key, dat, sizeof(dat));
				CHK;
			}else{
				ret = mapGet(key, temp, sizeof(temp));
				CHK;
				//Make sure it is divisible by two
				if((ret % 2) != 0)
					ret++;
				// repeat for case of number was ZERO
				if((ret % 2) != 0)
					ret++;

				// NOTE: all local vars are in HEX
				hex2bin(dat,(const char *) temp, ret/2);
			}
			break;
		default:// All others come from Kernel
			ret = mapGet(key, dat, sizeof(dat));
			CHK;
			break;
		}

		//mapGet(key,...,
		//append Tag Len Value
		tag[0] = *ptr >> 8;
		tag[1] = *ptr & 0xFF;
		if(*tag) {
			ret = bufApp(val, tag, 1);
			CHK;
		}
		ret = bufApp(val, tag + 1, 1);
		CHK;

		//        trcS("Tag: \n");
		//        trcBN(bufPtr(val), bufLen(val));
		//        trcS("\n");

		ret = bufApp(val, dat, 1);
		CHK;
		//        trcS("Tag Len: \n");
		//        trcBN(bufPtr(val), bufLen(val));
		//        trcS("\n");

		ret = bufApp(val, dat + 1, *dat);
		CHK;
		//        trcS("Tag Len Val: \n");
		//        trcBN(bufPtr(val), bufLen(val));
		//        trcS("\n");

		ptr++;
	}

	memset(isoField055, 0, sizeof(isoField055));
	bin2hex(isoField055, bufPtr(val),bufLen(val));

	return bufLen(val);
	lblKO:
	return -1;
}

#endif

static int getPIN(tBuffer * val) {
	int ret;
	byte PIN[1 + lenPinBlk];
	//	char tmp[2 * lenPIN + 1];

	VERIFY(val);

	ret = mapGet(traPinBlk, PIN, sizeof(PIN));
	CHK;

	//	CHECK(*PIN == lenPinBlk, lblKO);
	//	bin2hex(tmp, (byte *) PIN + 1, lenPinBlk);
	//	ret = bufApp(val, (byte *) tmp, lenPinBlk);
	//	CHK;
	ret = bufApp(val, PIN, lenPinBlk);
	CHK;

	return ret;
	lblKO:
	return -1;
}


static int getIso53(tBuffer * val) {
	char tempData[100 + 1];
	byte fieldData[100 + 1];
	int ret = 0;

	memset(tempData, 0, sizeof(tempData));
	memset(fieldData, 0, sizeof(fieldData));

	mapGet(appDUKPT_KSN, tempData, 21);
	fmtPad(tempData, 96, '0');

	hex2bin(fieldData, tempData, 48);

	bufReset(val);
	ret = bufApp(val, (byte *)fieldData, 48);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getAddAmt(tBuffer * val) {
	int ret;
	char TipAmt[lenAmt + 1];
	word key;
	card mnuitem;

	VERIFY(val);

	switch (LocationType) {
	//      case 'R':
	//          key = revTipAmt;
	//          break;
	//      case 'L':
	//          key = logTipAmt;
	//          break;
	case 'T':
	default:
		key = traTipAmt;
		break;
	}

	mnuitem = getCurrentMenu();
	if(mnuitem == mnuSaleCB)
		key = traCashbackAmt;

	ret = mapGet(key, TipAmt, sizeof(TipAmt));
	CHK;

	fmtPad(TipAmt, -lenAmt, '0');

	ret = bufApp(val, (byte *) TipAmt, lenAmt);
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getRoc(tBuffer * val) {
	int ret;
	char InvNum[lenInvNum + 1];
	word key;
	word Inv = 0;

	VERIFY(val);

	switch (LocationType) {
	//      case 'R':
	//          key = cvtRevInvNum;
	//          break;
	//      case 'L':
	//          key = cvtRptInvNum;
	//          break;
	case 'T':
	default:
		//          key = cvtInvNum;
		key = appSTAN;
		break;
	}
	ret = mapGetWord(key, Inv);
	CHK;

	num2dec(InvNum, Inv, 0);

	ret = bufApp(val, (byte *) InvNum, lenInvNum);
	CHK;

	//ret = bufAppStr(val, "000024");

	return ret;
	lblKO:
	return -1;
}


static int getFLD62(tBuffer * val) {
	int ret;
	card mnuItem;
	word key;
	char MTI[lenMti + 3];
	byte InvNum[12];
	card MTI_int = 0;
	card CardVal;

	memset(MTI, 0, sizeof(MTI));

	VERIFY(val);

	MAPGET(traMnuItm,MTI,lblKO);
	dec2num(&mnuItem, MTI, 0);

	memset(InvNum, 0, sizeof(InvNum));
	memset(MTI, 0, sizeof(MTI));

	switch (mnuItem) {
	//	case mnuCompletion:
	//		key = traReferenceNo;  //key = logROC;
	//		getVal(val, key);
	//		break;
	case mnuSettlement:
		getRoc(val);
		break;
	default:
		MAPGET(traRqsMTI, MTI, lblKO);

		dec2num(&MTI_int, MTI, 0);
		switch (MTI_int) {
		case 20200:
		case 20100:
		case 20220:
		case 20320:
			bufReset(val);
			key = appInvNum;

			ret = mapGetCard(key, CardVal);
			num2dec((char *)InvNum, CardVal, lenInvNum);

			ret = bufApp(val, InvNum, strlen((char *)InvNum));
			break;
		default:
			break;
		}

		break;
	}

	return ret;
	lblKO:
	return -1;
}



int getTotal(tBuffer * val) {
	int ret;
	char Temp[3 + 1];
	char ReconField63[100 + 1];
	char CurrencyNumeric[5 + 1];
	char Dr[lenAmt + 1], Cr[lenAmt + 1];
	char DrCount[lenAmt + 1], CrCount[lenAmt + 1];
	char DrRev[lenAmt + 1], CrRev[lenAmt + 1];
	char DrRevCount[lenAmt + 1], CrRevCount[lenAmt + 1];
	char Totals[lenAmt + 1];

	memset(Dr, 0, sizeof(Dr));
	memset(Cr, 0, sizeof(Cr));
	memset(DrRev, 0, sizeof(DrRev));
	memset(CrRev, 0, sizeof(CrRev));
	memset(Totals, 0, sizeof(Totals));
	memset(Temp, 0, sizeof(Temp));

	VERIFY(val);

	strcpy(CurrencyNumeric, "020");

	///Get Currency data
	MAPGET(traCurrencyNum, &CurrencyNumeric[3], lblKO);
	lblKO:;
	//Get totals
	logCalcTot(CurrencyNumeric, Dr,Cr,DrRev,CrRev,DrCount,CrCount,DrRevCount,CrRevCount,Totals);

	//Append total Length
	ret = bufApp(val,(byte *) "\x00\x60", 2);

	///// CREDITS---------------------------------------------------------------------------
	memset(ReconField63, 0, sizeof(ReconField63));
	Telium_Sprintf(ReconField63, "%03d", CrCount);

	fmtPad(Cr, -lenAmt, '0');
	strncat(ReconField63, Cr,lenAmt);

	ret = bufAppStr(val, ReconField63);

	///// CREDIT REVERSALS------------------------------------------------------------------
	memset(ReconField63, 0, sizeof(ReconField63));
	Telium_Sprintf(ReconField63, "%03d", CrRevCount);

	fmtPad(CrRev, -lenAmt, '0');
	strncat(ReconField63, CrRev,lenAmt);

	ret = bufAppStr(val, ReconField63);

	///// DEBITS---------------------------------------------------------------------------
	memset(ReconField63, 0, sizeof(ReconField63));
	Telium_Sprintf(ReconField63, "%03d", DrCount);

	fmtPad(Dr, -lenAmt, '0');
	strncat(ReconField63, Dr,lenAmt);

	ret = bufAppStr(val, ReconField63);

	///// DEBIT REVERSALS------------------------------------------------------------------
	memset(ReconField63, 0, sizeof(ReconField63));
	Telium_Sprintf(ReconField63, "%03d", CrCount);

	fmtPad(DrRev, -lenAmt, '0');
	strncat(ReconField63, DrRev,lenAmt);

	ret = bufAppStr(val, ReconField63);
	/////---------------------------------------------------------------------------------


	return ret;
}

static int getFLD63(tBuffer * val) {
	int ret;
	char additionalAmt[lenAmt + 3];
	char TempData[lenAmt + 3];
	char FullTemp[256];
	char temp[100];
	word key;
	card mnuItem;
	char EntMod,MTI[lenMti + 3];
	byte len[2];
	char tmp[5 + 1];
	card MTI_int = 0;
	byte PIN[1 + lenPinBlk];
	byte BinaryData[100];

	VERIFY(val);

	memset(additionalAmt, 0, sizeof(additionalAmt));
	memset(FullTemp, 0, sizeof(FullTemp));
	memset(TempData, 0, sizeof(TempData));
	memset(temp, 0, sizeof(temp));
	memset(MTI, 0, sizeof(MTI));
	memset(PIN, 0, sizeof(PIN));

	MAPGET(traMnuItm,TempData,lblKO);
	dec2num(&mnuItem, TempData, 0);

	if(mnuItem==mnuSettlement)
		goto lblSkip;

	memset(TempData, 0, sizeof(TempData));
	MAPGET(traCVV2,TempData,lblKO);
	ret = mapGetByte(traEntMod, EntMod);

	lblSkip:
	switch (mnuItem) {
	case mnuBillerInquiry:

		//SERIAL NUMBER
		memcpy(len,"\x00\x10",2);
		ret = bufApp(val, len,2);//Length

		memset(TempData, 0, sizeof(TempData));
		strcpy(TempData,"SN");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		PSQ_Give_Serial_Number(FullTemp);
		ret = bufApp(val, (byte *) FullTemp, strlen(FullTemp));//data
		///--------------------------
		///THIS IS PREFIXED
		memset(FullTemp, 0, sizeof(FullTemp));
		strcpy(FullTemp,"B2");

		memset(TempData, 0, sizeof(TempData));
		MAPGET(appBillerServiceProduct, TempData, lblKO);
		strcat(FullTemp,TempData);

		num2dec(tmp, strlen(FullTemp),4);
		hex2bin(len, tmp, 2);
		// memcpy(len,"\x00\x06",2);


		ret = bufApp(val, len,2);
		ret = bufApp(val, (byte *) FullTemp, atoi(tmp));
		CHK;

		///--------------------------
		/// BILLER NUMBER CODE
		strcpy(FullTemp,"C4");

		memset(TempData, 0, sizeof(TempData));
		MAPGET(appBillerPrefix, TempData, lblKO);
		strcat(FullTemp,TempData);

		memset(TempData, 0, sizeof(TempData));
		MAPGET(traBillerCode, TempData, lblKO);
		strcat(FullTemp,TempData);

		num2dec(tmp, strlen(FullTemp),4);
		hex2bin(len, tmp, 2);

		ret = bufApp(val, len,2);
		ret = bufApp(val, (byte *) FullTemp, atoi(tmp));
		CHK;
		///--------------------------

		memcpy(len,"\x00\x06",2);
		ret = bufApp(val, len,2);
		strcpy(TempData,"ET9300");
		ret = bufApp(val, (byte *) TempData, 6);
		CHK;

		goto lblNonFinancial;
		break;
	case mnuBiller:
	case mnuBillerPayment:

		//SERIAL NUMBER
		memcpy(len,"\x00\x10",2);
		ret = bufApp(val, len,2);//Length

		memset(TempData, 0, sizeof(TempData));
		strcpy(TempData,"SN");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		PSQ_Give_Serial_Number(FullTemp);
		ret = bufApp(val, (byte *) FullTemp, strlen(FullTemp));//data

		///--------------------------
		///THIS IS BILLER
		memset(FullTemp, 0, sizeof(FullTemp));
		strcpy(FullTemp,"B2");

		memset(TempData, 0, sizeof(TempData));
		MAPGET(appBillerServiceProduct, TempData, lblKO);
		strcat(FullTemp,TempData);

		num2dec(tmp, strlen(FullTemp),4);
		hex2bin(len, tmp, 2);
		// memcpy(len,"\x00\x06",2);


		ret = bufApp(val, len,2);
		ret = bufApp(val, (byte *) FullTemp, atoi(tmp));
		CHK;

		///--------------------------
		/// BILLER NUMBER CODE
		memset(FullTemp, 0, sizeof(FullTemp));
		strcpy(FullTemp,"C4");

		memset(temp, 0, sizeof(temp));
		memset(TempData, 0, sizeof(TempData));
		MAPGET(appBillerPrefix, TempData, lblKO);
		strcat(temp,TempData);
		strcat(FullTemp,TempData);

		memset(TempData, 0, sizeof(TempData));
		MAPGET(traBillerCode, TempData, lblKO);
		strcat(temp,TempData);
		strcat(FullTemp,TempData);

		num2dec(tmp, strlen(FullTemp),4);
		hex2bin(len, tmp, 2);

		ret = bufApp(val, len,2);
		ret = bufApp(val, (byte *) FullTemp, atoi(tmp));
		CHK;
		///--------------------------

		memcpy(len,"\x00\x06",2);
		ret = bufApp(val, len,2);
		strcpy(TempData,"ET9300");
		ret = bufApp(val, (byte *) TempData, 6);
		CHK;

		///------------  Currency Code  -------------
		memset(FullTemp, 0, sizeof(FullTemp));
		memset(temp, 0, sizeof(temp));
		memcpy(len,"\x00\x05",2);
		ret = bufApp(val, len,2);

		strcpy(FullTemp,"CC");
		ret = bufApp(val, (byte *) FullTemp, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		key = traCurrencyNum; //manage Key of different approaches exist
		ret = mapGet(key, temp, sizeof(temp));
		CHK;
		strcpy(FullTemp,temp);

		ret = bufApp(val, (byte *) FullTemp, 3);
		CHK;
		break;

	case mnuBillerAdvice:

		//SERIAL NUMBER
		memcpy(len,"\x00\x10",2);
		ret = bufApp(val, len,2);//Length

		memset(TempData, 0, sizeof(TempData));
		strcpy(TempData,"SN");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		PSQ_Give_Serial_Number(FullTemp);
		ret = bufApp(val, (byte *) FullTemp, strlen(FullTemp));//data

		///--------------------------
		///THIS IS BILLER
		memset(FullTemp, 0, sizeof(FullTemp));
		strcpy(FullTemp,"B2");

		memset(TempData, 0, sizeof(TempData));
		MAPGET(appBillerServiceProduct, TempData, lblKO);
		strcat(FullTemp,TempData);

		num2dec(tmp, strlen(FullTemp),4);
		hex2bin(len, tmp, 2);


		ret = bufApp(val, len,2);
		ret = bufApp(val, (byte *) FullTemp, atoi(tmp));
		CHK;

		///--------------------------
		/// BILLER NUMBER CODE
		strcpy(FullTemp,"C4");

		memset(TempData, 0, sizeof(TempData));
		MAPGET(appBillerPrefix, TempData, lblKO);
		strcat(FullTemp,TempData);

		memset(TempData, 0, sizeof(TempData));
		MAPGET(traBillerCode, TempData, lblKO);
		strcat(FullTemp,TempData);

		num2dec(tmp, strlen(FullTemp),4);
		hex2bin(len, tmp, 2);

		ret = bufApp(val, len,2);
		ret = bufApp(val, (byte *) FullTemp, atoi(tmp));
		CHK;
		///--------------------------

		memcpy(len,"\x00\x06",2);
		ret = bufApp(val, len,2);
		strcpy(TempData,"ET9300");
		ret = bufApp(val, (byte *) TempData, 6);
		CHK;

		///------------  Currency Code  -------------
		memset(FullTemp, 0, sizeof(FullTemp));
		memset(temp, 0, sizeof(temp));
		memcpy(len,"\x00\x05",2);
		ret = bufApp(val, len,2);

		strcpy(FullTemp,"CC");
		ret = bufApp(val, (byte *) FullTemp, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		key = traCurrencyNum; //manage Key of different approaches exist
		ret = mapGet(key, temp, sizeof(temp));
		CHK;
		strcpy(FullTemp,temp);

		ret = bufApp(val, (byte *) FullTemp, 3);
		CHK;

		///------------  Surcharge amount  -------------
		//		memset(FullTemp, 0, sizeof(FullTemp));
		//		memset(temp, 0, sizeof(temp));
		//		memcpy(len,"\x00\x05",2);
		//		ret = bufApp(val, len,2);
		//
		//		strcpy(FullTemp,"44");
		//		ret = bufApp(val, (byte *) FullTemp, 2);//indicator
		//
		//		memset(FullTemp, 0, sizeof(FullTemp));
		//		key = traSurchargeAmt; //manage Key of different approaches exist
		//		ret = mapGet(key, temp, sizeof(temp));
		//		CHK;
		//		strcpy(FullTemp,temp);
		//
		//		ret = bufApp(val, (byte *) FullTemp, ret);
		//		CHK;

		///--------------------------------------------------------

		mapGet(traPinBlk, PIN, sizeof(PIN));
		if(PIN[0] != 0){
			///------------  KSN for the DUKPT transactions  -------------
			memset(FullTemp, 0, sizeof(FullTemp));
			memset(temp, 0, sizeof(temp));
			memcpy(len,"\x00\x22",2);
			ret = bufApp(val, len,2);

			strcpy(FullTemp,"33");
			ret = bufApp(val, (byte *) FullTemp, 2);//indicator

			memset(FullTemp, 0, sizeof(FullTemp));
			key = appDUKPT_KSN; //manage Key of different approaches exist
			ret = mapGet(key, temp, sizeof(temp));
			CHK;
			fmtPad(temp, 20, '\x00');
			strcpy(FullTemp,temp);

			ret = bufApp(val, (byte *) FullTemp, 20);
			CHK;
		}

		goto lblNonFinancial;
		break;
	case mnuSettlement:
		getTotal(val);
		break;
	case mnuLogOn:
		memset(FullTemp, 0, sizeof(FullTemp));
		memset(temp, 0, sizeof(temp));
		memcpy(len,"\x00\x22",2);
		ret = bufApp(val, len,2);

		strcpy(FullTemp,"33");
		ret = bufApp(val, (byte *) FullTemp, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		key = appDUKPT_KSN; //manage Key of different approaches exist
		ret = mapGet(key, temp, sizeof(temp));
		CHK;
		fmtPad(temp, 20, '\x00');
		strcpy(FullTemp,temp);

		ret = bufApp(val, (byte *) FullTemp, lenAmt);
		CHK;
		break;
	case mnuWithdrawal:
		memcpy(len,"\x00\x14",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"41");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		key = traAmt; //manage Key of different approaches exist
		ret = mapGet(key, additionalAmt, sizeof(additionalAmt));
		CHK;
		fmtPad(additionalAmt, -lenAmt, '0');
		strcpy(TempData,additionalAmt);

		ret = bufApp(val, (byte *) TempData, lenAmt);
		CHK;
		break;
	case mnuPreaut:
		memcpy(len,"\x00\x06",2);
		ret = bufApp(val, len,2);
		strcpy(TempData,"ET9201");
		ret = bufApp(val, (byte *) TempData, 6);
		CHK;

		break;
	case mnuCompletion:
		memcpy(len,"\x00\x06",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"ET9202");
		ret = bufApp(val, (byte *) TempData, 6); //static data completion
		CHK;
		///--------------------------
		memcpy(len,"\x00\x08",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"IN");
		ret = bufApp(val, (byte *) TempData, 2);

		mapGet(traReferenceNo,TempData,6);
		ret = bufApp(val, (byte *) TempData, 6); //original invoice number

		///--------------------------
		memcpy(len,"\x00\x08",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"BN");
		ret = bufApp(val, (byte *) TempData, 2);

		mapGet(appCurBat,TempData,6);
		ret = bufApp(val, (byte *) TempData, 6); //original Batch number
		CHK;
		///--------------------------

		//BATCH NUMBER FOR ALL TRANSACTION
		memcpy(len,"\x00\x08",2);
		ret = bufApp(val, len,2);//Length

		strcpy(TempData,"37");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		mapGet(appCurBat,TempData,6);
		ret = bufApp(val, (byte *) TempData, 6);//data
		///--------------------------

		break;
	case mnuVoid:
		memcpy(len,"\x00\x06",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"ET9200"); //Extended Transaction Type
		ret = bufApp(val, (byte *) TempData, 6); //static data completion
		CHK;
		///--------------------------
		memcpy(len,"\x00\x05",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"OT");
		ret = bufApp(val, (byte *) TempData, 2); //Original PCODE
		CHK;

		memset(FullTemp, 0, sizeof(FullTemp));
		memset(temp, 0, sizeof(temp));
		key = traVoid63Data; //manage Key of different approaches exist
		ret = mapGet(key, temp, sizeof(temp));
		CHK;
		hex2bin(BinaryData, temp, 3);

		ret = bufApp(val, BinaryData, 3);  //data
		CHK;
		///--------------------------

		//BATCH NUMBER FOR ALL TRANSACTION
		memcpy(len,"\x00\x08",2);
		ret = bufApp(val, len,2);//Length

		strcpy(TempData,"37");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		//		mapGet(regBatNum,TempData,6);
		mapGet(appBatchNumber,TempData,6);
		ret = bufApp(val, (byte *) TempData, 6);//data
		///--------------------------


		memset(additionalAmt, 0, sizeof(additionalAmt));
		key = traCashbackAmt; //manage Key of different approaches exist
		ret = mapGet(key, additionalAmt, sizeof(additionalAmt));
		CHK;
		if (strlen(additionalAmt) > 100) {
			memcpy(len,"\x00\x14",2);
			ret = bufApp(val, len,2);

			strcpy(TempData,"41");
			ret = bufApp(val, (byte *) TempData, 2);//indicator
			fmtPad(additionalAmt, -lenAmt, '0');
			strcpy(TempData,additionalAmt);

			ret = bufApp(val, (byte *) TempData, lenAmt);
			CHK;
		}
		///--------------------------
		break;
	case mnuSaleCB:
		memcpy(len,"\x00\x14",2);
		ret = bufApp(val, len,2);

		strcpy(TempData,"41");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		memset(additionalAmt, 0, sizeof(additionalAmt));
		key = traCashbackAmt; //manage Key of different approaches exist
		ret = mapGet(key, additionalAmt, sizeof(additionalAmt));
		CHK;
		fmtPad(additionalAmt, -lenAmt, '0');
		strcpy(TempData,additionalAmt);

		ret = bufApp(val, (byte *) TempData, lenAmt);
		CHK;
		///--------------------------

		//SERIAL NUMBER
		memcpy(len,"\x00\x10",2);
		ret = bufApp(val, len,2);//Length

		memset(TempData, 0, sizeof(TempData));
		strcpy(TempData,"SN");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		PSQ_Give_Serial_Number(FullTemp);
		ret = bufApp(val, (byte *) FullTemp, strlen(FullTemp));//data
		///--------------------------

		break;
	default:
		//BATCH NUMBER FOR ALL TRANSACTION

		memcpy(len,"\x00\x10",2);
		ret = bufApp(val, len,2);//Length

		memset(TempData, 0, sizeof(TempData));
		strcpy(TempData,"SN");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		PSQ_Give_Serial_Number(FullTemp);
		ret = bufApp(val, (byte *) FullTemp, strlen(FullTemp));//data
		///--------------------------

		break;
	}

	switch (EntMod) {
	case 'k':
	case 'K':
		memcpy(len,"\x00\x08",2);
		ret = bufApp(val, len,2);
		ret = bufApp(val,(byte *)TempData,8);
		break;
	}
	//// GLOBAL SETTINGS
	///
	MAPGET(traRqsMTI, MTI, lblKO);

	dec2num(&MTI_int, MTI, 0);
	switch (MTI_int) {
	case 20200:
	case 20201:
	case 20100:
		//	case 20101:
		//	case 20120:
	case 20220:
	case 20221:
	case 20320:
	case 20321:
		///------------  Currency Code  -------------
		memset(FullTemp, 0, sizeof(FullTemp));
		memset(temp, 0, sizeof(temp));
		memcpy(len,"\x00\x05",2);
		ret = bufApp(val, len,2);

		strcpy(FullTemp,"CC");
		ret = bufApp(val, (byte *) FullTemp, 2);//indicator

		memset(FullTemp, 0, sizeof(FullTemp));
		key = traCurrencyNum; //manage Key of different approaches exist
		ret = mapGet(key, temp, sizeof(temp));
		CHK;
		strcpy(FullTemp,temp);

		ret = bufApp(val, (byte *) FullTemp, 3);
		CHK;

		mapGet(traPinBlk, PIN, sizeof(PIN));
		if(PIN[0] != 0){
			///------------  KSN for the DUKPT transactions  -------------
			memset(FullTemp, 0, sizeof(FullTemp));
			memset(temp, 0, sizeof(temp));
			memcpy(len,"\x00\x22",2);
			ret = bufApp(val, len,2);

			strcpy(FullTemp,"33");
			ret = bufApp(val, (byte *) FullTemp, 2);//indicator

			memset(FullTemp, 0, sizeof(FullTemp));
			key = appDUKPT_KSN; //manage Key of different approaches exist
			ret = mapGet(key, temp, sizeof(temp));
			CHK;
			fmtPad(temp, 20, '\x00');
			strcpy(FullTemp,temp);

			ret = bufApp(val, (byte *) FullTemp, 20);
			CHK;
		}

		//// ----------  BATCH NUMBER FOR ALL TRANSACTION  ------------
		memcpy(len,"\x00\x08",2);
		ret = bufApp(val, len,2);//Length

		memset(TempData, 0, sizeof(TempData));
		strcpy(TempData,"37");
		ret = bufApp(val, (byte *) TempData, 2);//indicator

		//		mapGet(regBatNum,TempData,6);
		memset(TempData, 0, sizeof(TempData));
		mapGet(appBatchNumber,TempData,6);
		ret = bufApp(val, (byte *) TempData, 6);//data

		//// ------------------------------------------------------------

		break;
	default:
		break;
	}

	lblNonFinancial:

	return ret;
	lblKO:
	return -1;
}


//Card Acceptor Name Location
static int getIso043(tBuffer * val) {
	int ret,ctr;
	char entMod;
	char Iso043Data[40+1];
	byte bcd[(sizeof(Iso043Data))/2];
	char track2[lenTrk2 + 1];
	char BIN[6 + 1];
	word key;
	byte tucIssTrnCrt[1 + lenIssTrnCrt];
	char tcBuf[64 + 1];

	VERIFY(val);
	memset(Iso043Data, 0x00, sizeof(Iso043Data));
	memset(BIN, 0x00, sizeof(BIN));
	memset(tcBuf,0x00,sizeof tcBuf);

	switch (LocationType) {
	//	case 'R':
	//		key = revEntMod;
	//		break;
	//	case 'L':
	//		key = logEntMod;
	//		break;
	//	case 'T':
	default:
		key = traEntMod;
		break;
	}

	ret = mapGet(key, track2, sizeof(track2));
	CHK;
	//Extract the BIN of the card
	for (ctr = 0; ctr < 6; ctr++) {
		BIN[ctr]=track2[ctr];
	}

	ret = mapGetByte(key, entMod);
	CHK;

	switch (entMod) {
	case 'm':
	case 'M':
	case 'k':
	case 'K':

		strcat(Iso043Data, "SV:");// location information exclusive of city, state and country(23char)
		strcat(Iso043Data, "KIGALI"); //city in which the Point-of-Service is located (13char)
		strcat(Iso043Data, "DA"); // state in which the Point-of-Service is located (2char)
		strcat(Iso043Data, "RW"); //country in which the Point-of-Service is located (2char)

		break;
	case 'c':
	case 'C':
		ret = mapGet(emvIssTrnCrt, tucIssTrnCrt, 1 + lenIssTrnCrt);
		VERIFY(tucIssTrnCrt[0] == 8);
		VERIFY(tucIssTrnCrt[0] == lenIssTrnCrt);
		bin2hex(tcBuf, &tucIssTrnCrt[1], tucIssTrnCrt[0]);  // Conversion to ascii
		VERIFY(strlen(tcBuf) == 16);

		strcat(Iso043Data, "SV:");// constant
		// strcat(Iso043Data, "1234567890123456"); //Visa load request signature "gENERATED BY THE ICC" (16char)
		strcat(Iso043Data, tcBuf); //Visa load request signature "gENERATED BY THE ICC" (16char)
		strcat(Iso043Data, BIN); // Visa load acquirer BIN (6char)
		strcat(Iso043Data, "DAR-ES-SALAAM"); //The city (positions 26 - 38) in which the Point-of-Service is located (13char)
		strcat(Iso043Data, "TZ"); // The country (positions 39 - 40) in which the Point-of-Service is located. (2char)

		break;
	default:
		break;
	}
	// DO STUFF here..!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	// ----------------------------------------------------------------

	hex2bin(bcd, Iso043Data, 0);
	ret = bufApp(val, (byte *) bcd, sizeof(Iso043Data));
	CHK;

	return ret;
	lblKO:
	return -1;
}

static int getIso8(tBuffer * val) {
	int ret;

	VERIFY(val);
	switch (reqtyp) {
	case 'P':
		ret = bufApp(val, (byte *) "\x00\x00\x05\x00", 4);
		CHK;
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
	lblKO:
	return -1;
}



static int getManaged47(void){
	word key = 0;
	byte TransType = 0;
	int ret;
	//	char trmCurrType[3+1];
	//	char buf[200];
	//	char TerminalID[lenTid + 1];
	//	char TerminalID2[lenTid + 1];
	//	char charExp[2];
	//	card exponent;

	//	memset(buf,0,sizeof buf);
	//	memset(TerminalID,0,sizeof TerminalID);
	//	memset(TerminalID2,0,sizeof TerminalID2);
	//	memset(charExp,0,sizeof charExp);
	//	memset(trmCurrType,0,sizeof trmCurrType);

	ret = mapGetByte(traTxnType,TransType);
	CHECK(ret > 0, lblKO);//Terminal ID(41)

	switch(TransType){
	//	case trtPosirisAdvise:
	//		MAPGETBYTE(appExpTemp,exponent,lblKO);
	//		if (exponent>2) {
	//			exponent = 2;
	//		}
	//		num2dec(charExp,exponent,1);
	//		MAPPUTSTR(traCurrExpIris, charExp, lblKO);
	//		key = traCurrExpIris;
	//
	//		break;
	//	case trtFailedTxn:
	//		key = rptHdrSiteAddr1;  // get terminal user
	//		break;
	//	case trtPosirisOn:
	//	case trtPosIris:
	//		//===check for dual currency to send two TIDs
	//		MAPGET(appDualCurrency,trmCurrType,lblKO);
	//		if(memcmp(trmCurrType,"00",1)==0)
	//			key = appTid;
	//		else{
	//			ret = mapGet(appTid, TerminalID, lenTid + 1);
	//			CHECK(ret >= 0, lblDBA);
	//			ret = mapGet(appTid3, TerminalID2, lenTid + 1);
	//			CHECK(ret >= 0, lblDBA);
	//			sprintf((char*)buf, "%s,%s",TerminalID,TerminalID2);  //join the two TIDs
	//			MAPPUTSTR(traDualTID, buf, lblKO);
	//			key = traDualTID;
	//		}
	//		break;
	default:
		//		key = traerrorinf;
		key = traRrn;
		break;
	}
	return key;

	//	lblDBA:
	//	goto lblKO;

	lblKO:
	return -1;
}

static int getCurrency(tBuffer * val) {
	int ret;
	char CCY[3 + 1];

	VERIFY(val);

	memset(CCY, 0, sizeof(CCY));

	ret = mapGet(traCurrencyNum, CCY, 3);

	CHK;

	ret = bufApp(val, (byte *)CCY, 3);
	CHK;

	return ret;
	lblKO:
	return -1;
}



static int getCurrencyStl(tBuffer * val) {
	int ret;
	char CCY[3 + 1];

	VERIFY(val);

	//	ret = mapGet(appCur, CCY,3);
	ret = mapGet(traCurrencyNum, CCY,3);
	CHK;

	ret = bufApp(val, (byte *)CCY, 3);
	CHK;

	return ret;
	lblKO:
	return -1;
}


static int getCurrencyStl_51(tBuffer * val) {
	int ret;
	char CCY[3 + 1];

	VERIFY(val);

	//	ret = mapGet(appCur, CCY,3);
	ret = mapGet(traCurrencyNum, CCY,3);
	CHK;

	ret = bufApp(val, (byte *)CCY, 3);
	CHK;

	return ret;
	lblKO:
	return -1;
}



static int getBatch(tBuffer * val) {
	int ret;
	char BatchNum[lenInvNum + 1];
	word key;

	VERIFY(val);

	switch (LocationType) {
	//      case 'R':
	//          key = revBatNum;
	//      case 'L':
	//          key = logBatNum;
	//          break;
	//      case 'T':
	default:
		key = appCurBat;
		key = appBatchNumber;
		break;
	}
	ret = mapGet(key, BatchNum, sizeof(BatchNum));
	CHK;

	ret = bufApp(val, (byte *) BatchNum, lenInvNum);
	CHK;

	//ret = bufAppStr(val, "000123");

	return ret;
	lblKO:
	return -1;
}

static int getIso60(tBuffer * val) {
	int ret = 0;
	card mnuItem = 0;
	byte DataFld60[256 + 1];
	char TempData[100];

	memset(TempData, 0, sizeof(TempData));
	MAPGET(traMnuItm,TempData,lblKO);
	dec2num(&mnuItem, TempData, 0);

	VERIFY(val);

	switch (mnuItem) {
	case mnuReversal:
	case mnuVoid:
		memset(DataFld60, 0, sizeof(DataFld60));
		//Length Attribute                n 3  2 '0015' - BCD length of data to follow
		strcpy((char *)DataFld60,"\x00\x22");
		//Original Data Elements
		mapGet(traRevVoidData,&DataFld60[2],10);

		ret = bufAppStr(val, DataFld60);
		break;
	case mnuLogOn:
		memset(DataFld60, 0, sizeof(DataFld60));
		//Length Attribute                n 3  2 '0015' - BCD length of data to follow
		strcpy((char *)DataFld60,"\x00\x15");
		//PIN Pad Id (PPID)               n 16 8 Current PIN Pad Id (serial number)
		mapGet(appTerminalSerial,&DataFld60[2],8);

		//eKTM(PPID)                      b 32 4 The most significant 32 bits of the PIN key.
		mapGet(appKeyPart, (char *)&DataFld60[10], 4);//	strncpy((char *)&DataFld60[10],"\x00\x00\x00\x00",4);

		//PIN Pad Software Version Number n 6  3 PIN pad software version number.
		strncpy((char *)&DataFld60[14],"01J",3);

		ret = bufAppStr(val, DataFld60);
		break;
	default:

		switch (reqtyp) {
		case 'S':
			getBatch(val);
			break;
		default:
			ret = -1;
			break;
		}

		break;
	}

	return ret;
	lblKO:
	return -1;
}

static int getFld(tBuffer * val, byte bit) {
	word key;

	VERIFY(val);
	bufReset(val);
	switch (bit) {
	case isoPan:
		return getPanVal(val);
	case isoPrcCod:
		return getPrcCod(val);
	case isoAmt:
		return getAmt(val);
	case isoDatTim:
		return getDatTim(val);
	case isoSTAN:
		return getSTAN(val);
	case isoTim:
		return getTim(val);
	case isoDat:
		return getDat(val);
	case iso019:
		return getAquiringInstitution(val);
	case isoDatExp:
		key = traExpDat;
		return getExpDatVal(val, key);
	case isoPosEntMod:
		return getPosEntMod(val);
	case isoCrdSeq:
		return getCardSeq(val);
	case isoNII:
		return getNII(val);
	case isoPosCndCod:
		return getPosCndCod(val);
	case isoTrk2:
		return getTrack2(val);
	case isoRrn:              //RRN - Reference Retrieval Number(37)
		key = traRrn;
		return getVal(val, key);
	case isoAutCod:			 //Authorization code(38)
		key = traAutCod;
		return getVal(val, key);
	case isoRspCod:		     //Response Code(39)
		key = traRspCod;
		return getVal(val, key);
	case isoTid:
		key = appTID;
		return getVal(val, key);
	case isoMid:
		key = appMID;         //change later
		return getVal(val, key);
	case iso043:				 //(43)
		return getIso043(val);
	case iso045: 				 //Track 1 data
		key = traTrk1;
		return getVal(val, key);
		break;
	case isoSpnsrId:  //field 47
		//		key = traerrorinf;
		key = getManaged47();
		return getVal(val, key);
		break;
	case isoPinMacKey:
		key = traPinBlk;
		return getVal(val, key);
		break;
	case isoCur: 				 //Currency Code(49)
		return getCurrency(val);
	case isoCurStl: 			 // Currency 2
		return getCurrencyStl(val);
	case iso051: 			 // Currency 2
		return getCurrencyStl_51(val);
	case isoPinDat:          //052  PIN
		return getPIN(val);
	case isoSecCtl:
		return getIso53(val);
	case isoAddAmt:          //054  Additional Amount
		return getAddAmt(val);
#ifdef __EMV__
	case isoEmvPds:          //055  ICC System related data
		return getICCData(val);
#endif
	case isoMaxBuf:          //008  maximum buffer to be received by application
		return getIso8(val);
	case isoBatNum:          //060  Reserved Private
		return getIso60(val);
	case isoRoc:             //062  ROC
		return getFLD62(val);
	case isoAddDat:          //063  Settlement Totals
		return getFLD63(val);
	default:
		break;
	}
	return -1;
}


int getLen_fmt(byte bit,int len){

	if(bit==35){
		len=len*2;
		if (thereis_F)
			len = len - 1;
		if(len>37)len=37;

		thereis_F = 0; //notify that 'f' was processed
	}else if(bit==2){
		len=len*2;
		if(len>19)len=19;
	}

	return len;
}


word getLen_(byte bit,int fmt){
	word returnData=0;

	switch (bit) {
	case 37: case 38: case 39: case 41:
	case 42: case 43: case 52: case 53:
		returnData=fmt;
		break;
	default:
		if(fmt % 2 != 0)
			fmt++;
		returnData= fmt / 2;
		break;
	}

	return returnData;
}


static int appFld(tBuffer * req, byte bit, const tBuffer * val) {
	int ret = 0;
	int fmt;
	char tmp[5 + 1];
	byte buf[1024];
	byte bcd[5 + 1];
	word len;
	byte cnt;

	memset(bcd, 0, sizeof(bcd));
	memset(buf, 0, sizeof(buf));
	memset(tmp, 0, sizeof(tmp));

	fmt = isoFmt(bit);

	if(fmt < 0) {               //LLVAR or LLLVAR
		len = bufLen(val);

		cnt = -fmt;
		VERIFY(cnt);

		len = getLen_fmt(bit,len);

		memcpy(buf, bufPtr(val), len);
		num2dec(tmp, len,(cnt - 1) * 2);
		hex2bin(bcd, tmp, 0);

		ret = bufApp(req, bcd, cnt - 1);

		if(bit==35 || bit==2){ // custom length of postilion
			if(len % 2 != 0) len++;
			len=len/2;
		}
	} else {
		len = getLen_(bit,fmt);///(kev-Code) Modification
		memcpy(buf, bufPtr(val), len);
	}

	ret = bufApp(req, buf, len);
	CHK;

	return bufLen(req);
	lblKO:
	return -1;
}

static int modifyBitmap(byte * BitMap) {
	char EntMod;
	char PIN[1 + lenPinBlk];
	byte isMagstripeMode = 0;
	byte isMagstripeMode_Tra = 0;
	int ret = 0;
	byte Autoreversal = 0;
	card MenuSelected = 0;
	char crdSeq[lenCrdSeq + 1];
	int KernelUsed = 0;

	VERIFY(BitMap);



	MenuSelected = getCurrentMenu();

	bitOff(BitMap, iso045);

	mapGetCard(traCardKernel, KernelUsed);

	mapGet(traPinBlk, PIN, sizeof(PIN));
	if(PIN[0] != 0){
		bitOn(BitMap, isoPinDat);
	}
	if(PIN[0] != 0){
		bitOn(BitMap, isoSecCtl);
	}


	mapGetByte(traEntMod, EntMod);
	switch (EntMod) {
	case 'k':
	case 'K':
		bitOff(BitMap, isoPinDat);
		bitOff(BitMap, isoTrk2);
		bitOn(BitMap, isoPan);
		bitOn(BitMap, isoDatExp);
		break;
	case 'l':
	case 'L':
		MAPGETBYTE(traClessMagMode, isMagstripeMode_Tra, lblKO);
		MAPGETBYTE(appClessMagMode, isMagstripeMode, lblKO);
		if ((isMagstripeMode == 1) || (isMagstripeMode_Tra == 1)) {
			bitOff(BitMap, isoEmvPds);
		} else {
			bitOn(BitMap, isoEmvPds);
		}
		bitOn(BitMap, iso045); ///Contains Track 1 data
		break;
	case 'm':
	case 'M':
		bitOn(BitMap, iso045); ///Contains Track 1 data
		break;
	case 'c':
	case 'C':
		bitOn(BitMap, isoEmvPds);
		break;
	}

	if (MenuSelected == mnuReversal) {
		mapGetByte(appAutoReversal, Autoreversal);
		if (Autoreversal == 1) {
			bitOff(BitMap, isoEmvPds);
			bitOff(BitMap, isoPinDat);
			bitOff(BitMap, isoSecCtl);
			Autoreversal = 0;
			mapPutByte(appAutoReversal, Autoreversal);
		}
	}

	////----------  Based on Kernel ---------
	switch (KernelUsed) {
	case DEFAULT_EP_KERNEL_PAYPASS:
		//. . . . .  Card sequence check . . . . . . . .
		memset(crdSeq, 0, sizeof(crdSeq));
		ret = mapGet(traCrdSeq, crdSeq, sizeof(crdSeq));
		CHK;
		fmtPad(crdSeq, -lenCrdSeq, '0');

		if (atoi(crdSeq) == 0) {
			bitOff(BitMap, isoCrdSeq);
		}
		//. . . . . . . . . . . . . . . . . . . . . . .

		break;
	default:
		bitOn(BitMap, isoCrdSeq);
		break;
	}

	//Bill payments dont need some fields
	if (MenuSelected == mnuBillerInquiry) {
		bitOff(BitMap, isoCrdSeq);
	}

	lblKO:
	return 1;
}

int reqBuild(tBuffer * req) {
	int ret;
	byte bit = 0, idx = 0;
	byte mti[1 + lenMti];
	byte Bitmap[1 + (lenBitmap*2)];
	char mtiStr[4 + lenMti];
	char BitmapStr[4 + (lenBitmap*4)];
	tBuffer buf;
	byte dat[999 + 1];
	card key;
	char keyStr[40];
	byte txnId = 0;
	card bitLen = 0;
	char bitLenStr[2 + 1];

	VERIFY(req);

	//    ret = mapGetByte(regLocType, LocationType);
	//    CHK;
	memset(dat, 0, sizeof(dat));
	memset(mti, 0, sizeof(mti));
	memset(keyStr, 0, sizeof(keyStr));
	memset(mtiStr, 0, sizeof(mtiStr));
	memset(Bitmap, 0, sizeof(Bitmap));
	memset(BitmapStr, 0, sizeof(BitmapStr));
	memset(bitLenStr, 0, sizeof(bitLenStr));

	MAPGET(traMnuItm, keyStr, lblKO);
	dec2num(&key, keyStr, 0);

	VERIFY(key);
	switch (key) {
	case mnuSettlement:
		reqtyp = 'S';
		break;
		//      case mnuSale:
		//          txnId = rqtSale;
		reqtyp = 'A';
		break;
	case mnuPreaut:
		//          txnId = rqtPreAuth;
		reqtyp = 'A';
		break;
	case mnuRefund:
		//          txnId = rqtRefund;
		reqtyp = 'A';
		break;
	case mnuVoid:
		//          txnId = rqtVoid;
		reqtyp = 'A';
		break;
	case mnuOffline:
		//          txnId = rqtOflSale;
		reqtyp = 'A';
		break;
	case mnuLogOn:
		//          txnId = rqtLogOn;
		reqtyp = 'A';
		break;
	case mnuReversal:
		//          txnId = rqtReversal;
		reqtyp = 'R';
		break;
	default:
		reqtyp = 'A';
		break;
	}
	if(key != mnuSettlement) {
		ret = mapPutByte(traTxnType, txnId);
		CHK;
	}
	ret = mapGetByte(traTxnType, idx);
	CHK;

	//get the MTI of the transaction
	MAPGET(traRqsMTI, mtiStr, lblKO);
	hex2bin(mti, mtiStr, 0);
	ret = bufApp(req, mti + 1, 2);

	memset(Bitmap, 0, sizeof(Bitmap));

	MAPGET(traRqsBitMap, BitmapStr, lblKO);
	hex2bin(Bitmap, BitmapStr, 0);

	ret = modifyBitmap(Bitmap + 1);
	CHK;

	//get length of the bitmap
	bin2hex(bitLenStr, Bitmap,1);
	hex2num(&bitLen, bitLenStr, 0);

	ret = bufApp(req, Bitmap + 1, bitLen);
	CHK;

	for (bit = 2; bit <= bitLen * lenBitmap; bit++) {
		if(!bitTest(Bitmap + 1, bit))
			continue;

		//clear buffer to hold data
		memset(dat, 0, sizeof(dat));
		bufInit(&buf, dat, sizeof(dat));   //initialize temporary buffer for current field

		//get field data
		ret = getFld(&buf, bit);
		CHK;                    //retrieve the value from the data base

		//append to request
		ret = appFld(req, bit, &buf);
		CHK;                    //append it to the iso message
	}

	return bufLen(req);
	lblKO:
	return -1;
}
