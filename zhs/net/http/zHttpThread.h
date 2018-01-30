//
// Created by zhangweiwen on 17-2-6.
//

#ifndef ZHS_ZHTTPTHREAD_H
#define ZHS_ZHTTPTHREAD_H


#include <vector>
#include <list>

#include <zhs/thread/zThread.h>
#include "zHttpTask.h"

// 连接任务链表
typedef std::list<zHttpTask*,  __gnu_cxx::__pool_alloc<zHttpTask*> > t_ListHttpTask;

/**
 * 轻量级http服务的主处理线程
 */
class zHttpThread : public zThread
{
public:

    explicit zHttpThread(zHttpTaskPool *, const std::string & = std::string("zHttpThread"), unsigned int = 50000);
    ~zHttpThread() override ;

    void run() override ;

    /**
     * 添加一个http连接任务
     *
     * @param task 一个http连接
     */
    void add(zHttpTask *task)
    {
        mutex.lock();
        task->addEpoll(epfd, EPOLLIN | EPOLLERR | EPOLLPRI, static_cast<void *> (task));
        tasks.push_back(task);
        tasks_count = tasks.size();
        if (tasks_count > events.size()) {
            events.resize(tasks_count + 16);
        }
        mutex.unlock();
    }

    /**
     * 从http任务列表中删除一个任务
     *
     * @param task http任务
     */
    void remove(zHttpTask *task)
    {
        task->delEpoll(epfd, EPOLLIN | EPOLLERR | EPOLLPRI);
        tasks.remove(task);
        tasks_count = tasks.size();
        SAFE_DELETE(task);
    }

    /**
     * 从http任务列表中删除一个任务
     *
     * @param iter http任务列表的迭代器指针
     */
    void remove(t_ListHttpTask::iterator &iter)
    {
        zHttpTask *task = *iter;

        (*iter)->delEpoll(epfd, EPOLLIN | EPOLLERR | EPOLLPRI);
        tasks.erase(iter);
        tasks_count = tasks.size();
        SAFE_DELETE(task);
    }

private:

    // 所属的线程池
    zHttpTaskPool* pool;

    // epoll文件描述符
    int epfd;

    // epoll事件集合
    std::vector<struct epoll_event> events;

    // http任务列表
    t_ListHttpTask tasks;

    // http任务列表计数
    t_ListHttpTask::size_type tasks_count;

    // 线程处理间隔(微秒)
    unsigned int interval;

    // 当前时间
    zRealTime now;

    // 互斥锁
    zMutex mutex;
};


#endif //ZHS_ZHTTPTHREAD_H
