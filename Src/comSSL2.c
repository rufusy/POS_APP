/*!
 ------------------------------------------------------------------------------
                  INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2012, Ingenico.
 28-32 boulevard de Grenelle 75015 Paris, France.
 All rights reserved.

  This source program is the property of INGENICO Company and may not be copied
  in any form or by any means, whether in part or in whole, except under license
  expressly granted by INGENICO company

  All copies of this program, whether in part or in whole, and
  whether modified or not, must display this and all other
  embedded copyright and ownership notices in full.
 ------------------------------------------------------------------------------

 Project : SSL Sample
 Module  : DFU - Demo Features Unit

 @file      FUN_ssl.c
 @brief     Contains all definitions of functions utilized for SSL.
 @date      03/10/2012

 ------------------------------------------------------------------------------
 */

#ifdef __cplusplus
extern "C" {
#endif


#include "sdk.h"
#include "SSL_.h"
#include "X509_.h"

#include "utilities.h"
#include "err_process.h"
#include "ui_userinterface.h"
#include "par.h"
#include "flow.h"

#define _FUN_SSL_TIMEOUT_20_SECONDS             2000 /**< Time out used to connect to SSL server*/

/** Structure used to declare an SSL communication handle.
 */
typedef struct
{
    SSL_HANDLE hdl;/**< SSL handle to be initialized using SSL_New()*/
    SSL_PROFILE_HANDLE profile; /**< SSL profile to be initialized using SSL_LoadProfile()*/
}_FUN_Ssl_ComsChannel_t;

static void _FUN_SslProfilePrint( SSL_PROFILE_HANDLE hProfile );
static int _FUN_SslStart(const char *profile);
static void _FUN_SslStop(void);
static int _FUN_SslConnect(const char *host, unsigned int port);
static void _FUN_SslDisconnect(void);
static int _FUN_SslProtocolGet(void);

_FUN_Ssl_ComsChannel_t _FUN_Ssl_comsChannel; /**< SSL Connection test handle */

/** Prints the profile details including the last SSL server to connect. The very last error
 * encountered using the specified profile is also printed.
 *
 * \param[in] hProfile  The SSL profile handle initialized using SSL_LoadProfile() or obtained
 *                      using SSL_GetProfile().
 */
static void _FUN_SslProfilePrint( SSL_PROFILE_HANDLE hProfile )
{
    int iError;
    int iPort;
    int i;
    int protocol;
    X509_HANDLE hX509;

    char szTmp[80];
    int szTmpSize = 80;
    char szBuffer[100];

    prtStart();
    if( hProfile == NULL ) {
        return;
    }

    UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_PRINTING_PROFILE);

    /* Retrieve the name of the specified profile and place it on the trace console.*/
    if( SSL_ProfileGetName( hProfile, szTmp ) == 0 )
    {
        printText(szTmp);
    }
    /* Retrieves the last error encountered during an SSL connection.*/
    if(SSL_ProfileGetProtocol( hProfile, &protocol ) == 0 )
    {
        switch (protocol)
        {
            case SSLv2:
                printText("Protocol: SSL V2");
                break;
            case SSLv3:
                printText("Protocol: SSL V3");
                break;
            case TLSv1:
                printText("Protocol: TLS V1");
                break;
            case SSLv23:
                printText("Protocol: SSl V23");
                break;
            case TLSv1_1:
                printText("Protocol: TLS V1.1");
                break;
            case TLSv1_2:
                printText("Protocol: TLS V1.2");
                break;
            default:
                break;
        }
    }

    /* Retrieves the last error encountered during an SSL connection.*/
    if( SSL_ProfileGetLastIpServer( hProfile, szTmp, &iPort ) == 0 )
    {
        if(strcmp(szTmp,"")!=0)
        {
            printText("Last connection :");
            printText(szTmp);

            if( SSL_ProfileGetLastError( hProfile, &iError ) == 0 )
            {
                switch( iError )
                {
                    case SSL_PROFILE_OK:
                        Telium_Sprintf( szBuffer, "Connection OK\n" );
                        break;
                    case SSL_PROFILE_TCP_CONNECT_TIMEOUT:
                        Telium_Sprintf( szBuffer, "TCP connection timeout\n" );
                        break;
                    case SSL_PROFILE_TCP_CONNECT_FAILED:
                        Telium_Sprintf( szBuffer, "TCP connection failed\n"  );
                        break;
                    case SSL_PROFILE_TCP_ADDR_NOT_AVAILABLE:
                        Telium_Sprintf( szBuffer, "Address Not available\n" );
                        break;
                    case SSL_PROFILE_SSL_HANDSHAKE_FAILURE:
                        Telium_Sprintf( szBuffer, "Not SSL server\n" );
                        break;
                    case SSL_PROFILE_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
                        Telium_Sprintf( szBuffer, "Unknown server certificate\n" );
                        break;
                    case SSL_PROFILE_ERROR_IN_CERT_NOT_BEFORE_FIELD:
                        Telium_Sprintf( szBuffer, "Server certificate not yet valid\n" );
                        break;
                    case SSL_PROFILE_ERROR_IN_CERT_NOT_AFTER_FIELD:
                        Telium_Sprintf( szBuffer, "Server certificate has expired\n" );
                        break;
                    case SSL_PROFILE_E_SSLV3_ALERT_HANDSHAKE_FAILURE:
                        Telium_Sprintf( szBuffer, "Server refused connection\n" );
                        break;
                    default:
                        Telium_Sprintf( szBuffer, "Error: %d\n", iError );
                }
                printText(szTmp);
            }
        }
    }

    /* Retrieves the details of the client certificate associated with the profile.*/
    if( SSL_ProfileGetCertificateFile( hProfile, szTmp ) == 0 )
    {
        printText("Client Certificate:");

        /* Open the certificate file */
        if(( hX509 = X509_Load( szTmp )) != NULL )
        {
            /* Get beginning validity date of certificate */
            if(X509_GetInfo(hX509,X509_NOTBEFORE,0,szTmp,szTmpSize)>0)
            {
                printText("Not Before :");
                printText(szTmp);
            }
            /* Get end validity date of certificate */
            if(X509_GetInfo(hX509,X509_NOTAFTER,0,szTmp,szTmpSize)>0)
            {
                printText("Not After :");
                printText(szTmp);
            }
            /* Unload the certificate file*/
            X509_Unload( hX509 );
        }
    }

    /* Retrieves the details of all the CA certificate/s associated with the profile.*/
    i = 0;
    while(SSL_ProfileGetCAFile( hProfile, i, szTmp ) == 0)
    {
        Telium_Sprintf( szBuffer, "Certificate Authority :\n");
        printText("Certificate Authority :");

        if(( hX509 = X509_Load( szTmp )) != NULL ) {
            /* Get beginning validity date of certificate */
            if(X509_GetInfo(hX509,X509_NOTBEFORE,0,szTmp,szTmpSize)>0) {
                printText("Not Before :");
                printText(szTmp);
            }
            /* Get end validity date of certificate */
            if(X509_GetInfo(hX509,X509_NOTAFTER,0,szTmp,szTmpSize)>0) {
                printText("Not After :");
                printText(szTmp);
            }
            /* Get issuer information of certificate by NID */
            if(X509_GetInfo(hX509,X509_ISSUER,NID_commonName,szTmp,szTmpSize)>0) {
                printText("Issuer :");
                printText(SN_commonName);
                printText(szTmp);
            }
            /* Unload the certificate file*/
            X509_Unload( hX509 );
        }
        i++;
        printText("\n");
    }

    printDocument();
    prtStop();
}

