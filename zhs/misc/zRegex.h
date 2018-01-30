//
// Created by zhangweiwen on 17-2-11.
//

#ifndef ZHS_ZREGEX_H
#define ZHS_ZREGEX_H

#include <string>
#include <regex.h>

/**
 * 正则表达式类，对regex进行了封装。
 * 支持子字符串匹配，但最多支持31个字串。
 * 非线程安全
 */
class zRegex
{
public:

    /**
     * 自定义标记: 支持多行匹配，默认不支持。
     */
    static const int REG_MULTILINE;

    /**
     * 自定义标记: 默认标记
     */
    static const int REG_DEFAULT;

    zRegex();
    ~zRegex();
    bool compile(const char *, int = REG_DEFAULT);
    bool match(const char *);
    std::string& substr(std::string&, int = 0);
    const std::string& getError();

private:

    /**
     * 错误信息存放处
     */
    std::string errstr;

    /**
     * 错误代码
     */
    int errcode;

    /**
     * 正则表达式句柄
     */
    regex_t preg;

    /**
     * 要匹配的字符串 
     */
    std::string smatch;

    /**
     * 表达式是否已编译 
     */
    bool compiled;

    /**
     * 是否匹配 
     */
    bool matched;

    /**
     * 子串匹配位置 
     */
    regmatch_t rgm[32];

    /**
     * 自定义错误代码: 标记错误
     */
    static const int REG_FLAGS;

    /**
     * 自定义错误代码: 未编译错误
     */
    static const int REG_COMP;

    /**
     * 自定义错误代码: 未知错误
     */
    static const int REG_UNKNOW;

    /**
     * 自定义错误代码: 未进行匹配错误
     */
    static const int REG_MATCH;
};


#endif //ZHS_ZREGEX_H
