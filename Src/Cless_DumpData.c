/**
 * \author	Ingenico
 * \author	Copyright (c) 2008 Ingenico, rue claude Chappe,\n
 *			07503 Guilherand-Granges, France, All Rights Reserved.
 *
 * \author	Ingenico has intellectual property rights relating to the technology embodied \n
 *			in this software. In particular, and without limitation, these intellectual property rights may\n
 *			include one or more patents.\n
 *			This software is distributed under licenses restricting its use, copying, distribution, and\n
 *			and decompilation. No part of this software may be reproduced in any form by any means\n
 *			without prior written authorization of Ingenico.
 */


/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////

static Telium_File_t * gs_hOuputHandle;											/*!< File handle of the output driver. */
static unsigned char gs_ucCustomOutputId = CUSTOM_OUTPUT_PRINTER;		/*!< Default output driver. */


/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////

extern int IsPrinter(void);


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

//! \brief Initialise the default output mode (printer if a printer is present, none else).

void Cless_DumpData_InitOutput (void)
{
#ifdef __TELIUM3__
	gs_ucCustomOutputId = CUSTOM_OUTPUT_USB;
#else
	gs_ucCustomOutputId = ((IsPrinter()) ? CUSTOM_OUTPUT_PRINTER : CUSTOM_OUTPUT_NONE);
#endif
}



//! \brief Get the output driver handle.
//! \return	The handle of the output driver

Telium_File_t * Cless_DumpData_DumpGetOutputHandle (void)
{
	return (gs_hOuputHandle);
}



//! \brief Set the current driver type id.
//! \param[in] ucOutputId output driver id to select (\a CUSTOM_OUTPUT_COM0, \a CUSTOM_OUTPUT_PRINTER, \a CUSTOM_OUTPUT_NONE, \a CUSTOM_OUTPUT_USB, \a CUSTOM_OUTPUT_TRACE).

void Cless_DumpData_DumpSetOutputId (unsigned char ucOutputId)
{
	gs_ucCustomOutputId = ucOutputId;
}



//! \brief return output driver id.
//! \return The output driver type
//! 	- \a CUSTOM_OUTPUT_NONE No selected devide (dump functions have no effect).
//! 	- \a CUSTOM_OUTPUT_PRINTER Printer output device.
//! 	- \a CUSTOM_OUTPUT_COM0 COM0 output device.
//! 	- \a CUSTOM_OUTPUT_USB USB output device.
//! 	- \a CUSTOM_OUTPUT_TRACE Trace output device.

unsigned char Cless_DumpData_DumpGetOutputId (void)
{
	return (gs_ucCustomOutputId);
}



//! \brief Output line feed : "\n" or a "\r" according the output driver idenifier.

void Cless_DumpData_DumpNewLine(void) {
	switch (Cless_DumpData_DumpGetOutputId()) {
		// =========================================
		case (CUSTOM_OUTPUT_COM0):
		case (CUSTOM_OUTPUT_USB):
			Cless_DumpData ("\r\n");
			break;

		// =========================================
		case (CUSTOM_OUTPUT_PRINTER):
			Cless_DumpData ("\n");
			Telium_Ttestall (PRINTER, 0);
			break;

		// =========================================
		case (CUSTOM_OUTPUT_TRACE):
		default: // Nothing
			break;
	}
}



//! \brief Open current Output driver.
//! \return	
//!		- \ref TRUE if correctly opened.
//!		- \ref FALSE else.
//! \note If the driver is not correctly opened, the dumping is not performed even if requested.

int Cless_DumpData_DumpOpenOutputDriver (void) {
	if (gs_hOuputHandle != NULL) {
		Telium_Fclose (gs_hOuputHandle);
		gs_hOuputHandle = NULL;
	}

	switch (Cless_DumpData_DumpGetOutputId())
	{
	case (CUSTOM_OUTPUT_PRINTER):
		gs_hOuputHandle = Telium_Fopen("PRINTER", "w");
		break;

	case (CUSTOM_OUTPUT_COM0):
		gs_hOuputHandle = Telium_Fopen("COM0", "rw*");
#ifdef __TELIUM2__
		Telium_Format("COM0", 9600, 8, 1, NO_PARITY, EVEN, FALSE);
#endif
		break;

	case (CUSTOM_OUTPUT_USB):
		gs_hOuputHandle = Telium_Fopen("COM5", "rw*");
		//Telium_Format("COM5", 115200, 8, 1, NO_PARITY, EVEN, FALSE);
		break;

	case (CUSTOM_OUTPUT_TRACE): // No driver to open. Trace is allready managed
		gs_hOuputHandle = 0;
		return TRUE;
		break;
		
	default:
		break;
	}

	if (Cless_DumpData_DumpGetOutputHandle() != NULL)
		return TRUE;
	else
		return FALSE;
}



