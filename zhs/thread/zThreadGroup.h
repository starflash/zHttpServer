//
// Created by zhangweiwen on 16-12-23.
//

#ifndef ZHS_ZTHREADGROUP_H
#define ZHS_ZTHREADGROUP_H


#include <vector>

#include <zhs/base/zNoncopyable.h>
#include "zThread.h"
#include "zRWLock.h"
#include "zRWLockEx.h"

/**
 * 对线程进行分组管理
 */
class zThreadGroup : private zNoncopyable
{
public:

    // 定义一个线程容器类型
    typedef std::vector<zThread *> ThreadContainer;

    //
    struct ThreadCallback
    {
        virtual void exec(zThread *) = 0;
        virtual ~ThreadCallback() = default;
    };

    zThreadGroup();
    ~zThreadGroup() override;

    void addThread(zThread *);
    zThread* getThread(const ThreadContainer::size_type);
    zThread* operator[](const ThreadContainer::size_type);

    void joinAll();
    void execAll(ThreadCallback &);

    /**
     * 获取线程组中线程的数量
     *
     * @return 线程数量
     */
    const ThreadContainer::size_type size()
    {
        zRWLockEx_Rd rdlock(rwlock);

        return vtc.size();
    }

private:

    // 线程容器
    ThreadContainer vtc;

    // 读写锁
    zRWLock rwlock;
};


#endif //ZHS_ZTHREADGROUP_H
