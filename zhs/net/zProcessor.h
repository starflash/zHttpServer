//
// Created by zhangweiwen on 16-12-22.
//

#ifndef ZHS_ZPROCESSOR_H
#define ZHS_ZPROCESSOR_H

#include "zCmd.h"

/**
 * 定义了消息处理接口，所有接收到的TCP数据指令需要通过这个接口来处理。
 */
class zProcessor
{
public:

    // 处理消息
    virtual bool parseMessage(const Zhs::t_CmdNull *, unsigned int) = 0;
};

#endif //ZHS_ZPROCESSOR_H
