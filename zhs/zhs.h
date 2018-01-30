//
// Created by zhangweiwen on 2016/8/23.
//

#ifndef ZHS_ZHS_H
#define ZHS_ZHS_H

#include "./base/zType.h"
#include "./base/zProperties.h"
#include "./log/zLogger.h"

namespace Zhs
{
    // 随机数种子
    extern __thread unsigned int seedp;

    // 服务器运行时间
    extern volatile QWORD serverTime;

    // 日志
    extern zLogger *logger;

    // 存取全局变量的容器
    extern zProperties global;
}

#endif //ZHS_ZHS_H
