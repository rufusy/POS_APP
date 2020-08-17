//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  GOAL.C                              (Copyright INGENICO 2012)
//============================================================================
//  Created :       17-July-2012     Kassovic
//  Last modified : 17-July-2012     Kassovic
//  Module : GOAL
//                                                                          
//  Purpose :                                                               
//  Since the beginning of 2012, Graphic Object Advanced Library (GOAL) is available:
//  GOAL provides 3 usage modes:
//  - Predefined dialog box mode (provides a set of high level UI functions)
//    MESSAGE to display a message. The user can optionally react on it.
//    INPUT to enter information of different types (like date, hour, amount, formatted field, etc...)
//    CHOICE to provide different ways of selecting  elements through a text or icons list.
//    MULTIMEDIA to render a multimedia file.
//  - Modular customization mode (How to build your own screen with file resource)
//    Group of simple widgets objects (Text, Graphical, Bar & Slider, etc...)
//    Group of complex widgets objects (Containers, Plug-in, Miscellaneous, etc...)
//    Available soon a WYSIWYG, otherwise use DAL (Data Abstraction Layer)
//  - Monolithic customization mode (How to build your own screen in 'C' using predefined API(s))
//    GOAL generic API(s) (GraphicLib_, Widget_, Message_, etc...)
//    GOAL specific API(s) (Button_, CheckButton_, Icon_, Label_, Layout_, List_, ScrollView_, etc...)
//
//  This file will provides a set of API(s) developed in Monolithic customization mode to handle
//  text, drawing, picture on screen and printing.
//
//  List of routines in file :
//      CallBackPressKey : Call back for processing a key pressed.
//      CallBackValidKey : Call back for processing a valid key pressed or touched.
//      CallBackCancelKey : Call back for processing a cancel key pressed or touched.
//      CallBackDownKey : Call back for processing a down key pressed or touched.
//      CallBackUpKey : Call back for processing a up key pressed or touched.
//      CallBackTimeout : Call back for processing a timeout message.
//                                *** SCREEN ***
//      GoalCreateScreen : Create a screen context.
//      GoalClrScreen : Clear all widgets on screen context.
//      GoalRefreshScreen : Refresh all widgets on screen.
//      GoalDspLine : Display a line on screen.
//      GoalDspMultiLine : Display multi-lines on screen.
//      GoalGetKey : Wait for shortcut key pressed or touched.
//      GoalDestroyScreen : Destroy a screen context.
//                                *** DRAWING ***
//      GoalCreateDrawing : Create  a drawing context.
//      GoalClrDrawing : Clear all widgets on drawing context.
//      GoalDrawLine : Draw a line on screen.
//      GoalDrawRect : Draw a rectangle on screen.
//      GoalDrawEllipse : Draw an ellipse on screen.
//      GoalDrawText : Draw a text on screen.
//      GoalDestroyDrawing : Destroy a drawing context.
//                                *** PICTURE ***
//      GoalCreatePicture : Create a picture context.
//      GoalClrPicture : Clear all widgets on picture context.
//      GoalDspPicture : Display a picture on screen.
//      GoalDestroyPicture : Destroy a picture context.
//                                *** DOCUMENT ***
//      GoalCreateDocument : Create a document context.
//      GoalPrnLine : Build a line on document.
//      GoalMultiLine : Build multi-lines on document.
//      GoalPrnPicture : Build a picture on document.
//      GoalPrnDocument : Print the document built.
//      GoalDestroyDocument : Destroy a document context.
//                            
//  File history :
//  071712-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES
//****************************************************************************
#include "Goal.h"

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
#define CHECK(CND,LBL) {if(!(CND)){goto LBL;}}

// Maximum line supported by the layout (Screen)
// =============================================
#define MAX_LINE    9

// Widget Id(s)
// ============
#define TIMER_ID       0x0000
#define LABEL_0_ID     0x0100
#define BUTTON_VAL_ID  0x0200
#define BUTTON_CAN_ID  0x0300
#define BUTTON_DOWN_ID 0x0400
#define BUTTON_UP_ID   0x0500
#define LAYOUT2_ID     0x0600
#define DRAWING_ID     0x0700
#define PICTURE_ID     0x0800
#define LAYOUT3_ID     0x0900

// Result returns from the call back function
// ==========================================
#define RES_VALID_STATUS  0  // Valid key pressed or touched (Green key)
#define RES_CANCEL_STATUS 1  // Red key pressed or touched (Red key)
#define RES_DOWN_STATUS   2  // Down key pressed or touched
#define RES_UP_STATUS     3  // Up key pressed or touched
#define RES_TIMER_STATUS  4  // Timeout expiration
#define RES_PRESS_STATUS  5  // Key pressed

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
	/* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
	/* */

//****************************************************************************
//              bool CallBackPressKey(T_GL_HMESSAGE hMessage)
// This function handles the call back for processing a key pressed and returns
// its value to GL_Window_MainLoop.
// This function has parameters.
//   hMessage (I-) : Message handle (Stylus button, key, timer, ...)
// This function has return value.
//  true : When leaving the loop GL_Window_MainLoop
//  false : When leaving the loop GL_Window_MainLoop
//****************************************************************************

static bool CallBackPressKey(T_GL_HMESSAGE hMessage)
{
	T_GL_WCHAR ulKey;

	// Retrieve key pressed
	ulKey = GL_Message_GetKey(hMessage);

    // Sets the data user attached to the window
    GL_Message_SetUserData(hMessage, &ulKey, sizeof(ulKey));

	// Will exit the application from the loop on GL_Window_MainLoop by returning
	// the result RES_PRESS_STATUS
	GL_Message_SetResult(hMessage, RES_PRESS_STATUS);

	// Force the value to true when releasing the application from the main loop
	return true;
}

//****************************************************************************
//              bool CallBackValidKey(T_GL_HMESSAGE hMessage)
// This function handles the call back for processing a valid key pressed or
//  touched.
// This function has parameters.
//   hMessage (I-) : Message handle (Stylus button, key, timer, ...)
// This function has return value.
//  true : When leaving the loop GL_Window_MainLoop
//  false : When leaving the loop GL_Window_MainLoop
//****************************************************************************

static bool CallBackValidKey(T_GL_HMESSAGE hMessage)
{
	// Will exit the application from the loop on GL_Window_MainLoop by returning
	// the result RES_VALID_STATUS
	GL_Message_SetResult(hMessage, RES_VALID_STATUS);

	// Force the value to true when releasing the application from the main loop
	return true;
}

//****************************************************************************
//              bool CallBackCancelKey(T_GL_HMESSAGE hMessage)
// This function handles the call back for processing a cancel key pressed or
//  touched.
// This function has parameters.
//   hMessage (I-) : Message handle (Stylus button, key, timer, ...)
// This function has return value.
//  true : When leaving the loop GL_Window_MainLoop.
//  false : When leaving the loop GL_Window_MainLoop.
//****************************************************************************

static bool CallBackCancelKey(T_GL_HMESSAGE hMessage)
{
	// Will exit the application from the loop on GL_Window_MainLoop by returning
	// the result RES_CANCEL_STATUS
	GL_Message_SetResult(hMessage, RES_CANCEL_STATUS);

	// Force the value to true when releasing the application from the main loop
	return true;
}

//****************************************************************************
//              bool CallBackDownKey(T_GL_HMESSAGE hMessage)
// This function handles the call back for processing a down key pressed or
//  touched.
// This function has parameters.
//   hMessage (I-) : Message handle (Stylus button, key, timer, ...)
// This function has return value.
//  true : When leaving the loop GL_Windpw_MainLoop.
//  false : When leaving the loop GL_Window_MainLoop.
//****************************************************************************

static bool CallBackDownKey(T_GL_HMESSAGE hMessage) {
	// Will exit the application from the loop on GL_Window_MainLoop by returning
	// the result RES_DOWN_STATUS
	GL_Message_SetResult(hMessage, RES_DOWN_STATUS);

	// Force the value to true when releasing the application from the main loop
	return true;
}

//****************************************************************************
//              bool CallBackUpKey(T_GL_HMESSAGE hMessage)
// This function handles the call back for processing a up key pressed or
//  touched.
// This function has parameters.
//   hMessage (I-) : Message handle (Stylus button, key, timer, ...)
// This function has return value.
//  true : When leaving the loop GL_Window_MainLoop.
//  false : When leaving the loop GL_Window_MainLoop.
//****************************************************************************

static bool CallBackUpKey(T_GL_HMESSAGE hMessage)
{
	// Will exit the application from the loop on GL_Window_MainLoop by returning
	// the result RES_UP_STATUS
	GL_Message_SetResult(hMessage, RES_UP_STATUS);

	// Force the value to true when releasing the application from the main loop
	return true;
}

//****************************************************************************
//              bool CallBackTimeout(T_GL_HMESSAGE hMessage)
// This function handles the call back for processing a timeout message.
// This function has parameters.
//   hMessage (I-) : Message handle (Stylus button, key, timer, ...)
// This function has return value.
//  true : When leaving the loop GL_Window_MainLoop.
//  false : When leaving the loop GL_Window_MainLoop.
//****************************************************************************

static bool CallBackTimeout(T_GL_HMESSAGE hMessage)
{
	// Will exit the application from the loop on GL_Window_MainLoop by returning
	// the result RES_TIMER_STATUS
	GL_Message_SetResult(hMessage, RES_TIMER_STATUS);

	// Stop the timer
	GL_Timer_Stop(GL_Message_GetWidget(hMessage));

	// Force the value to true when releasing the application from the main loop
	return true;
}

//##############################################################################
//                                   SCREEN
//##############################################################################

//****************************************************************************
//      T_GL_HWIDGET GoalCreateScreen (T_GL_HGRAPHIC_LIB hGraphicLib,
//                                     const ST_LINE txLine[],
//                                     ushort usMaxLine,
//                                     T_GL_ENCODING_CHARSET eCharSet)
// This function creates a screen context which is composed of:
//  - A timer to exit from this screen.
//  - A number of line which composed this screen.
//  - Shortcut keys on 4 buttons to react on this screen:
//      Cancel Key, Down Key, Up Key and Valid Key
//  - Encoding charsets:
//      GL_ENCODING_UTF8 => Unicode utf8
//      GL_ENCODING_ISO_8859_X => ISO 8859
// This function has parameters.
//   hGraphicalLib (I-) : Graphic library context handle.
//   txLine[] (I-) : Screen properties.
//   usMaxLine (I-) : Number of line which composed the screen.
//   eCharSet (I-) : Encoding charsets.
// This function has return value.
//   !NULL : The handle of the screen.
//   NULL : Screen failed
//****************************************************************************

