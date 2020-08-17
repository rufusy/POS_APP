//! \file
//! Module that gives developers an helping interface for menu creation
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
//// Includes ///////////////////////////////////////////////////

#include "Cless_Implementation.h"

/////////////////////////////////////////////////////////////////
//// Macros & preprocessor definitions //////////////////////////

#define C_FIRSTCOL						0						//!< Definition of the first comlumn.
#define BACKGROUNDPIXELS				1						//!< Number of pixels added on the top and under the message to form the background.

#ifdef __TELIUM3__
#define PATCH_MESSAGES
#endif

/////////////////////////////////////////////////////////////////
//// Types //////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//// Global variables ///////////////////////////////////////////


#ifdef PATCH_MESSAGES
typedef struct
{
	int id;
	const char* msg;
} Message_t;

static const Message_t Messages_en[]=
{
		{ STD_MESS_AVAILABLE, "AVAILABLE:" },
		{ STD_MESS_REMOVE_CARD, "REMOVE CARD" },
		{ STD_MESS_USER, "USER" },
		{ STD_MESS_KEYBOARD, "KEYBOARD" },
		{ STD_MESS_STOPPED, "STOPPED:?" },
		{ STD_MESS_ERROR, "ERROR" },
		{ STD_MESS_ERROR_STATUS, "ERROR STATUS" },
		{ STD_MESS_OFFLINE_APPROVED, "OFFLINE APPROVED" },
		{ STD_MESS_OFFLINE_DECLINED, "DECLINED" },
		{ STD_MESS_ONLINE_REQUEST, "ONLINE REQUEST" },
		{ STD_MESS_ONLINE_APPROVED, "ONLINE APPROVED" },
		{ STD_MESS_ONLINE_DECLINED, "ONLINE DECLINED" },
		{ STD_MESS_APPROVED, "APPROVED" },
		{ STD_MESS_PIN_ERROR, "PIN ENTRY ERROR" },
		{ STD_MESS_ENTER_AMOUNT, "ENTER AMOUNT" },
		{ STD_MESS_SIGNATURE_CHECK_L1, "SIGNATURE" },
		{ STD_MESS_SIGNATURE_CHECK_L2, "CHECKING" },
		{ STD_MESS_PRINT_RECEIPT, "PRINT RECEIPT ?" },
		{ STD_MESS_PRINT_TRANSACTION_LOG, "PRINT LOG ?" },
		{ STD_MESS_SIGNATURE_OK, "SIGNATURE OK ?" },
		{ STD_MESS_SIGNATURE_CORRECT, "SIGNATURE OK" },
		{ STD_MESS_SIGNATURE_INCORRECT, "BAD SIGNATURE" },
		{ STD_MESS_UNABLE_ON_LINE, "ONLINE UNABLE" },
		{ STD_MESS_ONLINE_PIN_ERROR, "ONLINE PIN\n  FAILED" },
		{ STD_MESS_CARD_BLOCKED, "CARD BLOCKED" },
		{ STD_MESS_APPLICATION_BLOCKED, "APPLI. BLOCKED" },
		{ STD_MESS_AMOUNT, "AMOUNT:" },
		{ STD_MESS_DEBIT, "DEBIT" },
		{ STD_MESS_CASH, "CASH" },
		{ STD_MESS_REFUND, "REFUND" },
		{ STD_MESS_TRANSAC_TYPE, "TRANSAC TYPE" },
		{ STD_MESS_APPROVED_TICKET, "APPROVED" },
		{ STD_MESS_DECLINED_TICKET, "DECLINED" },
		{ STD_MESS_BATCH_ERROR, "BATCH ERROR" },
		{ STD_MESS_CHOICE_YES_NO, "YES              NO" },
		{ STD_MESS_CHOICE_YES, "YES" },
		{ STD_MESS_CHOICE_NO, "NO" },
		{ STD_MESS_VALIDATE, "VALIDATE" },
		{ STD_MESS_CANCEL, "CANCEL" },
		{ STD_MESS_ENTER_PIN, "ENTER YOUR PIN" },
		{ STD_MESS_TRY_AGAIN, "TRY AGAIN" },
		{ STD_MESS_SAFE, "SAFE FROM PRYING EYES" },
		{ STD_MESS_REQUESTED_DATA, "REQUESTED DATA" },
		{ STD_MESS_DATA_RECORD, "DATA RECORD" },
		{ STD_MESS_DATABASE_DUMP, "DATABASE DUMP" },
		{ STD_MESS_PRESENT_CARD, "PRESENT CARD" },
		{ STD_MESS_MENU_MAIN_DEBIT, "DEBIT" },
		{ STD_MESS_MENU_MAIN_CASH, "CASH" },
		{ STD_MESS_MENU_MAIN_REFUND, "REFUND" },
		{ STD_MESS_MENU_MAIN_CASHBACK, "CASHBACK" },
		{ STD_MESS_MENU_MAIN_TEST, "TEST" },
		{ STD_MESS_MENU_MAIN_PARAMETERS, "PARAMETERS" },
		{ STD_MESS_MENU_MAIN_MERCHANT, "MERCHANT" },
		{ STD_MESS_MENU_MAIN_BATCH, "BATCH" },
		{ STD_MESS_MENU_TEST_DUMP_TR, "DUMP DATA TR" },
		{ STD_MESS_MENU_TEST_DUMP_DB, "DUMP DATA DB" },
		{ STD_MESS_MENU_TEST_LOOP, "TRANSAC LOOP" },
		{ STD_MESS_MENU_IMPLICIT_SEL_LOOP, "IMPLICIT SEL. LOOP" },
		{ STD_MESS_MENU_TEST_CHANGE_OUTPUT, "CHANGE OUTPUT" },
		{ STD_MESS_MENU_TEST_TRACES, "TRACES" },
		{ STD_MESS_MENU_TEST_VISA_EUROPE, "VISA EUROPE" },
		{ STD_MESS_MENU_TEST_INPUT_TRANSACTION_TYPE, "INPUT TR. TYPE" },
		{ STD_MESS_MENU_OUTPUT_NONE, "NONE" },
		{ STD_MESS_MENU_OUTPUT_PRINTER, "PRINTER" },
		{ STD_MESS_MENU_OUTPUT_COM0, "COM0" },
		{ STD_MESS_MENU_OUTPUT_USB, "USB" },
		{ STD_MESS_MENU_PARAMS_DUMP, "DUMP" },
		{ STD_MESS_MENU_PARAMS_DEFAULT, "DEFAULT" },
		{ STD_MESS_MENU_PARAMS_ERASE_ALL, "ERASE ALL" },
		{ STD_MESS_MENU_PARAMS_DUMP_ID, "IDENTIFIER" },
		{ STD_MESS_MENU_PARAMS_DUMP_AIDS, "AIDS" },
		{ STD_MESS_MENU_PARAMS_DUMP_CA, "CA KEYS" },
		{ STD_MESS_MENU_PARAMS_DUMP_GLOBALS, "GLOBAL PARAMS" },
		{ STD_MESS_MENU_PARAMS_DUMP_KERNEL_SPECIFIC, "KERNEL SPECIFIC" },
		{ STD_MESS_MENU_PARAMS_DUMP_ALL, "ALL" },
		{ STD_MESS_MENU_PARAMS_MERCHANT_FORCE_ONLINE, "FORCE ONLINE" },
		{ STD_MESS_MENU_PARAMS_MERCHANT_RESET_TSC, "RESET TSC" },
		{ STD_MESS_MENU_PARAMS_BATCH_TOTALS, "TOTALS" },
		{ STD_MESS_MENU_PARAMS_BATCH_PRINT_LOG, "PRINT LOG" },
		{ STD_MESS_MENU_PARAMS_BATCH_CLEAR, "CLEAR" },
		{ STD_MESS_MENU_PARAMS_BATCH_LAST_TR, "LAST TRANSACTION" },
		{ STD_MESS_ENABLE, "ENABLED" },
		{ STD_MESS_DISABLE, "DISABLED" },
		{ STD_MESS_IN_PROGRESS, "IN PROGRESS" },
		{ STD_MESS_COMPLETE, "COMPLETED" },
		{ STD_MESS_UPDATING_PARAMS, "PARAMS UPDATE" },
		{ STD_MESS_ERASING_PARAMS, "PARAMS ERASE" },
		{ STD_MESS_INVALID_CERTIFICATE, "INVALID CERTIFICATE" },
		{ STD_MESS_PIN_CANCEL, "INPUT PIN CANCELED" },
		{ STD_MESS_REPRESENT_CARD, "REPRESENT CARD" },
		{ STD_MESS_CARD_DEVICE, "Card or Device" },
		{ STD_MESS_CARD_NOT_SUPPORTED, "Not Supported" },
		{ STD_MESS_PHONE_INSTRUCTIONS_L1, "SEE PHONE FOR" },
		{ STD_MESS_PHONE_INSTRUCTIONS_L2, "INSTRUCTIONS" },
		{ STD_MESS_USE_OTHER_CARD_L1, "PLEASE TRY" },
		{ STD_MESS_USE_OTHER_CARD_L2, "ANOTHER CARD" },
		{ VISAWAVE_MESS_PROCESSING, "PROCESSING..." },
		{ VISAWAVE_MESS_REMOVE_CARD, "REMOVE CARD" },
		{ VISAWAVE_MESS_PLEASE_WAIT, "PLEASE WAIT" },
		{ VISAWAVE_MESS_THANK_YOU, "THANK YOU" },
		{ VISAWAVE_MESS_TRANSACTION, "TRANSACTION" },
		{ VISAWAVE_MESS_TRANSACTION_COMPLETED, "COMPLETED" },
		{ VISAWAVE_MESS_SIGNATURE_CHECK_L1, "PLEASE SIGN ON" },
		{ VISAWAVE_MESS_SIGNATURE_CHECK_L2, "THE RECEIPT" },
		{ VISAWAVE_MESS_ENTER_PIN, "PLEASE ENTER PIN" },
		{ VISAWAVE_MESS_OFFLINE_AVAILABLE_FUNDS_L1, "OFFLINE" },
		{ VISAWAVE_MESS_OFFLINE_AVAILABLE_FUNDS_L2, "AVAILABLE FUNDS" },
		{ VISAWAVE_MESS_TRANSACTION_DECLINED, "DECLINED" },
		{ VISAWAVE_MESS_TRANSACTION_TERMINATED, "TERMINATED" },
		{ VISAWAVE_MESS_USE_OTHER_VISA_CARD_L1, "PLEASE USE OTHER" },
		{ VISAWAVE_MESS_USE_OTHER_VISA_CARD_L2, "VISA CARD" },
		{ VISAWAVE_MESS_INSERT_OR_SWIPE, "INSERT OR SWIPE" },
		{ VISAWAVE_MESS_PIN_REQUIRED, "PIN ENTRY REQUIRED" },
		{ VISAWAVE_MESS_SIGNATURE_REQUIRED, "SIGNATURE REQUIRED" },
		{ VISAWAVE_MESS_TRANSACTION_NOT_COMPLETED, "NOT COMPLETED" },
		{ VISAWAVE_MESS_NO_CARD, "NO CARD" },
		{ VISAWAVE_MESS_TRY_AGAIN, "PLEASE TRY AGAIN" },
		{ VISAWAVE_MESS_SELECT_ONE_CARD_L1, "PLEASE SELECT" },
		{ VISAWAVE_MESS_SELECT_ONE_CARD_L2, "1 CARD" },
		{ VISAWAVE_MESS_PHONE_INSTRUCTIONS_L1, "SEE PHONE FOR" },
		{ VISAWAVE_MESS_PHONE_INSTRUCTIONS_L2, "INSTRUCTIONS" },
		{ STD_MESS_INSERT_CARD, "INSERT CARD" },


		{ STD_MESS_PLEASE_REMOVE, "PLEASE REMOVE" },
		{ STD_MESS_SALE, "SALE" },
		{ STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L1, "INSERT, SWIPE, OR" },
		{ STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L2, "TRY ANOTHER CARD" },
		{ STD_MESS_AUTHORISING, "AUTHORISING" },
		{ STD_MESS_PLEASE_WAIT, "PLEASE WAIT" },
		{ STD_MESS_MENU_TEST_DUMP_APDU_DE, "DUMP APDU DE" },
		{ STD_MESS_MENU_TEST_HALKBANK_DE, "HALKBANK" },
		{ INTERAC_MESS_SELECT_ONE_CARD_L1, "PRESENT/PRESENTER" },
		{ INTERAC_MESS_SELECT_ONE_CARD_L2, "1 CCARD/CARTE" },
		{ INTERAC_MESS_NOT_SUPPORTED_L1, "NOT SUPPORTED" },
		{ INTERAC_MESS_NOT_SUPPORTED_L2, "CARTE NON GEREE" },
		{ INTERAC_MESS_INSERT_CARD_L1, "INSERT/SWIPE CARD" },
		{ INTERAC_MESS_INSERT_CARD_L2, "INSERER/SWIPE CARTE" },
		{ INTERAC_MESS_PRESENT_CARD, "PRESENT CARD/CARTE" },
		{ INTERAC_MESS_REFUND, "CREDIT" },
		{ INTERAC_MESS_DEBIT, "DEBIT" },
		{ INTERAC_MESS_NO_CARD, "NO CARD/CARTE" },
		{ EXPRESSPAY_MESS_PROCESSING, "PROCESSING" },
		{ EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L1, "TRANSACTION" },
		{ EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L2, "NOT PERMITTED" },
		{ PAYPASS_MESS_NOT_AUTHORISED, "NOT AUTHORISED" },
		{ PAYPASS_MESS_ENTER_PIN, "ENETER PIN" },
		{ PAYPASS_MESS_ENTER_PIN_L2, "ON TERMINAL" },
		{ PAYPASS_MESS_REMOVE, "REMOVE" },
		{ PAYPASS_MESS_REMOVE_L2, "CARD / DEVICE" },
		{ PAYPASS_DS_MESS_MENU_ERASE, "ERASE" },
		{ PAYPASS_DS_MESS_MENU_CONSULT, "CONSULT" },
		{ PAYPASS_DS_MESS_MENU_DATA_STORAGE, "DATA STORAGE" },
		{ PAYPASS_DS_MESS_MENU_REDEEM, "REDEEM" },
		{ PAYPASS_DS_MESS_MENU_REGISTER, "REGISTER" },
		{ STD_MESS_COLLISION_LINE_1, "PLEASE PRESENT" },
		{ STD_MESS_COLLISION_LINE_2, "ONLY ONE CARD" },
		{ STD_MESS_RECEIPT_TXN_WITH_MOBILE, "TRANSACTION USING MOBILE" },
		{ STD_MESS_RECEIPT_TXN_WITH_CARD, "TRANSACTION USING CARD" },
		{ STD_MESS_RECEIPT_PIN_VERIFIED, "CARDHOLDER PIN VERIFIED" },
		{ STD_MESS_RECEIPT_NO_CARDHOLDER_VERIFICATION, "NO CARDHOLDER VERIFICATION" },
		{ STD_MESS_FAILED, "FAILED" },
		{ STD_MESS_DLL_TPASS, "TPASS DLL" },
		{ STD_MESS_NOT_LOADED, "MISSING" },
		{ STD_MESS_MENU_DO_TRANSACTION, "DO TXN" },
		{ STD_MESS_CARD_NOT_SUPPORTED, "NOT SUPPORTED" },
		{ STD_MESS_MENU_TORN_LOG, "TORN LOG" },
		{ STD_MESS_MENU_PARAMS_TORN_PRINT_LOG, "PRINT LOG" },
		{ STD_MESS_MENU_PARAMS_TORN_CLEAN_LOG, "CLEAN" },
		{ STD_MESS_MENU_MAIN_MANUAL_CASH, "MANUAL CASH" },
		{ STD_MESS_MENU_MAIN_CASH_DEPOSIT, "CASH DEPOSIT" },
		{ STD_MESS_MENU_DATA_EXCHANGE, "DATA EXCHANGE" },
		{ STD_MESS_CASHBACK, "CASHBACK" },
		{ STD_MESS_EMPTY, " " },
		{ STD_MESS_MENU_PARAMS_TORN_AUTO_CLEAN_LOG, "AUTO CLEAN" },
		{ STD_MESS_MENU_STD_TXN, "Std txn" },
		{ STD_MESS_MENU_SPECIFIC_TXN, "Specific txn" },
		{ STD_MESS_MENU_CLEAN_TORN, "Clean torn" },
		{ STD_MESS_MENU_MENU, "Menu" },
		{ STD_MESS_MENU_PARAMS_SELECT_FILE, "SELECT FILE" },
		{ STD_MESS_MENU_MAIN_CASH_DISBURSMENT, "CASH DISBURSMENT"},
		{ STD_MESS_MENU_MAIN_TTYPE_UNKNOWN, "UNKNOWN (0x88)"},
		{ 0, NULL }
};

