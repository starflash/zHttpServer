//
// Created by zhangweiwen on 2016/8/26.
//

#ifndef ZHS_ZRWLOCKEX_H
#define ZHS_ZRWLOCKEX_H

#include "zRWLock.h"

/**
 * 对读写锁的封装，方便在复杂环境中使用读锁。
 */
class zRWLockEx_Rd : private zNoncopyable
{
public:

    /**
     * 构造函数，加读锁。
     */
    explicit zRWLockEx_Rd(zRWLock &rwlock) : _rwlock(rwlock)
    {
        _rwlock.rdlock();
    }

    /**
     * 析构函数，解锁。
     */
    ~zRWLockEx_Rd() override
    {
        _rwlock.unlock();
    }

private:

    /**
     * 读写锁对象
     */
    zRWLock &_rwlock;
};

/**
 * 对读写锁的封装，方便在复杂环境中使用写锁。
 */
class zRWLockEx_Wr : private zNoncopyable
{
public:

    /**
     * 构造函数，加写锁。
     */
    explicit zRWLockEx_Wr(zRWLock &rwlock) : _rwlock(rwlock)
    {
        _rwlock.wrlock();
    }

    /**
     * 析构函数，解锁。
     */
    ~zRWLockEx_Wr() override
    {
        _rwlock.unlock();
    }

private:

    /**
     * 读写锁对象
     */
    zRWLock &_rwlock;
};

#endif //ZHS_ZRWLOCKEX_H
