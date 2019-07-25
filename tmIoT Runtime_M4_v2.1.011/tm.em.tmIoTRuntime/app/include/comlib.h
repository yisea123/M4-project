/*
 * comlib.h
 *
 *  Created on: 2016Äê11ÔÂ22ÈÕ
 *      Author: pro
 */

#ifndef APP_INCLUDE_COMLIB_H_
#define APP_INCLUDE_COMLIB_H_

//#include "type.h"
#include "kernel/include/type.h"

WORD_T htons(WORD_T n);

WORD_T ntohs(WORD_T n);

DWORD_T htonl(DWORD_T n);

DWORD_T ntohl(DWORD_T n);

BYTE_T wtob_L( WORD_T n);
BYTE_T wtob_H( WORD_T n);
WORD_T btow(BYTE_T a, BYTE_T b);

WORD_T CRC_check(BYTE_T *data, BYTE_T length);

/*
 * Get word (2 BYTE_T) result value from data
 */
WORD_T fetchword(BYTE_T *data, int offset);

/*
 * Get double word (4 BYTE_T) result value from data
 */
DWORD_T fetchdword(BYTE_T *data, int offset);

WORD_T dwtow_L(DWORD_T dwValue);
WORD_T dwtow_H(DWORD_T dwValue);
#endif /* APP_INCLUDE_COMLIB_H_ */