static const Message_t Messages_fr[]=
{
		{ STD_MESS_AVAILABLE, "DISPONIBLE:" },
		{ STD_MESS_REMOVE_CARD, "RETIRER CARTE" },
		{ STD_MESS_USER, "USAGER" },
		{ STD_MESS_KEYBOARD, "CLAVIER" },
		{ STD_MESS_STOPPED, "STOP:?" },
		{ STD_MESS_ERROR, "ERREUR" },
		{ STD_MESS_ERROR_STATUS, "ERREUR STATUS" },
		{ STD_MESS_OFFLINE_APPROVED, "APPROUVE" },
		{ STD_MESS_OFFLINE_DECLINED, "DECLINE" },
		{ STD_MESS_ONLINE_REQUEST, "AUTORISATION" },
		{ STD_MESS_ONLINE_APPROVED, "AUTORISATION OK" },
		{ STD_MESS_ONLINE_DECLINED, "AUTORISATION KO" },
		{ STD_MESS_APPROVED, "ACCEPTEE" },
		{ STD_MESS_PIN_ERROR, "ERREUR PIN" },
		{ STD_MESS_ENTER_AMOUNT, "SAISIR MONTANT" },
		{ STD_MESS_SIGNATURE_CHECK_L1, "VERIFICATION" },
		{ STD_MESS_SIGNATURE_CHECK_L2, "SIGNATURE" },
		{ STD_MESS_PRINT_RECEIPT, "IMPRIMER TICKET?" },
		{ STD_MESS_PRINT_TRANSACTION_LOG, "IMPRIMER LOG?" },
		{ STD_MESS_SIGNATURE_OK, "SIGNATURE OK ?" },
		{ STD_MESS_SIGNATURE_CORRECT, "SIGNATURE OK" },
		{ STD_MESS_SIGNATURE_INCORRECT, "SIGNATURE KO" },
		{ STD_MESS_UNABLE_ON_LINE, "AUTOR IMPOSSIBLE" },
		{ STD_MESS_ONLINE_PIN_ERROR, "VERIF PIN KO" },
		{ STD_MESS_CARD_BLOCKED, "CARTE BLOQUEE" },
		{ STD_MESS_APPLICATION_BLOCKED, "APPLI. BLOQUEE" },
		{ STD_MESS_AMOUNT, "MONTANT:" },
		{ STD_MESS_DEBIT, "DEBIT" },
		{ STD_MESS_CASH, "CASH" },
		{ STD_MESS_REFUND, "CREDIT" },
		{ STD_MESS_TRANSAC_TYPE, "TYPE TRANSAC" },
		{ STD_MESS_APPROVED_TICKET, "APPROUVE" },
		{ STD_MESS_DECLINED_TICKET, "DECLINE" },
		{ STD_MESS_BATCH_ERROR, "ERREUR BATCH" },
		{ STD_MESS_CHOICE_YES_NO, "OUI             NON" },
		{ STD_MESS_CHOICE_YES, "OUI" },
		{ STD_MESS_CHOICE_NO, "NON" },
		{ STD_MESS_VALIDATE, "VALIDER" },
		{ STD_MESS_CANCEL, "ANNULER" },
		{ STD_MESS_ENTER_PIN, "SAISIR PIN" },
		{ STD_MESS_TRY_AGAIN, "NOUVEL ESSAI" },
		{ STD_MESS_SAFE, "A L'ABRIS DES REGARDS" },
		{ STD_MESS_REQUESTED_DATA, "DONNEES DEMANDEES" },
		{ STD_MESS_DATA_RECORD, "ENREGISTREMENT DONNEES" },
		{ STD_MESS_DATABASE_DUMP, "LISTE DES DONNEES" },
		{ STD_MESS_PRESENT_CARD, "PRESENTER CARTE" },
		{ STD_MESS_MENU_MAIN_DEBIT, "DEBIT" },
		{ STD_MESS_MENU_MAIN_CASH, "CASH" },
		{ STD_MESS_MENU_MAIN_REFUND, "CREDIT" },
		{ STD_MESS_MENU_MAIN_CASHBACK, "CASHBACK" },
		{ STD_MESS_MENU_MAIN_TEST, "TEST" },
		{ STD_MESS_MENU_MAIN_PARAMETERS, "PARAMETRES" },
		{ STD_MESS_MENU_MAIN_MERCHANT, "COMMERCANT" },
		{ STD_MESS_MENU_MAIN_BATCH, "ENREGISTREMENTS" },
		{ STD_MESS_MENU_TEST_DUMP_TR, "DUMP DONNEES TR" },
		{ STD_MESS_MENU_TEST_DUMP_DB, "DUMP DONNEES DB" },
		{ STD_MESS_MENU_TEST_LOOP, "TRANSAC BOUCLE" },
		{ STD_MESS_MENU_IMPLICIT_SEL_LOOP, "IMPLICIT SEL. LOOP" },
		{ STD_MESS_MENU_TEST_CHANGE_OUTPUT, "CHANGER SORTIE" },
		{ STD_MESS_MENU_TEST_TRACES, "TRACES" },
		{ STD_MESS_MENU_TEST_VISA_EUROPE, "VISA EUROPE" },
		{ STD_MESS_MENU_TEST_INPUT_TRANSACTION_TYPE, "SAISIE TR. TYPE" },
		{ STD_MESS_MENU_OUTPUT_NONE, "AUCUNE" },
		{ STD_MESS_MENU_OUTPUT_PRINTER, "IMPRIMANTE" },
		{ STD_MESS_MENU_OUTPUT_COM0, "COM0" },
		{ STD_MESS_MENU_OUTPUT_USB, "USB" },
		{ STD_MESS_MENU_PARAMS_DUMP, "DUMP" },
		{ STD_MESS_MENU_PARAMS_DEFAULT, "DEFAUT" },
		{ STD_MESS_MENU_PARAMS_ERASE_ALL, "TOUT SUPPRIMER" },
		{ STD_MESS_MENU_PARAMS_DUMP_ID, "IDENTIFIANT" },
		{ STD_MESS_MENU_PARAMS_DUMP_AIDS, "AIDS" },
		{ STD_MESS_MENU_PARAMS_DUMP_CA, "CERTIFICATS" },
		{ STD_MESS_MENU_PARAMS_DUMP_GLOBALS, "PARAMS GLOBAUX" },
		{ STD_MESS_MENU_PARAMS_DUMP_KERNEL_SPECIFIC, "SPECIFIQUE NOYAU" },
		{ STD_MESS_MENU_PARAMS_DUMP_ALL, "TOUT" },
		{ STD_MESS_MENU_PARAMS_MERCHANT_FORCE_ONLINE, "FORCE AUTORISATION" },
		{ STD_MESS_MENU_PARAMS_MERCHANT_RESET_TSC, "RESET TSC" },
		{ STD_MESS_MENU_PARAMS_BATCH_TOTALS, "STATS" },
		{ STD_MESS_MENU_PARAMS_BATCH_PRINT_LOG, "IMPRIMER LOG" },
		{ STD_MESS_MENU_PARAMS_BATCH_CLEAR, "EFFACER" },
		{ STD_MESS_MENU_PARAMS_BATCH_LAST_TR, "DERNIERE TRANSAC" },
		{ STD_MESS_ENABLE, "ACTIVE" },
		{ STD_MESS_DISABLE, "DESACTIVE" },
		{ STD_MESS_IN_PROGRESS, "EN COURS" },
		{ STD_MESS_COMPLETE, "TERMINE" },
		{ STD_MESS_UPDATING_PARAMS, "MAJ PARAMETRES" },
		{ STD_MESS_ERASING_PARAMS, "EFFACEMENT" },
		{ STD_MESS_INVALID_CERTIFICATE, "CERTIFICAT INVALIDE" },
		{ STD_MESS_PIN_CANCEL, "PIN NON SAISI" },
		{ STD_MESS_REPRESENT_CARD, "REPRESENTER CARTE" },
		{ STD_MESS_CARD_DEVICE, "Carte ou Mobile" },
		{ STD_MESS_CARD_NOT_SUPPORTED, "Non Geree" },
		{ STD_MESS_PHONE_INSTRUCTIONS_L1, "VOIR INSTRUCTIONS" },
		{ STD_MESS_PHONE_INSTRUCTIONS_L2, "SUR LE TELEPHONE" },
		{ STD_MESS_USE_OTHER_CARD_L1, "UTILISER UNE" },
		{ STD_MESS_USE_OTHER_CARD_L2, "AUTRE CARTE" },
		{ VISAWAVE_MESS_PROCESSING, "TRAITEMENT..." },
		{ VISAWAVE_MESS_REMOVE_CARD, "RETIRER CARTE" },
		{ VISAWAVE_MESS_PLEASE_WAIT, "PATIENTER" },
		{ VISAWAVE_MESS_THANK_YOU, "MERCI" },
		{ VISAWAVE_MESS_TRANSACTION, "TRANSACTION" },
		{ VISAWAVE_MESS_TRANSACTION_COMPLETED, "TERMINEE" },
		{ VISAWAVE_MESS_SIGNATURE_CHECK_L1, "SIGNER SUR" },
		{ VISAWAVE_MESS_SIGNATURE_CHECK_L2, "LE TICKET" },
		{ VISAWAVE_MESS_ENTER_PIN, "SAISIR PIN" },
		{ VISAWAVE_MESS_OFFLINE_AVAILABLE_FUNDS_L1, "MONTANT OFFLINE" },
		{ VISAWAVE_MESS_OFFLINE_AVAILABLE_FUNDS_L2, "DISPONIBLE" },
		{ VISAWAVE_MESS_TRANSACTION_DECLINED, "DECLINEE" },
		{ VISAWAVE_MESS_TRANSACTION_TERMINATED, "TERMINEE" },
		{ VISAWAVE_MESS_USE_OTHER_VISA_CARD_L1, "UTILISER UNE" },
		{ VISAWAVE_MESS_USE_OTHER_VISA_CARD_L2, "AUTRE CARTE VISA" },
		{ VISAWAVE_MESS_INSERT_OR_SWIPE, "INSERER OU SWIPER" },
		{ VISAWAVE_MESS_PIN_REQUIRED, "SAISIE PIN REQUISE" },
		{ VISAWAVE_MESS_SIGNATURE_REQUIRED, "SIGNATURE REQUISE" },
		{ VISAWAVE_MESS_TRANSACTION_NOT_COMPLETED, "INCOMPLETE" },
		{ VISAWAVE_MESS_NO_CARD, "PAS DE CARTE" },
		{ VISAWAVE_MESS_TRY_AGAIN, "NOUVEL ESSAI" },
		{ VISAWAVE_MESS_SELECT_ONE_CARD_L1, "PRESENTER UNE" },
		{ VISAWAVE_MESS_SELECT_ONE_CARD_L2, "SEULE CARTE" },
		{ VISAWAVE_MESS_PHONE_INSTRUCTIONS_L1, "VOIR INSTRUCTIONS" },
		{ VISAWAVE_MESS_PHONE_INSTRUCTIONS_L2, "SUR LE TELEPHONE" },

		{ STD_MESS_PLEASE_REMOVE, "RETIRER" },
		{ STD_MESS_SALE, "VENTE" },
		{ STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L1, "INSERER, SWIPER OU" },
		{ STD_MESS_INSERT_OR_TRY_ANOTHER_CARD_L2, "ESSAYER UNE AUTRE CARTE" },
		{ STD_MESS_AUTHORISING, "AUTORISATION" },
		{ STD_MESS_PLEASE_WAIT, "PATIENTER" },
		{ STD_MESS_MENU_TEST_DUMP_APDU_DE, "DUMP APDU DE" },
		{ STD_MESS_MENU_TEST_HALKBANK_DE, "HALKBANK" },
		{ INTERAC_MESS_SELECT_ONE_CARD_L1, "PRESENTER/PRESENT" },
		{ INTERAC_MESS_SELECT_ONE_CARD_L2, "1 CARTE/CARD" },
		{ INTERAC_MESS_NOT_SUPPORTED_L1, "CARTE NON GEREE" },
		{ INTERAC_MESS_NOT_SUPPORTED_L2, "NOT SUPPORTED" },
		{ INTERAC_MESS_INSERT_CARD_L1, "INSERER/SWIPER CARTE" },
		{ INTERAC_MESS_INSERT_CARD_L2, "INSERT/SWIPE CARD" },
		{ INTERAC_MESS_PRESENT_CARD, "PRESENT CARTE/CARD" },
		{ INTERAC_MESS_REFUND, "CREDIT" },
		{ INTERAC_MESS_DEBIT, "DEBIT" },
		{ INTERAC_MESS_NO_CARD, "NO CARTE/CARD" },
		{ EXPRESSPAY_MESS_PROCESSING, "TRAITEMENT" },
		{ EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L1, "TRANSACTION" },
		{ EXPRESSPAY_MESS_CLESS_TRANS_NOT_PERMITTED_L2, "PAS PERMISE" },
		{ PAYPASS_MESS_NOT_AUTHORISED, "NON AUTORISE" },
		{ PAYPASS_MESS_ENTER_PIN, "SAISIR PIN" },
		{ PAYPASS_MESS_ENTER_PIN_L2, "SUR LE TERMINAL" },
		{ PAYPASS_MESS_REMOVE, "RETIRER" },
		{ PAYPASS_MESS_REMOVE_L2, "CARTE/MOBILE" },
		{ PAYPASS_DS_MESS_MENU_ERASE, "EFFACER" },
		{ PAYPASS_DS_MESS_MENU_CONSULT, "CONSULTER" },
		{ PAYPASS_DS_MESS_MENU_DATA_STORAGE, "DATA STORAGE" },
		{ PAYPASS_DS_MESS_MENU_REDEEM, "REDEEM" },
		{ PAYPASS_DS_MESS_MENU_REGISTER, "ENREGISTREMENT" },
		{ STD_MESS_COLLISION_LINE_1, "PRESENTER UNE" },
		{ STD_MESS_COLLISION_LINE_2, "SEULE CARTE" },
		{ STD_MESS_RECEIPT_TXN_WITH_MOBILE, "TRANSACTION MOBILE" },
		{ STD_MESS_RECEIPT_TXN_WITH_CARD, "TRANSACTION CARTE" },
		{ STD_MESS_RECEIPT_PIN_VERIFIED, "VERIFICATION CODE PORTEUR" },
		{ STD_MESS_RECEIPT_NO_CARDHOLDER_VERIFICATION, "PAS DE VERIFICATION PORTEUR" },
		{ STD_MESS_FAILED, "ECHEC" },
		{ STD_MESS_DLL_TPASS, "DLL TPASS" },
		{ STD_MESS_NOT_LOADED, "MANQUANTE" },
		{ STD_MESS_MENU_DO_TRANSACTION, "FAIRE TXN" },
		{ STD_MESS_MENU_TORN_LOG, "TORN LOG" },
		{ STD_MESS_MENU_PARAMS_TORN_PRINT_LOG, "IMPRIMER LOG" },
		{ STD_MESS_MENU_PARAMS_TORN_CLEAN_LOG, "NETTOYER" },
		{ STD_MESS_MENU_MAIN_MANUAL_CASH, "CASH MANUEL" },
		{ STD_MESS_MENU_MAIN_CASH_DEPOSIT, "DEPOT CASH" },
		{ STD_MESS_MENU_DATA_EXCHANGE, "DATA EXCHANGE" },
		{ STD_MESS_INSERT_CARD, "INSERER CARTE" },
		{ STD_MESS_CASHBACK, "CASHBACK" },
		{ STD_MESS_EMPTY, " " },
		{ STD_MESS_MENU_PARAMS_TORN_AUTO_CLEAN_LOG, "AUTO CLEAN" },
		{ STD_MESS_MENU_STD_TXN, "Std txn" },
		{ STD_MESS_MENU_SPECIFIC_TXN, "Specific txn" },
		{ STD_MESS_MENU_CLEAN_TORN, "Clean torn" },
		{ STD_MESS_MENU_MENU, "Menu" },
		{ STD_MESS_MENU_PARAMS_SELECT_FILE, "CHOIX FICHIER" },
		{ STD_MESS_MENU_MAIN_CASH_DISBURSMENT, "CASH DISBURSMENT"},
		{ STD_MESS_MENU_MAIN_TTYPE_UNKNOWN, "UNKNOWN (0x88)"},
		{ 0, NULL }

};
#endif

