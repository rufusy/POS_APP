//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  CONFSOFTWARE.C                     (Copyright INGENICO 2012)
//============================================================================
//  Created :       17-July-2012     Kassovic
//  Last modified : 17-July-2012     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//               *** Describe the software configuration ***
//          Transmission by serial communication COM0/COM5/COM_EXT
//             Test done with Hyper-terminal (8-N-1-115200)
//  Application(s), Dll(s), Driver(s), Param(s), Key(s) and memory state.
//                                                                            
//  List of routines in file :  
//      ConfSoftware : Configuration regarding the software loaded.                                           
//                            
//  File history :
//  071712-BK : File created
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
    /* */

//****************************************************************************
//      PRIVATE TYPES                                                       
//****************************************************************************
    /* */

//****************************************************************************
//      PRIVATE DATA                                                        
//****************************************************************************
static char tcBuffer[256+1];

//****************************************************************************
//                  void ConfSoftware(doubleword uiCom)
//  This function sent to a console (hyper-terminal) the configuration
//  regarding the software loaded inside the terminal.
//   Similar to the configuration ticket from Manager:
//   (TELIUM MANAGER-Consultation-Configuration-Software)
//  1) Application(s) => App Name, App File, CRC, App Type, Loaded flag,
//                       App position, Ram/Flash used
//  2) Dll(s) => Dll Name, Dll File, CRC, Dll Type, Loaded flag, Dll position,
//               Ram/Flash used
//  3) Driver(s) => Driver Name, Driver File, Driver Type, Loaded flag,
//                  Driver position, Ram/Flash used
//  4) Param(s) => Param Name, Param File, Param Type, Loaded flag,
//                 Param position, Ram/Flash used
//  5) Key(s) => Key Name, Key File, Key Type, Loaded flag, Key position
//               Ram/Flash used
//  6) Memory state => Total Ram/Flash used, Size Ram, Free Ram,
//                     Size Flash, Free Flash (code+data)
//      - ObjectGetNumber() : return the object number available for a given object type.
//      - ObjectGetInfo() : get information about the object.
//      - ObjectGetSize() : retrieve memory information of an object.
//      - RamGetSize() : return RAM size.
//      - FreeSpace() : return the free RAM space.
//      - FS_AreaGetSize() : return size of flash area (CFS or DFS).
//      - FS_AreaGetFreeSize() : return free size of a flash area (CFS or DFS).
//      - FS_AreaGetGarbageSize() : return garbage size of a flash area (CFS or DFS).
//  This function has parameters.
//    uiCom (I-) : Communication channel.
//  This function has no return value.
//****************************************************************************