//! \brief Close current Output driver.

void Cless_DumpData_DumpCloseOutputDriver (void)
{
	if (gs_hOuputHandle)
		Telium_Fclose (gs_hOuputHandle);
	gs_hOuputHandle = NULL;
}



//! \brief Dump data on current output.
//!	\param[in] pString String to be dumped.
//! \param[in] ... parameters to trace

void Cless_DumpData (const char* pString, ...) {
	char String[300];
	va_list Params;

	va_start(Params, pString);
	Telium_Vsprintf(String, pString, Params);
	
	va_end(Params);

	switch (Cless_DumpData_DumpGetOutputId()) {
		// ================================================================================
		case (CUSTOM_OUTPUT_COM0):
		{
			if (Cless_DumpData_DumpGetOutputHandle() != NULL) {
				int i;
				int len;
				i = 0;
				len = strlen(String);
				do {
					i += Telium_Fwrite(String + i, 1, len - i, Cless_DumpData_DumpGetOutputHandle());
					if (i < len)
						Telium_Ttestall(0,1);
				} while(i < len);
				Telium_Ttestall (0,1);
			}
			break;
		}

		// ================================================================================
		case (CUSTOM_OUTPUT_USB): {
			if (Cless_DumpData_DumpGetOutputHandle() != NULL) {
				int i;
				int len;
				i = 0;
				len = strlen(String);
				do {
					i += Telium_Fwrite(String + i, 1, len - i, Cless_DumpData_DumpGetOutputHandle());
					if (i < len)
						Telium_Ttestall(0,1);
				} while(i < len);
			}
			break;
		}
		
		// ================================================================================
		case (CUSTOM_OUTPUT_PRINTER):
		{
			if (Cless_DumpData_DumpGetOutputHandle() != NULL)
				Telium_Pprintf (String);
			break;
		}

		// ================================================================================
		case (CUSTOM_OUTPUT_TRACE):
		{
		    GTL_Traces_TraceDebug("%s", String);
			break;
		}
		
		// ================================================================================
		default: // Nothing
			break;
	}
}


//! \brief Dump a shared exchange buffer content (each constructed tag is dumped with a indentation).
//!	\param[in] pDataStruct Shared exchange buffer to be dumped.
//!	\param[in] nIndent Shall be set to 0 at the first call (for indentation purpose).

void Cless_DumpData_DumpSharedBuffer (T_SHARED_DATA_STRUCT * pDataStruct, const int nIndent)

{
	int nIndex, nIndex2;
	int nPosition;
	int nResult;
	unsigned long ulReadTag;
	unsigned long ulReadLength;
	unsigned char * pValue;
	unsigned char bDriverOpened, bDriverAlreadyOpened;
	T_SHARED_DATA_STRUCT tTempStruct;
	int bFirstLinePrinted;

	nPosition = SHARED_EXCHANGE_POSITION_NULL;
	nResult = STATUS_SHARED_EXCHANGE_OK;

	if (Cless_DumpData_DumpGetOutputHandle() == NULL)
	{
		bDriverOpened = Cless_DumpData_DumpOpenOutputDriver();
		bDriverAlreadyOpened = FALSE;
	}
	else
	{
		bDriverOpened = TRUE;
		bDriverAlreadyOpened = TRUE;
	}

	if (bDriverOpened)
	{
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x0f");
		
		while (nResult == STATUS_SHARED_EXCHANGE_OK)
		{
			nResult = GTL_SharedExchange_GetNext(pDataStruct, &nPosition, &ulReadTag, &ulReadLength, (const unsigned char **)&pValue);

			if (nResult == STATUS_SHARED_EXCHANGE_OK)
			{
				for (nIndex=0;nIndex<nIndent;nIndex++)
					Cless_DumpData (" ");
				
				Cless_DumpData ("T:%08lx L:%04lx ", ulReadTag, ulReadLength);
				
				if (GTL_BerTlv_IsTagConstructedObject(ulReadTag))
				{
					Cless_DumpData_DumpNewLine();
					GTL_SharedExchange_InitEx(&tTempStruct, ulReadLength, ulReadLength, pValue);
					Cless_DumpData_DumpSharedBuffer (&tTempStruct, nIndent+1);
					Cless_DumpData_DumpNewLine();
				}
				else
				{
					nIndex = 0;
					bFirstLinePrinted = FALSE;

					if (ulReadLength > 0)
					{
						Cless_DumpData ("V:");
						while (nIndex < (int)ulReadLength)
						{
							nIndex2 = 0;
						
							while ((nIndex+nIndex2 < (int)ulReadLength) && (nIndex2 < (14 - nIndent + ((10 + nIndent) * (bFirstLinePrinted != FALSE)))))
							{
								Cless_DumpData("%02x", pValue[nIndex+nIndex2]);
								nIndex2++;
							}
							nIndex += nIndex2;
							Cless_DumpData_DumpNewLine();
							bFirstLinePrinted = TRUE;
						}
					}
					else
					{
						Cless_DumpData_DumpNewLine();
					}
				}
			}
		}
		
		if (!bDriverAlreadyOpened)
		{
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
				Cless_DumpData("\x1b" "@");
			Cless_DumpData_DumpCloseOutputDriver();
		}
	}
}



