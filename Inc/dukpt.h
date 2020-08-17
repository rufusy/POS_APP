//-------------------------------------------------------------------------------------------------------
// 2017 (c) Ryabinin sla.000@yandex.ru
//-------------------------------------------------------------------------------------------------------
#pragma once

#include "globals.h"
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
#define DUKPT_VER_HI            1
#define DUKPT_VER_LO            0
#define DUKPT_VER_FIX           0
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

/// Data blob
typedef struct {
    /**
     * Pointer to data
     */
    byte * Data;
    /**
     * Max data length when input, data overall length when output
     */
    card Len;
} DukptData_t;

//-------------------------------------------------------------------------------------------------------

/**
 * Increment KSN before new transaction
 *
 * Design of KSN provides 21 less significant bits for incrementing counter
 *
 * @param ksn10 KSN to change
 */
extern void DukptIncrementKsn(void);

//-------------------------------------------------------------------------------------------------------

/**
 * Generate IPEK from BDK and KSN
 *
 * BDK is super-secret key on server. If stolen, all transactions are compromised.
 * IPEK is secret key on client. If stolen, all transactions with this client are compromised.
 * KSN is a serial number of device, usually 2 first bytes is FFFF, then 43 bits of unique device serial, other bits are ignored.
 *
 * @param ipek16 IPEK, 16 bytes
 * @param bdk16 BDK, 16 bytes
 * @param ksn10 KSN, 10 bytes
 */
extern void DukptGenerateIpek(  byte *  ipek16, //
		 byte *  bdk16, //
		 byte *  ksn10 );

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

/**
 * Encrypt data with IPEK (on client)
 *
 * @param out_data Encrypted data
 * @param in_data Data to encrypt
 * @param ksn10 KSN, 10 bytes
 * @param ipek16 IPEK, 16 bytes
 */
extern void DukptEncryptDataIpek(  DukptData_t *  out_data, //
		 DukptData_t *  in_data, //
		 byte *  ksn10,
		 byte *  ipek16 );

//-------------------------------------------------------------------------------------------------------

/**
 * Decrypt data with BDK (on server)
 *
 * @param out_data Decrypted data
 * @param in_data Data to decrypt
 * @param ksn10 KSN, 10 bytes
 * @param ipek16 IPEK, 16 bytes
 */
extern void DukptDecryptDataBdk(  DukptData_t *  out_data, //
		 DukptData_t *  in_data, //
		 byte *  ksn10,
		 byte *  bdk16 );

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//
//#if defined(DUKPT_USE_INVERTED)

/**
 * Encrypt data with BDK (on server)
 *
 * @param out_data Encrypted data
 * @param in_data Data to encrypt
 * @param ksn10 KSN, 10 bytes
 * @param bdk16 BDK, 16 bytes
 * @return
 */
extern void DukptEncryptDataBdk(  DukptData_t *  out_data, //
		 DukptData_t *  in_data, //
		 byte *  ksn10,
		 byte *  bdk16 );

//-------------------------------------------------------------------------------------------------------

/**
 * Decrypt data with IPEK (on client)
 *
 * @param out_data Decrypted data
 * @param in_data Data to decrypt
 * @param ksn10 KSN, 10 bytes
 * @param ipek16 IPEK, 16 bytes
 */
extern void DukptDecryptDataIpek(  DukptData_t *  out_data, //
		 DukptData_t *  in_data, //
		 byte *  ksn10,
		 byte *  ipek16 );
//
//#endif // defined(DUKPT_USE_INVERTED)

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

#if defined(DUKPT_TEST)

/// Unit test
extern void DukptUnittest(void);

#endif // defined(DUKPT_TEST)

//-------------------------------------------------------------------------------------------------------
