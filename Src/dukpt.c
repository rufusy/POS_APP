//-------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "dukpt.h"

//-------------------------------------------------------------------------------------------------------

#define DES_ENCRYPT 1
#define DES_DECRYPT 0
//-------------------------------------------------------------------------------------------------------

static  byte BDK_MASK[16] = {0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00};

//-------------------------------------------------------------------------------------------------------

/**
 * AND two arrays
 *
 * @param out Output array
 * @param in1 In array 1
 * @param in2 In array 2
 *
 * @param len Length, must be equal for all three arrays
 */
static void And( byte *  out, byte *  in1, byte *  in2, int len	) {
	int i = 0;

	for (i = 0; i < len; i++) {
		out[i] = in1[i] & in2[i];
	}
}

//-------------------------------------------------------------------------------------------------------

/**
 * OR two arrays
 *
 * @param out Output array
 * @param in1 In array 1
 * @param in2 In array 2
 *
 * @param len Length, must be equal for all three arrays
 */
static void Or( byte *  out, byte *  in1, byte *  in2, int len	) {
	int i = 0;

	for (i = 0; i < len; i++) {
		out[i] = in1[i] | in2[i];
	}
}

//-------------------------------------------------------------------------------------------------------

/**
 * XOR two arrays
 *
 * @param out Output array
 * @param in1 In array 1
 * @param in2 In array 2
 *
 * @param len Length, must be equal for all three arrays
 */
static void Xor( byte *  out, byte *  in1, byte *  in2, int len	) {
	int i = 0;
	for (i = 0; i < len; i++) {
		out[i] = in1[i] ^ in2[i];
	}
}

static void DES_ecb_encrypt(byte * Unencrypted, byte * EncryptedData, byte * key, int Encr_Decr){
	switch (Encr_Decr) {
	case 0: //Decrypting
		stdSED(EncryptedData, Unencrypted, key);
		break;
	default: //Everything else is Encrypting
		stdDES(EncryptedData, Unencrypted, key);
		break;
	}
}

static void DES_ede3_cbc_encrypt(byte * Unencrypted, byte * EncryptedData, byte * key, byte * IV, int Encr_Decr){
	switch (Encr_Decr) {
	case 0: //Decrypting
		stdDED(EncryptedData, Unencrypted, key);
//		stdDED(EncryptedData+8, Unencrypted+8, key);
		break;
	default: //Everything else is Encrypting
		stdEDE(EncryptedData, Unencrypted, key);
//		stdEDE(EncryptedData+8, Unencrypted+8, key);
		break;
	}
}

//-------------------------------------------------------------------------------------------------------

/**
 * Check if array have at least one 1
 *
 * @param in In array
 * @param len Length
 *
 * @return TRUE Have 1, FALSE All zeros
 */
static Boolean HaveOnes(  byte *  in, int len	) {
	int i = 0;

	for (i = 0; i < len; i++) {
		if( in[i] != 0 ) {
			return TRUE;
		}
	}

	return FALSE;
}

//-------------------------------------------------------------------------------------------------------

/**
 * Generate temp key from KSN with current counter and IPEK
 *
 * @param ipek Generated key
 * @param ksn10 KSN
 * @param ipek16 IPEK
 */
