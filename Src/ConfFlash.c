//****************************************************************************
//       INGENICO                                INGEDEV 7                   
//============================================================================
//       FILE  CONFFLASH.C                        (Copyright INGENICO 2012)
//============================================================================
//  Created :       17-July-2011     Kassovic
//  Last modified : 17-July-2011     Kassovic
//  Module : TRAINING                                                          
//                                                                          
//  Purpose :                                                               
//            *** Describe the complete Flash memory configuration ***
//          Transmission by serial communication COM0/COM5/COM_EXT
//             Test done with Hyper-terminal (8-N-1-115200)
//  CFS disk (System disk)
//  DFS disks (Host disk, Os disks, Manager disks,..., Application disks)
//                                                                            
//  List of routines in file :  
//      ReadDirectories : Read files from a disk.
//      ConfFlash : Configuration regarding the Flash memory.                                            
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
static Telium_File_t *hCom=NULL;

static T_GL_HDIR hDir;
static S_FS_FILE *hFid;

static int iTotStorage;
static int iTotFiles;
static char tcBuffer[256+1];

//****************************************************************************
//            int ReadDirectories (doubleword uiCom, char *pcDir)
//  This function parses directories and sub-directories inside a disk to
//  read files (Name, Size and Storage). A recursive method is used.
//  The information is sent through COM0 to a console.
//   - GL_Dir_Open() : open a flow on a directory.
//   - GL_Dir_Next() : select the next item in a directory.
//   - GL_Dir_IsDirectory() : indicate if the selected item is a directory.
//   - GL_Dir_GetName() : return the name of the selected item.
//  This function has parameters.
//    uiCom (I-) : Communication channel.
//	  pcDir (I-) : Path regarding the directory to parse.
//  This function has return value.
//    <0 : Open flow on directory failed.
//     0 : Read files on directory failed.
//    >0 : Read files on directory done.
//****************************************************************************

static int ReadDirectories(doubleword uiCom, char *pcDir)
{
	// Local variables
	// ***************
	char tcDir[128+1];
	unsigned long ulStorage;
	int iRet;

	// Parsing directories and sub-directories in progress
	// ***************************************************
	iRet=-1;                                                                     // By default open flow on directory failed
    Telium_Sprintf(tcDir, "dir://flash%s", pcDir);
	hDir = GL_Dir_Open(tcDir);                                                   // *** Open flow on a directory to read files "dir://flash/DISKNAME/DIRECTORY/" ***
	if(hDir)
	{
		iRet=0;                                                                  // By default reading files on directory failed
		while(GL_Dir_Next(hDir) == GL_SUCCESS)                                   // *** Read files on a directory "dir://flash/DISKNAME/DIRECTORY/" ***
		{
			iRet=1;                                                              // Access file or sub-directory done
			if (GL_Dir_IsDirectory(hDir))                                        // Check if SUB-DIRECTORY
			{
				Telium_Sprintf(tcDir, "%s/%s", pcDir, GL_Dir_GetName(hDir));
				Telium_Sprintf(tcBuffer, "%s Size: -1 Storage: -1\r\n", tcDir);
				iRet=ReadDirectories(uiCom, tcDir);                              // *** Open flow on a sub-directory to read files ***
				if (iRet==0)                                                     // Access files on sub-directory failed
				{
					// *** Send buffer to console (hyper terminal) ***
					Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom);   // Send sub-directory without access
					Telium_Mask_event(hCom, COM_SEND_END);
					Telium_Ttestall(uiCom, 5*100);
					iRet=1;
				}
				return iRet;
			}
			else
			{
				Telium_Sprintf(tcDir, "%s/%s", pcDir, GL_Dir_GetName(hDir));
				hFid = FS_open(tcDir, "r");                                      // Open file in reading mode "/DISKNAME/DIRECTORY/FILENAME"
				ulStorage = FS_StorageSize(hFid);                                // File description (Name, Size and Storage)
				Telium_Sprintf(tcBuffer, "%s Size: %ld Storage: %ld\r\n", tcDir, FS_length(hFid), ulStorage);

            	if (ulStorage != -1)                                             // Update counter if necessary
            	{
            		iTotStorage += ulStorage;
        		    iTotFiles++;
            	}
    		    FS_close(hFid);                                                  // Close file

				// *** Send buffer to console (hyper terminal) ***
				Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom);       // Send file description
				Telium_Mask_event(hCom, COM_SEND_END);
				Telium_Ttestall(uiCom, 5*100);
			}
		}
	}

	return iRet;
}

