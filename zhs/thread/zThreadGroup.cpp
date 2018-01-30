//
// Created by zhangweiwen on 16-12-23.
//

#include <algorithm>

#include <zhs/base/zCommon.h>
#include "zThreadGroup.h"

/**
 * 构造函数
 */
zThreadGroup::zThreadGroup() : vtc(), rwlock()
{}

/**
 * 析构函数
 */
zThreadGroup::~zThreadGroup()
{
    joinAll();
}

/**
 * 添加一个线程到线程组中
 *
 * @param thread 待添加线程
 */
void zThreadGroup::addThread(zThread *thread)
{
    zRWLockEx_Wr wrlock(rwlock);

    auto iter = std::find(vtc.begin(), vtc.end(), thread);
    if (iter == vtc.end()) {
        vtc.push_back(thread);
    }
}

/**
 * 根据下标获取线程
 *
 * @param index 下标
 * @return      线程指针
 */
zThread* zThreadGroup::getThread(const ThreadContainer::size_type index)
{
    zRWLockEx_Rd rdlock(rwlock);

    if (index >= vtc.size()) {
        return nullptr;
    }

    return vtc[index];
}

/**
 * 重载数组操作符，根据下标获取线程。
 *
 * @param index 下标
 * @return      线程指针
 */
zThread* zThreadGroup::operator[](const ThreadContainer::size_type index)
{
    return getThread(index);
}

/**
 * 等待线程组中所有线程结束
 */
void zThreadGroup::joinAll()
{
    zRWLockEx_Wr wrlock(rwlock);

    while (!vtc.empty()) {
        zThread *thread = vtc.back();
        if (nullptr != thread) {
            thread->terminate();
            thread->join();

            SAFE_DELETE(thread);
        }
        vtc.pop_back();
    }
}

/**
 * 对线程组中所有线程执行回调函数
 *
 * @param callback 回调函数
 */
void zThreadGroup::execAll(ThreadCallback &callback)
{
    zRWLockEx_Rd rdlock(rwlock);

    for (const auto &iter : vtc) {
        callback.exec(iter);
    }
}