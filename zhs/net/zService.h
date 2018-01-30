//
// Created by zhangweiwen on 2016/8/26.
//

#ifndef ZHS_ZSERVICE_H
#define ZHS_ZSERVICE_H

#include <string>

#include <zhs/base/zNoncopyable.h>
#include <zhs/base/zProperties.h>
#include <zhs/base/zType.h>

/**
 * 定义了服务器的框架基类
 * 所有服务器程序需要继承这个类，无论其有多少个子类，整个运行环境只有一个类的实例。
 * 需要派生类实现单例模式。
 */
class zService : private zNoncopyable
{
public:

    /**
     * 析构函数
     */
    ~zService() override
    {
        serviceInstance = nullptr;
    }

    /**
     * 获取服务类型
     */
    virtual const WORD getServerType()
    {
        return 0;
    }

    /**
     * 读取配置文件
     */
    virtual void loadConfig()
    {}

    /**
     * 判断主循环是否结束
     */
    bool isTerminated() const
    {
        return terminated;
    }

    /**
     * 结束主循环
     */
    void terminate()
    {
        terminated = true;
    }

    /**
     * 服务程序框架的主函数
     */
    void run();

    /**
     * 返回指向服务器对象的指针
     *
     * @return 指针
     */
    static zService * getInstance()
    {
        return serviceInstance;
    }

    // 存储当前运行系统的环境变量
    zProperties env;

protected:

    /**
     * 构造函数，初始化服务器名称。
     *
     * @param name 服务器名称
     */
    explicit zService(const std::string &name) : serviceName(name)
    {
        serviceInstance = this;
        terminated = false;
    }

    /**
     * 验证服务器初始化是否成功，之后进入主回调函数（即：callback）。
     */
    virtual bool validate()
    {
        return true;
    }

    /**
     * 初始化服务器
     */
    virtual bool init();

    /**
     * 主回调函数，用来监听服务端口。
     * 如果返回true，服务器将继续运行，返回false，则服务器停止。
     */
    virtual bool callback() = 0;

    /**
     * 停止服务器程序，回收资源。
     */
    virtual void stop() = 0;

private:

    // 类的唯一实例对象，包括派生类，初始化为空指针。
    static zService *serviceInstance;

    // 服务名称
    std::string serviceName;

    // 主循环结束标记
    bool terminated;
};

#endif //ZHS_ZSERVICE_H
