/*
 * DualSim.c
 *
 *  Created on: Oct 29, 2019
 *      Author: KEVSHAKE_HOME
 */
#include "globals.h"
#include "TlvTree.h"
#include "LinkLayer.h"
#include "gprs_lib.h"
#include "ExtraGPRS.h"

extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

#ifndef CR_ENTRY_OK
#define CR_ENTRY_OK				0		/*!< Successful entry*/
#endif

#ifndef CR_ENTRY_NOK
#define CR_ENTRY_NOK		    1		/*!< Correction (if authorised ) */
#endif

int CurrentSimSlot;

//static int comLogicResolve_Ip(char * srvrIP) {
//	int ret = 0;
//	char IP_Bits[2 + 1];
//	char ip_Part_A[3+1];
//	char ip_Part_B[3+1];
//	char ip_Part_C[3+1];
//	char ip_Part_D[3+1];
//	char ConstructedIP[15+1];
//	int skipped = 0;
//
//	char *array;
//
//	//	memset(fmtIP, 0x00, sizeof(fmtIP));
//	memset(IP_Bits, 0x00, sizeof(IP_Bits));
//
//	//------ Format the IP address
//	memset(ip_Part_A, 0x00, sizeof(ip_Part_A));
//	memset(ip_Part_B, 0x00, sizeof(ip_Part_B));
//	memset(ip_Part_C, 0x00, sizeof(ip_Part_C));
//	memset(ip_Part_D, 0x00, sizeof(ip_Part_D));
//	memset(ConstructedIP, 0x00, sizeof(ConstructedIP));
//	array = srvrIP; //copy to buffer
//
//	// TREAT EACH BIT WELL
//	//A
//	ret = fmtTok(ip_Part_A, array, "."); //extract  token
//	array += ret;                        //skip token extracted
//	array++;                             //skip separator
//
//	skipped = 0;
//	if (ip_Part_A[0] == '0') {
//		skipped = 1;
//		if (ip_Part_A[1] == '0') {
//			skipped = 1;
//		}
//	}else {
//		skipped = 0;
//	}
//	strcat(ConstructedIP, &ip_Part_A[skipped]);
//	strcat(ConstructedIP, ".");
//	//========================================================
//	//B
//	ret = fmtTok(ip_Part_B, array, "."); //extract  token
//	array += ret;                        //skip token extracted
//	array++;                             //skip separator
//
//	skipped = 0;
//	if (ip_Part_B[0] == '0') {
//		skipped = 1;
//		if (ip_Part_B[1] == '0') {
//			skipped = 1;
//		}
//	}else {
//		skipped = 0;
//	}
//
//	strcat(ConstructedIP, &ip_Part_B[skipped]);
//	strcat(ConstructedIP, ".");
//	//========================================================
//	//A
//	ret = fmtTok(ip_Part_C, array, "."); //extract  token
//	array += ret;                        //skip token extracted
//	array++;                             //skip separator
//
//	skipped = 0;
//	if (ip_Part_C[0] == '0') {
//		skipped = 1;
//		if (ip_Part_C[1] == '0') {
//			skipped = 1;
//		}
//	}else {
//		skipped = 0;
//	}
//	strcat(ConstructedIP, &ip_Part_C[skipped]);
//	strcat(ConstructedIP, ".");
//	//========================================================
//	//A
//	ret = fmtTok(ip_Part_D, array, "."); //extract  token
//	array += ret;                        //skip token extracted
//	array++;                             //skip separator
//
//
//	skipped = 0;
//	if (ip_Part_D[0] == '0') {
//		skipped = 1;
//		if (ip_Part_D[1] == '0') {
//			skipped = 1;
//		}
//	}else {
//		skipped = 0;
//	}
//
//	strcat(ConstructedIP, &ip_Part_D[skipped]);
//	//========================================================
//
//	strcpy(srvrIP, ConstructedIP);
//
//
//	return ret;
//}

static int DualSim_SwitchSIM(int slot){
	int ret = 0;
	int * simSlot = umalloc(2);
	//	Telium_File_t *localgprs = NULL;

	ret = gprs_is_dual_sim();
	//	if (ret == GPRS_OK) {

	//		localgprs = Telium_Stdperif(( char *)"DGPRS", NULL );

	ret = gprs_get_sim_slot(simSlot);
	CHECK(ret == CR_ENTRY_OK, lblKO);

	if (slot !=0 ) {
		CurrentSimSlot = slot-1;
		if (CurrentSimSlot != (*simSlot )) { //check if slot is same as what is already there
			//				gprs_stop();
			//				GPRS_Restart(localgprs, 0);
			ret = gprs_select_sim_slot(CurrentSimSlot);
		}
	} else {

		if (*simSlot == 0) {
			//				gprs_stop();
			//				GPRS_Restart(localgprs, 0);
			ret = gprs_select_sim_slot(1);
			CurrentSimSlot = 1;
		}else{
			//				gprs_stop();
			//				GPRS_Restart(localgprs, 0);
			ret = gprs_select_sim_slot(0);
			CurrentSimSlot = 0;
		}
		CHECK(ret == CR_ENTRY_OK, lblKO);
	}
	//	}

	goto lblEnd;

	lblKO:
	ret = -1;

	lblEnd:
	if(simSlot != NULL)
		ufree(simSlot);

	return ret;
}

