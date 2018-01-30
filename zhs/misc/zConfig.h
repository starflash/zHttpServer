//
// Created by zhangweiwen on 2016/8/26.
//

#ifndef ZHS_ZCONFIG_H
#define ZHS_ZCONFIG_H

#include <string>

#include "zXMLParser.h"

// 定义常量指针
typedef xmlNodePtr const xmlNodePtrConst;

/**
 * 配置文件解析器，此类必须继承使用。
 * 本类实现了全局参数的解析标记为<global></global>，并把解析的参数保存在一个全局的参数容器中。
 * 如果用户有自己的配置,用户应该实现自己的参数解析。
 */
class zConfig
{
public:

    /**
     * 构造函数
     *
     * @param config 配置文件名
     */
    explicit zConfig(const char *config = "config.xml")
    {
        this->_config = config;
    }

    /**
     * 析构函数
     */
    virtual ~zConfig()
    {
        this->_parser.freeXML();
    }

    bool parse(const char *);

protected:

    // XML解析器
    zXMLParser _parser {};

    // 配置文件
    std::string _config;

    bool parseGlobal(xmlNodePtrConst);
    bool parseNormal(xmlNodePtrConst);
    bool parseSuperServer(xmlNodePtrConst);
    virtual bool parseCustom(xmlNodePtrConst) = 0;
};

#endif //ZHS_ZCONFIG_H
