
/****************************************************************************
 *		INCLUDE
 ****************************************************************************/
#include <globals.h>
#include "Sqlite_Def.h"
#include "Sqlite.h"
#include "MenuManager.h"


/****************************************************************************
 *		EXTERN
 ****************************************************************************/
#define PARAM_DISK "PARAMDISK"
#define DISK_HOST "/HOST"
#define DISK_PATH "/TDISK"
#define DISK_NAME "TDISK"

#ifndef LAYOUT3_ID
#define LAYOUT3_ID     0x0900
#endif

char DataBaseName[100];

/****************************************************************************
 *		PRIVATE TYPES
 ****************************************************************************/

/****************************************************************************
 *		PRIVATE DATA
 ****************************************************************************/
sqlite3 *handle;
sqlite3_stmt *stmt;
static int bCallBack;

/****************************************************************************
 *		PRIVATE CONSTANTS
 ****************************************************************************/
// Create Tables
static const char *tabCreate[] = {
		"CREATE TABLE IF NOT EXISTS AppMenus ( TableId INTEGER DEFAULT 0 PRIMARY KEY AUTOINCREMENT, MenuId INTEGER DEFAULT 0, MenuName TEXT, MenuIdParent INTEGER, Hidden INTEGER DEFAULT 0, SecureMenu INTEGER DEFAULT 0, SecureMenuLevel INTEGER DEFAULT 1,DrCr TEXT ,IconPathName TEXT );",
		"CREATE TABLE IF NOT EXISTS aid ( id INTEGER PRIMARY KEY AUTOINCREMENT, emvAidName TEXT, emvAid TEXT, emvTACDft TEXT, emvTACDen TEXT, emvTACOnl TEXT, emvThrVal TEXT, emvTarPer TEXT, emvMaxTarPer TEXT, emvDftValDDOL TEXT, emvDftValTDOL TEXT, emvTrmAvn TEXT, emvAcqId TEXT, emvTrmFlrLim TEXT, emvTCC TEXT, emvAidTxnType TEXT);",
		"CREATE TABLE IF NOT EXISTS log (id INTEGER PRIMARY KEY AUTOINCREMENT, DateTimeStamp TEXT DEFAULT CURRENT_TIMESTAMP, MenuItem TEXT, InvoiceNo TEXT, isoField000 TEXT, isoField001 TEXT, isoField002 TEXT, isoField003 TEXT, isoField004 TEXT, isoField005 TEXT, isoField006 TEXT, isoField007 TEXT, isoField008 TEXT, isoField009 TEXT, isoField010 TEXT, isoField011 TEXT, isoField012 TEXT, isoField013 TEXT, isoField014 TEXT, isoField015 TEXT, isoField016 TEXT, isoField017 TEXT, isoField018 TEXT, isoField019 TEXT, isoField020 TEXT, isoField021 TEXT, isoField022 TEXT, isoField023 TEXT, isoField024 TEXT, isoField025 TEXT, isoField026 TEXT, isoField027 TEXT, isoField028 TEXT, isoField029 TEXT, isoField030 TEXT, isoField031 TEXT, isoField032 TEXT, isoField033 TEXT, isoField034 TEXT, isoField035 TEXT, isoField036 TEXT, isoField037 TEXT, isoField038 TEXT, isoField039 TEXT, isoField040 TEXT, isoField041 TEXT, isoField042 TEXT, isoField043 TEXT, isoField044 TEXT, isoField045 TEXT, isoField046 TEXT, isoField047 TEXT, isoField048 TEXT, isoField049 TEXT, isoField050 TEXT, isoField051 TEXT, isoField052 TEXT, isoField053 TEXT, isoField054 TEXT, isoField055 TEXT, isoField056 TEXT, isoField057 TEXT, isoField058 TEXT, isoField059 TEXT, isoField060 TEXT, isoField061 TEXT, isoField062 TEXT, isoField063 TEXT, isoField064 TEXT, isoField065 TEXT, isoField066 TEXT, isoField067 TEXT, isoField068 TEXT, isoField069 TEXT, isoField070 TEXT, isoField071 TEXT, isoField072 TEXT, isoField073 TEXT, isoField074 TEXT, isoField075 TEXT, isoField076 TEXT, isoField077 TEXT, isoField078 TEXT, isoField079 TEXT, isoField080 TEXT, isoField081 TEXT, isoField082 TEXT, isoField083 TEXT, isoField084 TEXT, isoField085 TEXT, isoField086 TEXT, isoField087 TEXT, isoField088 TEXT, isoField089 TEXT, isoField090 TEXT, isoField091 TEXT, isoField092 TEXT, isoField093 TEXT, isoField094 TEXT, isoField095 TEXT, isoField096 TEXT, isoField097 TEXT, isoField098 TEXT, isoField099 TEXT, isoField100 TEXT, isoField101 TEXT, isoField102 TEXT, isoField103 TEXT, isoField104 TEXT, isoField105 TEXT, isoField106 TEXT, isoField107 TEXT, isoField108 TEXT, isoField109 TEXT, isoField110 TEXT, isoField111 TEXT, isoField112 TEXT, isoField113 TEXT, isoField114 TEXT, isoField115 TEXT, isoField116 TEXT, isoField117 TEXT, isoField118 TEXT, isoField119 TEXT, isoField120 TEXT, isoField121 TEXT, isoField122 TEXT, isoField123 TEXT, isoField124 TEXT, isoField125 TEXT, isoField126 TEXT, isoField127 TEXT, isoField128 TEXT, isoDrCr TEXT, isoVoided TEXT);"
		"CREATE TABLE IF NOT EXISTS Users (id INTEGER PRIMARY KEY AUTOINCREMENT, userName TEXT NOT NULL, password TEXT NOT NULL);",
};

