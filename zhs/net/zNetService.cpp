//
// Created by zhangweiwen on 17-1-26.
//

#include <netinet/in.h>

#include <zhs/zhs.h>
#include "zNetService.h"

zNetService *zNetService::serviceInstance = nullptr;

/**
 * 初始化服务器
 *
 * 1.调用父类的初始化方法 2.初始化TCP服务器
 *
 * @return 初始化是否成功
 */
bool zNetService::init()
{
    Zhs::logger->trace("zNetService::init");

    if (!zService::init()) {
        return false;
    }

    // 创建TCP服务器对象
    if (nullptr == (tcpServer = new zTCPServer(serviceName))) {
        return false;
    }

    Zhs::logger->debug("zNetService initialize done");

    return true;
}

/**
 * 网络服务器程序的主回调函数
 *
 * 接受客户端的请求建立连接
 *
 * @return true
 */
bool zNetService::callback()
{
    Zhs::logger->trace("zNetService::callback");

    int sockfd;
    struct sockaddr_in addr {};

    if ((sockfd = tcpServer->accept(&addr)) >= 0) {
        createTCPTask(sockfd, &addr);
    }

    return true;
}

/**
 * 停止网络服务器程序
 */
void zNetService::stop()
{
    Zhs::logger->trace("zNetService::stop");

    SAFE_DELETE(tcpServer);
}