static void DeriveKeyFromIpek( byte *  ipek_drvd16, byte *  ksn10, byte *  ipek16 ) {
	memcpy( ipek_drvd16, ipek16, 16 );

	byte counter[8];
	memcpy( counter, &ksn10[2], 8 );

	counter[0] = 0x00;
	counter[1] = 0x00;
	counter[2] = 0x00;
	counter[3] = 0x00;
	counter[4] = 0x00;
	counter[5] &= 0x1F;
	counter[6] &= 0xFF;
	counter[7] &= 0xFF;

	byte serial[8];
	memcpy( serial, &ksn10[2], 8 );

	serial[5] &= 0xE0;
	serial[6] &= 0x00;
	serial[7] &= 0x00;

	byte shiftr[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00};

	byte crypto_register_1[8];
	memcpy(crypto_register_1, serial, 8);

	unsigned long long intShiftr = 16*65536; // in fact it's shiftr as int

	while( intShiftr > 0 ) {
		byte temp[8];

		And( temp, shiftr, counter, 8 );

		if( HaveOnes(temp, 8) ) {
			Or(serial, serial, shiftr, 8);

			byte crypto_register_2[8];
			Xor(crypto_register_2, serial, &ipek_drvd16[8], 8 );

			{
				byte crypto_register_2_temp[8];

				DES_ecb_encrypt(crypto_register_2, crypto_register_2_temp, ipek_drvd16, DES_ENCRYPT);

				memcpy(crypto_register_2, crypto_register_2_temp, 8);
			}

			Xor( crypto_register_2, crypto_register_2, &ipek_drvd16[8], 8);

			Xor( ipek_drvd16, ipek_drvd16, BDK_MASK, 16 );

			Xor( crypto_register_1, serial, &ipek_drvd16[8], 8 );

			{
				byte crypto_register_1_temp[8];

				DES_ecb_encrypt((byte *) crypto_register_1, (byte *) crypto_register_1_temp, ipek_drvd16, DES_ENCRYPT);

				memcpy(crypto_register_1, crypto_register_1_temp, 8);
			}

			Xor( crypto_register_1, crypto_register_1, &ipek_drvd16[8], 8 );

			memcpy( &ipek_drvd16[0], crypto_register_1, 8 );
			memcpy( &ipek_drvd16[8], crypto_register_2, 8 );
		}

		intShiftr >>= 1;

		shiftr[7] = (intShiftr >> 0) & 0xFF;
		shiftr[6] = (intShiftr >> 8) & 0xFF;
		shiftr[5] = (intShiftr >> 16) & 0xFF;
		shiftr[4] = (intShiftr >> 24) & 0xFF;
		shiftr[3] = (intShiftr >> 32) & 0xFF;
		shiftr[2] = (intShiftr >> 40) & 0xFF;
		shiftr[1] = (intShiftr >> 48) & 0xFF;
		shiftr[0] = (intShiftr >> 56) & 0xFF;
	}
}

//-------------------------------------------------------------------------------------------------------

/**
 * Generate temp key from KSN with current counter and BDK
 *
 * @param derivedKey Generated key
 * @param ksn10 KSN
 * @param bdk16 BDK
 */
static void DeriveKeyFromBdk(byte *  key_drvd16, byte *  ksn10, byte *  bdk16) {
	byte ipek[16];

	DukptGenerateIpek(ipek, bdk16, ksn10);

	DeriveKeyFromIpek(key_drvd16, ksn10, ipek);
}

//-------------------------------------------------------------------------------------------------------

static  byte data_encryption_variant_constant_both_ways[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00};

/**
 * Generate key for data encryption from derived key
 *
 * @param dec16 Data encryption key
 * @param key16 Derived key
 */
static void CalculateDataEncryptionKey(byte *  dek16,  byte *  key16) {
	byte variant_key_L_R[16];

	byte variant_key_L[8];
	Xor(variant_key_L, &key16[0], data_encryption_variant_constant_both_ways, 8);

	memcpy(&variant_key_L_R[0], variant_key_L, 8);

	byte variant_key_R[8];
	Xor(variant_key_R, &key16[8], data_encryption_variant_constant_both_ways, 8);

	memcpy(&variant_key_L_R[8], variant_key_R, 8);

	{
		byte encryption_key_L[24];

		DES_ede3_cbc_encrypt( variant_key_L, encryption_key_L, variant_key_L_R, NULL, DES_ENCRYPT);

		memcpy(&dek16[0], encryption_key_L, 8);
	}

	{
		byte encryption_key_R[24];

		DES_ede3_cbc_encrypt(variant_key_R, encryption_key_R, variant_key_L_R, NULL, DES_ENCRYPT);

		memcpy(&dek16[8], encryption_key_R, 8);
	}
}

//-------------------------------------------------------------------------------------------------------

void DukptIncrementKsn(void) {
	byte ksn10[32 + 1];
	byte counter[3];
	char KSN_Value[32 + 1];
	int ret = 0;

	memset(ksn10, 0, sizeof(ksn10));
	memset(KSN_Value, 0, sizeof(KSN_Value));

	PAR_PADDED_KSN_Get(ksn10);

	counter[0] = 0x1F & ksn10[7];
	counter[1] = ksn10[8];
	counter[2] = ksn10[9];

	DoubleWord val = counter[0] * 256 * 256 + counter[1] * 256 + counter[2];

	++val;

	counter[0] = (val >> 16) & 0x1F;
	counter[1] = (val >> 8) & 0xFF;
	counter[2] = (val >> 0) & 0xFF;

	ksn10[7] &= 0xE0;

	ksn10[7] |= counter[0];
	ksn10[8] = counter[1];
	ksn10[9] = counter[2];

	bin2hex(KSN_Value, ksn10, 10);

	MAPPUTSTR(appDUKPT_KSN, &KSN_Value[4], lblKO);

	lblKO:;
}