//****************************************************************************
//                  void ConfFlash (doubleword uiCom)
//  This function sent to a console (hyper-terminal) the complete configuration
//  regarding the Flash memory.
//  1) 3 blocks of Flash => Size, Free and Garbage spaces
//  2) Issuer, Type, Size, Pages by Block, Block size and Bad Blocks
//  3) CFS (SYSTEM DISK) => Size, Free and Garbage spaces
//     DFS (OTHER DISKS) => Size, Free and Garbage spaces
//  4) Disks => Name, Size and Free spaces
//              Access, Appli owner, Appli using it
//              Files description => Name, Size and Storage
//     - FS_Diag() : retrieve flash memory informations.
//     - FS_AreaGetSize() : return size of flash area (CFS or DFS).
//     - FS_AreaGetFreeSize() : return free size of a flash area (CFS or DFS).
//     - FS_AreaGetGarbageSize() : return garbage size of a flash area (CFS or DFS).
//     - FS_disklist() : list the number of disks.
//  This function has parameters.
//    uiCom (I-) : Communication channel.
//  This function has no return value.
//****************************************************************************

void ConfFlash(doubleword uiCom)
{
	// Local variables
    // ***************
	Telium_size_t uiLen;
	char tcDriver[24+1];
	S_FS_DIAG xDiag;
	doubleword uiNbrDisk;
	S_FS_INFO_CONTEXT xInfo;
	char* pcBuffer;
	char tcName[1+FS_DISKNAMESIZE+1];
	doubleword uiMode;  // Bug in prototype => 2nd parameter (output) on FS_mount returns Mode (FS_WRITEONCE or FS_WRITEMANY)
                        // and NOT AccessMode (FS_NOFLAGS or FS_RONLYMOD or FS_WRTMOD)
	char tcAccessMode[16+1];
    int iRet, i;

    // Com channel initialization
    // **************************
	switch (uiCom)
	{
	case COM0:
		hCom = Telium_Fopen("COM0", "rw*");
		CHECK(hCom!=NULL, lblKO);                                         // Open "com0" peripheral
	    iRet = Telium_Format("COM0", 115200, 8, 1, NO_PARITY, EVEN, 0);   // Format COM0 channel
		CHECK(iRet>=0, lblKO);
		break;
	case COM1:
		hCom = Telium_Fopen("COM1", "rw*");
		CHECK(hCom!=NULL, lblKO);                                         // Open "com1" peripheral
	    iRet = Telium_Format("COM1", 115200, 8, 1, NO_PARITY, EVEN, 0);   // Format COM1 channel
		CHECK(iRet>=0, lblKO);
		break;
	case COM2:
		hCom = Telium_Fopen("COM2", "rw*");
		CHECK(hCom!=NULL, lblKO);                                         // Open "com2" peripheral
	    iRet = Telium_Format("COM2", 115200, 8, 1, NO_PARITY, EVEN, 0);   // Format COM2 channel
	    CHECK(iRet>=0, lblKO);
		break;
	case COM5:
		hCom = Telium_Fopen("COM5", "rw*");
		CHECK(hCom!=NULL, lblKO);                                         // Open "com5" peripheral
		break;
	case COM_EXT:                                                         // Open "Converter USB->RS232" peripheral
		// Telium supports the following drivers:
		// - "COM_KEYSPAN" (Keyspan USA19HS)
		// - "COM_SL" (SiliconLabs CP2102)
		// - "COM20" (Prolific 2303)
		// - "COM21" (FTDI)
		// - "COM_MDBX" (Generic Magic Box)
		// With event generated COM_EXT
		hCom = OpenConverter(tcDriver, sizeof(tcDriver));
		CHECK(hCom!=NULL, lblKO);
	    iRet = Telium_Format(tcDriver, 115200, 8, 1, NO_PARITY, EVEN, 0); // Format converter channel
	    CHECK(iRet>=0, lblKO);
		break;
	default:
		goto lblKO;
	}
	GL_Dialog_Message(hGoal, NULL, "Configuration Flash\nTransmitting...", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);

    // Flash memory configuration
    // **************************
    FS_Diag(&xDiag);

	pcBuffer=tcBuffer;
	pcBuffer += Telium_Sprintf(pcBuffer, "###################################################################\r\n");
	pcBuffer += Telium_Sprintf(pcBuffer, "#                     FLASH CONFIGURATION                         #\r\n");
	Telium_Sprintf(pcBuffer,             "###################################################################\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    pcBuffer=tcBuffer;
    pcBuffer += Telium_Sprintf(pcBuffer, "Area[0] Ident: %i Size: %ld Free: %ld Garbage: %ld\r\n", xDiag.Area[0].Ident, xDiag.Area[0].Size, xDiag.Area[0].Free, xDiag.Area[0].Erasable);
    pcBuffer += Telium_Sprintf(pcBuffer, "Area[1] Ident: %i Size: %ld Free: %ld Garbage: %ld\r\n", xDiag.Area[1].Ident, xDiag.Area[1].Size, xDiag.Area[1].Free, xDiag.Area[1].Erasable);
    pcBuffer += Telium_Sprintf(pcBuffer, "Area[2] Ident: %i Size: %ld Free: %ld Garbage: %ld\r\n", xDiag.Area[2].Ident, xDiag.Area[2].Size, xDiag.Area[2].Free, xDiag.Area[2].Erasable);

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

	pcBuffer=tcBuffer;
    pcBuffer += Telium_Sprintf(pcBuffer, "Issuer: 0x%02X Type: %ld Size: %ld\r\n", (int)xDiag.DeviceMaker, xDiag.DeviceType, xDiag.SizeFlash);
    pcBuffer += Telium_Sprintf(pcBuffer, "PagesBlock: %ld Block: %ld BadBlocks: %ld\r\n", xDiag.PagesBlock, xDiag.SizeBlock, xDiag.BadBlockNb);
	Telium_Sprintf (pcBuffer,            "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    pcBuffer=tcBuffer;
    pcBuffer += Telium_Sprintf(pcBuffer, "Code File System (SYSTEM disk)\r\n");                      // *** CFS => Total size, Free size, Garbage size(become free after garbage collection) ***
    pcBuffer += Telium_Sprintf(pcBuffer, "Size: %i ", FS_AreaGetSize(FS_WO_ZONE_APPLICATION));
    pcBuffer += Telium_Sprintf(pcBuffer, "Free: %i ", FS_AreaGetFreeSize(FS_WO_ZONE_APPLICATION));
    pcBuffer += Telium_Sprintf(pcBuffer, "Garbage: %i\r\n", FS_AreaGetGarbageSize(FS_WO_ZONE_APPLICATION));
    pcBuffer += Telium_Sprintf(pcBuffer, "Data File System (Other disks)\r\n");                      // *** DFS => Total size, Free size, Garbage size(become free after garbage collection) ***
    pcBuffer += Telium_Sprintf(pcBuffer, "Size: %i ", FS_AreaGetSize(FS_WO_ZONE_DATA));
    pcBuffer += Telium_Sprintf(pcBuffer, "Free: %i ", FS_AreaGetFreeSize(FS_WO_ZONE_DATA));
    pcBuffer += Telium_Sprintf(pcBuffer, "Garbage: %i\r\n", FS_AreaGetGarbageSize(FS_WO_ZONE_DATA));
	Telium_Sprintf (pcBuffer,            "===================================================================\r\n");

	// *** Send buffer to console (hyper terminal) ***
	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
	Telium_Mask_event(hCom, COM_SEND_END);
	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    // Disks memory information
    // ************************
    FS_dsklist(&uiNbrDisk, &xInfo);                                                           // *** Retrieve number of disks ***
    for (i=0; i<uiNbrDisk; i++)
    {
    	Telium_Sprintf(tcName, "/%.16s", xInfo.DiskIdent[i].Label);                                  // *** Mount disk "/DISKNAME" ***
    	iRet = FS_mount (tcName, &uiMode);
    	pcBuffer=tcBuffer;
    	pcBuffer += Telium_Sprintf(pcBuffer, "Disk: %s ", tcName);                                   // Disk name
    	pcBuffer += Telium_Sprintf(pcBuffer, "Size: %ld ", FS_dsksize(tcName));                      // Disk total size
//    	pcBuffer += Telium_Sprintf(pcBuffer, "Size1: %i ", FS_AreaGetSize(FS_WO_ZONE_APPLICATION));
//    	pcBuffer += Telium_Sprintf(pcBuffer, "Free1: %i ", FS_AreaGetFreeSize(FS_WO_ZONE_APPLICATION));
    	pcBuffer += Telium_Sprintf(pcBuffer, "Free: %ld\r\n", FS_dskfree(tcName));                   // Disk free size

    	iRet = FS_readmode(tcName);                                                           // *** Access disk ***
    	switch (iRet)
    	{
    	case FS_WRTMOD:   strcpy(tcAccessMode, "ReadWrite"); break;                           // Read/Write mode
    	case FS_RONLYMOD: strcpy(tcAccessMode, "ReadOnly");  break;                           // Read only mode
    	default:          strcpy(tcAccessMode, "NoAccess");  break;                           // No access mode
    	}
    	pcBuffer += Telium_Sprintf(pcBuffer, "Access: %s ", tcAccessMode);                           // Disk access
    	pcBuffer += Telium_Sprintf(pcBuffer, "AppliCreate: 0x%04X ", xInfo.DiskIdent[i].NumAppli);   // Which appli creates the disk
    	Telium_Sprintf(pcBuffer,             "AppliUsed: 0x%04X\r\n", _ING_APPLI_TYPE);              // Which appli uses the disk

		// *** Send buffer to console (hyper terminal ***
    	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
    	Telium_Mask_event(hCom, COM_SEND_END);
    	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    	iTotStorage =0;                                                                       // Reset storage size in disk
    	iTotFiles=0;                                                                          // Reset number of files in disk
    	Telium_Sprintf(tcName, "/%.16s", xInfo.DiskIdent[i].Label);                                  // *** Open flow on a directory to read files "/DISKNAME/*" ***
    	iRet=ReadDirectories(uiCom, tcName);
    	pcBuffer=tcBuffer;
    	if (iRet>=0)
    	{
        	pcBuffer += Telium_Sprintf(pcBuffer, "%i files ", iTotFiles);                            // Total files in disk
    		pcBuffer += Telium_Sprintf(pcBuffer, "Storage: %i\r\n", iTotStorage);                    // Total storage size in disk
    	}
    	Telium_Sprintf (pcBuffer,                "-------------------------------------------------------------------\r\n");

        // *** Send buffer to console (hyper terminal) ***
    	uiLen = Telium_Fwrite((void*)tcBuffer, 1, strlen(tcBuffer), hCom); CHECK(uiLen>0, lblKO);
    	Telium_Mask_event(hCom, COM_SEND_END);
    	iRet = Telium_Ttestall(uiCom, 5*100); CHECK(iRet!=0, lblKO);

    	tcName[strlen(tcName)-1]=0;                                                           // *** Unmount disk "/DISKNAME" ***
    	FS_unmount(tcName);
    }

    goto lblEnd;

	// Errors treatment
    // ****************
lblKO:                                                                                        // None-classified low level error
    GL_Dialog_Message(hGoal, NULL, "Processing Error", GL_ICON_ERROR, GL_BUTTON_VALID, 5*1000);
    goto lblEnd;
lblEnd:
	if(hCom)
	{
		Telium_Fclose(hCom);                                                                  // Close "com" peripheral
		hCom=NULL;
	}
}


