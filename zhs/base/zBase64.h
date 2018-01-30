//
// Created by zhangweiwen on 17-4-29.
//

#ifndef ZHS_ZBASE64_H
#define ZHS_ZBASE64_H

#include <string>

#define CHARPAD '='
#define BADCHAR 0x01FFFFFF

/**
 * Given a source string of length len, this returns the amount of
 * memory the destination string should have.
 *
 * remember, this is integer math
 * 3 bytes turn into 4 chars
 * ceiling[len / 3] * 4 + 1
 *
 * +1 is for any extra null.
 */
#define modp_b64_encode_len(A) ((A+2)/3 * 4 + 1)

/**
 * Given a base64 string of length len,
 *   this returns the amount of memory required for output string
 *  It maybe be more than the actual number of bytes written.
 * NOTE: remember this is integer math
 * this allocates a bit more memory than traditional versions of b64
 * decode  4 chars turn into 3 bytes
 * floor[len * 3/4] + 2
 */
#define modp_b64_decode_len(A) (A / 4 * 3 + 2)

/**
 * Will return the strlen of the output from encoding.
 * This may be less than the required number of bytes allocated.
 *
 * This allows you to 'deserialized' a struct
 * \code
 * char* b64encoded = "...";
 * int len = strlen(b64encoded);
 *
 * struct datastuff foo;
 * if (modp_b64_encode_strlen(sizeof(struct datastuff)) != len) {
 *    // wrong size
 *    return false;
 * } else {
 *    // safe to do;
 *    if (modp_b64_decode((char*) &foo, b64encoded, len) == -1) {
 *      // bad characters
 *      return false;
 *    }
 * }
 * // foo is filled out now
 * \endcode
 */
#define modp_b64_encode_strlen(A) ((A + 2)/ 3 * 4)

namespace Zhs
{
    typedef unsigned char uint8_t;
    typedef unsigned int uint32_t;

    static const char e0[256] = {
        'A',  'A',  'A',  'A',  'B',  'B',  'B',  'B',  'C',  'C',
        'C',  'C',  'D',  'D',  'D',  'D',  'E',  'E',  'E',  'E',
        'F',  'F',  'F',  'F',  'G',  'G',  'G',  'G',  'H',  'H',
        'H',  'H',  'I',  'I',  'I',  'I',  'J',  'J',  'J',  'J',
        'K',  'K',  'K',  'K',  'L',  'L',  'L',  'L',  'M',  'M',
        'M',  'M',  'N',  'N',  'N',  'N',  'O',  'O',  'O',  'O',
        'P',  'P',  'P',  'P',  'Q',  'Q',  'Q',  'Q',  'R',  'R',
        'R',  'R',  'S',  'S',  'S',  'S',  'T',  'T',  'T',  'T',
        'U',  'U',  'U',  'U',  'V',  'V',  'V',  'V',  'W',  'W',
        'W',  'W',  'X',  'X',  'X',  'X',  'Y',  'Y',  'Y',  'Y',
        'Z',  'Z',  'Z',  'Z',  'a',  'a',  'a',  'a',  'b',  'b',
        'b',  'b',  'c',  'c',  'c',  'c',  'd',  'd',  'd',  'd',
        'e',  'e',  'e',  'e',  'f',  'f',  'f',  'f',  'g',  'g',
        'g',  'g',  'h',  'h',  'h',  'h',  'i',  'i',  'i',  'i',
        'j',  'j',  'j',  'j',  'k',  'k',  'k',  'k',  'l',  'l',
        'l',  'l',  'm',  'm',  'm',  'm',  'n',  'n',  'n',  'n',
        'o',  'o',  'o',  'o',  'p',  'p',  'p',  'p',  'q',  'q',
        'q',  'q',  'r',  'r',  'r',  'r',  's',  's',  's',  's',
        't',  't',  't',  't',  'u',  'u',  'u',  'u',  'v',  'v',
        'v',  'v',  'w',  'w',  'w',  'w',  'x',  'x',  'x',  'x',
        'y',  'y',  'y',  'y',  'z',  'z',  'z',  'z',  '0',  '0',
        '0',  '0',  '1',  '1',  '1',  '1',  '2',  '2',  '2',  '2',
        '3',  '3',  '3',  '3',  '4',  '4',  '4',  '4',  '5',  '5',
        '5',  '5',  '6',  '6',  '6',  '6',  '7',  '7',  '7',  '7',
        '8',  '8',  '8',  '8',  '9',  '9',  '9',  '9',  '+',  '+',
        '+',  '+',  '/',  '/',  '/',  '/'
    };