//-------------------------------------------------------------------------------------------------------

void DukptEncryptDataIpek( DukptData_t *  out_data, DukptData_t *  in_data, byte *  ksn10, byte *  ipek16 ) {
	byte incoming[1024 + 1];

	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}
	char TPK[32 + 1];
	byte derivedKey[16];

	memset(TPK, 0, sizeof(TPK));
	memset(incoming, 0, sizeof(incoming));
	memset(out_data->Data, 0, out_data->Len + 1);

	memcpy(incoming, in_data->Data, in_data->Len);

	DeriveKeyFromIpek(derivedKey, ksn10, ipek16);

	byte pinKey[16];

	CalculateDataEncryptionKey(pinKey, derivedKey);

	DES_ede3_cbc_encrypt(incoming, out_data->Data, pinKey, NULL, DES_ENCRYPT);

	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;

	out_data->Data[out_data->Len] = 0;
	out_data->Data[out_data->Len + 1] = 0;

	bin2hex(TPK, pinKey, 16);
	mapPut(appKeyPart, &TPK[strlen(TPK)-4], 4);
}

//-------------------------------------------------------------------------------------------------------

void DukptDecryptDataBdk( DukptData_t *  out_data, DukptData_t *  in_data, byte *  ksn10, byte *  bdk16 ) {

	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}

	byte derivedKey[16];

	DeriveKeyFromBdk(derivedKey, ksn10, bdk16);

	byte pinKey[16];

	CalculateDataEncryptionKey(pinKey, derivedKey);

	DES_ede3_cbc_encrypt( in_data->Data,  out_data->Data, pinKey, NULL, DES_DECRYPT);

	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;
}

//-------------------------------------------------------------------------------------------------------
//
//#if defined(DUKPT_USEVERTED)

void DukptEncryptDataBdk( DukptData_t *  out_data, DukptData_t *  in_data, byte *  ksn10, byte *  bdk16 ) {

	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}
	char TPK[32 + 1];
	byte derivedKey[16];

	memset(TPK, 0, sizeof(TPK));

	DeriveKeyFromBdk(derivedKey, ksn10, bdk16);

	byte pinKey[16];

	CalculateDataEncryptionKey(pinKey, derivedKey);

	DES_ede3_cbc_encrypt((unsigned char*) in_data->Data, (unsigned char*) out_data->Data, pinKey, NULL, DES_ENCRYPT);

	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;

	bin2hex(TPK, pinKey, 16);
	mapPut(appKeyPart, &TPK[strlen(TPK)-4], 4);
}

//-------------------------------------------------------------------------------------------------------

void DukptDecryptDataIpek( DukptData_t *  out_data, //
		DukptData_t *  in_data, //
		byte *  ksn10,
		byte *  ipek16 ) {


	if( in_data->Len == 0 ) {
		out_data->Len = 0;
		return;
	}

	byte derivedKey[16];

	DeriveKeyFromIpek(derivedKey, ksn10, ipek16);

	byte pinKey[16];

	CalculateDataEncryptionKey(pinKey, derivedKey);

	DES_ede3_cbc_encrypt((unsigned char*) in_data->Data, (unsigned char*) out_data->Data, pinKey, NULL, DES_DECRYPT);

	out_data->Len = ((in_data->Len-1) / 8) * 8 + 8;
}
//
//#endif // #if defined(DUKPT_USEVERTED)

//-------------------------------------------------------------------------------------------------------

