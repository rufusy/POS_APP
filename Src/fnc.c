
#include <globals.h>
#include "Sqlite.h"
#include "util_sq.h"
#include "math.h"

extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

#ifndef MAX_SND
#define MAX_SND  2048
#endif
#ifndef MAX_RSP
#define MAX_RSP  2048
#endif

static const char *TerminalConfiguration[] = {
		"Terminal ID",
		"Merchant ID",
		NULL
};

static const char *BillerConfiguration[] = {
		"Product Code Setup",
		"Biller Prefix Setup",
		"Customer Name ON-OFF",
		"Convenience Fee(%)",
		NULL
};

static const char *TerminalModeConfiguration[] = {
		"NORMAL MODE",
		"BILLER MODE",
		NULL
};

static const char *BillerRef__ON_OFF[] = {
		"ON",
		"OFF",
		NULL
};

static const char *UsersMenu[] = {
		"ADD USER",
		"EDIT USER",
		"DELETE USER",
		NULL
};

static const char *UsersList[] = {
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		"                                                   ",
		NULL
};


int fncAddToField_From_Str(word key, char *str_Cat_data,  char *Separator ) {
	int ret;
	char data[999 + 1];


	memset(data, 0, sizeof(data));
	MAPGET(key, data, lblKO);

	if(strncmp(&data[0], "\0", 1) != 0)
		strcat(data, Separator);

	strcat(data, (char *) str_Cat_data);

	MAPPUTSTR(key, data, lblKO);

	ret = strlen(data);

	goto lblEnd;

	lblKO:
	ret = -1;
	goto lblEnd;
	lblEnd:
	return ret;
}
//****************************************************************************
//                    void PromptParameters (void)
//  This function asks for the Terminal parameters.
//  This function has no parameters.
//  This function has no return value
//****************************************************************************

void fncPromptTerminalParameters(void) {
	// Local variables
	// ***************
	int i = 0, iRet = 0;
	ulong Result = 0;
	ulong EntryResult = 0;
	char tcMask[256];
	char dataCaptured[256+1];
	char dataOriginal[256+1];
	word selected_ID = 0;

	memset(dataOriginal, 0, sizeof(dataOriginal));
	memset(dataCaptured, 0, sizeof(dataCaptured));

	PARAM_AGAIN:
	// Get the menu listed and selection done
	// Open a list box of items
	// ************************
	Result = GL_Dialog_Menu(hGoal, "Select Terminal Parameter", TerminalConfiguration, Result, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((Result!=GL_KEY_CANCEL) && (Result!=GL_RESULT_INACTIVITY), lblEnd);

	selected_ID = ApplicationSelectCurrency(); //Make sure since this is a dual currency terminal the correct ID is selected
	if (selected_ID < 1) {
		goto lblCancelled;
	}

	switch (Result) {
	///-----------------------------------------------------------------------------------------------------
	case 0://TID

		mapGet(selected_ID, dataOriginal, lenTID);

		memset(tcMask, 0, sizeof(tcMask));
		for (i=0; i<lenTID; i++)
			strcat(tcMask, "/c");

		EntryResult = GL_Dialog_Text(hGoal, "Terminal ID", "Enter TID :", tcMask, dataCaptured, sizeof(dataCaptured), GL_TIME_MINUTE);
		CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblCancelled);  // Exit on cancel/timeout

		if (strcmp(dataOriginal, dataCaptured) != 0) {
			iRet = mapPut(selected_ID, dataCaptured, lenTID);                  // Save new user name
			CHECK(iRet>=0, lblDbaErr);
		}

		break;
		///-----------------------------------------------------------------------------------------------------
	case 1://MID

		if (selected_ID == appTID_1) {
			selected_ID = appMID_1; //Make sure since this is a dual currency terminal the correct ID is selected
		} else {
			selected_ID = appMID_2; //Make sure since this is a dual currency terminal the correct ID is selected
		}

		mapGet(selected_ID, dataOriginal, lenMid);

		memset(tcMask, 0, sizeof(tcMask));
		for (i=0; i<lenMid; i++)
			strcat(tcMask, "/c");

		EntryResult = GL_Dialog_Text(hGoal, "Merchant ID", "Enter MID :", tcMask, dataCaptured, sizeof(dataCaptured), GL_TIME_MINUTE);
		CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblCancelled);  // Exit on cancel/timeout

		if (strcmp(dataOriginal, dataCaptured) != 0) {
			iRet = mapPut(selected_ID, dataCaptured, lenMid);                  // Save new user name
			CHECK(iRet>=0, lblDbaErr);
		}

		break;
	default:
		break;
	}


	GL_Dialog_Message(hGoal, NULL, "Done.", GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
	goto PARAM_AGAIN;

	lblCancelled:
	GL_Dialog_Message(hGoal, NULL, "Entry Cancelled !!!", GL_ICON_INFORMATION, GL_BUTTON_ALL, 2*1000);
	goto lblEnd;

	// Errors treatment
	// ****************

	lblDbaErr:                                                                // Data base error
	GL_Dialog_Message(hGoal, NULL,  "Error in data Entry", GL_ICON_ERROR, GL_BUTTON_VALID, 2*1000);
	goto lblEnd;

	lblEnd:
	return;
}

static int fncGetCharPosition(char * SourceData, char c){
	int i,len, f=0;
	len=strlen(SourceData);

	for(i=0;i<len;i++) {
		if(SourceData[i]==c) {
			f=i;
		}
	}
	return f;
}

void fncRemoveDecimal(char * amount){
	int idxToDel = 0;

	idxToDel = fncGetCharPosition(amount, '.');
	memmove(&amount[idxToDel], &amount[idxToDel + 1], strlen(amount) - idxToDel);

}

int fncIsNumeric(char * isNum){
	int ret = 1;
	int i = 0;
	int length = 0;

	length = strlen (isNum);
	for (i=0;i<length; i++){
		if (!isdigit(isNum[i])) {
			ret = 0;
		}
	}

	return ret;
}