T_GL_HWIDGET GoalCreateScreen (T_GL_HGRAPHIC_LIB hGraphicLib, const ST_DSP_LINE txLine[], ushort usMaxLine, T_GL_ENCODING_CHARSET eCharSet)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hWindow, hTimer, hLayout, hLabel, hLayout2, hButtonVal, hButtonCan, hButtonDown, hButtonUp;
	bool bTouch;
	ushort usLine;

	// Build screen context with MAX labels and 4 buttons (Cancel, Valid, Down and Up)
	// *******************************************************************************
	GL_GraphicLib_SetCharset(hGraphicLib, eCharSet);                                  // Change the encoding charset of the graphic library

	bTouch = GL_GraphicLib_IsTouchPresent(hGraphicLib);                               // Is it a touch screen?

	hWindow = GL_Window_Create(hGraphicLib);                                          // Create a window in the user area
	CHECK(hWindow!=NULL, lblKO);

	hTimer = GL_Timer_Create(hWindow);                                                // Create a timer on this window
	CHECK(hTimer!=NULL, lblKO);
	GL_Widget_SetId(hTimer, TIMER_ID);                                                // Timer Identifier TIMER_ID
	GL_Widget_RegisterCallback(hTimer, GL_EVENT_TIMER_OUT, CallBackTimeout);          // Call back when the timer expired

	hLayout = GL_Layout_Create(hWindow);                                              // Create a layout on this window
	CHECK(hLayout!=NULL, lblKO);

	for (usLine=0; usLine<usMaxLine; usLine++)                                        // Build labels on this layout
	{
		hLabel = GL_Label_Create(hLayout);                                            // Create a label on this layout
		CHECK(hLabel!=NULL, lblKO);
		GL_Widget_SetItem(hLabel, 0, usLine);                                         // Label location on this layout
		GL_Widget_SetText(hLabel, NULL);                                              // Clear text on this label
		GL_Widget_SetShrink(hLabel, GL_DIRECTION_WIDTH);                              // Shrink beyond its size (No width scrolling cell if label goes over screen)
		if (IsColorDisplay())                                                         // Color display?
			GL_Widget_SetFontScale(hLabel, txLine[usLine].xColor.xFont.eScale);       // Yes, font scale color
		else
			GL_Widget_SetFontScale(hLabel, txLine[usLine].xBW.xFont.eScale);          // No, font scale B&W
		GL_Widget_SetId(hLabel, LABEL_0_ID+usLine);                                   // Label identifier LABEL0_ID + (0..8)
	}

	hLayout2 = GL_Layout_Create(hLayout);                                             // Create a second layout on this layout
	CHECK(hLayout2!=NULL, lblKO);
	GL_Widget_SetId(hLayout2, LAYOUT2_ID);                                            // Second layout identifier
	GL_Widget_SetItem(hLayout2, 0, usLine);                                           // Second layout location on this layout
	GL_Widget_SetGrow(hLayout2, GL_DIRECTION_WIDTH);                                  // Grow beyond its size (Background width around layout will fill the complete cell)

	hButtonCan = GL_Button_Create(hLayout2);                                          // Create a cancel button on this second layout
	CHECK(hButtonCan!=NULL, lblKO);
	GL_Widget_SetSource(hButtonCan, "skin://images/cancel.wgu");                      // Set the cancel skinned image
	GL_Widget_SetItem(hButtonCan, 0, 0);                                              // Button location on this layout
	GL_Widget_SetExpand(hButtonCan, GL_DIRECTION_WIDTH);                              // Expand button width
	GL_Widget_SetShortcut(hButtonCan, GL_KEY_CANCEL);                                 // Shortcut on this button
	GL_Widget_SetId(hButtonCan, BUTTON_CAN_ID);                                       // Cancel button Identifier
	GL_Widget_SetGrow(hButtonCan, GL_DIRECTION_WIDTH);                                // Grow beyond its size (Background width around cancel button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonCan, false);                                        // No focus on this button
	GL_Widget_RegisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Call back when key pressed

	hButtonDown = GL_Button_Create(hLayout2);                                         // Create a down button on this layout
	CHECK(hButtonDown!=NULL, lblKO);
	GL_Widget_SetSource(hButtonDown, "skin://images/downArrow.wgu");                  // Set the down arrow skinned image
	GL_Widget_SetItem(hButtonDown, 1, 0);                                             // Button location on this layout
	GL_Widget_SetExpand(hButtonDown, GL_DIRECTION_WIDTH);                             // Expand button width
	GL_Widget_SetForeColor(hButtonDown, GL_COLOR_WHITE);                              // Foreground color
	if (bTouch)
		GL_Widget_SetShortcut(hButtonDown, GL_KEY_DOT);                               // Shortcut on this button
	else
		GL_Widget_SetShortcut(hButtonDown, GL_KEY_DOWN);                              // Shortcut on this button
	GL_Widget_SetId(hButtonDown, BUTTON_DOWN_ID);                                     // Down button Identifier
	GL_Widget_SetGrow(hButtonDown, GL_DIRECTION_WIDTH);                               // Grow beyond its size (Background width around down button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonDown, false);                                       // No focus on this button
	GL_Widget_RegisterCallback(hButtonDown, GL_EVENT_STYLUS_CLICK, CallBackDownKey);  // Call back when key pressed

	hButtonUp = GL_Button_Create(hLayout2);                                           // Create a up button on this layout
	CHECK(hButtonUp!=NULL, lblKO);
	GL_Widget_SetSource(hButtonUp, "skin://images/upArrow.wgu");                      // Set the up arrow skinned image
	GL_Widget_SetItem(hButtonUp, 2, 0);                                               // Button location on this layout
	GL_Widget_SetExpand(hButtonUp, GL_DIRECTION_WIDTH);                               // Expand button width
	GL_Widget_SetForeColor(hButtonUp, GL_COLOR_WHITE);                                // Foreground color
	if (bTouch)                                                                       // Touch screen?
		GL_Widget_SetShortcut(hButtonUp, GL_KEY_F);                                   // Yes, Shortcut (Key F) on this button
	else
		GL_Widget_SetShortcut(hButtonUp, GL_KEY_UP);                                  // No, Shortcut (Key Up) on this button
	GL_Widget_SetId(hButtonUp, BUTTON_UP_ID);                                         // Up button Identifier
	GL_Widget_SetGrow(hButtonUp, GL_DIRECTION_WIDTH);                                 // Grow beyond its size (Background width around up button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonUp, false);                                         // No focus on this button
	GL_Widget_RegisterCallback(hButtonUp, GL_EVENT_STYLUS_CLICK, CallBackUpKey);      // Call back when key pressed

	hButtonVal = GL_Button_Create(hLayout2);                                          // Create a valid button on this second layout
	CHECK(hButtonVal!=NULL, lblKO);
	GL_Widget_SetSource(hButtonVal, "skin://images/validation.wgu");                  // Set the valid skinned image
	GL_Widget_SetItem(hButtonVal, 3, 0);                                              // Button location on this layout
	GL_Widget_SetExpand(hButtonVal, GL_DIRECTION_WIDTH);                              // Expand button width
	GL_Widget_SetShortcut(hButtonVal, GL_KEY_VALID);                                  // Shortcut on this button
	GL_Widget_SetId(hButtonVal, BUTTON_VAL_ID);                                       // Valid button Identifier
	GL_Widget_SetGrow(hButtonVal, GL_DIRECTION_WIDTH);                                // Grow beyond its size (Background width around valid button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonVal, false);                                        // No focus on this button
	GL_Widget_RegisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey);  // Call back when key pressed

	if(!bTouch)                                                                       // Touch screen?
		GL_Widget_SetVisible(hLayout2, false);                                        // No, do not show second layout (keep shortcut access on key pressed)

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                                // Window creation failed
	if (hWindow)
	{
		GL_Widget_Destroy(hWindow);                                                   // Destroy window and all widgets
		hWindow=NULL;
	}
	goto lblEnd;
lblEnd:
	return hWindow;
}

//****************************************************************************
//       int GoalClrScreen(T_GL_HWIDGET hWindow, T_GL_COLOR ulBackColor,
//                         unsigned int uiKeys, bool bRefresh)
// This function clears the screen context, can change the background color
//  and sets the shortcut keys needed (Cancel, Down, Up and Valid).
//  Background color is forced to white for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   ulBackColor (I-) : Screen background color.
//   uiKeys (I-) : Shortcut keys (exp: KEY_CANCEL | KEY_VALID)
//                   Cancel Key (KEY_CANCEL)
//                   Down Key (KEY_DOWN)
//                   Up Key (KEY_UP)
//                   Valid Key (KEY_VALID)
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Clear screen done.
//   <0 : Clear screen failed.
//****************************************************************************