void DukptGenerateIpek( byte *  ipek16, byte *  bdk16, byte *  ksn10 ) {
	int i = 0;
	int ret = 0;
	char IPEK[32 + 1];

	memset(IPEK, 0, sizeof(IPEK));

	byte partKsn[8];

	memcpy( partKsn, ksn10, 8);

	partKsn[7] &= 0xE0;

	{
		byte leftIpek[24];

		DES_ede3_cbc_encrypt( (unsigned char*)partKsn, (unsigned char*)leftIpek, bdk16, NULL, DES_ENCRYPT);

		memcpy( &ipek16[0], leftIpek, 8 );
	}

	byte derivedBdk[16];

	for (i = 0; i < 16; i++) {
		derivedBdk[i] = bdk16[i] ^ BDK_MASK[i];
	}

	{
		byte rightIpek[24];

		DES_ede3_cbc_encrypt( (unsigned char*)partKsn, (unsigned char*)rightIpek, derivedBdk, NULL, DES_ENCRYPT);

		memcpy( &ipek16[8], rightIpek, 8 );
	}

	bin2hex(IPEK, ipek16, 16);
	MAPPUTSTR(appDUKPT_IPEK, IPEK, lblKO);

	lblKO:;
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
#if defined(DUKPT_TEST)
//-------------------------------------------------------------------------------------------------------

static void testGenerateIpek(void) {
	SLOG(LL_VERBOSE, SLOG_LIVE, "testGenerateIpek");

	byte ipek[16];
	byte bdk[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
	byte ksn[10] = {0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0x00};

	DukptGenerateIpek( ipek, bdk, ksn );

	SLOG(LL_VERBOSE, SLOG_LIVE, "testGenerateIpek: ipek=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
			ipek[0], ipek[1], ipek[2], ipek[3], ipek[4], ipek[5], ipek[6], ipek[7], //
			ipek[8], ipek[9], ipek[10], ipek[11], ipek[12], ipek[13], ipek[14], ipek[15]); //

	{
		byte test[16] = {0x6A, 0xC2, 0x92, 0xFA, 0xA1, 0x31, 0x5B, 0x4D, 0x85, 0x8A, 0xB3, 0xA3, 0xD7, 0xD5, 0x93, 0x3A};

		assert(memcmp(ipek, test, 16) == 0);
	}
}

//-------------------------------------------------------------------------------------------------------

// Just to exchange tests with Java code
#define byte				byte

static void testDeriveKeyFromIpek(void) {
	SLOG(LL_VERBOSE, SLOG_LIVE, "testDeriveKeyFromIpek");

	{
		byte ipek[16] = {0x6A, 0xC2, 0x92, 0xFA, 0xA1, 0x31, 0x5B, 0x4D, 0x85, 0x8A, 0xB3, 0xA3, 0xD7, 0xD5, 0x93, 0x3A};
		byte ksn[10] = {0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0x01};
		byte ipek_der[16];

		DeriveKeyFromIpek( ipek_der, ksn, ipek );

		byte test[16] = {0x04, 0x26, 0x66, 0xB4, 0x91, 0x84, 0xCF, 0xA3, 0x68, 0xDE, 0x96, 0x28, 0xD0, 0x39, 0x7B, 0xC9};

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDeriveKeyFromIpek: ipek_der=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				ipek_der[0], ipek_der[1], ipek_der[2], ipek_der[3], ipek_der[4], ipek_der[5], ipek_der[6], ipek_der[7], //
				ipek_der[8], ipek_der[9], ipek_der[10], ipek_der[11], ipek_der[12], ipek_der[13], ipek_der[14], ipek_der[15]); //

		assert(memcmp(ipek_der, test, 16) == 0);
	}

	{
		byte ipek[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (byte)0x88,
				(byte)0x99, (byte)0xAA, (byte)0xBB, (byte)0xCC, (byte)0xDD, (byte)0xEE, (byte)0xFF, 0x00};
		byte ksn[10] = {(byte)0xFF, (byte)0xFF, (byte)0x98, 0x76, 0x54, 0x32, 0x10, (byte)0xE0, 0x00, 0x03};
		byte ipek_der[16];

		DeriveKeyFromIpek( ipek_der, ksn, ipek );

		byte test[16] = {0x59, 0xDC, 0x7A, 0x36, 0x14, 0x84, 0x2E, 0x01, 0xD0, 0x0A, 0x28, 0x26, 0x6F, 0xAD, 0xE8, 0x5D};

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDeriveKeyFromIpek: ipek_der=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				ipek_der[0], ipek_der[1], ipek_der[2], ipek_der[3], ipek_der[4], ipek_der[5], ipek_der[6], ipek_der[7], //
				ipek_der[8], ipek_der[9], ipek_der[10], ipek_der[11], ipek_der[12], ipek_der[13], ipek_der[14], ipek_der[15]); //

		assert(memcmp(ipek_der, test, 16) == 0);
	}

	{
		byte ipek[16] = {(byte)0x99, (byte)0xAA, (byte)0xBB, (byte)0xCC, (byte)0xDD, (byte)0xEE, (byte)0xFF, 0x00,
				0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (byte)0x88};
		byte ksn[10] = {(byte)0xFF, (byte)0xFF, (byte)0x98, 0x76, 0x54, 0x32, 0x10, (byte)0xE0, 0x00, 0x07};
		byte ipek_der[16];

		DeriveKeyFromIpek( ipek_der, ksn, ipek );

		byte test[16] = {0xA7, 0x0E, 0x48, 0xF9, 0x82, 0xD0, 0x26, 0x33, 0x78, 0x08, 0xA0, 0x20, 0xA8, 0xFD, 0x16, 0xEE};

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDeriveKeyFromIpek: ipek_der=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				ipek_der[0], ipek_der[1], ipek_der[2], ipek_der[3], ipek_der[4], ipek_der[5], ipek_der[6], ipek_der[7], //
				ipek_der[8], ipek_der[9], ipek_der[10], ipek_der[11], ipek_der[12], ipek_der[13], ipek_der[14], ipek_der[15]); //

		assert(memcmp(ipek_der, test, 16) == 0);
	}
}

