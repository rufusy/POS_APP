
//****************************************************************************
//      INCLUDES
//****************************************************************************
#include <globals.h>
#include "Sqlite.h"

//****************************************************************************
//      EXTERN
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

/*****************************
 *
 *Reset Database fields
 *
 */
static void memsetAllFields(void){
	memset(isoDrCr ,0 ,sizeof(isoDrCr));
	memset(isoMnuItm ,0 ,sizeof(isoMnuItm));
	memset(invoiceNo ,0 ,sizeof(invoiceNo));
	memset(isoField000 ,0 ,sizeof(isoField000));
	memset(isoField001 ,0 ,sizeof(isoField001));
	memset(isoField002 ,0 ,sizeof(isoField002));
	memset(isoField003 ,0 ,sizeof(isoField003));
	memset(isoField004 ,0 ,sizeof(isoField004));
	memset(isoField005 ,0 ,sizeof(isoField005));
	memset(isoField006 ,0 ,sizeof(isoField006));
	memset(isoField007 ,0 ,sizeof(isoField007));
	memset(isoField008 ,0 ,sizeof(isoField008));
	memset(isoField009 ,0 ,sizeof(isoField009));
	memset(isoField010 ,0 ,sizeof(isoField010));
	memset(isoField011 ,0 ,sizeof(isoField011));
	memset(isoField012 ,0 ,sizeof(isoField012));
	memset(isoField013 ,0 ,sizeof(isoField013));
	memset(isoField014 ,0 ,sizeof(isoField014));
	memset(isoField015 ,0 ,sizeof(isoField015));
	memset(isoField016 ,0 ,sizeof(isoField016));
	memset(isoField017 ,0 ,sizeof(isoField017));
	memset(isoField018 ,0 ,sizeof(isoField018));
	memset(isoField019 ,0 ,sizeof(isoField019));
	memset(isoField020 ,0 ,sizeof(isoField020));
	memset(isoField021 ,0 ,sizeof(isoField021));
	memset(isoField022 ,0 ,sizeof(isoField022));
	memset(isoField023 ,0 ,sizeof(isoField023));
	memset(isoField024 ,0 ,sizeof(isoField024));
	memset(isoField025 ,0 ,sizeof(isoField025));
	memset(isoField026 ,0 ,sizeof(isoField026));
	memset(isoField027 ,0 ,sizeof(isoField027));
	memset(isoField028 ,0 ,sizeof(isoField028));
	memset(isoField029 ,0 ,sizeof(isoField029));
	memset(isoField030 ,0 ,sizeof(isoField030));
	memset(isoField031 ,0 ,sizeof(isoField031));
	memset(isoField032 ,0 ,sizeof(isoField032));
	memset(isoField033 ,0 ,sizeof(isoField033));
	memset(isoField034 ,0 ,sizeof(isoField034));
	memset(isoField035 ,0 ,sizeof(isoField035));
	memset(isoField036 ,0 ,sizeof(isoField036));
	memset(isoField037 ,0 ,sizeof(isoField037));
	memset(isoField038 ,0 ,sizeof(isoField038));
	memset(isoField039 ,0 ,sizeof(isoField039));
	memset(isoField040 ,0 ,sizeof(isoField040));
	memset(isoField041 ,0 ,sizeof(isoField041));
	memset(isoField042 ,0 ,sizeof(isoField042));
	memset(isoField043 ,0 ,sizeof(isoField043));
	memset(isoField044 ,0 ,sizeof(isoField044));
	memset(isoField045 ,0 ,sizeof(isoField045));
	memset(isoField046 ,0 ,sizeof(isoField046));
	memset(isoField047 ,0 ,sizeof(isoField047));
	memset(isoField048 ,0 ,sizeof(isoField048));
	memset(isoField049 ,0 ,sizeof(isoField049));
	memset(isoField050 ,0 ,sizeof(isoField050));
	memset(isoField051 ,0 ,sizeof(isoField051));
	memset(isoField052 ,0 ,sizeof(isoField052));
	memset(isoField053 ,0 ,sizeof(isoField053));
	memset(isoField054 ,0 ,sizeof(isoField054));
	//	memset(isoField055 ,0 ,sizeof(isoField055)); // ALREADY HAS DATA DONT CLEAR
	memset(isoField056 ,0 ,sizeof(isoField056));
	memset(isoField057 ,0 ,sizeof(isoField057));
	memset(isoField058 ,0 ,sizeof(isoField058));
	memset(isoField059 ,0 ,sizeof(isoField059));
	memset(isoField060 ,0 ,sizeof(isoField060));
	memset(isoField061 ,0 ,sizeof(isoField061));
	memset(isoField062 ,0 ,sizeof(isoField062));
	memset(isoField063 ,0 ,sizeof(isoField063));
	memset(isoField064 ,0 ,sizeof(isoField064));
	memset(isoField065 ,0 ,sizeof(isoField065));
	memset(isoField066 ,0 ,sizeof(isoField066));
	memset(isoField067 ,0 ,sizeof(isoField067));
	memset(isoField068 ,0 ,sizeof(isoField068));
	memset(isoField069 ,0 ,sizeof(isoField069));
	memset(isoField070 ,0 ,sizeof(isoField070));
	memset(isoField071 ,0 ,sizeof(isoField071));
	memset(isoField072 ,0 ,sizeof(isoField072));
	memset(isoField073 ,0 ,sizeof(isoField073));
	memset(isoField074 ,0 ,sizeof(isoField074));
	memset(isoField075 ,0 ,sizeof(isoField075));
	memset(isoField076 ,0 ,sizeof(isoField076));
	memset(isoField077 ,0 ,sizeof(isoField077));
	memset(isoField078 ,0 ,sizeof(isoField078));
	memset(isoField079 ,0 ,sizeof(isoField079));
	memset(isoField080 ,0 ,sizeof(isoField080));
	memset(isoField081 ,0 ,sizeof(isoField081));
	memset(isoField082 ,0 ,sizeof(isoField082));
	memset(isoField083 ,0 ,sizeof(isoField083));
	memset(isoField084 ,0 ,sizeof(isoField084));
	memset(isoField085 ,0 ,sizeof(isoField085));
	memset(isoField086 ,0 ,sizeof(isoField086));
	memset(isoField087 ,0 ,sizeof(isoField087));
	memset(isoField088 ,0 ,sizeof(isoField088));
	memset(isoField089 ,0 ,sizeof(isoField089));
	memset(isoField090 ,0 ,sizeof(isoField090));
	memset(isoField091 ,0 ,sizeof(isoField091));
	memset(isoField092 ,0 ,sizeof(isoField092));
	memset(isoField093 ,0 ,sizeof(isoField093));
	memset(isoField094 ,0 ,sizeof(isoField094));
	memset(isoField095 ,0 ,sizeof(isoField095));
	memset(isoField096 ,0 ,sizeof(isoField096));
	memset(isoField097 ,0 ,sizeof(isoField097));
	memset(isoField098 ,0 ,sizeof(isoField098));
	memset(isoField099 ,0 ,sizeof(isoField099));
	memset(isoField100 ,0 ,sizeof(isoField100));
	memset(isoField101 ,0 ,sizeof(isoField101));
	memset(isoField102 ,0 ,sizeof(isoField102));
	memset(isoField103 ,0 ,sizeof(isoField103));
	memset(isoField104 ,0 ,sizeof(isoField104));
	memset(isoField105 ,0 ,sizeof(isoField105));
	memset(isoField106 ,0 ,sizeof(isoField106));
	memset(isoField107 ,0 ,sizeof(isoField107));
	memset(isoField108 ,0 ,sizeof(isoField108));
	memset(isoField109 ,0 ,sizeof(isoField109));
	memset(isoField110 ,0 ,sizeof(isoField110));
	memset(isoField111 ,0 ,sizeof(isoField111));
	memset(isoField112 ,0 ,sizeof(isoField112));
	memset(isoField113 ,0 ,sizeof(isoField113));
	memset(isoField114 ,0 ,sizeof(isoField114));
	memset(isoField115 ,0 ,sizeof(isoField115));
	memset(isoField116 ,0 ,sizeof(isoField116));
	memset(isoField117 ,0 ,sizeof(isoField117));
	memset(isoField118 ,0 ,sizeof(isoField118));
	memset(isoField119 ,0 ,sizeof(isoField119));
	memset(isoField120 ,0 ,sizeof(isoField120));
	memset(isoField121 ,0 ,sizeof(isoField121));
	memset(isoField122 ,0 ,sizeof(isoField122));
	memset(isoField123 ,0 ,sizeof(isoField123));
	memset(isoField124 ,0 ,sizeof(isoField124));
	memset(isoField125 ,0 ,sizeof(isoField125));
	memset(isoField126 ,0 ,sizeof(isoField126));
	memset(isoField127 ,0 ,sizeof(isoField127));
	memset(isoField128 ,0 ,sizeof(isoField128));
}