// Multilanguage management
const t_lang_code lang_table[C_NBLANGHOLDER]= {
		{"en"    ,"ENGLISH"    ,0       },
		{"fr"    ,"FRANCAIS"   ,1       },
		{"en"    ,"ENGLISH"    ,0xFF    }    // Not supported : english messages
};

static int FontHeight;										//!< variable for font height.

extern const unsigned char CLESS_lang[];				// Default messages in LANG

/////////////////////////////////////////////////////////////////
//// Static function definitions ////////////////////////////////


#ifdef _SIMULPC_
// This function is used ONLY IN SIMULATION
// TransfertData copies the local messages from the PC.
// to the terminal (char *LocalMessages variable) to avoid MMU errors.
extern char * TransfertData(char * msg);
#endif



/////////////////////////////////////////////////////////////////
//// Functions //////////////////////////////////////////////////


//===========================================================================
//! \brief This function display a text line in graphic mode. 
//! \param[in] nline : line index.
//! \param[in] ptrMSG : informations on text to display.
//! \param[in] nAlignment : center, left, rigth, ....
//! \param[in] bmode : TRUE to erase before display something, else nothing
//! \param[in] bLongLine : TRUE for long line to display (little police will be used), FALSE else.
//! \param[in] nFontSize : Possibility to force the font size.
//===========================================================================
void Cless_Term_DisplayGraphicLine(int nline, MSGinfos * ptrMSG, int nAlignment, unsigned char bmode, unsigned char bLongLine, const int nFontSize)
{
	int lg, x, y;
	int nScreenHeight, nScreenWidth;
	char * police;
	int nType; // _FIXED_WIDTH_ (all characters have the same width) or _PROP_WIDTH_
	int nNbLineInScreen = 5;

	if (nFontSize == HELPERS_FONT_SIZE_LARGE)
	{
		police = _dLARGE_;
	}
	else if (nFontSize == HELPERS_FONT_SIZE_MEDIUM)
	{
		police = _dMEDIUM_;
	}
	else if (nFontSize == HELPERS_FONT_SIZE_SMALL)
	{
		police = _dSMALL_;
	}
	else
	{
		if (bLongLine)
			police = _dMEDIUM_;
		else
			police = _dLARGE_;
	}
	nType = _PROP_WIDTH_;

	// In order to get back the width/heigth of the screen
	GetScreenSize(&nScreenHeight, &nScreenWidth);

	if(ptrMSG->file ==NULL)
		ptrMSG->file = GetDefaultFONaddress();  // Use the default manager font

	DefCurrentFont(ptrMSG->file);

	FontHeight = GetExtendedPoliceSize(police); // Get the height of the police 
	if (FontHeight < 0)
		GTL_Traces_TraceDebug("Police not found : %s", police);
	else
		nNbLineInScreen = nScreenHeight / FontHeight;

	// Get the length of the input text
	if(ptrMSG->coding == _UNICODE_)
		lg = SizeOfUnicodeString((unsigned char *)(ptrMSG->message), police, nType);
	else
		lg = SizeOfISO8859String((unsigned char *)(ptrMSG->message), police, nType);

	// Calculate the position of the text to be displayed
	y = FontHeight * nline;
	x = C_FIRSTCOL;

	switch (nAlignment)
	{
	case (Cless_ALIGN_CENTER):
									x = (nScreenWidth - lg) / 2;
	break;
	case (Cless_ALIGN_LEFT):
									x = C_FIRSTCOL;
	break;
	case (Cless_ALIGN_RIGHT):
									x = nScreenWidth - lg;
	break;
	default:
		x = C_FIRSTCOL;
		break;
	} // End of switch on xt_justify

	// If the text lenght is superior to column it will be automatically truncated
	if (x < 0)
		x = C_FIRSTCOL;

	if (bmode == YESCLEAR) {
		// Clear the screen
		_clrscr();
	} else {
		// Clear only the used line
		if(!bLongLine)
			DrawRect(0, y, nScreenWidth, y + FontHeight, 0, _OFF_, _OFF_);
	}

	if (bLongLine)
	{
		int nRectPosition;
		nRectPosition = nScreenHeight - (nNbLineInScreen - nline - 1) * FontHeight;

		DrawRect(0, nRectPosition - 2*BACKGROUNDPIXELS, nScreenWidth, nRectPosition + FontHeight + BACKGROUNDPIXELS, 0, _OFF_, _ON_);
		DisplayMSGinfos(x, nRectPosition - BACKGROUNDPIXELS, ptrMSG, _ON_, police, nType);
	}
	else    
	{
		DisplayMSGinfos(x, y, ptrMSG, _OFF_, police, nType);
	}
}


