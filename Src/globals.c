#include <globals.h>
#include "GTL_StdTimer.h"
#include "GTL_Assert.h"

//****************************************************************************
//      EXTERN
//****************************************************************************
extern T_GL_HGRAPHIC_LIB hGoal; // Handle of the graphics object library

#define tmrN 4

typedef struct {
	card hi;                    /*!< \brief Most significant 32bits of the timer value. */
	card lo;                    /*!< \brief Least significant 32bits of the timer value. */
} tTimer;
static tTimer timer[tmrN];


//#undef trcOpt
//#define trcOpt (TRCOPT & BIT(trcTBX))

const byte *bPtr(byte val) {
	static byte x;

	x = val;
	return &x;
}

const word *wPtr(word val) {
	static word x;

	x = val;
	return &x;
}

const card *cPtr(card val) {
	static card x;

	x = val;
	return &x;
}

const byte *dPtr(const char *s) {
	return (byte *) s;
}

//get number of digits in the numeric value based on base
static byte getNumLen(card num, byte base) {
	byte ret;

	ret = 0;
	while(num) {
		ret++;
		num /= base;
	}
	return ret;
}

/** Convert a numeric value num into a decimal string dec of length len.
 * The zero value of len means that the length is to be calculated as the number of decimal digits in num.
 * If the argument len is too small, zero is returned; otherwise the number of characters converted is returned.
 * \param dec (O) Destination zero-ended string to be filled by (len+1) characters.
 * \param num (I) Numeric value to be converted.
 * \param len (I) Number of characters in the destination.
 * \pre dec!=0
 * \remark len is too small, i.e. the number of decimal digits in num is more than len
 * \return
 *    - strlen(dec) if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte num2dec(char *dec, card num, byte len) {
	byte ret;                   //to be returned: number of characters processed
	byte idx;                   //current index

	VERIFY(dec);

	//normally len is the length of the acceptor
	//if len is large, destination is zero padded from left
	if(!len) {                  //calculate len to be equal to the number of decimal digits in num
		len = getNumLen(num, 10);
		if(!len)
			len = 1;            //zero should be convertod to "0"
	}

	ret = len;                  //number of bytes processed is returned
	idx = len;
	dec[idx] = 0;
	while(idx--) {              //fill dec from right to left
		dec[idx] = (char) ((num % 10) + '0');
		num /= 10;
	}
	CHECK(!num, lblKO);         //error: len too small
	return ret;
	lblKO:
	return 0;
}

static byte chr2nib(char chr) { //convert hexadecimal character to nibble
	switch (chr) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return (byte) (chr - '0');
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return (byte) (chr - 'A' + 10);
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return (byte) (chr - 'a' + 10);
	default:
		break;
	}
	return 0x10;                //KO
}

static char nib2chr(byte nib) { //convert nibble to hexadecimal character
	VERIFY(nib <= 0x0F);
	if(nib < 10)
		return (char) ('0' + nib);
	return (char) ('A' + (nib - 10));
}

/** Convert a decimal string dec of length len into a numeric value num.
 * The zero value of len means that the length is to be calculated as the strlen(dec).
 * The spaces and tabs at the beginning of dec are ignored.
 * If operation is performed successfully, the number of characters converted is returned.
 * In case of invalid input (empty string, too long string, invalid character) zero value is returned.
 * \param       dec (I) Source string containing decimal digits only.
 * \param num (O) Pointer to the result numeric value.
 * \param len (I) Number of characters in the destination.
 * \pre
 *    - num!=0
 *    - dec!=0
 * \remarks
 *    - dec contains blank characters only.
 *        - dec contains an invalid character (non-decimal digit).
 *        - dec is too long (i.e. it is not in the card range 00000000..FFFFFFFF).
 * \return
 *    - number of characters processed from dec if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte dec2num(card * num, const char *dec, byte len) {
	byte ret;                   //to be returned: number of characters processed
	byte tmp;

	VERIFY(num);
	VERIFY(dec);

	//normally len is the number of characters to get from source string
	if(!len)
		len = (byte) strlen(dec);   //calcualte length if missing

	ret = 0;
	*num = 0;
	while((*dec == ' ') || (*dec == '\t')) {    //skip blank characters
		dec++;
		ret++;
		CHECK(len, lblKO);      //the input is empty
		len--;
	}

	while(*dec) {               //main loop: perform the conversion
		if(*dec < '0')
			break;
		if(*dec > '9')
			break;
		tmp = chr2nib(*dec++);  //convert one more character
		VERIFY(tmp <= 10);
		CHECK(tmp < 10, lblKO); //validity check
		CHECK(*num <= 0xFFFFFFFFL / 10, lblKO); //dec exceeds 0xFFFFFFFFL
		CHECK(!((*num == 0xFFFFFFFFL / 10) && (tmp > 5)), lblKO);   //dec exceeds 0xFFFFFFFFL
		*num *= 10;             //OK, we can shift it left
		*num += tmp;            //OK, let us do it
		ret++;                  //one more digit processed
		len--;
		if(!len)
			break;
	}

	return ret;
	lblKO:
	return 0;
}

/** Convert a numeric value num into a hexadecimal string hex of length len.
 * The zero value of len means that the length is to be calculated as the number of hexadecimal digits in num.
 * If the argument len is too small, zero is returned; otherwise the number of characters converted is returned.
 * \param hex (O) Destination zero-ended string to be filled by (len+1) characters.
 * \param num (I) Numeric value to be converted.
 * \param       len (I) Number of characters in the destination.
 * \pre hex!=0
 * \remark len too small, i.e. the number of hexadecimal digits in num is more than len.
 * \return
 *    - strlen(hex) if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte num2hex(char *hex, card num, byte len) {
	byte ret;                   //to be returned: number of characters processed
	byte idx;                   //current index

	VERIFY(hex);

	//normally len is the length of the acceptor
	//if len is large, destination is zero padded from left
	if(!len) {                  //calculate len to be equal to the number of hexadecimal digits in num
		len = getNumLen(num, 16);
		if(!len)
			len = 1;            //zero should be convertod to "0"
	}

	ret = len;
	idx = len;
	hex[idx] = 0;
	while(idx--) {              //fill hex from right to left
		hex[idx] = (byte) (num % 16);
		if(hex[idx] < 10)
			hex[idx] += (char) '0';
		else
			hex[idx] += (char) ('A' - 10);
		num /= 16;
	}
	CHECK(!num, lblKO);         //error: len is too small
	return ret;
	lblKO:
	return 0;
}

/** Convert a hexadecimal string hex of length len into a numeric value num.
 * The zero value of len means that the length is to be calculated as the strlen(hex).
 * The spaces and tabs at the beginning of hex are ignored.
 * If operation is performed successfully, the number of characters converted is returned.
 * In case of invalid input (empty string, too long string, invalid character) zero value is returned.
 * \param num (O) Pointer to the result numeric value.
 * \param       hex (I) Source string containing hexadecimal digits only.
 * \param len (I) Number of characters in the destination.
 * \pre
 *    - num!=0
 *    - hex!=0
 * \remark
 *        - hex contains blank characters only.
 *        - hex contains an invalid character (non-hexadecimal digit).
 *    - hex is too long (i.e. it is not in the card range 00000000..FFFFFFFF).
 * \return
 *    - number of characters processed from hex if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte hex2num(card * num, const char *hex, byte len) {
	byte ret;                   //to be returned: number of characters processed
	byte tmp;

	VERIFY(num);
	VERIFY(hex);

	//normally len is the number of characters to get from source string
	if(!len)
		len = (byte) strlen(hex);   //calcualte length if missing

	ret = 0;
	*num = 0;
	while((*hex == ' ') || (*hex == '\t')) {    //skip blank characters
		hex++;
		ret++;
		CHECK(len, lblKO);      //the input is empty
		len--;
	}

	while(*hex) {               //main loop: perform the conversion
		tmp = chr2nib(*hex++);  //convert one more character
		VERIFY(tmp <= 0x10);
		CHECK(tmp < 0x10, lblKO);   //validity check
		CHECK(*num < 0xFFFFFFFL, lblKO);    //is it too large?
		*num *= 16;             //OK, we can shift it left
		CHECK(*num < 0xFFFFFFFFL - tmp, lblKO); //can we add one more digit?
		*num += tmp;            //OK, let us do it
		ret++;                  //one more digit processed
		len--;
		if(!len)
			break;
	}

	return ret;
	lblKO:
	return 0;
}

/** Convert a binary buffer bin of length len  into a hexadecimal string hex.
 * The return value is always len*2.
 * \param hex (O) Destination zero-ended string to be filled by (2*len+1) characters.
 * \param bin (I) Pointer to the binary buffer to be converted.
 * \param len (O) Number of bytes to convert.
 * \pre
 *    - hex!=0
 *    - bin!=0
 *    - len!=0
 * \remark no
 * \return
 *    - strlen(hex). There is no possibility of errors: any input data are acceptable.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
int bin2hex(char *hex, const byte * bin, int len) {
	int ret;                    //to be returned: number of characters processed
	byte tmp;

	VERIFY(hex);
	VERIFY(bin);
	VERIFY(len);

	//normally len is the number of characters to get from source string
	//if(!len) len= strlen((char *)bin); //is not allowed
	ret = len + len;            //each byte is converted into 2 characters
	while(len--) {
		tmp = *bin++;           //get a byte
		*hex++ = nib2chr((byte) (tmp / 16));    //convert first nibble
		*hex++ = nib2chr((byte) (tmp % 16));    //convert second nibble
	}
	*hex = 0;
	return ret;
}

/** Convert a hexadecimal string hex into a binary buffer bin of length len.
 * The zero value of len means that the length is to be calculated as the strlen(hex).
 * The spaces and tabs at the beginning of hex are not allowed.
 * If operation is performed successfully, the number of bytes converted is returned.
 * In case of invalid input (invalid character) zero value is returned.
 * \param bin (O) Pointer to the buffer to be filled.
 * \param hex (I) Source string containing hexadecimal digits only.
 * \param len (I) Number of characters in the destination.
 * \pre
 *    - bin!=0
 *    - hex!=0
 * \remark
 *        - The length of hex is not even in case of len==0.
 *        - hex contains an invalid character (non-hexadecimal digit).
 * \return
 *    - number of bytes written into bin if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
int hex2bin(byte * bin, const char *hex, int len) {
	int ret;                    //to be returned: number of characters processed
	byte tmp;

	VERIFY(bin);
	VERIFY(hex);

	//if non-zero, len is the length of acceptor buffer bin
	if(!len) {                  //calcualte length if missing
		len = strlen(hex);
		//CHECK(len % 2 == 0, lblKO); //there should be 2 source characters for each output byte
		//len /= 2;
	}
	ret = 0;
	while(len--) {
		tmp = chr2nib(*hex++);  //get first nibble
		if(tmp >= 0x10)
			break;
		*bin = (byte) (tmp << 4);

		tmp = chr2nib(*hex++);  //get second nibble
		CHECK(tmp < 0x10, lblKO);
		*bin |= tmp;

		bin++;
		ret++;
	}
	return ret;
	lblKO:
	return 0;
}

/** Convert a hexadecimal string hex into a Text representation buffer of length len.
 * The zero value of len means that the length is to be calculated as the strlen(hex).
 * The spaces and tabs at the beginning of hex are not allowed.
 * If operation is performed successfully, the number of bytes converted is returned.
 * In case of invalid input (invalid character) zero value is returned.
 * \param Text(O) Pointer to the buffer to be filled.
 * \param hex (I) Source string containing hexadecimal digits only.
 * \param len (I) Number of characters in the source.
 * \pre
 *    - bin!=0
 *    - hex!=0
 * \remark
 *        - The length of hex is not even in case of len==0.
 *        - hex contains an invalid character (non-hexadecimal digit).
 * \return
 *    - number of bytes written into bin if OK.
 *    - 0 otherwise.
 */
