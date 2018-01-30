//
// Created by zhangweiwen on 2016/8/23.
//

#include <libxml/parser.h>
#include "zhs.h"

namespace Zhs
{
    __thread unsigned int seedp = 0;

    volatile QWORD serverTime = 0;

    zLogger *logger = nullptr;

    zProperties global;

    /**
     * 在main函数执行前做一些初始化工作
     */
    static void initialize() __attribute__((constructor));
    void initialize()
    {
        xmlInitParser();
    }

    /**
     * 在main函数退出后做一些清理工作
     */
    static void release() __attribute__((destructor));
    void release()
    {
        xmlCleanupParser();
    }
}