/** Initializes the SSL communication handles.
 *
 * \param[in] profile Pointer to the name of the SSL profile to be used.
 *
 * \return
 *      -   \ref ERR_RET_OK = Profile has been successfully loaded.
 *      -   \ref ERR_FUN_SSL_PROFILE_NOT_LOADED = Profile has not been loaded.
 *          Check the existence of the profile specified.
 */
static int _FUN_SslStart(const char *profile)
{
    int ret = ERR_RET_OK;

    memset(&_FUN_Ssl_comsChannel, 0, sizeof(_FUN_Ssl_comsChannel));
    /* Search for the profile in the terminal and load it*/
    _FUN_Ssl_comsChannel.profile = SSL_LoadProfile(profile);

    if (_FUN_Ssl_comsChannel.profile==NULL) {
        ret = ERR_FUN_SSL_PROFILE_NOT_LOADED;
    }

    return ret;
}

/** Connects to the SSL server identified by \ref host and \ref port.
 *
 * \param[in] host  Pointer to the name of the host to connect to. This can be the
 *                  DNS name or the IP address(e.g. 192.168.2.1).
 * \param[in] port  The port number where the SSL is listening.
 *
 * \return
 *      - \ref ERR_FUN_SSL_HANDLE_INIT_FAILED = No SSL handle has been initialized.
 *      - \ref ERR_FUN_SSL_HANDLE_CONNECT_FAILED = Connection to SSL server failed.
 */