//-------------------------------------------------------------------------------------------------------

static void testCalculateDataEncryptionKey() {
	SLOG(LL_VERBOSE, SLOG_LIVE, "testCalculateDataEncryptionKey");

	{
		byte key[16] = {(byte)0x99, (byte)0xAA, (byte)0xBB, (byte)0xCC, (byte)0xDD, (byte)0xEE, (byte)0xFF, 0x00,
				0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (byte)0x88};
		byte dek[16];

		CalculateDataEncryptionKey(dek, key);

		byte test[16] = { 0xA6, 0xEF, 0xA5, 0x1F, 0x9B, 0xB9, 0x26, 0xBF, 0xE5, 0x39, 0x53, 0x6B, 0xBA, 0x89, 0x6F, 0x97 };

		SLOG(LL_VERBOSE, SLOG_LIVE, "testCalculateDataEncryptionKey: dek=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				dek[0], dek[1], dek[2], dek[3], dek[4], dek[5], dek[6], dek[7], //
				dek[8], dek[9], dek[10], dek[11], dek[12], dek[13], dek[14], dek[15]); //

		assert(memcmp(dek, test, 16) == 0);
	}

	{
		byte key[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, (byte)0x88,
				(byte)0x99, (byte)0xAA, (byte)0xBB, (byte)0xCC, (byte)0xDD, (byte)0xEE, (byte)0xFF, 0x00};
		byte dek[16];

		CalculateDataEncryptionKey(dek, key);

		byte test[16] = { 0x88, 0xC1, 0x68, 0x7C, 0x09, 0x1B, 0x1A, 0x13, 0x5D, 0x67, 0xC4, 0x96, 0x6B, 0x7F, 0xB9, 0x4E };

		SLOG(LL_VERBOSE, SLOG_LIVE, "testCalculateDataEncryptionKey: dek=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				dek[0], dek[1], dek[2], dek[3], dek[4], dek[5], dek[6], dek[7], //
				dek[8], dek[9], dek[10], dek[11], dek[12], dek[13], dek[14], dek[15]); //

		assert(memcmp(dek, test, 16) == 0);
	}
}

//-------------------------------------------------------------------------------------------------------

