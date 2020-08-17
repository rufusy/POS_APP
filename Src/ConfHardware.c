//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  CONFHARDWARE.C                      (Copyright INGENICO 2012)
//============================================================================
//  Created :       17-July-2012     Kassovic
//  Last modified : 17-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//             *** Describe the hardware configuration ***
//          Transmission by serial communication COM0/COM5/COM_EXT
//             Test done with Hyper-terminal (8-N-1-115200)
//                                                                        
//  List of routines in file :  
//      ProductName : Terminal product name.
//      ConfHardware : Hardware configuration of the terminal.                                          
//                            
//  File history :
//  071712-BK : File created
//                                                                           
//****************************************************************************

//****************************************************************************
//      INCLUDES                                                            
//****************************************************************************
#include <globals.h>
#include "ExtraGPRS.h"
#ifdef __TELIUM3__
#include <arpa/inet.h>
#endif

//****************************************************************************
//      EXTERN                                                              
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

//****************************************************************************
//      PRIVATE CONSTANTS                                                   
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
static const char zTrue[] = "YES";
static const char zFalse[] = "NO";
static const char zFast[] = "YES (FAST)";
static const char zSlow[] = "YES (SLOW)";
static const char zV34[] = "YES (V34)";
static const char zColor[] = "COLOR";
static const char z2Lines[] = "2 LINES";
static const char z128x64[] = "128x64";
static const char z128x128[] = "128x128";
static const char zTTL[] = "YES (TTL)";

static char tcBuffer[256+1];
static char tcName[24+1];

//****************************************************************************
//                     char* ProductName (void)
//  This function returns the product terminal name.
//  This function has no parameters.
//  This function has return value.
//    A string buffer regarding the product name.
//****************************************************************************