/*
 * feed in the fields for saving
 */
static void logFeedTableFields(void){
	int ret = 0;
	char MENU[lenMnu + 1];

	memset(MENU, 0, sizeof(MENU));

	//clear all fields
	memsetAllFields();

	//get the Transaction on database
	MAPGET(traMnuItm, isoMnuItm, lblKO);

	//transaction type Dr/Cr
	MAPGET(traDrCr, isoDrCr,lblKO);

	//Invoice Number
	MAPGET(traInvNum, invoiceNo, lblKO);

	//FIELD 0 (MTI)
	MAPGET(traRqsMTI, isoField000, lblKO);

	//FIELD 1

	//FIELD 2 (PAN)
	MAPGET(traPan, isoField002,lblKO);

	//FIELD 3 (PROCESSING CODE)
	MAPGET(traRqsProcessingCode, isoField003,lblKO);

	//FIELD 4 (AMOUNT)
	MAPGET(traAmt, isoField004,lblKO);

	//FIELD 5 (AMOUNT)
	//FIELD 6

	//FIELD 7 (DATE AND TIME)
	MAPGET(traDatTim, isoField007,lblKO);

	//FIELD 8
	//FIELD 9
	//FIELD 10

	//FIELD 11 ('STAN' SYSTEM TRACE AUDIT NUMBER)
	MAPGET(traSTAN, isoField011,lblKO);

	//FIELD 12
	//FIELD 13

	//FIELD 14 (EXPIRY DATE)
	MAPGET(traExpDat, isoField014,lblKO);

	//FIELD 15
	//FIELD 16
	//FIELD 17
	//FIELD 18
	//FIELD 19
	//FIELD 20
	//FIELD 21

	//FIELD 22 (POS ENT MODE)
	MAPGET(traPosEntMod, isoField022,lblKO);

	//FIELD 23 (CARD SEQUENCE NUMBER)
	MAPGET(traCrdSeq, isoField023,lblKO);

	//FIELD 24 (NII)
	MAPGET(appNII, isoField024,lblKO);

	//FIELD 25 (EXPIRY DATE)
	MAPGET(traConCode, isoField025,lblKO);

	//FIELD 26
	//FIELD 27
	//FIELD 28
	//FIELD 29
	//FIELD 30
	//FIELD 31
	//FIELD 32
	//FIELD 33
	//FIELD 34

	//FIELD 35 (TRACK 2 EQUIVALENT DATA)
	MAPGET(traTrk2, isoField035,lblKO);

	//FIELD 36

	//FIELD 37 (REFERENCE RETRIEVAL NUMBER)
	MAPGET(traRrn, isoField037,lblKO);

	//FIELD 38 (APPROVAL CODE)
	MAPGET(traAutCod, isoField038,lblKO);

	//FIELD 39 (RESPONSE CODE)
	MAPGET(traRspCod, isoField039,lblKO);

	//FIELD 40

	//FIELD 41 (TERMINAL ID)
	MAPGET(appTID, isoField041,lblKO);

	//FIELD 42 (MERCHANT ID)
	MAPGET(appMID, isoField042,lblKO);

	//FIELD 43
	//FIELD 44
	//FIELD 45
	//FIELD 46
	//FIELD 47
	//FIELD 48

	//FIELD 49
	MAPGET(emvTrnCurCod, isoField049,lblKO);

	//FIELD 50
	//FIELD 51
	//FIELD 52
	//FIELD 53
	//FIELD 54
	MAPGET(traCashbackAmt, isoField054,lblKO);

	//FIELD 55
	/// DATA was fed into field by Transaction earlier //MAPGET(traEMVDATA, isoField055,lblKO);

	//FIELD 56
	//FIELD 57
	//FIELD 58
	//FIELD 59
	//FIELD 60
	//FIELD 61
	//FIELD 62
	MAPGET(traBillerPaymentDetails, isoField062,lblKO);
	//FIELD 63
	MAPGET(traField063, isoField063,lblKO);
	//FIELD 64

	lblKO:;
}