//****************************************************************************
//                    void PromptParameters (void)
//  This function asks for the Terminal parameters.
//  This function has no parameters.
//  This function has no return value
//****************************************************************************
void fncUserManager(void) {
	// Local variables
	// ***************
	int i = 0, iRet = 0;
	ulong Result = 0;
	ulong ResultProcedure = 0;
	ulong EntryResult = 0;
	char tcMask[256];
	char Statement[256+1];
	char QueryData[2048 + 1];
	char dataCaptured[256+1];
	char dataOriginal[256+1];
	int NumberOfUsers = 0;
	char *Raw_Record;
	char *User_Record_Array[100];
	char User_Names[100][100];
	char User_Password[100][100];
	char UserID[100][100];

	memset(dataOriginal, 0, sizeof(dataOriginal));
	memset(dataCaptured, 0, sizeof(dataCaptured));
	memset(Statement, 0, sizeof(Statement));
	memset(QueryData, 0, sizeof(QueryData));

	OpenPeripherals();

	// Get the menu listed and selection done
	// Open a list box of items
	// ************************

	NumberOfUsers = NumberOfUsers + 1;

	REMANAGE:
	///// ------------------- SELECT USER MANAGEMENT PROCEDURE ----------------------------
	ResultProcedure = GL_Dialog_Menu(hGoal, "MANAGE USERS", UsersMenu, ResultProcedure, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((ResultProcedure!=GL_KEY_CANCEL) && (ResultProcedure!=GL_RESULT_INACTIVITY), lblEnd);

	if ((ResultProcedure == 1) || (ResultProcedure == 2)) { //Here Search for the list of users to 'EDIT' or 'DELETE'
		//Create the Query
		Telium_Sprintf (Statement, "SELECT userName,password,id FROM Users;");

		NumberOfUsers = Sqlite_Run_Statement_MultiRecord_NoColumnName(Statement, QueryData);

		//// ----------- split the records into an array ---------------
		Raw_Record = strtok(QueryData,";");//record separator
		User_Record_Array[0] = Raw_Record;
		for (i = 1; i < NumberOfUsers; i++){
			if(Raw_Record == NULL){
				break;
			}
			Raw_Record = strtok(NULL,";");
			User_Record_Array[i] = Raw_Record;
		}

		if (NumberOfUsers > 0) {
			for (i = 0; i < NumberOfUsers; i++){
				if (i > sizeof(UsersList)) //Check if the list is bigger than the size allocated
					break;
				char *array=User_Record_Array[i];
				char Data1[512 + 1];

				//----------------- Treat User records ---------------------
				///Username
				memset(Data1, 0, sizeof(Data1));
				memset(User_Names[i], 0, sizeof(User_Names[i]));
				iRet = fmtTok(User_Names[i], array, ",");    //extract first token
				array+=iRet;//skip data
				array++;   //skip separator

				///Password
				memset(Data1, 0, sizeof(Data1));
				memset(User_Password[i], 0, sizeof(User_Password[i]));
				iRet = fmtTok(User_Password[i], array, ",");    //extract first token
				array+=iRet;//skip data
				array++;   //skip separator

				///id
				memset(UserID[i], 0, sizeof(UserID[i]));
				iRet = fmtTok(UserID[i], array, ",");    //extract first token
				array+=iRet;//skip data
				array++;   //skip separator
			}
		}

		for (i = 0; i < NumberOfUsers; i++){
			UsersList[i] = User_Names[i];
		}
		UsersList[i] = NULL;

		///// ------------------- SELECT USER PROCEDURE ----------------------------
		Result = GL_Dialog_Menu(hGoal, "Select User:", UsersList, Result, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
		CHECK((Result!=GL_KEY_CANCEL) && (Result!=GL_RESULT_INACTIVITY), lblEnd);


		memset(dataOriginal, 0, sizeof(dataOriginal));
		memset(dataCaptured, 0, sizeof(dataCaptured));

		strcpy(dataOriginal, User_Names[Result]);
		strcpy(dataCaptured, User_Password[Result]);


		switch (ResultProcedure) {
		///-----------------------------------------------------------------------------------------------------
		case 1:/// ============  EDITTING
			//--------- Enter the Username ----------
			memset(tcMask, 0, sizeof(tcMask));
			for (i=0; i<lenMid; i++)
				strcat(tcMask, "/c");

			EntryResult = GL_Dialog_Text(hGoal, "Edit User", "Alter USERNAME :", tcMask, dataOriginal, sizeof(dataOriginal), GL_TIME_MINUTE);
			CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblCancelled);  // Exit on cancel/timeout

			//--------- Enter the Password ----------
			memset(tcMask, 0, sizeof(tcMask));
			memset(dataCaptured, 0, sizeof(dataCaptured));
			for (i=0; i<lenMid; i++)
				strcat(tcMask, "/d");

			EntryResult = GL_Dialog_Password(hGoal, "Edit User", "Alter PASSWORD :", tcMask, dataCaptured, sizeof(dataCaptured), GL_TIME_MINUTE);
			CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblCancelled);  // Exit on cancel/timeout

			// ----- save the data now -------
			memset(Statement, 0, sizeof(Statement));
			//Create the Query
			Telium_Sprintf (Statement, "UPDATE Users SET userName = '%s', password = '%s' WHERE id = '%s';", dataOriginal, dataCaptured, UserID[Result]);
			//UPDATE parameters SET details = '%s' where paramName = '%s';
			iRet = Sqlite_Run_Statement(Statement, QueryData);
			CHECK(iRet > 0,lblDbaErr);

			memset(Statement, 0, sizeof(Statement));
			Telium_Sprintf (Statement, "User %s Updated Successfully!", dataOriginal);

			GL_Dialog_Message(hGoal, NULL, Statement, GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
			goto REMANAGE;

			break;
			///-----------------------------------------------------------------------------------------------------
		case 2:/// ============  DELETTING
			//--------- Enter the Username ----------
			Telium_Sprintf (Statement, "Delete User %s ?", dataOriginal);

			// Delete User confirmation
			// **************************
			iRet = GL_Dialog_Message(hGoal, Statement, "No=Cancel / Yes=Valid", GL_ICON_QUESTION, GL_BUTTON_VALID_CANCEL, 30*1000);
			CHECK((iRet!=GL_KEY_CANCEL) && (iRet!=GL_RESULT_INACTIVITY), lblCancelled);  // Cancel/Timeout

			if (iRet == GL_KEY_VALID) {

				// ----- save the data now -------
				memset(Statement, 0, sizeof(Statement));
				//Create the Query
				Telium_Sprintf (Statement, "DELETE FROM Users WHERE id = '%s';", UserID[Result]);
				//UPDATE parameters SET details = '%s' where paramName = '%s';
				iRet = Sqlite_Run_Statement(Statement, QueryData);
				CHECK(iRet > 0,lblDbaErr);

				memset(Statement, 0, sizeof(Statement));
				Telium_Sprintf (Statement, "User %s Deleted Successfully!", dataOriginal);
			}else{
				Telium_Sprintf (Statement, "User %s was NOT deleted", dataOriginal);
			}
			GL_Dialog_Message(hGoal, NULL, Statement, GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
			goto REMANAGE;

			break;
		default:
			break;
		}


	} else { ///// -----------  Create new user -----------
		//--------- Enter the Username ----------
		memset(tcMask, 0, sizeof(tcMask));
		memset(dataOriginal, 0, sizeof(dataOriginal));
		for (i=0; i<lenMid; i++)
			strcat(tcMask, "/c");

		EntryResult = GL_Dialog_Text(hGoal, "Create New User", "Enter USERNAME :", tcMask, dataOriginal, sizeof(dataOriginal), GL_TIME_MINUTE);
		CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblCancelled);  // Exit on cancel/timeout


		//--------- Enter the Password ----------
		memset(tcMask, 0, sizeof(tcMask));
		memset(dataCaptured, 0, sizeof(dataCaptured));
		for (i=0; i<lenMid; i++)
			strcat(tcMask, "/d");

		EntryResult = GL_Dialog_Password(hGoal, "Create New User", "Enter PASSWORD :", tcMask, dataCaptured, sizeof(dataCaptured), GL_TIME_MINUTE);
		CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblCancelled);  // Exit on cancel/timeout

		// ----- save the data now -------
		memset(Statement, 0, sizeof(Statement));
		//Create the Query
		Telium_Sprintf (Statement, "INSERT INTO Users (userName, password) VALUES ('%s', '%s');", dataOriginal, dataCaptured);

		iRet = Sqlite_Run_Statement(Statement, QueryData);
		CHECK(iRet > 0,lblDbaErr);

		memset(Statement, 0, sizeof(Statement));
		Telium_Sprintf (Statement, "User %s created Successfully!", dataOriginal);

		GL_Dialog_Message(hGoal, NULL, Statement, GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
	}


	GL_Dialog_Message(hGoal, NULL, "Done.", GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
	goto REMANAGE;

	lblCancelled:
	GL_Dialog_Message(hGoal, NULL, "Entry Cancelled !!!", GL_ICON_INFORMATION, GL_BUTTON_ALL, 2*1000);
	goto lblEnd;

	// Errors treatment
	// ****************

	lblDbaErr:                                                                // Data base error
	GL_Dialog_Message(hGoal, NULL,  "Error in data Entry", GL_ICON_ERROR, GL_BUTTON_VALID, 2*1000);
	goto lblEnd;

	lblEnd:

	ClosePeripherals();

	return;
}

void fncDigestConfigFile(char * FileDigestConfData){
	int ret = 0;
	char *array;
	char Data1[512 + 1];
	char Data2[512 + 1];

	////---------- Merchant Name ------------------
	array=strtok(FileDigestConfData,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appMerchantName,Data1, mapDatLen(appMerchantName));
	}else
		return;


	////---------- Outlet Number ------------------
	array=strtok(NULL,";");
	if(array!=NULL){	//
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		//		mapPut(traTrk2Context,Data1, mapDatLen(traTrk2Context));
	}else
		return;

	////---------- Address ------------------
	array=strtok(NULL,";");
	if(array!=NULL){		//
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appHeader1,Data1, mapDatLen(appHeader1));
	}else
		return;

	////---------- Location ------------------
	array=strtok(NULL,";");
	if(array!=NULL){		 //
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appHeader2,Data1, mapDatLen(appHeader2));
	}else
		return;

	////---------- phone ------------------
	array=strtok(NULL,";");
	if(array!=NULL){		 //
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appHeader3,Data1, mapDatLen(appGprsIpRemote));
	}else
		return;

	////---------- Postilion IP Port ------------------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		memset(Data2, 0, sizeof(Data2));
		ret = fmtTok(Data1, (char *) array, "|");    //HOST IP
		array += ret;                  //skip token extracted
		array++;                       //skip separator
		mapPut(appEthIpLocal,Data1, strlen(Data1));
		mapPut(appGprsIpRemote,Data1, strlen(Data1));

		ret = fmtTok(Data2, (char *) array, "|");    //HOST Port
		array += ret;                  //skip token extracted
		array++;                       //skip separator
		mapPut(appEthPort,Data2, strlen(Data2));
		mapPut(appGprsPort,Data2, strlen(Data2));
	}else
		return;

	//// --------- TMS HOST IP and Port ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		memset(Data2, 0, sizeof(Data2));
		ret = fmtTok(Data1, (char *) array, "|");    //TMS IP
		array += ret;                  //skip token extracted
		array++;                       //skip separator
		//		mapPut(traTrk2Context,Data1, mapDatLen(traTrk2Context));

		ret = fmtTok(Data2, (char *) array, "|");    //TMS Port
		array += ret;                  //skip token extracted
		array++;                       //skip separator
		//		mapPut(traTrk2Context,Data2, mapDatLen(traTrk2Context));
	}else
		return;

	//// --------- Tsync IP and port -------------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		memset(Data2, 0, sizeof(Data2));
		ret = fmtTok(Data1, (char *) array, "|");    //TMS IP
		array += ret;                  //skip token extracted
		array++;                       //skip separator
		//		mapPut(traTrk2Context,Data1, mapDatLen(traTrk2Context));

		ret = fmtTok(Data2, (char *) array, "|");    //TMS Port
		array += ret;                  //skip token extracted
		array++;                       //skip separator
		//		mapPut(traTrk2Context,Data2, mapDatLen(traTrk2Context));
	}else
		return;

	////---------- Merchant ID 1 ------------------
	array= strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appMID_1,Data1, mapDatLen(appMID_1));
	}else
		return;

	//// --------- Terminal Id 1 ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appTID_1,Data1, mapDatLen(appTID_1));
	}else
		return;

	//// --------- Currency Code 1 ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appCurrCodeAlpha1,Data1, mapDatLen(appCurrCodeAlpha1));
	}else
		return;

	//// --------- Currency 1 numeric value ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appCurrCodeNumerc1,Data1, mapDatLen(appCurrCodeNumerc1));
	}else
		return;

	//// --------- Currency 1 decimal Places ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appCurrExp1,Data1, mapDatLen(appCurrExp1));
	}else
		return;

	////---------- Merchant ID 2 ------------------
	array= strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appMID_2,Data1, mapDatLen(appMID_2));
	}else
		return;

	//// --------- Terminal id 2 ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appTID_2,Data1, mapDatLen(appTID_2));
	}else
		return;

	//// --------- Currency code 2 ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appCurrCodeAlpha2,Data1, mapDatLen(appCurrCodeAlpha2));
	}else
		return;

	//// --------- Currency 2 numeric value ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appCurrCodeNumerc2,Data1, mapDatLen(appCurrCodeNumerc2));
	}else
		return;

	//// --------- Currency 2 decimal places ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appCurrExp2,Data1, mapDatLen(appCurrExp2));
	}else
		return;

	//// --------- Admin password ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appAdminPass,Data1, mapDatLen(appAdminPass));
	}else
		return;

	//// --------- Merchant passsword ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPut(appMerchPass,Data1, mapDatLen(appMerchPass));
	}else
		return;

	//// --------- Receipt profile ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		//		mapPut(appGprsPort,Data1, mapDatLen(appGprsPort));
	}else
		return;

	//// --------- Transaction counter ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		//		mapPut(appGprsPort,Data1, mapDatLen(appGprsPort));
	}else
		return;

	//// --------- SSL and TLS usage ----------
	array=strtok(NULL,";");
	if(array!=NULL){
		memset(Data1, 0, sizeof(Data1));
		strcpy(Data1, array);
		mapPutByte(appCommSSL,Data1[0]);
	}else
		return;
}

