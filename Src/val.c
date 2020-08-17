#include <globals.h>

/** \return
 * - 1 if current transaction is approved
 * - 0 if not
 * - 0 in case of fatal error
 */
int valRspCod(void) {
	int ret;
	char RspCod[lenRspCod + 1];

	MAPGET(traRspCod, RspCod, lblKO);
	if(strncmp(RspCod, "00",2) == 0)
		return 1;

	return 0;
	lblKO:
	return 0;
}