static char* ProductName(void)
{
	// Local variables
    // ***************
	byte ucProduct;
	byte ucRange;

	// Return the product name
	// ***********************
	ucRange=PSQ_Get_product_type(&ucProduct);
	switch(ucProduct)
	{
	case TYPE_TERMINAL_SMART :      strcpy(tcName, "EFTSMART"); break;  // 2 EFTsmart
	case TYPE_TERMINAL_EFT30F:      strcpy(tcName, "EFT30F");   break;  // 3 EFT30F
	case TYPE_TERMINAL_EFT30P:      strcpy(tcName, "EFT30P");   break;  // 4 EFT30plus
	case TYPE_TERMINAL_TWIN30:                                          // 5
		switch(ucRange)
		{
		case TYPE_TERMINAL_TWIN30:  strcpy(tcName, "TWIN30");   break;  // 55 TWIN30
		case TYPE_TERMINAL_TWIN33:  strcpy(tcName, "TWIN33");   break;  // 56 TWIN33
		case TYPE_TERMINAL_TWIN32:  strcpy(tcName, "TWIN32");   break;  // 57 TWIN32
		default:                    strcpy(tcName, "UNKNOWN");  break;
		}
		break;
	case TYPE_TERMINAL_EFT930:                                          // 6
		switch(ucRange)
		{
		case TYPE_TERMINAL_EFT930G: strcpy(tcName, "EFT930G");  break;  // 67 EFT930G
		case TYPE_TERMINAL_EFT930B: strcpy(tcName, "EFT930B");  break;  // 68 EFT930B
		case TYPE_TERMINAL_EFT930F: strcpy(tcName, "EFT930F");  break;  // 69 EFT930F
		case TYPE_TERMINAL_EFT930W:	strcpy(tcName, "EFT930W");  break;  // 65 EFT930W
		case TYPE_TERMINAL_EFT930P: strcpy(tcName, "EFT930P");  break;  // 66 EFT930P
		default:                    strcpy(tcName, "UNKNOWN");  break;
		}
		break;
	case TYPE_TERMINAL_ML30:                                            // 1
		switch(ucRange)
		{
		case TYPE_TERMINAL_ML30:    strcpy(tcName, "ML30");     break;  // 11 ML30
		case TYPE_TERMINAL_SPM:     strcpy(tcName, "SPM");      break;  // 12 SPM
		case TYPE_TERMINAL_IPP320:  strcpy(tcName, "IPP320");   break;  // 13 IPP320
		case TYPE_TERMINAL_IPP350:  strcpy(tcName, "IPP350");   break;  // 14 IPP350
		case TYPE_TERMINAL_IPP480:  strcpy(tcName, "IPP480");   break;  // 16 IPP480
		default:                    strcpy(tcName, "UNKNOWN");  break;
		}
		break;
	case TYPE_TERMINAL_CAD30:       strcpy(tcName, "CAD30");    break;  // 7 CAD30
	case TYPE_TERMINAL_MR40:        strcpy(tcName, "MR40");     break;  // 9 MR40
	case TYPE_TERMINAL_X07:                                             // 8
		switch(ucRange)
		{
		case TYPE_TERMINAL_IWL220:  strcpy(tcName, "IWL220");   break;  // 83 IWL220
		case TYPE_TERMINAL_IWL250:  strcpy(tcName, "IWL250");   break;  // 82 IWL250
		case TYPE_TERMINAL_ICT220:  strcpy(tcName, "ICT220");   break;  // 88 ICT220
		case TYPE_TERMINAL_ICT250:  strcpy(tcName, "ICT250");   break;  // 87 ICT250
		case TYPE_TERMINAL_ICT280:  strcpy(tcName, "ICT280");   break;  // 86 ICT280
		case TYPE_TERMINAL_ISC350:  strcpy(tcName, "ISC350");   break;  // 85 ISC350
		case TYPE_TERMINAL_ISC250:  strcpy(tcName, "ISC250");   break;  // 84 ISC250
		default:                    strcpy(tcName, "UNKNOWN");  break;
		}
		break;
#ifdef __TELIUM3__
	case TYPE_TERMINAL_DESK:
		switch(ucRange)
		{
		case TYPE_TERMINAL_D5000: strcpy(tcName, "DESK5000"); break;
		case TYPE_TERMINAL_D3500: strcpy(tcName, "DESK3500"); break;
		case TYPE_TERMINAL_D3200: strcpy(tcName, "DESK3200"); break;
		default:                  strcpy(tcName, "UNKNOWN");  break;
		}
		break;
	case TYPE_TERMINAL_MOVE:
		switch(ucRange)
		{
		case TYPE_TERMINAL_M5000: strcpy(tcName, "MOVE5000"); break;
		case TYPE_TERMINAL_M3500: strcpy(tcName, "MOVE3500"); break;
		case TYPE_TERMINAL_M2500: strcpy(tcName, "MOVE2500"); break;
		default:                  strcpy(tcName, "UNKNOWN");  break;
		}
		break;
	case TYPE_TERMINAL_LANE:
		switch(ucRange)
		{
		case TYPE_TERMINAL_L5000: strcpy(tcName, "LANE5000"); break;
		default:                  strcpy(tcName, "UNKNOWN");  break;
		}
		break;
#endif
	default:                        strcpy(tcName, "UNKNOWN");  break;
	}

	return tcName;
}

