//! \file
//! \brief Module that manages the CGUI features.

#ifndef __CLESS_SAMPLE_CGUI_H__INCLUDED__
#define __CLESS_SAMPLE_CGUI_H__INCLUDED__


/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define CLESS_SAMPLE_CGUI_CONTEXT_UNCHANGED			(-1)		/*!< Indicates the status unchanged for Push/Pop CGUI contexts. */


/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////

int Cless_Goal_IsAvailable(void);

//! \brief Create the graphic library instance.
//! \return  pointer to graphic library handle.
T_GL_HGRAPHIC_LIB Cless_Goal_Init(void);

//! \brief Get the graphic library instance.
//! \return  pointer to graphic library handle.
T_GL_HGRAPHIC_LIB Cless_Goal_Handle(void);

//! \brief Destroy the graphic library instance.
void Cless_Goal_Destroy(void);

void Cless_Goal_ClearScreen(void);

//! \brief Open the GOAL interface (open drivers ...).
void Cless_Goal_Open(void);

//! \brief Close the GOAL interface (close drivers ...).
void Cless_Goal_Close(void);

//! \brief Displays a message with a choice (validate, cancel).
//! \param[in] MessageL1 Message displayed on the first line.
//! \param[in] MessageL2 Message displayed on the second line.
//! \return
//!		- \ref TRUE if user selected the left choice.
//!		- \ref FALSE if user selected the right choice.
int Cless_Goal_DisplayChoice(char* MessageL1, char* MessageL2);

//! \brief Displays a message with signature capture and a choice (validate, cancel).
//! \param[in] LeftChoice Message to validate (function returns \ref TRUE in this case).
//! \param[in] LeftChoice Message to cancel (function returns \ref FALSE in this case).
//! \param[in] MessageL1 Message displayed on the first line.
//! \param[in] MessageL2 Message displayed on the second line.
//! \return
//!		- \ref TRUE if user selected the left choice.
//!		- \ref FALSE if user selected the right choice.
int Cless_Goal_SignatureCapture (char* MessageL1, char* MessageL2);

//! \brief Ask for a PIN entry.
//! \param[in] pinType \a INPUT_PIN_OFF for an offline PIN entry or \a INPUT_PIN_ON for an online PIN entry.
//! \param[in] pinTryCounter The PIN try counter from the card. Give (-1) if unknown.
//! \param[out] pinLength Length of entered PIN.
//! \return Any value of \ref EPSTOOL_PinEntry_Status_e.
PinEntry_Status_e Cless_Goal_PinEntry(const char *title, const char *text, const char *help, int *pinLength);

//! \brief Display a message on the diaply using WGUI.
//! \param[in] nline Line where the message shall be displayed.
//! \param[in] ptrMSG Message structure to be used.
//! \param[in] bmode : TRUE to erase before display something, else nothing
void Cless_Goal_SetMessage (int nline, MSGinfos * ptrMSG, unsigned char bMode);

//! \brief Refresh the screen
void Cless_Goal_RefreshScreen (void);

//! \brief Display a message on the diaply using WGUI.
//! \param[in] nline Line where the message shall be displayed.
//! \param[in] bmode : TRUE to erase before display something, else nothing
void Cless_Goal_SetBlankLine (int nline, unsigned char bMode);

#endif //__CLESS_SAMPLE_CGUI_H__INCLUDED__