static int _FUN_SslConnect(const char *host, unsigned int port)
{
    int ret;

    /* Create an SSL Structure */
    ret = SSL_New(&_FUN_Ssl_comsChannel.hdl, _FUN_Ssl_comsChannel.profile);
    if (ret!=0) {
        return ERR_FUN_SSL_HANDLE_INIT_FAILED;
    }

    /* Connect to the server */
    ret = SSL_Connect(_FUN_Ssl_comsChannel.hdl, host, port, _FUN_SSL_TIMEOUT_20_SECONDS );
    if (ret!=0) {
        return ERR_FUN_SSL_HANDLE_CONNECT_FAILED;
    }

    return ret;
}

/** Disconnects to a previously connected SSL server.
 */
static void _FUN_SslDisconnect(void) {
    SSL_Disconnect(_FUN_Ssl_comsChannel.hdl);
}

/** Unloads an SSL profile used to connect to a given SSL server and frees
 * the memory used by SSL.
 */
static void _FUN_SslStop(void) {
    /* Unload the profile and free the ssl handler */
    SSL_UnloadProfile(_FUN_Ssl_comsChannel.profile);
    SSL_Free(_FUN_Ssl_comsChannel.hdl);
}

/** Print and trace  the error from the last connection attempt */
static void _FUN_Ssl_Error(void) {
    int err;

    SSL_ProfileGetLastError(_FUN_Ssl_comsChannel.profile, &err);
    switch (err) {
        case SSL_PROFILE_EOK:
            break;
        case SSL_PROFILE_EEXIST:
            printText("Profile already exist");
            break;
        case SSL_PROFILE_ENEXIST:
            printText("Profile does not exist");
            break;
        case SSL_PROFILE_ETOOMANY:
            printText("Too many profile are loaded");
            break;
        case SSL_PROFILE_ENOMEM:
            printText("Allocation error");
            break;
        case SSL_PROFILE_ELOADED:
            printText("Profile already loaded");
            break;
        case SSL_PROFILE_ENLOADED:
            printText("Profile not loaded");
            break;
        case SSL_PROFILE_ECAEXIST:
            printText("ca already exist");
            break;
        case SSL_PROFILE_ECANEXIST:
            printText("ca does not exist in the profile");
            break;
        case SSL_PROFILE_ECATOOMANY:
            printText("Too many ca in the profile");
            break;
        case SSL_PROFILE_EPARAM:
            printText("A parameter is wrong");
            break;
        case SSL_PROFILE_ENOCERT:
            printText("There is no certificate");
            break;
        default:
            printText("Unknown Error");
            break;
    }

    if (err!=SSL_PROFILE_EOK) {
        printDocument();
    }
}

static int _FUN_SslProtocolGet(void) {
    int ret=TLSv1_2;
    enum eprotocol data;

    ret = PAR_ProtocolGet(&data);
    if (ret!=PAR_OK) {
        return ret;
    }

    switch (data) {
        case protocol_TLSv1_1:
            return TLSv1_1;
        default:
            return TLSv1_2;
    }

    return TLSv1_2;
}

void FUN_SslProfileListPrint(void)
{
    SSL_PROFILE_HANDLE hProfile;
    int i = 0;
    int count;

    /* Obtain the number of profiles loaded in the terminal */
    count = SSL_GetProfileCount();
    if(count == 0) {
        UI_ErrMsgDisplay(ERR_FUN_SSL_NO_PROFILE_FOUND);
        return;
    }

   /* For each profile, print the certificate details */
    while(( hProfile = SSL_GetProfile( i++ )) != NULL ) {
        _FUN_SslProfilePrint( hProfile );

        SSL_UnloadProfile( hProfile );
    }
}

char **FUN_SslProfileListConstruct( int *pnNbItems ) {
    int nNbProfiles = 0;
    char **pItems = NULL;
    char *szName = NULL;

    SSL_PROFILE_HANDLE hProfile;
    int i = 0;

    /* Obtain the number of profiles loaded in the terminal */
    nNbProfiles = SSL_GetProfileCount();
    if( nNbProfiles != 0 ) {
        /* Allocate a memory space big enough for to create a menu containing all of the profiles as menu item*/
        pItems = (char **) umalloc( sizeof(const char*) * (nNbProfiles +1));

        /* Fr each profile in the terminal, add it to the menu char buffer*/
        while(( hProfile = SSL_GetProfile( i )) != NULL ) {
            szName = (char *) umalloc( PROFILE_NAME_SIZE );

            SSL_ProfileGetName( hProfile, szName );
            pItems[i] = szName;

            SSL_UnloadProfile( hProfile );
            i++;
        }
        pItems[i]=0;
    }
    *pnNbItems = nNbProfiles;
    /* Return the created menu parameter to be displayed for profile selection*/
    return pItems;
}