int Hex2Text(char * Text, const char *hex, int len) {
	int var = 0;
	int length = 0;
	byte binaryData[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

	if (len == 0)
		length = strlen(hex);
	else {
		length = len;
	}

	hex2bin(binaryData, hex, (length / 2));
	for (var = 0; var < (length / 2); var++) {
		Telium_Sprintf(&Text[var],"%c",binaryData[var]);
	}

	return var;
}

/** Convert a binary buffer bin of length len into a numeric value num using direct byte order.
 * The zero value of len is not accepted.
 * The return value is always len.
 * \param num (O) pointer to a numeric value to be calculated.
 * \param       bin (I) pointer to the binary buffer to be converted.
 * \param       len (I) number of bytes to convert.
 * \pre
 *    - num!=0
 *    - bin!=0
 *    - len!=0
 *    - len<=4
 * \remark no
 * \return
 *    - len. There is no possibility of errors: any input data are acceptable.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte bin2num(card * num, const byte * bin, byte len) {
	byte ret;                   //to be returned: number of characters processed

	VERIFY(num);
	VERIFY(bin);
	VERIFY(len);                //there is no way to calculate default value of len
	VERIFY(len <= 4);

	//len is the number of characters to get from source string
	ret = len;
	*num = 0;
	while(len--) {
		*num *= 256;
		*num += *bin++;
	}
	return ret;
}

/** Convert a numeric value num into a binary buffer bin of length len using direct byte order.
 * The zero value of len is not accepted.
 * \param bin (O) Pointer to the buffer to be filled.
 * \param num (I) Numeric value to be converted.
 * \param       len (I) Number of bytes in the destination.
 * \pre
 *    - bin!=0
 *    - len!=0
 *    - len<=4
 * \remark len is too small, i.e. the number of non-zero bytes in num is more than len
 * \return
 *    - number of bytes written into bin if OK.
 *    - 0 otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0004.c
 */
byte num2bin(byte * bin, card num, byte len) {
	byte ret;                   //to be returned: number of characters processed

	VERIFY(bin);
	VERIFY(len);                //there is no way to calculate default value of len
	VERIFY(len <= 4);

	//len is the length of acceptor buffer bib
	ret = len;
	while(len--) {              //fill bin from right to left in direct order
		bin[len] = (byte) (num % 256);
		num /= 256;
	}
	CHECK(!num, lblKO);         //error: len is too small
	return ret;
	lblKO:
	return 0;
}

/** Start a timer number tmr for dly/100 seconds.
 *
 * There are 4 timers numbered from 0 to 3.
 *
 * After starting a timer a function tmrGet() should be called to know whether it is over or not.
 *
 * The timer should be over after dly/100 seconds.
 *
 * \param tmr (I)  Timer number 0..tmrN-1
 * \param dly (I)  Initial timer value in hundredth of second
 * \return non-negative value if OK; negative otherwise
 * \header sys\\sys.h
 * \source sys\\i32.c sys\\tlm.c
 * \test tcab0001.c
 */
int tmrStart(byte tmr, int dly) {
	tTimer cnt;

	VERIFY(tmr < tmrN);

	/* Get the current timer value */
	cnt.hi = 0;
	//    cnt.lo = get_tick_counter();    // In 10ms ticks.
	cnt.lo = GTL_StdTimer_GetCurrent();

	/* Set the timer a 'period' into the future */
	cnt.lo += dly;

	/* If an overflow occurs, adjust the high tick counter */
	//if ((unsigned int)dly > (0xffffffffL - cnt.lo))
	if(cnt.lo < (card) dly)
		(cnt.hi)++;

	/* Assign it to the proper timer */
	timer[tmr].hi = cnt.hi;
	timer[tmr].lo = cnt.lo;

	return dly;
}

/** Return the state of the timer tmr.
 * \param tmr (I) Timer number 0..tmrN-1
 * \return
 *    - negative if failure.
 *    - otherwise the number of centiseconds rest.
 * \header sys\\sys.h
 * \source sys\\i32.c sys\\tlm.c
 * \test tcab0001.c
 */
int tmrGet(byte tmr) {
	card lo;
	tTimer cnt;

	VERIFY(tmr < 4);

	/* Get the new time */
	cnt.hi = timer[tmr].hi;
	cnt.lo = timer[tmr].lo;

	lo = GTL_StdTimer_GetCurrent();  //telium_get_tick_counter() ;    // In 10ms ticks.

	if(lo < cnt.lo)
		return (cnt.lo - lo);
	return 0;
}

/** Stop a timer number tmr.
 *
 * Should be called when the timer is no more needed.
 * \param tmr (I) Timer number 0..tmrN-1
 * \return no
 * \header sys\\sys.h
 * \source sys\\i32.c sys\\tlm.c
 * \test tcab0001.c
 */
void tmrStop(byte tmr) {
	VERIFY(tmr < 4);
	timer[tmr].hi = 0;
	timer[tmr].lo = 0;
}

void tmrPause(byte sec) {
	tmrStart(0, sec * 100);     //start 0th timer for sec seconds
	while(tmrGet(0));           //wait until the 0th timer stops
	tmrStop(0);                 //close the 0th timer
}

/** Fill the reserved memory space by zeroes and reset the current position.
 * \param buf (M) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 **/
void bufReset(tBuffer * buf) {
	VERIFY(buf);
	VERIFY(buf->ptr);
	VERIFY(buf->dim);

	memset(buf->ptr, 0, buf->dim);
	buf->pos = 0;
}

/** Initialise the buffer buf of size dim; the memory space pointed to by ptr should be reserved before calling this function.
 * This method should be called before using a buffer.
 * \param buf (O) pointer to buffer descriptor to be initialized
 * \param ptr (I) pointer to a piece of memory reserved for buffer operations
 * \param dim (I) the size of the memory space pointed to by the pointer ptr (buffer dimension)
 * \pre
 *    - buf!=0
 *    - ptr!=0
 *    - dim!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
void bufInit(tBuffer * buf, byte * ptr, word dim) {
	VERIFY(buf);
	VERIFY(ptr);
	VERIFY(dim);
	buf->ptr = ptr;
	buf->dim = dim;
	buf->pos = 0;

	bufReset(buf);
}

/** Return buffer dimension,i.e. the maximum number of bytes reserved in buffer memory space.
 * This value is defined during buffer initialisation.
 * \param buf (I) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return buffer dimension
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
word bufDim(const tBuffer * buf) {
	VERIFY(buf);
	VERIFY(buf->dim);
	return buf->dim;
}

/** Return buffer pointer,i.e. the pointer to the reserved space defined during buffer initialisation.
 * \param buf (I) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return pointer to the buffer data
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
const byte *bufPtr(const tBuffer * buf) {
	VERIFY(buf);
	VERIFY(buf->ptr);
	return buf->ptr;
}

/** Return the number of bytes already filled.
 * The buffer length is updated by bufSet, bufIns, bufApp, bufCpy functions and can be set to zero by bufReset function.
 * \param buf (I) pointer to buffer descriptor
 * \pre
 *    - buf!=0
 * \return current buffer position
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 **/
word bufLen(const tBuffer * buf) {
	VERIFY(buf);
	return buf->pos;
}


/** Set num bytes of the reserved memory space to the value val starting from the current position.
 * The current position is moved by num bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param val (I) the value that is assigned to the next num bytes in the buffer
 * \param num (I) number of bytes to be set
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 **/
int bufSet(tBuffer * buf, byte val, word num) {
	VERIFY(buf);
	VERIFY(buf->ptr);
	VERIFY(buf->dim);

	CHECK(buf->pos + num <= buf->dim, lblKO);
	memset(buf->ptr + buf->pos, val, num);
	buf->pos += num;
	return buf->pos;
	lblKO:return -1;
}

/** Insert len bytes before the position ofs and copy the content of dat into it.
 * The current position is moved by len bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param ofs (I) the offset within the buffer where to insert the data
 * \param dat (I) pointer to the data to be inserted
 * \param len (I) number of bytes to be inserted
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
int bufIns(tBuffer * buf, word ofs, const byte * dat, int len) {
	word num;

	VERIFY(buf);
	VERIFY(buf->ptr);
	VERIFY(buf->dim);
	VERIFY(ofs <= buf->pos);

	num = (word) len;           //number of bytes from dat to operate
	if(len <= 0) {              //calculate it if a special case
		VERIFY(len >= -1);
		num = (word) strlen((char *) dat);
	}

	CHECK(buf->pos + num <= buf->dim, lblKO);   //is there enough space?
	if(!len) {                  //special case: add zero at the end
		VERIFY(ofs == buf->pos);    //adding ending zero is valid only in case of append
		CHECK(buf->pos + num < buf->dim, lblKO);
	}

	if(ofs != buf->pos) {       //if not at the end, the data will be inserted
		memmove(buf->ptr + ofs + num, buf->ptr + ofs, buf->pos - ofs);
	}

	memcpy(buf->ptr + ofs, dat, num);   //now perform the copy operation
	buf->pos += num;            //and move current position
	if(!len) {                  //for the special case append zero
		VERIFY(buf->pos < buf->dim);
		buf->ptr[buf->pos] = 0;
	}

	return buf->pos;
	lblKO:return -1;
}

/** Write len bytes at the position ofs, overwriting any existing data
 * The current position is updated when written bytes
 * go beyond the original position
 * \param buf (M) pointer to buffer descriptor
 * \param ofs (I) the offset within the buffer where to write the data
 * \param dat (I) pointer to the data to be written
 * \param len (I) number of bytes to be written
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; -1 if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
int bufOwr(tBuffer * buf, word ofs, const byte * dat, int len) {
	int ret;
	word oldPos;

	VERIFY(buf);

	oldPos = buf->pos;
	buf->pos = ofs;

	ret = bufIns(buf, ofs, dat, len);
	CHECK(ret != -1, lblKO);

	if(oldPos > buf->pos) {
		buf->pos = oldPos;
	}

	return buf->pos;

	lblKO:
	// bufIns failed : restore the original position
	buf->pos = oldPos;
	return -1;
}

/** Delete len bytes from the position ofs.
 * The current position is moved by len bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param ofs (I) the starting offset of data to be deleted
 * \param len (I) number of bytes to be deleted, if zero then delete all
 * \pre
 *    - buf!=0
 * \return current buffer position
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcgs0021.c
 */
int bufDel(tBuffer * buf, word ofs, int len) {
	VERIFY(buf);
	VERIFY(buf->ptr);
	VERIFY(buf->dim);
	VERIFY(ofs <= buf->pos);
	VERIFY(len >= 0);

	if(len == 0 || (ofs + len > buf->pos)) {    //"erase" all after ofs
		buf->pos = ofs;
		return buf->pos;
	}

	memmove(buf->ptr + ofs, buf->ptr + ofs + len, buf->pos - ofs - len);
	buf->pos -= len;
	return buf->pos;
}

/** Append len bytes to the end of the buffer and copy the content of dat into it.
 * The current position is moved by len bytes.
 * \param buf (M) pointer to buffer descriptor
 * \param dat (I) pointer to the data to be appended
 * \param len (I) number of bytes to be appended
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
int bufApp(tBuffer * buf, const byte * dat, int len) {
	return bufIns(buf, buf->pos, dat, len);
}

/** Append the content of src buffer to dst buffer.
 * The current dst position is moved by the length of src buffer.
 * \param dst (M) pointer to destination buffer descriptor
 * \param src (I) pointer to the source buffer to be appended
 * \pre
 *    - dst!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
int bufCat(tBuffer * dst, tBuffer * src) {
	return bufIns(dst, dst->pos, bufPtr(src), bufLen(src));
}

/** Just call bufReset and bufApp functions.
 * \param buf (M) pointer to buffer descriptor
 * \param dat (I) pointer to the data to be appended
 * \param len (I) number of bytes to be appended
 * \pre
 *    - buf!=0
 * \return current buffer position if OK; negative if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0009.c
 */
int bufCpy(tBuffer * buf, const byte * dat, int len) {
	bufReset(buf);
	return bufApp(buf, dat, len);
}

int bufGet(tBuffer * buf, byte * dat) {
	VERIFY(buf);
	VERIFY(dat);
	VERIFY(buf->ptr);
	memcpy(dat, buf->ptr, buf->pos);
	return buf->pos;
}


/** Empty the queue que.
 * \param que (M) pointer to queue descriptor
 * \pre
 *    - que!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
void queReset(tQueue * que) {
	VERIFY(que);
	VERIFY(que->buf);

	que->cur = que->put = que->get = 0;
	bufReset(que->buf);
}

/** Initialize a queue que linked to the buffer buf.
 * The buffer will be used to store all the elements of the queue and their lengths.
 * \param que (O) pointer to queue descriptor to be initialized
 * \param buf (I) pointer to the buffer that will stroe the queue elements
 * \pre
 *    - que!=0
 *    - buf!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
void queInit(tQueue * que, tBuffer * buf) {
	VERIFY(que);
	VERIFY(buf);

	que->buf = buf;
	queReset(que);
}

/** Put an element of len bytes pointed to by dat in the queue que.
 * The data are copied into the queue.
 * The last parameter len can have a special value len==0; it means that the number of bytes should be calculated as strlen(dat).
 * \param que (M) pointer to queue descriptor
 * \param dat (I) pointer to the data to put into the queue
 * \param len (I) length the data to put into the queue
 * \pre
 *    - que!=0
 *    - dat!=0
 * \return number of bytes put into the queue; negative value if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
int quePut(tQueue * que, const byte * dat, int len) {
	int ret;
	byte tmp[2];

	VERIFY(que);
	VERIFY(que->buf);

	if(!len)
		len = strlen((char *) dat); //calculate len if needed
	tmp[0] = (byte) (len / 256);    //get highest byte
	tmp[1] = (byte) (len % 256);    //get lowest byte

	ret = bufApp(que->buf, tmp, 2); //save element length in direct byte order
	CHECK(ret > 0, lblKO);

	ret = bufApp(que->buf, dat, len);   //save the data itself
	CHECK(ret > 0, lblKO);

	que->put++;                 //increment number of put elements
	que->get++;                 //increment number of get elements

	return len;
	lblKO:return -1;
}

/** Get the number of bytes in the next element of queue to be retrieved.
 * \param que (I) pointer to queue descriptor
 * \pre
 *    - que!=0
 * \return the length of the next data element; zero if the current element is the last
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
word queNxt(const tQueue * que) {
	const byte *ptr;
	word len;

	VERIFY(que);
	VERIFY(que->buf);

	if(!que->get)
		return 0;

	ptr = bufPtr(que->buf) + que->cur;  //ptr points to the length in direct order
	len = *ptr++;               //get highest byte
	len *= (word) 256;
	len += *ptr;                //and now get lowest byte

	return len;
}


/** Retrieve an element from the queue que into the buffer pointed by dat.
 * It is assumed that there is enought space in the destination buffer.
 * The size of the date retrieved from the que following the FIFO principle -First In First Out.
 * \param que (M) pointer to queue descriptor
 * \param dat (I) pointer to the data to get from the queue
 * \pre
 *    - que!=0
 *    - dat!=0
 * \return length of data element retrieved; negative value if there is no more data elements to be retrieved
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
int queGet(tQueue * que, byte * dat) {
	word len;

	VERIFY(que);
	VERIFY(dat);
	VERIFY(que->buf);
	CHECK(que->get, lblKO);     //are there any element to get?

	len = queNxt(que);          //retrieve the length of the next element
	que->cur += (word) 2;       //skip the length

	memcpy(dat, bufPtr(que->buf) + que->cur, len);  //copy data into destination buffer
	que->cur += len;            //move current position

	que->get--;                 //decrement number of elements to get

	return len;
	lblKO:return -1;
}

/** Return the number of elements in the queue that are saved but not retrieved yet.
 * \param que (I) pointer to queue descriptor to be initialized
 * \pre
 *    - que!=0
 * \return the number of data elements waiting to be retrieved
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
word queLen(const tQueue * que) {
	VERIFY(que);
	return que->get;
}

/** Move the reading head of the queue to the begining.
 * After calling this function the next element to read will be the first element saved.
 * \param que (M) pointer to queue descriptor
 * \pre
 *    - que!=0
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 * \test tcab0010.c
 */
void queRewind(tQueue * que) {
	VERIFY(que);
	que->cur = 0;
	que->get = que->put;
}

/** Put a tag into a queue.
 * This structure is mainly used to transfert parameters to EMV Kernel wrappers. Data object list is
 * organised as a queue of tags. Two shortcut functions are provided for this type of queues.
 * \param que (M) pointer to queue descriptor
 * \param tag (I) the tag to be put
 * \pre
 *    - que!=0
 * \return number of bytes put into the queue (sizeof(card)); negative value if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
int quePutTag(tQueue * que, card tag) {
	int ret;

	VERIFY(que);

	ret = quePut(que, (byte *) & tag, sizeof(card));
	CHECK(ret >= 0, lblKO);

	return ret;
	lblKO:return -1;
}

/** Put a tag into a queue followed by the value val of length len
 * \param que (M) pointer to queue descriptor
 * \param tag (I) the tag to be put
 * \param val (I) pointer to the data containing tag value
 * \param len (I) the length of the tag to be put
 * \pre
 *    - que!=0
 *    - tag!=0
 * \return number of bytes put into the queue; negative value if overflow
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
int quePutTlv(tQueue * que, card tag, word len, byte * val) {
	int ret;

	VERIFY(que);
	VERIFY(val);

	if(!len)
		len = (word) strlen((char *) val);

	if(len < 1)
		return 0;

	VERIFY(len);

	ret = quePut(que, (byte *) & tag, sizeof(card));
	CHECK(ret >= 0, lblKO);

	ret = quePut(que, val, len);
	CHECK(ret >= 0, lblKO);

	return ret;
	lblKO:return -1;
}

/** Get a tag from a queue
 * \param que (M) pointer to queue descriptor
 * \param tag (I) the tag to be retrieved
 * \pre
 *    - que!=0
 *    - queNxt(que)==sizeof(card)
 * \return number of bytes retrieved (sizeof(card)); negative value if failure
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
int queGetTag(tQueue * que, card * tag) {
	int ret;

	VERIFY(queLen(que));
	VERIFY(queNxt(que) == sizeof(card));

	ret = queGet(que, (byte *) tag);
	VERIFY(ret == sizeof(card));

	return ret;
}

/** Get a tag from a queue; then get its length and value.
 * \param que (M) pointer to queue descriptor
 * \param tag (O) the tag to be retrieved
 * \param len (O) length of the data retrieved into val
 * \param val (O) pointer to the data containing tag value
 * \pre
 *    - que!=0
 *    - tag!=0
 *    - len!=0
 *    - val!=0
 * \return number of bytes retrieved; negative value if failure
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
int queGetTlv(tQueue * que, card * tag, word * len, byte * val) {
	int ret;

	VERIFY(queLen(que) >= 2);

	ret = queGetTag(que, tag);
	VERIFY(ret == sizeof(card));

	ret = queGet(que, val);
	VERIFY(ret > 0);

	*len = (word) ret;

	return ret;
}

/** Find a tag from a queue
 * \param que (M) pointer to queue descriptor
 * \param tag (O) the tag to be retrieved
 * \param val (O) pointer to the data containing tag value
 * \return number of bytes retrieved; 0 if tag is not found in the queue
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
word queFindTlv(tQueue * que, card tag, byte * val) {
	card tmp;
	word len = 0;

	queRewind(que);
	while(queLen(que)) {
		queGetTlv(que, &tmp, &len, val);
		VERIFY(len <= 256);

		if(tag == tmp) {
			return len;
		}
	}

	len = 0;
	val = 0;
	return len;
}

/** Merge 2 Queue's (que1 and que2).
 * If TLV data are found on both queue's Main Queue's (que1) data is retained
 * \param que1 (O) Output mereged queue containing TLV data from the 2 Queue's
 * \param que2 (I) TLV data to be copied
 * \return number of elements in merged queue if OK; negative value if failure
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
int queMergeTlv(tQueue * que1, tQueue * que2) {
	int ret;
	card tag1, tag2;
	word len1, len2;
	byte buf1[256], buf2[256];

	//Check que1 if tags from que2 are already present
	queRewind(que2);
	while(queLen(que2)) {
		memset(buf2, 0, sizeof(buf2));
		ret = queGetTlv(que2, &tag2, &len2, buf2);
		VERIFY(ret >= 0);
		VERIFY(ret == len2);
		CHECK(len2 <= 256, lblKO);

		queRewind(que1);
		while(queLen(que1)) {
			memset(buf1, 0, sizeof(buf1));
			ret = queGetTlv(que1, &tag1, &len1, buf1);
			VERIFY(ret >= 0);
			VERIFY(ret == len1);
			CHECK(len1 <= 256, lblKO);

			if(tag1 == tag2) {
				continue;
			}
		}

		quePutTlv(que1, tag2, len2, buf2);  //add tag if not present on main queue
	}

	queRewind(que1);
	ret = queLen(que1);
	goto lblEnd;

	lblKO:
	ret = -1;
	lblEnd:
	return ret;
}

/** Switch on the bit number idx in buf. Bits are numbered from left to right starting from 1.
 * \param buf (I) Binary buffer containing a bitmap.
 * \param idx (I) Related bit number.
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
void bitOn(byte * buf, byte idx) {
	VERIFY(buf);
	VERIFY(idx);
	idx--;
	while(idx >= 8) {
		buf++;
		idx -= 8;
	}
	*buf |= (0x01 << (7 - idx));
}

/** Switch off the bit number idx in buf. Bits are numbered from left to right starting from 1.
 * \param       buf (I) Binary buffer containing a bitmap.
 * \param       idx (I) Related bit number.
 * \return N/A
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
void bitOff(byte * buf, byte idx) {
	VERIFY(buf);
	VERIFY(idx);
	idx--;
	while(idx >= 8) {
		buf++;
		idx -= 8;
	}
	*buf &= ~(0x01 << (7 - idx));
}

/** Tests the bit number idx in buf. Bits are numbered from left to right starting from 1.
 * \param buf (I) Binary buffer containing a bitmap.
 * \param       idx (I) Related bit number.
 * \return
 *    - zero if the related bit is off.
 *    - non-zero otherwise.
 * \header sys\\sys.h
 * \source sys\\tbx.c
 */
byte bitTest(const byte * buf, byte idx) {
	VERIFY(buf);
	VERIFY(idx);
	idx--;
	while(idx >= 8) {
		buf++;
		idx -= 8;
	}
	return ((*buf) >> (7 - idx)) & 0x01;
}

/** Set parity bits; par=1..8 is number of parity bit.
 * \param buf (I) Binary buffer to be set
 * \param dim (I) Buffer size
 * \param par (I) Position of the parity bit 1 or 8
 * \param typ (I) Even or Odd parity
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\tbx.c
 */
void tbxParity(byte * buf, word dim, byte par, byte typ) {  //set parity bits
	byte cnt, idx;

	VERIFY(buf);
	while(dim--) {
		cnt = 0;
		for (idx = 1; idx <= 8; idx++) {
			if(idx == par)
				continue;
			if(bitTest(buf, idx))
				cnt++;
		}
		switch (typ) {
		case 'E':
			if(cnt % 2)
				bitOff(buf, par);
			else
				bitOn(buf, par);
			break;
		case 'O':
		default:
			if(cnt % 2)
				bitOn(buf, par);
			else
				bitOff(buf, par);
			break;
		}

		buf++;
	}
}

/** Check parity bits; par=1..8 is number of parity bit.
 * \param buf (I) Binary buffer to be set
 * \param dim (I) Buffer size
 * \param par (I) Position of the parity bit 1 or 8
 * \param typ (I) Even or Odd parity
 * \return 1 if parity is OK, 0 if parity NOK;
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\tbx.c
 */
byte tbxCheckParity(byte * buf, word dim, byte par, byte typ) { //check parity bits
	byte ret;
	byte cnt, idx;

	VERIFY(buf);
	ret = 1;
	while(dim--) {
		cnt = 0;
		for (idx = 1; idx <= 8; idx++) {
			if(idx == par)
				continue;
			if(bitTest(buf, idx))
				cnt++;
		}
		bitTest(buf, par);
		switch (typ) {
		case 'O':
			if(!(cnt % 2))
				ret = 0;
			break;
		case 'E':
		default:
			if(cnt % 2)
				ret = 0;
			break;
		}
		buf++;
	}
	return ret;
}

///////==========================================================================================================================


/** Extract a substring dst from the string src of length len starting from the position ofs.
 * The function returns the length of the string extracted, strlen(dst).
 * \param dst (O) Destination buffer for the substring extracted
 * \param       src (I) Original string
 * \param       ofs (I) Starting position of the substring to be extracted
 * \param       len (I) Number of characters in the destination
 * \pre
 *    - dst!=0
 *    - src!=0
 * \return
 *    - strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
 */
int fmtSbs(char *dst, const char *src, byte ofs, byte len) {
	VERIFY(dst);
	VERIFY(src);

	memcpy(dst, src + ofs, len);
	dst[len] = 0;

	return strlen(dst);
}

/** Extract a token dst from the string src that can contain several tokens separated
 * by characters from sep.
 * The function returns the length of the string extracted, strlen(dst).
 * If dst is zero the characters are counted only; they are not copied into dst.
 * \param dst (O) Destination buffer for the token extracted; if zero, the characters are counted only
 * \param src (I) Original string
 * \param sep (I) String containing token separators
 * \pre
 *    - src!=0
 *    - sep!=0
 * \return
 *    - strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
 */
int fmtTok(char *dst, const char *src, const char *sep) {
	int len;
	const char *ptr;

	VERIFY(src);
	VERIFY(sep);

	len = 0;
	while(*src) {
		ptr = sep;
		while(*ptr) {
			if(*src == *ptr)
				break;
			ptr++;
		}
		if(*src == *ptr)
			break;
		if(dst)
			*dst++ = *src;
		src++;
		len++;
	}
	if(dst)
		*dst = 0;
	return len;
}

/** Pad the string str to length abs(len) by the character chr.
 * It is right padded if len>0 and left padded otherwise.
 * The function returns the length of the resulted padded string, strlen(str).
 * \param str (I/O) The original input string; the padded output string replaces it
 * \param len (I) Number of characters in the padded string.
 *        The negative value indicates left padding, the positive value provides
 *        right padding.
 *        It is the absolute value abs(len) that is the real length of resulting string
 * \param chr (I) Pad character
 * \pre
 *    - str!=0
 *    - len!=0
 * \return
 *    - the length of the result string strlen(str)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
 */
int fmtPad(char *str, int len, char chr) {
	int dim;                    //the initial string length
	char *ptr;                  //points to the start of padding
	char dir;                   //pad direction (left or right)

	VERIFY(str);
	VERIFY(len != 0);

	dir = 'r';
	if(len < 0) {               //left pad?
		dir = 'l';
		len = -len;
	}
	str[len] = 0;

	dim = strlen(str);
	if(dim >= len)
		return dim;             //string is long enough - nothing to pad

	ptr = str;                  //ptr will pont to the start of padding
	if(dir == 'l')
		memmove(str + len - dim, str, dim); //left padding: move characters to the right
	else
		ptr += dim;             //right padding: move the padding pointer to the end of the string

	memset(ptr, chr, len - dim);    //now pad dim characters chr starting from ptr

	return strlen(str);
}

/** Center the string str to length abs(len) by the character chr.
 * The function returns the length of the resulted padded string, strlen(str).
 * Before padding left and right, all leading and trailing characters chr are removed.
 * \param str (I) The original input string; the padded output string replaces it
 * \param len (I) number of characters in the centered string.
 * \param       chr (I) pad character
 * \pre
 *    - str!=0
 *    - len!=0
 * \return
 *    - strlen(str)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
 */
int fmtCnt(char *str, int len, char chr) {
	byte idx;
	char *ptr;

	VERIFY(str);
	VERIFY(len != 0);

	idx = strlen(str);
	while(idx--) {              //remove trailing chr
		if(str[idx] != chr)
			break;
		str[idx] = 0;
	}

	idx = 0;
	while(str[idx] == chr)
		idx++;                  //calculate leading chr

	if(idx) {
		ptr = str;
		while(str[idx]) {
			*ptr++ = str[idx++];
		}
		*ptr = 0;
	}

	idx = strlen(str);
	while(idx < len) {
		if(idx % 2) {
			str[idx++] = chr;
			str[idx++] = 0;
		} else {
			str[idx + 1] = 0;
			while(idx) {
				str[idx] = str[idx - 1];
				idx--;
			}
			*str = chr;
		}
		idx = strlen(str);
	}
	return len;
}

/** Insert a string to an another sting
 * \param dst (O) Destination string
 * \param bgd (I) the string where you insert the another string
 * \param ins (I) the string, what you insert
 * \param ofs (I) starting point for inserting in bgd
 * \param len (I) maximal length of the destenation string
 *        (0 means the length will be the sum of the bgd and ins)
 * \pre
 *    - dst!=0
 *    - src!=0
 * \return
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcgs0007.c
 */
int fmtIns(char *dst, const char *bgd, const char *ins, byte ofs, int len) {
	word idx;
	int lenIns;

	VERIFY(dst);
	VERIFY(ins);
	VERIFY(bgd);

	if(len == 0)
		len = strlen(bgd) + strlen(ins);
	VERIFY(ofs < len);
	lenIns = strlen(ins);
	lenIns = (lenIns < len - ofs) ? lenIns : (len - ofs);

	memcpy(dst, bgd, ofs);
	for (idx = len - 1; idx >= ofs + lenIns; idx--)
		memcpy(dst + idx, bgd + idx - lenIns, 1);
	memcpy(dst + ofs, ins, lenIns);
	memset(dst + len, 0, 1);

	return strlen(dst);
}

/** Format an amount string with decimals.
 * \param dst (O) Destination string to put a formatted amount
 * \param src (I) Original string containing decimal digits
 * \param exp (I) number of decimals, can be 0 or 2 or 3
 * \param sep (I) separators, a string containing up to 2 characters
 *  - decimal separator
 *  - thousands separator
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - The original string contains decimal digits only
 * \return
 *    - the length of the resulting string strlen(dst)
 * If sep is zero the default value "." is used.
 * It means that decimals are separated by '.', and thousands are not separated.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c, tcgs0017.c
 */
int fmtAmt(char *dst, const char *src, byte exp, const char *sep) {
	int ret;
	int len;
	char tmp[prtW + 1];
	char _dst[prtW + 1];
	char decSep[2] = { '.', 0 };    //decimal separator
	char thsSep[2] = { 0, 0 };  //thousand separator
	//    byte isSign = 0;

	VERIFY(dst);
	VERIFY(src);

	memset(_dst, 0, prtW + 1);
	if(*src == '-' || *src == '+') {
		char sign[2] = { 0, 0 };
		//        isSign = 1;
		sign[0] = *src;
		strcpy(_dst, sign);
		src++;
	}

	ret = fmtTok(0, src, "123456789");
	src += ret;
	len = strlen(src);

	memset(tmp, 0, sizeof(tmp));
	if(len) {
		if(len <= exp) {
			memset(tmp, '0', exp - len + 1);
			strcat(tmp, src);
		} else
			strcpy(tmp, src);
	} else {
		memset(tmp, '0', exp + 1);
	}
	len = strlen(tmp);

	if(sep && sep[0]) {
		decSep[0] = sep[0];
		if(sep[1]) {
			thsSep[0] = sep[1];
			//            VERIFY(isSign + len + !!exp +
			//                   (exp <
			//                    len ? (len - isSign - exp - 1) : (len - isSign - 1)) / 3 <=
			//                   prtW);
		} else {
			//            VERIFY(isSign + len + !!exp <= prtW);
		}
	}

	if(exp && exp < len) {
		fmtIns(tmp, tmp, decSep, (byte) (len - exp), prtW);
		len++;
	}
	if(thsSep[0]) {
		for (len -= 3 + (exp ? (exp + 1) : 0); len > 0; len -= 3) {
			ret = fmtIns(tmp, tmp, thsSep, (byte) len, prtW);
		}
	}
#ifdef __CURR__
	if(exp) {
		strcat(_dst, __CURR__);
	}
#endif
	strcat(_dst, tmp);          //concatenate the prefix and the decimals
	strcpy(dst, _dst);
	return strlen(dst);
}

/** Permutation of fgd and bgd according to the control string ctl.
 * The control string ctl contains uppercase and lowercase Latin letters.
 * Lowercase letters 'a'..'z' are associated with correspondent characters of fgd string:
 * 'a' with fgd[0], 'b' with fgd[1] etc.
 * In the same way uppercase letters 'A'..'Z' are associated with correspondent characters
 * of bgd string: 'A' with bgd[0], 'B' with bgd[1] etc.
 * The function converts ctl into dst replacing upper- and lower-case control characters
 * by related characters from bgd and fgd.
 *
 *For example the input strings :
 * - fgd: "20040902164700"
 * - bgd: "-: "
 * - ctl: "ghAefAabcdCCijBklBmn"
 *
 * provide the result:
 * - dst: "02-09-2004  16:47:00"
 *
 * \param dst (O) Destination string for permutation
 * \param fgd (I) Foreground string
 * \param bgd (I) Background string
 * \param ctl (I) Control string
 * \pre
 *    - dst!=0
 *    - fgd!=0
 *    - bgd!=0
 *    - ctl!=0
 *    - The destination buffer should be big enough to accept the result (greater than or equal to strlen(ctl)+1)
 * \return
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcab0024.c
 */
int fmtMut(char *dst, const char *fgd, const char *bgd, const char *ctl) {
	byte idx;
	byte ref;
	byte len;

	VERIFY(dst);
	VERIFY(fgd);
	VERIFY(bgd);
	VERIFY(ctl);

	idx = 0;
	len = 0;
	while(*ctl) {
		if(('a' <= (*ctl)) && ((*ctl) <= 'z')) {    //foreground index
			ref = (*ctl) - 'a';
			if(strlen(fgd) <= ref)
				dst[idx] = ' '; //fgd too small
			else
				dst[idx] = fgd[ref];
		} else if(('A' <= (*ctl)) && ((*ctl) <= 'Z')) { //background index
			ref = (*ctl) - 'A';
			if(strlen(bgd) <= ref)
				break;          //bgd too short
			else
				dst[idx] = bgd[ref];
		} else {
			if(strlen(fgd) <= len)
				break;          //fgd too short
			else
				dst[idx] = *ctl;
		}
		idx++;
		ctl++;
		len++;
	}
	dst[idx] = 0;               //add ending zero
	return idx;
}

/** Erase the len long substring from ofs
 * \param dst (O) Destination string
 * \param src (I) Original string
 * \param ofs (I) starting point for erasing
 * \param len (I) number of erased characters
 * \pre
 *    - dst!=0
 *    - src!=0
 * \return
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 * \test tcgs0007.c
 */
int fmtDel(char *dst, const char *src, byte ofs, int len) {
	word idx;

	VERIFY(src);
	VERIFY(dst);
	VERIFY(len > 0);

	if((word) (ofs + len) >= strlen(src)) {
		memcpy(dst, src, ofs);
		dst[ofs] = 0;
		return strlen(dst);
	}
	memcpy(dst, src, ofs);
	for (idx = ofs + len; idx <= strlen(src); idx++) {
		memcpy(dst + idx - len, src + idx, 1);
	}
	return strlen(dst);
}

/** Format an Host IP Address string with ".".
 * \param dst (O) Destination string to put a formatted Address
 * \param src (I) Original string
 * \param len (I) length of the Original string without null pointer
 * \pre
 *    - src!=0
 *    - The original string contains decimal digits only
 * \return
 *    - the length of the resulting string strlen(dst)
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\fmt.c
 */

int fmtTcp(char *dst, char *src, int len) {
	int dim;

	VERIFY(src);
	VERIFY(len <= dspW);

	for (dim = 0; dim < len; dim++) // Verify the size of the address
	{
		if(dim != 0 && (dim + 1) % 4 == 0 && *src != '.') {
			*dst = '.';         // Add a dot every 4 inputs - the index starts at 0
			len++;
		} else {
			*dst = *src;
			src++;
		}

		dst++;
	}
	*dst = 0;
	dim--;
	return dim;
}

int fmtDate(char *dst, char *src, int len) {
	int dim;

	VERIFY(src);
	VERIFY(len <= dspW);

	for (dim = 0; dim < len; dim++) // Verify the size of the address
	{
		if(dim != 0 && (dim + 1) % 3 == 0 && *src != '/') {
			*dst = '/';         // Add a dot every 4 inputs - the index starts at 0
			len++;
		} else {
			*dst = *src;
			src++;
		}

		dst++;
	}
	*dst = 0;
	dim--;
	return dim;
}

int fmtTime(char *dst, char *src, int len) {
	int dim;

	VERIFY(src);
	VERIFY(len <= dspW);

	for (dim = 0; dim < len; dim++) {// Verify the size of the address

		if(dim != 0 && (dim + 1) % 3 == 0 && *src != ':') {
			*dst = ':';         // Add a dot every 4 inputs - the index starts at 0
			len++;
		} else {
			*dst = *src;
			src++;
		}

		dst++;
	}
	*dst = 0;
	dim--;
	return dim;
}

/** Returns 1 if a<b<c and 0 otherwise.
 * \param  a,b,c (I) Some numbers
 * \return
 *    - 1 if a<b<c and 0 otherwise.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte isSorted(word a, word b, word c) {
	if(b < a)
		return 0;
	if(c < b)
		return 0;
	return 1;
}


static void negStr(char *str) { //convert str into it supplementary
	byte i, p;
	int n;
	char tmp[16 + 1];
	char dst[16 + 1];

	VERIFY(str);

	strcpy(tmp, str);
	fmtPad(tmp, -16, '0');      //left pad by zeroes to length 16

	i = 16;
	p = 0;
	memset(dst, 0, 16 + 1);
	while(i--) {
		VERIFY(tmp[i] <= '9');
		VERIFY(tmp[i] >= '0');
		n = tmp[i] - '0';

		n = -n;
		n -= p;
		if(n < 0) {
			n += 10;
			VERIFY(n >= 0);
			p = 1;
		} else
			p = 0;

		VERIFY(n >= 0);
		VERIFY(n < 10);
		dst[i] = n + '0';
	}

	VERIFY(strlen(dst) <= 16);
	strcpy(str, dst);
}

static void neg2sup(char *str) {    //if negative, convert it to supplementary
	VERIFY(str);
	if(str[0] != '-')
		return;                 //non-negative - nothing to do
	*str = '0';
	negStr(str);
}

static void sup2neg(char *str) {    //if supplementary, add '-' sign
	byte i;
	char tmp[16 + 1];

	VERIFY(str);
	if(strlen(str) < 16)
		return;                 //non-negative - nothing to do
	if(str[0] < '5')
		return;                 //non-negative - nothing to do
	negStr(str);

	i = 0;
	while(str[i]) {             //the leading zeroes are to be skipped
		if(str[i] != '0')
			break;
		i++;
	}
	strcpy(tmp, str + i);
	strcpy(str, tmp);

	VERIFY(strlen(str) + 1 <= 16);
	fmtPad(str, -(int) (strlen(str) + 1), '-'); //add leading '-'
}

static byte addSup(char *dst, const char *arg1, const char *arg2) { //add two values converted into supplementaries if necessary
	byte i, n, p;
	char tmp[16 + 1];
	char tmp1[16 + 1];
	char tmp2[16 + 1];

	VERIFY(dst);
	VERIFY(arg1);
	VERIFY(arg2);

	strcpy(tmp1, arg1);
	fmtPad(tmp1, -16, '0');     //left pad by zeroes to length 16

	strcpy(tmp2, arg2);
	fmtPad(tmp2, -16, '0');     //left pad by zeroes to length 16

	i = 16;
	p = 0;
	memset(tmp, 0, 16 + 1);
	while(i--) {                //perform the addition
		VERIFY(tmp1[i] <= '9');
		VERIFY(tmp1[i] >= '0');
		n = tmp1[i] - '0';

		VERIFY(tmp2[i] <= '9');
		VERIFY(tmp2[i] >= '0');
		n += tmp2[i] - '0';

		n += p;
		if(n >= 10) {
			VERIFY(n <= 19);
			n -= 10;
			p = 1;
		} else
			p = 0;

		VERIFY(n < 10);
		tmp[i] = n + '0';
	}

	i = 0;
	while(tmp[i]) {             //the leading zeroes are to be skipped
		if(tmp[i] != '0')
			break;
		i++;
	}

	VERIFY(strlen(tmp + i) <= 16);
	if(!*(tmp + i))             //full zero string
		strcpy(dst, "0");
	else
		strcpy(dst, tmp + i);
	return strlen(dst);
}

/** Add the content of the arg1 to the content of arg2 as strings of decimal digits and write the result into dst.
 * All the strings should be no more than 16 characters length.
 * The negative value strings should start by ‘-‘.
 *
 * \param dst (O) Destination string
 * \param       arg1 (I) First argument
 * \param       arg2 (I) Second argument
 *
 * \pre
 *    - dst!=0
 *    - arg1!=0
 *    - arg2!=0
 *    - The destination buffer should be large enough to accept 16-character long string (16+1)
 *
 * \return
 *    - strlen(dst)
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte addStr(char *dst, const char *arg1, const char *arg2) {
	byte i;
	char tmp1[16 + 1];
	char tmp2[16 + 1];

	VERIFY(dst);
	VERIFY(arg1);
	VERIFY(arg2);
	VERIFY(strlen(arg1) <= 16);
	VERIFY(strlen(arg2) <= 16);

	strcpy(tmp1, arg1);
	neg2sup(tmp1);

	strcpy(tmp2, arg2);
	neg2sup(tmp2);

	i = addSup(dst, tmp1, tmp2);
	sup2neg(dst);
	return i;
}

/**
 * Subtract the content of the arg2 from the content of arg1 as strings of decimal digits and write the result into dst.
 * All the strings should be no more than 16 characters length.
 * The negative value strings should start by ‘-‘.
 *
 * \param dst (O) Destination string
 * \param       arg1 (I) First argument
 * \param       arg2 (I) Second argument
 *
 * \pre
 *    - dst!=0
 *    - arg1!=0
 *    - arg2!=0
 *    - The destination buffer should be large enough to accept 16-character long string (16+1)
 *
 * \return
 *    - strlen(dst)
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte subStr(char *dst, const char *arg1, const char *arg2) {
	byte i;
	char tmp1[16 + 1];
	char tmp2[16 + 1];

	VERIFY(dst);
	VERIFY(arg1);
	VERIFY(arg2);
	VERIFY(strlen(arg1) <= 16);
	VERIFY(strlen(arg2) <= 16);

	strcpy(tmp1, arg1);
	neg2sup(tmp1);

	strcpy(tmp2, arg2);
	neg2sup(tmp2);
	negStr(tmp2);

	i = addSup(dst, tmp1, tmp2);
	sup2neg(dst);
	return i;
}

/**
 * \todo To be documented
 */
card stdPow(int num, int exp) {
	int i;
	card c = 1;

	for (i = 1; i <= exp; i++, c *= num);
	return c;
}

/** Calculate checksum lrc for the buffer buf of length len.
 * LRC is the result of XORing of all the bytes in buf.
 * This checksum is used in BASE24-like messages.
 * \param buf (O) String on which the LRC should be calculated
 * \param       len (I) Number of bytes in the buf
 * \pre
 *    - buf!=0
 * \return
 *    - lrc checksum calculated
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
byte stdLrc(const byte * buf, int len) {
	byte lrc;

	VERIFY(buf);
	lrc = 0;
	while(len--)
		lrc ^= *buf++;
	return lrc;
}

/** Calculate checksum lrc for the buffer buf of length len.Buf is track2 data including STX and ETX.
 * LRC is the result of XORing of all the bytes in buf.
 * bit 7,6,and 5 must be kept 011 according to APACS 30 common attachment and bit 8 set to zero to ignore parity.
 * \param buf (I) String on which the LRC should be calculated
 * \param len (I) Number of bytes in the buf
 * \pre
 *    - buf!=0
 * \return
 *    - lrc checksum calculated
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcnl0028.c
 */
byte trk2Lrc(const byte * buf, int len) {
	byte lrc;
	byte a = 0;

	VERIFY(buf);
	lrc = 0;
	while(len--) {
		a = *buf;
		lrc ^= a;
		buf++;
	}
	lrc = lrc & 0x0F;
	lrc = lrc + 0x30;
	return lrc;

}

/** Extract account number from pan for ANSIX 9.8 pin calculation.
 * It is the last 12 pan digits not including Luhn code preceded by 0000 account
 * is converted into binary.
 * \param       acc (O) Account to be calculated
 * \param       pan (I) Original pan number
 * \pre
 *    - acc!=0
 *    - pan!=0
 *    - the length of pan should be greater than or equal to 12
 * \return
 *    - 8 if OK.
 *    - negative otherwise (short pan).
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 */
#define lenAcc 12
byte stdGetAcc(byte * acc, const char *pan) {
	int ret;
	byte idx;
	char asc[lenAcc + 1];       //account in ASCII

	VERIFY(acc);
	VERIFY(pan);
	VERIFY(strlen(pan));

	memset(acc, 0, 8);

	idx = strlen(pan) - 1;      //non include Luhn code
	if(idx < lenAcc)
		return 0;               //pan too short

	idx -= lenAcc;
	ret = fmtSbs(asc, pan, idx, lenAcc);
	VERIFY(ret == lenAcc);
	VERIFY(strlen(asc) == lenAcc);

	ret = hex2bin(acc + 2, asc, lenAcc / 2);
	VERIFY(ret == (lenAcc / 2));

	return (byte) ret;
}

/** Verify PAN according to the Luhn check digit, algorithm ANSI  X4.13
 * \param pan (I) pan number
 * \pre
 *    - pan!=0
 * \return
 *    - 1 if OK.
 *    - 0 otherwise.
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcab0053.c
 */
byte stdLuhnCheck(const char *pan) {
	byte idx;                   //current character index
	byte len;                   //pan length
	byte sum;                   //check sum
	byte odd;                   //odd/even flag
	byte tmp;

	VERIFY(pan);

	len = strlen(pan);
	if(!len)
		return 0;               //empty pan
	idx = len;
	sum = 0;
	odd = 1;
	while(idx--) {
		if(pan[idx] < '0')
			return 0;           //invalid character
		if(pan[idx] > '9')
			return 0;           //invalid character
		tmp = pan[idx] - '0';
		if(!odd) {
			tmp += tmp;
			tmp = (tmp % 10) + (tmp / 10);
		}
		sum += tmp;
		odd = !odd;
	}
	if(sum % 10 == 0)
		return 1;
	return 0;
}

/**
 * \todo To be documented
 */
void stdArgMsgCrc16(word * crc_val) {
	word i = 0, flag;

	for (i = 0; i < 16; i++) {
		flag = (crc_val[0] & 0x8000);
		crc_val[0] <<= 1;
		if(flag)
			crc_val[0] ^= 0x1021;
	}
}

/** Crc16(Xmodem) calculation according to CCITT-1021
 * \todo To be documented
 * \test tcnl0015.C
 */
word stdCrc16(const byte * buf, word len, byte augment_message_for_crc) {

	word crc_val = 0x0000;
	word i = 0;
	word shifter, flag;

	crc_val = 0;
	do {
		for (shifter = 0x80; shifter; shifter >>= 1) {
			flag = ((crc_val) & 0x8000);
			(crc_val) <<= 1;
			(crc_val) |= ((shifter & buf[i]) ? 1 : 0);
			if(flag)
				(crc_val) ^= 0x1021;
		}

	} while(++i < len);
	if(augment_message_for_crc) {
		stdArgMsgCrc16(&crc_val);
	}

	return crc_val;
}

/** XOR len byte of b1 and b2 to obtain dst.
 * \param b1 (I) Binary buffer 1
 * \param b2 (I) Binary buffer 2
 * \param dst (O) Destination buffer containing b1 XOR b2
 * \param len (I) Len byte to XOR
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\tbx.c
 * \test tcab0057.c
 */
void xor(const byte * b1, const byte * b2, byte * dst, word len) {
	VERIFY(b1);
	VERIFY(b2);
	VERIFY(dst);
	while(len--)
		*dst++ = *b1++ ^ *b2++;
}

/** Set parity bits; par=1..8 is number of parity bit.
 * \param buf (I) Binary buffer to be set
 * \param dim (I) Buffer size
 * \param par (I) Position of the parity bit 1 or 8
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdParity(byte * buf, word dim, byte par) {    //set parity bits
	byte cnt, idx;

	VERIFY(buf);
	while(dim--) {
		cnt = 0;
		for (idx = 1; idx < 8; idx++) {
			if(idx == par)
				continue;
			if(bitTest(buf, idx))
				cnt++;
		}
		if(cnt % 2)
			bitOff(buf, par);
		else
			bitOn(buf, par);
		buf++;
	}
}

static void justdes2(byte * dst, const byte * src, const byte * key, int ctl) {
	card x, c, d, r, l;
	int i;
	static const card s[64] = { /* S-tables and permutation E combined */
			0xD8D8DBBC, 0xD737D1C1, 0x8B047441, 0x35A9E2FE,
			0x146E9560, 0x8A420CFB, 0xF8FBAF1F, 0xC7B4DD10,
			0x7A97A497, 0x4CFCFA1C, 0x456ADA86, 0xFAC710E9,
			0xE52149EF, 0x338D2004, 0x1E5580F1, 0xE04A2F3D,
			0x870A4E20, 0x28BE9C1F, 0x74D5E339, 0x8240BD00,
			0x6AA1ABC3, 0x3F55E2A8, 0xAF1F56BC, 0x51BB11CF,
			0xB7FC035E, 0xE00307B0, 0x08A3B44B, 0x3F786D67,
			0x09967CBC, 0x45EB7B47, 0xF3683962, 0x9C14C6D2,
			0x16452B42, 0xADDACEBA, 0x58F91ABC, 0x8B68B547,
			0xFAA36659, 0x47BF8901, 0x671AEBA9, 0x30C452AB,
			0x493893E1, 0x72C16866, 0xB7C78574, 0xCD1E6B9A,
			0xB6DCD49C, 0x9822B7FB, 0x89B07E43, 0x77B78644,
			0xA566F5DF, 0xD22D6AC3, 0xAF9A0423, 0x77B71BBC,
			0x81DC043E, 0xC8837314, 0x78659153, 0xAF782C7D,
			0x8C0F78A0, 0x0D3095EF, 0x7A506B8E, 0x8445D610,
			0x5223AB47, 0x724C0C34, 0x45AF54BC, 0x38DBF9CB
	};

	/* initial permutations IP and PC1 */
	l = r = c = d = 0;
	i = 7;
	do {
		x = src[i];
		l = l << 1 | (x & 1 << 0) | (x & 1 << 2) << 6 | (x & 1 << 4) << 12 | (x
				&
				1
				<<
				6)
				<< 18;
		r = r << 1 | (x & 1 << 1) >> 1 | (x & 1 << 3) << 5 | (x & 1 << 5) << 11
				| (x & 1 << 7) << 17;
		x = key[i];
		c = c << 1 | (x & 1 << 7) << 17 | (x & 1 << 6) << 10 | (x & 1 << 5) << 3
				| (x & 1 << 4) >> 4;
		d = d << 1 | (x & 1 << 1) << 19 | (x & 1 << 2) << 10 | (x & 1 << 3) <<
				1;
	} while(--i >= 0);
	d |= c & 0x0F;
	c >>= 4;
	i = 24;
	/* first round is special: one left shift on encipher, no shift en decipher */
	if(ctl != 0)
		goto startround;
	leftby1:
	c = c << 1 | ((c >> 27) & 1);
	d = d << 1 | ((d >> 27) & 1);
	startround:
	/* a round - apply PC2, the S-boxes and permutation E */
	x = (s[((r >> 26 & 62) | (r & 1)) ^
		   ((c >> 6 & 32) | (c >> 13 & 16) | (c >> 1 & 8) | (c >> 25 & 4) |
				   (c >> 22 & 2) | (c >> 14 & 1))] & 0x00808202) ^ l;
	x ^= s[(r >> 23 & 63) ^
		   ((c >> 20 & 32) | (c << 4 & 16) | (c >> 10 & 8) | (c >> 20 & 4) |
				   (c >> 6 & 2)
				   | (c >> 18 & 1))] & 0x40084010;
	x ^= s[(r >> 19 & 63) ^
		   ((c & 32) | (c >> 5 & 16) | (c >> 13 & 8) | (c >> 22 & 4) |
				   (c >> 1 & 2) | (c >> 20 & 1))] & 0x04010104;
	x ^= s[(r >> 15 & 63) ^
		   ((c >> 7 & 32) | (c >> 17 & 16) | (c << 2 & 8) | (c >> 6 & 4) |
				   (c >> 14 & 2) | (c >> 26 & 1))] & 0x80401040;
	x ^= s[(r >> 11 & 63) ^
		   ((d >> 10 & 32) | (d & 16) | (d >> 22 & 8) | (d >> 17 & 4) |
				   (d >> 8 & 2) | (d >> 1 & 1))] & 0x21040080;
	x ^= s[(r >> 7 & 63) ^
		   ((d >> 21 & 32) | (d >> 12 & 16) | (d >> 2 & 8) | (d >> 9 & 4) |
				   (d >> 22 & 2)
				   | (d >> 8 & 1))] & 0x10202008;
	x ^= s[(r >> 3 & 63) ^
		   ((d >> 7 & 32) | (d >> 3 & 16) | (d >> 14 & 8) | (d << 2 & 4) |
				   (d >> 21 & 2) | (d >> 3 & 1))] & 0x02100401;
	x ^= s[((r & 31) | (r >> 26 & 32)) ^
		   ((d >> 19 & 32) | (d >> 6 & 16) | (d >> 11 & 8) | (d >> 4 & 4) |
				   (d >> 19 & 2)
				   | (d >> 27 & 1))] & 0x08020820;
	l = r;
	r = x;
	/* decide/perform key shifts */
	if((i & 7) == 0) {
		i -= 2;
		if(ctl == 0)
			goto leftby1;
		c = c >> 1 | (c & 1) << 27;
		d = d >> 1 | (d & 1) << 27;
		goto startround;
	}
	if(i != 6) {
		--i;
		if(ctl == 0) {
			c = c << 2 | (c >> 26 & 3);
			d = d << 2 | (d >> 26 & 3);
			goto startround;
		} else {
			c = c >> 2 | (c & 3) << 26;
			d = d >> 2 | (d & 3) << 26;
			goto startround;
		}
	}
	/* final swap and permutations IP` */
	i = 7;
	do {
		*dst++ =
				(byte) ((r & 1 << 0) | (r >> 6 & 1 << 2) | (r >> 12 & 1 << 4) |
						(r >> 18 & 1 << 6) | (l << 1 & 1 << 1) | (l >> 5 & 1 << 3) |
						(l >> 11 & 1 << 5) | (l >> 17 & 1 << 7));
		l >>= 1;
		r >>= 1;
	} while(--i >= 0);
}

