//
// Created by zhangweiwen on 17-1-26.
//

#ifndef ZHS_ZNETSERVICE_H
#define ZHS_ZNETSERVICE_H

#include "zService.h"
#include "zTCPServer.h"

/**
 * 网络服务器类，实现了网络服务器框架代码。
 */
class zNetService : public zService
{
public:

    /**
     * 虚析构函数
     */
    ~zNetService() override
    {
        serviceInstance = nullptr;
    }

    /**
     * 根据TCP连接创建一个连接任务
     *
     * @param sockfd 连接套接字
     * @param addr   地址
     */
    virtual void createTCPTask(int sockfd, const struct sockaddr_in *addr) = 0;

    /**
     * 绑定TCP服务器到某个端口
     *
     * @param port 端口
     * @return     绑定端口是否成功
     */
    bool bind(const unsigned short port)
    {
        return tcpServer ? tcpServer->bind(serviceName, port) : false;
    }

protected:

    /**
     * 构造函数，初始化服务器名称。
     *
     * @param name 服务器名称
     */
    explicit zNetService(const std::string &name) : zService(name), serviceName(name)
    {
        serviceInstance = this;
        tcpServer = nullptr;
    }

    bool init() override ;
    bool callback() override ;
    void stop() override ;

private:

    // 类的唯一实例对象，包括派生类，初始化为空指针。
    static zNetService *serviceInstance;

    // 网络服务器名称
    std::string serviceName;

    // TCP服务器对象
    zTCPServer *tcpServer;
};


#endif //ZHS_ZNETSERVICE_H