// Insert rqs Data
static const char *tabInsert[] = {                                                                                                                                                                                //emvAidName,            emvAid,              emvTACDft,      emvTACDen,      emvTACOnl,       emvThrVal,   emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL,                      emvTrmAvn, emvAcqId,       emvTrmFlrLim,  emvTCC, emvAidTxnType
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'VSDC',            '07A0000000031010',  '05DC4000A800', '050010000000', '05DC4004F800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '02008C',  '059999999999', '0400000100', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'VisaElectron',    '07A0000000032010',  '05DC4000A800', '050010000000', '05DC4004F800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '02008C',  '059999999999', '0400000100', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'MasterCard',      '07A0000000041010',  '05FC50BCA000', '050010800000', '05FCF0FCF800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '020002',  '059999999999', '0400000000', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'JCB',             '07A0000000421010',  '05DC4000A800', '050010000000', '05DC4004F800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '020001',  '059999999999', '0400000100', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'Maestro',         '07A0000000043060',  '05FC50BCA000', '050010800000', '05F850FCF800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '020002',  '059999999999', '0400000100', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'Amex',            '06A00000002501',    '05DC4000A800', '050010000000', '05DC4004F800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '020001',  '059999999999', '0400000100', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'VisaInterLink',   '07A0000000033010',  '05DC4000A800', '050010000000', '05DC4004F800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '020001',  '059999999999', '0400000100', '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'VisaInterLink2',  '08A000000003801001','05DC4000A800', '050010000000', '05DC4004F800', '0400000000', '0114',    '0132',       '039F3704',    '0F9F02065F2A029A039C0195059F3704', '02008C',  '059999999999', '0400000100', '0152',  '0100');",
		//UPI
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'UPI_Debit',       'A000000333010101',  '05D84040A800', '050000000000', '05DC4004F800', '0400000000', '0199',    '0199',       '039F3704',    '00',                               '020020',  '059999999999', '0400000000',  '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'UPI_Credit',      'A000000333010102',  '05D84040A800', '050000000000', '05D84004F800', '0400000000', '0199',    '0199',       '039F3704',    '00',                               '020020',  '059999999999', '0400000000',  '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'UPI_QuasiCredit', 'A000000333010103',  '05D84040A800', '050000000000', '05D84004F800', '0400000000', '0199',    '0199',       '039F3704',    '00',                               '020020',  '059999999999', '0400000000',  '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'UPI_ECash',       'A000000333010106',  '05D84000A800', '050000000000', '05D84004F800', '0400000000', '0199',    '0199',       '039F3704',    '00',                               '020020',  '059999999999', '0400000000',  '0152',  '0100');",
		"INSERT INTO aid (emvAidName, emvAid, emvTACDft, emvTACDen, emvTACOnl, emvThrVal, emvTarPer, emvMaxTarPer, emvDftValDDOL, emvDftValTDOL, emvTrmAvn, emvAcqId, emvTrmFlrLim, emvTCC, emvAidTxnType) VALUES ( 'UPI',             'A000000333010108',  '05D84000A800', '050000000000', '05D84004F800', '0400000000', '0199',    '0199',       '039F3704',    '00',                               '020020',  '059999999999', '0400000000',  '0152',  '0100');",
};

static void refreshDBName(void){

	memset(DataBaseName, 0, sizeof(DataBaseName));
	strcpy(DataBaseName, DISK_PATH);
	strncat(DataBaseName, "/TSLDb",6);
	strncat(DataBaseName, _ING_APPLI_TELIUM_TETRA_PACKAGE_VERSION, 2);
	strncat(DataBaseName, &_ING_APPLI_TELIUM_TETRA_PACKAGE_VERSION[2], 2);
	strncat(DataBaseName, &_ING_APPLI_TELIUM_TETRA_PACKAGE_VERSION[4], 2);

}

