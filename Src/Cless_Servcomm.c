// ========================================================================	/
// Project       : -                                                        /
// Module        : COMM                                                     /
// Creation Date : 08/12/2004                                               /
// Author        : M. GATESOUPE                                             /
// Description   : This module contains the interface with                  /
//                 a communication component.                               /
// ========================================================================	/
// SAGEM Monetel                                                            /
// 1 rue Claude Chappe - BP 346                                             /
// 07503 Guilherand-Granges                                                 /
// ========================================================================	/
// REVISION HISTORY                                                         /
// ========================================================================	/
// Date          :                                                          /
// Author        :                                                          /
// Description   :                                                          /
// ========================================================================	/

/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"
#include "Globals.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

typedef struct
{
	unsigned short type_code;	/**< Code type associated with EMVDC Component (96) */
	unsigned short service;		/**< Service to be called */
	int    function;			/**< Step to be called, in case of call to EMVDC_process_step service */
	_DEL_  DEL;					/**< Input / Output DEL.	*/
} CommmStructProcessStep;

/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

#ifdef __TELIUM3__
static unsigned short g_usCommAppType = 0x57aa;
#else
static unsigned short g_usCommAppType = I_COMM_DEFAULT_TYPE_APPLI;
#endif
static CommmStructProcessStep g_ProcessStep;
static DEL g_Del;


/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////

static void COMM_CopyInput(const DEL* aDEL, _DEL_* _vDEL);
static void COMM_CopyOutput(_DEL_* _vDEL, DEL* aDEL);
static DEL* COMM_Call_Service(unsigned short usAppType, unsigned short usServiceID,
		unsigned short usInfoAppType, unsigned short usInfoService,
		int nInfoFunction, const DEL* pInputDel,
		CommmStructProcessStep* pProcessStep, DEL* pOutputDel);
static DEL* COMM_Service(unsigned short usServiceID, const DEL* pInputDel);


//// Functions //////////////////////////////////////////////////

/*void _DEL_init(_DEL_ *aDEL)
{
	int i;

    aDEL->current = 0;
	aDEL->count   = 0;
	for (i=0; i<MAX_DATA_ELEMENT; i++)
	{
      aDEL->list[i].tag = 0;
      aDEL->list[i].length = 0;
      memclr(aDEL->list[i].ptValue,sizeof(aDEL->list[i].ptValue));
	}
}*/


/*void CopyEntree(DEL *aDEL, _DEL_ *_vDEL)
{
	int i;

    _DEL_init(_vDEL);
	if (aDEL!=NULL)
	{
      _vDEL->current = aDEL->current;
	  _vDEL->count   = aDEL->count;
	  for (i=0; i<aDEL->count; i++)
	  {
        _vDEL->list[i].tag    = aDEL->list[i].tag;
        _vDEL->list[i].length = aDEL->list[i].length;
	    if (aDEL->list[i].ptValue!=NULL)
		{
          memcpy(_vDEL->list[i].ptValue,aDEL->list[i].ptValue,aDEL->list[i].length);
		}
	  }
	}
}*/


/*void CopySortie(_DEL_ *_vDEL,DEL *aDEL)
{
	int i;

    DEL_init(aDEL);
    aDEL->current = _vDEL->current;
	aDEL->count   = _vDEL->count;
	for (i=0; i<_vDEL->count; i++)
	{
      aDEL->list[i].tag     = _vDEL->list[i].tag;
      aDEL->list[i].length  = _vDEL->list[i].length;
      aDEL->list[i].ptValue = _vDEL->list[i].ptValue;
	}
}*/


void COMM_CopyInput(const DEL* aDEL, _DEL_* _vDEL)
{
	CopyEntree((DEL*)aDEL, _vDEL);
}