//===========================================================================
//! \brief This function initialise Messages menagement. 
//===========================================================================
void Cless_Term_Initialise(void)
{
	static int bInitialised = FALSE;
	char *NewMessages;     // Address of the loaded message file
	const unsigned char* pLocalMessages;

	if (bInitialised)
		return;

	bInitialised = TRUE;

#ifdef _SIMULPC_
	pLocalMessages = TransfertData((char *)Cless_lang);
#else
	pLocalMessages = CLESS_lang;
#endif // _SIMULPC_

	trace(0, strlen("Cless_Term_Initialise()\n"), "Cless_Term_Initialise()\n");

	// Load the message file in memory
	NewMessages=LoadMSG("/SYSTEM/LANG.SGN");    
	if(NewMessages == NULL )
	{
		GTL_Traces_TraceDebug("Cless_Term_Initialise : MSG file not found :\n /SYSTEM/LANG.SGN");
		// Local messages are used
		DefCurrentMSG((char *)pLocalMessages); // Higher priority table
	} else {
		GTL_Traces_TraceDebug("Cless_Term_Initialise : MSG file loaded :\n /SYSTEM/LANG.SGN"); 
		// New messages are used
		DefCurrentMSG(NewMessages);     // Higher priority table
	}

	DefDefaultMSG((char *)pLocalMessages); // If msg not found in higher priority table, this one will be used
	// Define current and default languages (English)
	DefCurrentLang("en");
	DefDefaultLang("en");
}