//****************************************************************************
//                    void ConfHardware(doubleword uiCom)
//  This function sent to a console (hyper-terminal) the configuration regarding
//  the hardware of the terminal.
//   Similar to the configuration ticket from Manager:
//   (TELIUM MANAGER-Consultation-Configuration-Hardware)
//  1) GENERAL INFORMATIONS
//     Product Name, Product Id, Serial Number, Product Code, Constructor Code,
//     Product Ref, Manufacturing, Coupler Hardware and Software.
//  2) ACTIVATION INFORMATIONS
//     First activation, SXX extension, Profile ID, Ingetrust Security,
//     ZKA protection, Fallback, Security VARID and Scheme VARID.
//  3) MEMORY INFORMATIONS
//     Flash Memory and RAM Memory.
//  4) HARDWARE INFORMATIONS
//     -- Miscellaneous --
//     Printer, Display, Buzzer, Modem, Portable, Tilto, MMC, Cless, Hardware LEDS,
//     -- Serial links --
//     COM0, COM1, COM2, COM3, COMN and COMU.
//     -- USB Controller --
//     USB Host and USB Device.
//     -- MORPHO Device --
//     Biometric
//     -- Radio Device --
//     GPRS, CDMA, Bluetooth and Wifi.
//     -- Swipe Readers --
//     ISO1, ISO2, ISO3, CAM0, CAM1 and CAM2.
//     -- CAM & SAM Readers --
//     SAM1, SAM2, SAM3 and SAM4.
//  5) SOFTWARE INFORMATIONS
//     Country Code, Firmware Version, Booster Type, Booster and Thunder versions.
//      - GetConfiguration() : return standard hardware configuration.
//      - Telium_Systemfioctl() : return specific hardware configuration.
//      - IsXXX() : check hardware configuration.
//      - Telium_Fioctl() : call low level OEMC driver.
//      - HWCNF_EthernetGetMacAddress() : return the MAC address of the terminal.
//      - HWCNF_EthernetGetCurrentParameters() : return Ethernet setting.
//      - BoosterType() : return Booster Type (1-2-3).
//  This function has parameters.
//    uiCom (I-) : Communication channel.
//  This function has no return value.
//****************************************************************************