int logSave(void){
	int ret = 0;
	char Statement[8192];
	char DataResponse[256];

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));

	ret = isApproved();
	CHECK(ret > 0, lblDeclined);   // Transaction approved?

	//Prepare data for database
	logFeedTableFields();

	//Create the Query
	Telium_Sprintf (Statement, "INSERT INTO log (MenuItem, InvoiceNo, isoField000, isoField001, isoField002, isoField003, isoField004, isoField005, isoField006, isoField007, isoField008, isoField009, isoField010, isoField011, isoField012, isoField013, isoField014, isoField015, isoField016, isoField017, isoField018, isoField019, isoField020, isoField021, isoField022, isoField023, isoField024, isoField025, isoField026, isoField027, isoField028, isoField029, isoField030, isoField031, isoField032, isoField033, isoField034, isoField035, isoField036, isoField037, isoField038, isoField039, isoField040, isoField041, isoField042, isoField043, isoField044, isoField045, isoField046, isoField047, isoField048, isoField049, isoField050, isoField051, isoField052, isoField053, isoField054, isoField055, isoField056, isoField057, isoField058, isoField059, isoField060, isoField061, isoField062, isoField063, isoField064, isoField065, isoField066, isoField067, isoField068, isoField069, isoField070, isoField071, isoField072, isoField073, isoField074, isoField075, isoField076, isoField077, isoField078, isoField079, isoField080, isoField081, isoField082, isoField083, isoField084, isoField085, isoField086, isoField087, isoField088, isoField089, isoField090, isoField091, isoField092, isoField093, isoField094, isoField095, isoField096, isoField097, isoField098, isoField099, isoField100, isoField101, isoField102, isoField103, isoField104, isoField105, isoField106, isoField107, isoField108, isoField109, isoField110, isoField111, isoField112, isoField113, isoField114, isoField115, isoField116, isoField117, isoField118, isoField119, isoField120, isoField121, isoField122, isoField123, isoField124, isoField125, isoField126, isoField127, isoField128, isoDrCr, isoVoided) VALUES ('%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s' ,'%s', '0');", isoMnuItm, invoiceNo, isoField000, isoField001, isoField002, isoField003, isoField004, isoField005, isoField006, isoField007, isoField008, isoField009, isoField010, isoField011, isoField012, isoField013, isoField014, isoField015, isoField016, isoField017, isoField018, isoField019, isoField020, isoField021, isoField022, isoField023, isoField024, isoField025, isoField026, isoField027, isoField028, isoField029, isoField030, isoField031, isoField032, isoField033, isoField034, isoField035, isoField036, isoField037, isoField038, isoField039, isoField040, isoField041, isoField042, isoField043, isoField044, isoField045, isoField046, isoField047, isoField048, isoField049, isoField050, isoField051, isoField052, isoField053, isoField054, isoField055, isoField056, isoField057, isoField058, isoField059, isoField060, isoField061, isoField062, isoField063, isoField064, isoField065, isoField066, isoField067, isoField068, isoField069, isoField070, isoField071, isoField072, isoField073, isoField074, isoField075, isoField076, isoField077, isoField078, isoField079, isoField080, isoField081, isoField082, isoField083, isoField084, isoField085, isoField086, isoField087, isoField088, isoField089, isoField090, isoField091, isoField092, isoField093, isoField094, isoField095, isoField096, isoField097, isoField098, isoField099, isoField100, isoField101, isoField102, isoField103, isoField104, isoField105, isoField106, isoField107, isoField108, isoField109, isoField110, isoField111, isoField112, isoField113, isoField114, isoField115, isoField116, isoField117, isoField118, isoField119, isoField120, isoField121, isoField122, isoField123, isoField124, isoField125, isoField126, isoField127, isoField128, isoDrCr);

	ret = Sqlite_Run_Statement(Statement, DataResponse);
	CHECK(ret > 0,lblKO);

	lblDeclined:
	ret = 1;

	lblKO:
	return ret;
}

