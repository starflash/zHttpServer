//
// Created by zhangweiwen on 2016/8/18.
//

#ifndef ZHS_ZMUTEX_H
#define ZHS_ZMUTEX_H

#include <pthread.h>
#include <iostream>

#include <zhs/base/zNoncopyable.h>

/**
 * 封装了系统互斥量，避免了使用系统互斥量时候需要手工初始化和销毁互斥对象的操作。
 */
class zMutex : private zNoncopyable
{
public:

    // 友元类
    friend class zCond;

    // 构造函数，创建互斥量。
#ifdef __USE_GNU
    explicit zMutex(int kind = PTHREAD_MUTEX_FAST_NP)
#else
    explicit zMutex(int kind = PTHREAD_MUTEX_DEFAULT)
#endif
    {
        pthread_mutexattr_t attr {};

        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, kind);
        pthread_mutex_init(&_mutex, &attr);
    }

    // 析构函数，销毁互斥量。
    ~zMutex() override
    {
        pthread_mutex_destroy(&_mutex);
    }

    // 复制构造函数，在C++2011中，当定义了析构函数后，就不会生成默认的复制构造函数。
    zMutex(const zMutex& mutex)
    {
        _mutex = mutex._mutex;
    }

    /**
     * 加锁互斥量
     */
    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    /**
     * 解锁互斥量
     */
    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

private:

    // 系统互斥量
    pthread_mutex_t _mutex {};
};

#endif //ZHS_ZMUTEX_H