// Insert mnu Data
static const char *tabInsert_Menu[] = {
		/// NB.: You must edit menus in the function "Generate_Menu_Content()" and in "resetMenuVars()"
		var_MnuCustomer,
		var_MnuSale,
		var_MnuSaleCB,
		var_MnuDeposit,
		var_MnuWithdrawal,
		var_MnuPreaut,
		var_MnuCompletion,
		var_MnuBalanceInquiry,
		var_MnuMiniSatement,
		var_MnuRefund,
		var_MnuBiller,
		var_MnuOffline,
		var_MnuVoid,
		var_MnuAdjust,
		var_MnuReversal,
		var_MnuLogOn,
		var_MnuEchoTest,
		var_MnuCustSettlement,

		var_MnuMerchant,
		var_MnuSettlement,
		var_MnuDetailedRpt,
		var_MnuSummaryRpt,
		var_MnuDuplicateReceipt,
		var_MnuReprintReceipt,
		var_MnuAdmChgPwd,
		var_MnuMrcChgPwd,
		var_MnuMrcReset,
		var_MnuMrcResetRev,
		var_MnuMngUsers,
		var_MnuBillerResend,

		var_MnuAgent,
		var_MnuAgencyDeposit,

		var_MnuSupervisor,

		var_MnuAdmin,
		var_MnuTerminalMode,
		var_MnuTerminalPar,
		var_MnuBillerConfig,
		var_MnuTMKey,
		var_MnuGenerateTLSkey,
		var_MnuCmmIS,
		var_MnuClessModeOff,
		var_MnuConnMode,
		var_MnuControlPanel,
		var_MnuSwapSimSlot,
		var_MnuCvmMode,
		var_MnuTraces,
};



/****************************************************************************
 *		DiskMount()
 *
 *		Mount a disk
 *		Create if not exist
 * --------------------------------------------------------------------------
 * @param intput:	char 	*volume			Disk name
 * @param intput:	int 	maxNbrFiles
 * @param output:
 * @return:   	SQL_OK or Error
 ****************************************************************************/
static int DiskMount( char * volume, int maxNbrFiles ) {
	S_FS_PARAM_CREATE cfg;
	unsigned long size;             // Not a doubleword but unsigned long inside prototype
	int iRet;

	if (strcmp(DISK_PATH, DISK_HOST) == 0) {
		return SQLITE_OK;
	}
	strcpy(cfg.Label, volume+1);                        		// Disk name
	cfg.Mode 	     = FS_WRITEONCE;							// Disk on Flash
	cfg.AccessMode	 = FS_WRTMOD;                               // r/w access
	cfg.NbFichierMax = maxNbrFiles;                          	// Max files number
	cfg.IdentZone	 = FS_WO_ZONE_DATA;                         // Zone id
	size			 = cfg.NbFichierMax * 32768;                // Disk size in bytes
	iRet = FS_mount (volume, &cfg.Mode);
	if (iRet != FS_OK) {
		iRet = FS_dskcreate(&cfg, &size);
	}
	return (iRet == FS_OK) ? SQLITE_OK : SQLITE_ERROR;
}


/*
 ** This is the callback routine that the SQLite library
 ** invokes for each row of a query result.
 */

/**
 * This is a callback routine that the SQLite library invoves for each row of a
 * \n query result.
 * \param    NotUsed:
 * \param    argc:int -(O)indicate the number of column in the one row
 * \param    argv:char** -(O) fetched data items for each column;
 * \param    azColName:char** -(O) indicate the names of each column in the one row
 */
static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int ii;
	int rowpr=argc-1;

	if (bCallBack) {
		bCallBack = 0;
		Telium_Fprintf (Telium_Stdprt(), " ***Inside Callback***\n");
	}
	NotUsed=0;
	for(ii = 0; ii < rowpr; ii++) {
		Telium_Fprintf(Telium_Stdprt(),"%s ",azColName[ii]);
	}
	Telium_Fprintf(Telium_Stdprt(),"%s\n",azColName[rowpr]);

	for(ii = 0; ii < rowpr; ii++) {
		Telium_Fprintf(Telium_Stdprt(),"%s ",  argv[ii] ? argv[ii] : "NULL");
	}
	Telium_Fprintf(Telium_Stdprt(),"%s\n",  argv[rowpr] ? argv[rowpr] : "NULL");

	return 0;
}

/*
 ** Return TRUE if the last non-whitespace character in z[] is a semicolon.
 ** z[] is N characters long.
 */
static int _ends_with_semicolon(const char *z, int N){
	while(N > 0 && isspace((unsigned char)z[N-1])) {
		N--;
	}
	return N > 0 && z[N-1] == ';';
}

/**
 * Open a new database handle with the given name.
 * \n If it is exist return with opened handle else create it with the given name.
 * \param    zDbFilename:char* (I) indicate database name.
 * \param    handle:sqlite3 ** (O) handle of the opened database.
 * \return 0:if success else return error code and error string
 *
 */
