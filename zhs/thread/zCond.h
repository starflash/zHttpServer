//
// Created by zhangweiwen on 2016/8/18.
//

#ifndef ZHS_ZCOND_H
#define ZHS_ZCOND_H

#include <pthread.h>

#include "zMutex.h"

/**
 * 封装了系统条件变量，避免了使用系统条件变量时候需要手工初始化和销毁条件变量的操作。
 */
class zCond : private zNoncopyable
{
public:

    // 构造函数，创建系统条件变量。
    zCond()
    {
        pthread_cond_init(&_cond, nullptr);
    }

    // 析构函数，销毁系统条件变量。
    ~zCond()
    {
        pthread_cond_destroy(&_cond);
    }

    /**
     * 对所有等待这个条件变量的线程广播，使这些线程能够继续往下执行。
     */
    void broadcast()
    {
        pthread_cond_broadcast(&_cond);
    }

    /**
     * 对某个等待这个条件变量的线程发送信号，使该线程能够继续往下执行。
     */
    void signal()
    {
        pthread_cond_signal(&_cond);
    }

    /**
     * 等待特定的条件变量成立
     *
     * @param mutex 需要等待的互斥量对象
     */
    void wait(zMutex &mutex)
    {
        pthread_cond_wait(&_cond, &mutex._mutex);
    }

private:

    // 系统条件变量
    pthread_cond_t _cond {};
};

#endif //ZHS_ZCOND_H
