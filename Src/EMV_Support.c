#include "EMV_Support.h"


/**  This function returns the emv key from DBA (record "tra" transaction stuff)
 *   corresponding to the emv tag. The emv key will be used to extract the TLV
 *   from DBA.
 * \return
 *    - >0 : emv key corresponding to the tag.
 *    -  0 : key not found.
 * \header str\\str.h
 * \source str\\str.c
 */
word mapKeyTag(card tag) {
	byte temp = 0;

	switch (tag) {
#ifdef __EMV__
	case tagEMV_AfterODA_Error:
	case tagPayWaveDeclinedByCard:
	case tagCTQ:
	case tagKernelTraceError:
		return traAlternateRsp;
	case tagTrmRskMng:
		return emvTrmRskMng;
	case tagAIDT:
		return emvAid;
	case tagAppPrfNam:
		return emvAppPrfNam;
	case tagTrnCurCod:
		return emvTrnCurCod;
	case tagAmtBin:
		return emvAmtBin;
	case tagTrnTyp:
		return emvTrnTyp;
	case tagAmtNum:
		return emvAmtNum;
	case tagAmtOthNum:
		return emvAmtOthNum;
	case tagAmtOthBin:
		return emvAmtOthBin;
	case tagTrnCurExp:
		return emvTrnCurExp;
	case tagTrnDat:
		return emvTrnDat;
		//      case tagAcqId:
		//          return emvAcqId;
	case tagTrmAvn:
		return emvTrmAvn;
	case tagMrcCatCod:
		return emvMrcCatCod;
	case tagAccCntCod:
		return emvTrmCntCod;
		//      case tagTrmFlrLim:
		//          return emvTrmFlrLim;
	case tagTrmId:
		return emvTrmId;
	case tagTrnTim:
		return emvTrnTim;
	case tagTrmCap:
		return emvTrmCap;
	case tagTrmTyp:
		return emvTrmTyp;
	case tagAddTrmCap:
		return emvAddTrmCap;
	case tagTrnSeqCnt:
		return emvTrnSeqCnt;
	case tagVlpSupInd:
		return emvVlpSupInd;
	case tagVlpTrmLim:
		return emvVlpTrmLim;
	case tagTSI:
		return emvTSI;
	case tagTVR:
		return emvTVR;
	case tagCVMRes:
		return emvCVMRes;
	case tagRspCod:
		return emvRspCod;
	case tagUnpNum:
		return emvUnpNum;
	case tagAutCod:
		return emvAutCod;
		/// ------------------------------------------------------
	case tagTCC:
		return emvTCC;
	case tagTACDft:
		return emvTACDft;
	case tagTACDen:
		return emvTACDen;
	case tagTACOnl:
		return emvTACOnl;
	case tagThrVal:
		return emvThrVal;
	case tagTarPer:
		return emvTarPer;
	case tagMaxTarPer:
		return emvMaxTarPer;
		/// ------------------------------------------------------
	case tagLstRspCod:
		return emvLstRspCod;
		//      case tagDftValDDOL:
		//          return emvDftValDDOL;
	case tagRSAKeyExp:
		return emvRSAKeyExp;
	case tagRSAKey:
		return emvRSAKey;
	case tagCVMOutRes:
		return emvCVMOutRes;
	case tagFrcOnl:
		return emvFrcOnl;
	case tagHotLst:
		return emvHotLst;
	case tagAmtPrv:
		return emvAmtPrv;
	case tagTrmDc1:
		return emvTrmDc1;
	case tagTrmDc2:
		return emvTrmDc2;
	case tagPAN:
		return emvPAN;
	case tagPANSeq:
		return emvPANSeq;
	case tagAIP:
		return emvAIP;
	case tagCapkIdx:
		return emvCapkIdx;
	case tagTrk2:
		return emvTrk2;
	case tagExpDat:
		return emvExpDat;
	case tagCVM:
		return emvCVM;
	case tagCrdAvn:
		return emvCrdAvn;
	case tagPaypassTrack2:
		return traPaypassTrk2_Mstripe;
	case tagVisaTrack2:
		return traVisaTrk2_Mstripe;
	case tagChdNam:
		return emvChdNam;
	case tagIAD:
		return emvIAD;
	case tagIssTrnCrt:
		return emvIssTrnCrt;
	case tagCID:
		return emvCID;
	case tagATC:
		return emvATC;
	case tagIssAutDta:
		return emvIssAutDta;
	case tagIssSc1:{
		temp = 1;
		mapPutByte(traIssScript1, temp);
		return emvIssSc1;
	}
	case tagIssSc2:{
		temp = 1;
		mapPutByte(traIssScript2, temp);
		return emvIssSc2;
	}
	case tagOnlSta:
		return emvOnlSta;
	case tagIFDSerNum:
		return emvIFDSerNum;
	case tagDFNam:
		return emvDFNam;
	case tagCDOL1:
		return emvDftValTDOL;
	case tagAppLbl:
		return emvAppPrfNam;
	case tagAuthRes:
		return emvAuthRes;
	case tagFormFactor:
		return emvFormFactor;

	case tagMrcPrcCry:
		return emvMrcPrcCry;
#endif
	default:
		break;
	}
	return 0;
}