static int Sqlite_Open(char *fileName, sqlite3 **handle){
	int iRet;
	char cErrorMsg[100];

	if (strlen(fileName) < 16) {
		refreshDBName();
	}

	iRet = DiskMount(DISK_PATH, 16);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_open(fileName, handle);
	}
	if( SQLITE_OK != sqlite3_errcode(*handle)){
		Telium_Sprintf(cErrorMsg, "Unable to open database \"%s\": %s\n", fileName, sqlite3_errmsg(*handle));
		iRet =  SQLITE_CANTOPEN;
	} else
		Telium_Sprintf(cErrorMsg, "Database %s Opened : %d\n", fileName, iRet);

	//Telium_Fprintf(Telium_Stdprt(),cErrorMsg);
	return iRet;
}

/**
 * Close the opened database with the given handle.
 * \n If it is exist return with opened handle else create it with the given name.
 * \param    handle:sqlite3** (O) handle of the opened database.
 * \return 0:if success else return error code and error string
 */
int Sqlite_Close(sqlite3 *handle){
	int iRet;
	char cErrorMsg[50];

	iRet = sqlite3_close(handle);
	if (iRet != SQLITE_OK){
		Telium_Sprintf(cErrorMsg, "Unable to close database :%s\n", sqlite3_errmsg(handle));
	} else {
		Telium_Sprintf(cErrorMsg, "close database:%s\n", "OK");
		handle = NULL;
	}
	//Telium_Fprintf(Telium_Stdprt(),cErrorMsg);
	return iRet;
}

/**
 * \n Execute SQL code.  Return one of the SQLITE_ success/failure
 * \n codes.  Also write an error message into memory obtained from
 * \n malloc() and make *zErrMsg point to that message.
 * \n
 * \n If the SQL is a query, then for each row in the query result
 * \n the Callback() function is called.  pArg becomes the first
 * \n argument to Callback().  If Callback=NULL then no callback
 * \n is invoked, even for queries.
 */

/* \param    handle:sqlite3** (I) handle of the opened database.
 * \param    zSql:char* (I) queried string.
 * \return 0:if success else return >0
 */
int Sqlite_Exec(sqlite3 *handle, char *zSql){
	int nSql = 0;
	char zErrorMsg[100];
	char *zErrMsg = NULL;

	int iRet;
	int errCnt = SQLITE_OK;
	int b, c;

	nSql = strlen(zSql);
	b = _ends_with_semicolon(zSql, nSql);
	nSql = 0;
	if (b == 0){
		nSql = Telium_Sprintf(&zErrorMsg[nSql], "%s\n", "missing ; end of the statement");
		errCnt++;
	}
	c = sqlite3_complete(zSql);
	if (c == 0){
		Telium_Sprintf(&zErrorMsg[nSql], "%s\n", "Uncompleted  SQL statement");
		errCnt++;
	}
	if( zSql && b && c ){
		bCallBack = 1;
		iRet = sqlite3_exec(handle, zSql, callback, 0, &zErrMsg);
		bCallBack = 0;
		if( iRet || zErrMsg ){
			char zPrefix[100];
			Telium_Sprintf(zPrefix, "SQL error:");
			if( zErrMsg!=0 ){
				Telium_Sprintf(zErrorMsg, "%s %s\n", zPrefix, zErrMsg);
				sqlite3_free(zErrMsg);
				zErrMsg = 0;
			} else {
				Telium_Sprintf(zErrorMsg, "%s %s\n", zPrefix, sqlite3_errmsg(handle));
			}
			errCnt++;
		}
		zSql = 0;
		nSql = 0;
	}
	//	if (errCnt)
	//		Telium_Fprintf(Telium_Stdprt(), zErrorMsg);
	return errCnt;
}

/**
 * 	Open and create table
 */
static void SqliteApp_Create(void){
	int iRet;
	int ii;

	//Make sure DB Name is correct
	refreshDBName();

	//	iRet = Sqlite_Open("/TDISK/TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		// creating a multiple tables
		for (ii = 0; ii < sizeof(tabCreate) / sizeof(tabCreate[0]); ii++){
			Sqlite_Exec(handle, (char *)tabCreate[ii]);
		}
		Sqlite_Close(handle);
	}
}

/**
 * 	Insert Datas
 */
void SqliteApp_Insert(void){
	int iRet;
	int ii;

	//Make sure DB Name is correct
	refreshDBName();

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);

	if (iRet == SQLITE_OK){
		// Insert rows into table UsersMenu and SettingsMenu first method
		for (ii = 0; ii < sizeof(tabInsert) / sizeof(tabInsert[0]); ii++){
			Sqlite_Exec(handle, (char *)tabInsert[ii]);
		}
		// Insert rows into table AppMenus
		for (ii = 0; ii < sizeof(tabInsert_Menu) / sizeof(tabInsert_Menu[0]); ii++){
			Sqlite_Exec(handle, (char *)tabInsert_Menu[ii]);
		}

		Sqlite_Close(handle);
	}
}

/**
 * 	Run statement
 */
