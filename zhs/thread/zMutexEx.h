//
// Created by zhangweiwen on 2016/8/18.
//

#ifndef ZHS_ZMUTEXEX_H
#define ZHS_ZMUTEXEX_H

#include "zMutex.h"

/**
 * 对互斥量对象的封装，方便在复杂环境中使用。
 */
class zMutexEx : private zNoncopyable
{
public:

    /**
     * 构造函数，对互斥量加锁。
     *
     * @param mutex 互斥量
     */
    explicit zMutexEx(zMutex &mutex) : _mutex(mutex)
    {
        _mutex.lock();
    }

    /**
     * 析构函数，对互斥量解锁。
     */
    ~zMutexEx() override
    {
        _mutex.unlock();
    }

private:

    /**
     * 互斥量对象
     */
    zMutex _mutex {};
};

#endif //ZHS_ZMUTEXEX_H