//===========================================================================
//! \brief This function gives message referenced by number in appropriate language. 
//! If the specific message in not found, a default text is pointed.
//! \param[in] num message number.
//! \param[in] language language number (0:English, 1:french ...)
//! \return
//===========================================================================
void Cless_Term_Read_Message(int nNum, int nLanguage, MSGinfos * pMsgInfo)
{
#ifdef PATCH_MESSAGES
	const Message_t* msg;

	if (nLanguage == 1)
		msg = Messages_fr;
	else msg = Messages_en;

	pMsgInfo->coding = _ISO8859_;
	pMsgInfo->file = GetCurrentFont();
	pMsgInfo->message = "MSG NOT FOUND";

	while(msg->msg != NULL)
	{
		if (msg->id == nNum)
		{
			pMsgInfo->message = (char*)msg->msg;
			return;
		}

		msg++;
	}

#else
	int index;
	char ac_l_iso[] = "xx";		// Unused value

	// if language code is not valid: DefDefaultLang("en") = English will be used
	for(index=0; index<C_NBLANGHOLDER; index++)
	{
		if (lang_table[index].table_id == (unsigned char)nLanguage)
		{
			strcpy(ac_l_iso, (char *)(lang_table[index].iso639_code));
			break;
		}
	}

	DefCurrentLang(ac_l_iso);

	// Test GetMessageInfos function
	GetMessageInfos(nNum, pMsgInfo);
#endif
}

//===========================================================================
//! \brief This function return lang identifier. 
//! \param[in] puc_Lang iso639 text code.
//! \return
//! 	lang code.
//===========================================================================
int Cless_Term_GiveLangNumber(unsigned char * puc_Lang)
{
	int nIndex, nResult;

	nResult = -1;  	// Default = Bad result

	// Bad input parameter protection
	if (puc_Lang == NULL)
		return (nResult);

	// if language code is not valid: DefDefaultLang("en") = English will be used
	for(nIndex=0; nIndex<C_NBLANGHOLDER; nIndex++)
	{
		if (memcmp(lang_table[nIndex].iso639_code, puc_Lang, 2) == 0)
		{
			nResult = lang_table[nIndex].table_id;
			break;
		}
	}

	return (nResult);
}