int Sqlite_Run_Statement(const char * statement,char * data){
	int iRet;
	int col;
	int cols;

	//Make sure DB Name is correct
	refreshDBName();

	// select query from the table UsersMenu

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)statement, -1, &stmt, 0);
		if (iRet){
			goto lblErr;
		}
		// Read the number of rows fetched
		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){
				// SQLITE_ROW means fetched a row
				// sqlite3_column_text returns a const void* , typecast it to const char*
				for (col = 0 ; col < cols; col++){
					const char *val = (const char*)sqlite3_column_text(stmt,col);
					strcpy(data,"#");
					strcpy(data,val);
				}
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
				break;
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);


	}
	return 1;
	lblErr:
	Sqlite_Close(handle);
	return -1;
}

/**
 * 	Run statement
 */
int Sqlite_Update_Parameter(const char * parameter,char * data){
	int iRet;
	char statement[100];

	memset(statement,0,sizeof(statement));

	//Create the Query
	Telium_Sprintf (statement, "UPDATE parameters SET details = '%s' where paramName = '%s';", data, parameter);

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		Sqlite_Exec(handle, (char *)statement);
	}
	Sqlite_Close(handle);
	return 1;
}

/**
 * 	Run statement
 */
int Sqlite_Get_Parameter(const char * parameter,char * data){
	int iRet;
	//	int cols;
	char statement[300];

	memset(statement,0,sizeof(statement));

	//Create the Query
	Telium_Sprintf (statement, "SELECT * FROM parameters WHERE paramName = '%s';", parameter);

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)statement, -1, &stmt, 0);
		if (iRet){
			Sqlite_Close(handle);
			return -1;
		}
		//		// Read the number of rows fetched
		//		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){
				// SQLITE_ROW means fetched a row
				const char *val = (const char*)sqlite3_column_text(stmt,2);
				strcpy(data,val);
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);

	}
	return 1;
}


/**
 * 	Run statement
 */
int Sqlite_Get_Menu(const char * parentID,char * data){
	int iRet;
	int cols;
	int col;
	char statement[1024];
	int returnVal = 0;
	char columnName[100];

	memset(statement,0,sizeof(statement));

	//Create the Query
	Telium_Sprintf (statement, "SELECT MenuId,MenuName,SecureMenu,SecureMenuLevel,IconPathName FROM AppMenus WHERE Hidden = '0' and MenuIdParent = '%s';", parentID);

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)statement, -1, &stmt, 0);
		if (iRet){
			Sqlite_Close(handle);
			return -1;
		}
		// Read the number of rows fetched
		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){
				// SQLITE_ROW means fetched a row
				// sqlite3_column_text returns a const void* , typecast it to const char*
				for (col = 0 ; col < cols; col++) {
					memset(columnName, 0, sizeof(columnName));
					const char *val = (const char*)sqlite3_column_text(stmt, col);
					strcpy(columnName,sqlite3_column_name(stmt,col));
					strcat(data,columnName);
					strcat(data, ",");
					strcat(data, val);
					strcat(data, ";");

				}
				strcat(data, "#");
				returnVal++;
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);

	}
	return returnVal;
}



/**
 * 	Run statement for users
 */
int Sqlite_Get_UserDetails(int UserID,char * data){
	int iRet;
	int cols;
	int col;
	char statement[1024];
	int returnVal = 0;
	char columnName[100];

	memset(statement,0,sizeof(statement));

	if (UserID == 0) {
		//Create the Query
		Telium_Sprintf (statement, "SELECT id, userName, password  FROM Users;");
	} else {
		//Create the Query
		Telium_Sprintf (statement, "SELECT userName, password  FROM Users WHERE id = '%d';", UserID);
	}

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)statement, -1, &stmt, 0);
		if (iRet){
			Sqlite_Close(handle);
			return -1;
		}
		// Read the number of rows fetched
		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){
				// SQLITE_ROW means fetched a row
				// sqlite3_column_text returns a const void* , typecast it to const char*
				for (col = 0 ; col < cols; col++) {
					memset(columnName, 0, sizeof(columnName));
					const char *val = (const char*)sqlite3_column_text(stmt, col);
					strcpy(columnName,sqlite3_column_name(stmt,col));
					strcat(data,columnName);
					strcat(data, ",");
					strcat(data, val);
					strcat(data, ";");

				}
				strcat(data, "#");
				returnVal++;
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);

	}
	return returnVal;
}




/**
 * 	Run statement
 */