void COMM_CopyOutput(_DEL_* _vDEL, DEL* aDEL)
{
	CopySortie(_vDEL, aDEL);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

static DEL* COMM_Call_Service(unsigned short usAppType, unsigned short usServiceID,
		unsigned short usInfoAppType, unsigned short usInfoService,
		int nInfoFunction, const DEL* pInputDel,
		CommmStructProcessStep* pProcessStep, DEL* pOutputDel)
{
	unsigned char ucPriority;
	int nResult;

	// Does the service exist ?
	if (Telium_ServiceGet(usAppType, usServiceID, &ucPriority) == 0)
	{
		if (pInputDel != NULL)
		{
			// Copy the input DEL to a global _DEL_ object
			COMM_CopyInput(pInputDel, &pProcessStep->DEL);
		}
		else
		{
			// Empty the global _DEL_ object
			_DEL_init(&pProcessStep->DEL);
		}

		// Call the service
		pProcessStep->type_code = usInfoAppType;
		pProcessStep->service = usInfoService;
		pProcessStep->function = nInfoFunction;
		Telium_ServiceCall(usAppType, usServiceID, sizeof(CommmStructProcessStep), pProcessStep, &nResult);

		if (pOutputDel != NULL)
		{
			// Copy the filled local _DEL_ object into a global DEL object
			COMM_CopyOutput(&pProcessStep->DEL, pOutputDel);
		}

		return (pOutputDel);
	}
	else
	{
		// The service does not exist
		return (NULL);
	}
}

static DEL* COMM_Service(unsigned short usServiceID, const DEL* pInputDel)
{
	DEL* pOutputDel;

	// Call the communication component
	pOutputDel = COMM_Call_Service(g_usCommAppType, usServiceID, g_usCommAppType, usServiceID,
			0, pInputDel, &g_ProcessStep, &g_Del);

#ifdef _TRACK_IAC_
	// Send the input and output DEL to the SPY tool
	TrackIAC(g_usCommAppType, usServiceID, 0, pInputDel, pOutputDel);
#endif

	return (pOutputDel);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

void COMM_Set_Application_Type(unsigned short usAppType)
{
	g_usCommAppType = usAppType;
}

unsigned short COMM_Get_Application_Type(void)
{
	return (g_usCommAppType);
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

DEL* COMM_Initiate_Online_Transaction(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_INITIATE_ONLINE_TRANSACTION, pInputDel);
}

DEL* COMM_Authorisation_Request(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_AUTHORISATION_REQUEST, pInputDel);
}

DEL* COMM_Send_Online_Advice(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_SEND_ONLINE_ADVICE, pInputDel);
}

DEL* COMM_Reversal(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_REVERSAL, pInputDel);
}

DEL* COMM_Send_Financial_Transaction_Request(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_SEND_FINANCIAL_TRANSACTION_REQUEST, pInputDel);
}

DEL* COMM_Financial_Transaction_Confirmation(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_FINANCIAL_TRANSACTION_CONFIRMATION, pInputDel);
}

DEL* COMM_Terminate_Online_Transaction(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_TERMINATE_ONLINE_TRANSACTION, pInputDel);
}

DEL* COMM_Initiate_Batch_Data_Capture(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_INITIATE_BATCH_DATA_CAPTURE, pInputDel);
}

DEL* COMM_Send_Batch_Data(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_SEND_BATCH_DATA, pInputDel);
}

DEL* COMM_Terminate_Batch_Data_Capture(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_TERMINATE_BATCH_DATA_CAPTURE, pInputDel);
}

DEL* COMM_Perform_Reconciliation(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_PERFORM_RECONCILIATION, pInputDel);
}

DEL* COMM_Get_Black_List(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_GET_BLACK_LIST, pInputDel);
}

DEL* COMM_PIN_Online_Request(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_PIN_ONLINE_REQUEST, pInputDel);
}

/////////////////////////////////////////////////////////////////

DEL* COMM_Get_Protocol_Info(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_GET_PROTOCOL_INFO, pInputDel);
}

DEL* COMM_Configure(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_CONFIGURE, pInputDel);
}

DEL* COMM_Connect(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_CONNECT, pInputDel);
}

DEL* COMM_Disconnect(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_DISCONNECT, pInputDel);
}

DEL* COMM_Send_And_Receive_Message(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_SEND_AND_RECEIVE_MESSAGE, pInputDel);
}

DEL* COMM_Send_Message(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_SEND_MESSAGE, pInputDel);
}

DEL* COMM_Get_Status(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_GET_STATUS, pInputDel);
}

