//
// Created by zhangweiwen on 17-2-6.
//

#include "zHttpThread.h"

/**
 * 构造函数
 *
 * @param pool     线程池
 * @param name     线程名称
 * @param interval 线程执行间隔时间
 */
zHttpThread::zHttpThread(zHttpTaskPool *pool, const std::string &name, const unsigned int interval): zThread(name), pool(pool), interval(interval), now()
{
    tasks_count = 0;
    epfd = epoll_create(1);
    assert(-1 != epfd);
    events.resize(256);
}

/**
 * 析构函数
 */
zHttpThread::~zHttpThread()
{
    TEMP_FAILURE_RETRY(close(epfd));
}

/**
 *
 */
void zHttpThread::run()
{
    t_ListHttpTask::iterator iter, next;

    while (!isTerminated()) {
        mutex.lock();

        if (!tasks.empty()) {
            int nfds = epoll_wait(epfd, &events[0], tasks_count, 0);
            if (nfds > 0) {
                for (int i = 0; i < nfds; i++) {
                    auto *task = static_cast<zHttpTask *> (events[i].data.ptr);

                    if (events[i].events & (EPOLLERR | EPOLLPRI)) {
                        // 套接字出现错误
                        remove(task);
                    } else if (events[i].events & EPOLLIN) {
                        switch (task->httpCore()) {
                            case 1:
                                // 成功
                                remove(task);
                                break;
                            case 0:
                                // 超时
                                break;
                            case -1:
                                remove(task);
                                break;
                                // 失败
                            default:
                                break;
                        }
                    }
                }
            }

            now();

            for (iter = tasks.begin(), next = iter, next++; iter != tasks.end(); iter = next, next++)
            {
                zHttpTask *task = *iter;

                if (task->isHttpTimeout(now)) {
                    // 超过指定时间验证还没有通过，需要回收连接。
                    remove(iter);
                }
            }
        }

        mutex.unlock();

        zThread::usleep(interval);
    }

    for (iter = tasks.begin(), next = iter, next++; iter != tasks.end(); iter = next, next++)
    {
        remove(iter);
    }
}