int fncReadConfigFile(void){
	char tcDirReadConfFile[100];
	char FileReadConfData[2048+1];
	T_GL_HFILE hFile;
	int iRet = 0, iLen;

	memset(FileReadConfData, 0, sizeof(FileReadConfData));
	memset(tcDirReadConfFile, 0, sizeof(tcDirReadConfFile));

	memcpy(tcDirReadConfFile,"file://flash/HOST/CONFIG.txt",28);
	hFile = GL_File_Open(tcDirReadConfFile, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
	CHECK(hFile!=NULL, lblEnd);           // Open the file received
	// Eventually read the file and get parameters
	iLen = GL_File_GetSize(hFile);           // File length in bytes
	CHECK(iLen!=-1, lblEmptyFile);
	if (iLen > 2048) {                    // File too big?
		iLen = 2048;                      // File truncated
	}

	iRet = GL_File_Read(hFile, FileReadConfData, iLen); // Read data from file
	CHECK(iRet==iLen, lblEmptyFile);

	fncDigestConfigFile(FileReadConfData);

	lblEmptyFile:

	iRet = GL_File_Close(hFile);             // Close the file
	CHECK(iRet==GL_SUCCESS, lblKOFile);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOFile:
	iRet = -1;
	goto lblEnd;

	lblEnd:;
	return iRet;
}


int fncWriteStatusOfConnection(char Status_1_or_0){
	char tcDirFile[100];
	char FileStatusData[100+1];
	T_GL_HFILE hFile;
	int iRet = 0;

	memset(tcDirFile, 0, sizeof(tcDirFile));

	memcpy(tcDirFile,"file://flash/HOST/SESSION.txt",29);
	hFile = GL_File_Open(tcDirFile, GL_FILE_CREATE_ALWAYS, GL_FILE_ACCESS_READ_WRITE);
	CHECK(hFile!=NULL, lblEnd);           // Open the file received

	memset(FileStatusData, 0, sizeof(FileStatusData));
	FileStatusData[0] = Status_1_or_0;
	GL_File_Write(hFile, FileStatusData, 1); // Read data from file

	iRet = GL_File_Close(hFile);             // Close the file
	CHECK(iRet==GL_SUCCESS, lblKOFile);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOFile:
	iRet = -1;
	goto lblEnd;

	lblEnd:;
	return iRet;
}


int fncOverwriteConfigFile(char * FileName, char * FilewriteConfData, int iLen){
	char tcDirFile[100];
	T_GL_HFILE hFile;
	int iRet = 0;

	memset(tcDirFile, 0, sizeof(tcDirFile));

	memcpy(tcDirFile,"file://flash/HOST/",18);
	strcat(tcDirFile, FileName);
	hFile = GL_File_Open(tcDirFile, GL_FILE_CREATE_ALWAYS, GL_FILE_ACCESS_READ_WRITE);
	CHECK(hFile!=NULL, lblEnd);           // Open the file received

	//overwrite data
	GL_File_Write(hFile, FilewriteConfData, iLen);

	iRet = GL_File_Close(hFile);             // Close the file
	CHECK(iRet==GL_SUCCESS, lblKOFile);

	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOFile:
	iRet = -1;
	goto lblEnd;

	lblEnd:;
	return iRet;
}

static int CustomerMenuUsers(char * Username, char * Password){
	// Local variables
	// ***************
	int i = 0, iRet = 0;
	ulong Result = 0;
	char Statement[256+1];
	char QueryData[2048 + 1];
	int NumberOfUsers = 0;
	char *Raw_Record;
	char *User_Record_Array[100];
	char User_Names[100][100];
	char User_Password[100][100];
	char UserID[100][100];

	memset(Statement, 0, sizeof(Statement));
	memset(QueryData, 0, sizeof(QueryData));

	Telium_Sprintf (Statement, "SELECT userName,password,id FROM Users;");

	NumberOfUsers = Sqlite_Run_Statement_MultiRecord_NoColumnName(Statement, QueryData);

	//// ----------- split the records into an array ---------------
	Raw_Record = strtok(QueryData,";");//record separator
	User_Record_Array[0] = Raw_Record;
	for (i = 1; i < NumberOfUsers; i++){
		if(Raw_Record == NULL){
			break;
		}
		Raw_Record = strtok(NULL,";");
		User_Record_Array[i] = Raw_Record;
	}

	if (NumberOfUsers > 0) {
		for (i = 0; i < NumberOfUsers; i++){
			if (i > sizeof(UsersList)) //Check if the list is bigger than the size allocated
				break;
			char *array=User_Record_Array[i];
			char Data1[512 + 1];

			//----------------- Treat User records ---------------------
			///Username
			memset(Data1, 0, sizeof(Data1));
			memset(User_Names[i], 0, sizeof(User_Names[i]));
			iRet = fmtTok(User_Names[i], array, ",");    //extract first token
			array+=iRet;//skip data
			array++;   //skip separator

			///Password
			memset(Data1, 0, sizeof(Data1));
			memset(User_Password[i], 0, sizeof(User_Password[i]));
			iRet = fmtTok(User_Password[i], array, ",");    //extract first token
			array+=iRet;//skip data
			array++;   //skip separator

			///id
			memset(UserID[i], 0, sizeof(UserID[i]));
			iRet = fmtTok(UserID[i], array, ",");    //extract first token
			array+=iRet;//skip data
			array++;   //skip separator
		}
	}else{
		Beep(0x09,0x03,5,BEEP_ON|BEEP_WAIT|BEEP_OFF);
		Beep(0x08,0x03,5,BEEP_ON|BEEP_WAIT|BEEP_OFF);
		Beep(0x07,0x03,5,BEEP_ON|BEEP_WAIT|BEEP_OFF);
		GL_Dialog_Message(hGoal, NULL, "No Users found!!", GL_ICON_INFORMATION, GL_BUTTON_NONE, 3 * GL_TIME_SECOND);
		goto lblKO;
	}

	for (i = 0; i < NumberOfUsers; i++){
		UsersList[i] = User_Names[i];
	}
	UsersList[i] = NULL;

	///// ------------------- SELECT USER PROCEDURE ----------------------------
	Result = GL_Dialog_Menu(hGoal, "Select User:", UsersList, Result, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((Result!=GL_KEY_CANCEL) && (Result!=GL_RESULT_INACTIVITY), lblKO);

	strcpy(Username, User_Names[Result]);
	strcpy(Password, User_Password[Result]);

	return 1;

	lblKO:
	return 0;
}

/****
 *
 * @param SecurityLevel
 *  SecurityLevel:
 *  		1 = Customer menu
 *  		2 = Merchant menu
 *  		3 = Supervisor menu
 *  		4 = Agent menu
 *  		5 = Admin menu
 * @return
 */
int fncSecurityPassword(char * SecurityLevel){
	word usKey = 0;
	int ret = 0, i = 0;
	int lvlSecurity = 0;
	char User[lenName + 1];
	char tcPwd1[100 + 1];
	char tcPwd2[100 + 1];
	ulong EntryResult = 0;
	char tcMask[256];

	memset(User, 0, sizeof(User));
	memset(tcMask, 0, sizeof(tcMask));
	memset(tcPwd1, 0, sizeof(tcPwd1));
	memset(tcPwd2, 0, sizeof(tcPwd2));

	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<lenTID; i++)
		strcat(tcMask, "/d");

	lvlSecurity = atoi(SecurityLevel);
	MAPPUTSTR(appAppLoggedName, "----", lblKO);

	OpenPeripherals();

	switch (lvlSecurity) {
	case 2: //Merchant menu
		strcpy(User, "Merchant");
		usKey = appMerchPass;
		break;
	case 3: //Supervisor menu
		strcpy(User, "Supevisor");
		usKey = appSupervPass;
		break;
	case 4: //Agent menu
		strcpy(User, "Supevisor");
		usKey = appMerchPass;
		break;
	case 5: //Admin menu
		strcpy(User, "Admin");
		usKey = appAdminPass;
		break;
	case 1: //Customer menu
		ret = CustomerMenuUsers(User, tcPwd2);
		CHECK(ret > 0, lblKO);
		MAPPUTSTR(appAppLoggedName, User, lblKO);
		break;
	default:
		ret = 1;
		goto lblDone;
		break;
	}

	EntryResult = GL_Dialog_Password(hGoal, User, "Enter PASSWORD :", tcMask, tcPwd1, sizeof(tcPwd1), GL_TIME_MINUTE);
	CHECK((EntryResult!=GL_KEY_CANCEL) && (EntryResult!=GL_RESULT_INACTIVITY), lblBadPwd);  // Exit on cancel/timeout

	if (lvlSecurity != 1) { //Never get from database if user is customer level
		MAPGET(usKey, tcPwd2, lblBadPwd);
	}
	CHECK(strcmp(tcPwd1, tcPwd2) == 0, lblBadPwd);  // Compare the 2 passwords


	lblDone:
	return 1;

	lblKO:
	return 0;

	lblBadPwd:
	Beep(0x09,0x03,5,BEEP_ON|BEEP_WAIT|BEEP_OFF);
	Beep(0x08,0x03,5,BEEP_ON|BEEP_WAIT|BEEP_OFF);
	Beep(0x07,0x03,5,BEEP_ON|BEEP_WAIT|BEEP_OFF);
	GL_Dialog_Message(hGoal, NULL, "Invalid Password!!", GL_ICON_ERROR, GL_BUTTON_NONE, GL_TIME_SECOND);

	ClosePeripherals();

	return 0;
}


/** Set system date and time in format YYMMDDhhmmss.
 * \param       YYMMDDhhmmss (I) Buffer containing the new date and time
 * \return non-negative value if OK; negative otherwise
 */
int fncSetDateTime(const char *YYMMDDhhmmss) {
	Telium_Date_t Date;
	int ret;

	VERIFY(YYMMDDhhmmss);

	Telium_Read_date(&Date);

	Date.year[0] = YYMMDDhhmmss[0];
	Date.year[1] = YYMMDDhhmmss[1];
	Date.month[0] = YYMMDDhhmmss[2];
	Date.month[1] = YYMMDDhhmmss[3];
	Date.day[0] = YYMMDDhhmmss[4];
	Date.day[1] = YYMMDDhhmmss[5];
	Date.hour[0] = YYMMDDhhmmss[6];
	Date.hour[1] = YYMMDDhhmmss[7];
	Date.minute[0] = YYMMDDhhmmss[8];
	Date.minute[1] = YYMMDDhhmmss[9];
	Date.second[0] = YYMMDDhhmmss[10];
	Date.second[1] = YYMMDDhhmmss[11];

	ret = Telium_Ctrl_date(&Date);
	CHECK(ret == 0, lblKO);     // Returns 0 if function has been correctly executed or -1 if date is incoherent

	ret = Telium_Write_date(&Date);
	CHECK(ret == 0, lblKO);     // Returns 0 if function has been correctly executed or -1 if date is incoherent

	ret = 1;
	goto lblEnd;
	lblKO:
	ret = -1;
	lblEnd:
	return ret;
}


static const char *tzControlPanel[] = {
		"(YES)Show Control Panel",
		"(NO)Hide Control Panel",
		NULL
};

void fncShowControlPanel(void){
	int ret = 0;
	ulong Result = 0;

	Result = GL_Dialog_Menu(hGoal, "SHOW CONTROL PANEL", tzControlPanel, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);

	switch (Result) {
	case 0:
		MAPPUTWORD(appShowControlPanel, 1, lblKO);
		break; // Show the C-PANEL
	case 1:
	default:
		MAPPUTWORD(appShowControlPanel, 0, lblKO);
		break; // Hide the C-PANEL
	}

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Done...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 100);

}
static const char *tzSimSlotSelect[] = {
		"Slot 1",
		"Slot 2",
		NULL
};

