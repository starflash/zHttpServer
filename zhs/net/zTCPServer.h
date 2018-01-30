//
// Created by zhangweiwen on 17-1-26.
//

#ifndef ZHS_ZTCPSERVER_H
#define ZHS_ZTCPSERVER_H

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <zhs/base/zNoncopyable.h>

/**
 * 封装了服务器监听模块，可以方便的创建一个服务器对象，等待客户端的连接。
 */
class zTCPServer : private zNoncopyable
{
public:

    explicit zTCPServer(const std::string &);
    ~zTCPServer() override ;

    bool bind(const std::string &, unsigned short);
    int accept(struct sockaddr_in *);

private:

    // 轮询超时，毫秒。
    static const int POLLING_TIMEOUT = 2100;

    // 最大等待队列
    static const int MAX_WAITQUEUE = 2000;

    // 服务器名称
    std::string serverName;

    // 监听套接字
    int sockfd;

    // epoll文件描述符
    int epfd;
};

#endif //ZHS_ZTCPSERVER_H
