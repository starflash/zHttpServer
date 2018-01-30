//
// Created by zhangweiwen on 16-12-22.
//

#ifndef ZHS_ZTHREAD_H
#define ZHS_ZTHREAD_H

#include <string>
#include <ctime>
#include <unistd.h>
#include <pthread.h>

#include <zhs/base/zNoncopyable.h>
#include "zMutex.h"
#include "zCond.h"

/**
 * 封装了线程操作，所有使用线程的基类。
 */
class zThread : private zNoncopyable
{
public:

    // 线程启动函数
    static void* routine(void *args);

    /**
     * 构造函数
     * 线程状态参数设置：为真时在线程退出时保存状态，为假时则不保存状态。
     *
     * @param name     线程名称
     * @param joinable 线程状态
     */
    explicit zThread(const std::string &name = std::string("zThread"), const bool joinable = true)
            : thread_name(name), thread_id(0), joinable(joinable), alive(false), terminated(false)
    {}

    /**
     * 析构函数
     */
    ~zThread() override = default;

    /**
     * 使当前线程睡眠
     *
     * @param sec 秒
     */
    static void sleep(const long sec) {
        ::sleep((unsigned int) sec);
    }

    /**
     * 使当前线程睡眠
     *
     * @param msec 毫秒
     */
    static void msleep(const long msec) {
        usleep(msec * 1000);
    }

    /**
     * 使当前线程睡眠
     *
     * @param msec 微秒
     */
    static void usleep(const long usec) {
        struct timespec ureq {};

        ureq.tv_sec = usec / (1000 * 1000);
        ureq.tv_nsec = (usec % (1000 * 1000)) * 1000;

        nanosleep(&ureq, nullptr);
    }

    /**
     * 返回线程名称
     *
     * @return 线程名称
     */
    const std::string& getThreadName() const {
        return thread_name;
    }

    /**
     * 线程状态是否结合的（joinable）
     *
     * @return 是否结合
     */
    const bool isJoinable() const {
        return joinable;
    }

    /**
     * 线程状态是否分离的（detached）
     *
     * @return 是否分离
     */
    const bool isDetached() const {
        return !joinable;
    }

    /**
     * 线程是否在运行状态
     *
     * @return 是否在运行状态
     */
    const bool isAlive() const {
        return alive;
    }

    /**
     * 线程是否需要终止
     * 用在run()函数循环中，判断循环是否继续执行下去。
     *
     * @return 是否需要终止
     */
    const bool isTerminated() const {
        return terminated;
    }

    /**
     * 终止线程
     * 设置线程终止标记，当线程的主回调函数检测到该标记为真时就退出循环。
     */
    void terminate() {
        terminated = true;
    }

    /**
     * 判断是否为同一个线程
     *
     * @param thread 待比较的线程
     * @return       是否为同一个线程
     */
    bool operator==(zThread& thread) const {
        return pthread_equal(thread_id, thread.thread_id) != 0;
    }

    /**
     * 判断是否为不同的线程
     *
     * @param thread 待比较的线程
     * @return       是否为不同的线程
     */
    bool operator!=(zThread& thread) const {
        return !operator==(thread);
    }

    // 线程主函数，实现线程的主要业务，具体功能由子类实现。
    virtual void run() = 0;

    // 创建一个线程
    bool start();

    // 等待某个线程结束
    void join();

private:

    // 线程名称
    std::string thread_name;

    // 线程编号
    pthread_t thread_id;

    // 互斥锁
    zMutex mutex {};

    // 条件变量
    zCond cond {};

    // 线程的状态：结合的（joinable）或分离的（detached）。
    bool joinable;

    // 线程是否在运行
    volatile bool alive;

    // 线程是否将终止
    volatile bool terminated;
};


#endif //ZHS_ZTHREAD_H