    static const char e1[256] = {
        'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',
        'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',
        'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',
        'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',
        'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',
        'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',
        '8',  '9',  '+',  '/',  'A',  'B',  'C',  'D',  'E',  'F',
        'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',
        'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',
        'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',
        'k',  'l',  'm',  'n',  'o',  'p',  'q',  'r',  's',  't',
        'u',  'v',  'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',
        '4',  '5',  '6',  '7',  '8',  '9',  '+',  '/',  'A',  'B',
        'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',
        'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',
        'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',  'e',  'f',
        'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',  'p',
        'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
        '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',
        '+',  '/',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',
        'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',
        'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',
        'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',
        'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',
        'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',
        '6',  '7',  '8',  '9',  '+',  '/'
    };

    static const char e2[256] = {
        'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',
        'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',
        'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',
        'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',
        'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',
        'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',
        '8',  '9',  '+',  '/',  'A',  'B',  'C',  'D',  'E',  'F',
        'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',
        'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',
        'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',
        'k',  'l',  'm',  'n',  'o',  'p',  'q',  'r',  's',  't',
        'u',  'v',  'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',
        '4',  '5',  '6',  '7',  '8',  '9',  '+',  '/',  'A',  'B',
        'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',
        'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',
        'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',  'e',  'f',
        'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',  'p',
        'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
        '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',
        '+',  '/',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',
        'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',
        'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',
        'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',
        'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',
        'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',
        '6',  '7',  '8',  '9',  '+',  '/'
    };

    /* SPECIAL DECODE TABLES FOR LITTLE ENDIAN (INTEL) CPUS */

    static const uint32_t d0[256] = {
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x000000f8, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
        0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
        0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
        0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
        0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
        0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
        0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
        0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
        0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
        0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
        0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
        0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
    };

    static const uint32_t d1[256] = {
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x0000e003, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
        0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
        0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
        0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
        0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
        0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
        0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
        0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
        0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
        0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
        0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
        0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
    };

    static const uint32_t d2[256] = {
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x00800f00, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
        0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
        0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
        0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
        0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
        0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
        0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
        0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
        0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
        0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
        0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
        0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
    };

    static const uint32_t d3[256] = {
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x003e0000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
        0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
        0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
        0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
        0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
        0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
        0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
        0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
        0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
        0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
        0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
        0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
        0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
    };