int Sqlite_Run_Statement_MultiRecord(const char * SqlStatement,char * data){
	int iRet;
	int cols;
	int col;
	char statement[1024];
	int returnVal = 0;
	char columnName[100];

	memset(statement,0,sizeof(statement));

	//Make sure DB Name is correct
	refreshDBName();

	//Create the Query
	Telium_Sprintf (statement, "%s", SqlStatement);

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)statement, -1, &stmt, 0);
		if (iRet){
			Sqlite_Close(handle);
			return -1;
		}
		// Read the number of rows fetched
		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){

				if(returnVal>0)
					strcat(data, "#");

				// SQLITE_ROW means fetched a row
				// sqlite3_column_text returns a const void* , typecast it to const char*
				for (col = 0 ; col < cols; col++) {
					memset(columnName, 0, sizeof(columnName));
					const char *val = (const char*)sqlite3_column_text(stmt, col);
					strcpy(columnName,sqlite3_column_name(stmt,col));

					if (cols > 1) {
						strcat(data,columnName);
						strcat(data, ",");
					}
					if(val != NULL)
						strcat(data, val);
					if (cols > 1)
						strcat(data, ";");

				}
				returnVal++;
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);

	}
	return returnVal;
}



/**
 * 	Run statement
 */
int Sqlite_Run_Statement_MultiRecord_NoColumnName(const char * SqlStatement,char * data){
	int iRet;
	int cols;
	int col;
	char statement[1024];
	int returnVal = 0;
	char columnName[100];

	memset(statement,0,sizeof(statement));

	//Create the Query
	Telium_Sprintf (statement, "%s", SqlStatement);

	//Make sure DB Name is correct
	refreshDBName();

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)statement, -1, &stmt, 0);
		if (iRet){
			Sqlite_Close(handle);
			return -1;
		}
		// Read the number of rows fetched
		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){

				if(returnVal>0)
					strcat(data, ";");

				// SQLITE_ROW means fetched a row
				// sqlite3_column_text returns a const void* , typecast it to const char*
				for (col = 0 ; col < cols; col++) {
					memset(columnName, 0, sizeof(columnName));
					const char *val = (const char*)sqlite3_column_text(stmt, col);

					if(val != NULL)
						strcat(data, val);
					if (cols > 1)
						strcat(data, ",");
				}
				returnVal++;
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);

	}
	return returnVal;
}

/**
 *	Delete Database
 *
 */
int SqliteApp_DropDataBase(void) {
	int iRet;

	iRet = DiskMount(DISK_PATH, 16);
	if (iRet == SQLITE_OK) {
		//		iRet = FS_unlink(DISK_PATH"/""TSLDb");
		iRet = FS_unlink(DataBaseName);
	}
	FS_unmount(DISK_PATH);
	FS_dskkill(DISK_PATH);
	return iRet;
}

/**
 *	Copies database to the HOST directory in the terminal.
 *
 */
int SqliteApp_CopyFileToHost(void) {
	int iRet;
	int nbBytes;
	unsigned int mode;
	S_FS_FILE *hSrcFile = NULL;
	S_FS_FILE *hDestFile = NULL;
	char *bufFile = NULL;

	iRet = DiskMount(DISK_PATH, 16);
	if (iRet == SQLITE_OK) {
		//		hSrcFile = FS_open(DISK_PATH"/""TSLDb", "r");
		hSrcFile = FS_open(DataBaseName, "r");

		if (hSrcFile == NULL) {
			iRet = -1;
		}
	}
	if (iRet == SQLITE_OK) {
		if (FS_mount(DISK_HOST, &mode) != FS_OK) {
			iRet = -1;
		}
	}
	if (iRet == SQLITE_OK) {

		//		FS_unlink(DISK_HOST"/""TSLDb");
		FS_unlink((const char *)refreshDBName);
		//		hDestFile = FS_open(DISK_HOST"/""TSLDb", "a");
		hDestFile = FS_open((const char *)refreshDBName, "a");
		if (hDestFile == NULL) {
			iRet = -1;
		}
	}

	if (iRet == SQLITE_OK) {
		nbBytes = FS_length (hSrcFile);
		if ((nbBytes > 0) && (nbBytes != FS_ERROR)) {
			bufFile = (char *)umalloc(nbBytes);
			if (bufFile != NULL) {
				if (nbBytes == FS_read(bufFile, 1, nbBytes, hSrcFile)) {
					if (FS_write(bufFile, 1, nbBytes, hDestFile) < nbBytes) {
						iRet = -1;
					}
				}
			}
		}
	}
	if (hDestFile != NULL) {
		FS_close(hDestFile);
	}
	if (hSrcFile != NULL) {
		FS_close(hSrcFile);
	}
	if (bufFile != NULL) {
		ufree(bufFile);
	}
	return iRet;
}

int SqliteDB_Init(void){
	int ret = 0;

	refreshDBName();
	refreshDBName();//Make sure it is cleaned

	//make sure the DB is not there
	ret = SqliteApp_DropDataBase();
	ret = SqliteApp_DropDataBase();

	// Create the database
	SqliteApp_Create();

	//prepare App data
	Generate_Menu_Content();

	//Insert temporary data
	SqliteApp_Insert();

	return ret;
}

