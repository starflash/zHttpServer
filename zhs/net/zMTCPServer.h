//
// Created by zhangweiwen on 2016/8/29.
//

#ifndef ZHS_ZMTCPSERVER_H
#define ZHS_ZMTCPSERVER_H

#include <string>
#include <map>
#include <vector>
#include <sys/epoll.h>

#include <zhs/base/zNoncopyable.h>
#include <zhs/thread/zMutexEx.h>

/**
 * 封装了服务器监听模块，可以方便的创建一个服务器对象，等待客户端的连接，可以同时监听多个端口。
 */
class zMTCPServer : private zNoncopyable
{
public:

    // 定义一个map类型，绑定套接字与端口的关系。
    typedef std::map<int, unsigned short> sockfd2port;

    explicit zMTCPServer(const std::string &);
    ~zMTCPServer() override ;

    bool bind(const std::string &, unsigned short);
    int accept(sockfd2port &);

private:

    // 轮询超时，毫秒。
    static const int POLLING_TIMEOUT = 2100;

    // 最大等待队列
    static const int MAX_WAITQUEUE = 2000;

    // 服务器名称
    std::string serverName;

    // 套接字与端口映射表
    sockfd2port sockfds;

    // epoll文件描述符
    int epfd;

    // epoll事件集
    std::vector<struct epoll_event> events;

    // 互斥锁
    zMutex mutex;
};

#endif //ZHS_ZMTCPSERVER_H
