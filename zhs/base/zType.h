//
// Created by zhangweiwen on 2016/8/23.
//

#ifndef ZHS_ZTYPE_H
#define ZHS_ZTYPE_H

#include <cstring>
#include <crypto++/config.h>
#include "zCommon.h"

/**
 * 单字节无符号整数
 */
typedef unsigned char BYTE;

/**
 * 双字节无符号整数
 */
typedef unsigned short WORD;

/**
 * 双字节符号整数
 */
typedef signed short SWORD;

/**
 * 四字节无符号整数
 */
typedef unsigned int DWORD;

/**
 * 四字节符号整数
 */
typedef signed int SDWORD;

/**
 * 八字节无符号整数
 */
typedef unsigned long long QWORD;

/**
 * 八字节符号整数
 */
typedef signed long long SQWORD;

/**
 * DES密钥
 */
typedef byte DESKey[8];

/**
 * 默认DES密钥
 */
const DESKey DES_DEFAULT_KEY {};

/**
 * RC5密钥
 */
typedef byte RC5Key[16];

/**
 * 默认RC5密钥
 */
const RC5Key RC5_DEFAULT_KEY {28, 196, 25, 36, 193, 125, 86, 197, 35, 92, 194, 41, 31, 240, 37, 223};

#endif //ZHS_ZTYPE_H
