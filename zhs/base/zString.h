//
// Created by zhangweiwen on 2016/8/23.
//

#ifndef ZHS_ZSTRING_H
#define ZHS_ZSTRING_H

#include <string>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <cctype>

/**
 * 封装一些常用的字符串操作
 */
namespace Zhs
{
    /**
     * 把字符串根据分隔符转化为多个字符串
     *
     * @param container 容器，用于存放字符串。
     * @param input     输入字符串
     * @param token     分隔符
     * @param deep      分割的深度，缺省没有限制。
     */
    template <typename Container>
    inline void string_token(Container &container, const std::string &input,
                             const char * const token = "\t\n", const int deep = 0)
    {
        const std::string::size_type len = input.length();
        std::string::size_type i = 0;
        int count = 0;

        while (i < len) {
            i = input.find_first_not_of(token, i);
            if (i == std::string::npos) {
                return;
            }

            // find the end of the token
            std::string::size_type j = input.find_first_of(token, i);
            count++;

            // push token
            if (j == std::string::npos || (deep > 0 && count > deep)) {
                container.push_back(input.substr(i));
                return;
            }

            container.push_back(input.substr(i, j - i));


            // set up for next loop
            i = j + 1;
        }
    }

    /**
     * 把字符转化为小写的函数对象
     */
    struct Lower
    {
        char operator()(char c) const {
            return static_cast<char>(std::tolower(c));
        }
    };

    /**
     * 把字符串转化为小写
     *
     * @param str 需要转化的字符串
     */
    inline void strtolower(std::string &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), Lower());
    }

    /**
     * 把字符转化为大写的函数对象
     */
    struct Upper
    {
        char operator()(char c) const {
            return static_cast<char>(std::toupper(c));
        }
    };

    /**
     * 把字符串转化为大写
     *
     * @param str 需要转化的字符串
     */
    inline void strtoupper(std::string &str)
    {
        std::transform(str.begin(), str.end(), str.begin(), Upper());
    }

    //
    static const char xor_map[4] = {'#', '*', '!', 'N'};

    /**
     * 对字符串进行Base64编码
     *
     * @param input  输入字符串
     * @param output 输出字符串
     */
    inline void base64_encrypt(const std::string &input, std::string &output)
    {
        auto len = input.length();

        len = (((len + 3) / 4) * 4);

        char buffer[len];
        char dest[(len / 4) * 6 + 1]; // NULL terminate

        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, input.c_str());
        for (size_t i = 0; i < len; i++) {
            buffer[i] ^= ~xor_map[i % 4];
        }
        memset(dest, 0, sizeof(dest));
        for (size_t i = 0; i < len / 4; i++) {
            strcpy(&dest[i * 6], l64a(*(int*) (&buffer[i * 4])));
            for (size_t j = i * 6; j < i * 6 + 6; j++) {
                if (dest[j] == '\0') {
                    dest[j] = '.';
                }
            }
        }
        output = dest;
    }

    /**
     * 对字符串进行Base64解码
     *
     * @param input  输入字符串
     * @param output 输出字符串
     */
    inline void base64_decrypt(const std::string &input, std::string &output)
    {
        auto len = input.length();
        len = (((len + 5) / 6) * 6);

        char buffer[len];
        char dest[(len / 6) * 4 + 1]; // NULL terminate

        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, input.c_str());
        memset(dest, 0, sizeof(dest));
        for (size_t i = 0; i < len / 6; i++) {
            *((int*) (&dest[i * 4])) = static_cast<int>(a64l(&buffer[i * 6]));
        }
        for (size_t i = 0; i < (len / 6) * 4; i++) {
            dest[i] ^= ~xor_map[i % 4];
        }
        output = dest;
    }

    // 十六进制对应的字符
    static unsigned char hexchars[] = "0123456789ABCDEF";

    /**
	 * 对url特殊字符进行编码
     *
	 * @param in   输入字符串
	 * @param len  输入字符串长度
	 * @param rlen 输出字符串长度
	 * @return     输出编码后的url字符串，这段内存再使用完成以后需要释放。
	 */
    inline char* urlencode(const char *in, int len, int *rlen)
    {
        register int x, y;
        unsigned char *str;

        str = new unsigned char[3 * len + 1];
        for (x = 0, y = 0; len-- > 0; x++, y++) {
            str[y] = static_cast<unsigned char> (in[x]);
            if (str[y] == ' ') {
                str[y] = '+';
            } else if ((str[y] < '0' && str[y] != '-' && str[y] != '.')
                        || (str[y] < 'A' && str[y] > '9')
                        || (str[y] > 'Z' && str[y] < 'a' && str[y] != '_')
                        || (str[y] > 'z')) {
                str[y++] = '%';
                str[y++] = hexchars[static_cast<unsigned char> (in[x]) >> 4];
                str[y] = hexchars[static_cast<unsigned char> (in[x]) & 15];
            }
        }
        str[y] = '\0';
        if (rlen != nullptr) {
            *rlen = y;
        }

        return (char *) str;
    }

    /**
     * 对url特殊字符进行编码
     *
     * @param in 待编码的字符串，同时也作为输出。
     */
    inline void urlencode(std::string &in)
    {
        char *buf = urlencode(in.c_str(), static_cast<int>(in.length()), nullptr);

        if (buf != nullptr) {
            in = buf;
            free(buf);
        }
    }

    /**
     * 将十六进制字符串转化为整型
     *
     * @param str 待转换字符串
     * @return    转换后结果
     */
    inline static int htoi(char const *str)
    {
        int value;
        int c;

        c = ((unsigned char *)str)[0];
        if (isupper(c) != 0) {
            c = tolower(c);
        }
        value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

        c = ((unsigned char *)str)[1];
        if (isupper(c) != 0) {
            c = tolower(c);
        }
        value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

        return value;
    }

    /**
     * url字符串解码
     *
     * @param in   待解码的字符串，同时也作为输出。
     * @param len  待解码字符串的长度
     * @return     解码以后的字符串长度
     */
    inline int urldecode(char *in, int len)
    {
        char *dest = in;
        char *data = in;

        while (len-- > 0) {
            if (*data == '+') {
                *dest = ' ';
            } else if (*data == '%' && len >= 2
                                    && isxdigit(static_cast<int> (*(data + 1))) != 0
                                    && isxdigit(static_cast<int> (*(data + 2))) != 0) {
                *dest = static_cast<char> (htoi(data + 1));
                data += 2;
                len -= 2;
            } else {
                *dest = *data;
            }
            data++;
            dest++;
        }
        *dest = '\0';

        return static_cast<int>(dest - in);
    }

    /**
     * url字符串解码
     *
     * @param in 待解码的字符串，同时也作为输出。
     */
    inline void urldecode(std::string &in)
    {
        char buf[in.length() + 1];
        strcpy(buf, in.c_str());
        urldecode(buf, static_cast<int>(in.length()));
        in = buf;
    }
}

#endif //ZHS_ZSTRING_H
