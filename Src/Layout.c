//****************************************************************************
//       INGENICO                                INGEDEV 7
//============================================================================
//       FILE  LAYOUT.C                           (Copyright INGENICO 2013)
//============================================================================
//  Created :       12-December-2013     Kassovic
//  Last modified : 12-December-2013     Kassovic
//  Module : TRAINING
//
//  Purpose :
//                       *** Layout management ***
//  This file DOES NOT use the following API(s) for displaying:
//    # System function level (printf, ...)
//      - Native method (obsolete)
//      - Does not support font
//      - No access to color function, only monochrome mode
//      - Does not support touch screen
//    # Graphic library level (DrawString, ...)
//      - Graphic functionalities
//      - Support fonts
//      - No access to color functions, only colorized mode
//      - Does not support touch screen
//    # Color library level (DrawText, ...)
//      - Full color functionalities
//      - Support fonts
//      - Only for color terminals
//      - Does not support touch screen
//  This file uses the Graphic Object Advanced Library (GOAL) for displaying:
//    # Manager goal must be loaded
//    # For all type of terminals (B&W, Color, touch) and display ration supported
//    # 2 modes available
//      - Predefined mode (not used inside this file)
//        Message dialog box, Input dialog box, List dialog box, File dialog box etc...
//      - Customized mode
//        . By API(s) (used inside this file)
//          Build your screen using API(s) widgets
//        . Resource file (not used inside this file)
//          Build your screen using WYSIWYG editor
//
//  List of routines in file :
//      Layout : To show the creation of a layout which supports 9 label widgets.
//
//  File history :
//  121213-BK : File created
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

//****************************************************************************
//      PRIVATE CONSTANTS
//****************************************************************************
#define ID_LABEL_1_1  0
#define ID_TIMER      1

//****************************************************************************
//      PRIVATE TYPES
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA
//****************************************************************************
static bool bMargins=true, bPaddings=true, bBorders=true;
static bool bNewLabel=true;

// Sets properties by default
// ==========================
void defaultProperties(T_GL_HWIDGET hLabel)
{
	GL_Widget_SetGrow(hLabel, GL_DIRECTION_ALL);
	GL_Widget_SetBackAlign(hLabel, GL_ALIGN_CENTER);
	GL_Widget_SetForeAlign(hLabel, GL_ALIGN_CENTER);
	GL_Widget_SetMargins(hLabel, 0, 0, 0, 0, GL_UNIT_PERCENT);
	bMargins=true;
	GL_Widget_SetTextPaddings(hLabel, 0, 0, 0, 0, GL_UNIT_PERCENT);
	bPaddings=true;
	GL_Widget_SetBorders(hLabel, 0, 0, 0, 0, GL_UNIT_PERCENT, GL_COLOR_WHITE);
	bBorders=true;
	GL_Widget_SetShrink(hLabel, GL_DIRECTION_NONE);
	GL_Widget_SetWrap(hLabel, false); // Bug!, does not work
	GL_Widget_SetVisible(hLabel, true);
}