void logIncrementBatch(void){
	int ret = 0;
	char BatchNumber[lenBatNum + 3];

	memset(BatchNumber, 0, sizeof(BatchNumber));

	MAPGET(appBatchNumber, BatchNumber, lblKO);
	addStr(BatchNumber,BatchNumber,"000001");
	fmtPad(BatchNumber, -6, '0');

	if (strncmp(BatchNumber, "999999", 6) == 0) {
		memset(BatchNumber, 0, sizeof(BatchNumber));
		strcpy( BatchNumber, "000001");
	}

	MAPPUTSTR(appBatchNumber, BatchNumber, lblKO);

	lblKO:;
}

int logReset(void){
	int ret = 0;
	char Statement[2512];
	char DataResponse[256];


	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));

	///clear terminal records
	strcpy(Statement,"DELETE FROM log;");
	ret = Sqlite_Run_Statement(Statement,DataResponse);
	CHECK(ret > 0,lblKO);

	memset(Statement, 0, sizeof(Statement));
	strcpy(Statement,"VACUUM;");
	ret = Sqlite_Run_Statement(Statement,DataResponse);
	CHECK(ret > 0,lblKO);

	memset(Statement, 0, sizeof(Statement));
	strcpy(Statement,"DROP TABLE log;");
	ret = Sqlite_Run_Statement(Statement,DataResponse);
	CHECK(ret > 0,lblKO);

	memset(Statement, 0, sizeof(Statement));
	strcpy(Statement,"CREATE TABLE IF NOT EXISTS log (id INTEGER PRIMARY KEY AUTOINCREMENT, DateTimeStamp TEXT DEFAULT CURRENT_TIMESTAMP, MenuItem TEXT, InvoiceNo TEXT, isoField000 TEXT, isoField001 TEXT, isoField002 TEXT, isoField003 TEXT, isoField004 TEXT, isoField005 TEXT, isoField006 TEXT, isoField007 TEXT, isoField008 TEXT, isoField009 TEXT, isoField010 TEXT, isoField011 TEXT, isoField012 TEXT, isoField013 TEXT, isoField014 TEXT, isoField015 TEXT, isoField016 TEXT, isoField017 TEXT, isoField018 TEXT, isoField019 TEXT, isoField020 TEXT, isoField021 TEXT, isoField022 TEXT, isoField023 TEXT, isoField024 TEXT, isoField025 TEXT, isoField026 TEXT, isoField027 TEXT, isoField028 TEXT, isoField029 TEXT, isoField030 TEXT, isoField031 TEXT, isoField032 TEXT, isoField033 TEXT, isoField034 TEXT, isoField035 TEXT, isoField036 TEXT, isoField037 TEXT, isoField038 TEXT, isoField039 TEXT, isoField040 TEXT, isoField041 TEXT, isoField042 TEXT, isoField043 TEXT, isoField044 TEXT, isoField045 TEXT, isoField046 TEXT, isoField047 TEXT, isoField048 TEXT, isoField049 TEXT, isoField050 TEXT, isoField051 TEXT, isoField052 TEXT, isoField053 TEXT, isoField054 TEXT, isoField055 TEXT, isoField056 TEXT, isoField057 TEXT, isoField058 TEXT, isoField059 TEXT, isoField060 TEXT, isoField061 TEXT, isoField062 TEXT, isoField063 TEXT, isoField064 TEXT, isoField065 TEXT, isoField066 TEXT, isoField067 TEXT, isoField068 TEXT, isoField069 TEXT, isoField070 TEXT, isoField071 TEXT, isoField072 TEXT, isoField073 TEXT, isoField074 TEXT, isoField075 TEXT, isoField076 TEXT, isoField077 TEXT, isoField078 TEXT, isoField079 TEXT, isoField080 TEXT, isoField081 TEXT, isoField082 TEXT, isoField083 TEXT, isoField084 TEXT, isoField085 TEXT, isoField086 TEXT, isoField087 TEXT, isoField088 TEXT, isoField089 TEXT, isoField090 TEXT, isoField091 TEXT, isoField092 TEXT, isoField093 TEXT, isoField094 TEXT, isoField095 TEXT, isoField096 TEXT, isoField097 TEXT, isoField098 TEXT, isoField099 TEXT, isoField100 TEXT, isoField101 TEXT, isoField102 TEXT, isoField103 TEXT, isoField104 TEXT, isoField105 TEXT, isoField106 TEXT, isoField107 TEXT, isoField108 TEXT, isoField109 TEXT, isoField110 TEXT, isoField111 TEXT, isoField112 TEXT, isoField113 TEXT, isoField114 TEXT, isoField115 TEXT, isoField116 TEXT, isoField117 TEXT, isoField118 TEXT, isoField119 TEXT, isoField120 TEXT, isoField121 TEXT, isoField122 TEXT, isoField123 TEXT, isoField124 TEXT, isoField125 TEXT, isoField126 TEXT, isoField127 TEXT, isoField128 TEXT, isoDrCr TEXT, isoVoided TEXT);");
	ret = Sqlite_Run_Statement(Statement,DataResponse);
	CHECK(ret > 0,lblKO);

	lblKO:
	return ret;
}

