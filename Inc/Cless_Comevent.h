//! \file
//! Interface with a generic communication component.

#ifndef _COMMEVT_H_
#define _COMMEVT_H_

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////
/////////////////////////////////////////////////////////////////

#define COM_EVT_COMMUNICATION_ORDER       0x01
#define COM_EVT_START_ORDER               0x02
#define COM_EVT_STOP_ORDER                0x03


/////////////////////////////////////////////////////////////////
//// Functions definitions //////////////////////////////////////
/////////////////////////////////////////////////////////////////

//! \brief Start task for COM0 management.
//! \param[in] no : application number.
void Cless_Comevent_Start(NO_SEGMENT no);


//! \brief Manage events.
//! \param[in] action.
//! \return allways FCT_OK 
int Cless_Comevent_Manage (unsigned char action);


//! \brief Manage events to manage serial link.
//! \param[in] noappli application number.
//! \param[in] param_in input parameters buffer.
//! \param[in] param_out output parameters buffer.
//! \return allways FCT_OK.
int Cless_Comevent_Event(NO_SEGMENT AppliNum, S_COMIN* pParamIn, S_COMOUT* pParamOut);


//! \brief Dump data on PRINTER.
//! \param[in] Header line to print on begining of ticket.
//! \param[in] String hexa buffer to print.
//! \param[in] Number hexa buffer length.
void Cless_Comevent_HexDump(unsigned char *Header,unsigned char *String,unsigned long Number);


// \brief Allows to knows if the transaction is asked by a serial link request.
// \return   
// 	- \ref TRUE if the transaction is due to a serial link order, FALSE else.
unsigned char Cless_ComEvent_IsSerialRequest(void);


//! \brief Memorised that orded is asked by serial link
void Cless_ComEventSetSerialRequest(void);


//! \brief Deleted information that orded is asked by serial link
void Cless_ComEventUnSetSerialRequest(void);


//! \brief 
//! \param[in] Msg 
//! \param[in] ucEventType 
void Cless_Comevent_SendMsg (char * Msg, unsigned short ucEventType);



#endif // _COMMEVT_H_
