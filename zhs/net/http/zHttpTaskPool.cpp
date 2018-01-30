//
// Created by zhangweiwen on 17-2-6.
//

#include "zHttpTaskPool.h"
#include "zHttpThread.h"

/**
 * 把一个http连接添加到http服务处理队列中
 *
 * @param task 一个http连接任务
 * @return     true
 */
bool zHttpTaskPool::addHttp(zHttpTask *task)
{
    // 由于存在多个http处理线程，需要按照一定的算法添加到不同的http处理线程中。
    static unsigned int hashcode = 0;
    zHttpThread *thread;

    thread = dynamic_cast<zHttpThread *> (httpThreadGroup.getThread(hashcode++ % max_http_threads));
    if (thread) {
        thread->add(task);
    }

    return true;
}

/**
 * 初始化线程池
 *
 * @param interval 线程执行间隔时间
 * @return         是否成功
 */
bool zHttpTaskPool::init(const unsigned int interval)
{
    std::ostringstream tname;

    for (int i = 0; i < max_http_threads; i++) {
        tname.str("");
        tname << "zHttpThread[" << i << "]";

        zHttpThread *thread;
        try {
            thread = new zHttpThread(this, tname.str(), interval);
        } catch (std::bad_alloc &e) {
            return false;
        }

        if (!thread->start()) {
            return false;
        }

        httpThreadGroup.addThread(thread);
    }

    return true;
}

/**
 * 初始化线程池
 *
 * @param interval 线程执行间隔时间
 * @return         是否成功
 */
bool zHttpTaskPool::init(const std::string &interval)
{
    return init((unsigned short) std::stoul(interval));
}

/**
 * 释放线程池，释放各种资源，等待各种线程退出。
 */
void zHttpTaskPool::free()
{
    httpThreadGroup.joinAll();
}