// Call back function on key pressed
// =================================
static bool OnKeyPress(T_GL_HMESSAGE hMessage)
{
	T_GL_WCHAR ulKey; T_GL_HWIDGET hLabel;
	T_GL_HWIDGET hTimer;
//	T_GL_HWIDGET hWindow;
	char tcBuffer[100+1] = "Hello, How are you ?";

	ulKey = GL_Message_GetKey(hMessage);                        // Retrieves key pressed
	hLabel = GL_Message_GetWidgetById(hMessage, ID_LABEL_1_1);  // Retrieves label widget(1,1) using the Id
	hTimer = GL_Message_GetWidgetById(hMessage, ID_TIMER);      // Retrieves timer widget using the Id
	GL_Timer_SetInterval(hTimer, 30*1000);                      // Restarts timer

//	memset(tcBuffer, 0, sizeof(tcBuffer));
//	hWindow = GL_Message_GetWindow(hMessage);
//	GL_Widget_GetUserData(hWindow, tcBuffer, sizeof(tcBuffer)); // Retrieves the data from window widget


	if (ulKey == GL_KEY_CANCEL)
	{
		bMargins=true, bPaddings=true, bBorders=true;
		bNewLabel=true;
		GL_Message_SetResult(hMessage, GL_KEY_CANCEL);          // Exits from GL_Window_MainLoop on key cancel
	}

	// BackColor property
	// Press key1 until return to green
	if (ulKey == GL_KEY_1)
	{
		if (GL_Widget_GetBackColor(hLabel) == GL_COLOR_GREEN)
			GL_Widget_SetBackColor(hLabel, GL_COLOR_ORANGE);
		else
			GL_Widget_SetBackColor(hLabel, GL_COLOR_GREEN);
	}

	// BackAlign properties
	// Press key2 until return to center
	if (ulKey == GL_KEY_2)
	{
		switch (GL_Widget_GetBackAlign(hLabel))
		{
		case GL_ALIGN_CENTER: GL_Widget_SetBackAlign(hLabel, GL_ALIGN_TOP);    break;
		case GL_ALIGN_TOP:    GL_Widget_SetBackAlign(hLabel, GL_ALIGN_BOTTOM); break;
		case GL_ALIGN_BOTTOM: GL_Widget_SetBackAlign(hLabel, GL_ALIGN_LEFT);   break;
		case GL_ALIGN_LEFT:   GL_Widget_SetBackAlign(hLabel, GL_ALIGN_RIGHT);  break;
		default:              GL_Widget_SetBackAlign(hLabel, GL_ALIGN_CENTER); break;
		}
	}

	// Grow properties
	// Press key3 once (grow width), press key2 until return to center
	// Press key3 again (grow height), press key2 until return to center
	// Press key3 again (grow none), press key2 stay in center forever
	// Press key3 again (grow all), press key2 until return to center
	if (ulKey == GL_KEY_3)
	{
		// Property by default
		GL_Widget_SetBackAlign(hLabel, GL_ALIGN_CENTER);

		// Grow properties
		switch (GL_Widget_GetGrow(hLabel))
		{
		case GL_DIRECTION_ALL:    GL_Widget_SetGrow(hLabel, GL_DIRECTION_WIDTH);  break;
		case GL_DIRECTION_WIDTH:  GL_Widget_SetGrow(hLabel, GL_DIRECTION_HEIGHT); break;
		case GL_DIRECTION_HEIGHT: GL_Widget_SetGrow(hLabel, GL_DIRECTION_NONE);   break;
		default:                  GL_Widget_SetGrow(hLabel, GL_DIRECTION_ALL);    break;
		}
	}

	// Set text property
	// Press key4 to set new text
	if (ulKey == GL_KEY_4)
	{
		// Properties by default
		defaultProperties(hLabel);
		GL_Widget_SetExpand(hLabel, GL_DIRECTION_NONE);

		// Text property
		if (bNewLabel)
		{
			GL_Widget_SetText(hLabel, tcBuffer);
			bNewLabel=false;
		}
		else
		{
			GL_Widget_SetText(hLabel, "1:1");
			bNewLabel=true;
		}
	}

	// Shrink properties
	// Press key5 to shrink the cell
	if (ulKey == GL_KEY_5)
	{
		if (GL_Widget_GetShrink(hLabel) == GL_DIRECTION_NONE)
			GL_Widget_SetShrink(hLabel, GL_DIRECTION_WIDTH);
		else
			GL_Widget_SetShrink(hLabel, GL_DIRECTION_NONE);
	}

	// Wrap property
	// Press key6 to wrap the text
	if (ulKey == GL_KEY_6)
	{
		if (GL_Widget_GetWrap(hLabel) == false)
			GL_Widget_SetWrap(hLabel, true);  // Bug!, does not work
		else
			GL_Widget_SetWrap(hLabel, false); // Bug!, does not work
	}

	// Expand properties
	// Press key7 until to return to expand none
	if (ulKey == GL_KEY_7)
	{
		// Properties by default
		defaultProperties(hLabel);
		GL_Widget_SetText(hLabel, "1:1");
		bNewLabel=true;

		// Expand properties
		switch (GL_Widget_GetExpand(hLabel))
		{
		case GL_DIRECTION_NONE:   GL_Widget_SetExpand(hLabel, GL_DIRECTION_WIDTH);  break;
		case GL_DIRECTION_WIDTH:  GL_Widget_SetExpand(hLabel, GL_DIRECTION_HEIGHT); break;
		case GL_DIRECTION_HEIGHT: GL_Widget_SetExpand(hLabel, GL_DIRECTION_ALL);    break;
		default:                  GL_Widget_SetExpand(hLabel, GL_DIRECTION_NONE);   break;
		}
	}

	// ForeAlign properties
	// Press key7 until expand all
	// Press key8 until left
	if (ulKey == GL_KEY_8)
	{
		switch (GL_Widget_GetForeAlign(hLabel))
		{
		case GL_ALIGN_CENTER: GL_Widget_SetForeAlign(hLabel, GL_ALIGN_TOP);    break;
		case GL_ALIGN_TOP:    GL_Widget_SetForeAlign(hLabel, GL_ALIGN_BOTTOM); break;
		case GL_ALIGN_BOTTOM: GL_Widget_SetForeAlign(hLabel, GL_ALIGN_LEFT);   break;
		case GL_ALIGN_LEFT:   GL_Widget_SetForeAlign(hLabel, GL_ALIGN_RIGHT);  break;
		default:              GL_Widget_SetForeAlign(hLabel, GL_ALIGN_CENTER); break;
		}
	}

	// Margins properties
	// Press key9 to get margins
	// Stay in expand all
	if (ulKey == GL_KEY_9)
	{
		if (bMargins)
		{
			GL_Widget_SetMargins(hLabel, 5, 5, 5, 5, GL_UNIT_PERCENT);
			bMargins=false;
		}
		else
		{
			GL_Widget_SetMargins(hLabel, 0, 0, 0, 0, GL_UNIT_PERCENT);
			bMargins=true;
		}
	}

	// Paddings properties
	// Press key0 to get paddings
	if (ulKey == GL_KEY_0)
	{
		if (bPaddings)
		{
			GL_Widget_SetTextPaddings(hLabel, 5, 5, 5, 5, GL_UNIT_PERCENT);
			bPaddings=false;
		}
		else
		{
			GL_Widget_SetTextPaddings(hLabel, 0, 0, 0, 0, GL_UNIT_PERCENT);
			bPaddings=true;
		}
	}

	// Borders properties
	// Press key dot to get borders
	// Press cancel to exit
	if (ulKey == GL_KEY_DOT)
	{
		if (bBorders)
		{
			GL_Widget_SetBorders(hLabel, 1, 1, 1, 1, GL_UNIT_PERCENT, GL_COLOR_WHITE);
			bBorders=false;
		}
		else
		{
			GL_Widget_SetBorders(hLabel, 0, 0, 0, 0, GL_UNIT_PERCENT, GL_COLOR_WHITE);
			bBorders=true;
		}
	}

	// Visible property
	// Press key yellow to remove label widget
	if (ulKey == GL_KEY_CORRECTION)
	{
		if (GL_Widget_GetVisible(hLabel) == true)
			GL_Widget_SetVisible(hLabel, false);
		else
			GL_Widget_SetVisible(hLabel, true);
	}

	return true;
}

