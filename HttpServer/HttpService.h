//
// Created by zhangweiwen on 18-1-20.
//

#ifndef ZHS_HTTPSERVICE_H
#define ZHS_HTTPSERVICE_H


#include <zhs/net/zMNetService.h>
#include <zhs/misc/zArgs.h>
#include <zhs/net/http/zHttpTaskPool.h>
#include <zhs/script/zLuaSystem.h>
#include <zhs/script/zLuaBinder.h>

/**
 *
 */
class HttpService : public zMNetService
{
public:
    /**
     * 析构函数
     */
    ~HttpService() override
    {
        serviceInstance = nullptr;

        SAFE_DELETE(httpTaskPool);
    }

    /**
     * 返回唯一实例对象
     *
     * @return 实例对象
     */
    static HttpService& getInstance()
    {
        if (nullptr == serviceInstance) {
            serviceInstance = new HttpService();
        }

        return *serviceInstance;
    }

    /**
     * 销毁唯一实例对象
     */
    static void destroyInstance()
    {
        SAFE_DELETE(serviceInstance);
    }

    /**
     * 获取服务类型
     *
     * @return 服务类型
     */
    const WORD getServerType() override
    {
        return HTTP_SERVER;
    }

    /**
     * 设置网页文件主目录
     */
    static void setHtdocs(std::string &filepath)
    {
        char buf[512];

        htdocs = std::string(getcwd(buf, sizeof(buf))) + filepath;
    }

    /**
     * 返回网页文件主目录
     */
    static const std::string &getHtdocs()
    {
        return htdocs;
    }

    /**
     * 设置默认文件
     */
    static void setIndex(std::string &filename)
    {
        index = filename;
    }

    /**
     * 返回默认文件
     */
    static const std::string &getIndex()
    {
        return index;
    }

    void loadConfig() override ;

    static char doc[];
    static struct argp_option options[];
    static argp_parser_t parser;

protected:

    /**
     * 网页文件主目录
     */
    static std::string htdocs;

    /**
     * 默认文件
     */
    static std::string index;

private:

    /**
     * 监听端口
     */
    unsigned short server_port {80};

    /**
     * 线程池
     */
    zHttpTaskPool *httpTaskPool;

    /**
     * 类的唯一实例指针
     */
    static HttpService *serviceInstance;

    /**
     * Lua虚拟机
     */
    zLuaVM *luaVM;

    bool init() override ;
    void stop() override ;
    void createTCPTask(int, unsigned short) override ;

    /**
     * 构造函数
     */
    HttpService() : zMNetService("HttpService")
    {
        httpTaskPool = nullptr;
        luaVM = nullptr;
    }
};


#endif //ZHS_HTTPSERVICE_H