    extern "C" {

    /**
     * Encode a raw binary string into base 64.
     * src contains the bytes
     * len contains the number of bytes in the src
     * dest should be allocated by the caller to contain
     *   at least modp_b64_encode_len(len) bytes (see below)
     *   This will contain the null-terminated b64 encoded result
     * returns length of the destination string plus the ending null byte
     *    i.e.  the result will be equal to strlen(dest) + 1
     *
     * Example
     *
     * \code
     * char* src = ...;
     * int srclen = ...; //the length of number of bytes in src
     * char* dest = (char*) malloc(modp_b64_encode_len);
     * int len = modp_b64_encode(dest, src, sourcelen);
     * if (len == -1) {
     *   printf("Error\n");
     * } else {
     *   printf("b64 = %s\n", dest);
     * }
     * \endcode
     *
     */
    int modp_b64_encode(char* dest, const char* str, int len)
    {
        int i;
        uint8_t* p = (uint8_t*) dest;

        /* unsigned here is important! */
        uint8_t t1, t2, t3;

        for (i = 0; i < len - 2; i += 3) {
            t1 = str[i]; t2 = str[i+1]; t3 = str[i+2];
            *p++ = e0[t1];
            *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *p++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
            *p++ = e2[t3];
        }

        switch (len - i) {
            case 0:
                break;
            case 1:
                t1 = str[i];
                *p++ = e0[t1];
                *p++ = e1[(t1 & 0x03) << 4];
                *p++ = CHARPAD;
                *p++ = CHARPAD;
                break;
            default: /* case 2 */
                t1 = str[i]; t2 = str[i+1];
                *p++ = e0[t1];
                *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
                *p++ = e2[(t2 & 0x0F) << 2];
                *p++ = CHARPAD;
        }

        *p = '\0';
        return p - (uint8_t*)dest;
    }

    /**
     * Decode a base64 encoded string
     *
     * src should contain exactly len bytes of b64 characters.
     *     if src contains -any- non-base characters (such as white
     *     space, -1 is returned.
     *
     * dest should be allocated by the caller to contain at least
     *    len * 3 / 4 bytes.
     *
     * Returns the length (strlen) of the output, or -1 if unable to
     * decode
     *
     * \code
     * char* src = ...;
     * int srclen = ...; // or if you don't know use strlen(src)
     * char* dest = (char*) malloc(modp_b64_decode_len(srclen));
     * int len = modp_b64_decode(dest, src, sourcelen);
     * if (len == -1) { error }
     * \endcode
     */
    int modp_b64_decode(char* dest, const char* src, int len)
    {
        if (len == 0) return 0;
        /*
         * if padding is used, then the message must be at least
         * 4 chars and be a multiple of 4
         */
        if (len < 4 || (len % 4 != 0)) return -1; /* error */
        /* there can be at most 2 pad chars at the end */
        if (src[len-1] == CHARPAD) {
            len--;
            if (src[len -1] == CHARPAD) {
                len--;
            }
        }

        int i;
        int leftover = len % 4;
        int chunks = (leftover == 0) ? len / 4 - 1 : len /4;

        uint8_t* p = (uint8_t*)dest;
        uint32_t x = 0;
        uint32_t* destInt = (uint32_t*) p;
        uint32_t* srcInt = (uint32_t*) src;
        uint32_t y = *srcInt++;
        for (i = 0; i < chunks; ++i) {
            x = d0[y & 0xff] |
                d1[(y >> 8) & 0xff] |
                d2[(y >> 16) & 0xff] |
                d3[(y >> 24) & 0xff];

            if (x >= BADCHAR) return -1;
            *destInt = x ;
            p += 3;
            destInt = (uint32_t*)p;
            y = *srcInt++;}


        switch (leftover) {
            case 0:
                x = d0[y & 0xff] |
                    d1[(y >> 8) & 0xff] |
                    d2[(y >> 16) & 0xff] |
                    d3[(y >> 24) & 0xff];

                if (x >= BADCHAR) return -1;
                *p++ =  ((uint8_t*)(&x))[0];
                *p++ =  ((uint8_t*)(&x))[1];
                *p =    ((uint8_t*)(&x))[2];
                return (chunks+1)*3;
                break;
            case 1:  /* with padding this is an impossible case */
                x = d0[y & 0xff];
                *p = *((uint8_t*)(&x)); // i.e. first char/byte in int
                break;
            case 2: // * case 2, 1  output byte */
                x = d0[y & 0xff] | d1[y >> 8 & 0xff];
                *p = *((uint8_t*)(&x)); // i.e. first char
                break;
            default: /* case 3, 2 output bytes */
                x = d0[y & 0xff] |
                    d1[y >> 8 & 0xff ] |
                    d2[y >> 16 & 0xff];  /* 0x3c */
                *p++ =  ((uint8_t*)(&x))[0];
                *p =  ((uint8_t*)(&x))[1];
                break;
        }

        if (x >= BADCHAR) return -1;

        return 3*chunks + (6*leftover)/8;
    }
    }

    inline std::string& modp_b64_encode(std::string& s)
    {
        std::string x(modp_b64_encode_len(s.size()), '\0');
        int d = modp_b64_encode(const_cast<char*>(x.data()), s.data(), s.size());
        x.erase(d, std::string::npos);
        s.swap(x);
        return s;
    }

    /**
     * base 64 decode a string (self-modifing)
     * On failure, the string is empty.
     *
     * This function is for C++ only (duh)
     *
     * \param[in,out] s the string to be decoded
     * \return a reference to the input string
     */
    inline std::string& modp_b64_decode(std::string& s)
    {
        std::string x(modp_b64_decode_len(s.size()), '\0');
        int d = modp_b64_decode(const_cast<char*>(x.data()), s.data(), s.size());
        if (d < 0) {
            x.clear();
        } else {
            x.erase(d, std::string::npos);
        }
        s.swap(x);
        return s;
    }

    // Encodes the input string in base64.  Returns true if successful and false
    // otherwise.  The output string is only modified if successful.
    bool base64_encode(const std::string& input, std::string *output)
    {
        std::string temp;
        temp.resize(modp_b64_encode_len(input.size()));  // makes room for null byte

        // null terminates result since result is base64 text!
        int input_size = static_cast<int> (input.size());
        int output_size= modp_b64_encode(&(temp[0]), input.data(), input_size);
        if (output_size < 0) {
            return false;
        }

        temp.resize(output_size);  // strips off null byte
        output->swap(temp);

        return true;
    }

    // Decodes the base64 input string.  Returns true if successful and false
    // otherwise.  The output string is only modified if successful.
    bool base64_decode(const std::string& input, std::string *output) {
        std::string temp;
        temp.resize(modp_b64_decode_len(input.size()));

        // does not null terminate result since result is binary data!
        int input_size = static_cast<int> (input.size());
        int output_size = modp_b64_decode(&(temp[0]), input.data(), input_size);
        if (output_size < 0) {
            return false;
        }

        temp.resize(output_size);
        output->swap(temp);

        return true;
    }
}


#endif //ZHS_ZBASE64_H
