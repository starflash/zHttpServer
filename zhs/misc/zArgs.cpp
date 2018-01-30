//
// Created by zhangweiwen on 17-1-27.
//

#include <cstring>

#include "zArgs.h"
#include "../base/zCommon.h"

// 参数描述
static const char ZHS_args_doc[] = "";

// 默认参数的描述
static const char ZHS_doc[] = "this is default argument document.";

// 默认可用选项
static struct argp_option ZHS_options[] =
{
    {nullptr, 0, nullptr, 0, nullptr, 0}
};

// 默认的分析函数
error_t ZHS_parse_opt(int key, char *arg, struct argp_state *state)
{
    argp_parser_t parser = zArgs::getInstance()->getParser();

    if (parser != nullptr) {
        return parser(key, arg, state);
    } else {
        return ARGP_ERR_UNKNOWN;
    }
}

// 实例指针
zArgs* zArgs::instance = nullptr;

/**
 * 构造函数
 */
zArgs::zArgs()
{
    _parser = nullptr;
    _options = nullptr;

    _argp.children = nullptr;
    _argp.help_filter = nullptr;
    _argp.argp_domain = nullptr;
    _argp.parser = ZHS_parse_opt;
    _argp.args_doc = ZHS_args_doc;
    _argp.doc = ZHS_doc;

    addOptions(ZHS_options);
}

/**
 * 析构函数
 */
zArgs::~zArgs()
{
    SAFE_DELETE_VECTOR(_options);
}

/**
 * 返回唯一的实例
 *
 * @return 实例对象
 */
zArgs* zArgs::getInstance()
{
    if (nullptr == instance) {
        instance = new zArgs();
    }

    return instance;
}

/**
 * 销毁实例
 */
void zArgs::destroyInstance()
{
    SAFE_DELETE(instance);
}

/**
 * 添加参数选项
 *
 * @param opt　选项
 */
void zArgs::addOptions(const struct argp_option *opt)
{
    if (opt != nullptr) {
        int ucount = 0;

        while (opt[ucount].name != nullptr) {
            ucount++;
        }

        if (_options == nullptr) {
            _options = new struct argp_option[ucount + 1];
            memcpy(_options, opt, sizeof(argp_option) * (ucount + 1));
        } else {
            int ocount = 0;

            while (_options[ocount].name != nullptr) {
                ocount++;
            }

            struct argp_option* tmp = _options;

            _options = new struct argp_option[ucount + ocount + 1];
            if (ocount > 0) {
                memcpy(_options, tmp, sizeof(argp_option) * ocount);
            }
            memcpy(_options + ocount, opt, sizeof(argp_option) * (ucount + 1));

            SAFE_DELETE_VECTOR(tmp);
        }
    }

    _argp.options = _options;
}

/**
 * 添加参数选项和分析器，及参数文档，如果省略用默认值。
 *
 * @param opt       参数选项
 * @param parser    参数的分析函数
 * @param args_doc  参数的描述
 * @param doc       默认参数的描述
 * @return          true
 */
bool zArgs::add(const struct argp_option* opt, argp_parser_t parser, const char* args_doc, const char* doc)
{
    if (opt != nullptr) {
        addOptions(opt);
    }
    if (parser != nullptr) {
        _parser = parser;
    }
    if (args_doc != nullptr) {
        _argp.args_doc = args_doc;
    }
    if (doc != nullptr) {
        _argp.doc = doc;
    }

    return true;
}

/**
 * 参数分析
 *
 * @param argc 参数数量
 * @param argv 参数内容
 * @return     true
 */
bool zArgs::parse(int argc, char** argv)
{
    argp_parse(&_argp, argc, argv, 0, nullptr, nullptr);

    return true;
}