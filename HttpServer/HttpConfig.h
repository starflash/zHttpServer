//
// Created by zhangweiwen on 18-1-21.
//

#ifndef ZHS_HTTPCONFIG_H
#define ZHS_HTTPCONFIG_H

#include <zhs/misc/zConfig.h>

/**
 * 处理配置信息
 */
class HttpConfig : public zConfig
{
    /**
     * 读取配置文件
     *
     * @param node 节点
     * @return     是否读取成功
     */
    bool parseCustom(xmlNodePtrConst node) override
    {
        if (node) {
            xmlNodePtr child = _parser.getChildNode(node, nullptr);
            while (child) {
                parseNormal(child);
                child = _parser.getNextNode(child, nullptr);
            }

            return true;
        }

        return false;
    }
};

#endif //ZHS_HTTPCONFIG_H