static void Sqlite_SaveTo_tra(char * columnName, char * DataToSave){
	//	card temp_Word = 0;
	int ret = 0;

	if (strncmp(columnName,"MenuItem",8) == 0) {
		//		dec2num(&temp_Word, DataToSave,0);
		//		mapPutWord(traMnuItmContext, DataToSave);
		MAPPUTSTR(traMnuItmContext, DataToSave, lblKO);
	}

	//Invoice Number
	if (strncmp(columnName,"InvoiceNo",9) == 0) {
		MAPPUTSTR(traInvNum, DataToSave, lblKO);
	}

	//FIELD 2 (PAN)
	else if (strcmp(columnName,"isoField002") == 0) {
		MAPPUTSTR(traPan, DataToSave, lblKO);
	}
	//FIELD 3 (PROCESSING CODE)
	else if (strcmp(columnName,"isoField003") == 0) {
		MAPPUTSTR(traRqsProcessingCode, DataToSave, lblKO);
	}
	//FIELD 4 (AMOUNT)
	else if (strcmp(columnName,"isoField004") == 0) {
		MAPPUTSTR(traAmt, DataToSave, lblKO);
	}
	//FIELD 5
	//FIELD 6
	//FIELD 7 (DATE AND TIME)
	else if (strcmp(columnName,"isoField007") == 0) {
		MAPPUTSTR(traDatTim, DataToSave, lblKO);
	}
	//FIELD 8
	//FIELD 9
	//FIELD 10
	//FIELD 11 ('STAN' SYSTEM TRACE AUDIT NUMBER)
	else if (strcmp(columnName,"isoField011") == 0) {
		MAPPUTSTR(traSTAN, DataToSave, lblKO);
	}
	//FIELD 12
	//FIELD 13
	//FIELD 14 (EXPIRY DATE)
	else if (strcmp(columnName,"isoField014") == 0) {
		MAPPUTSTR(traExpDat, DataToSave, lblKO);
	}
	//FIELD 15
	//FIELD 16
	//FIELD 17
	//FIELD 18
	//FIELD 19
	//FIELD 20
	//FIELD 21
	//FIELD 22 (POS ENT MODE)
	else if (strcmp(columnName,"isoField022") == 0) {
		MAPPUTSTR(traPosEntMod, DataToSave, lblKO);
	}
	//FIELD 23 (CARD SEQUENCE NUMBER)
	else if (strcmp(columnName,"isoField023") == 0) {
		MAPPUTSTR(traCrdSeq, DataToSave, lblKO);
	}
	//////FIELD 24 (CARD SEQUENCE NUMBER)
	//	else if (strcmp(columnName,"isoField024") == 0) {
	//		MAPPUTSTR(appNII, DataToSave, lblKO);
	//	}
	//FIELD 25 (COND CODE)
	else if (strcmp(columnName,"isoField025") == 0) {
		MAPPUTBYTE(traConCode, DataToSave[0], lblKO);
	}
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
	else if (strcmp(columnName,"isoField035") == 0) {
		MAPPUTSTR(traTrk2, DataToSave, lblKO);
	}
	//FIELD 36
	//FIELD 37 (REFERENCE RETRIEVAL NUMBER)
	else if (strcmp(columnName,"isoField037") == 0) {
		MAPPUTSTR(traRrn, DataToSave, lblKO);
	}
	//FIELD 38 (APPROVAL CODE)
	else if (strcmp(columnName,"isoField038") == 0) {
		MAPPUTSTR(traAutCod, DataToSave, lblKO);
	}
	//FIELD 39 (RESPONSE CODE)
	else if (strcmp(columnName,"isoField039") == 0) {
		MAPPUTSTR(traRspCod, DataToSave, lblKO);
	}
	//FIELD 40
	//////FIELD 41 (TERMINAL ID)
	//	else if (strcmp(columnName,"isoField041") == 0) {
	//		MAPPUTSTR(appTID, DataToSave, lblKO);
	//	}
	//////FIELD 42 (MERCHANT ID)
	//	else if (strcmp(columnName,"isoField042") == 0) {
	//		MAPPUTSTR(appMID, DataToSave, lblKO);
	//	}
	//	//FIELD 43
	//	//FIELD 44
	//	//FIELD 45
	//	//FIELD 46
	//	//FIELD 47
	//	//FIELD 48
	////FIELD 49 (CURRENCY CODE)
	else if (strcmp(columnName,"isoField049") == 0) {
		ApplicationCurrencyFillAuto(&DataToSave[2]);
		//			MAPPUTSTR(emvTrnCurCod, DataToSave, lblKO);
	}
	//FIELD 50
	//FIELD 51
	//FIELD 52
	//FIELD 53
	//FIELD 54
	else if (strcmp(columnName,"isoField054") == 0) {
		MAPPUTSTR(traCashbackAmt, DataToSave, lblKO);
	}

	//FIELD 55
	else if (strcmp(columnName,"isoField055") == 0) {
		MAPGET(traEMVDATA, isoField055,lblKO); // DATA was fed into field by Transaction earlier
	}

	//FIELD 56
	//FIELD 57
	//FIELD 58
	//FIELD 59
	//FIELD 60
	//FIELD 61
	//FIELD 62
	else if (strcmp(columnName,"isoField062") == 0) {
		MAPPUTSTR(traBillerPaymentDetails, DataToSave, lblKO);
	}
	//FIELD 63
	else if (strcmp(columnName,"isoField063") == 0) {
		MAPPUTSTR(traField063, DataToSave, lblKO);
	}
	//FIELD 64
	lblKO:;
}

