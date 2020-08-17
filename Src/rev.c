#include <globals.h>

int rev_ReverseLastTxn(void){
	int ret = 0;
	byte revFlag = 0;

	MAPGETBYTE(appReversalFlag, revFlag, lblKO);

	if (revFlag == 1)
		return 1;
	else
		return 0;

	lblKO:
	return 0;
}

void revAutoReversal(void){
	char OldMenu[100];
	char OldResponse[lenRspCod + 1];
	int ret = 0;
	char menu[100];
	char Bitmap[100];

	memset(OldResponse, 0, sizeof(OldResponse));
	memset(OldMenu, 0, sizeof(OldMenu));
	memset(Bitmap, 0, sizeof(Bitmap));
	memset(menu, 0, sizeof(menu));

	MAPGET(traMnuItm, OldMenu, lblKO);

	//check if reversal needs to be done
	if (rev_ReverseLastTxn() == 1) {
		//Buildfield 60
		ApplicationBuildReversalData();

		MAPPUTBYTE(appAutoReversal, 1, lblKO);
		//Hold previous Txn Details
		MAPGET(traRspCod, OldResponse, lblKO);

		//Change the transaction menu
		num2dec(menu, mnuReversal, 0);
		MAPPUTSTR(traMnuItm, menu, lblKO);
		MAPPUTSTR(traRqsMTI, "020400",lblKO);
		MAPGET(traRqsBitMap, Bitmap, lblKO);
		if (strlen(Bitmap)<18) {
			strcpy(Bitmap, "08303805802CC80016");
		}
		MAPPUTSTR(traRqsBitMap, Bitmap,lblKO);

		//Send the Transaction online
		performOlineTransaction();

		//revert to old transaction details
		MAPPUTSTR(traMnuItm, OldMenu, lblKO);
		MAPPUTSTR(traRspCod, OldResponse, lblKO);

	}

	MAPPUTBYTE(appReversalFlag, 0, lblKO);
	MAPPUTBYTE(appAutoReversal, 0, lblKO);

	lblKO:;
}