int GoalClrScreen(T_GL_HWIDGET hWindow, T_GL_COLOR ulBackColor, unsigned int uiKeys, bool bRefresh)
{
	// Local variables
    // ***************
	ushort usIdx;
	T_GL_HWIDGET hLabel, hButtonVal, hButtonCan, hButtonDown, hButtonUp;
	int iRet;

	// Disable the complete second layout
	// **********************************
	hButtonCan = GL_Widget_SearchId(hWindow, BUTTON_CAN_ID);      // Retrieve the cancel button on this second layout
	CHECK(hButtonCan!=NULL, lblKO);
	hButtonDown = GL_Widget_SearchId(hWindow, BUTTON_DOWN_ID);    // Retrieve the down button on this second layout
	CHECK(hButtonDown!=NULL, lblKO);
	hButtonUp = GL_Widget_SearchId(hWindow, BUTTON_UP_ID);        // Retrieve the up button on this second layout
	CHECK(hButtonUp!=NULL, lblKO);
	hButtonVal = GL_Widget_SearchId(hWindow, BUTTON_VAL_ID);      // Retrieve the valid button on this second layout
	CHECK(hButtonVal!=NULL, lblKO);

	GL_Widget_SetVisible(hButtonCan, false);                      // Blind the cancel button
	GL_Widget_UnregisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Unregister call back on cancel button

	GL_Widget_SetVisible(hButtonDown, false);                     // Blind the down button
	GL_Widget_UnregisterCallback(hButtonDown, GL_EVENT_STYLUS_CLICK, CallBackDownKey); // Unregister call back on down button

	GL_Widget_SetVisible(hButtonUp, false);                       // Blind the up button
	GL_Widget_UnregisterCallback(hButtonUp, GL_EVENT_STYLUS_CLICK, CallBackUpKey); // Unregister call back on up button

	GL_Widget_SetVisible(hButtonVal, false);                      // Blind the valid button
	GL_Widget_UnregisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Unregister call back on valid button

	// Set the second layout with the right properties
	// ***********************************************
	if ((uiKeys & KEY_CANCEL) == KEY_CANCEL)                      // Cancel key?
	{
		GL_Widget_SetVisible(hButtonCan, true);                   // Yes, show the cancel key on this second layout
		GL_Widget_RegisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Register call back when cancel key pressed or touched
	}

	if ((uiKeys & KEY_DOWN) == KEY_DOWN)                          // Down key?
	{
		GL_Widget_SetVisible(hButtonDown, true);                  // Yes, show the down key on this second layout
		GL_Widget_RegisterCallback(hButtonDown, GL_EVENT_STYLUS_CLICK, CallBackDownKey); // Register call back when down key pressed or touched
	}

	if ((uiKeys & KEY_UP) == KEY_UP)                              // Up key?
	{
		GL_Widget_SetVisible(hButtonUp, true);                    // Yes, show the up key on this second layout
		GL_Widget_RegisterCallback(hButtonUp, GL_EVENT_STYLUS_CLICK, CallBackUpKey); // Register call back when up key pressed or touched
	}

	if ((uiKeys & KEY_VALID) == KEY_VALID)                        // Valid key?
	{
		GL_Widget_SetVisible(hButtonVal, true);                   // Yes, show the valid key on this second layout
		GL_Widget_RegisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Register call back when key pressed or touched
	}

	if (IsColorDisplay())                                         // Color display?
		GL_Widget_SetBackColor(hWindow, ulBackColor);             // Yes, set window back color
	else
	{
		ulBackColor = GL_COLOR_WHITE;                             // No, force window back color to white
		GL_Widget_SetBackColor(hWindow, ulBackColor);
	}

	// Clear screen
	// ************
	usIdx=0;
	while(1)
	{
		hLabel = GL_Widget_SearchId(hWindow, LABEL_0_ID+usIdx++);  // Get label from its id
		if(hLabel == NULL)
			break;
		GL_Widget_SetText(hLabel, NULL);                          // Remove label text
		GL_Widget_SetBorders(hLabel, 0, 0, 0, 0, GL_UNIT_PIXEL, ulBackColor); // Remove border with window back color
		GL_Widget_SetBackColor(hLabel, ulBackColor);              // Paint label back color with window back color
	}

	if (bRefresh)                                                 // Refresh window?
		GL_Window_Dispatch(hWindow, 0);                           // Yes, refresh it

	iRet=1;
	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                            // Clear screen failed
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//                void GoalRefreshScreen (T_GL_HWIDGET hWindow)
// This function refreshes the screen context.
// This function has parameters.
//   hWindow (I-) : Window context handle.
// This function has return value.
//****************************************************************************

void GoalRefreshScreen(T_GL_HWIDGET hWindow)
{
	// Local variables
    // ***************
		// Empty

	// Refresh screen
	// **************
	GL_Window_Dispatch(hWindow, 0);  // Refresh the window and dispatch all messages received
}

//****************************************************************************
//    int GoalDspLine(T_GL_HWIDGET hWindow, ushort usLine, char *pcText,
//                    const ST_DSP_LINE* pxLine, ulong ulTimeOut, bool bRefresh)
// This function displays a string on screen for B&W terminal or color terminal.
//  The line has the following properties:
//   - Text Align, Background Align, Pen/Brush Color, Reverse mode,
//   - Background size (width), Background grow size (width),
//   - Text padding, Border, Margin, Font
//  A Time-out to exit from the screen.
//  Refresh or not the screen (if Time-out!=0 screen always refreshed).
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   usLine (I-) : Line number.
//   pcText (I-) : Text to display.
//   pxLine (I-) : Line properties.
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Display line done (Exit screen: 0=timeout,GL_KEY_CANCEL,GL_KEY_VALID).
//   <0 : Display line failed.
//****************************************************************************

int GoalDspLine(T_GL_HWIDGET hWindow, ushort usLine, char *pcText, const ST_DSP_LINE* pxLine, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hLabel;
	int iRet;

	// Display line
	// ************
	CHECK(pxLine!=NULL, lblKO);

	hLabel = GL_Widget_SearchId(hWindow, LABEL_0_ID+usLine); // Get label from its id
	CHECK(hLabel!=NULL, lblKO);

	if (pcText)                                              // Null text?
	{
		GL_Widget_SetVisible(hLabel, true);
		GL_Widget_SetText(hLabel, "%s", pcText);             // No, show text (add %s to allow '%' to be displayed)
	}
	else
		GL_Widget_SetVisible(hLabel, false);                 // Yes, blind text (line is removed)

	if (IsColorDisplay())                                    // Color display?
	{                                                        // Yes
		// Color properties
		// ================
		GL_Widget_SetTextAlign(hLabel, pxLine->xColor.eTextAlign);      // Text alignment
		GL_Widget_SetBackAlign(hLabel, pxLine->xColor.eBackAlign);      // Background alignment
		GL_Widget_SetForeColor(hLabel, pxLine->xColor.ulPen);           // Pen color
		GL_Widget_SetBackColor(hLabel, pxLine->xColor.ulBrush);         // Brush color
		if (pxLine->xColor.usSizeWidth < 99)                            // Background width size
			GL_Widget_SetMinSize(hLabel, pxLine->xColor.usSizeWidth, 0, GL_UNIT_PERCENT); // The size
		else
			GL_Widget_SetExpand(hLabel, GL_DIRECTION_WIDTH);                              // Full size
		if (pxLine->xColor.bGrowWidth)                                  // Grow beyond its size
			GL_Widget_SetGrow(hLabel, GL_DIRECTION_WIDTH);              // Background width around the label will fill the complete cell
			                                                            // Paddings, Borders and Margins
		GL_Widget_SetTextPaddings(hLabel, pxLine->xColor.xPadding.usLeft, pxLine->xColor.xPadding.usTop, pxLine->xColor.xPadding.usRight, pxLine->xColor.xPadding.usBottom, GL_UNIT_PERCENT);
		GL_Widget_SetBorders(hLabel, pxLine->xColor.xBorder.usLeft, pxLine->xColor.xBorder.usTop, pxLine->xColor.xBorder.usRight, pxLine->xColor.xBorder.usBottom, GL_UNIT_PIXEL, pxLine->xColor.xBorder.ulColor);
		GL_Widget_SetMargins(hLabel, pxLine->xColor.xMargin.usLeft, pxLine->xColor.xMargin.usTop, pxLine->xColor.xMargin.usRight, pxLine->xColor.xMargin.usBottom, GL_UNIT_PIXEL);
		if (pxLine->xColor.xFont.pcName != NULL)
			GL_Widget_SetFontName(hLabel, pxLine->xColor.xFont.pcName); // Font name
		GL_Widget_SetFontScale(hLabel, pxLine->xColor.xFont.eScale);    // Font scale
		GL_Widget_SetFontStyle(hLabel, pxLine->xColor.xFont.eStyle);    // Font style
	}
	else                                                     // No
	{
		// Black & White properties
		// ========================
		GL_Widget_SetTextAlign(hLabel, pxLine->xBW.eTextAlign);         // Text alignment
		GL_Widget_SetBackAlign(hLabel, pxLine->xBW.eBackAlign);         // Background alignment
		if (!pxLine->xBW.bReverse)                                      // B&W reverse?
		{                                                               // Yes
			GL_Widget_SetForeColor(hLabel, GL_COLOR_BLACK);             // Force pen color to black and brush color to white
			GL_Widget_SetBackColor(hLabel, GL_COLOR_WHITE);
		}
		else                                                            // No
		{
			GL_Widget_SetForeColor(hLabel, GL_COLOR_WHITE);             // Force pen color to white and brush color to black
			GL_Widget_SetBackColor(hLabel, GL_COLOR_BLACK);
		}
		if (pxLine->xBW.usSizeWidth < 99)                               // Background width size
			GL_Widget_SetMinSize(hLabel, pxLine->xBW.usSizeWidth, 0, GL_UNIT_PERCENT); // The size
		else
			GL_Widget_SetExpand(hLabel, GL_DIRECTION_WIDTH);                           // Full size
		if (pxLine->xBW.bGrowWidth)                                     // Grow beyond its size
			GL_Widget_SetGrow(hLabel, GL_DIRECTION_WIDTH);              // Background width around the label will fill the complete cell
			                                                            // Paddings, Borders and Margins
		GL_Widget_SetTextPaddings(hLabel, pxLine->xBW.xPadding.usLeft, pxLine->xBW.xPadding.usTop, pxLine->xBW.xPadding.usRight, pxLine->xBW.xPadding.usBottom, GL_UNIT_PERCENT);
		if (pxLine->xBW.xBorder.ulColor != GL_COLOR_BLACK)              // Force border to black or white
			GL_Widget_SetBorders(hLabel, pxLine->xBW.xBorder.usLeft, pxLine->xBW.xBorder.usTop, pxLine->xBW.xBorder.usRight, pxLine->xBW.xBorder.usBottom, GL_UNIT_PIXEL, GL_COLOR_WHITE);
		else
			GL_Widget_SetBorders(hLabel, pxLine->xBW.xBorder.usLeft, pxLine->xBW.xBorder.usTop, pxLine->xBW.xBorder.usRight, pxLine->xBW.xBorder.usBottom, GL_UNIT_PIXEL, GL_COLOR_BLACK);
		GL_Widget_SetMargins(hLabel, pxLine->xBW.xMargin.usLeft, pxLine->xBW.xMargin.usTop, pxLine->xBW.xMargin.usRight, pxLine->xBW.xMargin.usBottom, GL_UNIT_PIXEL);
		if (pxLine->xBW.xFont.pcName != NULL)
			GL_Widget_SetFontName(hLabel, pxLine->xBW.xFont.pcName);    // Font name
		GL_Widget_SetFontScale(hLabel, pxLine->xBW.xFont.eScale);       // Font scale
		GL_Widget_SetFontStyle(hLabel, pxLine->xBW.xFont.eStyle);       // Font style
	}

	CHECK(ulTimeOut!=0, lblNoTimer)                          // No timer to set

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                       // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF);   // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
	GL_Window_MainLoop(hWindow);                             // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                     // Get the result
	{
	case RES_VALID_STATUS:                                   // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                                  // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_TIMER_STATUS:                                   // Timer result
		iRet=0;
		break;
	default:                                                 // Unknown result
		iRet=-1;
		break;
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblNoTimer:                                                  // No timer used
	iRet=0;
	if (bRefresh)
	{
		iRet = GL_Window_Dispatch(hWindow, 0);               // Refresh the window
		if (iRet == RES_VALID_STATUS)
			iRet=GL_KEY_VALID;
		if (iRet == RES_CANCEL_STATUS)
			iRet=GL_KEY_CANCEL;
	}
	goto lblEnd;
lblKO:                                                       // Display line failed
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//    int GoalDspMultiLine(T_GL_HWIDGET hWindow, ushort usLine, char *pcData,
//                         int iLen, const ST_LINE txLine[], ulong ulTimeOut,
//                         bool bRefresh)
//  This function sends a data buffer to the display.
//   The buffer is divided on lines of 100 bytes max except the last line.
//   Each line is automatically displayed on screen. The '\n' automatically
//   switches to the next line.
//   The function can display up to max line defined on screen creation.
//  This function has parameters:
//    hWindow (I-) : Window context handle.
//    usLine (I-) : Line number.
//    pcData (I-) : Data to display.
//    iLen (I-) : Length of data.
//    ulTimeOut (I-) : Timeout in milliseconds.
//    bRefresh (I-) : Refresh or not the screen.
//  This function has return value.
//    >=0 : Display buffer done (Exit screen : 0=timeout,GL_KEY_CANCEL,GL_KEY_VALID).
//    <0 : Display buffer failed.
//****************************************************************************

int GoalDspMultiLine(T_GL_HWIDGET hWindow, ushort usLine, char *pcData , int iLen, const ST_DSP_LINE txLine[], ushort usMaxLine, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	ST_DSP_LINE xLine;
	int iCount=0;
	char tcBuffer[100+1];
	int iRet=-1;

	// Printing in progress
	// ********************
	CHECK ((pcData!=NULL) && (iLen>=0), lblKO);

	memset(tcBuffer, 0, sizeof(tcBuffer));
	while(usLine < usMaxLine)
	{
		if (iLen > 0)               // Check if it is the end of data
		{                           // No, it's a line with '\n' detected
			if (*pcData != '\n')    // Save character into buffer
			{
				if (iCount < 100)
				{
					tcBuffer[iCount++] = *pcData++;
					tcBuffer[iCount]=0;
				}
				iLen--;
			}
			else                    // '\n' detected
			{
				xLine = txLine[usLine];
				if (usLine == usMaxLine-1)
				{                   // Last line to display
					iRet = GoalDspLine(hWindow, usLine++, tcBuffer, &xLine, ulTimeOut, bRefresh);
					CHECK(iRet>=0, lblKO);
				}
				else
				{                   // Next line to display
					iRet = GoalDspLine(hWindow, usLine++, tcBuffer, &xLine, 0, false);
					CHECK(iRet>=0, lblKO);
				}
				iCount=0;
				tcBuffer[iCount]=0;
				pcData++;
				iLen--;
			}
		}
		else
		{                           // Yes, it's the end of data
			xLine = txLine[usLine]; // Display line
			iRet = GoalDspLine(hWindow, usLine++, tcBuffer, &xLine, ulTimeOut, bRefresh);
			CHECK(iRet>=0, lblKO);
			break;                  // Exit from the loop
		}
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                              // Display buffer failed.
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//     int GoalGetKey(T_GL_HWIDGET hWindow, T_GL_HGRAPHIC_LIB hGraphicLib,
//                    ulong ulTimeOut, bool bClear)
// This function waits for shortcut key pressed or touched.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   hGraphicalLib (I-) : Graphic library context handle.
//   bShortcut (I-) : true => Return only shortcut keys pressed
//                    false => Return all keys pressed
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bClear (I-) : true => FIFO touch and keyboard reseted before waiting
//                         into GL_Window_MainLoop.
// This function has return value.
//   >0 : Key value.
//   =0 : Timeout.
//****************************************************************************

int GoalGetKey(T_GL_HWIDGET hWindow, T_GL_HGRAPHIC_LIB hGraphicLib, bool bShortcut, ulong ulTimeOut, bool bClear)
{
	// Local variables
    // ***************
	Telium_File_t *hKeybd=NULL;  // Handle peripheral Keyboard
	Telium_File_t *hTsc=NULL;  // Handle peripheral Touch Screen
#ifndef __TELIUM3__
	bool bTouch;
#endif
	T_GL_WCHAR ulKey;
	int iRet;

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
#ifndef __TELIUM3__
	bTouch = GL_GraphicLib_IsTouchPresent(hGraphicLib);       // Touch screen?
	if (bTouch)
	{                                                         // Yes,
		GL_GraphicLib_UnbindKey(hGraphicLib, GL_KEY_F);       // Unbind "F" key to Telium Key
		GL_GraphicLib_UnbindKey(hGraphicLib, GL_KEY_DOT);     // Unbind "Dot" key to Telium Key
	}
	else
	{                                                         // No,
		GL_GraphicLib_UnbindKey(hGraphicLib, GL_KEY_UP);      // Unbind "Up" key to Telium Key
		GL_GraphicLib_UnbindKey(hGraphicLib, GL_KEY_DOWN);    // Unbind "Down" key to Telium Key
		GL_GraphicLib_UnbindKey(hGraphicLib, GL_KEY_F1);      // Unbind "F1" key to Telium Key
		GL_GraphicLib_UnbindKey(hGraphicLib, GL_KEY_F4);      // Unbind "F4" key to Telium Key
	}
#endif

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                        // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF);          // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Detect key pressed
	// ******************
	if (!bShortcut)                                           // Remove shortcut and enable callback on key pressed?
	{                                                                                     // Yes
		GL_Widget_RegisterCallback(hWindow, GL_EVENT_KEY_DOWN, CallBackPressKey);         // Call back when key pressed
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_CAN_ID), GL_KEY_NONE);   // Remove shortcut from cancel button
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_VAL_ID), GL_KEY_NONE);   // Remove shortcut from valid button
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_DOWN_ID), GL_KEY_NONE);  // Remove shortcut from down button
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_UP_ID), GL_KEY_NONE);    // Remove shortcut from up button
	}

	// Reset FIFO keyboard and touch
	// *****************************
	if (bClear)
	{
		hKeybd = Telium_Stdperif("KEYBOARD", NULL);             // Check "keyboard" peripheral opened?
		if(hKeybd)
			Telium_Reset_buf(hKeybd, _receive_id);              // Yes, reset "keyboard" FIFO

		hTsc = Telium_Stdperif("TSCREEN", NULL);              // Check "touch" peripheral opened?
		if(hTsc)
			Telium_Reset_buf(hTsc, _receive_id);              // Yes, reset "touch" FIFO
	}
	GL_Window_MainLoop(hWindow);                              // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                      // Get the result
	{
	case RES_PRESS_STATUS:                                    // Key press result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		GL_Widget_GetUserData(hWindow, &ulKey, sizeof(ulKey));
		iRet=ulKey;
		break;
	case RES_VALID_STATUS:                                    // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                                   // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_DOWN_STATUS:                                     // Key down result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_DOWN;
		break;
	case RES_UP_STATUS:                                       // Key up result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_UP;
		break;
	case RES_TIMER_STATUS:                                    // Timer result
		iRet=0;
		break;
	default:                                                  // Unknown result
		iRet=0;
		break;
	}

	if (!bShortcut)                                           // Restore shortcut and disable callback on key pressed?
	{                                                                                     // Yes
		GL_Widget_UnregisterCallback(hWindow, GL_EVENT_KEY_DOWN, CallBackPressKey);       // Unregister call back on key pressed
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_CAN_ID), GL_KEY_CANCEL); // Set shortcut on cancel button
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_VAL_ID), GL_KEY_VALID);  // Set shortcut on valid button
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_DOWN_ID), GL_KEY_DOWN);  // Set shortcut on down button
		GL_Widget_SetShortcut(GL_Widget_SearchId(hWindow, BUTTON_UP_ID), GL_KEY_UP);      // Set shortcut on up button
	}