static void testDukptEncryptDecrypt() {
	SLOG(LL_VERBOSE, SLOG_LIVE, "testDukptEncryptDecrypt");

	byte bdk[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };

	{
		byte ipek[16] = {0x6A, (byte)0xC2, (byte)0x92, (byte)0xFA, (byte)0xA1, 0x31, 0x5B, 0x4D,
				(byte)0x85, (byte)0x8A, (byte)0xB3, (byte)0xA3, (byte)0xD7, (byte)0xD5, (byte)0x93, 0x3A};
		byte ksn[10] = {(byte) 0xFF, (byte) 0xFF, (byte) 0x98, 0x76, 0x54, 0x32, 0x10, (byte) 0xE0, 0x00, 0x07};

		byte buf_dec[] = { 't', 'e', 's', 't', '!', 0x00, 0x00, 0x00 };
		DukptData_t data_dec = {
				.Data = buf_dec,
				.Len = sizeof(buf_dec),
		};

		byte buf_enc[16];
		DukptData_t data_enc = {
				.Data = buf_enc,
				.Len = 16,
		};

		DukptEncryptDataIpek( &data_enc, &data_dec, ksn, ipek );

		byte test[16] = { 0xFA, 0xD9, 0x33, 0xE2, 0x74, 0x32, 0x8A, 0xA0, 0x00 };

		assert(data_enc.Len == 8);

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDukptEncryptDecrypt: buf_enc=%02X%02X%02X%02X%02X%02X%02X%02X", //
				buf_enc[0], buf_enc[1], buf_enc[2], buf_enc[3], buf_enc[4], buf_enc[5], buf_enc[6], buf_enc[7]); //

		assert(memcmp(data_enc.Data, test, 8) == 0);

		memset(data_dec.Data, 0x00, 8);

		DukptDecryptDataBdk( &data_dec, &data_enc, ksn, bdk );

		assert(data_dec.Len == 8);

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDukptEncryptDecrypt: buf_dec=%c%c%c%c%c%c%c%c", //
				buf_dec[0], buf_dec[1], buf_dec[2], buf_dec[3], buf_dec[4], buf_dec[5], buf_dec[6], buf_dec[7]); //

		assert(memcmp(data_dec.Data, "test!\0\0\0", 8) == 0);
	}

	{
		byte ipek[16] = {0x6A, (byte)0xC2, (byte)0x92, (byte)0xFA, (byte)0xA1, 0x31, 0x5B, 0x4D,
				(byte)0x85, (byte)0x8A, (byte)0xB3, (byte)0xA3, (byte)0xD7, (byte)0xD5, (byte)0x93, 0x3A};
		byte ksn[10] = {(byte) 0xFF, (byte) 0xFF, (byte) 0x98, 0x76, 0x54, 0x32, 0x10, (byte) 0xE0, 0x00, 0x08};

		byte buf_dec[] = { 't', 'e', 's', 't', '!', 't', 'e', 's', 't', '?', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
		DukptData_t data_dec = {
				.Data = buf_dec,
				.Len = 16,
		};

		byte buf_enc[32];
		DukptData_t data_enc = {
				.Data = buf_enc,
				.Len = 32,
		};

		DukptEncryptDataIpek( &data_enc, &data_dec, ksn, ipek );

		byte test[16] = { 0x14, 0x88, 0x06, 0xC5, 0xCD, 0xFF, 0x7C, 0xA7, 0x56, 0x3E, 0x6F, 0xDF, 0xC6, 0x1F, 0x76, 0xC9 };

		assert(data_enc.Len == 16);

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDukptEncryptDecrypt: buf_enc=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				buf_enc[0], buf_enc[1], buf_enc[2], buf_enc[3], buf_enc[4], buf_enc[5], buf_enc[6], buf_enc[7],
				buf_enc[8], buf_enc[9], buf_enc[10], buf_enc[11], buf_enc[12], buf_enc[13], buf_enc[14], buf_enc[15]); //

		assert(memcmp(data_enc.Data, test, 16) == 0);

		memset(data_dec.Data, 0x00, 16);

		DukptDecryptDataBdk(&data_dec, &data_enc, ksn, bdk);

		assert(data_dec.Len == 16);

		SLOG(LL_VERBOSE, SLOG_LIVE, "testDukptEncryptDecrypt: buf_dec=%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", //
				buf_dec[0], buf_dec[1], buf_dec[2], buf_dec[3], buf_dec[4], buf_dec[5], buf_dec[6], buf_dec[7],
				buf_dec[8], buf_dec[9], buf_dec[10], buf_dec[11], buf_dec[12], buf_dec[13], buf_dec[14], buf_dec[15]); //

		assert(memcmp(data_dec.Data, "test!test?\0\0\0\0\0\0", 8) == 0);
	}
}

//-------------------------------------------------------------------------------------------------------

