//
// Created by zhangweiwen on 2016/8/29.
//

#ifndef ZHS_ZMNETSERVICE_H
#define ZHS_ZMNETSERVICE_H

#include "zService.h"
#include "zMTCPServer.h"

/**
 * 网络服务器类，实现了网络服务器框架代码。
 */
class zMNetService : public zService
{
public:

    /**
     * 虚析构函数
     */
    ~zMNetService() override
    {
        serviceInstance = nullptr;
    }

    /**
     * 根据TCP连接创建一个连接任务
     *
     * @param sockfd 连接套接字
     * @param port   端口号
     */
    virtual void createTCPTask(int sockfd, unsigned short port) = 0;

    /**
     * 绑定服务到某个端口
     *
     * @param name 服务名称
     * @param port 端口
     * @return     绑定是否成功
     */
    bool bind(const std::string &name, const unsigned short port)
    {
        return tcpServer ? tcpServer->bind(name, port) : false;
    }

protected:

    /**
     * 构造函数，初始化服务器名称。
     *
     * @param name 服务器名称
     */
    explicit zMNetService(const std::string &name) : zService(name), serviceName(name)
    {
        serviceInstance = this;
        tcpServer = nullptr;
    }

    bool init() override;
    bool callback() override;
    void stop() override;

private:

    // 类的唯一实例对象，包括派生类，初始化为空指针。
    static zMNetService *serviceInstance;

    // 网络服务器名称
    std::string serviceName;

    // TCP服务器对象
    zMTCPServer *tcpServer;
};


#endif //ZHS_ZMNETSERVICE_H