int AutoSwitchSimSlots(void){/*
	T_EGPRS_GET_INFORMATION xInfo;
	int switchNow = 0, iRet = 0;
	Telium_File_t *hGprs = NULL;
	int SimCheckDone = 0;
	byte SwitchLoops = 0;
	char TLS_Enabled[10];

	memset(TLS_Enabled, 0, sizeof(TLS_Enabled));

	CommsGetChannel(TLS_Enabled);

	if (strncmp(TLS_Enabled, "G", 1) != 0){
		return 1;
	}

	//	if (fncTMSConnectionSession() == 0) { //check if the TMS is already doing something
	//		return 1;
	//	}

	fncWriteStatusOfConnection('1');//Notify TMS transaction is in session
	lblSimCheck:
	////---- check if the terminal network or sim card is inserted in the SIM slot ----
	hGprs = Telium_Stdperif((char*)"DGPRS", NULL);

	iRet = gprs_GetInformation(hGprs,&xInfo,sizeof(xInfo));
	CHECK(iRet==0, lblEnd);

	// GPRS check report
	// =================
	switch (xInfo.start_report) {
	case EGPRS_REPORT_SIM_NOT_PRESENT: //No sim card
		switchNow = 1;
		break;
	}
	if (xInfo.sim_status == EGPRS_SIM_NOT_INSERTED) {
		switchNow = 1;
	}
	if ( xInfo.radio_level < 2) {
		switchNow = 1;
	}

	if (!switchNow) {

		if ( xInfo.status_gprs == EGPRS_GPRS_DISCONNECTED) {
			TaskInitiateGPRS();
		} else if ( xInfo.network_connection == EGPRS_GSM_NETWORK_DISCONNECT) {
			TaskInitiateGPRS();
		} else if ( xInfo.status_gprs == EGPRS_GPRS_PDP_READY) {
			TaskInitiateGPRS();
		}else if ( xInfo.network_connection == EGPRS_GSM_NETWORK_NORMAL_CONNECT) {
			TaskInitiateGPRS();
		}

		//Check if port is down
		if (checkOlineServer() == 0) {
			switchNow = 1;
		}
	}

	//Check if the lapsed time is three minutes only to swap sim cards
	if (switchNow) {
		mapGetByte(appSwitchLoops, SwitchLoops);
		if (SwitchLoops == 3) {
			switchNow = 1;
			SwitchLoops = 0;
		}else{
			switchNow = 0;
		}
		SwitchLoops++;
		mapPutByte(appSwitchLoops, SwitchLoops);
	}

	//check if you need a switch
	if (switchNow) {
		GL_Dialog_Message(hGoal, NULL, "SIM Switching in Progress Please wait!!!", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);

		DualSim_SwitchSIM(0);
		tmrPause(3);

		//make sure GPRS is started
		//		ComGPRS_Prepare(); //Hangs the system
		TaskInitiateGPRS();

		GL_Dialog_Message(hGoal, NULL, "SIM Switching in Progress Please wait!!!", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);

		tmrPause(5);

		//After switching check again for network
		if ((SimCheckDone == 1) && (xInfo.sim_status != EGPRS_SIM_NOT_INSERTED)){
			goto lblEnd;
		}
		switchNow = 0;
		SimCheckDone = 1;
		goto lblSimCheck;
	} else {
		if ( xInfo.status_gprs == EGPRS_GPRS_DISCONNECTED) {
			TaskInitiateGPRS();
		} else if ( xInfo.network_connection == EGPRS_GSM_NETWORK_DISCONNECT) {
			TaskInitiateGPRS();
		}
	}

	lblEnd:
	//return the idle image just in case
	IdleImageDisplay();
	fncWriteStatusOfConnection('0');//Notify TMS transaction is in session*/
	return 1;
}


void ManualSwitchSimSlots(int SelectedSlot){

	//Swap the SIM card
	DualSim_SwitchSIM(SelectedSlot);

	//make sure GPRS is started
	ComGPRS_Prepare();

}