/** Encrypt the binary array src using the key and put the result into the buffer dst.
 * The standard DES algorithm is used for encryption.
 * \param dst (O) Destination binary buffer.
 * \param src (I) Binary buffer to be encrypted.
 * \param key (I) DES encryption key.
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \remark all the buffer are 8 bytes long.
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdDES(byte * dst, byte * src, byte * key) {
	justdes2(dst, src, key, 0);
}

/** Decrypt the binary array src using the key key and put the result into the buffer dst.
 * The standard DES algorithm is used for encryption.
 * \param dst (O) Destination binary buffer.
 * \param       src (I) Binary buffer to be encrypted.
 * \param       key (I) DES decryption key.
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \remark all the buffer are 8 bytes long.
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdSED(byte * dst, byte * src, byte * key) {
	justdes2(dst, src, key, 1);
}

/** Triple DES encryption of  binary array src by double length key;
 * the result is saved into dst.
 * \param       dst (O) Destination binary buffer.
 * \param       src (I) Binary buffer to be encrypted.
 * \param       key (I) Double length DES encryption key.
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdEDE(byte * dst, byte * src, byte * key) {
	byte tmp[8];

	stdDES(dst, src, key);
	stdSED(tmp, dst, key + 8);
	stdDES(dst, tmp, key);
}

/** Triple DES decryption of  binary array src by double length key;
 * the result is saved into dst.
 * \param dst (O) Destination binary buffer.
 * \param       src (I) Binary buffer to be decrypted.
 * \param       key (I) Double length DES decryption key.
 * \pre
 *    - dst!=0
 *    - src!=0
 *    - key!=0
 * \return no
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
void stdDED(byte * dst, byte * src, byte * key) {
	byte tmp[8];

	stdSED(dst, src, key);
	stdDES(tmp, dst, key + 8);
	stdSED(dst, tmp, key);
}

//Integer hash function for 32 bits based on Robert Jerkins' 32 bit Mix Function.
//http://www.concentric.net/~Ttwang/tech/inthash.htm
static card hashJerkins(card key) {
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);
	return key;
}

/** Hash function calculated on a binary buffer
 * \param buf binary buffer to calculate hash
 * \param len buffer length
 * \return
 *    - output integer hash value.
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcts0002.c
 */
