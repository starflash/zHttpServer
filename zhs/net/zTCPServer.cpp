//
// Created by zhangweiwen on 17-1-26.
//

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <zhs/zhs.h>
#include "zTCPServer.h"

/**
 * 构造函数
 *
 * @param name 服务器名称
 */
zTCPServer::zTCPServer(const std::string &name) : serverName(name), sockfd(-1)
{
    Zhs::logger->trace("zTCPServer::zTCPServer");

    epfd = epoll_create(1); // 自从Linux 2.6.8开始，size参数被忽略，但是依然要大于0。
    assert(-1 != epfd);
}

/**
 * 析构函数
 *
 * 关闭服务器
 */
zTCPServer::~zTCPServer()
{
    Zhs::logger->trace("zTCPServer::~zTCPServer");

    TEMP_FAILURE_RETRY(close(epfd));
    if (-1 != sockfd) {
        shutdown(sockfd, SHUT_RD);
        TEMP_FAILURE_RETRY(close(sockfd));

        sockfd = -1;
    }
}

/**
 * 绑定服务到某一个端口
 *
 * @param name 服务名称
 * @param port 端口
 * @return     是否绑定成功
 */
bool zTCPServer::bind(const std::string &name, const unsigned short port)
{
    Zhs::logger->trace("zTCPServer::bind");

    if (-1 != sockfd) {
        Zhs::logger->error("服务器已经初始化");

        return false;
    }

    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) {
        Zhs::logger->error("创建套接字失败");

        return false;
    }

    // 设置套接字为可重用状态
    int reuse = 1;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) {
        Zhs::logger->error("不能设置套接字为可重用状态");

        TEMP_FAILURE_RETRY(close(sockfd));
        sockfd = -1;

        return false;
    }

    // 设置套接字发送接收缓冲区大小，并且服务器程序必须在调用accept函数之前设置。
    socklen_t buff_size = 128 * 1024;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &buff_size, sizeof(buff_size))) {
        TEMP_FAILURE_RETRY(close(sockfd));

        return false;
    }
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &buff_size, sizeof(buff_size))) {
        TEMP_FAILURE_RETRY(close(sockfd));

        return false;
    }

    // 绑定套接字
    struct sockaddr_in addr {};
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (-1 == ::bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))) {
        Zhs::logger->error("不能绑定服务器端口 %d %s", port, strerror(errno));

        TEMP_FAILURE_RETRY(close(sockfd));
        sockfd = -1;

        return false;
    }

    // 监听套接字
    if (-1 == listen(sockfd, MAX_WAITQUEUE)) {
        Zhs::logger->error("监听套接字失败");

        TEMP_FAILURE_RETRY(close(sockfd));
        sockfd = -1;

        return false;
    }

    // 使用epoll来处理TCP连接
    struct epoll_event ev {};
    ev.events = EPOLLIN;
    ev.data.ptr = nullptr;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)) {
        Zhs::logger->error("在epoll注册TCP连接事件失败");

        TEMP_FAILURE_RETRY(close(sockfd));
        sockfd = -1;

        return false;
    }

    Zhs::logger->info("服务器 %s:%u 端口初始化绑定成功", name.c_str(), port);

    return true;
}

/**
 * 接受客户端的连接
 *
 * @param addr 返回的TCP连接地址
 * @return     连接套接字
 */
int zTCPServer::accept(struct sockaddr_in *addr)
{
    Zhs::logger->trace("zTCPServer::accept");

    int retval = -1;
    struct epoll_event ev {};
    socklen_t len = sizeof(struct sockaddr_in);

    memset(addr, 0, len);
    if (1 == epoll_wait(epfd, &ev, 1, POLLING_TIMEOUT)) {
        if (ev.events & EPOLLIN) {
            retval = (int) TEMP_FAILURE_RETRY(::accept(sockfd, (struct sockaddr *) addr, &len));
        }
    }

    return retval;
}