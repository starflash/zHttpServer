//
// Created by zhangweiwen on 17-2-11.
//

#include "zRegex.h"

const int zRegex::REG_UNKNOW(78325);
const int zRegex::REG_FLAGS(78326);
const int zRegex::REG_COMP(78327);
const int zRegex::REG_MATCH(78328);
const int zRegex::REG_MULTILINE(REG_NEWLINE);
const int zRegex::REG_DEFAULT(0);

/**
 * 构造函数 
 */
zRegex::zRegex()
{
    compiled = false;
    matched = false;
    errcode = REG_UNKNOW;
}

/**
 * 析构函数 
 */
zRegex::~zRegex()
{
    if (compiled) {
        regfree(&preg);
    }
}

/**
 * 正则表达式编译函数
 *
 * @param regex 要编译的正则表达式
 * @param flags 编译选项，目前支持REG_MULTILINE REG_DEFAULT，建议用默认值。
 * @return      编译是否成功
 */
bool zRegex::compile(const char *regex, int flags)
{
    if (compiled) {
        regfree(&preg);
        matched = false;
    }

    if (flags == REG_MULTILINE) {
        errcode = regcomp(&preg, regex, REG_EXTENDED);
    } else if (flags == REG_DEFAULT) {
        errcode = regcomp(&preg, regex, REG_EXTENDED | REG_NEWLINE);
    } else {
        errcode = REG_FLAGS;
    }

    compiled = (errcode == 0);

    return compiled;
}

/**
 * 匹配字符串，在匹配前请保证已经正确编译了正则表达式。
 *
 * @param str 要匹配的字符串
 * @return    匹配是否成功
 */
bool zRegex::match(const char *str)
{
    if (str == nullptr) {
        return false;
    }
    smatch = str;
    if (compiled) {
        errcode = regexec(&preg, str, 32, rgm, 0);
    } else {
        errcode = REG_COMP;
    }
    matched = (errcode == 0);

    return matched;
}

/**
 * 得到匹配的子字符串，在此之前请保证已经正确得进行匹配。
 *
 * @param sub 得到的子字符串
 * @param pos 子字符串的位置，注意匹配的字符串位置为0，其他子字符串以此类推，最大值为31。
 * @return    得到的子字符串
 */
std::string &zRegex::substr(std::string &sub, int pos)
{
    if (matched) {
        if (pos < 32 && pos >= 0 && rgm[pos].rm_so != -1) {
            sub = std::string(smatch, rgm[pos].rm_so, rgm[pos].rm_eo - rgm[pos].rm_so);
        } else {
            sub = "";
        }
    } else {
        errcode = REG_MATCH;
    }

    return sub;
}

/**
 * 返回错误信息
 *
 * @return 当进行编译或匹配时返回错误，可以用此得到错误信息。
 */
const std::string& zRegex::getError()
{
    if (errcode == REG_UNKNOW) {
        errstr = "unknow error";
    } else if (errcode == REG_FLAGS) {
        errstr = "flags error";
    } else if (errcode == REG_COMP) {
        errstr = "uncompiled error";
    } else if (errcode == REG_MATCH) {
        errstr = "unmatched error";
    } else {
        char buf[1024];
        regerror(errcode, &preg, buf, 1023);
        errstr.assign(buf);
    }

    return errstr;
}