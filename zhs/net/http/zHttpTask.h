//
// Created by zhangweiwen on 17-2-6.
//

#ifndef ZHS_ZHTTPTASK_H
#define ZHS_ZHTTPTASK_H


#include <zhs/base/zNoncopyable.h>
#include <zhs/net/zSocket.h>

class zHttpTaskPool;

/**
 * 轻量级http任务类，封装一些低层接口。
 */
class zHttpTask : private zNoncopyable
{
public:

    /**
     * 构造函数
     *
     * @param pool   线程池
     * @param sockfd 连接套接字
     * @param addr   连接地址
     */
    zHttpTask(zHttpTaskPool *pool, const int sockfd, const struct sockaddr_in *addr = nullptr)
            : socket(sockfd, addr), pool(pool), lifetime()
    {}

    /**
     * 析构函数
     */
    ~zHttpTask() override = default;

    /**
     * 添加事件到epoll描述符
     *
     * @param epfd   epoll描述符
     * @param events 待添加的事件
     * @param ptr    额外参数
     */
    void addEpoll(int epfd, uint32_t events, void *ptr)
    {
        socket.addEpoll(epfd, events, ptr);
    }

    /**
     * 从epoll描述符中删除事件
     *
     * @param epfd   epoll描述符
     * @param events 待删除的事件
     */
    void delEpoll(int epfd, uint32_t events)
    {
        socket.delEpoll(epfd, events);
    }

    /**
     * 是否验证超时
     *
     * @param time     目标时间
     * @param interval 超时时间，毫秒。
     * @return bool    是否超时
     */
    bool isHttpTimeout(zRealTime &time, const unsigned long long interval = 2000) const
    {
        return lifetime.elapse(time) > interval;
    }

    /**
     * http任务主处理函数
     *
     * @return 1: 成功
     *         0: 还要继续等待
     *        -1: 失败
     */
    virtual int httpCore()
    {
        return 1;
    }

    bool sendCmd(const void *, unsigned int);
    bool sendResponse(std::string &, bool = false);

protected:

    // 对底层套接字的封装
    zSocket socket;

private:

    // 任务所属的连接池
    zHttpTaskPool *pool;

    // 创建连接的时间
    zRealTime lifetime;
};


#endif //ZHS_ZHTTPTASK_H