//! \brief Dump a shared exchange buffer content with a title.
//!	\param[in] pDataStruct Shared exchange buffer to be dumped.
//!	\param[in] pTitle Title of the dump.

void Cless_DumpData_DumpSharedBufferWithTitle (T_SHARED_DATA_STRUCT * pDataStruct, const char * pTitle)
{
	if (!Cless_DumpData_DumpOpenOutputDriver())
		return;

	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
		Cless_DumpData("\x1b" "@\x1b" "E");

	Cless_DumpData ("%s", pTitle);
	Cless_DumpData_DumpNewLine();
	Cless_DumpData_DumpNewLine();

	if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
		Cless_DumpData("\x1b" "@");

	Cless_DumpData_DumpCloseOutputDriver();

	Cless_DumpData_DumpSharedBuffer (pDataStruct, 0);
}



//! \brief Dump data ocntained in a TLV Tree node
//! \param[in] pLabel Label to be printed before the node data.
//! \param[in] TlvTreeNodeToDump TLV Tree node to be dump.
//! \note Label is printed in bold (if printer is used).
//! \note Data are printed in smal character using the following Telium_Format T:00009F06 L:07 V:A00000000991010

void Cless_DumpData_DumpTlvTreeNodeWithTitle (unsigned char * pLabel, TLV_TREE_NODE TlvTreeNodeToDump)
{
	TLV_TREE_NODE pResearch;			// Working Node
	unsigned char * pValue;
	unsigned int nDataLength;
	unsigned int nIndex;
	
	if (TlvTreeNodeToDump != NULL) {
		// Dump the title
		if (Cless_DumpData_DumpOpenOutputDriver()) {
			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
				Cless_DumpData ("\x1b" "E");

			if (pLabel != NULL)
				Cless_DumpData ("%s", pLabel);
				
			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
				Cless_DumpData ("\x1b" "@" "\x0F");

			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();

			pResearch = TlvTree_GetFirstChild (TlvTreeNodeToDump);
			if (pResearch == NULL) {
				// Specific case, nothing in note (only a value)
				nDataLength = TlvTree_GetLength(TlvTreeNodeToDump);
				Cless_DumpData ("T:%08x L:%02x V:", TlvTree_GetTag (TlvTreeNodeToDump), nDataLength);

				pValue = TlvTree_GetData(TlvTreeNodeToDump);
				for (nIndex=0; ((nIndex<nDataLength)&&(nIndex<20)); nIndex++)
					Cless_DumpData ("%02x", pValue[nIndex]);
				if ((nIndex == 20) && (nIndex < nDataLength))
					Cless_DumpData ("...");

				Cless_DumpData_DumpNewLine();
			}

			while (pResearch != NULL) {
				nDataLength = TlvTree_GetLength(pResearch);
				Cless_DumpData ("T:%08x L:%02x V:", TlvTree_GetTag (pResearch), nDataLength);
					
				pValue = TlvTree_GetData(pResearch);
				for (nIndex=0; ((nIndex<nDataLength)&&(nIndex<20)); nIndex++)
					Cless_DumpData ("%02x", pValue[nIndex]);
				if ((nIndex == 20) && (nIndex < nDataLength))
					Cless_DumpData ("...");

				Cless_DumpData_DumpNewLine();

				pResearch = TlvTree_GetNext (pResearch);
			}
				
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();
			Cless_DumpData_DumpNewLine();

			if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
				Cless_DumpData ("\x1b" "@");

			// Close the output driver
			Cless_DumpData_DumpCloseOutputDriver();
		}
	}
}



//! \brief Dump data ocntained in a TLV Tree node
//! \param[in] pLabel Label to be printed (bold printed).

void Cless_DumpData_DumpTitle (unsigned char * pLabel) {
	if (Cless_DumpData_DumpOpenOutputDriver()) {
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "E");

		if (pLabel != NULL)
			Cless_DumpData ("%s", pLabel);
			
		if (Cless_DumpData_DumpGetOutputId() == CUSTOM_OUTPUT_PRINTER)
			Cless_DumpData ("\x1b" "@");

		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();
		Cless_DumpData_DumpNewLine();


		// Close the output driver
		Cless_DumpData_DumpCloseOutputDriver();
	}
}