#ifndef __TELIUM3__
	if (bTouch)                                               // Touch screen?
	{                                                                                     // Yes
		GL_GraphicLib_BindSingleKey(hGraphicLib, GL_KEY_F, GL_VIRTUAL_KEY_PREVIOUS);      // Bind "F" key to virtual Key
		GL_GraphicLib_BindSingleKey(hGraphicLib, GL_KEY_DOT, GL_VIRTUAL_KEY_NEXT);        // Bind "Dot" key to virtual Key
	}
	else
	{                                                                                     // No
		GL_GraphicLib_BindSingleKey(hGraphicLib, GL_KEY_UP, GL_VIRTUAL_KEY_PREVIOUS);     // Bind "Up" key to virtual Key
		GL_GraphicLib_BindSingleKey(hGraphicLib, GL_KEY_DOWN, GL_VIRTUAL_KEY_NEXT);       // Bind "Down" key to virtual Key
		GL_GraphicLib_BindSingleKey(hGraphicLib, GL_KEY_F1, GL_VIRTUAL_KEY_LEFT);         // Bind "F1" key to virtual Key
		GL_GraphicLib_BindSingleKey(hGraphicLib, GL_KEY_F4, GL_VIRTUAL_KEY_RIGHT);        // Bind "F4" key to virtual Key
	}
#endif

	return iRet;
}

//****************************************************************************
//            void GoalDestroyScreen(T_GL_HWIDGET *phWindow)
// This function destroys the screen context by releasing all allocated memory
//  and all widgets attached.
//  This call is crucial, if it is forgotten you get a memory leak.
// This function has parameters.
//   phWindow (I-) : Pointer on window context handle.
// This function has no return value.
//****************************************************************************

void GoalDestroyScreen(T_GL_HWIDGET *phWindow)
{
	// Local variables
    // ***************
		// Empty

	// Destroy screen
	// **************
	GL_Widget_Destroy(*phWindow); // Destroy window with all widgets attached
	*phWindow=NULL;
}

//##############################################################################
//                                   DRAWING
//##############################################################################

//****************************************************************************
//      T_GL_HWIDGET GoalCreateDrawing (T_GL_HGRAPHIC_LIB hGraphicLib,
//                                      T_GL_ENCODING_CHARSET eCHarSet)
// This function creates a drawing context which is composed of:
//  - A timer to exit from this screen.
//  - A drawing window.
//  - Shortcut keys on 2 buttons to react on this screen:
//      Cancel Key and Valid Key
//  - Encoding charsets:
//      GL_ENCODING_UTF8 => Unicode utf8
//      GL_ENCODING_ISO_8859_X => ISO 8859
// This function has parameters.
//   hGraphicalLib (I-) : Graphic library context handle.
//   eCharSet (I-) : Encoding charsets.
// This function has return value.
//   !NULL : Pointer to the handle of the drawing.
//   NULL : Drawing failed
//****************************************************************************

