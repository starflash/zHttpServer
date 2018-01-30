//
// Created by zhangweiwen on 17-2-6.
//

#ifndef ZHS_ZHTTPTASKPOOL_H
#define ZHS_ZHTTPTASKPOOL_H


#include <zhs/base/zNoncopyable.h>
#include <zhs/thread/zThreadGroup.h>
#include "zHttpTask.h"

/**
 * 实现轻量级的http服务框架类
 */
class zHttpTaskPool : private zNoncopyable
{
public:

    // 构造函数
    zHttpTaskPool() = default;

    // 析构函数
    ~zHttpTaskPool() override
    {
        free();
    }

    bool addHttp(zHttpTask*);
    bool init(unsigned int);
    bool init(const std::string &);
    void free();

private:

    // 最大http线程数量
    static const int max_http_threads = 8;

    // http服务处理线程组
    zThreadGroup httpThreadGroup;
};


#endif //ZHS_ZHTTPTASKPOOL_H
