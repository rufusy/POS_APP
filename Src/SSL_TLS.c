#include <sdk_tplus.h> // IngeDev automatic refactoring - 2017/02/07 09:32:52 - Replace '#include <SDK30.h>' to '#include <sdk_tplus.h>'
#include <X509_.h>
#include "globals.h"


const char * disk_tmp = "file://flash/HOST";
const unsigned long disk_tmp_size = 8192;
const char * rsa_tmp = "KEY.CRYPT";
const char * csr_tmp = "KEY.CSR";

#define NB_OPTIONS 4

CsrOption listOptions[NB_OPTIONS] = {
		{ NID_organizationalUnitName, "MOVE2500" },
		{ NID_commonName, "POS" },
		{ NID_countryName, "TZ" },
		{ NID_organizationName, "BSEAPP" }
};

//
// Generate a crypted RSA key (1024bits, exponent=3) and a CSR in a tmp disk
// It returns: OK (0) if successful, otherwise negative error code
//
int GenerateKeyAndCSR( void ) {
//	S_FS_FILE *hFile;
	char file_path[FS_PATHSIZE];
	int size;
	char * buffer;
	int buflen = 2048;

	memset(file_path, 0, sizeof(file_path));

	fncDisplayData_Goal("","","Please Wait...",500,0);

	// RSA Key generation (1024 bits. Exponent 3.)
	buffer = umalloc( buflen );
	size = rsaKeyGen( 2048, 3, buffer, buflen );
	if( size <= 0 ) {
		ufree( buffer );
		return -1;
	}

	// Save the generated RSA in a temporary file.
	Telium_Sprintf( file_path,"%s%c%s", disk_tmp, FS_SEPARATOR, rsa_tmp );
	//        hFile = FS_open( file_path, "w+" );
	//        FS_write( buffer, size, sizeof(byte), hFile );
	//        FS_close( hFile );
	fncOverwriteConfigFile((char *)rsa_tmp, buffer, size);

	// Certificate Signature Request generation.
	size = rsaCsrGen  ( file_path, listOptions, NB_OPTIONS, buffer, buflen );
	//size = X509_GenCsr( file_path, listOptions, NB_OPTIONS, buffer, buflen );
	if( size <= 0 ) {
		ufree( buffer );
		return -1;
	}

	// Save the generated CSR in a temporary file.
	Telium_Sprintf( file_path, "%s%c%s", disk_tmp, FS_SEPARATOR, csr_tmp );
	//        hFile = FS_open( file_path, "w+" );
	//        FS_write( buffer, size, sizeof(byte),hFile );
	//        FS_close( hFile );
	fncOverwriteConfigFile((char *)csr_tmp, buffer, size);

	ufree(buffer);

	fncDisplayData_Goal("","","Done...",500,0);

	return 0;
}