void ConfSoftware(doubleword uiCom)
{
	// Local variables
    // ***************
	Telium_File_t *hCom=NULL;
	Telium_size_t uiLen;
	char tcDriver[24+1];
#ifndef __TELIUM3__
	object_type_t eType;
	object_info_t xInfo;
	object_size_t xSize;
#else
	unsigned int             packId;
	T_OSL_PACKAGE_INFO_TYPE  eType;
	T_OSL_HPACKAGE_INFO      pkgInfos;
	char					 name[24+1];	      /* Object name (application family given at generation time ) */
    char					 file_name[24+1];     /* Binary name */
#endif

#ifndef __TELIUM3__
	doubleword uiRamLength, uiFlashLength, uiTotalRamLength=0, uiTotalFlashLength=0;
#endif
	int iNbr, i;
	char* pcBuffer;
    int iRet;

    // Com channel initialization
    // **************************
	switch (uiCom)
	{
	case COM0:
		hCom = Telium_Fopen("COM0", "rw*");
		CHECK(hCom!=NULL, lblKO);                                  // Open "com0" peripheral
	    iRet = Telium_Format("COM0", 115200, 8, 1, NO_PARITY, EVEN, 0);   // Format the COM0 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM1:
		hCom = Telium_Fopen("COM1", "rw*");
		CHECK(hCom!=NULL, lblKO);                                  // Open "com1" peripheral
	    iRet = Telium_Format("COM1", 115200, 8, 1, NO_PARITY, EVEN, 0);   // Format the COM1 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM2:
		hCom = Telium_Fopen("COM2", "rw*");
		CHECK(hCom!=NULL, lblKO);                                  // Open "com2" peripheral
	    iRet = Telium_Format("COM2", 115200, 8, 1, NO_PARITY, EVEN, 0);   // Format the COM2 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM5:
		hCom = Telium_Fopen("COM5", "rw*");                        // Open "com5" peripheral
		CHECK(hCom!=NULL, lblKO);
		break;
	case COM_EXT:                                                  // Open "Converter USB->RS232" peripheral
		// Telium supports the following drivers:
		// - "COM_KEYSPAN" (Keyspan USA19HS)
		// - "COM_SL" (SiliconLabs CP2102)
		// - "COM20" (Prolific 2303)
		// - "COM21" (FTDI)
		// - "COM_MGBX" (Generic Magic Box)
		// With event generated COM_EXT
		hCom = OpenConverter(tcDriver, sizeof(tcDriver));
		CHECK(hCom!=NULL, lblKO);
	    iRet = Telium_Format(tcDriver, 115200, 8, 1, NO_PARITY, EVEN, 0); // Format converter channel
	    CHECK(iRet>=0, lblKO);
		break;
	default:
		goto lblKO;
	}
	GL_Dialog_Message(hGoal, NULL, "Configuration Software\nTransmitting...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

    // Objects configuration (Applications, Dlls, Drivers, Parameter files, Key files)
    // *****************************************************************************
	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "###################################################################\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "#                   SOFTWARE CONFIGURATION                        #\r\n");
	Telium_Sprintf(pcBuffer,             "###################################################################\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

#ifndef __TELIUM3__
	eType = OBJECT_TYPE_APPLI;                                                                // Start by Application object first
    i=0;
    while(eType <= OBJECT_TYPE_KEY)                                                           // Finish by Key object
    {
    	iNbr = ObjectGetNumber(eType);                  // Object number available
#else
    eType = OSL_PACKAGE_TYPE_TRUSTED;
    i=0;
    while(eType <= OSL_PACKAGE_TYPE_T3_CUSTOM_APPLICATION)
    {
    	iNbr = OSL_PackageList_GetNumber(eType);        // Object number available
#endif

    	while(i<iNbr)
    	{
    	    if (i==0)
    	    {
    	    	pcBuffer=tcBuffer;                                                            // Send which object to analyze
    	    	pcBuffer += Telium_Sprintf (pcBuffer, "===================================================================\r\n");
#ifndef __TELIUM3__
    	    	switch (eType)
    	    	{
    	    	case OBJECT_TYPE_APPLI:  pcBuffer += Telium_Sprintf(pcBuffer, "APPLICATION(S)\r\n"); break;
    	    	case OBJECT_TYPE_DLL:    pcBuffer += Telium_Sprintf(pcBuffer, "DLL(S)\r\n");         break;
    	    	case OBJECT_TYPE_DRIVER: pcBuffer += Telium_Sprintf(pcBuffer, "DRIVER(S)\r\n");      break;
    	    	case OBJECT_TYPE_PARAM:  pcBuffer += Telium_Sprintf(pcBuffer, "PARAM(S)\r\n");       break;
    	    	default:                 pcBuffer += Telium_Sprintf(pcBuffer, "KEY(S)\r\n");         break;
    	    	}
#else
    	    	switch (eType)
    	    	{
    	    	case OSL_PACKAGE_TYPE_TRUSTED:                pcBuffer += Telium_Sprintf(pcBuffer, "TRUSTED\r\n");               break;
    	    	case OSL_PACKAGE_TYPE_OS: 					  pcBuffer += Telium_Sprintf(pcBuffer, "OS\r\n");                    break;
    	    	case OSL_PACKAGE_TYPE_LEGACY_APPLICATION:     pcBuffer += Telium_Sprintf(pcBuffer, "LEGACY APPLICATION(S)\r\n"); break;
    	    	case OSL_PACKAGE_TYPE_LEGACY_DLL:             pcBuffer += Telium_Sprintf(pcBuffer, "DLL(S)\r\n");                break;
    	    	case OSL_PACKAGE_TYPE_LEGACY_PARAMETER:       pcBuffer += Telium_Sprintf(pcBuffer, "PARAM(S)\r\n");              break;
    	    	case OSL_PACKAGE_TYPE_T3_FRAMEWORK: 		  pcBuffer += Telium_Sprintf(pcBuffer, "T3 FRAMEWORK\r\n");          break;
    	    	case OSL_PACKAGE_TYPE_T3_CUSTOM_APPLICATION:  pcBuffer += Telium_Sprintf(pcBuffer, "CUSTOM APPLICATION(S)\r\n"); break;
    	    	default:                                      pcBuffer += Telium_Sprintf(pcBuffer, "UNKNOWN\r\n");               break;
    	    	}
#endif
    	    	Telium_Sprintf(pcBuffer,              "===================================================================\r\n");

        		// *** Send buffer to console (hyper terminal) ***
        		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
        		Telium_Mask_event(hCom, COM_SEND_END);
        		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);
    	    }

#ifndef __TELIUM3__
    		ObjectGetInfo(eType, i, &xInfo);                                                  // Information about the object
    		ObjectGetSize(eType, xInfo.application_type, sizeof(object_size_t), &xSize);      // Memory about the object

    	    pcBuffer=tcBuffer;                                                                // Send full description of the object
    	    pcBuffer += Telium_Sprintf(pcBuffer, "%s\r\n", xInfo.name);
    	    pcBuffer += Telium_Sprintf(pcBuffer, "File: %s\r\n", xInfo.file_name);
    	    pcBuffer += Telium_Sprintf(pcBuffer, "CRC: 0x%04X Type: 0x%04X Loaded: %d (%d)\r\n", xInfo.crc, xInfo.application_type, xInfo.loaded_flag, i);

    		// *** Send buffer to console (hyper terminal) ***
    		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
    		Telium_Mask_event(hCom, COM_SEND_END);
    		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    		uiRamLength = xSize.code_size + xSize.data_size + xSize.umalloc_size;             // Ram used by the object
    		uiFlashLength = xSize.flash_prog + xSize.flash_data;                              // Flash used by the object

    		pcBuffer=tcBuffer;                                                                // Send memory used by the object
    		pcBuffer += Telium_Sprintf(pcBuffer, "RAM length: %i Code: %i Data: %i Dyn: %i\r\n", uiRamLength, xSize.code_size, xSize.data_size, xSize.umalloc_size);
    		pcBuffer += Telium_Sprintf(pcBuffer, "FLASH length: %i Code: %i Data: %i\r\n", uiFlashLength, xSize.flash_prog, xSize.flash_data);
    	    Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");
#else
    	    // Get the package Id
    	    if (OSL_PackageList_GetPackageId(eType, i, &packId) == OSL_SUCCESS) {
    	    	pkgInfos = OSL_PackageInfo_Open(eType, packId);
    	    	if (pkgInfos != NULL) {

    	    		OSL_PackageInfo_GetFamilyName(pkgInfos, name, sizeof(name));
    	    		OSL_PackageInfo_GetBinaryName(pkgInfos, file_name, sizeof(name));

    	    		pcBuffer=tcBuffer;                                                                // Send full description of the object
    	    		pcBuffer += Telium_Sprintf(pcBuffer, "%s\r\n", name);
    	    		pcBuffer += Telium_Sprintf(pcBuffer, "File: %s\r\n", file_name);
    	    		pcBuffer += Telium_Sprintf(pcBuffer, "CRC: 0x%04X Type: 0x%04X (%d)\r\n",
    	    							OSL_PackageInfo_GetCrc(pkgInfos),
    	    							OSL_PackageInfo_GetType(pkgInfos),
    	    							i);
    	    	}
    	    	OSL_PackageInfo_Close(pkgInfos);
    	    }
#endif

    		// *** Send buffer to console (hyper terminal) ***
    		uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
    		Telium_Mask_event(hCom, COM_SEND_END);
    		iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

#ifndef __TELIUM3__
    		if (xInfo.loaded_flag == 1)
    			uiTotalRamLength += uiRamLength;
    		uiTotalFlashLength += uiFlashLength;
#endif
    		i++;
    	}
    	eType+=1;
    	i=0;
    }

#ifndef __TELIUM3__
	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "===================================================================\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "MEMORY STATE\r\n");
	Telium_Sprintf (pcBuffer,            "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;                                                                        // Send full Ram/Flash memory information
	pcBuffer += Telium_Sprintf(pcBuffer, "Total length Ram: %i\r\n", uiTotalRamLength);
	pcBuffer += Telium_Sprintf(pcBuffer, "Total length Flash: %i\r\n", uiTotalFlashLength);
	pcBuffer += Telium_Sprintf(pcBuffer, "RAM Memory: %i RAM Free: %ld\r\n", RamGetSize(), FreeSpace());
	pcBuffer += Telium_Sprintf(pcBuffer, "FLASH Memory: %i FLASH Free: %i\r\nCode Free: %i Data Free: %i\r\n",
									FS_AreaGetSize(FS_WO_ZONE_APPLICATION) + FS_AreaGetSize(FS_WO_ZONE_DATA),
									FS_AreaGetFreeSize(FS_WO_ZONE_APPLICATION) + FS_AreaGetFreeSize(FS_WO_ZONE_DATA),
									FS_AreaGetFreeSize(FS_WO_ZONE_APPLICATION),
									FS_AreaGetFreeSize(FS_WO_ZONE_DATA));
	Telium_Sprintf(pcBuffer,             "-------------------------------------------------------------------\r\n");
#endif

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                                        // None-classified low level error
	GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
	goto lblEnd;
lblEnd:
	if(hCom)
		Telium_Fclose(hCom);                                                                  // Close "com" peripheral
}


