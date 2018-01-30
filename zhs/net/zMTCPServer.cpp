//
// Created by zhangweiwen on 2016/8/29.
//

#include <unistd.h>
#include <cerrno>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>

#include <zhs/zhs.h>
#include "zMTCPServer.h"

/**
 * 构造函数
 *
 * @param name 服务器名称
 */
zMTCPServer::zMTCPServer(const std::string &name): serverName(name)
{
    Zhs::logger->trace("zMTCPServer::zMTCPServer");

    epfd = epoll_create(1); // 自从Linux 2.6.8开始，size参数被忽略，但是依然要大于0。
    assert(-1 != epfd);
    events.resize(8);
}

/**
 * 析构函数
 */
zMTCPServer::~zMTCPServer()
{
    Zhs::logger->trace("zMTCPServer::~zMTCPServer");

    TEMP_FAILURE_RETRY(close(epfd));
    for (sockfd2port::const_iterator citer = sockfds.begin(); citer != sockfds.end(); citer++) {
        if (-1 != citer->first) {
            shutdown(citer->first, SHUT_RD);
            TEMP_FAILURE_RETRY(close(citer->first));
        }
    }
    sockfds.clear();
}

/**
 * 绑定服务到某一个端口
 *
 * @param name 服务名称
 * @param port 端口
 * @return     是否绑定成功
 */
bool zMTCPServer::bind(const std::string &name, const unsigned short port)
{
    Zhs::logger->trace("zMTCPServer::bind");

    zMutexEx mutexEx(mutex);

    for (sockfd2port::const_iterator citer = sockfds.begin(); citer != sockfds.end(); citer++) {
        if (citer->second == port) {

            Zhs::logger->warn("端口 %u 已经绑定服务", port);

            return false;
        }
    }

    int sockfd;
    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))) {
        Zhs::logger->error("创建套接字失败");

        return false;
    }

    // 设置套接字为可重用状态
    int reuse = 1;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) {
        Zhs::logger->error("不能设置套接字为可重用状态");

        TEMP_FAILURE_RETRY(close(sockfd));

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
    if (-1 == ::bind(sockfd, (struct sockaddr*) &addr, sizeof(struct sockaddr_in))) {
        Zhs::logger->error("不能绑定服务器端口 %d %s", port, strerror(errno));

        TEMP_FAILURE_RETRY(close(sockfd));

        return false;
    }

    // 监听套接字
    if (-1 == listen(sockfd, MAX_WAITQUEUE)) {
        Zhs::logger->error("监听套接字失败");

        TEMP_FAILURE_RETRY(close(sockfd));

        return false;
    }

    // 使用epoll来处理TCP连接
    struct epoll_event ev {};
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)) {
        Zhs::logger->error("在epoll注册TCP连接事件失败");

        TEMP_FAILURE_RETRY(close(sockfd));

        return false;
    }

    sockfds.insert(sockfd2port::value_type(sockfd, port));
    if (sockfds.size() > events.size()) {
        events.resize(sockfds.size() + 8);
    }

    Zhs::logger->info("服务器 %s:%u 端口初始化绑定成功", name.c_str(), port);

    return true;
}

/**
 * 建立客户端的TCP连接
 *
 * @param connfds 返回的TCP连接集合
 * @return        建立的连接数
 */
int zMTCPServer::accept(sockfd2port &connfds)
{
    Zhs::logger->trace("zMTCPServer::accept");

    int retval = 0;
    zMutexEx mutexEx(mutex);

    int ready;
    if (0 < (ready = epoll_wait(epfd, &events[0], (unsigned int) sockfds.size(), POLLING_TIMEOUT))) {
        for (int i = 0; i < ready; i++) {
            if (events[i].events & EPOLLIN) {
                auto connfd = TEMP_FAILURE_RETRY(::accept(events[i].data.fd, nullptr, nullptr));
                auto port = sockfds[events[i].data.fd];
                connfds.insert(sockfd2port::value_type(connfd, port));
                retval++;
            }
        }
    }

    return retval;
}