card hashJerkinsBin(const byte * buf, byte len) {
	card ret;
	card *ptr;
	card tmp[256 / sizeof(card)];

	VERIFY(len);

	memset(tmp, 0xFF, sizeof(tmp));
	memcpy(tmp, buf, len);

	ret = 0;
	ptr = tmp;
	len += 3;
	len /= 4;
	while(len) {
		ret ^= hashJerkins(*ptr);
		ptr++;
		len--;
	}

	return ret;
}

enum {                          //bmp header structure
	ofsBmpHdrId1,               //Production header ID1
	ofsBmpHdrId2,               //Production header ID2
	ofsBmpHdrId3,               //Production header ID3
	ofsBmpChkSum,               //Checksum of header
	ofsBmpHdrLen,               //Length of header
	ofsBmpInvLen,               //Inverted HdrLen
	ofsBmpHdrVer,               //Header version number
	ofsBmpLen1,                 //BMP len 1 (MSB)
	ofsBmpLen2,                 //BMP len 2
	ofsBmpLen3,                 //BMP len 3
	ofsBmpLen4,                 //BMP len 4 (LSB)
	ofsBmpWdtMsb,               //MSB of Width
	ofsBmpWdtLsb,               //LSB of Width
	ofsBmpHgtMsb,               //MSB of Height
	ofsBmpHgtLsb,               //LSB of Height
	ofsBmpBpp,                  //Bits per pixel
	ofsBmpDatChk,               //BMP Data checksum
	ofsBmpDat                   //data follow
};