void FUN_SslProfileAdd(void) {
    int ret;
    int idx=0;
    int protocol;
    int err;
    char szName[80];
    object_info_t info;

    SSL_PROFILE_HANDLE hProfile = NULL;

    UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_CREATING_PROFILE);

    ObjectGetInfo(OBJECT_TYPE_APPLI, ApplicationGetCurrent(), &info);
    err = SSL_PROFILE_EEXIST;
    /* Search for the profile in the terminal if it already exists,
     * If it already exist increment the profile id and try to create it again */
    while (err==SSL_PROFILE_EEXIST) {
        Telium_Sprintf(szName, "%04X_PF%02d", info.application_type, idx++);
        hProfile = SSL_NewProfile( szName, &err );
    }

    if( hProfile != NULL ) {
       FLOW_ParamConfigProtocolProcess();
        ret = FLOW_ParamConfigProtocolStatus();
        if (ret==0) {
            SSL_UnloadProfile( hProfile );
            SSL_DeleteProfile(szName);
            return;
        }
       protocol = _FUN_SslProtocolGet();
        SSL_ProfileSetProtocol(hProfile, protocol);
        SSL_ProfileSetCipher( hProfile,
                              SSL_RSA | SSL_DES | SSL_3DES | SSL_RC4 | SSL_RC2 |
                              SSL_MD5 | SSL_SHA1 , SSL_HIGH | SSL_NOT_EXP);

        /* The created profile will use the following certificates in the terminal */
        SSL_ProfileSetKeyFile( hProfile, "/HOST/CLIENT_KEY.PEM", FALSE );
        SSL_ProfileSetCertificateFile( hProfile, "/HOST/CLIENT.CRT" );
        SSL_ProfileAddCertificateCA( hProfile, "/HOST/CA.CRT" );

        /* Save the profile.*/
        SSL_SaveProfile( hProfile );

        /* Tests whether the files linked to the profile are loaded into the terminal.*/
        {
            SSL_HANDLE sslHdl;
            err = SSL_New(&sslHdl, hProfile);
            if(err != 0) {
                UI_ErrMsgDisplay(ERR_FUN_SSL_CERTS_KEYS_NOT_LOADED);
                UI_ErrMsgDisplay(ERR_FUN_SSL_PROFILE_CREATION_FAILED);
                SSL_UnloadProfile( hProfile );
                SSL_DeleteProfile(szName);
                return;
            }
            SSL_Free(sslHdl);
        }

        SSL_UnloadProfile( hProfile );

        UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_PROFILE_CREATION_OK);
    }
    else
    {
        UI_ErrMsgDisplay(ERR_FUN_SSL_PROFILE_CREATION_FAILED);
        _FUN_Ssl_Error();
    }
}


int FUN_SslProfileDelete(char *profileName)
{
    int ret;

    /* Search for the profile in the terminal and delete it*/
    ret = SSL_DeleteProfile(profileName);
    if(ret != 0)
    {
        ret = ERR_RET_OK;
    }
    else
    {
        ret = ERR_FUN_SSL_PROFILE_NOT_FOUND;
    }

    return ret;
}

int FUN_SslConnectionTest( const char *szHostName, unsigned int nPort, const char *szProfile )
{
    int ret = ERR_RET_OK;

    UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_LOADING_SSL_PROFILE);
    ret = _FUN_SslStart(szProfile);
    if(ret>=0)
    {
        UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_CONNECTING_TO_SERVER);
        ret = _FUN_SslConnect(szHostName, nPort);
        if (ret==0)
        {
            UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_CONNECTION_OK);

            UI_PromptMsgDisplay(UI_PROMPT_MSG_STAT_DISCONNECTING_FROM_SERVER);
            _FUN_SslDisconnect();
        }
        else
        {
            UI_ErrMsgDisplay(ERR_FUN_SSL_CONNECTION_FAILED);
            ret = ERR_FUN_SSL_CONNECTION_FAILED;
        }

        _FUN_SslStop();
    }
    else
    {
        UI_ErrMsgDisplay(ERR_FUN_SSL_PROFILE_NOT_FOUND);
    }

    return ret;
}


#ifdef __cplusplus
}
#endif


