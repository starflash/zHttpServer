//
// Created by zhangweiwen on 2016/8/29.
//

#include "zMNetService.h"
#include <zhs/base/zCommon.h>
#include <zhs/zhs.h>

zMNetService *zMNetService::serviceInstance = nullptr;

/**
 * 初始化服务器
 *
 * 1.调用父类的初始化方法
 * 2.初始化TCP服务器
 *
 * @return 初始化是否成功
 */
bool zMNetService::init()
{
    Zhs::logger->trace("zMNetService::init");

    if (!zService::init()) {
        return false;
    }

    // 创建TCP服务器对象
    if (nullptr == (tcpServer = new zMTCPServer(serviceName))) {
        return false;
    }

    Zhs::logger->debug("zMNetService initialize done");

    return true;
}

/**
 * 网络服务程序的主回调函数
 *
 * 接受客户端的请求建立连接
 *
 * @return true
 */
bool zMNetService::callback()
{
    Zhs::logger->trace("zMNetService::callback");

    zMTCPServer::sockfd2port connfds;
    if (tcpServer->accept(connfds) > 0) {
        for (zMTCPServer::sockfd2port::const_iterator citer = connfds.begin(); citer != connfds.end(); citer++) {
            if (citer->first >= 0) {
                // 创建TCP连接任务，第一个参数是连接套接字，第二个参数是端口号。
                createTCPTask(citer->first, citer->second);
            }
        }
    }

    return true;
}

/**
 * 停止网络服务器程序
 */
void zMNetService::stop()
{
    Zhs::logger->trace("zMNetService::stop");

    SAFE_DELETE(tcpServer);
}