void logGetUniqueTransactionRecordMenus(char * MenuRecords){
	char Statement[100];
	char DataResponse[256];
	int ret = 0;

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));

	strcpy(Statement,"SELECT DISTINCT MenuItem FROM log WHERE isoField039 = '00' AND isoVoided != '1';");
	ret = Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	CHECK(ret > 0,lblKO);

	strcpy(MenuRecords, DataResponse);

	lblKO:;
}



/**
 *
 * @param STAN returned
 * @return Integer value
 */
int logGetLastStan(char *STAN){
	char Statement[100];
	char DataResponse[1024];
	int ret = 0;

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));


	///-- -> Debit
	memset(DataResponse, 0, sizeof(DataResponse));
	Telium_Sprintf(Statement,"SELECT isoField011 FROM log WHERE isoField039 = '00' AND isoVoided != '1' ORDER BY id DESC;");
	ret = Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);

	strcpy(STAN, DataResponse);

	return ret;
}


/**
 *
 * @param STAN returned
 * @return Integer value
 */
int logGetLastInv(char *Inv){
	char Statement[100];
	char DataResponse[1024];
	int ret = 0;

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));


	///-- -> Debit
	memset(DataResponse, 0, sizeof(DataResponse));
	Telium_Sprintf(Statement,"SELECT isoField011 FROM log WHERE isoField039 = '00' AND isoVoided != '1' ORDER BY id DESC;");
	ret = Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);

	strcpy(Inv, DataResponse);

	return ret;
}