void fncSwitchSimSlot(void){
	ulong Result = 0;

	Result = GL_Dialog_Menu(hGoal, "SELECT SIM SLOT", tzSimSlotSelect, 0, GL_BUTTON_ALL, GL_KEY_0, GL_TIME_MINUTE);

	GL_Dialog_Message(hGoal, NULL, "Please Wait...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 100);

	switch (Result) {
	case 0:
		ManualSwitchSimSlots(1);
		break; // Show the C-PANEL
	case 1:
	default:
		ManualSwitchSimSlots(2);
		break; // Hide the C-PANEL
	}

	GL_Dialog_Message(hGoal, NULL, "Done...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 100);

}

void fncAutoSettlementChecker(void){
	int ret = 0;
	Telium_Date_t xDate;
	char tcDate[lenDatTim + 1];
	char tcTime[lenDatTim + 1];
	char db_Date[lenDatTim + 1];
	int DoSettlement = 0;
	char menu[lenMnu + 1];

	memset(menu, 0, sizeof(menu));
	memset(tcDate, 0, sizeof(tcDate));
	memset(tcTime, 0, sizeof(tcTime));
	memset(db_Date, 0, sizeof(db_Date));

	Telium_Read_date (&xDate);
	Telium_Sprintf (tcDate, "%2.2s%2.2s20%2.2s", xDate.day, xDate.month, xDate.year); // Retrieve date

	Telium_Sprintf (tcTime, "%2.2s%2.2s", xDate.hour, xDate.minute);        // Retrieve time


	// Periodical function in progress
	// *******************************
	mapGet(appLastSettlementDate, db_Date, mapDatLen(appLastSettlementDate));

	if (strncmp(db_Date, tcDate, 8) != 0){
		DoSettlement = 1;
	} else {
		DoSettlement = 0;
	}


	//If settlement is to be done DO IT
	if (DoSettlement) {
		clrscr();

		fncWriteStatusOfConnection('1');//Notify TMS transaction is in session
		//Print Traces and logs
		PrintDetailedLog();
		PrintSummaryLog();

		//Do Settlement
		num2dec(menu, mnuSettlement, 0);
		MAPPUTSTR(traMnuItm, menu, lblKO);

		MAPPUTSTR(traRqsBitMap, "082020030000C00016",lblKO);
		MAPPUTSTR(traRqsMTI, "020500",lblKO);
		MAPPUTSTR(traRqsProcessingCode, "920000",lblKO);
		logSettlement();
		logReset();
		logIncrementBatch();

		mapPutStr(appLastSettlementDate, tcDate);

		///if Time is set also do a wake up
		fncWriteStatusOfConnection('0');//Notify TMS transaction is in session
	}

	lblKO:;
}


void MemoryManagerClearGarbage(void){
	//	FS_GarbageCollection(FS_WO_ZONE_DATA);
}

void fncBillerInquiry(void){
	int ret = 0;
	int IsRef_ON = 0;
	char Menu[lenMnu];
	char DataSave[1024];
	card key = mnuBillerInquiry;

	num2dec(Menu, key, 0);
	MAPPUTSTR(traMnuItm, Menu, lblKO);

	//Biller Account
	ret = Application_Request_Data("Enter order Num# :", traBillerCode, 13, "/d");
	CHECK(ret>0, lblKO);

	memset(DataSave, 0, sizeof(DataSave));
	MAPPUTSTR(traBillerPaymentDetails, DataSave, lblKO); //Clear that Field

	fncAddToField_From_Str(traBillerPaymentDetails, "Bill Ref No.: ", "");
	MAPGET(appBillerPrefix, DataSave, lblKO);
	fncAddToField_From_Str(traBillerPaymentDetails, DataSave, "");

	memset(DataSave, 0, sizeof(DataSave));
	MAPGET(traBillerCode, DataSave, lblKO);
	fncAddToField_From_Str(traBillerPaymentDetails, DataSave, "");

	mapGetCard(appIsRef_ON, IsRef_ON);

	if (IsRef_ON) {
		//Customer Name
		ret = Application_Request_Data("Enter Customer Ref.:", traBillerRef, 50, "/c");
		CHECK(ret>0, lblKO);
	}


	MAPPUTSTR(traRqsBitMap, "087020058000C00002",lblKO);
	MAPPUTSTR(traRqsMTI, "020200",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "320000",lblKO);
	MAPPUTSTR(traPan, "1234568888888882",lblKO);
	MAPPUTSTR(traEntMod, "k", lblKO);

	//Send Online
	onlSession();

	lblKO:;
}

static void fncProcessSurcharge(char * SourceData, char * ResultData, char * Commission){
	float valueToRound = 1500.777779;
	float roundedValue = ceilf(valueToRound * 100) / 100;
	float Percentage = 3, surchargeAmt = 0;
	char result[17];
	char Surcharge[17];
	char SurchargeAmtStr[17];
	int ret = 0;

	memset(result, 0, sizeof(result));
	memset(Surcharge, 0, sizeof(Surcharge));
	memset(SurchargeAmtStr, 0, sizeof(SurchargeAmtStr));

	valueToRound = atof(SourceData);
	MAPGET(appBillerSurcharge, Surcharge, lblKO);
	lblKO:

	///First Save the Raw amount
	roundedValue = roundFloat(valueToRound, 2);
	snprintf(result, sizeof result, "%.2f", roundedValue);
	fncRemoveDecimal(result);
	MAPPUTSTR(traBillerOriginalAmt ,result, lblKO);
	memset(result, 0, sizeof(result));


	//Apply surcharge
	Percentage =  atof(Surcharge);
	if (Percentage < 1) {
		goto lblNOCOMMISSION; //if the commission is not applicable
	}

	surchargeAmt =  (valueToRound * (100 - (Percentage/100))) / 100;
	snprintf(SurchargeAmtStr, sizeof SurchargeAmtStr, "%.2f", surchargeAmt);
	strcpy(Commission, SurchargeAmtStr);

	Percentage = atof(Surcharge) + 10000;
	if (Percentage > 20000) {
		Percentage = 20000;
	}


	valueToRound = (valueToRound * (Percentage/100)) / 100;

	lblNOCOMMISSION:
	//Current job of decimal places
	roundedValue = roundFloat(valueToRound, 2);

	snprintf(result, sizeof result, "%.2f", roundedValue);

	strcpy(ResultData, result);
}


void fncUnFormatAndSaveAmount(char * AmountFormatted, word saveLocation){
	int ret = 0;
	int idxToDel = 0;
	int decimalplace = 2;
	char AmtToformat[100];
	char AmtSurcharge[100];

	memset(AmtToformat, 0, sizeof(AmtToformat));

	idxToDel = fncGetCharPosition(AmountFormatted, '.');

	decimalplace = strlen(AmountFormatted) - idxToDel - 1;

	if (decimalplace < 2) {
		strcat(AmountFormatted,"0");
	}
	//----- add commision ----
	fncProcessSurcharge(AmountFormatted, AmtToformat, AmtSurcharge);
	//------------------------

	memmove(&AmtSurcharge[idxToDel], &AmtSurcharge[idxToDel + 1], strlen(AmtSurcharge) - idxToDel);
	memmove(&AmtToformat[idxToDel], &AmtToformat[idxToDel + 1], strlen(AmtToformat) - idxToDel);

	MAPPUTSTR(saveLocation ,AmtToformat, lblKO);
	MAPPUTSTR(traSurchargeAmt ,AmtSurcharge, lblKO);

	lblKO:;
}

int fncBillerBeforePayment(void){
	int ret = 0;
	char Menu[lenMnu];
	card key = mnuBiller;
	char DataRsp[1024];
	char DataSave[100];
	char PrintData[1024];

	memset(DataRsp, 0, sizeof(DataRsp));
	memset(PrintData, 0, sizeof(PrintData));

	//------------- Prepare all data from response -----------
	MAPGET(traAlternateRsp, DataRsp, lblKO);
	char *array=DataRsp;

	memset(DataSave, 0, sizeof(DataSave));
	ret = fmtTok(DataSave, array, ":");    //extract BILLNAME  token
	array+=ret;//skip data
	array++;   //skip separator

	memset(DataSave, 0, sizeof(DataSave));
	ret = fmtTok(DataSave, array, ":");    //extract REF token
	array+=ret;//skip data
	array++;   //skip separator

	memset(DataSave, 0, sizeof(DataSave));
	ret = fmtTok(DataSave, array, ":");    //extract PAYEE token
	array+=ret;//skip data
	array++;   //skip separator

	memset(DataSave, 0, sizeof(DataSave));
	ret = fmtTok(DataSave, array, ":");    //extract AMOUNT token
	array+=ret;//skip data
	array++;   //skip separator
	fncUnFormatAndSaveAmount(array, traAmt);

	//-------------------------------------------------------
	num2dec(Menu, key, 0);
	MAPPUTSTR(traMnuItm, Menu, lblKO);

	MAPPUTSTR(traRqsBitMap, "087038058020C00006",lblKO);
	MAPPUTSTR(traRqsMTI, "020200",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);

	return fncConfirmTotalAmount();

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Transaction Cancelled!!!", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);

	return -1;
}

void fncBillerAdvice(void){
	int ret = 0;
	char Menu[lenMnu];
	card key = mnuBillerAdvice;
	char OriginalAmount[lenAmt + 1];


	ret = valRspCod();
	CHECK(ret > 0, lblKO);

	num2dec(Menu, key, 0);
	MAPPUTSTR(traMnuItm, Menu, lblKO);

	MAPPUTSTR(traRqsBitMap, "087038058004C00006",lblKO);
	MAPPUTSTR(traRqsMTI, "020220",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "000000",lblKO);

	memset(OriginalAmount, 0, sizeof(OriginalAmount));
	MAPGET(traBillerOriginalAmt, OriginalAmount, lblKO);
	MAPPUTSTR(traAmt, OriginalAmount,lblKO);

	//Send Online
	onlSession();

	//	// Save transaction into log table (Batch)
	//	ret = logSave();
	//	CHECK(ret > 0, lblKO);

	lblKO:;
}

static void fncBillerProductCodes(void){
	int ret = 0;
	char displayData[1000];
	char temp[100];

	memset(temp, 0, sizeof(temp));
	memset(displayData, 0, sizeof(displayData));

	MAPGET(appBillerServiceProduct, temp, lblKO);
	strcpy(displayData, "Old value: ");
	strcat(displayData, temp);
	strcat(displayData, "\nNew Service Product Code:");


	ret = Application_Request_Data(displayData, appBillerServiceProduct, 50, "/c");
	CHECK(ret>0, lblKO);

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Service Product Code Setup Completed!!!", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);
}

static void fncBillerPrefix(void){
	int ret = 0;
	char displayData[1000];
	char temp[100];

	memset(temp, 0, sizeof(temp));
	memset(displayData, 0, sizeof(displayData));

	MAPGET(appBillerPrefix, temp, lblKO);
	strcpy(displayData, "Old value: ");
	strcat(displayData, temp);
	strcat(displayData, "\nEnter Biller Prefix.:");

	ret = Application_Request_Data(displayData, appBillerPrefix, 50, "/c");
	CHECK(ret>0, lblKO);

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Setup Completed!!!", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);
}


static void fncBillerSurcharge(void){
	int ret = 0;
	char displayData[1000];
	char temp[100];

	memset(temp, 0, sizeof(temp));
	memset(displayData, 0, sizeof(displayData));

	MAPGET(appBillerSurcharge, temp, lblKO);
	strcpy(displayData, "Old value: ");
	strcat(displayData, temp);
	strcat(displayData, "\nEnter Convenience Fee(%):");

	ret = Application_Request_Data(displayData, appBillerSurcharge, 6, "/d");
	CHECK(ret>0, lblKO);

	lblKO:
	GL_Dialog_Message(hGoal, NULL, "Setup Completed!!!", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);
}


static void fncBillerRef__ON_OFF(void){
	ulong Result = 0;
	int ret = 0;

	Result = GL_Dialog_Menu(hGoal, "Biller Reference:", BillerRef__ON_OFF, Result, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((Result!=GL_KEY_CANCEL) && (Result!=GL_RESULT_INACTIVITY), lblEnd);

	switch (Result) {
	case 0: // ENABLED
		MAPPUTCARD(appIsRef_ON, 1,lblEnd);
		break;

	case 1:// OFF
	default:
		MAPPUTCARD(appIsRef_ON, 0,lblEnd);
		break;

	}

	lblEnd:
	GL_Dialog_Message(hGoal, NULL, "Setup Completed!!!", GL_ICON_NONE, GL_BUTTON_ALL, 2*1000);
}

void fncBillerConfig(void){
	ulong Result = 0;

	MenuAgain:

	Result = GL_Dialog_Menu(hGoal, "Select Biller Conf. EDIT:", BillerConfiguration, Result, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((Result!=GL_KEY_CANCEL) && (Result!=GL_RESULT_INACTIVITY), lblEnd);

	switch (Result) {
	case 0: //Create a Product Code
		fncBillerProductCodes();
		break;
	case 1: //Create a biller Prefix Code
		fncBillerPrefix();
		break;
	case 2: //Mode of reference ON or OFF
		fncBillerRef__ON_OFF();
		break;
	case 3: //Surcharge percentage
		fncBillerSurcharge();
		break;
	}
	goto MenuAgain;

	lblEnd:;

}

void fncTerminalModeConfig(void){
	ulong Result = 0;


	Result = GL_Dialog_Menu(hGoal, "SELECT TERMINAL USE MODE:", TerminalModeConfiguration, Result, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	CHECK((Result!=GL_KEY_CANCEL) && (Result!=GL_RESULT_INACTIVITY), lblEnd);

	switch (Result) {
	case 1: //Set Terminal as Biller terminal mode
		mapPutByte(appTerminalMode, 1);
		break;
	case 0: //Set Terminal as normal terminal mode
	default:
		mapPutByte(appTerminalMode, 0);
		break;
	}

	GL_Dialog_Message(hGoal, NULL, "Setting Up\nPlease Wait...", GL_ICON_NONE, GL_BUTTON_ALL, GL_TIME_SECOND);


	lblEnd:;

}


static void fncEnableDisableMenu(word menuItem, word setParent, int Enabled){
	char Hidden[2];
	char Statement[256];
	char QueryData[100];
	int iRet = 0;

	// ----- save the data now -------
	memset(QueryData, 0, sizeof(QueryData));
	memset(Statement, 0, sizeof(Statement));
	memset(Hidden, 0, sizeof(Hidden));

	if(Enabled == 1)
		strcpy(Hidden, "0");
	else
		strcpy(Hidden, "1");

	//Create the Query
	Telium_Sprintf (Statement, "UPDATE AppMenus SET MenuIdParent = '%d', Hidden = '%s'  WHERE MenuId = '%d';", setParent, Hidden, menuItem);

	//UPDATE parameters SET details = '%s' where paramName = '%s';
	iRet = Sqlite_Run_Statement(Statement, QueryData);
	CHECK(iRet > 0,lblDbaErr);

	lblDbaErr:;
}

static void fncRenameMenu(word menuItem,  char * NewMenuName){
	char Statement[256];
	char QueryData[100];
	int iRet = 0;

	// ----- save the data now -------
	memset(QueryData, 0, sizeof(QueryData));
	memset(Statement, 0, sizeof(Statement));

	//Create the Query
	Telium_Sprintf (Statement, "UPDATE AppMenus SET MenuName = '%s' WHERE MenuId = '%d';", NewMenuName, menuItem);

	//UPDATE parameters SET details = '%s' where paramName = '%s';
	iRet = Sqlite_Run_Statement(Statement, QueryData);
	CHECK(iRet > 0,lblDbaErr);

	lblDbaErr:;
}

void fncProcessTemrinalModes(void){
	byte Mode = 0;

	mapGetByte(appTerminalMode, Mode);

	switch (Mode) {
	case 1: //Modify Terminal behavior for Biller terminal
		///---Customer Modifications
		fncEnableDisableMenu(mnuCustSettlement, mnuCustomer, 1); //Enable Settlement
		fncEnableDisableMenu(mnuBiller, mnuCustomer, 1);         //Enable Settlement
		fncEnableDisableMenu(mnuEchoTest, mnuCustomer, 1);       //Enable EchoTest

		fncEnableDisableMenu(mnuSale, mnuCustomer, 0);           //Disable Sale
		fncEnableDisableMenu(mnuPreaut, mnuCustomer, 0);         //Disable Pre-auth
		fncEnableDisableMenu(mnuCompletion, mnuCustomer, 0);     //Disable Completion
		fncEnableDisableMenu(mnuRefund, mnuCustomer, 0);         //Disable Refund
		fncEnableDisableMenu(mnuVoid, mnuCustomer, 0);           //Disable Void
		fncEnableDisableMenu(mnuLogOn, mnuCustomer, 0);          //Disable Logon
		fncEnableDisableMenu(mnuBalanceEnquiry, mnuCustomer, 0); //Disable Balance Inquiry


		///---merchant Modifications
		//Rename merchant menu name
		fncRenameMenu(mnuMerchant, "OTHER FUNCTIONS");

		fncEnableDisableMenu(mnuMrcResetRev, mnuMerchant, 0);  //Disable delete reversal
		fncEnableDisableMenu(mnuMrcReset, mnuMerchant, 0);     //Disable delete Batch

		///---Admin Modifications
		fncEnableDisableMenu(mnuAdmin, 0, 0);                  //Disable Admin module

		break;
	case 0:// Modify terminal behavior to normal
	default:

		///---Customer Modifications
		fncEnableDisableMenu(mnuAdmin, 0, 0);
		fncEnableDisableMenu(mnuCustSettlement, mnuCustomer, 0); //Disable Settlement
		fncEnableDisableMenu(mnuBiller, mnuCustomer, 0);         //Disable Settlement
		fncEnableDisableMenu(mnuEchoTest, mnuCustomer, 0);       //Disable EchoTest

		fncEnableDisableMenu(mnuSale, mnuCustomer, 1);           //Enable Sale
		fncEnableDisableMenu(mnuPreaut, mnuCustomer, 1);         //Enable Preauth
		fncEnableDisableMenu(mnuCompletion, mnuCustomer, 1);     //Enable Completion
		fncEnableDisableMenu(mnuRefund, mnuCustomer, 1);         //Enable Refund
		fncEnableDisableMenu(mnuVoid, mnuCustomer, 1);           //Enable Void
		fncEnableDisableMenu(mnuLogOn, mnuCustomer, 1);          //Enable Logon
		fncEnableDisableMenu(mnuBalanceEnquiry, mnuCustomer, 1); //Enable Balance Inquiry


		///---merchant Modifications
		//re instate the menu name of Merchant
		fncRenameMenu(mnuMerchant, "MERCHANT>");

		fncEnableDisableMenu(mnuMrcResetRev, mnuMerchant, 1);   //Enable delete reversal
		fncEnableDisableMenu(mnuMrcReset, mnuMerchant, 1);      //Enable delete Batch

		///---Admin Modifications
		fncEnableDisableMenu(mnuAdmin, 0, 1);                   //Enable Admin Menu

		break;
	}

	GL_Dialog_Message(hGoal, NULL, "Done...", GL_ICON_NONE, GL_BUTTON_ALL, 100);
}

int fncDisplayData_Goal(const char *header, const char *line1, const char *line2,int duration,int beeping) {
	int ret;
	char temp1[50];
	char dspData[300];

	memset(temp1, 0, sizeof(temp1));
	memset(dspData, 0, sizeof(dspData));

	strcpy(dspData,line1);
	strcat(dspData," \n");
	strcat(dspData,line2);

	if(beeping == 1){
		buzzer(1);
		buzzer(1);
	}else if(beeping == 2){
		buzzer(1);
		buzzer(1);
		buzzer(10);
	}

	confirmGraphicLibHandle();

	ret = GL_Dialog_Message  (hGoal, header, dspData, GL_ICON_NONE, GL_BUTTON_ALL, duration);

	switch (ret){
	case GL_KEY_VALID:
		ret = kbdVAL;
		break;
	case GL_KEY_CANCEL:
		ret = kbdANN;
		break;
	case GL_RESULT_INACTIVITY:
		break;
	default:
		break;
	}

	goto lblEnd;

	lblEnd:
	return ret;
}

int fncConfirmData(char * Data_to_confirm){
	int ret = 0;
	char DspData[2048];

	memset(DspData, 0, sizeof(DspData));

	strcat(DspData, Data_to_confirm);

	strcat(DspData, "\nNo=Cancel / Yes=Valid");
	ret = GL_Dialog_Message(hGoal,  NULL, (char *)DspData, GL_ICON_QUESTION, GL_BUTTON_VALID_CANCEL, 30*1000);

	if (ret == GL_KEY_VALID) {
		ret = 1;
	}else{
		ret = 0;
	}

	return ret;
}


int fncConfirmTotalAmount(void){
	int ret = 0;
	char Amt[lenAmt + 1];
	char TempData[100];
	char DataConfirming[2047];

	memset(Amt, 0, sizeof(Amt));
	memset(TempData, 0, sizeof(TempData));
	memset(DataConfirming, 0, sizeof(DataConfirming));

	strcpy(DataConfirming, "Please Confirm Total Amount:");

	MAPGET(traAmt, Amt, lblKO);
	fmtAmt(TempData,Amt,2,".,");

	strcat(DataConfirming, TempData);

	return fncConfirmData(DataConfirming);

	lblKO:
	return -1;
}

/**
 * This function reads a parameter set by other app for connection in session
 * @return
 */
int isApp_Already_in_Session(void){
	char tcDirin_SesFile[100];
	char Filein_SesData[100+1];
	T_GL_HFILE hFile;
	int iRet = 0, iLen = 1;
	int reData = 0;

	memset(Filein_SesData, 0, sizeof(Filein_SesData));
	memset(tcDirin_SesFile, 0, sizeof(tcDirin_SesFile));

	memcpy(tcDirin_SesFile,"file://flash/HOST/SESSION.txt",29);
	hFile = GL_File_Open(tcDirin_SesFile, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
	CHECK(hFile!=NULL, lblEnd);           // Open the file received

	iLen = GL_File_GetSize(hFile);           // File length in bytes

	iRet = GL_File_Read(hFile, Filein_SesData, iLen); // Read data from file
	CHECK(iRet==iLen, lblEmptyFile);

	if (strncmp(Filein_SesData, "1", 1) == 0) {
		reData = 1;
	}else{
		reData = 0;
	}

	lblEmptyFile:

	iRet = GL_File_Close(hFile);             // Close the file
	CHECK(iRet==GL_SUCCESS, lblKOFile);

	iRet = reData;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOFile:
	iRet = -1;
	goto lblEnd;

	lblEnd:
	return iRet;
}



/**
 * This function reads a parameter set by other app for connection in session
 * @return
 */
int fncTMSConnectionSession(void){
	char tcDirFile[100];
	char FileConnSessData[2048+1];
	T_GL_HFILE hFile;
	int iRet = 0, iLen = 1;
	int reData = 0;

	memset(tcDirFile, 0, sizeof(tcDirFile));

	memcpy(tcDirFile,"file://flash/HOST/TMSRUN.txt",28);
	hFile = GL_File_Open(tcDirFile, GL_FILE_OPEN_EXISTING, GL_FILE_ACCESS_READ);
	CHECK(hFile!=NULL, lblEnd);           // Open the file received

	iLen = GL_File_GetSize(hFile);           // File length in bytes

	memset(FileConnSessData, 0, sizeof(FileConnSessData));
	iRet = GL_File_Read(hFile, FileConnSessData, iLen); // Read data from file
	CHECK(iRet==iLen, lblEmptyFile);

	if (strncmp(FileConnSessData, "1", 1) == 0) {
		reData = 1;
	} else {
		reData = 0;
	}

	lblEmptyFile:

	iRet = GL_File_Close(hFile);             // Close the file
	CHECK(iRet==GL_SUCCESS, lblKOFile);

	iRet = reData;
	goto lblEnd;

	// Errors treatment
	// ****************
	lblKOFile:
	iRet = -1;
	goto lblEnd;

	lblEnd:
	return iRet;
}

void fncResetTerminalData(void){
	SqliteDB_Init();
}

void fncPanRemove_F(char * PAN_Number){
	int len = 0;
	int ctr = 0;
	int localcount = 0;
	char LocalPAN[40];

	memset(LocalPAN, 0, sizeof(LocalPAN));

	len = strlen(PAN_Number);
	for (ctr = 0; ctr < len; ctr++) {
		if(PAN_Number[ctr] == 'F') {
			continue;
		}
		LocalPAN[localcount] = PAN_Number[ctr];
		localcount++;
	}

	memset(PAN_Number, 0, len);
	strcpy(PAN_Number, LocalPAN);

}
