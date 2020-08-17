//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  DLL.C                              (Copyright INGENICO 2012)
//============================================================================
//  Created :       13-July-2012     Kassovic
//  Last modified : 13-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                          *** DLL management ***
//      Show how to call a DLL.
//      The dynamic library BeepDll is generated during the training.
//      OSLayer.lib must be implemented to link properly.
//                                                                            
//  List of routines in file :  
//      BeepDll_Open : Open the DLL called Beep.
//      BeepDll_Init : Access to the Beep DLL functions.
//      BeepDll : Beep DLL demo.                                            
//                            
//  File history :
//  071312-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library
extern T_OSL_HDLL hBeepDll;     // Handle of the Beep Dll.

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define DLL_NAME     "BEEPDLL"  // The name of the "family name of the DLL descriptor"

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
// Properties of the Dll screen (Goal)
// ===================================
static const ST_DSP_LINE txDll[] =
{
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line0
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line1
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
	{ {GL_ALIGN_CENTER, GL_ALIGN_CENTER, GL_COLOR_WHITE, GL_COLOR_BLACK, 0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_WHITE}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}}, // Line2
	  {GL_ALIGN_CENTER, GL_ALIGN_CENTER, FALSE,  0, FALSE, {0, 0, 0, 0}, {0, 0, 0, 0, GL_COLOR_BLACK}, {0, 0, 0, 0}, {NULL, GL_FONT_STYLE_NORMAL, GL_SCALE_XLARGE}} },
};

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
typedef void (*ONE_BEEP) (void);
typedef int (*MANY_BEEP) (int);
typedef int (*GET_COUNTER) (void);

static ONE_BEEP _OneBeep=NULL;
static MANY_BEEP _ManyBeep=NULL;
static GET_COUNTER _GetCounter=NULL;

//****************************************************************************
//                          int BeepDll_Open (void)                            
//  This function opens the DLL called Beep at start up (see entry() function
//  from the file Entry.c).
//   - OSL_Dll_Open() : Open the DLL by its family name
//  This function has no parameters.    
//  This function has return value.
//   !NULL : Pointer to the handle of the Beep DLL.
//   NULL : Beep DLL opening failed.
//****************************************************************************

T_OSL_HDLL BeepDll_Open(void)
{
	// Local variables
	// ***************
		// Empty

	return (OSL_Dll_Open(DLL_NAME)); // Open the DLL by its family name
}

//****************************************************************************
//                  int BeepDll_Init (T_OSL_HDLL dllHandle)
//  This function retrieves the functions addresses located inside the DLL.
//   - OSL_Dll_GetSymbolByName : Get the function address
//  This function has parameters.
//   dllHandle (I-) : Handle of a the DLL.
//  This function has return value.
//    >=0 : DLL initialization done.
//     <0 : DLL initialization failed.
//****************************************************************************

static int BeepDll_Init(T_OSL_HDLL dllHandle)
{
	// Local variables
	// ***************
    int iRet;

	// Get the functions address
	// *************************
	_OneBeep = (ONE_BEEP) OSL_Dll_GetSymbolByName(dllHandle, "OneBeep");
	CHECK(_OneBeep !=NULL, lblKO);
	_ManyBeep = (MANY_BEEP) OSL_Dll_GetSymbolByName(dllHandle, "ManyBeep");
	CHECK(_ManyBeep!=NULL, lblKO);
	_GetCounter = (GET_COUNTER) OSL_Dll_GetSymbolByName(dllHandle, "GetCounter");
	CHECK(_GetCounter!=NULL, lblKO);

	iRet=1;
	goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                     // None-classified low level error
    iRet=-1;
    goto lblEnd;
lblEnd:
    return iRet;
}

//****************************************************************************
//                          void BeepDll (void)                            
//  This function runs the DLL called Beep.
//   - OSL_Dll_IsPresent : Check the presence of a dll
//  This function has no parameters.    
//  This function has no return value.
//****************************************************************************

void BeepDll(void) 
{
	// Local variables 
    // ***************
	T_GL_HWIDGET hScreen=NULL;
	char tcDisplay[50];
    int iValue, iRet;

    // Check if BeepDll is loaded
    // **************************
    iRet = OSL_Dll_IsPresent(DLL_NAME);
    CHECK(iRet!=0, lblNoBeepDll);

	// Check if BeepDLL is initialized to access DLL functions
	// *******************************************************
	iRet = BeepDll_Init(hBeepDll);
	CHECK(iRet>=0, lblBeepDllKO);
	
	// Call the functions from the DLL
	// *******************************
	hScreen = GoalCreateScreen(hGoal, txDll, NUMBER_OF_LINES(txDll), GL_ENCODING_UTF8);
	CHECK(hScreen!=NULL, lblKO);       // Create screen and clear it
	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false);
	CHECK(iRet>=0, lblKO);

	iRet = GoalDspLine(hScreen, 0, "Press Valid", &txDll[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "To hear 1 Beep", &txDll[1], 30*1000, true);
	CHECK(iRet>=0, lblKO);
	CHECK(iRet==GL_KEY_VALID, lblEnd); // Exit on cancel key or timeout

	_OneBeep();                        // Call function OneBeep()

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false); // Clear screen
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 0, "Press Valid", &txDll[0], 0, false);
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 1, "To hear 3 Beeps", &txDll[1], 30*1000, true);
	CHECK(iRet>=0, lblKO);
	CHECK(iRet==GL_KEY_VALID, lblEnd); // Exit on cancel key or timeout

	iRet = _ManyBeep(3);               // Call function ManyBeep()
	CHECK(iRet==2*3, lblKO);

	iValue = _GetCounter();            // Call function GetCounter()

	iRet = GoalClrScreen(hScreen, GL_COLOR_BLACK, KEY_CANCEL | KEY_VALID, false); // Clear screen
	CHECK(iRet>=0, lblKO);
	iRet = GoalDspLine(hScreen, 0, "DLL Counter Value", &txDll[0], 0, false);
	CHECK(iRet>=0, lblKO);
	Telium_Sprintf (tcDisplay, "%d", (word)iValue);
	iRet = GoalDspLine(hScreen, 1, tcDisplay, &txDll[1], 30*1000, true);
	CHECK(iRet>=0, lblKO);

    goto lblEnd;

	// Errors treatment 
    // ****************
lblKO:                                 // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblNoBeepDll:                          // Missing Beep DLL
	GL_Dialog_Message(hGoal, NULL, "Beep DLL missing\nPlease load it", GL_ICON_WARNING, GL_BUTTON_VALID, 3*1000);
	goto lblEnd;
lblBeepDllKO:                          // Cannot access functions from Beep DLL
	GL_Dialog_Message(hGoal, NULL, "Beep DLL access failed", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hScreen)
		GoalDestroyScreen(&hScreen);   // Destroy screen
}


