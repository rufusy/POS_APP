
#include <globals.h>
#include "Sqlite_Def.h"
#include "Sqlite.h"
#include "EMV_Manager.h"

int EMV_Manager_get_num_AID(void){
	int ret = 0;
	int NumOfRecords = -99;
	char Statement[512];
	char DataResponse[256];

	memset(DataResponse, 0, sizeof(DataResponse));
	memset(Statement, 0, sizeof(Statement));

	strcpy(Statement,"SELECT COUNT(*) FROM aid;");
	ret = Sqlite_Run_Statement(Statement,DataResponse);
	CHECK(ret > 0,lblKO);
	NumOfRecords = atoi(DataResponse) + 1;

	goto lblEnd;

	lblKO:
	NumOfRecords = -99;

	lblEnd:
	return NumOfRecords;
}