T_GL_HWIDGET GoalCreateDrawing (T_GL_HGRAPHIC_LIB hGraphicLib, T_GL_ENCODING_CHARSET eCharSet)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hWindow, hTimer, hLayout, hDrawing, hLayout2, hButtonVal, hButtonCan;
	bool bTouch=FALSE;

	// Build drawing context with 2 buttons (Cancel and Valid)
	// *******************************************************
    GL_GraphicLib_SetCharset(hGraphicLib, eCharSet);                  // Change the encoding charset of the graphic library

	bTouch = GL_GraphicLib_IsTouchPresent(hGraphicLib);               // Touch screen?

	hWindow = GL_Window_Create(hGraphicLib);                          // Create a window in a user area
	CHECK(hWindow!=NULL, lblEnd);

	hTimer = GL_Timer_Create(hWindow);                                // Create a timer on this window
	CHECK(hTimer!=NULL, lblKO);
	GL_Widget_SetId(hTimer, TIMER_ID);                                // Timer Identifier TIMER_ID
	GL_Widget_RegisterCallback(hTimer, GL_EVENT_TIMER_OUT, CallBackTimeout); // Call back when the timer expired

	hLayout = GL_Layout_Create(hWindow);                              // Create a layout on this window
	CHECK(hLayout!=NULL, lblKO);

	hDrawing = GL_Drawing_Create(hLayout);                            // Create a drawing on this layout
	CHECK(hDrawing!=NULL, lblKO);
	GL_Widget_SetId(hDrawing, DRAWING_ID);                            // Drawing Identifier DRAWING_ID
	GL_Widget_SetItem(hDrawing, 0, 0);                                // Drawing location on this layout

	hLayout2 = GL_Layout_Create(hLayout);                             // Create a second layout on this layout
	CHECK(hLayout2!=NULL, lblKO);
	GL_Widget_SetId(hLayout2, LAYOUT2_ID);                            // Second layout identifier
	GL_Widget_SetItem(hLayout2, 0, 1);                                // Second layout location on this layout
	GL_Widget_SetGrow(hLayout2, GL_DIRECTION_WIDTH);                  // Grow beyond its size (Background width around layout will fill the complete cell)

	hButtonCan = GL_Button_Create(hLayout2);                          // Create a cancel button on this second layout
	CHECK(hButtonCan!=NULL, lblKO);
	GL_Widget_SetSource(hButtonCan, "skin://images/cancel.wgu");      // Set the cancel skinned image
	GL_Widget_SetItem(hButtonCan, 0, 0);                              // Button location on this layout
	GL_Widget_SetExpand(hButtonCan, GL_DIRECTION_WIDTH);              // Expand button width
	GL_Widget_SetShortcut(hButtonCan, GL_KEY_CANCEL);                 // Shortcut on this button
	GL_Widget_SetId(hButtonCan, BUTTON_CAN_ID);                       // Cancel button Identifier
	GL_Widget_SetGrow(hButtonCan, GL_DIRECTION_WIDTH);                // Grow beyond its size (Background width around cancel button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonCan, false);                        // No focus on this button
	GL_Widget_RegisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Call back when key pressed

	hButtonVal = GL_Button_Create(hLayout2);                          // Create a valid button on this second layout
	CHECK(hButtonVal!=NULL, lblKO);
	GL_Widget_SetSource(hButtonVal, "skin://images/validation.wgu");  // Set the valid skinned image
	GL_Widget_SetItem(hButtonVal, 1, 0);                              // Button location on this layout
	GL_Widget_SetExpand(hButtonVal, GL_DIRECTION_WIDTH);              // Expand button width
	GL_Widget_SetShortcut(hButtonVal, GL_KEY_VALID);                  // Shortcut on this button
	GL_Widget_SetId(hButtonVal, BUTTON_VAL_ID);                       // Valid button Identifier
	GL_Widget_SetGrow(hButtonVal, GL_DIRECTION_WIDTH);                // Grow beyond its size (Background width around valid button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonVal, false);                        // No focus on this button
	GL_Widget_RegisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Call back when key pressed

	if(!bTouch)                                                       // Touch screen?
		GL_Widget_SetVisible(hLayout2, false);                        // No, do not show second layout (keep shortcut access on key pressed)

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                // Window creation failed
	if (hWindow)
	{
		GL_Widget_Destroy(hWindow);                                   // Destroy window and all widgets
		hWindow=NULL;
	}
	goto lblEnd;
lblEnd:
	return hWindow;
}

//****************************************************************************
//        int GoalClrDrawing(T_GL_HWIDGET hWindow, T_GL_COLOR ulBackColor,
//                           unsigned int uiKeys, bool bRefresh)
// This function clears the drawing context, can change the background color
//  and sets the shortcut keys needed (Cancel and Valid).
//  Background color is forced to white for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   ulBackColor (I-) : Drawing background color.
//   uiKeys (I-) : Shortcut keys (exp: KEY_CANCEL | KEY_VALID)
//                   Cancel Key (KEY_CANCEL)
//                   Valid Key (KEY_VALID)
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Clear drawing done.
//   <0 : Clear drawing failed.
//****************************************************************************

int GoalClrDrawing(T_GL_HWIDGET hWindow, T_GL_COLOR ulBackColor, unsigned int uiKeys, bool bRefresh)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hButtonVal, hButtonCan;
	int iRet;

	// Disable the complete second layout
	// **********************************
	hButtonCan = GL_Widget_SearchId(hWindow, BUTTON_CAN_ID);     // Retrieve the cancel button on this second layout
	CHECK(hButtonCan!=NULL, lblKO);
	hButtonVal = GL_Widget_SearchId(hWindow, BUTTON_VAL_ID);     // Retrieve the valid button on this second layout
	CHECK(hButtonVal!=NULL, lblKO);

	GL_Widget_SetVisible(hButtonCan, false);                     // Blind the cancel button
	GL_Widget_UnregisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Unregister call back on cancel button
	GL_Widget_SetVisible(hButtonVal, false);                     // Blind the valid button
	GL_Widget_UnregisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Unregister call back on valid button

	// Set the second layout with the right properties
	// ***********************************************
	if ((uiKeys & KEY_CANCEL) == KEY_CANCEL)                     // Cancel key?
	{
		GL_Widget_SetVisible(hButtonCan, true);                  // Yes, show the cancel key on this second layout
		GL_Widget_RegisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Register call back when cancel key pressed or touched
	}

	if ((uiKeys & KEY_VALID) == KEY_VALID)                       // Valid key?
	{
		GL_Widget_SetVisible(hButtonVal, true);                  // Yes, show the valid key on this second layout
		GL_Widget_RegisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Register call back when valid key pressed or touched
	}

	if (IsColorDisplay())                                        // Color display?
		GL_Widget_SetBackColor(hWindow, ulBackColor);            // Yes, set window back color
	else
		GL_Widget_SetBackColor(hWindow, GL_COLOR_WHITE);         // No, force window back color to white

	// Clear drawing
	// *************
	GL_Drawing_Clear(GL_Widget_SearchId(hWindow, DRAWING_ID));   // Clear drawing

	if (bRefresh)                                                // Refresh window?
		GL_Window_Dispatch(hWindow, 0);                          // Yes, refresh it

	iRet=1;
	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                           // Clear drawing failed
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//    int GoalDrawLine(T_GL_HWIDGET hWindow, T_GL_COLOR ulPen,
//                     T_GL_COORD sX1, T_GL_COORD sY1,
//                     T_GL_COORD sX2, T_GL_COORD sY2,
//                     ulong ulTimeOut, bool bRefresh)
// This function draws a line on screen for B&W terminal or color terminal.
//  The drawing line has the following properties:
//   - Pen color, Coordinates (X1,Y1,X2,Y2)
//  A Time-out to exit from the screen.
//  Refresh or not the screen.
//  Pen color is forced to black for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   ulPen (I-) : Pen color.
//   sX1 (I-), sY1(I-) : Coordinates of the first point.
//   sX2 (I-), sY2 (I-) : Coordinates of the second point.
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Drawing line done (Exit screen : 0=timeout,GL_KEY_CANCEL,GL_KEY_VALID).
//   <0 : Drawing line failed.
//****************************************************************************