// Call back function on timeout
// =============================
static bool OnTimeout(T_GL_HMESSAGE hMessage)
{
	GL_Message_SetResult(hMessage, GL_RESULT_INACTIVITY);       // Exit from GL_Window_MainLoop on timer expired

	return true;
}

//****************************************************************************
//                          void Layout (void)
//  This function shows the purpose of a layout. The layout contains 9 label
//  widgets. Properties can be added to the widgets: Id, Item, Grow, Shrink,
//  Expand, BackAlign, ForeAlign, TextAlign, Margins, Borders, Paddings, etc...
//  Implementation of call back functions to react on events (key pressed and
//  timeout).
//  This function has no parameters.
//  This function has no return value.
//****************************************************************************

void Layout(void)
{
	// Local variables
    // ***************
	T_GL_HWIDGET hWindow, hLayout, hLabelGrid, hLabel, hTimer;
	T_GL_DIM row, column;
	int iHeader, iFooter;

	// Remove header and footer
	// ************************
	iHeader = IsHeader();  // Save header state
	iFooter = IsFooter();  // Save footer state
	DisplayLeds(_OFF_);    // Disable Leds
	DisplayHeader(_OFF_);  // Disable Header
	DisplayFooter(_OFF_);  // Disable Footer

	// Create a window with 9 label widgets
	// ************************************
	hWindow = GL_Window_Create(hGoal);                                 // *** Creates a window
	hLayout = GL_Layout_Create(hWindow);                               // *** Creates a layout on this window
	for (row=0; row<3; row++)                                          // For all rows of the layout
	{
		for (column=0; column<3; column++)                             // For all columns of the layout
		{
			// Only to show the grid layout in white
			hLabelGrid = GL_Label_Create(hLayout);
			GL_Widget_SetItem(hLabelGrid, column, row);
			GL_Widget_SetExpand(hLabelGrid, GL_DIRECTION_ALL);
			GL_Widget_SetBorders(hLabelGrid, 1, 1, 1, 1, GL_UNIT_PIXEL, GL_COLOR_WHITE);

			hLabel = GL_Label_Create(hLayout);                         // *** Creates a label on this layout
			GL_Widget_SetItem(hLabel, column, row);                    // Sets the label location in the layout
#ifndef __TELIUM3__
			GL_Widget_SetText(hLabel, "%d:%d", column, row);           // Sets the label text
#else
			GL_Widget_SetText(hLabel, "%ld:%ld", column, row);
#endif			
			switch (row)
			{
			case 0: GL_Widget_SetBackColor(hLabel, GL_COLOR_RED);  break;
			case 1: GL_Widget_SetBackColor(hLabel, GL_COLOR_GREEN);
				if (column == 1)
				{
					GL_Widget_SetId(hLabel, ID_LABEL_1_1);             // Sets label Widget(1,1) with Id=0
				}
				break;
			case 2: GL_Widget_SetBackColor(hLabel, GL_COLOR_BLUE); break;
			default: break;
			}
		}
	}

	// Management of widget events on key pressed and timeout
	// ******************************************************
	GL_Widget_RegisterCallback(hWindow, GL_EVENT_KEY_DOWN, OnKeyPress); // Call back when key pressed
//	GL_Widget_SetUserData(hWindow, "Training done by Bruno", 22);         // Attached data string to this window

	hTimer = GL_Timer_Create(hWindow);                                  // *** Creates a timer on this window
	GL_Timer_SetInterval(hTimer, 30*1000);                              // Sets timer to 30s
	GL_Widget_RegisterCallback(hTimer, GL_EVENT_TIMER_OUT, OnTimeout);  // Call back when timer expired
	GL_Widget_SetId(hTimer, ID_TIMER);                                  // Sets timer widget with Id=1

	GL_Window_MainLoop(hWindow);                                        // Displays window and waits until exit

	GL_Widget_Destroy(hWindow);                                         // Destroys the window and all widgets

    // Restore header and footer
	// *************************
	DisplayLeds(_ON_);                       // Enable Leds
	DisplayHeader(iHeader);                  // Enable Header
	if(!GL_GraphicLib_IsTouchPresent(hGoal)) // Check if it is a touch screen
		DisplayFooter(iFooter);              // Enable Footer
}

