//
// Created by zhangweiwen on 17-1-27.
//

#ifndef ZHS_ZARGS_H
#define ZHS_ZARGS_H

#include <argp.h>

#include "../base/zNoncopyable.h"

/**
 * 处理命令行参数
 * 由子类实现某个程序的默认参数
 */
class zArgs : private zNoncopyable
{
public:

    static zArgs* getInstance();
    static void destroyInstance();

    /**
     * 返回argp处理函数
     *
     * @return 处理argp的函数
     */
    argp_parser_t getParser()
    {
        return _parser;
    }

    bool add(const struct argp_option * = nullptr, argp_parser_t = nullptr, const char * = nullptr, const char * = nullptr);
    bool parse(int, char * []);

protected:

    // 用于参数解析的结构体
    struct argp _argp;

    // 自定义的参数解析函数
    argp_parser_t _parser;

    zArgs();
    ~zArgs();

private:

    // 实例指针
    static zArgs *instance;

    // 所有参数选项
    struct argp_option *_options;

    //
    void addOptions(const struct argp_option *);
};


#endif //ZHS_ZARGS_H