int GoalDrawLine(T_GL_HWIDGET hWindow, T_GL_COLOR ulPen, T_GL_COORD sX1, T_GL_COORD sY1, T_GL_COORD sX2, T_GL_COORD sY2, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	int iRet;

	// Drawing line
	// ************
	GL_Drawing_SetPen(GL_Widget_SearchId(hWindow, DRAWING_ID), ulPen, 1, GL_UNIT_PIXEL); // Set pen color and stroke (only 1 pixel supported)
	GL_Drawing_DrawLine(GL_Widget_SearchId(hWindow, DRAWING_ID), sX1, sY1, sX2, sY2, GL_UNIT_PERCENT); // Draw line with pen

	CHECK(ulTimeOut!=0, lblNoTimer);                       // No timer to set

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                     // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF); // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
	GL_Window_MainLoop(hWindow);                           // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                   // Get the result
	{
	case RES_VALID_STATUS:                                 // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                                // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_TIMER_STATUS:                                 // Timer result
		iRet=0;
		break;
	default:                                               // Unknown result
		iRet=-1;
		break;
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblNoTimer:                                                // No timer used
	iRet=0;
	if (bRefresh)
	{
		iRet = GL_Window_Dispatch(hWindow, 0);             // Refresh the window
		if (iRet == RES_VALID_STATUS)
			iRet=GL_KEY_VALID;
		if (iRet == RES_CANCEL_STATUS)
			iRet=GL_KEY_CANCEL;
	}
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//  int GoalDrawRect(T_GL_HWIDGET hWindow, T_GL_COLOR ulPen, T_GL_COLOR ulBrush,
//                   T_GL_COORD sX, T_GL_COORD sY,
//                   T_GL_COORD sWidth, T_GL_COORD sHeight,
//                   ulong ulTimeOut, bool bRefresh)
// This function draws a rectangle on screen for B&W terminal or color terminal.
//  The drawing rectangle has the following properties:
//   - Pen/Brush color, Coordinates (X,Y,Width, Height)
//  A Time-out to exit from the screen.
//  Refresh or not the screen.
//  Pen=black and Brush=white for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   ulPen (I-), ulBrush (I-) : Pen and brush color.
//   sX (I-), sY(I-) : Coordinates of the first point (X,Y).
//   sWidth (I-), sHeight (I-) : Coordinates of the second point (Width, Height).
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Drawing rectangle done (Exit screen : 0=timeout,GL_KEY_CANCEL,GL_KEY_VALID).
//   <0 : Drawing rectangle failed.
//****************************************************************************

int GoalDrawRect(T_GL_HWIDGET hWindow, T_GL_COLOR ulPen, T_GL_COLOR ulBrush, T_GL_COORD sX, T_GL_COORD sY, T_GL_COORD sWidth, T_GL_COORD sHeight, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	int iRet;

	// Drawing rectangle
	// *****************
	GL_Drawing_SetPen(GL_Widget_SearchId(hWindow, DRAWING_ID), ulPen, 1, GL_UNIT_PIXEL); // Set pen color and stroke (only 1 pixel supported)
	GL_Drawing_SetBrush(GL_Widget_SearchId(hWindow, DRAWING_ID), ulBrush);               // Set brush color
	GL_Drawing_DrawRect(GL_Widget_SearchId(hWindow, DRAWING_ID), sX, sY, sWidth, sHeight, GL_UNIT_PERCENT); // Draw rectangle with pen and brush

	CHECK(ulTimeOut!=0, lblNoTimer);                       // No timer to set

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                     // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF); // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
	GL_Window_MainLoop(hWindow);                           // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                   // Get the result
	{
	case RES_VALID_STATUS:                                 // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                                // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_TIMER_STATUS:                                 // Timer result
		iRet=0;
		break;
	default:                                               // Unknown result
		iRet=-1;
		break;
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblNoTimer:                                                // No timer used
	iRet=0;
	if (bRefresh)
	{
		iRet = GL_Window_Dispatch(hWindow, 0);             // Refresh the window
		if (iRet == RES_VALID_STATUS)
			iRet=GL_KEY_VALID;
		if (iRet == RES_CANCEL_STATUS)
			iRet=GL_KEY_CANCEL;
	}
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//  int GoalDrawEllipse(T_GL_HWIDGET hWindow, T_GL_COLOR ulPen, T_GL_COLOR ulBrush,
//                      T_GL_COORD sX, T_GL_COORD sY,
//                      T_GL_DIM usRadiusX, T_GL_DIM usRadiusY,
//                      ulong ulTimeOut, bool bRefresh)
// This function draws an ellipse on screen for B&W terminal or color terminal.
//  The drawing elipse has the following properties:
//   - Pen/Brush color, Coordinates (X,Y,RadiusX,RadiusY)
//  A Time-out to exit from the screen.
//  Refresh or not the screen.
//  Pen=black and Brush=white for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   ulPen (I-), ulBrush (I-) : Pen and brush color.
//   sX (I-), sY(I-) : Coordinates of the first point (X,Y).
//   usRadiusX (I-), usRadiusY (I-) : Coordinates of the second point (RadiusX, RadiusY).
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Drawing ellipse done (Exit screen : 0=timeout,GEL_KEY_CANCEL,GL_KEY_VALID).
//   <0 : Drawing ellipse failed.
//****************************************************************************

int GoalDrawEllipse(T_GL_HWIDGET hWindow, T_GL_COLOR ulPen, T_GL_COLOR ulBrush, T_GL_COORD sX, T_GL_COORD sY, T_GL_DIM usRadiusX, T_GL_DIM usRadiusY, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	int iRet;

	// Drawing ellipse
	// ***************
	GL_Drawing_SetPen(GL_Widget_SearchId(hWindow, DRAWING_ID), ulPen, 1, GL_UNIT_PIXEL); // Set pen color and stroke (only 1 pixel supported)
	GL_Drawing_SetBrush(GL_Widget_SearchId(hWindow, DRAWING_ID), ulBrush);               // Set brush color
	GL_Drawing_DrawEllipse(GL_Widget_SearchId(hWindow, DRAWING_ID), sX, sY, usRadiusX, usRadiusY, GL_UNIT_PERCENT); // Draw ellipse with pen and brush

	CHECK(ulTimeOut!=0, lblNoTimer);                       // No timer to set

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                     // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF); // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
	GL_Window_MainLoop(hWindow);                           // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                   // Get the result
	{
	case RES_VALID_STATUS:                                 // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                                // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_TIMER_STATUS:                                 // Timer result
		iRet=0;
		break;
	default:                                               // Unknown result
		iRet=-1;
		break;
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblNoTimer:                                                // No timer used
	iRet=0;
	if (bRefresh)
	{
		iRet = GL_Window_Dispatch(hWindow, 0);             // Refresh the window
		if (iRet == RES_VALID_STATUS)
			iRet=GL_KEY_VALID;
		if (iRet == RES_CANCEL_STATUS)
			iRet=GL_KEY_CANCEL;
	}
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//  int GoalDrawText(T_GL_HWIDGET hWindow, const char* pcText,
//                   const ST_DRAW_FONT* pxFont, T_GL_COLOR ulPen,
//                   T_GL_COORD sX, T_GL_COORD sY,
//                   ulong ulTimeOut, bool bRefresh)
// This function draws text on screen for B&W terminal or color terminal.
//  The drawing text has the following properties:
//   - Pen color, Coordinates (X,Y) and font
//  A Time-out to exit from the screen.
//  Refresh or not the screen.
//  Pen color is forced to black for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   pcText (I-) : String to draw.
//   pxFont (I-) : Font properties.
//   ulPen (I-) : Pen color.
//   sX (I-), sY(I-) : Coordinates of the text position.
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Drawing text done (Exit screen : 0=timeout,GL_KEY_CANCEL,GL_KEY_VALID).
//   <0 : Drawing text failed.
//****************************************************************************

int GoalDrawText(T_GL_HWIDGET hWindow, const char* pcText, const ST_DRAW_FONT* pxFont, T_GL_COLOR ulPen, T_GL_COORD sX, T_GL_COORD sY, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	int iRet;

	// Drawing text
	// ************
	GL_Drawing_SetPen(GL_Widget_SearchId(hWindow, DRAWING_ID), ulPen, 1, GL_UNIT_PIXEL);           // Set pen color and stroke (only 1 pixel supported)
	if (pxFont->pcName!=NULL)                              // Customer font?
		GL_Drawing_SetFont(GL_Widget_SearchId(hWindow, DRAWING_ID), pxFont->pcName, pxFont->eStyle, pxFont->usWidth, pxFont->usHeight, GL_UNIT_PERCENT); // Yes, Set customer font
	else
		GL_Drawing_SetFont(GL_Widget_SearchId(hWindow, DRAWING_ID), "GOAL", pxFont->eStyle, pxFont->usWidth, pxFont->usHeight, GL_UNIT_PERCENT); // No, Set default GOAL font
	GL_Drawing_DrawText(GL_Widget_SearchId(hWindow, DRAWING_ID), sX, sY, GL_UNIT_PERCENT, pcText); // Draw text with pen

	CHECK(ulTimeOut!=0, lblNoTimer);                       // No timer to set

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                     // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF); // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
	GL_Window_MainLoop(hWindow);                           // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                   // Get the result
	{
	case RES_VALID_STATUS:                                 // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                                // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_TIMER_STATUS:                                 // Timer result
		iRet=0;
		break;
	default:                                               // Unknown result
		iRet=-1;
		break;
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblNoTimer:                                                // No timer used
	iRet=0;
	if (bRefresh)
	{
		iRet=GL_Window_Dispatch(hWindow, 0);               // Refresh the window
		if (iRet == RES_VALID_STATUS)
			iRet=GL_KEY_VALID;
		if (iRet == RES_CANCEL_STATUS)
			iRet=GL_KEY_CANCEL;
	}
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//          void GoalDestroyDrawing(T_GL_HWIDGET *phWindow)
// This function destroys the drawing context by releasing all allocated memory,
//  all widgets attached.
//  This call is crucial, if it is forgotten you get a memory leak.
// This function has parameters.
//   phWindow (I-) : Pointer on window context handle.
// This function has no return value.
//****************************************************************************

void GoalDestroyDrawing(T_GL_HWIDGET *phWindow)
{
	// Local variables
    // ***************
		// Empty

	// Destroy screen
	// **************
	GL_Widget_Destroy(*phWindow); // Destroy window with all widgets attached
	*phWindow=NULL;
}

//##############################################################################
//                                   PICTURE
//##############################################################################

//****************************************************************************
//      T_GL_HWIDGET GoalCreatePicture (T_GL_HGRAPHIC_LIB hGraphicLib)
// This function creates a picture context which is composed of:
//  - A timer to exit from this screen.
//  - A title line.
//  - A picture window.
//  - Shortcut keys on 2 buttons to react on this screen:
//      Cancel Key and Valid Key
// This function has parameters.
//   hGraphicalLib (I-) : Graphic library context handle.
// This function has return value.
//   !NULL : Pointer to the handle of the picture.
//   NULL : Picture failed
//****************************************************************************

T_GL_HWIDGET GoalCreatePicture (T_GL_HGRAPHIC_LIB hGraphicLib)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hWindow, hTimer, hLayout, hLabel, hPicture, hLayout2, hButtonVal, hButtonCan;
	bool bTouch;

	// Build picture context with 2 buttons (Cancel and Valid)
	// *******************************************************
	bTouch = GL_GraphicLib_IsTouchPresent(hGraphicLib);               // Is a touch screen?

	hWindow = GL_Window_Create(hGraphicLib);                          // Create a window in a user area
	CHECK(hWindow!=NULL, lblEnd);

	hTimer = GL_Timer_Create(hWindow);                                // Create a timer on this window
	CHECK(hTimer!=NULL, lblKO);
	GL_Widget_SetId(hTimer, TIMER_ID);                                // Timer Identifier TIMER_ID
	GL_Widget_RegisterCallback(hTimer, GL_EVENT_TIMER_OUT, CallBackTimeout); // Call back when the timer expired

	hLayout = GL_Layout_Create(hWindow);                              // Create a layout on this window
	CHECK(hLayout!=NULL, lblKO);

	hLabel = GL_Label_Create(hLayout);                                // Create a label on this layout
	CHECK(hLabel!=NULL, lblKO);
	GL_Widget_SetItem(hLabel, 0, 0);                                  // Label location on this layout
	GL_Widget_SetText(hLabel, NULL);                                  // Clear text on this label
	GL_Widget_SetShrink(hLabel, GL_DIRECTION_WIDTH);                  // Shrink beyond its size (No width scrolling cell if label goes over screen)
	GL_Widget_SetId(hLabel, LABEL_0_ID);                              // Label identifier LABEL0_ID

	hPicture = GL_Picture_Create(hLayout);                            // Create a picture on this layout
	CHECK(hPicture!=NULL, lblKO);
	GL_Widget_SetId(hPicture, PICTURE_ID);                            // Picture Identifier PICTURE_ID
	GL_Widget_SetItem(hPicture, 0, 1);                                // Picture location on this layout

	hLayout2 = GL_Layout_Create(hLayout);                             // Create a second layout on this layout
	CHECK(hLayout2!=NULL, lblKO);
	GL_Widget_SetId(hLayout2, LAYOUT2_ID);                            // Second layout identifier
	GL_Widget_SetItem(hLayout2, 0, 2);                                // Second layout location on this layout
	GL_Widget_SetGrow(hLayout2, GL_DIRECTION_WIDTH);                  // Grow beyond its size (Background width around layout will fill the complete cell)

	hButtonCan = GL_Button_Create(hLayout2);                          // Create a cancel button on this second layout
	CHECK(hButtonCan!=NULL, lblKO);
	GL_Widget_SetSource(hButtonCan, "skin://images/cancel.wgu");      // Set the cancel skinned image
	GL_Widget_SetItem(hButtonCan, 0, 0);                              // Button location on this layout
	GL_Widget_SetExpand(hButtonCan, GL_DIRECTION_WIDTH);              // Expand button width
	GL_Widget_SetShortcut(hButtonCan, GL_KEY_CANCEL);                 // Shortcut on this button
	GL_Widget_SetId(hButtonCan, BUTTON_CAN_ID);                       // Cancel button Identifier
	GL_Widget_SetGrow(hButtonCan, GL_DIRECTION_WIDTH);                // Grow beyond its size (Background width around cancel button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonCan, false);                        // No focus on this button
	GL_Widget_RegisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Call back when key pressed

	hButtonVal = GL_Button_Create(hLayout2);                          // Create a valid button on this second layout
	CHECK(hButtonVal!=NULL, lblKO);
	GL_Widget_SetSource(hButtonVal, "skin://images/validation.wgu");  // Set the valid skinned image
	GL_Widget_SetItem(hButtonVal, 1, 0);                              // Button location on this layout
	GL_Widget_SetExpand(hButtonVal, GL_DIRECTION_WIDTH);              // Expand button width
	GL_Widget_SetShortcut(hButtonVal, GL_KEY_VALID);                  // Shortcut on this button
	GL_Widget_SetId(hButtonVal, BUTTON_VAL_ID);                       // Valid button Identifier
	GL_Widget_SetGrow(hButtonVal, GL_DIRECTION_WIDTH);                // Grow beyond its size (Background width around valid button will fill the complete cell)
	GL_Widget_SetFocusable(hButtonVal, false);                        // No focus on this button
	GL_Widget_RegisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Call back when key pressed

	if(!bTouch)                                                       // Touch screen?
		GL_Widget_SetSize(hLayout2, 100, 0, GL_UNIT_PERCENT);         // No, do not show second layout (keep shortcut access on key pressed)

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                // Window creation failed
	if (hWindow)
	{
		GL_Widget_Destroy(hWindow);                                   // Destroy window and all widgets
		hWindow=NULL;
	}
	goto lblEnd;
lblEnd:
	return hWindow;
}

//****************************************************************************
//        int GoalClrPicture (T_GL_HWIDGET hWindow, T_GL_COLOR ulBackColor,
//                            unsigned int uiKeys, bool bRefresh)
// This function clears the picture context, can change the background color
//  and sets the shortcut keys needed (Cancel and Valid).
//  Background color is forced to white for B&W terminal.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   ulBackColor (I-) : Drawing background color.
//   uiKeys (I-) : Shortcut keys (exp: KEY_CANCEL | KEY_VALID)
//                   Cancel Key (KEY_CANCEL)
//                   Valid Key (KEY_VALID)
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Clear picture done.
//   <0 : Clear picture failed.
//****************************************************************************

int GoalClrPicture(T_GL_HWIDGET hWindow, T_GL_COLOR ulBackColor, unsigned int uiKeys, bool bRefresh)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hButtonVal, hButtonCan;
	int iRet;

	// Disable the complete second layout
	// **********************************
	hButtonCan = GL_Widget_SearchId(hWindow, BUTTON_CAN_ID);     // Retrieve the cancel button on this second layout
	CHECK(hButtonCan!=NULL, lblKO);
	hButtonVal = GL_Widget_SearchId(hWindow, BUTTON_VAL_ID);     // Retrieve the valid button on this second layout
	CHECK(hButtonVal!=NULL, lblKO);

	GL_Widget_SetVisible(hButtonCan, false);                     // Blind the cancel button
	GL_Widget_UnregisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Unregister call back on cancel button
	GL_Widget_SetVisible(hButtonVal, false);                     // Blind the valid button
	GL_Widget_UnregisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Unregister call back on valid button

	// Set the second layout with the right properties
	// ***********************************************
	if ((uiKeys & KEY_CANCEL) == KEY_CANCEL)                     // Cancel key?
	{
		GL_Widget_SetVisible(hButtonCan, true);                  // Yes, show the cancel key on this second layout
		GL_Widget_RegisterCallback(hButtonCan, GL_EVENT_STYLUS_CLICK, CallBackCancelKey); // Register call back when cancel key pressed or touched
	}

	if ((uiKeys & KEY_VALID) == KEY_VALID)                       // Valid key?
	{
		GL_Widget_SetVisible(hButtonVal, true);                  // Yes, show the valid key on this second layout
		GL_Widget_RegisterCallback(hButtonVal, GL_EVENT_STYLUS_CLICK, CallBackValidKey); // Register call back when valid key pressed or touched
	}

	if (IsColorDisplay())                                        // Color display?
		GL_Widget_SetBackColor(hWindow, ulBackColor);            // Yes, set window back color
	else
		GL_Widget_SetBackColor(hWindow, GL_COLOR_WHITE);         // No, force window back color to white

	// Clear Title
	// ***********
	GL_Widget_SetText(GL_Widget_SearchId(hWindow, LABEL_0_ID), NULL); // Remove label text

	// Clear picture
	// *************
	GL_Widget_SetSource(GL_Widget_SearchId(hWindow, PICTURE_ID), ""); // Clear picture
	GL_Widget_SetBorders(GL_Widget_SearchId(hWindow, PICTURE_ID), 0, 0, 0, 0, GL_UNIT_PIXEL, ulBackColor); // Remove border with window back color

	if (bRefresh)                                                // Refresh window?
		GL_Window_Dispatch(hWindow, 0);                          // Yes, refresh it

	iRet=1;
	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                           // Clear picture failed
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//    int GoalDspPicture (T_GL_HWIDGET hWindow, char *pcTitle,
//                        const char* pcPicture, const ST_IMAGE* pxImage,
//                        ulong ulTimeOut, bool bRefresh)
// This function displays a picture on screen for B&W terminal or color terminal.
//  The picture has the following properties:
//   - Pen Color,Font for title
//   - Expand, Border, Margin for picture
//  A Time-out to exit from the screen.
//  Refresh or not the screen.
// This function has parameters.
//   hWindow (I-) : Window context handle.
//   pcTitle (I-) : To show a title on top of the picture.
//   pcPicture (I-) : The picture to display (.BMP, .GIF etc...)
//   pxImage (I-) : Picture properties.
//   ulTimeOut (I-) : Timeout in milliseconds.
//   bRefresh (I-) : Refresh or not the screen.
// This function has return value.
//   >=0 : Display picture done (Exit screen : 0=timeout,GL_KEY_CANCEL,GL_KEY_VALID).
//   <0 : Display picture failed.
//****************************************************************************

int GoalDspPicture(T_GL_HWIDGET hWindow, char *pcTitle, const char* pcPicture, const ST_IMAGE* pxImage, ulong ulTimeOut, bool bRefresh)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hLabel, hPicture;
	int iRet;

	// Display label
	// *************
	hLabel = GL_Widget_SearchId(hWindow, LABEL_0_ID);   // Get label from its id
	CHECK(hLabel!=NULL, lblKO);

	if (pcTitle)                                        // Null title?
	{
		GL_Widget_SetVisible(hLabel, true);
		GL_Widget_SetText(hLabel, "%s", pcTitle);       // No, show title (add %s to allow '%' to be displayed)
	}
	else
		GL_Widget_SetVisible(hLabel, false);            // Yes, blind title (line is removed)


	if (IsColorDisplay())                               // Color display?
	{                                                   // Yes
		// Color properties
		// ================
		if (pxImage)
		{
			GL_Widget_SetForeColor(hLabel, pxImage->xColor.ulPen);           // Pen color
			if (pxImage->xColor.bGrowWidth)                                  // Grow beyond its size
				GL_Widget_SetGrow(hLabel, GL_DIRECTION_WIDTH);               // Background width around the label will fill the complete cell
			if (pxImage->xColor.xFont.pcName != NULL)
				GL_Widget_SetFontName(hLabel, pxImage->xColor.xFont.pcName); // Font name
			GL_Widget_SetFontScale(hLabel, pxImage->xColor.xFont.eScale);    // Font scale
			GL_Widget_SetFontStyle(hLabel, pxImage->xColor.xFont.eStyle);    // Font style
		}
	}
	else                                                // No
	{
		// Black & White properties
		// ========================
		if (pxImage)
		{
			if (pxImage->xBW.bGrowWidth)                                     // Grow beyond its size
				GL_Widget_SetGrow(hLabel, GL_DIRECTION_WIDTH);               // Background width around the label will fill the complete cell
			if (pxImage->xBW.xFont.pcName != NULL)
				GL_Widget_SetFontName(hLabel, pxImage->xBW.xFont.pcName);    // Font name
			GL_Widget_SetFontScale(hLabel, pxImage->xBW.xFont.eScale);       // Font scale
			GL_Widget_SetFontStyle(hLabel, pxImage->xBW.xFont.eStyle);       // Font style
		}
	}

	// Display picture
	// ***************
	hPicture = GL_Widget_SearchId(hWindow, PICTURE_ID); // Get picture from its id
	GL_Widget_SetSource(hPicture, pcPicture);
	GL_Widget_SetTransformation(hPicture, GL_TRANSFORMATION_STRETCH_ALL); // Stretched along all directions
	GL_Widget_SetShrink(hPicture, GL_DIRECTION_ALL);    // Shrink beyond its size (Picture shrink automatically if picture goes over screen)

	if (IsColorDisplay())                               // Color display?
	{                                                   // Yes, set expand, border and margin
		GL_Widget_SetExpand(hPicture, pxImage->xColor.eExpand);
		GL_Widget_SetBorders(hPicture, pxImage->xColor.xBorder.usLeft, pxImage->xColor.xBorder.usTop, pxImage->xColor.xBorder.usRight, pxImage->xColor.xBorder.usBottom, GL_UNIT_PIXEL, pxImage->xColor.xBorder.ulColor);
		GL_Widget_SetMargins(hPicture, pxImage->xColor.xMargin.usLeft, pxImage->xColor.xMargin.usTop, pxImage->xColor.xMargin.usRight, pxImage->xColor.xMargin.usBottom, GL_UNIT_PIXEL);
	}
	else
	{                                                   // No, set expand, border and margin
		GL_Widget_SetExpand(hPicture, pxImage->xBW.eExpand);
		GL_Widget_SetBorders(hPicture, pxImage->xBW.xBorder.usLeft, pxImage->xBW.xBorder.usTop, pxImage->xBW.xBorder.usRight, pxImage->xBW.xBorder.usBottom, GL_UNIT_PIXEL, pxImage->xBW.xBorder.ulColor);
		GL_Widget_SetMargins(hPicture, pxImage->xBW.xMargin.usLeft, pxImage->xBW.xMargin.usTop, pxImage->xBW.xMargin.usRight, pxImage->xBW.xMargin.usBottom, GL_UNIT_PIXEL);
	}

	CHECK(ulTimeOut!=0, lblNoTimer);                    // No timer to set

	// Restart timer
	// *************
	if (ulTimeOut == GL_TIME_INFINITE)                  // Set timeout value
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), 0x7FFFFFFF); // Bug 0xFFFFFFFF not supported
	else
		GL_Timer_SetInterval(GL_Widget_SearchId(hWindow, TIMER_ID), ulTimeOut);

	// Loop indefinitely until the result is set from the call back functions
	// **********************************************************************
	GL_Window_MainLoop(hWindow);                        // Wait until a result is set

	// Test the result set by the call back functions
	// **********************************************
	switch(GL_Window_GetResult(hWindow))                // Get the result
	{
	case RES_VALID_STATUS:                              // Key valid result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_VALID;
		break;
	case RES_CANCEL_STATUS:                             // Key cancel result
		GL_Timer_Stop(GL_Widget_SearchId(hWindow, TIMER_ID));
		iRet=GL_KEY_CANCEL;
		break;
	case RES_TIMER_STATUS:                              // Timer result
		iRet=0;
		break;
	default:                                            // Unknown result
		iRet=-1;
		break;
	}

	goto lblEnd;

	// Errors treatment
    // ****************
lblNoTimer:                                             // No timer used
	iRet=0;
	if (bRefresh)
	{
		iRet=GL_Window_Dispatch(hWindow, 0);            // Refresh the window
		if (iRet == RES_VALID_STATUS)
			iRet=GL_KEY_VALID;
		if (iRet == RES_CANCEL_STATUS)
			iRet=GL_KEY_CANCEL;
	}
	goto lblEnd;
lblKO:                                                  // Display picture failed
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//          void GoalDestroyPicture (T_GL_HWIDGET *phWindow)
// This function destroys the picture context by releasing all allocated memory,
//  and all widgets attached.
//  This call is crucial, if it is forgotten you get a memory leak.
// This function has parameters.
//   phWindow (I-) : Pointer on window context handle.
// This function has no return value.
//****************************************************************************

void GoalDestroyPicture(T_GL_HWIDGET *phWindow)
{
	// Local variables
    // ***************
		// Empty

	// Destroy screen
	// **************
	GL_Widget_Destroy(*phWindow); // Destroy window with all widgets attached
	*phWindow=NULL;
}

//##############################################################################
//                                   PRINTING
//##############################################################################

//****************************************************************************
//      T_GL_HWIDGET GoalCreateDocument (T_GL_HGRAPHIC_LIB hGraphicLib,
//                                       T_GL_ENCODING_CHARSET eCHarSet)
// This function creates a document context which is composed of:
//  - A document to print.
//  - Encoding charsets:
//      GL_ENCODING_UTF8 => Unicode utf8
//      GL_ENCODING_ISO_8859_X => ISO 8859
// This function has parameters.
//   hGraphicalLib (I-) : Graphic library context handle.
//   eCharSet (I-) : Encoding charsets.
// This function has return value.
//   !NULL : Pointer to the handle of the document.
//   NULL : Document failed
//****************************************************************************

T_GL_HWIDGET GoalCreateDocument (T_GL_HGRAPHIC_LIB hGraphicLib, T_GL_ENCODING_CHARSET eCharSet)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hDocument, hLayout;

	// Build document context
	// **********************
	GL_GraphicLib_SetCharset(hGraphicLib, eCharSet);  // Changes the encoding charset of the graphic library

	hDocument = GL_Document_Create(hGraphicLib);      // Create a document to print
	CHECK(hDocument!=NULL, lblEnd);

	hLayout = GL_Layout_Create(hDocument);            // Create a layout on this document
	CHECK(hLayout!=NULL, lblKO);
	GL_Widget_SetId(hLayout, LAYOUT3_ID);             // Layout Identifier LAYOUT3_ID

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                // Document creation failed
	if (hDocument)
	{
		GL_Widget_Destroy(hDocument);                 // Destroy document and all widgets
		hDocument=NULL;
	}
	goto lblEnd;
lblEnd:
	return hDocument;
}

//****************************************************************************
//    int GoalPrnLine(T_GL_HWIDGET hDocument, ushort usLine, char *pcText,
//                    const ST_PRN_LINE* pxLine)
// This function build a string on document for printing.
//  The line has the following properties:
//   - Text Align, Background Align, Reverse mode,
//   - Background size (width),
//   - Text padding, Border, Margin, Font
// This function has parameters.
//   hDocument (I-) : Document context handle.
//   usLine (I-) : Line number.
//   pcText (I-) : Text to print.
//   pxLine (I-) : Line properties.
// This function has return value.
//   >=0 : Build line done.
//   <0 : Build line failed.
//****************************************************************************

int GoalPrnLine(T_GL_HWIDGET hDocument, ushort usLine, char *pcText, const ST_PRN_LINE* pxLine)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hLayout, hPrint;
	int iRet=0;

	// Build line on document
	// **********************
	CHECK((pcText!=NULL) && (pxLine!=NULL), lblKO);
	CHECK(strlen(pcText)!=0, lblEnd);

	hLayout = GL_Widget_SearchId(hDocument, LAYOUT3_ID);      // Get layout from its id
	CHECK(hLayout!=NULL, lblKO);

	hPrint = GL_Print_Create(hLayout);                        // Create a print label designed to be printed on this layout
	CHECK(hPrint!=NULL, lblKO);
	GL_Widget_SetItem(hPrint, 0, usLine);                     // Print label location on this layout
	GL_Widget_SetText(hPrint, "%s", pcText);                  // Print text (Add %s to allow '%' to be printed)
	GL_Widget_SetWrap(hPrint, true);                          // Set the wrap of text (Allows text to automatically return to the next line if it does not fit in width)
	GL_Widget_SetShrink(hPrint, GL_DIRECTION_WIDTH);          // Shrink beyond its size (No width scrolling cell if text goes over printer size)

	GL_Widget_SetTextAlign(hPrint, pxLine->eTextAlign);       // Text alignment
	GL_Widget_SetBackAlign(hPrint, pxLine->eBackAlign);       // Background alignment
	if (pxLine->usSizeWidth < 99)                             // Background size
		GL_Widget_SetMinSize(hPrint, pxLine->usSizeWidth, 0, GL_UNIT_PERCENT); // The size
	else
		GL_Widget_SetExpand(hPrint, GL_DIRECTION_WIDTH);                       // Full size
	if (!pxLine->bReverse)                                    // Reverse mode?
	{                                                         // No
		GL_Widget_SetForeColor(hPrint, GL_COLOR_BLACK);       // Force pen color to black and brush color to white
		GL_Widget_SetBackColor(hPrint, GL_COLOR_WHITE);
	}
	else
	{                                                         // Yes
		GL_Widget_SetForeColor(hPrint, GL_COLOR_WHITE);       // Force pen color to white and brush color to black
		GL_Widget_SetBackColor(hPrint, GL_COLOR_BLACK);
	}
	                                                          // Padding, Border and Margin
	GL_Widget_SetTextPaddings(hPrint, pxLine->xPadding.usLeft, pxLine->xPadding.usTop, pxLine->xPadding.usRight, pxLine->xPadding.usBottom, GL_UNIT_PERCENT);
	if (pxLine->xBorder.ulColor != GL_COLOR_BLACK)            // Force border to black or white
		GL_Widget_SetBorders(hPrint, pxLine->xBorder.usLeft, pxLine->xBorder.usTop, pxLine->xBorder.usRight, pxLine->xBorder.usBottom, GL_UNIT_PIXEL, GL_COLOR_WHITE);
	else
		GL_Widget_SetBorders(hPrint, pxLine->xBorder.usLeft, pxLine->xBorder.usTop, pxLine->xBorder.usRight, pxLine->xBorder.usBottom, GL_UNIT_PIXEL, GL_COLOR_BLACK);
	GL_Widget_SetMargins(hPrint, pxLine->xMargin.usLeft, pxLine->xMargin.usTop, pxLine->xMargin.usRight, pxLine->xMargin.usBottom, GL_UNIT_PIXEL);
	if (pxLine->xFont.pcName != NULL)
		GL_Widget_SetFontName(hPrint, pxLine->xFont.pcName);  // Font name
	GL_Widget_SetFontScale(hPrint, pxLine->xFont.eScale);     // Font scale
	GL_Widget_SetFontStyle(hPrint, pxLine->xFont.eStyle);     // Font style

	iRet=1;
	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                        // Build line on document failed
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//   int GoalPrnMultiLine(T_GL_HWIDGET hDocument, ushort usLine, ushort usSize,
//                        char *pcData, ushort usLen, const ST_PRN_LINE* pxLine)
//  This function sends a data buffer to the document.
//   The buffer is divided on lines of size bytes max except the last line.
//   Each line is automatically sent to the document. The '\n' automatically
//   switches to the next line.
//  This function has parameters:
//    hDocument (I-) : Window context handle.
//    usLine (I-) : Line number.
//    usSize (I-) : Line size max.
//    pcData (I-) : Data to send to the document.
//    usLen (I-) : Length of data.
//    pxLine (I-) : Line properties.
//  This function has return value.
//    >=0 : Build buffer done.
//    <0 : Build buffer failed.
//****************************************************************************

int GoalPrnMultiLine(T_GL_HWIDGET hDocument, ushort usLine, ushort usSize, char *pcData , ushort usLen, const ST_PRN_LINE* pxLine)
{
	// Local variables
    // ***************
	ushort usIdx=0;
	ushort usCount=0;
	char tcBuffer[1024+1];
	int iRet=-1;

	// Build document in progress
	// **************************
	CHECK ((pcData!=NULL) && (usLen>=0), lblKO)

	memset(tcBuffer, 0, sizeof(tcBuffer));
	while(1)
	{
		if (usLen > 0)                 // Check if it is the end of data
		{                              // No, it's a line with '\n' detected
			if (usCount < usSize)      // Save character into buffer
			{
				if (*pcData == '\n')
					usCount=0;
				else
					usCount++;
				tcBuffer[usIdx++] = *pcData++;
				usLen--;
			}
			else                       // Bloc detected
			{
				tcBuffer[usIdx++] = '\n';
				usCount=0;
			}
		}
		else
		{                              // Yes, it's the end of data
			if (usLen != 0)
				tcBuffer[usIdx++] = '\n';
			break;                     // Exit from the loop
		}
	}
                                       // Send line to the document
	iRet = GoalPrnLine(hDocument, usLine, tcBuffer, pxLine);

	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                 // Build buffer failed.
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//    int GoalPrnPicture (T_GL_HWIDGET hDocument, ushort usLine,
//                        char *pcPicture, const ST_BORDER* pxBorder)
// This function build a picture on document for printing.
//  The picture has the following properties:
//   - Picture shrinks beyond its size
//   - Border around the picture
// This function has parameters.
//   hDocument (I-) : Document context handle.
//   usLine (I-) : Line number.
//   pcPicture (I-) : Picture to print.
//   pxBorder (I-) : Border properties.
// This function has return value.
//   >=0 : Build picture done.
//   <0 : Build picture failed.
//****************************************************************************

int GoalPrnPicture(T_GL_HWIDGET hDocument, ushort usLine, const char* pcPicture, const ST_BORDER* pxBorder)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hLayout, hPrint;
	int iRet;

	// Build picture on document
	// *************************
	hLayout = GL_Widget_SearchId(hDocument, LAYOUT3_ID);    // Get layout from its id
	CHECK(hLayout!=NULL, lblKO);

	hPrint = GL_Picture_Create(hLayout);                    // Create a picture designed to be printed on this layout
	CHECK(hPrint!=NULL, lblKO);
	GL_Widget_SetItem(hPrint, 0, usLine);                   // Picture location on this layout
	GL_Widget_SetSource(hPrint, pcPicture);                 // The picture to print

	GL_Widget_SetShrink(hPrint, GL_DIRECTION_WIDTH);        // Shrink beyond its size (Picture shrinks automatically if picture goes over printer size)
	                                                        // Border around the picture
	GL_Widget_SetBorders(hPrint, pxBorder->usLeft, pxBorder->usTop, pxBorder->usRight, pxBorder->usBottom, GL_UNIT_PIXEL, pxBorder->ulColor);

	iRet=1;
	goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                      // Build picture failed.
	iRet=-1;
	goto lblEnd;
lblEnd:
	return iRet;
}

//****************************************************************************
//          int GoalPrnDocument (T_GL_HWIDGET hDocument)
// This function prints the document built.
// This function has parameters.
//   hDocument (I-) : Document context handle.
// This function has return value.
//   >=0 : Print document done.
//   <0 : Print document failed.
//****************************************************************************

int GoalPrnDocument(T_GL_HWIDGET hDocument)
{
	// Local variables
    // ***************
	T_GL_ERROR_CODE eRet;
	int iRet=-1;

	// Printing in progress
	// ********************
	eRet = GL_Document_Print(hDocument, 0);
	if ((eRet == GL_SUCCESS) || !IsPrinter())
		iRet=1;

	return iRet;
}

//****************************************************************************
//          void GoalDestroyDocument (T_GL_HWIDGET *phDocument)
// This function destroys the document context by releasing all allocated memory,
//  and all widgets attached.
//  This call is crucial, if it is forgotten you get a memory leak.
// This function has parameters.
//   phDocument (I-) : Pointer on document context handle.
// This function has no return value.
//****************************************************************************

void GoalDestroyDocument(T_GL_HWIDGET *phDocument)
{
	// Local variables
    // ***************
		// Empty

	// Destroy document
	// ****************
	GL_Widget_Destroy(*phDocument); // Destroy document with all widgets attached
	*phDocument=NULL;
}