/**
 *
 * @param Debits(O)
 * @param Credits(O)
 * @param DebitCount(O)
 * @param CreditCount(O)
 * @param Totals(O)
 */
void logCalcTot(char *Curr, char *Debits, char *Credits,char *DebitReversal, char *CreditReversal, char *DebitCount, char *CreditCount, char *DebitReversalCount, char *CreditReversalCount, char * Totals){
	char Statement[1024];
	char DataResponse[1024];
	char Temp[20];
	char Total[20];

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));
	memset(Total, 0, sizeof(Total));
	memset(Temp, 0, sizeof(Temp));

	//Initialize the totals with Zeros
	fmtPad(Total, -lenAmt, '0');

	///-- -> Debit
	memset(DataResponse, 0, sizeof(DataResponse));
	//Telium_Sprintf(Statement,"SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'D' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement,"SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'D' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(DebitCount, DataResponse);

	memset(DataResponse, 0, sizeof(DataResponse));
	//Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'D' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'D' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(Temp, DataResponse);
	fmtPad(Temp,-lenAmt,'0');
	strcpy(Debits, DataResponse);
	//add to totals
	addStr(Total, Temp,Total);
	fmtPad(Total, -lenAmt, '0');

	///-- -> Debit Reversal
	memset(DataResponse, 0, sizeof(DataResponse));
	//Telium_Sprintf(Statement,"SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'D' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement,"SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'D' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(DebitReversalCount, DataResponse);

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Temp, 0, sizeof(Temp));
	//Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'D' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'D' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(Temp, DataResponse);
	fmtPad(Temp,-lenAmt,'0');
	strcpy(DebitReversal, DataResponse);
	//add to totals
	subStr(Total, Total,Temp);
	fmtPad(Total, -lenAmt, '0');

	///-- -> Credit
	memset(DataResponse, 0, sizeof(DataResponse));
	//Telium_Sprintf(Statement, "SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'C' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement, "SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'C' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(CreditCount, DataResponse);

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Temp, 0, sizeof(Temp));
	//Telium_Sprintf(Statement, "SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'C' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement, "SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'C' AND MenuItem != '%d' AND MenuItem != '%d' AND isoField049 = '%s';", mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(Temp, DataResponse);
	fmtPad(Temp,-lenAmt,'0');
	strcpy(Credits, DataResponse);
	//add to totals
	subStr(Total, Total,Temp);
	fmtPad(Total, -lenAmt, '0');

	///-- -> Credit Reversal
	memset(DataResponse, 0, sizeof(DataResponse));
	//Telium_Sprintf(Statement,"SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'C' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement,"SELECT COUNT(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'C' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(CreditReversalCount, DataResponse);

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Temp, 0, sizeof(Temp));
	//Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoVoided != '1' AND isoDrCr = 'C' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Telium_Sprintf(Statement,"SELECT SUM(isoField004) FROM log WHERE isoField039 = '00' AND isoDrCr = 'C' AND MenuItem = '%d' AND MenuItem != '%d' AND isoField049 = '%s';",mnuReversal, mnuBalanceEnquiry, Curr);
	Sqlite_Run_Statement_MultiRecord(Statement,DataResponse);
	strcpy(Temp, DataResponse);
	fmtPad(Temp,-lenAmt,'0');
	strcpy(CreditReversal, DataResponse);
	//add to totals
	addStr(Total, Temp,Total);

	strcpy(Totals,Total);

}