word bmpWdt(const byte * bmp) {
	return WORDHL(bmp[ofsBmpWdtMsb], bmp[ofsBmpWdtLsb]);
}
word bmpHgt(const byte * bmp) {
	return WORDHL(bmp[ofsBmpHgtMsb], bmp[ofsBmpHgtLsb]);
}
const byte *bmpDat(const byte * bmp) {
	return bmp + ofsBmpDat;
}

/** Converting three bytes to four characters in Base64 representation.
 * \param inBuf (I) The Data that will be encoded
 * \param inLen (I) The length of the data
 * \param outBuf (O)The Encoded output buffer
 * \return
 *    - Length of the Encoded output buffer.
 *
 * For the Binary data/printable string conversion, there are several usable methods. One of the
 * most frequently used is the Base64 algorithm described in the RCF1341 standard. Base64 is
 * sometimes called Radix64 coding, because it uses the 4 byte transformation system to four "6 bit"
 * characters. These characters are transformed to a printable form using a special translation table.
 *
 * The BASE64 coding system uses the following table:
 * code_table[0..63] = "ABCD...XYZabcd...xyz0123...789+/"
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcfa0004.c
 */
int Radix64Encode(byte * inBuf, word inLen, byte * outBuf) {
	byte bintoasc[64] = {
			'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
			'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
			'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
			'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
	};

	int i;
	unsigned char *p = outBuf;

	// Transform the 3x8 bits to 4x6 bits, as required by base64.
	for (i = 0; i < inLen; i += 3) {
		*p++ = bintoasc[inBuf[0] >> 2];
		*p++ = bintoasc[((inBuf[0] & 3) << 4) + (inBuf[1] >> 4)];
		*p++ = bintoasc[((inBuf[1] & 0xf) << 2) + (inBuf[2] >> 6)];
		*p++ = bintoasc[inBuf[2] & 0x3f];
		inBuf += 3;
	}
	// Pad the result if necessary...
	if(i == inLen + 1)
		*(p - 1) = '=';
	else if(i == inLen + 2)
		*(p - 1) = *(p - 2) = '=';

	// ...and zero-terminate it.
	*p = '\0';
	return strlen((char *) outBuf);
}

