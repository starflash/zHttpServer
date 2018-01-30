//
// Created by zhangweiwen on 2016/8/19.
//

#ifndef ZHS_ZCOMMON_H
#define ZHS_ZCOMMON_H

#include <cassert>
#include <memory.h>
#include <cstdarg>
#include <cstdio>


#define SAFE_DELETE(x) { if (x) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_VECTOR(x) { if (x) { delete[] (x); (x) = nullptr; } }

/**
 * 在指定的地址构造对象，不分配内存。
 *
 * @tparam T  对象类型
 * @param ptr 地址
 */
template <typename T>
inline T* __construct(T *ptr)
{
    return new (static_cast<void *> (ptr)) T();
}

/**
 * 在指定的数组位置构造对象
 *
 * @tparam T  对象类型
 * @tparam S  数组大小
 * @param buf 数组地址
 * @return    地址
 */
template <typename T, size_t S>
inline T* __allocator(char (&buf)[S])
{
    return __construct(reinterpret_cast<T *> (buf));
}

/**
 * 在指定的数组位置构造对象
 *
 * @tparam T  对象类型
 * @tparam S  数组大小
 * @param buf 数组地址
 * @return    地址
 */
template <typename T, size_t S>
inline T* __allocator(unsigned char (&buf)[S])
{
    return __construct(reinterpret_cast<T *> (buf));
}

/**
 * strerror_r函数的扩展
 *
 * @param errnum 错误编号
 * @param buf    错误信息存储区
 * @param len    错误信息存储区长度
 */
inline void strerror_rx(int errnum, char *buf, size_t len)
{
#ifdef _GNU_SOURCE
    char *error_tmp = strerror_r(errnum, buf, len);
    strncpy(buf, error_tmp, len);
    buf[len - 1] = '\0';
#else
    strerror_r(errnum, buf, len);
#endif
}

/**
 * 根据格式和参数组装字符串
 *
 * @param str    输出字符串
 * @param len    输出字符串最大长度
 * @param format 格式
 * @param ...    参数
 * @return       >0: 生成的字符串长度
 *               -1: 错误
 */
inline int xsnprintf(char *str, const size_t len, const char *format, ...)
{
    memset(str, 0, len);

    va_list args;
    va_start(args, format);
    auto retval = vsnprintf(str, len - 1, format, args);
    va_end(args);

    return retval;
}

/**
 * 检查某个状态是否设置
 *
 * @param bit  状态数组
 * @param test 测试的值
 * @return     是否设置
 */
inline bool issetBit(const unsigned char *bit, const int test)
{
    return 0 != (bit[test / 8] & (0xff & (1 << (test % 8))));
}

/**
 * 设置某个状态
 *
 * @param bit  状态数组
 * @param test 设置的值
 */
inline void setBit(unsigned char *bit, const int test)
{
    bit[test / 8] |= (0xff & (1 << (test % 8)));
}

/**
 * 清除某个状态
 *
 * @param bit  状态数组
 * @param test 设置的值
 */
inline void clearBit(unsigned char *bit, const int test)
{
    bit[test / 8] &= (0xff & (~(1 << (test % 8))));
}

#endif //ZHS_ZCOMMON_H
