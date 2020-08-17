/**
 * \file
 * \brief This module manages the user interface environment (GOAL context...).
 *
 * \author Ingenico
 * \author Copyright (c) 2012 Ingenico, 28-32, boulevard de Grenelle,\n
 * 75015 Paris, France, All Rights Reserved.
 *
 * \author Ingenico has intellectual property rights relating to the technology embodied\n
 * in this software. In particular, and without limitation, these intellectual property rights may\n
 * include one or more patents.\n
 * This software is distributed under licenses restricting its use, copying, distribution, and\n
 * and decompilation. No part of this software may be reproduced in any form by any means\n
 * without prior written authorisation of Ingenico.
 **/

/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "sdk.h"
#include "GL_GraphicLib.h"
#include "TlvTree.h"
#include "GTL_Assert.h"

#include "EPSTOOL_Convert.h"
#include "EPSTOOL_TlvTree.h"

#include "EMV_UserInterface.h"


//****************************************************************************
//      EXTERN
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//// Macros & preprocessor definitions //////////////////////////

//// Types //////////////////////////////////////////////////////

//// Static function definitions ////////////////////////////////

////  variables ///////////////////////////////////////////

static T_GL_HGRAPHIC_LIB __EMV_UI_goalHandle = NULL;	//!< The GOAL handle.
static Telium_File_t *__EMV_UI_displayDriver = NULL;			//!< The display driver handle.
static Telium_File_t *__EMV_UI_keyboardDriver = NULL;			//!< The keyboard driver handle.
static Telium_File_t *__EMV_UI_touchDriver = NULL;				//!< The touch screen driver handle.

//// Functions //////////////////////////////////////////////////

//! \brief Create the graphic library instance.
//! \return  pointer to graphic library handle.
T_GL_HGRAPHIC_LIB EMV_UI_GoalHandle(void)
{
	if (__EMV_UI_goalHandle == NULL)
	{
		__EMV_UI_goalHandle = GL_GraphicLib_Create();
	}

	return __EMV_UI_goalHandle;
}

//! \brief Destroy the graphic library instance.
void EMV_UI_GoalDestroy(void)
{
	if (__EMV_UI_goalHandle != NULL) {
		GL_GraphicLib_Destroy(__EMV_UI_goalHandle);
		__EMV_UI_goalHandle = NULL;
	}
}

//! \brief Open the GOAL interface (open drivers ...).
void EMV_UI_GoalOpen(void)
{
	if (__EMV_UI_displayDriver == NULL)
		__EMV_UI_displayDriver = Telium_Fopen("DISPLAY", "w*");
	if (__EMV_UI_keyboardDriver == NULL)
		__EMV_UI_keyboardDriver = Telium_Fopen("KEYBOARD", "r*");
	if (__EMV_UI_touchDriver == NULL)
		__EMV_UI_touchDriver = Telium_Fopen("TSCREEN", "r*");
}

//! \brief Close the GOAL interface (close drivers ...).
void EMV_UI_GoalClose(void)
{
	if (__EMV_UI_displayDriver != NULL)
	{
		Telium_Fclose(__EMV_UI_displayDriver);
		__EMV_UI_displayDriver = NULL;
	}
	if (__EMV_UI_keyboardDriver != NULL)
	{
		Telium_Fclose(__EMV_UI_keyboardDriver);
		__EMV_UI_keyboardDriver = NULL;
	}
	if (__EMV_UI_touchDriver != NULL)
	{
		Telium_Fclose(__EMV_UI_touchDriver);
		__EMV_UI_touchDriver = NULL;
	}
}
