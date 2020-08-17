//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  DIALOGBOX.C                        (Copyright INGENICO 2014)
//============================================================================
//  Created :       07-May-2014     Kassovic
//  Last modified : 07-May-2014     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//                       *** Dialog Box management ***
//  This file DOES NOT use the following API(s) for input:
//    # Manager level (G_Extended_entry(), GetAmount(), ...)
//  This file uses the Graphic Object Advanced Library (GOAL) for input:
//    # Predefined mode (used inside this file)
//      A list of dialogs boxes to display message.
//        - GL_Dialog_Message, GL_Dialog_Picture, GL_Dialog_Progress and GL_Dialog_Media.
//      A list of dialogs boxes for data entry.
//        - GL_Dialog_Text, GL_Dialog_Date, GL_Dialog_Amount, GL_Dialog_Ip, GL_Dialog_Password,
//        - GL_Dialog_Scheme, GL_Dialog_Color, GL_Dialog_Slider, GL_Dialog_VirtualKeyboard,
//        - GL_Dialog_Signature.
//      A list of dialogs boxes for list selection.
//        - GL_Dialog_Menu, GL_Dialog_IconMenu, GL_Dialog_IconButtonMenu, GL_Dialog_Choice,
//        - GL_Dialog_MultiChoice.
//      A list of dialogs boxes for file selection.
//        - GL_Dialog_File, GL_Dialog_MultiFile.
//                                                                            
//  List of routines in file :  
//      DialogInfo : Open an information message box.
//      DialogWarning : Open a warning message box.
//      DialogError : Open an error message box.
//      DialogQuestion : Open a question message box.
//      DialogPicture : Open a picture message box.
//      DialogProgress : Open a progress message box.
//      GlDialogMessage : A list of dialogs boxes to display message.
//                         ----------------------------
//      DialogText : Open a text input box.
//      DialogDate : Open a date input box.
//      DialogTime : Open a time input box.
//      DialogTime12h : Open a time 12h format input box.
//      DialogAmount : Open an amount input box.
//      DialogIp : Open an Ip input box.
//      DialogPassword : Open a password input box.
//      DialogColor : Open a color palette input box.
//      DialogSlider : Open a slider input box.
//      DialogVirtualKeyboard : Open a virtual keyboard input box.
//      DialogSignature : Open a signature capture input box.
//      DialogScheme : Open a pin entry input box using scheme.
//      GlDialogInput : A list of dialogs boxes for data entry.
//                         ----------------------------
//      DialogMenu : Open a list box of items.
//      DialogIconMenu : Open a list box of icons.
//      DialogIconButtonMenu : Open a list box of icons buttons.
//      DialogChoice : Open a choice list box.
//      DialogMultiChoice : Open a multi choice list box.
//      GlDialogList : A list of dialogs boxes to display lists.
//                         ----------------------------
//      DialogFile : Open a choice box of files.
//      DialogMultiFile : Open a multi choice box of files.
//      GlDialogFile : A list of dialogs boxes for selecting files.
//
//  File history :
//  050714-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>
#include "SEC_interface.h"
#include "schvar_def.h"

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define APN_LEN    30

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
typedef struct T_GL_SCHEME_CONTEXT
{
	int iState;
	int iFirstDisplay;
} T_GL_SCHEME_CONTEXT;

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
// For dialog menu, dialog choice and dialog multi choice
static const char *tzMenu[] =
{
	"Zero",
	"One",
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Height",
	"Nine",
	"Ten",
	"Eleven",
	"Twelve",
	NULL
};

// For dialog icon menu and dialog icon button menu (B&W terminal)
const char *tzMenuBW[] =
{
	"Zero",   "file://flash/HOST/ICONS.TAR/0.bmp",
	"One",    "file://flash/HOST/ICONS.TAR/1.bmp",
	"Two",    "file://flash/HOST/ICONS.TAR/2.bmp",
	"Three",  "file://flash/HOST/ICONS.TAR/3.bmp",
	"Four",   "file://flash/HOST/ICONS.TAR/4.bmp",
	"Five",   "file://flash/HOST/ICONS.TAR/5.bmp",
	"Six",    "file://flash/HOST/ICONS.TAR/6.bmp",
	"Seven",  "file://flash/HOST/ICONS.TAR/7.bmp",
	"Height", "file://flash/HOST/ICONS.TAR/8.bmp",
	"Nine",   "file://flash/HOST/ICONS.TAR/9.bmp",
	"Ten",    "file://flash/HOST/ICONS.TAR/10.bmp",
	"Eleven", "file://flash/HOST/ICONS.TAR/11.bmp",
	"Twelve", "file://flash/HOST/ICONS.TAR/12.bmp",
	NULL
};

// For dialog icon menu and dialog icon button menu (Color terminal)
const char *tzMenuCL[] =
{
	"Zero",   "file://flash/HOST/ICONS.TAR/0.png",
	"One",    "file://flash/HOST/ICONS.TAR/1.png",
	"Two",    "file://flash/HOST/ICONS.TAR/2.png",
	"Three",  "file://flash/HOST/ICONS.TAR/3.png",
	"Four",   "file://flash/HOST/ICONS.TAR/4.png",
	"Five",   "file://flash/HOST/ICONS.TAR/5.png",
	"Six",    "file://flash/HOST/ICONS.TAR/6.png",
	"Seven",  "file://flash/HOST/ICONS.TAR/7.png",
	"Height", "file://flash/HOST/ICONS.TAR/8.png",
	"Nine",   "file://flash/HOST/ICONS.TAR/9.png",
	"Ten",    "file://flash/HOST/ICONS.TAR/10.png",
	"Eleven", "file://flash/HOST/ICONS.TAR/11.png",
	"Twelve", "file://flash/HOST/ICONS.TAR/12.png",
	NULL
};

