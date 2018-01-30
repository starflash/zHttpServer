//
// Created by zhangweiwen on 16-12-22.
//

#include <csignal>
#include <mysql/mysql.h>

#include <zhs/zhs.h>
#include "zThread.h"

/**
 * 创建并启动线程
 *
 * @return 线程是否创建成功
 */
bool zThread::start()
{
    if (isAlive()) {
        Zhs::logger->warn("线程 %s 已存在", getThreadName().c_str());

        return true;
    }

    pthread_attr_t attr {};
    pthread_attr_init(&attr);
    if (isDetached()) {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }
    if (0 != pthread_create(&thread_id, &attr, zThread::routine, this)) {
        Zhs::logger->error("创建线程 %s 失败", getThreadName().c_str());

        return false;
    }
    pthread_attr_destroy(&attr);

    Zhs::logger->debug("创建线程 %s 成功，线程ID: 0x%lx。", getThreadName().c_str(), thread_id);

    mutex.lock();
    while (!isAlive()) {
        // 等待，直到线程启动函数开始执行。
        cond.wait(mutex);
    }
    mutex.unlock();

    return true;
}

/**
 * 线程启动函数
 *
 * @param args 传给线程的参数
 * @return     线程结束信息
 */
void* zThread::routine(void *args)
{
    auto thread = static_cast<zThread *> (args);

    // 初始化随机数
    Zhs::seedp = (unsigned int) time(nullptr);

    thread->mutex.lock();
    thread->alive = true;
    thread->cond.broadcast();
    thread->mutex.unlock();

    // 设置信号屏蔽集
    sigset_t set {};
    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, nullptr);

    // 执行线程主函数，由子类实现线程的主要业务。
    thread->run();

    thread->mutex.lock();
    thread->alive = false;
    thread->cond.broadcast();
    thread->mutex.unlock();

    // 如果线程状态是分离的(detached)，需要回收线程资源。
    if (thread->isDetached()) {
        thread->mutex.lock();
        while (thread->isAlive()) {
            thread->cond.wait(thread->mutex);
        }
        thread->mutex.unlock();

        SAFE_DELETE(thread);
    }

    pthread_exit(nullptr);
}

/**
 * 等待线程结束
 */
void zThread::join()
{
    if (0 != thread_id && isJoinable()) {
        Zhs::logger->debug("线程 %s 准备退出", getThreadName().c_str());

        pthread_join(thread_id, nullptr);
        thread_id = 0;
        mutex.lock();
        while (isAlive()) {
            // 等待，直到线程启动函数执行结束。
            cond.wait(mutex);
        }
        mutex.unlock();
    }

    Zhs::logger->debug("线程 %s 已经退出", getThreadName().c_str());
}