int sqlite_CloseVoid(char * STAN){

	int iRet;
	char statement[100];

	memset(statement,0,sizeof(statement));

	//Create the Query
	Telium_Sprintf (statement, "UPDATE log SET isoVoided = '1' WHERE isoField011 = '%s';", STAN);

	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		Sqlite_Exec(handle, (char *)statement);
	}
	Sqlite_Close(handle);
	return 1;


}

int sqlite_Get_LOG_Record(word RRN,word APPRVCODE,word STAN){
	int ret = 0;
	int whereClause = 0;
	int iRet;
	int cols;
	int col;
	char PreparedStatement[300];
	char Statement[300];
	int returnVal = 0;
	char columnName[256];
	char data[256];
	char RRN_Val[lenRrn + 1];
	char APPRVCODE_Val[lenAutCod + 1];
	char STAN_Val[lenSTAN + 3];

	memset(RRN_Val,0,sizeof(RRN_Val));
	memset(STAN_Val,0,sizeof(STAN_Val));
	memset(Statement,0,sizeof(Statement));
	memset(APPRVCODE_Val,0,sizeof(APPRVCODE_Val));
	memset(PreparedStatement,0,sizeof(PreparedStatement));

	//Create the query
	strcpy(Statement,"SELECT * FROM log WHERE ");

	if (RRN > 0){
		MAPGET(RRN, RRN_Val, lblKO);
		strcat(Statement,"isoField037 = '");
		strcat(Statement,RRN_Val);
		strcat(Statement,"' ");
		whereClause = 1;
	}

	if (APPRVCODE > 0){
		MAPGET(RRN, APPRVCODE_Val, lblKO);
		if (whereClause == 1)
			strcat(Statement,"AND ");
		strcat(Statement,"isoField038 = '");
		strcat(Statement,APPRVCODE_Val);
		strcat(Statement,"' ");
		whereClause = 1;
	}

	if (STAN > 0){
		MAPGET(STAN, STAN_Val, lblKO);
		if (whereClause == 1)
			strcat(Statement,"AND ");
		strcat(Statement,"isoField011 = '");
		strcat(Statement,STAN_Val);
		strcat(Statement,"' ");
		whereClause = 1;
	}

	if (whereClause == 0) {
		//Telium_Sprintf(PreparedStatement, "id = (SELECT MAX(id) FROM log)");
		Telium_Sprintf(PreparedStatement, "MenuItem != '%d' AND MenuItem != '%d' AND MenuItem != '%d' AND isoVoided != '1' ORDER BY id DESC LIMIT 1", mnuBalanceEnquiry, mnuVoid, mnuReversal);
		strcat(Statement,PreparedStatement);
	}else{
		Telium_Sprintf(PreparedStatement, "AND isoVoided != '1' AND MenuItem != '%d' AND MenuItem != '%d' ORDER BY id DESC LIMIT 1 ", mnuVoid, mnuReversal);
		strcat(Statement,PreparedStatement);
		//strcat(Statement,"AND isoVoided != '1' ");
	}
	strcat(Statement,";");

	//finish creating query
	ret = -1;
	//	iRet = Sqlite_Open(DISK_PATH"/""TSLDb", &handle);
	iRet = Sqlite_Open(DataBaseName, &handle);
	if (iRet == SQLITE_OK){
		iRet = sqlite3_prepare_v2(handle, (char *)Statement, -1, &stmt, 0);
		if (iRet){
			Sqlite_Close(handle);
			return -1;
		}
		// Read the number of rows fetched
		cols = sqlite3_column_count(stmt);
		while(1){
			// fetch a row's status
			iRet = sqlite3_step(stmt);

			if (iRet == SQLITE_ROW){


				// SQLITE_ROW means fetched a row
				// sqlite3_column_text returns a const void* , typecast it to const char*
				for (col = 0 ; col < cols; col++) {
					memset(data, 0, sizeof(data));
					memset(columnName, 0, sizeof(columnName));
					const char *val = (const char*)sqlite3_column_text(stmt, col);
					strcpy(columnName,sqlite3_column_name(stmt,col));
					strcat(data, val);

					Sqlite_SaveTo_tra(columnName, data);
					ret = 1;
				}
				returnVal++;
			} else if (iRet == SQLITE_DONE) {
				// All rows finished
				break;
			} else {
				// Some error encountered
			}
		}
		sqlite3_finalize(stmt);
		Sqlite_Close(handle);

	}


	return ret;
	lblKO:
	return -1;
}