//##############################################################################
//                       Message dialog boxes
//##############################################################################

//****************************************************************************
//                       void GlDialogMessage(void)
//  This function shows a list of dialogs boxes to display a message (predefined mode).
//  - GL_Dialog_Message: Creating a message box to display an info, a warning, an error
//                       and a question.
//  - GL_Dialog_Picture: Creating a message box to display a picture.
//  - GL_Dialog_Media: Creating a message box to display a video or to play audio.
//  This function has no return value.                                      
//****************************************************************************

static ulong DialogInfo(void)
{
	// Local variables
    // ***************
	ulong ulRet;

	// Open an information message box
	// *******************************
	ulRet = GL_Dialog_Message(hGoal, "Information title", "Information message", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, GL_TIME_MINUTE);

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogWarning(void)
{
	// Local variables 
    // ***************
	ulong ulRet;

	// Open a warning message box
	// **************************
	ulRet = GL_Dialog_Message(hGoal, "Warning title", "Warning message", GL_ICON_WARNING, GL_BUTTON_VALID_CANCEL, GL_TIME_MINUTE);

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogError(void)
{
	// Local variables
    // ***************
	ulong ulRet;

	// Open an error message box
	// *************************
	ulRet = GL_Dialog_Message(hGoal, "Error title", "Error message", GL_ICON_ERROR, GL_BUTTON_VALID_CANCEL, GL_TIME_MINUTE);
	
	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogQuestion(void)
{
	// Local variables 
    // ***************
	ulong ulRet;

	// Open a question message box
	// ***************************
	ulRet = GL_Dialog_Message(hGoal, "Question title", "Question message", GL_ICON_QUESTION, GL_BUTTON_VALID_CANCEL, GL_TIME_MINUTE);

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogPicture(void)
{
	// Local variables
	// ***************
	ulong ulRet;

	// Open a picture message box
	// **************************
    if (IsColorDisplay())                                                    // Color terminal? Yes, it is a color terminal
    {
    	if (GL_File_Exists("file://flash/HOST/TIGER_CL.JPG") == GL_SUCCESS)  // JPG file present? Yes, shows JPG picture
    		ulRet = GL_Dialog_Picture (hGoal, "Picture", "Jpeg format", "file://flash/HOST/TIGER_CL.JPG", GL_BUTTON_DEFAULT, GL_TIME_MINUTE);
    	else                                                                 // No, JPG file is missing
    	{
    		ulRet = GL_Dialog_Picture (hGoal, "Picture", "File TIGER_CL.JPG\nMissing from Host", NULL, GL_BUTTON_DEFAULT, 5*GL_TIME_SECOND);
			if (ulRet == GL_RESULT_INACTIVITY)
				ulRet=GL_KEY_VALID;
    	}
    }
    else                                                                     // No, it is a B&W terminal
    {
    	if (GL_File_Exists("file://flash/HOST/TIGERD_BW.BMP") == GL_SUCCESS) // BMP file present? Yes, shows BMP picture
    		ulRet = GL_Dialog_Picture (hGoal, "Picture", "Bmp format", "file://flash/HOST/TIGERD_BW.BMP", GL_BUTTON_DEFAULT, GL_TIME_MINUTE);
    	else                                                                 // No, BMP file is missing
    	{
    		ulRet = GL_Dialog_Picture (hGoal, "Picture", "File TIGERD_BW.BMP\nMissing from Host", NULL, GL_BUTTON_DEFAULT, 5*GL_TIME_SECOND);
    		if (ulRet == GL_RESULT_INACTIVITY)
    			ulRet=GL_KEY_VALID;
    	}
    }

	return ulRet;
}
//                                  -----------------------------------------
// This function is called periodically (10ms) to update the progress bar of the dialog box.
// The value received as parameter corresponds to the current value of the progress bar.
// When this value reaches the maximum, the dialog is automatically closed.
static T_GL_COORD update(T_GL_COORD usValue)
{
	return usValue+1;
}

static ulong DialogProgress(void)
{
	// Local variables
	// ***************
	ulong ulRet;

	// Open a progress message box
	// ***************************
	if (IsColorDisplay())            // Color terminal? Yes, it is a color terminal and shows a progress bar with JPG picture
		ulRet = GL_Dialog_Progress(hGoal, "Progress bar", "Please wait", "file://flash/HOST/TIGER_CL.JPG", 0, 0, 100, update, GL_BUTTON_DEFAULT, GL_TIME_MINUTE);
	else                             // No, it is a B&W terminal and shows a progress bar with BMP picture
		ulRet = GL_Dialog_Progress(hGoal, "Progress bar", "Please wait", "file://flash/HOST/TIGERD_BW.BMP", 0, 0, 100, update, GL_BUTTON_DEFAULT, GL_TIME_MINUTE);

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogMedia(void)
{
	// Local variables
	// ***************
	ulong ulRet;

	// Open a video message box
	// ************************
	if(GL_GraphicLib_IsTouchPresent(hGoal))  // Touch screen terminal? Yes, it is a touch terminal
	{
		if (GL_File_Exists("file://flash/HOST/INGENICO.MP4") == GL_SUCCESS)  // MP4 file present? Yes, shows MP4 video
			ulRet = GL_Dialog_Media(hGoal, "Video MP4", "Hello world", "file://flash/HOST/INGENICO.MP4", 150, GL_BUTTON_DEFAULT, 24*GL_TIME_SECOND);
		else                                                                 // No, MP4 file is missing
		{
			ulRet = GL_Dialog_Media(hGoal, "Video MP4", "File INGENICO.MP4\nMissing from Host", NULL, 0, GL_BUTTON_DEFAULT, 5*GL_TIME_SECOND);
    		if (ulRet == GL_RESULT_INACTIVITY)
    			ulRet=GL_KEY_VALID;
		}
	}
	else                                     // No, it is not a touch terminal
	{
		if (GL_File_Exists("file://flash/HOST/INGENICO.IPF") == GL_SUCCESS)  // IPF file present? Yes, shows IPF video
			ulRet = GL_Dialog_Media(hGoal, "Video IPF", "Hello world", "file://flash/HOST/INGENICO.IPF", 50, GL_BUTTON_DEFAULT, 24*GL_TIME_SECOND);
		else                                                                 // No, IPF file is missing
		{
			ulRet = GL_Dialog_Media(hGoal, "Video IPF", "File INGENICO.IPF\nMissing from Host", NULL, 0, GL_BUTTON_DEFAULT, 5*GL_TIME_SECOND);
    		if (ulRet == GL_RESULT_INACTIVITY)
    			ulRet=GL_KEY_VALID;
		}
	}

	return ulRet;
}
//                                  -----------------------------------------
void GlDialogMessage(void)
{
	// Local variables
    // ***************
	ulong ulRet;
	int iHeader, iFooter;

	// Shows boxes to display message
	// ******************************
	iHeader = IsHeader();                    // Save header state
	iFooter = IsFooter();                    // Save footer state

	if(!GL_GraphicLib_IsTouchPresent(hGoal)) // Check if it is not a touch screen
	{
		DisplayLeds(_OFF_);                  // Disable Leds
		DisplayHeader(_OFF_);                // Disable Header
		DisplayFooter(_OFF_);                // Disable Footer
	}

    ulRet = DialogInfo();                    // To open an information message box
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogWarning();                 // To open a warning message box
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogError();                   // To open an error message box
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogQuestion();                // To open a question message box
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogPicture();                 // To open a picture message box
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogProgress();                // To open a progress message box
    CHECK(ulRet!=GL_KEY_CANCEL, lblEnd);

    if (IsColorDisplay())                    // Check if it is a color screen
    	DialogMedia();                       // To open a video message box

lblEnd:
	DisplayLeds(_ON_);                       // Enable Leds
	DisplayHeader(iHeader);                  // Restore Header
	DisplayFooter(iFooter);              	 // Restore Footer
}

//##############################################################################
//                       Input dialog boxes
//##############################################################################

//****************************************************************************
//                       void GlDialogInput(void)
//  This function shows a list of dialogs boxes for data entry (predefined mode).
//   - GL_Dialog_Text: Creating an input box to enter a string value.
//   - GL_Dialog_Date: Creating an input box to enter date or time.
//   - GL_Dialog_Amount: Creating an input box to enter an amount.
//   - GL_Dialog_Ip: Creating an input box to enter an Ip address.
//   - GL_Dialog_Password: Creating an input box to enter a password.
//   - GL_Dialog_Scheme: Creating an input box to enter a pin code.
//   - GL_Dialog_Color: Creating an input box to select a color from a palette.
//   - GL_Dialog_Slider: Creating an input box to select a value from the slider.
//   - GL_Dialog_VirtualKeyboard: Creating an input virtual keyboard box.
//   - GL_Dialog_Signature: Creating an input box to capture a signature.
//  This function has no return value.
//****************************************************************************

static ulong DialogText(void)
{
	// Local variables
	// ***************
	char tcText[100] = "330412345678";
	ulong ulRet;

	// Open a text input box
	// *********************
	ulRet = GL_Dialog_Text(hGoal, "Text", "Enter phone # :","(/d/d)/d/d./d/d./d/d./d/d./d/d", tcText, sizeof(tcText), GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the phone#
	{
		ulRet = GL_Dialog_Message(hGoal, "Text", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogDate(void)
{
	// Local variables
	// ***************
	T_GL_DATE xDate;
	char tcText[100];
	ulong ulRet;

	// Open a date input box
	// *********************
	xDate.day    = 7;
	xDate.month  = 2;
	xDate.year   = 2011;
	ulRet = GL_Dialog_Date(hGoal, "Date", "Set date :", &xDate, "y/m/d", GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the date
	{
		Telium_Sprintf(tcText, "%d%02d%02d", xDate.year, xDate.month, xDate.day);
		ulRet = GL_Dialog_Message(hGoal, "Date", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogTime(void)
{
	// Local variables
	// ***************
	T_GL_DATE xDate;
	char tcText[100];
	ulong ulRet;

	// Open a time input box
	// *********************
	xDate.hour   = 13;
	xDate.minute = 56;
	xDate.second = 38;
	ulRet = GL_Dialog_Date(hGoal, "Time", "Set time :", &xDate, "H:M:S", GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the time
	{
		Telium_Sprintf(tcText, "%02d%02d%02d", xDate.hour, xDate.minute, xDate.second);
		ulRet = GL_Dialog_Message(hGoal, "Time", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogTime12h(void)
{
	// Local variables
	// ***************
	T_GL_DATE xDate;
	char tcText[100];
	ulong ulRet;

	// Open a time 12h format input box
	// ********************************
	xDate.hour   = 13;
	xDate.minute = 54;
	xDate.second = 32;
	ulRet = GL_Dialog_Date(hGoal, "Time 12H Format", "Set time :", &xDate, "h:M:S", GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the time
	{
		Telium_Sprintf(tcText, "%02d%02d%02d", xDate.hour, xDate.minute, xDate.second);
		ulRet = GL_Dialog_Message(hGoal, "Time 12H Format", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogAmount(void) {
	// Local variables
	// ***************
	char tcAmount[100] = "000000000000";
	char tcCurr[4] = "TZS";
	ulong ulRet;

	mapGet(traCurrencyLabel, tcCurr, 3);

	// Open an amount input box
	// ************************
	ulRet = GL_Dialog_Amount(hGoal, "Amount", "Enter amount :", "/d/d/d,/d/d/d,/d/d/D./D/D", tcAmount, sizeof(tcAmount), tcCurr, GL_ALIGN_LEFT, GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID){ // Shows the amount
		ulRet = GL_Dialog_Message(hGoal, "Amount", tcAmount, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogIp(void)
{
	// Local variables
	// ***************
	T_GL_IP ulIp;
	ulong ulRet;

	// Open an Ip input box
	// ********************
	ulIp = IpToUint("192.168.1.1");
	ulRet = GL_Dialog_Ip(hGoal, "Ip address", "Enter server Ip :", &ulIp, GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the Ip address
	{
		ulRet = GL_Dialog_Message(hGoal, "Ip address", UintToIp(ulIp), GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogPassword(void)
{
	// Local variables
	// ***************
	char tcText[100] = "";
	ulong ulRet;

	// Open a password input box
	// *************************
	ulRet = GL_Dialog_Password(hGoal, "Password", "Enter password :","/d/d/d/d/d/d", tcText, sizeof(tcText), GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the password
	{
		if (tcText[0] != 0)
			ulRet = GL_Dialog_Message(hGoal, "Password", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		else
			ulRet = GL_Dialog_Message(hGoal, "Password", "No password!", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogColor(void)
{
	// Local variables
	// ***************
	T_GL_COLOR ulColor;
	char tcText[100];
	ulong ulRet;

	// Open a color palette input box
	// ******************************
	ulRet = GL_Dialog_Color(hGoal, "Color palette", "Select color", &ulColor, GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the RGB color
	{
		Telium_Sprintf(tcText, "0x%lX", ulColor);
		ulRet = GL_Dialog_Message(hGoal, "Color palette", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
// This function is called periodically (10ms) to get the slider range min, max, value, step.
static void refresh(T_GL_RANGE xRange)
{
	/* Do treatment here */
}

static ulong DialogSlider(void)
{
	// Local variables
	// ***************
	T_GL_COORD value = 0;
	char cVal='%';
	char tcText[100];
	ulong ulRet;

	// Open a slider input box
	// ***********************
	if (IsColorDisplay())      // Color terminal? Yes, it is a color terminal and shows a slider with JPG picture
		ulRet = GL_Dialog_Slider (hGoal, "Slider", "Select level", "file://flash/HOST/TIGER_CL.JPG", &value, 0, 100, 5, refresh, GL_TIME_MINUTE);
	else                       // No, it is a B&W terminal and shows a slider with BMP picture
		ulRet = GL_Dialog_Slider (hGoal, "Slider", "Select level", "file://flash/HOST/TIGERD_BW.BMP", &value, 0, 100, 5, refresh, GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the slider level
	{
#ifndef __TELIUM3__
		Telium_Sprintf(tcText, "Slider = %d%c", value, cVal);
#else
	Telium_Sprintf(tcText, "Slider = %ld%c", value, cVal);
#endif
		ulRet = GL_Dialog_Message(hGoal, "Slider", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogVirtualKeyboard(void)
{
	// Local variables
	// ***************
	char tcMask[256];
	char tcText[100] = "telium@ingenico.com";
	int i;
	ulong ulRet;

	// Open a virtual keyboard input box
	// *********************************
	memset(tcMask, 0, sizeof(tcMask));
	for (i=0; i<APN_LEN; i++)
		strcat(tcMask, "/c");  // Accept any character
	ulRet = GL_Dialog_VirtualKeyboard(hGoal, "Virtual keyboard", "Enter Apn :", tcMask, tcText, sizeof(tcText), GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the Apn
	{
		ulRet = GL_Dialog_Message(hGoal, "Virtual keyboard", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogSignature(void)
{
	// Local variables
	// ***************
	ulong ulRet;

	// Open a signature capture input box
	// **********************************
	if (GL_File_Exists("file://flash/HOST/SIGNATURE.SIG") == GL_SUCCESS)     // Check if previous signature file is present in HOST
		GL_File_Delete("file://flash/HOST/SIGNATURE.SIG");                   // Delete signature file from HOST

	ulRet = GL_Dialog_Signature(hGoal, "Signature", "Sign please", "file://flash/HOST/SIGNATURE.SIG", GL_TIME_MINUTE);

	if (ulRet == GL_KEY_VALID)
	{
		if (GL_File_Exists("file://flash/HOST/SIGNATURE.SIG") == GL_SUCCESS) // Check if new signature file is present in HOST
			ulRet = GL_Dialog_Message(hGoal, "Signature", "file://flash/HOST/\nSIGNATURE.SIG", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		else
			ulRet = GL_Dialog_Message(hGoal, "Signature", "No signature!", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static unsigned long DialogOpenScheme (T_GL_HSCHEME_INTERFACE interfac)
{
	// Local variables
	// ***************
	T_SEC_ENTRYCONF entryConfig;

	// Initialize Pin entry using security DLL
	// ***************************************
	(void) (interfac);
	entryConfig.ucEchoChar = 0x2A;              // '*'
	entryConfig.ucMinDigits = 4;                // min digit
	entryConfig.ucMaxDigits = 7;                // max digit
	entryConfig.iFirstCharTimeOut = 60 * 1000;  // Timeout first pin key
	entryConfig.iInterCharTimeOut = 10 * 1000;  // Timeout inter pin key

	if (SEC_PinEntryInit(&entryConfig, C_SEC_PINCODE) == OK)
		return GL_RESULT_SUCCESS;
	else
		return GL_RESULT_FAILED;
}

static int PinGetKey(unsigned int *puiKey)
{
	// Local variables
	// ***************
	int iRet=TRUE;
	unsigned char ucOutData;
	unsigned int uiEventToWait=0;
	int iToContinue=TRUE;

	// Gets a key from the pin
	// ***********************
	switch(SEC_PinEntry(&uiEventToWait, &ucOutData, &iToContinue))
	{
	case OK:
		if (ucOutData == 0x2A) // '*'
			*puiKey = T_NUM0;  // This is a numeric key
		else
		{
			switch (ucOutData)
			{
			case 0x00: iRet = FALSE;        break;
			case 0x01: iRet = FALSE;        break;
			default:   *puiKey = ucOutData; break; // T_VAL, T_ANN, T_CORR
			}
		}
		break;

	case ERR_TIMEOUT:
		iRet = FALSE;
		break;

	default:
		iRet = FALSE;
		break;
	}

	return iRet;
}

static unsigned long DialogRefreshScheme(T_GL_HSCHEME_INTERFACE interfac, T_GL_HWIDGET hLabel)
{
	// Local variables
	// ***************
	unsigned long ulRet = GL_RESULT_SUCCESS;
	int iCr;
	unsigned int uiKey;
	T_GL_SCHEME_CONTEXT * schemeContext = (T_GL_SCHEME_CONTEXT*)interfac->privateData;

	// Refresh the display
	// *******************
	if (schemeContext->iFirstDisplay == 0)
	{
		schemeContext->iFirstDisplay = 1;
		GL_Widget_SetText(hLabel, "----");
	}
	else
	{
		iCr = PinGetKey(&uiKey);
		if (iCr == FALSE)
			ulRet = GL_RESULT_INACTIVITY;
		else
		{
			switch (uiKey)
			{
			case T_VAL:  ulRet = GL_KEY_VALID;                                    break; // end of the enter pin, any return value different than OK will stop
			case T_ANN:  ulRet = GL_KEY_CANCEL;                                   break; // cancel the enter pin
			case T_CORR: if (schemeContext->iState != 0) schemeContext->iState--; break; // correction
			default:     schemeContext->iState++;                                 break;
			}
		}

		if (ulRet == GL_RESULT_SUCCESS)
		{
			// Manage the display
			switch (schemeContext->iState)
			{
			case 0: GL_Widget_SetText(hLabel, "----");   break;
			case 1: GL_Widget_SetText(hLabel, "*");      break;
			case 2: GL_Widget_SetText(hLabel, "**");     break;
			case 3: GL_Widget_SetText(hLabel, "***");    break;
			case 4: GL_Widget_SetText(hLabel, "****");   break;
			case 5: GL_Widget_SetText(hLabel, "*****");  break;
			case 6: GL_Widget_SetText(hLabel, "******"); break;
			case 7: GL_Widget_SetText(hLabel, "*******"); break; // return GL_KEY_VALID to exit the dialog
			}
		}
	}

	return ulRet;
}

static void DialogCloseScheme (T_GL_HSCHEME_INTERFACE interfac)
{
	// Insert code here to close pin scheme
	// ************************************
	(void)(interfac);
}

static ulong DialogScheme(void)
{
	// Local variables
	// ***************
	ulong ulRet;
	T_GL_SCHEME_INTERFACE schemeInterface;
	T_GL_SCHEME_CONTEXT   schemeContext;

	// Initializes the scheme private context
	schemeContext.iState = 0;
	schemeContext.iFirstDisplay = 0;

	// Initializes interface scheme management
	schemeInterface.open        = DialogOpenScheme;
	schemeInterface.close       = DialogCloseScheme;
	schemeInterface.refresh     = DialogRefreshScheme;

	// Put here the pointer on your data private
	schemeInterface.privateData = &schemeContext;

	// Open a pin entry input box using scheme
	// ***************************************
	ulRet = GL_Dialog_Scheme(hGoal, "Pin entry", "Enter your pin", "Enter your code\nfrom prying eyes", &schemeInterface);
	if (ulRet == GL_KEY_VALID) {
		ulRet = GL_Dialog_Message(hGoal, "Pin entry", "Pin code done", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
void GlDialogInput(void)
{
	// Local variables
    // ***************
	ulong ulRet;
	int iHeader, iFooter;

	// Shows boxes for data entry
	// **************************
	iHeader = IsHeader();                    // Save header state
	iFooter = IsFooter();                    // Save footer state

	if(!GL_GraphicLib_IsTouchPresent(hGoal)) // Check if it is not a touch screen
	{
		DisplayLeds(_OFF_);                  // Disable Leds
		DisplayHeader(_OFF_);                // Disable Header
		DisplayFooter(_OFF_);                // Disable Footer
	}

    ulRet = DialogText();                    // To enter a value string
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogDate();                    // To enter a date
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogTime();                    // To enter a time
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogTime12h();                 // To enter a time 12H format
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogAmount();                  // To enter an amount
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogIp();                      // To enter Ip address
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogPassword();                // To enter a password
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogScheme();                  // To enter a pin code using scheme
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogColor();                   // To select a color from the palette
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogSlider();                  // To select a value from the slider
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogVirtualKeyboard();         // To enter a string value from the virtual keyboard
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

	if(GL_GraphicLib_IsTouchPresent(hGoal))  // Check if it is a touch screen
		DialogSignature();                   // To capture a signature

lblEnd:
	DisplayLeds(_ON_);                       // Enable Leds
	DisplayHeader(iHeader);                  // Enable Header
	DisplayFooter(iFooter);              	 // Enable Footer
}

//##############################################################################
//                       List dialog boxes
//##############################################################################

//****************************************************************************
//                          void GlDialogList(void)
//  This function shows a list of dialogs boxes for displaying lists (predefined mode).
//   - GL_Dialog_Menu: Creating a list box to display a list of items.
//   - GL_Dialog_Icon_Menu: Creating a list box to display a list of icons.
//   - GL_Dialog_IconButtonMenu: Creating a list box to display a list of icons
//     buttons.
//   - GL_Dialog_Choice: Creating a choice list box to select an item from the list.
//   - GL_Dialog_MultiChoice: Creating a multi-choice list box to select multiple
//     items from the list.
//  This function has no return value.
//****************************************************************************

static ulong DialogMenu(void)
{
/*
	const char *tzMenu[] =
	{
		"Zero",
		"One",
		"Two",
		"Three",
		"Four",
		"Five",
		"Six",
		"Seven",
		"Height",
		"Nine",
		"Ten",
		"Eleven",
		"Twelve",
		NULL
	};
*/
	// Local variables
    // ***************
	char tcText[100];
	ulong ulRet;

	// Open a list box of items
	// ************************
	ulRet = GL_Dialog_Menu(hGoal, "Menu", tzMenu, 3, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	if ((ulRet != GL_KEY_CANCEL) && (ulRet != GL_RESULT_INACTIVITY)) // Shows the item selected
	{
		Telium_Sprintf(tcText, "Item: %ld", ulRet);
		ulRet = GL_Dialog_Message(hGoal, "Menu", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogIconMenu(void)
{
/*
	const char *tzMenuCL|BW[] =
	{
		"Zero",   "file://flash/HOST/ICONS.TAR/0.png|bmp",
		"One",    "file://flash/HOST/ICONS.TAR/1.png|bmp",
		"Two",    "file://flash/HOST/ICONS.TAR/2.png|bmp",
		"Three",  "file://flash/HOST/ICONS.TAR/3.png|bmp",
		"Four",   "file://flash/HOST/ICONS.TAR/4.png|bmp",
		"Five",   "file://flash/HOST/ICONS.TAR/5.png|bmp",
		"Six",    "file://flash/HOST/ICONS.TAR/6.png|bmp",
		"Seven",  "file://flash/HOST/ICONS.TAR/7.png|bmp",
		"Height", "file://flash/HOST/ICONS.TAR/8.png|bmp",
		"Nine",   "file://flash/HOST/ICONS.TAR/9.png|bmp",
		"Ten",    "file://flash/HOST/ICONS.TAR/10.png|bmp",
		"Eleven", "file://flash/HOST/ICONS.TAR/11.png|bmp",
		"Twelve", "file://flash/HOST/ICONS.TAR/12.png|bmp",
		NULL
	};
*/
	// Local variables
    // ***************
	char tcText[100];
	ulong ulRet;

	// Open a list box of icons
	// ************************
	if (IsColorDisplay())          // Color terminal? Yes, it is a color terminal and shows a list of icons
		ulRet = GL_Dialog_IconMenu(hGoal, "Icon Menu", tzMenuCL, 0, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	else                           // No, it is a B&W terminal and shows a list of icons
		ulRet = GL_Dialog_IconMenu(hGoal, "Icon Menu", tzMenuBW, 0, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	if ((ulRet != GL_KEY_CANCEL) && (ulRet != GL_RESULT_INACTIVITY)) // Shows the item selected
	{
		Telium_Sprintf(tcText, "Item: %ld", ulRet);
		ulRet = GL_Dialog_Message(hGoal, "Icon Menu", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogIconButtonMenu(void)
{
/*
	const char *tzMenuCL|BW[] =
	{
		"Zero",   "file://flash/HOST/ICONS.TAR/0.png|bmp",
		"One",    "file://flash/HOST/ICONS.TAR/1.png|bmp",
		"Two",    "file://flash/HOST/ICONS.TAR/2.png|bmp",
		"Three",  "file://flash/HOST/ICONS.TAR/3.png|bmp",
		"Four",   "file://flash/HOST/ICONS.TAR/4.png|bmp",
		"Five",   "file://flash/HOST/ICONS.TAR/5.png|bmp",
		"Six",    "file://flash/HOST/ICONS.TAR/6.png|bmp",
		"Seven",  "file://flash/HOST/ICONS.TAR/7.png|bmp",
		"Height", "file://flash/HOST/ICONS.TAR/8.png|bmp",
		"Nine",   "file://flash/HOST/ICONS.TAR/9.png|bmp",
		"Ten",    "file://flash/HOST/ICONS.TAR/10.png|bmp",
		"Eleven", "file://flash/HOST/ICONS.TAR/11.png|bmp",
		"Twelve", "file://flash/HOST/ICONS.TAR/12.png|bmp",
		NULL
	};
*/
	// Local variables
    // ***************
	char tcText[100];
	ulong ulRet;

	// Open a list box of icons buttons
	// ********************************
	if (IsColorDisplay())           // Color terminal? Yes, it is a color terminal and shows a list of buttons
		ulRet = GL_Dialog_IconButtonMenu(hGoal, "IconButton Menu", tzMenuCL, 0, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	else                            // No, it is B&W terminal and shows a list of buttons
		ulRet = GL_Dialog_IconButtonMenu(hGoal, "IconButton Menu", tzMenuBW, 0, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	if ((ulRet != GL_KEY_CANCEL) && (ulRet != GL_RESULT_INACTIVITY)) // Shows the item selected
	{
		Telium_Sprintf(tcText, "Item: %ld", ulRet);
		ulRet = GL_Dialog_Message(hGoal, "IconButton Menu", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogChoice(void)
{
/*
	const char *tzMenu[] =
	{
		"Zero",
		"One",
		"Two",
		"Three",
		"Four",
		"Five",
		"Six",
		"Seven",
		"Height",
		"Nine",
		"Ten",
		"Eleven",
		"Twelve",
		NULL
	};
*/
	// Local variables
    // ***************
	char tcText[100];
	ulong ulRet;

	// Open a choice list box of items
	// *******************************
	ulRet = GL_Dialog_Choice(hGoal, "Choice Menu", tzMenu, 3, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	if ((ulRet != GL_KEY_CANCEL) && (ulRet != GL_RESULT_INACTIVITY)) // Shows the item selected
	{
		Telium_Sprintf(tcText, "Item: %ld", ulRet);
		ulRet = GL_Dialog_Message(hGoal, "Choice Menu", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
static ulong DialogMultiChoice(void)
{
/*
	const char *tzMenu[] =
	{
		"Zero",
		"One",
		"Two",
		"Three",
		"Four",
		"Five",
		"Six",
		"Seven",
		"Height",
		"Nine",
		"Ten",
		"Eleven",
		"Twelve",
		NULL
	};
*/
	// Local variables
    // ***************
	bool tbChecked[] =     // Initial state of checkbox
	{
		true,   //"Zero",
		false,  //"One",
		false,  //"Two",
		true,   //"Three",
		true,   //"Four",
		false,  //"Five",
		true,   //"Six",
		false,  //"Seven",
		false,  //"Height",
		false,  //"Nine",
		false,  //"Ten",
		true,   //"Eleven",
		true,   //"Twelve",
	};
	char tcText[100], *pcText;
	int i;
	ulong ulRet;

	// Open a multi-choice list box of items
	// *************************************
	ulRet = GL_Dialog_MultiChoice (hGoal, "MultiChoice Menu", tzMenu, 3, tbChecked, GL_BUTTON_VALID_CANCEL, GL_KEY_0, GL_TIME_MINUTE);
	if ((ulRet != GL_KEY_CANCEL) && (ulRet != GL_RESULT_INACTIVITY)) // Shows the list of items selected
	{
		pcText = tcText;
		pcText += Telium_Sprintf(pcText, "Item(s): ");
		for (i=0; i<sizeof(tbChecked)/sizeof(bool); i++)
		{
			if (tbChecked[i])
				pcText += Telium_Sprintf(pcText, "%d,", i);
		}
		*--pcText = 0; // Remove last comma
		ulRet = GL_Dialog_Message(hGoal, "Choice Menu", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
void GlDialogList(void)
{
	// Local variables
    // ***************
	ulong ulRet;
	int iHeader, iFooter;

	// Shows boxes to display list
	// ***************************
	iHeader = IsHeader();                    // Save header state
	iFooter = IsFooter();                    // Save footer state

	if(!GL_GraphicLib_IsTouchPresent(hGoal)) // Check if it is not a touch screen
	{
		DisplayLeds(_OFF_);                  // Disable Leds
		DisplayHeader(_OFF_);                // Disable Header
		DisplayFooter(_OFF_);                // Disable Footer
	}

	if (GL_File_Exists("file://flash/HOST/ICONS.TAR") != GL_SUCCESS) // Check if TAR file is absent
	{
		ulRet = GL_Dialog_Message(hGoal, "Dialog List", "File ICONS.TAR\nMissing from Host", GL_ICON_WARNING, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		CHECK(ulRet!=GL_KEY_CANCEL, lblEnd);
	}

    ulRet = DialogMenu();                    // To display a list of items and to wait the user selection
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogIconMenu();                // To display a list of icons and to wait the user selection
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogIconButtonMenu();          // To display a list of icons buttons and to wait the user selection
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    ulRet = DialogChoice();                  // To choose one item from a list of items
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    DialogMultiChoice();                     // To choose multi items from a list of items

lblEnd:
	DisplayLeds(_ON_);                       // Enable Leds
	DisplayHeader(iHeader);                  // Enable Header
	DisplayFooter(iFooter);              	 // Enable Footer
}

//##############################################################################
//                       File dialog boxes
//##############################################################################

//****************************************************************************
//                       void GlDialogFile(void)
//  This function shows a list of dialogs boxes for selecting files (predefined mode).
//   - GL_Dialog_File: Creating a file box to select a file in a file system.
//   - GL_Dialog_MultiFile: Creating a file box to select multiple files in a
//     file system.
//  This function has no return value.
//****************************************************************************

// This function is called to filter files or directories before displaying the menu
static bool Filter1(const char *pcPath, bool bIsFile)
{
	/* Do treatment here */
	return true;
}

static ulong DialogFile(void)
{
	// Local variables
    // ***************
	char tcFilename[256];
	char tcText[256];
	ulong ulRet;

	// Open a choice box of files
	// **************************
	memset(tcFilename, 0, sizeof(tcFilename));
	ulRet = GL_Dialog_File(hGoal, "Select file (HOST)", "dir://flash/HOST", "*.*", tcFilename, sizeof(tcFilename), Filter1, GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID) // Shows the URL of the file selected
	{
		if (tcFilename[0] != 0)
		{
			Telium_Sprintf(tcText, "%.18s\n%s", tcFilename, &tcFilename[18]);
			ulRet = GL_Dialog_Message(hGoal, "Select file (HOST)", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);
		}
		else
			ulRet = GL_Dialog_Message(hGoal, "Select file (HOST)", "No file!", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);

		if (ulRet == GL_RESULT_INACTIVITY)
				ulRet=GL_KEY_VALID;
	}

	return ulRet;
}
//                                  -----------------------------------------
// This function is called to filter files or directories before displaying the menu
static bool Filter2(const char *pcPath, bool bIsFile)
{
	/* Do treatment here */
	return true;
}

static ulong DialogMultiFile(void)
{
	// Local variables
    // ***************
	T_GL_HSTRINGLIST hList;
	int i;
	char tcFilename[256];
	char tcText[256];
	ulong ulRet;

	// Open a multi-choice box of files
	// ********************************
	hList = GL_StringList_Create(GL_ENCODING_ISO_8859_1); // Create an empty list of files

	ulRet = GL_Dialog_MultiFile(hGoal, "Select many files (HOST)", "dir://flash/HOST", "*.*", hList, Filter2, GL_TIME_MINUTE);
	if (ulRet == GL_KEY_VALID)    // Shows the list of URL files selected
	{
		if (GL_StringList_GetCount(hList) != 0)
		{
			for (i=0; i<GL_StringList_GetCount(hList); i++)
			{
				strcpy(tcFilename, GL_StringList_GetString(hList, i));
				Telium_Sprintf(tcText, "%.18s\n%s", tcFilename, &tcFilename[18]);
				ulRet = GL_Dialog_Message(hGoal, "Select many files (HOST)", tcText, GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 2*GL_TIME_SECOND);
				if (ulRet == GL_KEY_CANCEL)
					break;
			}
		}
		else
			ulRet = GL_Dialog_Message(hGoal, "Select many files (HOST)", "No file!", GL_ICON_INFORMATION, GL_BUTTON_VALID_CANCEL, 5*GL_TIME_SECOND);

		if (ulRet == GL_RESULT_INACTIVITY)
			ulRet=GL_KEY_VALID;
	}

	GL_StringList_Destroy(hList); // Destroy the list of files

	return ulRet;
}
//                                  -----------------------------------------
void GlDialogFile(void)
{
	// Local variables
    // ***************
	ulong ulRet;
	int iHeader, iFooter;

	// Shows boxes for selecting files
	// *******************************
	iHeader = IsHeader();                    // Save header state
	iFooter = IsFooter();                    // Save footer state

	if(!GL_GraphicLib_IsTouchPresent(hGoal)) // Check if it is not a touch screen
	{
		DisplayLeds(_OFF_);                  // Disable Leds
		DisplayHeader(_OFF_);                // Disable Header
		DisplayFooter(_OFF_);                // Disable Footer
	}

    ulRet = DialogFile();                    // To select a file in a file system
    CHECK(ulRet==GL_KEY_VALID, lblEnd);

    DialogMultiFile();                       // To select multiple files in a file system

lblEnd:
	DisplayLeds(_ON_);                       // Enable Leds
	DisplayHeader(iHeader);                  // Enable Header
	DisplayFooter(iFooter);              	 // Enable Footer
}