void ConfHardware(doubleword uiCom)
{
	// Local variables
    // ***************
	Telium_File_t *hCom=NULL;
	Telium_size_t uiLen;
	char tcDriver[24+1];
	int iSizeFlash, iSizeRam, iRamFree, iFlashFreeCode, iFlashFreeData, iSizeBlocksKO;
	ip_param_s xEth;
	Telium_File_t *hModem=NULL;
	Telium_File_t *hGprs=NULL;
	T_EGPRS_GET_INFORMATION xInfoGprs;
#ifndef __FRAMEWORK_TELIUM_PLUS__
	MODEM_FIOCTL_IDENT_S xInfoModem;
	SYS_FIOCTL_REPUDIATION_INFO_GET_S xRepudiation;
	char tcProfile[6+1];  // 6 OK!!! No, 7 OK yes => Buffer should be 3 but wait for 7
	char tcFirmware[4+1];
	char tcThunder[4+1], tcBooster[4+1];
#endif
	char *pcFlag, *pcV34;
	byte tucProduct[3];
	char tcManufacturing[8];
	byte tucReference[64];
	byte tucSerial[64];
	char tcActivation[12+1];
	char tcMac[6+1];
	byte tucSmcHard[4+1], tucSmcSoft[4+1];
	char* pcBuffer;
    int iRet;
#ifdef __TELIUM3__
    struct in_addr xAddr;
#endif

    // Com channel initialization
    // **************************
	switch (uiCom)
	{
	case COM0:
		hCom = Telium_Fopen("COM0", "rw*");
		CHECK(hCom!=NULL, lblKO);                                   // Open "com0" peripheral
	    iRet = Telium_Format("COM0", 115200, 8, 1, NO_PARITY, EVEN, 0);    // Format COM0 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM1:
		hCom = Telium_Fopen("COM1", "rw*");
		CHECK(hCom!=NULL, lblKO);                                   // Open "com1" peripheral
	    iRet = Telium_Format("COM1", 115200, 8, 1, NO_PARITY, EVEN, 0);    // Format COM1 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM2:
		hCom = Telium_Fopen("COM2", "rw*");
		CHECK(hCom!=NULL, lblKO);                                   // Open "com2" peripheral
	    iRet = Telium_Format("COM2", 115200, 8, 1, NO_PARITY, EVEN, 0);    // Format COM2 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM5:
		hCom = Telium_Fopen("COM5", "rw*");                         // Open "com5" peripheral
		CHECK(hCom!=NULL, lblKO);
		break;
	case COM_EXT:                                                   // Open "converter USB->RS232" peripheral
		// Telium supports the following drivers:
		// - "COM_KEYSPAN" (Keyspan USA19HS)
		// - "COM_SL" (SiliconLabs CP2102)
		// - "COM20" (Prolific 2303)
		// - "COM21" (FTDI)
		// - "COM_MGBX" (Generic Magic Box)
		// With event generated COM_EXT
		hCom = OpenConverter(tcDriver, sizeof(tcDriver));
		CHECK(hCom!=NULL, lblKO);
	    iRet = Telium_Format(tcDriver, 115200, 8, 1, NO_PARITY, EVEN, 0);  // Format converter channel
	    CHECK(iRet>=0, lblKO);
		break;
	default:
		goto lblKO;
	}
	GL_Dialog_Message(hGoal, NULL, "Configuration Hardware\nTransmitting...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

    // Hardware configuration
    // **********************
	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "###################################################################\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "#                   HARDWARE CONFIGURATION                        #\r\n");
	Telium_Sprintf(pcBuffer,             "###################################################################\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "===================================================================\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "GENERAL INFORMATIONS\r\n");
	Telium_Sprintf(pcBuffer,             "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcBuffer += Telium_Sprintf(pcBuffer, "Product Name       : %s\r\n", ProductName());                 // Product Name
    iRet = TM_GetFullReference(tucReference); CHECK(iRet==0, lblKO);
    pcBuffer += Telium_Sprintf(pcBuffer, "Full Reference     : %s\r\n", tucReference);                  // Product Reference
    PSQ_Give_Full_Serial_Number(tucSerial);
    pcBuffer += Telium_Sprintf(pcBuffer, "Full Serial        : %s\r\n", tucSerial);                     // Serial Number
 	iRet = TM_GetProductCode(tucProduct); CHECK(iRet==0, lblKO);
	Telium_Sprintf(pcBuffer,             "Product Code       : %.3s\r\n", tucProduct);                  // Product Code

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	iRet = TM_GetManufacturingDate(tcManufacturing); CHECK(iRet==0, lblKO);
	pcBuffer += Telium_Sprintf(pcBuffer, "Manufacturing      : %.8s\r\n", tcManufacturing);             // Manufacturing Date
	memset(tucSmcHard, 0, sizeof(tucSmcHard));
	memset(tucSmcSoft, 0, sizeof(tucSmcSoft));
	iRet = TM_GetCardReaderInfos(tucSmcHard, tucSmcSoft); CHECK(iRet==0, lblKO);                 // Smc Reader Hard and Software Infos
	Telium_Sprintf(pcBuffer,             "Coupler Hardware   : %.4s\r\n"
								  "Coupler Software   : %.4s\r\n", tucSmcHard, tucSmcSoft);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "===================================================================\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "ACTIVATION INFORMATIONS\r\n");
	Telium_Sprintf(pcBuffer,             "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	memset(tcActivation, 0, sizeof(tcActivation));
	iRet = TM_GetManufacturingDate(tcActivation); CHECK(iRet==0, lblKO);                         // First Activation Date
	pcBuffer += Telium_Sprintf(pcBuffer, "First Activation   : %s\r\n", tcActivation);
#ifndef __FRAMEWORK_TELIUM_PLUS__
	memset (tcProfile, 0, sizeof(tcProfile));
    Telium_Systemfioctl(SYS_FIOCTL_BOOSTER_GET_ACTIVATION_PROFILE, tcProfile);                   // Terminal Profile
	pcBuffer += Telium_Sprintf(pcBuffer, "SXX Extension      : %s\r\n", tcProfile);
#endif
#ifndef __FRAMEWORK_TELIUM_PLUS__
	memset (&xRepudiation, 0, sizeof(SYS_FIOCTL_REPUDIATION_INFO_GET_S));
	Telium_Systemfioctl(SYS_FIOCTL_REPUDIATION_INFO_GET, &xRepudiation);  					     // Repudiation table
	pcBuffer += Telium_Sprintf(pcBuffer, "Profile ID         : [%04d-%03d]\r\n", xRepudiation.Repudiation_Number1, xRepudiation.Repudiation_Number2);
#endif
	pcFlag = (char*)zFalse;
	iRet = IsTerminalIngetrust(PERIPH_DISPLAY);                                                  // Ingetrusted mode
	if (iRet == 1)
	{
		iRet = GetTerminalPKIVersion();
		switch (iRet)
		{
		case INGETRUST_TYPE_UNKNOWN: pcFlag="UNKNOWN"; break;
		case INGETRUST_TYPE_PKIv1:   pcFlag="PKIv1";   break;
		case INGETRUST_TYPE_PKIv3:   pcFlag="PKIv3";   break;
		default:                     pcFlag="NONE";    break;
		}
	}
	pcBuffer += Telium_Sprintf(pcBuffer, "INGETRUST Security : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;
    iRet = is_ZKA();                                                                             // Security ZKA mode
	if (iRet == 1)
		pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "ZKA Protection     : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;
	iRet = Telium_Systemfioctl(SYS_FIOCTL_SECURITY_GET_FALLBACK_DISABLED, (void*)NULL);                 // Fallback mode
	if (iRet == 0)
		pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "Fallback           : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;
	iRet = Telium_Systemfioctl(SYS_FIOCTL_SECURITY_GET_VARID_CHECKING, (void*)NULL);                    // Security VARID
	if (iRet == 1)
		pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "Security VARID     : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;
	iRet = Telium_Systemfioctl(SYS_FIOCTL_SECURITY_GET_SCHEME_VARID_CHECKING, (void*)NULL);             // Scheme VARID
	if (iRet == 1)
		pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "Scheme VARID       : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "===================================================================\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "MEMORY INFORMATIONS\r\n");
	Telium_Sprintf(pcBuffer,             "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;                                                                            // Flash & Ram sizes
	iRet = TM_GetInfoMemorySize(&iSizeFlash, &iSizeRam, &iRamFree, &iFlashFreeCode, &iFlashFreeData, &iSizeBlocksKO);
	pcBuffer += Telium_Sprintf(pcBuffer, "FLASH Memory       : %iKb\r\n", iSizeFlash/1024);
	Telium_Sprintf(pcBuffer,             "RAM Memory         : %iKb\r\n", iSizeRam/1024);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "===================================================================\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "HARDWARE INFORMATIONS\r\n");
	Telium_Sprintf(pcBuffer,             "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "Miscellaneous\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcFlag = (char*)zFalse;                                                                      // Printer
    if (IsPrinter() == 1)
    {
    	if (IsSlowPrinter() == 1)
    		pcFlag = (char*)zSlow;    // Slow
    	else
    		pcFlag = (char*)zFast;    // Fast
    }
	pcBuffer += Telium_Sprintf(pcBuffer, "PRINTER            : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                      // Display
    if (IsColorDisplay() == 1)
    	pcFlag = (char*)zColor;       // Color
    else
    {
    	pcFlag = (char*)z2Lines;      // 2 lines
    	if (IsSmallDisplay() == 1)
    		pcFlag = (char*)z128x64;  // 128x64
    	if (IsLargeDisplay() == 1)
    		pcFlag = (char*)z128x128; // 128x128
    }
	pcBuffer += Telium_Sprintf(pcBuffer, "DISPLAY            : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                      // Buzzer
    if (IsBUZZER() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "BUZZER             : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // Modem
	pcV34 = (char*)"";
    if (IsMODEM() == 1)
    {
    	pcFlag = (char*)zTrue;
    	hModem = Telium_Fopen("MODEM", "r*");
    	CHECK(hModem!=NULL, lblKO);

#ifndef __FRAMEWORK_TELIUM_PLUS__
    	memset(&xInfoModem, 0, sizeof(MODEM_FIOCTL_IDENT_S));
    	iRet = Telium_Fioctl(MODEM_FIOCTL_IDENT, &xInfoModem, hModem);
    	CHECK(iRet==0, lblKO);

    	switch(xInfoModem.manufacturer)
    	{
    	case MODEM_NETBRICKS: pcFlag = "NETBRICKS";  break;  // Modem Netbricks (Soft Modem)
    	case MODEM_MOTOROLA:  pcFlag = "MOTOROLA";   break;  // Modem Motorola (Soft Modem)
    	case MODEM_CONEXANT:  pcFlag = "CONEXANT";   break;  // Modem Conexant (Hard Modem)
    	default:              pcFlag = (char*)zTrue; break;
    	}
#endif

    	if (IsModemV34() == 1)
    		pcV34 = (char*)zV34; // V34
    }
	Telium_Sprintf(pcBuffer,             "MODEM              : %s %s\r\n", pcFlag, pcV34);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;                                                                       // Portable
    if (IsPortable() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "PORTABLE           : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                       // Tilto
    if (IsTILTO() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "TILTO              : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                       // MMC
    if (IsMMC() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "MMC                : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                       // Contactless
    if (IsCless() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "CLESS              : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                       // Hardware leds
    if (IsLedOnDisplay() == 0)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "HARDWARE LEDS      : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "Serial Links\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;                                                                         // RS232 COM0
    if (IsCOM0() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "COM0               : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                         // RS232/TTL COM1
    if (IsCOM1() == 1)
    {
    	if (IsCOM1RS232() == 1)
    		pcFlag = (char*)zTrue; // Com1 RS232
    	if (IsCOM1Pinpad() == 1)
    		pcFlag = (char*)zTTL;  // Com1 TTL
    }
	pcBuffer += Telium_Sprintf(pcBuffer, "COM1               : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                         // RS232 COM2
    if (IsCOM2() == 1)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "COM2               : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;                                                                         // RS232 COM3
    if (IsCOM3() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "COM3               : %s\r\n", pcFlag);
    pcFlag =  (char*)zFalse;                                                                        // RS232 COMN
    if (IsCOMN() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "COMN               : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                         // RS232 COMU
    if (IsCOMU() == 1)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "COMU               : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "USB Controler\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;                                                                         // USB Host
    if (IsUsbHost() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "USB HOST           : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                         // USB Slave
    if (IsUsbSlave() == 1)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "USB DEVICE         : %s\r\n", pcFlag);
    // USB BASE MISSING

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "MORPHO Device\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;                                                                          // Biometric
    if (IsBIO() == 1)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "BIO                : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "RADIO Device\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    if (IsRadio() == 1)
    {
    	if (IsRadioGPRS() == 1)                                                                      // GPRS
    	{
    		Telium_Sprintf(tcBuffer,     "GPRS               : YES\r\n");

    		// *** Send buffer to console (hyper terminal) ***
    		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
    		Telium_Mask_event(hCom, COM_SEND_END);
    		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

           	if (IsGPRS() != 0)
        	{
        	    hGprs = Telium_Stdperif((char*)"DGPRS", NULL);
        		CHECK(hGprs!=NULL, lblKO);

        		memset(&xInfoGprs, 0, sizeof(T_EGPRS_GET_INFORMATION));
        		iRet = gprs_GetInformation(hGprs, &xInfoGprs, sizeof(xInfoGprs));
        		CHECK(iRet==0, lblKO);

        		pcBuffer=tcBuffer;
        		pcBuffer += Telium_Sprintf(pcBuffer, "    Module Version : %s\r\n"             // Version
								              "    Imei Number    : %s\r\n"             // Imei
								              "    Sim ID         : %s\r\n",            // Id
									               xInfoGprs.module_software_version,
									               xInfoGprs.module_imei_number,
									               xInfoGprs.simIccId);
        		Telium_Sprintf(pcBuffer,             "    Network        : %s\r\n"             // Network
								              "    Provider       : %s\r\n",            // Provider
									               xInfoGprs.network_name,
									               xInfoGprs.sim_provider);

        		// *** Send buffer to console (hyper terminal) ***
        		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
        		Telium_Mask_event(hCom, COM_SEND_END);
        		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
        	}
    	}
    	else
    	{
    		Telium_Sprintf(tcBuffer,         "GPRS               : NO\r\n");

    		// *** Send buffer to console (hyper terminal) ***
    		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
    		Telium_Mask_event(hCom, COM_SEND_END);
    		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
    	}

    	pcBuffer=tcBuffer;
    	pcFlag = (char*)zFalse;
    	if (IsRadioCDMA() == 1)
    		pcFlag = (char*)zTrue;                                                                   // CDMA
		pcBuffer += Telium_Sprintf(pcBuffer, "CDMA               : %s\r\n", pcFlag);
    	pcFlag = (char*)zFalse;
    	if (IsBT() == 1)
    		pcFlag = (char*)zTrue;                                                                   // Bluetooth
		pcBuffer += Telium_Sprintf(pcBuffer, "BLUETOOTH          : %s\r\n", pcFlag);
    	pcFlag = (char*)zFalse;
    	if (IsRadioWifi() == 1)
    		pcFlag = (char*)zTrue;                                                                   // Wifi
		Telium_Sprintf(pcBuffer,             "WIFI               : %s\r\n", pcFlag);

		// *** Send buffer to console (hyper terminal) ***
		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
		Telium_Mask_event(hCom, COM_SEND_END);
		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
    }
	else
	{
		Telium_Sprintf(tcBuffer,             "RADIO              : NO\r\n");

		// *** Send buffer to console (hyper terminal) ***
		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
		Telium_Mask_event(hCom, COM_SEND_END);
		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
	}

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "Ethernet Controler\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	if (IsETHERNET() == 1)                                                                    // Ethernet
	{
		pcBuffer=tcBuffer;
		if (IsRadioETHERNET() == 1)
			pcBuffer += Telium_Sprintf(pcBuffer, "ETHERNET           : YES (Terminal)\r\n");         // Ethernet interface inside terminal
		else
			pcBuffer += Telium_Sprintf(pcBuffer, "ETHERNET           : YES (Base)\r\n");             // Ethernet interface inside base
		memset(tcMac, 0, sizeof(tcMac));
		HWCNF_EthernetGetMacAddress(tcMac);                                                   // Mac Address
		pcBuffer += Telium_Sprintf(pcBuffer, "    Mac Address    : %02X:%02X:%02X:%02X:%02X:%02X\r\n", tcMac[0], tcMac[1], tcMac[2], tcMac[3], tcMac[4], tcMac[5]);
		memset(&xEth, 0, sizeof(ip_param_s));
		iRet = HWCNF_EthernetGetCurrentParameters(&xEth, 0); CHECK(iRet==0, lblKO);
#ifdef __TELIUM3__
		xAddr.s_addr = xEth.addr;
		pcBuffer += Telium_Sprintf(pcBuffer, "    IP Address     : %s\r\n", inet_ntoa(xAddr));    // Ip
		xAddr.s_addr = xEth.netmask;
		Telium_Sprintf(pcBuffer,             "    Netmask        : %s\r\n", inet_ntoa(xAddr)); // Netmask
#else
		pcBuffer += Telium_Sprintf(pcBuffer, "    IP Address     : %s\r\n", UintToIp(xEth.addr));    // Ip
		Telium_Sprintf(pcBuffer,             "    Netmask        : %s\r\n", UintToIp(xEth.netmask)); // Netmask
#endif

		// *** Send buffer to console (hyper terminal) ***
		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
		Telium_Mask_event(hCom, COM_SEND_END);
		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

		pcBuffer=tcBuffer;
#ifdef __TELIUM3__
		xAddr.s_addr = xEth.gateway;
		pcBuffer += Telium_Sprintf(pcBuffer, "    Gateway        : %s\r\n", inet_ntoa(xAddr)); // Gateway
		xAddr.s_addr = xEth.dns1;
		pcBuffer += Telium_Sprintf(pcBuffer, "    DNS1           : %s\r\n", inet_ntoa(xAddr));    // Dns1
		xAddr.s_addr = xEth.dns2;
		Telium_Sprintf(pcBuffer,             "    DNS2           : %s\r\n", inet_ntoa(xAddr));    // Dns2
#else
		pcBuffer += Telium_Sprintf(pcBuffer, "    Gateway        : %s\r\n", UintToIp(xEth.gateway)); // Gateway
		pcBuffer += Telium_Sprintf(pcBuffer, "    DNS1           : %s\r\n", UintToIp(xEth.dns1));    // Dns1
		Telium_Sprintf(pcBuffer,             "    DNS2           : %s\r\n", UintToIp(xEth.dns2));    // Dns2
#endif

		// *** Send buffer to console (hyper terminal) ***
		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
		Telium_Mask_event(hCom, COM_SEND_END);
		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
	}
	else
	{
		Telium_Sprintf(tcBuffer,             "ETHERNET           : NO\r\n");

		// *** Send buffer to console (hyper terminal) ***
		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
		Telium_Mask_event(hCom, COM_SEND_END);
		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
	}

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "SWIPE Readers\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcFlag = (char*)zFalse;                                                                      // ISO1 reader
    if (IsISO1() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "ISO1               : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // ISO2 reader
    if (IsISO2() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "ISO2               : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // ISO3 reader
    if (IsISO3() == 1)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "ISO3               : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "-------------------------------------------------------------------\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "CAM-SAM Readers\r\n");
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcFlag = (char*)zFalse;                                                                      // CAM0 reader
    if (IsCAM1() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "CAM0               : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // CAM1 reader
    if (IsCAM2() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "CAM1               : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // CAM2 reader
    if (IsCAM3() == 1)
    	pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "CAM2               : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcFlag = (char*)zFalse;                                                                      // SAM0 reader
    if (IsSAM1() == 1)
    	pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "SAM1               : %s\r\n", pcFlag);
    pcFlag = (char*)zFalse;                                                                      // SAM1 reader
	if (IsSAM2() == 1)
		pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "SAM2               : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // SAM3 reader
	if (IsSAM3() == 1)
		pcFlag = (char*)zTrue;
	pcBuffer += Telium_Sprintf(pcBuffer, "SAM3               : %s\r\n", pcFlag);
	pcFlag = (char*)zFalse;                                                                      // SAM4 reader
	if (IsSAM4() == 1)
		pcFlag = (char*)zTrue;
	Telium_Sprintf(pcBuffer,             "SAM4               : %s\r\n", pcFlag);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "===================================================================\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "SOFTWARE INFORMATIONS\r\n");
	Telium_Sprintf(pcBuffer,             "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
#ifndef __FRAMEWORK_TELIUM_PLUS__
	memset (tcFirmware, 0, sizeof(tcFirmware));
	Telium_Systemfioctl(SYS_FIOCTL_GET_SECURITY_FIRMWARE_ID, tcFirmware);                        // Get firmware version
	pcBuffer += Telium_Sprintf (pcBuffer, "Firmware Version   : %s\r\n", tcFirmware);
#endif
	iRet = BoosterType();                                                                        // Booster type
	pcBuffer += Telium_Sprintf (pcBuffer, "Booster type       : %d\r\n", iRet);
#ifndef __FRAMEWORK_TELIUM_PLUS__
	memset(tcBooster, 0, sizeof(tcBooster));
	Telium_Systemfioctl(SYS_FIOCTL_BOOSTER_GET_ROM_VERSION, tcBooster);                          // Booster version
	memset(tcThunder, 0, sizeof(tcThunder));
	Telium_Systemfioctl(SYS_FIOCTL_THUNDER_GET_ROM_VERSION, tcThunder);                          // Thunder version
	pcBuffer += Telium_Sprintf (pcBuffer, "Booster Version    : %.4s\r\n"
			                       "Thunder Version    : %.4s\r\n",
			                              tcBooster,
			                              tcThunder);
#endif
	Telium_Sprintf(pcBuffer,              "-------------------------------------------------------------------\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                                           // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if (hModem)
		Telium_Fclose(hModem);                                                                   // Close "modem" peripheral

	if(hCom)
		Telium_Fclose(hCom);                                                                     // Close "com" peripheral
}