static void logBatchUpload(void){
	int ret = 0, var = 0;
	char rspCode[lenRspCod + 1];
	int NumOfrecords = 0;

	memset(rspCode, 0, sizeof(rspCode));

	//Check if the settlement was OKAY
	MAPGET(traRspCod, rspCode, lblKO);

	if (strncmp(rspCode, "95", 2) == 0) { //Settlement was not successful

		//Get the number of records in the terminal
		for (var = 0; var < NumOfrecords; var++) {

			//Now send the transaction online
			MAPPUTSTR(traRqsBitMap, "083038078020C80006",lblKO);
			MAPPUTSTR(traRqsMTI, "020320",lblKO);

			//Send the Transaction online
			performOlineTransaction();
		}
	}

	lblKO:;
}

void logSettlement(void){
	int ret = 0;
	char Temp[100];
	char DataTemp[100];
	char TID[lenTID + 1];
	char MID[lenMid + 1];
	char CurrencyCodeAlpha[3 + 1];
	char CurrencyCodeNumeric[3 + 1];
	char menu[lenMnu + 1];
	byte ConnDetails = 0;

	memset(TID, 0, sizeof(TID));
	memset(MID, 0, sizeof(MID));
	memset(Temp, 0, sizeof(Temp));
	memset(menu, 0, sizeof(menu));
	memset(DataTemp, 0, sizeof(DataTemp));
	memset(CurrencyCodeAlpha, 0, sizeof(CurrencyCodeAlpha));
	memset(CurrencyCodeNumeric, 0, sizeof(CurrencyCodeNumeric));

	MAPGET(appCurrCodeAlpha1,CurrencyCodeAlpha,lblKO);
	MAPGET(appCurrCodeNumerc1,CurrencyCodeNumeric,lblKO);
	MAPGET(appTID_1, TID, lblKO);
	MAPGET(appMID_1, MID, lblKO);

	strncat(Temp, CurrencyCodeNumeric, 3);
	MAPPUTSTR(appCurrCodeAlpha, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(traCurrencyLabel, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(emvTrnCurCod,Temp,lblKO);
	MAPPUTSTR(traCurrencyNum,CurrencyCodeNumeric,lblKO);
	MAPPUTSTR(appTID,TID,lblKO);
	MAPPUTSTR(appMID,MID,lblKO);

	CommsGetChannel(ConnDetails);

	//Create the Query
	Telium_Sprintf (DataTemp, "Settlement for TID\n %s", TID);

	GL_Dialog_Message(hGoal, NULL, DataTemp, GL_ICON_INFORMATION, GL_BUTTON_NONE, 3000);

	//////////////////////////////////    CURRENCY 1,TID 1,MID 1   ///////////////////////////////////////
	//////// SETTLEMENT
	//Send Online
	onlSession();

	/////// BATCH UPLOAD PROCESS -------------------------------------------------------------------
	logBatchUpload();

	/////// SETTLEMENT TRAILER / BATCH CUTOVER------------------------------------------------------
	MAPPUTSTR(traRqsBitMap, "082020010000C00012",lblKO);
	MAPPUTSTR(traRqsMTI, "020500",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "960000",lblKO);
	//Send Online
	onlSession();

	//	/////// TERMINAL STATISTICS---------------------------------------------------------------------
	//	MAPPUTSTR(traRqsBitMap, "082020010000C00802",lblKO);
	//	MAPPUTSTR(traRqsMTI, "020800",lblKO);
	//	MAPPUTSTR(traRqsProcessingCode, "910000",lblKO);
	//	//Send Online
	//	onlSession();

	//////////////////////////////////    CURRENCY 2,TID 2,MID 2   ///////////////////////////////////////

	memset(TID, 0, sizeof(TID));
	memset(MID, 0, sizeof(MID));
	memset(Temp, 0, sizeof(Temp));
	memset(menu, 0, sizeof(menu));
	memset(DataTemp, 0, sizeof(DataTemp));
	memset(CurrencyCodeAlpha, 0, sizeof(CurrencyCodeAlpha));
	memset(CurrencyCodeNumeric, 0, sizeof(CurrencyCodeNumeric));


	num2dec(menu, mnuSettlement, 0);
	MAPPUTSTR(traMnuItm, menu, lblKO);

	MAPPUTSTR(traRqsBitMap, "082020030000C00016",lblKO);
	MAPPUTSTR(traRqsMTI, "020500",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "920000",lblKO);

	MAPGET(appCurrCodeAlpha2,CurrencyCodeAlpha,lblKO);
	MAPGET(appCurrCodeNumerc2,CurrencyCodeNumeric,lblKO);
	MAPGET(appTID_2, TID, lblKO);
	MAPGET(appMID_2, MID, lblKO);

	strncat(Temp, CurrencyCodeNumeric, 3);
	MAPPUTSTR(appCurrCodeAlpha, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(traCurrencyLabel, CurrencyCodeAlpha,lblKO);
	MAPPUTSTR(emvTrnCurCod,Temp,lblKO);
	MAPPUTSTR(traCurrencyNum,CurrencyCodeNumeric,lblKO);
	MAPPUTSTR(appTID,TID,lblKO);
	MAPPUTSTR(appMID,MID,lblKO);

	//Create the Query
	Telium_Sprintf (DataTemp, "Settlement for TID\n %s", TID);

	GL_Dialog_Message(hGoal, NULL, DataTemp, GL_ICON_INFORMATION, GL_BUTTON_NONE, 3000);

	//////// SETTLEMENT
	//Send Online
	onlSession();

	/////// BATCH UPLOAD PROCESS -------------------------------------------------------------------
	logBatchUpload();

	/////// SETTLEMENT TRAILER / BATCH CUTOVER------------------------------------------------------
	MAPPUTSTR(traRqsBitMap, "082020010000C00012",lblKO);
	MAPPUTSTR(traRqsMTI, "020500",lblKO);
	MAPPUTSTR(traRqsProcessingCode, "960000",lblKO);
	//Send Online
	onlSession();

	//	/////// TERMINAL STATISTICS---------------------------------------------------------------------
	//	MAPPUTSTR(traRqsBitMap, "082020010000C00802",lblKO);
	//	MAPPUTSTR(traRqsMTI, "020800",lblKO);
	//	MAPPUTSTR(traRqsProcessingCode, "910000",lblKO);
	//	//Send Online
	//	onlSession();

	lblKO:;
}