/** Converting four characters in Base64 format to three byte representation.
 * \param inBuf (I) The encoded input buffer
 * \param outBuf (O)The decoded output buffer
 * \param outLength (O)The length of decoded buffer
 * \return 1 if OK, otherwise negative
 *
 * For the Binary data/printable string conversion, there are several usable methods. One of the
 * most frequently used is the Base64 algorithm described in the RCF1341 standard. Base64 is
 * sometimes called Radix64 coding, because it uses the 4 byte transformation system to four "6 bit"
 * characters. These characters are transformed to a printable form using a special translation table.
 *
 * The BASE64 coding system uses the following table:
 * code_table[0..63] = "ABCD...XYZabcd...xyz0123...789+/"
 *
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcfa0004.c
 */
int Radix64Decode(byte * inBuf, byte * outBuf, word * outLength) {
	byte asctobin[256] = {
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
			0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
			0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
			0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24,
			0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
			0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 0xff, 0xff
	};

	int idx = 0;
	unsigned char c, c2, val = '\0';
	int n = 0;

	for (idx = 0, c = *inBuf; c; c = *(++inBuf), idx = (idx + 1) % 4) {
		if(c == '\n' || c == ' ' || c == '\r' || c == '\t')
			continue;

		if(c == '=') {          // pad character: stop
			if(idx == 1)
				outBuf[n++] = val;
			break;
		}

		if((c = asctobin[(c2 = c)]) == 255)
			return -1;
		//RadixError;

		switch (idx) {
		case 0:
			val = c << 2;
			break;
		case 1:
			val |= (c >> 4) & 3;
			outBuf[n++] = val;
			val = (c << 4) & 0xf0;
			break;
		case 2:
			val |= (c >> 2) & 15;
			outBuf[n++] = val;
			val = (c << 6) & 0xc0;
			break;
		case 3:
			val |= c & 0x3f;
			outBuf[n++] = val;
			break;
		}

	}
	*outLength = n;
	return 1;
}

