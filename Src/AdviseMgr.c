#include <globals.h>
#include "Sqlite.h"

//static char ProcCode[(lenPrcCod * 2) + 2];
//static char BitMap[(lenBitmap*4) + 2];
//static char MTI[lenMti + 2];


/**
 * To capture the Details on current transaction state
 */
static void AdviseManage_Init(void){
//	int ret = 0;
//
//	//------------------------Processing code
//	memset(ProcCode, 0, sizeof(ProcCode));
//	MAPGET(traRqsProcessingCode, ProcCode,lblKO);
//
//	//------------------------MTI
//	memset(MTI, 0, sizeof(MTI));
//	MAPGET(traRqsMTI, MTI,lblKO);
//
//	//------------------------MTI
//	memset(BitMap, 0, sizeof(BitMap));
//	MAPGET(traRqsBitMap, BitMap,lblKO);
//
//	lblKO:;
}


/**
 * To restore the status of the current transaction Selection
 */
static void AdviseManage_Complete(void){
//	int ret = 0;
//
//	//------------------------Processing code
//	memset(ProcCode, 0, sizeof(ProcCode));
//	MAPPUTSTR(traRqsProcessingCode, ProcCode,lblKO);
//
//	//------------------------MTI
//	memset(MTI, 0, sizeof(MTI));
//	MAPPUTSTR(traRqsMTI, MTI,lblKO);
//
//	//------------------------MTI
//	memset(BitMap, 0, sizeof(BitMap));
//	MAPPUTSTR(traRqsBitMap, BitMap,lblKO);
//
//	lblKO:;
}

///Send PosIris Advise after transaction
static void AdvisePosiris(char *RspCode){

}

void AdviseTransactionManager(void){
	int ret = 0;
	char responseCode[lenRspCod + 1];
	card mnuVal = 0;

	//clear variable content from memory
	memset(responseCode, 0,sizeof(responseCode));

	dec2num(&mnuVal, isoMnuItm,0);

	///Collect data from databases(sqlite and tra/app data)
	//-- rsp Code treatment
	MAPGET(traRspCod, responseCode, lblKO);
	if(strlen(responseCode)<2){ //if transaction was declined offline then code Z1 will have been populated in EMV structure
		strcpy(responseCode, "Z3");
		MAPPUTSTR(traRspCod, responseCode,lblKO);
	}

	////=============== INITIALIZE ADVISE ===========
	AdviseManage_Init();
	////=============================================


	///Manage Final Advice and Information Advice
	switch (mnuVal) {
	case mnuCompletion:
	case mnuSale: // Can either do Final Advise(Online approve) or Information Advise(Offline declined)
		if (strncmp(responseCode,"00",2) == 0) {        /// Final Advise

		} else if (strncmp(responseCode,"Z1",2) == 0) { /// Information Advise

		}
		break;
	default:
		break;
	}

	AdvisePosiris(responseCode);

	////=============== COMPLETE ADVISE ===========
	AdviseManage_Complete();
	////=============================================

	lblKO:;
}