DEL* COMM_Read_Message(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_READ_MESSAGE, pInputDel);
}

DEL* COMM_Abort(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_ABORT, pInputDel);
}

DEL* COMM_Receive_Message(const DEL* pInputDel)
{
	return COMM_Service(I_COMM_RECEIVE_MESSAGE, pInputDel);
}



/**
 * Call the authorisation host.
 * @param[in,out] dataStruct Data to be sent to the host for online authorisation (input) and response tags are added to it (output).
 * @return
 *    - \a TRUE if correctly performed.
 *    - \a FALSE if an error occurred.
 */
int __CLESS_CallAuthorisationHost(T_SHARED_DATA_STRUCT* dataStruct) {
	int IRet = FALSE;
#ifndef __TEST_VERSION__
	int ret = FALSE;
	char EMV_AuthData[(lenIssAutDta * 2) + 3];
	char IssuerScript2[(lenIssSc2 * 2) + 3];
	char rspcode[lenRspCod + 1];
	byte BinData[(lenIssSc2 * 2) + 3];


	memset(EMV_AuthData, 0, sizeof(EMV_AuthData));
	memset(IssuerScript2, 0, sizeof(IssuerScript2));
	memset(rspcode, 0, sizeof(rspcode));

	// First Fill the tags with data
	CLESS_Data_Save_To_DB(dataStruct);

	//Do online Authorization
	IRet = performOlineTransaction();
#endif

	IRet = TRUE; //Force true so that a recipt is printed

#ifdef __TEST_VERSION__
	// Here we simulate an accepted authorisation with Issuer Script Processing
	GTL_SharedExchange_AddTag(dataStruct,TAG_EMV_ISSUER_AUTHENTICATION_DATA, 10,
			(unsigned char*) "\x11\x22\x33\x44\x55\x66\x77\x88\x30\x30");
	GTL_SharedExchange_AddTag(dataStruct,TAG_EMV_ISSUER_SCRIPT_TEMPLATE_2, 33,
			(unsigned char*) "\x9F\x18\x04\x11\x22\x33\x44\x86\x0D\x84\x24\x00\x00\x08\xAA\xBB\xCC\xDD\x11\x22\x33\x44\x86\x09\x84\x24\x00\x00\x04\x55\x66\x77\x88");
	GTL_SharedExchange_AddTag(dataStruct, TAG_EMV_AUTHORISATION_RESPONSE_CODE, 2,
			(unsigned char*) "\x30\x30");

#else
	// Here we simulate an accepted authorisation with Issuer Script Processing
	memset(BinData, 0, sizeof(BinData));
	MAPGET(emvIssAutDta, EMV_AuthData, lblKO);
	ret = strlen(EMV_AuthData);
	if(ret > 2){
		hex2bin(BinData, EMV_AuthData, ret/2);
		GTL_SharedExchange_AddTag(dataStruct,TAG_EMV_ISSUER_AUTHENTICATION_DATA, ret, (unsigned char*) BinData);
	}

	memset(BinData, 0, sizeof(BinData));
	MAPGET(emvIssSc2, IssuerScript2, lblKO);
	ret = strlen(IssuerScript2);
	if(ret > 2){
		hex2bin(BinData, IssuerScript2, ret/2);
		GTL_SharedExchange_AddTag(dataStruct,TAG_EMV_ISSUER_SCRIPT_TEMPLATE_2, ret, (unsigned char*) BinData);
	}

	memset(BinData, 0, sizeof(BinData));
	MAPGET(traRspCod, rspcode, lblKO);
	ret = strlen(rspcode);
	if(ret > 1){
		hex2bin(BinData, rspcode, ret/2);
		GTL_SharedExchange_AddTag(dataStruct, TAG_EMV_AUTHORISATION_RESPONSE_CODE, 2, (unsigned char*) rspcode);
//		GTL_SharedExchange_AddTag(dataStruct, TAG_EMV_AUTHORISATION_RESPONSE_CODE, ret, (unsigned char*) BinData);
	}

	// First Fill the tags with data including the response
	CLESS_Data_Save_To_DB(dataStruct);

	lblKO:
#endif

	return IRet;
}