static void testDukptIncrementKsn() {
	SLOG(LL_VERBOSE, SLOG_LIVE, "testDukptIncrementKsn");

	{
		byte ksn[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0x01};
		byte test[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0x02};

		DukptIncrementKsn(ksn);

		assert(memcmp(ksn, test, 10) == 0);
	}

	{
		byte ksn[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0xFF};
		byte test[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x01, 0x00};

		DukptIncrementKsn(ksn);

		assert(memcmp(ksn, test, 10) == 0);
	}

	{
		byte ksn[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0xFF, 0xFF};
		byte test[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE1, 0x00, 0x00};

		DukptIncrementKsn(ksn);

		assert(memcmp(ksn, test, 10) == 0);
	}

	{
		byte ksn[10] =			{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xF0, 0x00, 0x00};
		byte test[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xF0, 0x00, 0x01};

		DukptIncrementKsn(ksn);

		assert(memcmp(ksn, test, 10) == 0);
	}

	{
		byte ksn[10] =			{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xFF, 0xFF, 0xFF};
		byte test[10] =		{0xFF, 0xFF, 0x98, 0x76, 0x54, 0x32, 0x10, 0xE0, 0x00, 0x00};

		DukptIncrementKsn(ksn);

		assert(memcmp(ksn, test, 10) == 0);
	}
}

//-------------------------------------------------------------------------------------------------------

#if defined(DUKPT_USEVERTED)

static void testInverted() {
	SLOG(LL_VERBOSE, SLOG_LIVE, "testInverted");

	byte bdk[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 };

	byte ipek[16] = {0x6A, (byte)0xC2, (byte)0x92, (byte)0xFA, (byte)0xA1, 0x31, 0x5B, 0x4D,
			(byte)0x85, (byte)0x8A, (byte)0xB3, (byte)0xA3, (byte)0xD7, (byte)0xD5, (byte)0x93, 0x3A};

	{
		byte ksn[10] = {(byte) 0xFF, (byte) 0xFF, (byte) 0x98, 0x76, 0x54, 0x32, 0x10, (byte) 0xE0, 0x00, 0x08};

		byte buf_dec[] = { 't', 'e', 's', 't', '!', 't', 'e', 's', 't', '?', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
		DukptData_t data_dec = {
				.Data = buf_dec,
				.Len = 16,
		};

		byte buf_enc[32];
		DukptData_t data_enc = {
				.Data = buf_enc,
				.Len = 32,
		};

		DukptEncryptDataBdk( &data_enc, &data_dec, ksn, bdk );

		byte test[16] = { 0x14, 0x88, 0x06, 0xC5, 0xCD, 0xFF, 0x7C, 0xA7, 0x56, 0x3E, 0x6F, 0xDF, 0xC6, 0x1F, 0x76, 0xC9 };

		assert(data_enc.Len == 16);

		SLOG(LL_VERBOSE, SLOG_LIVE, "testInverted: buf_enc=%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", //
				buf_enc[0], buf_enc[1], buf_enc[2], buf_enc[3], buf_enc[4], buf_enc[5], buf_enc[6], buf_enc[7],
				buf_enc[8], buf_enc[9], buf_enc[10], buf_enc[11], buf_enc[12], buf_enc[13], buf_enc[14], buf_enc[15]); //

		assert(memcmp(data_enc.Data, test, 16) == 0);

		memset(data_dec.Data, 0x00, 16);

		DukptDecryptDataIpek(&data_dec, &data_enc, ksn, ipek);

		assert(data_dec.Len == 16);

		SLOG(LL_VERBOSE, SLOG_LIVE, "testInverted: buf_dec=%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", //
				buf_dec[0], buf_dec[1], buf_dec[2], buf_dec[3], buf_dec[4], buf_dec[5], buf_dec[6], buf_dec[7],
				buf_dec[8], buf_dec[9], buf_dec[10], buf_dec[11], buf_dec[12], buf_dec[13], buf_dec[14], buf_dec[15]); //

		assert(memcmp(data_dec.Data, "test!test?\0\0\0\0\0\0", 8) == 0);
	}
}

#endif // #if defined(DUKPT_USEVERTED)

//-------------------------------------------------------------------------------------------------------

void DukptUnittest(void) {
	SLOG(LL_VERBOSE, SLOG_LIVE, "DukptUnittest");

	testGenerateIpek();

	testDeriveKeyFromIpek();

	testCalculateDataEncryptionKey();

	testDukptEncryptDecrypt();

	testDukptIncrementKsn();

#if defined(DUKPT_USEVERTED)
	testInverted();
#endif // #if defined(DUKPT_USEVERTED)

	SLOG(LL_VERBOSE, SLOG_LIVE, "DukptUnittest: OK");
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
#endif // #if defined(DUKPT_TEST)
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