/** Returns the index of a particular character(if it exists) in an array
 * used by BaseToDec to find face values
 * \param pString (I) The array where in the input character 'search'  is searched
 * \param search (O) The character to be searched
 * \returns the last character to be searched
 * \sa Source: pri\\std.c
 */
static int GetIndex(char *pString, char search) {
	int index = 0;

	while(*pString != (char) 0) //Loop will finish at null character if no match is found
	{
		if(*pString == search)
			break;
		pString++;
		index++;
	}
	return index;
}

/** Returns the decimal equivalent of a number with base less than or equal to 36
 * \param number (I) The number to be converted to decimal
 * \param base (O) The base of the number
 * \returns the last character to be searched
 * \sa Header: pri\\pri.h
 * \sa Source: pri\\std.c
 * \test tcma0020.c
 */
int BaseToDec(char *number, card base) {
	int NumLength;
	int PlaceValue = 1, total = 0;
	int i;

	//Symbols used to display a number correctly
	//Numbers over base 10 use letters to represent values over and equal to 10
	//It is able to increase the max no. of bases by adding other symbols

	char symbols[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const int MAX_BASE = 36;    //Highest base allowed (make sure there are enough symbols first!)

	if(base < 2 || base > (card) MAX_BASE)
		return 0;               //Failed

	NumLength = strlen(number);

	//Work out the place value of the first digit (base^length-1)
	for (i = 1; i < NumLength; i++) {
		PlaceValue *= base;
	}

	//For each digit, multiply by its place value and add to total
	for (i = 0; i < NumLength; i++) {
		total += GetIndex(symbols, *number) * PlaceValue;
		number++;
		PlaceValue /= base;     //Next digit's place value (previous/base)
	}
	return total;
}

/** One Way Function of binary array dat by simple length key;
 * the result is saved into dst.
 * \param dst (O) Destination binary buffer.
 * \param dat (I) Binary butter to be OWF.
 * \param key (I) Simple length DES encryption key.
 * \pre
 *    - dst!=0
 *    - dat!=0
 *    - key!=0
 * \return non-negative if OK.
 * \sa Header: sys\\sys.h
 * \sa Source: sys\\std.c
 * \test tcab0057.c
 */
int stdOWF(byte * dst, const byte * dat, const byte * key) {
	byte tmpK[8];
	byte tmpD[8];
	byte *owfX = (byte *) "\xA5\xC7\xB2\x82\x84\x76\xA8\x29";
	byte *owfY = (byte *) "\xB5\xE3\x7F\xC5\xD4\xF7\xA3\x93";

	xor(dat, owfX, tmpD, 8);
	xor(key, owfY, tmpK, 8);
	stdParity(tmpK, 8, 8);
	stdDES(dst, tmpD, tmpK);
	xor(dst, tmpD, dst, 8);
	return 1;
}

//Info regarding timestamp processing:
//timestamp definition: http://en.wikipedia.org/wiki/Timestamp
//UNIX time: the number of seconds since 00:00:00 UTC on January 1, 1970
//The Unix Time "Apocalypse": January 19, 2038 03:14:07 GMT
//Unix Timestamp Converter: http://www.4webhelp.net/us/timestamp.php
//Julian Day Converter: http://www.iasfbo.inaf.it/~mauro/JD/
//Julian Day http://en.wikipedia.org/wiki/Julian_day
//Calendar Algorithms: http://www.tondering.dk/claus/algo.html

/* See http://www.tondering.dk/claus/algo.html
 * date_to_jdn:
 * Calculates the Julian Day Number for a given date.
 * Input parameters:
 *     Calendar style ('j' for JULIAN or 'g' for GREGORIAN)
 *     Year (must be > -4800). The year 1 BC must be given as 0, the
 *         year 2 BC must be given as -1, etc.
 *     Month (1..12)
 *     Day (1..31)
 * Returns:
 *     Julian Day Number
 *
 * Reference: Section 2.16.1 of version 2.9 of the FAQ.
 */
static int date_to_jdn(int year, int month, int day) {
	int a = (14 - month) / 12;
	int y = year + 4800 - a;
	int m = month + 12 * a - 3;

	return day + (153 * m + 2) / 5 + y * 365 + y / 4 - y / 100 + y / 400 -
			32045;
}

/* See http://www.tondering.dk/claus/algo.html
 * jdn_to_date:
 * Calculates the date for a given Julian Day Number.
 * Input parameter:
 *     Calendar style ('j' for JULIAN or 'g' for GREGORIAN)
 *     Julian Day Number
 * Output parameters:
 *     Address of year. The year 1 BC will be stored as 0, the year
 *         2 BC will be stored as -1, etc.
 *     Address of month (1..12)
 *     Address of day (1..31)
 *
 * Reference: Section 2.16.1 of version 2.9 of the FAQ.
 */
static void jdn_to_date(int JD, int *year, int *month, int *day) {
	int a, b, c, d, e, m;

	a = JD + 32044;
	b = (4 * a + 3) / 146097;
	c = a - (b * 146097) / 4;

	d = (4 * c + 3) / 1461;
	e = c - (1461 * d) / 4;
	m = (5 * e + 2) / 153;

	*day = e - (153 * m + 2) / 5 + 1;
	*month = m + 3 - 12 * (m / 10);
	*year = b * 100 + d - 4800 + m / 10;
}

/** Convert Timestamp to DateTime.
 * \param ts (I) Timestamp value to be converted
 * \param YYMMDDhhmmss (O) Buffer to capture the returned DateTime
 * \header sys\\pri.h
 * \source sys\\std.c
 * \test tcmp0000.c
 *
 */
void ts2dt(card ts, char *YYMMDDhhmmss) {
	card jdnUnix = 2440588;     //julian day number for 01/01/1970
	card spm = 60;              //seconds per minute
	card mph = 60;              //minutes per hour
	card hpd = 24;              //hours per day
	card sph = spm * mph;       //seconds per hour
	card spd = sph * hpd;       //seconds per day
	card days, secs;
	int Y = 0;
	int M = 0;
	int Da = 0;
	int h = 0;
	int m = 0;
	int s = 0;

	VERIFY(YYMMDDhhmmss);
	days = ts / spd;            //number of days since 01/01/1970
	secs = ts % spd;            //number of seconds since midnight

	jdn_to_date(jdnUnix + days, &Y, &M, &Da);
	h = secs / sph;
	secs %= sph;
	m = secs / spm;
	s = secs % spm;

	Telium_Sprintf(YYMMDDhhmmss, "%02d%02d%02d%02d%02d%02d", Y % 100, M, Da, h, m, s);

}

/** Convert DateTime to Timestamp.
 * \param YYMMDDhhmmss (I) Buffer to capture the returned DateTime
 * \return timestamp /the number of seconds since first call or since 01/01/1970
 * \header sys\\pri.h
 * \source sys\\std.c
 * \test tcmp0000.c
 *
 */
card dt2ts(const char *YYMMDDhhmmss) {
	int ret;
	card Y, M, Da, h, m, s;
	card days, secs;
	const char *ptr;
	card jdnUnix = 2440588;     //julian day number for 01/01/1970
	card spm = 60;              //seconds per minute
	card mph = 60;              //minutes per hour
	card hpd = 24;              //hours per day
	card sph = spm * mph;       //seconds per hour
	card spd = sph * hpd;       //seconds per day

	VERIFY(YYMMDDhhmmss);

	ptr = YYMMDDhhmmss;
	dec2num(&Y, ptr, 2);
	ptr += 2;
	dec2num(&M, ptr, 2);
	ptr += 2;
	dec2num(&Da, ptr, 2);
	ptr += 2;
	dec2num(&h, ptr, 2);
	ptr += 2;
	dec2num(&m, ptr, 2);
	ptr += 2;
	dec2num(&s, ptr, 2);

	if(Y > 70)
		Y += 1900;
	else
		Y += 2000;
	days = date_to_jdn(Y, M, Da);
	VERIFY(days >= jdnUnix);
	days -= jdnUnix;
	secs = s + m * spm + h * sph;
	ret = days * spd + secs;

	return ret;
}

/**
 * This function counts the number occurrence of certain characters in a string
 * @param string
 * @param ch
 * @return the counts
 */
int count_chars(const char* string, char ch){
	int count = 0;
	int i;

	// We are computing the length once at this point
	// because it is a relatively lengthy operation,
	// and we don't want to have to compute it anew
	// every time the i < length condition is checked.
	int length = strlen(string);

	for (i = 0; i < length; i++) {
		if (string[i] == ch) {
			count++;
		}
	}

	return count;
}

/** Find a key subspace corresponding to a given key
 * \param  key (I) Key to be located.
 * \return
 *  - starting sentinel of a subspace that the key belongs to
 *  - negative if the input key is out of space
 *
 * Used by other map functions to identify a data structure which the
 * data element belongs to
 */
int begKey(word key){ //find starting sentinel of key subspace
	if(isSorted(appBeg,key,appEnd)) return appBeg; //application parameters record
	//    if(isSorted(mnuBeg,key,mnuEnd)) return mnuBeg; //menu tree
	//    if(isSorted(msgBeg,key,msgEnd)) return msgBeg; //screen messages
	//    if(isSorted(rptBeg,key,rptEnd)) return rptBeg; //receipt formatting strings
	//    if(isSorted(scrBeg,key,scrEnd)) return scrBeg; //user screen structure
	//    if(isSorted(regBeg,key,regEnd)) return regBeg; //registry record
	if(isSorted(traBeg,key,traEnd)) return traBeg; //transaction related data situated in volatile memory
	if(isSorted(emvBeg,key,emvEnd)) return emvBeg; //Get Data from the database Emv SQLITE table
	return -1;
}

/** Retrieve a data element from a data structure.
 * \param  key (I) Index of a data element.
 * \param  ptr (O) Pointer to buffer where to write the data to.
 * \param  len (I) Buffer length.
 * \return
 * - size of the data element retrieved
 * - negative if failure
 *
 * The parameter len ensures that the memory is not overwritten.
 * If it is too small the exception condition is arised.
 * Zero length is treated as field length.
 * This way of calling the function is not recommended.
 *
 * \pre ptr!=0
 * \pre key belongs to the key space
 *
 * At first the key subspace is identified that contains the data element.
 * Then the relative position of the key within the subspace is calculated.
 * Depending on the subspace the retrieval function is called for related descriptor.
 * \sa
 *  - mapTabGet()
 *  - mapRecGet()
 *  - mapPut()
 *  - mapMove()
 */
int beg;
int mapGet(word key,void *ptr,word len){
	VERIFY(ptr);
	VERIFY(isSorted(keyBeg,key,keyEnd)); //TODO: KevCode - Assertion fails

	beg = begKey(key);
	VERIFY(beg>0);

	//	key-= beg+1;
	switch(beg){
	case appBeg: return appGet(key,ptr,len);
	//        case mnuBeg: return mapTabGet(key,ptr,len);
	//        case msgBeg: return mapTabGet(key,ptr,len);
	//        case rptBeg: return mapTabGet(key,ptr,len);
	//        case scrBeg: return mapRecGet(key,ptr,len);
	//        case regBeg: return mapRecGet(key,ptr,len);
	case traBeg: return traGet(key,ptr,len);
	case emvBeg: return mapGet_AID_Data(key, ptr);
	default: break;
	}
	return -1;
}


/** Save a data element into a data structure.
 * \param  key (I) Index of a data element.
 * \param  ptr (I) Pointer to buffer containing the data to be saved.
 * \param  len (I) Buffer length.
 * \return
 * - size of the data element saved
 * - negative if failure
 *
 * \pre ptr!=0
 * \pre key belongs to the key space
 * At first the key subspace is identified that contains the data element.
 * Then the relative position of the key within the subspace is calculated.
 * Depending on the subspace the save function is called for related descriptor.
 *
 * If len is to big the buffer is truncated to the size of data element.
 * \sa
 *  - mapTabPut()
 *  - mapRecPut()
 *  - mapGet()
 *  - mapMove()
 */
int mapPut(word key,const void *ptr,word len){
	int beg;
	VERIFY(ptr);
	VERIFY(isSorted(keyBeg,key,keyEnd));  //TODO: Kevcode Assertion fails

	beg= begKey(key);
	VERIFY(beg>0);
	//    key-= beg+1; // dont understand the use of this (KevCode)
	if(!len) len= strlen((char *)ptr);

	if (len > mapDatLen(key)) {
		len = mapDatLen(key);
	}

	switch(beg){
	case appBeg: return appPut(key,ptr,len);
	//        case mnuBeg: return mapTabPut(key,ptr,len);
	//        case msgBeg: return mapTabPut(key,ptr,len);
	//        case rptBeg: return mapTabPut(key,ptr,len);
	//        case scrBeg: return mapRecPut(key,ptr,len);
	//        case regBeg: return mapRecPut(key,ptr,len);
	case traBeg: return traPut(key,ptr,len);
	case emvBeg: return len;// mapGet_AID_Data(key, ptr);
	default: break;
	}
	return -1;
}

/** Get system date and time in format YYMMDDhhmmss.
 * \param YYMMDDhhmmss (O) Buffer[12+1] to accept date and time retrieved.
 * \return non-negative value if OK; negative otherwise, Sagem terminals will not fail this function
 * \header sys\\sys.h
 * \source sys\\i32.c sys\\tlm.c
 * \test tcab0050.c
 */
int getDateTime(char *YYMMDDhhmmss) {
	unsigned long ldate;
	Telium_Date_t Date;
	int yr, mth, dy;
	int hr, min, sec;
	char DateTimeTra[lenDatTim +  5];

	memset(DateTimeTra, 0, sizeof(DateTimeTra));

	VERIFY(YYMMDDhhmmss);

	Telium_Read_date (&Date);

	ldate = d_tolong(&Date);
	VERIFY(ldate);

	dy = (Date.day[0] - '0') * 10 + Date.day[1] - '0';
	mth = (Date.month[0] - '0') * 10 + Date.month[1] - '0';
	yr = (Date.year[0] - '0') * 10 + Date.year[1] - '0';

	hr = (Date.hour[0] - '0') * 10 + Date.hour[1] - '0';
	min = (Date.minute[0] - '0') * 10 + Date.minute[1] - '0';
	sec = (Date.second[0] - '0') * 10 + Date.second[1] - '0';

	Telium_Sprintf(YYMMDDhhmmss, "%02d%02d%02d%02d%02d%02d", yr, mth, dy, hr, min,sec);
	Telium_Sprintf(DateTimeTra, "20%02d%02d%02d%02d%02d%02d", yr, mth, dy, hr, min,sec);
	mapPut(traDatTim, DateTimeTra, 14);

	return (1);
}

static void getMonthName(card monthNumber,char * monthname){
	switch (monthNumber) {
	case 1:
		strcpy(monthname,"JAN");
		break;
	case 2:
		strcpy(monthname,"FEB");
		break;
	case 3:
		strcpy(monthname,"MAR");
		break;
	case 4:
		strcpy(monthname,"APR");
		break;
	case 5:
		strcpy(monthname,"MAY");
		break;
	case 6:
		strcpy(monthname,"JUN");
		break;
	case 7:
		strcpy(monthname,"JUL");
		break;
	case 8:
		strcpy(monthname,"AUG");
		break;
	case 9:
		strcpy(monthname,"SEP");
		break;
	case 10:
		strcpy(monthname,"OCT");
		break;
	case 11:
		strcpy(monthname,"NOV");
		break;
	case 12:
		strcpy(monthname,"DEC");
		break;
	}
}

int strFormatDatTim(word key){
	int ret;
	card month;
	char datetime[lenDatTim + 1];
	char newdatetime[lenDatTim + 1];
	char fmtdatetime[lenDatTim + 1];
	const char *bgd = " ";
	const char *ctl = "ghabcdijkl";

	memset(newdatetime, 0, sizeof(newdatetime));
	memset(fmtdatetime, 0, sizeof(fmtdatetime));
	MAPGET(key, datetime, lblKO);

	dec2num(&month, &datetime[2], 2);
	ret = fmtMut(newdatetime, (const char *) datetime, bgd, ctl);

	CHECK(month <= 12, lblKO);
	CHECK(month > 0, lblKO);

	getMonthName(month,fmtdatetime);
	memcpy(&fmtdatetime[3], newdatetime, strlen(newdatetime));

	MAPPUTSTR(traFmtDate, fmtdatetime, lblKO);
	CHECK(ret > 0, lblKO);

	goto lblEnd;

	lblKO:
	ret = -1;

	lblEnd:
	return ret;
}



//! \brief Wait card removal after an EMV transaction.
void message_RemoveCard(void){
	int bCardPresent;
	Telium_File_t* hCam;
	unsigned char ucStatus;

	hCam = NULL;

	hCam = stdcam0();
	if (hCam==NULL) {
		hCam = Telium_Fopen("CAM0", "rw");
	}


	bCardPresent = TRUE;
	if (hCam != NULL) {
		Telium_Status(hCam, &ucStatus);
		if ((ucStatus & CAM_PRESENT) == 0) {
			if (hCam != NULL) {
				Telium_Fclose(hCam);
				hCam = NULL;
			}
			// A card has been removed
			bCardPresent = FALSE;
		} else {
			OpenPeripherals();
			// Display message "Remove card"
			GL_Dialog_Message(hGoal, NULL, "Remove card", GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
			ClosePeripherals();
		}


		while(bCardPresent) {
			Telium_Ttestall(CAM0, 0);
			Telium_Status(hCam, &ucStatus);

			if ((ucStatus & CAM_PRESENT) == 0) {
				if (hCam != NULL) {
					Telium_Fclose(hCam);
					hCam = NULL;
				}
				// A card has been removed
				bCardPresent = FALSE;
			}
		}
	}
}

/** Increments the passed parameter type entry with the max value of 999999
 */
int incCard(word key) {
	int ret;
	card buffer;

	MAPGETCARD(key, buffer, lblKO);
	if(buffer >= 999999)
		buffer = 0;
	buffer++;

	MAPPUTCARD(key, buffer, lblKO);
	goto lblEnd;

	lblKO:
	ret = -1;
	goto lblEnd;
	lblEnd:
	return ret;
}


int isReversibleSend(void) {
	int ret;
	card MnuItm;
	char menu[100];

	memset(menu, 0, sizeof(menu));

	MAPGET(traMnuItm, menu, lblKO);
	dec2num(&MnuItm, menu, 0);

	switch (MnuItm) {
	case mnuPreaut:
	case mnuRefund:
	case mnuVoid:
	case mnuAdjust:
	case mnuSale:
	case mnuSaleCB:
	case mnuWithdrawal:
	case mnuCompletion:
	case mnuReversal:   //In CUP there's a reversal of a reversal
		MAPPUTBYTE(appReversalFlag, 1, lblKO);
		ret = 1;
		break;
	case mnuSettlement:
	default:
		MAPPUTBYTE(appReversalFlag, 0, lblKO);
		ret = 0;
		break;
	}
	return ret;
	lblKO:
	return -1;
}


int isApproved(void) {
	char tcRspCod[lenRspCod + 1];
	const char *ptr;
	int iRet;

	iRet = mapGet(traRspCod, tcRspCod, lenRspCod + 1);    // Retrieve response code from DBA (record "tra" transaction stuff)
	CHECK(iRet >= 0, lblEnd);
	ptr = tcRspCod;

	iRet = 0;                   // Transaction rejected by default

	if(strcmp(ptr, "00") <= 0) { // Transaction approved by server
		iRet = 1;
	}

	goto lblEnd;

	lblEnd:
	return iRet;
}

card getCurrentMenu(void){
	card mnuItem = 0;
	char MENU[lenMnu + 1];
	int ret = 0;

	memset(MENU, 0, sizeof(MENU));

	memset(MENU, 0, sizeof(MENU));
	MAPGET(traMnuItm,MENU,lblKO);
	dec2num(&mnuItem, MENU, 0);

	return mnuItem;
	lblKO:
	return 0;
}

void strtouppercase(char s[]){
	int c = 0;

	while (s[c] != '\0') {
		if (s[c] >= 'a' && s[c] <= 'z') {
			s[c] = s[c] - 32;
		}
		c++;
	}
}


int ComputeTotAmt(void) {
	int ret;
	char amtS[lenAmt + 3];
	char amtOth[lenAmt + 3];
	char amtTip[lenAmt + 3];
	char amtTot[lenTotAmt + 1];

	memset(amtS, 0, sizeof(amtS));
	memset(amtOth, 0, sizeof(amtOth));
	memset(amtTip, 0, sizeof(amtTip));
	memset(amtTot, 0, sizeof(amtTot));

	MAPGET(traAmt, amtS, lblKO);
	MAPGET(traTipAmt, amtTip, lblKO);
	MAPGET(traOtherAmt, amtOth, lblKO);

	ret = sizeof(amtTot);
	VERIFY(ret >= 16 + 1);      //the length should be >= 16+1, see syntax of addStr
	memset(amtTot, 0, sizeof(amtTot));
	addStr(amtTot, amtS, amtTip);
	addStr(amtTot, amtTot, amtOth);


	CHECK(strlen(amtTot) <= lenAmt, lblKO);
	MAPPUTSTR(traTotAmt, amtTot, lblKO);

	goto lblEnd;

	lblKO:
	ret = 0;
	goto lblEnd;

	lblEnd:
	return ret;
}


/** Retrieve a key pressed if any. Return its key code.
 * \return
 *    - Key code according to eKbd enum.
 *    - If no key pressed zero value is returned.
 *    - In case of error it returns 0xFF.
 * \header sys\\sys.h
 * \source sys\\tlmhmi.c
 * \test tcab0003.c
 */
char kbdKey(void) {
	int ret;
	byte key = 0;

	ret = Telium_Ttestall(KEYBOARD, 1);    // wait for the first event

	if(ret & KEYBOARD) {
		key = Telium_Getchar();
	}
	return key;
}

