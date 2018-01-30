//
// Created by zhangweiwen on 2016/8/26.
//

#ifndef ZHS_ZRWLOCK_H
#define ZHS_ZRWLOCK_H

#include <pthread.h>

#include <zhs/base/zNoncopyable.h>

/**
 * 封装了系统读写锁，由构造函数和析构函数来完成创建和销毁系统读写锁。
 */
class zRWLock : private zNoncopyable
{
public:

    /**
     * 构造函数，创建系统读写锁。
     */
    zRWLock()
    {
        pthread_rwlock_init(&_rwlock, nullptr);
    }

    /**
     * 析构函数，销毁系统读写锁。
     */
    ~zRWLock() override
    {
        pthread_rwlock_destroy(&_rwlock);
    }

    /**
     * 对读写锁进行读加锁操作
     */
    inline void rdlock()
    {
        pthread_rwlock_rdlock(&_rwlock);
    }

    /**
     * 对读写锁进行写加锁操作
     */
    inline void wrlock()
    {
        pthread_rwlock_wrlock(&_rwlock);
    }

    /**
     * 对读写锁进行解锁操作
     */
    inline void unlock()
    {
        pthread_rwlock_unlock(&_rwlock);
    }

private:

    // 系统读写锁
    pthread_rwlock_t _rwlock {};
};

#endif //ZHS_ZRWLOCK_H
