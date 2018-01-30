//
// Created by zhangweiwen on 2016/8/26.
//

#include <cstring>

#include "zConfig.h"
#include "zhs/zhs.h"

/**
 * 解析配置文件
 *
 * @param nodeName 节点名字
 * @return         是否解析成功
 */
bool zConfig::parse(const char *nodeName)
{
    if (_parser.initFile(_config)) {
        xmlNodePtr root = _parser.getRootNode("Zhs");
        if (root) {
            xmlNodePtr globalNode = _parser.getChildNode(root, "global");
            if (globalNode) {
                if (!parseGlobal(globalNode)) {
                    return false;
                }
            } else {
                Zhs::logger->warn("没有找到全局(global)配置。");
            }

            xmlNodePtr node = _parser.getChildNode(root, nodeName);
            if (node) {
                if (!parseCustom(node)) {
                    return false;
                }
            } else {
                Zhs::logger->warn("没有找到指定的(%s)配置。", nodeName);
            }

            return true;
        }
    }

    return false;
}

/**
 * 全局(global)配置解析
 *
 * @param node 全局(global)配置节点
 * @return     是否解析成功
 */
bool zConfig::parseGlobal(xmlNodePtrConst node)
{
    xmlNodePtr child = _parser.getChildNode(node, nullptr);

    while (child) {
        parseNormal(child);

        child = _parser.getNextNode(child, nullptr);
    }

    return true;
}

/**
 * 普通参数解析，把参数放入全局容器中。
 *
 * @param node 要解析的节点
 * @return     是否解析成功
 */
bool zConfig::parseNormal(xmlNodePtrConst node)
{
    char buff[128];

    if (_parser.getNodeContentStr(node, buff, 128)) {
        std::string value = buff;

        Zhs::global[(char *) node->name] = value;

        return true;
    }

    return false;
}