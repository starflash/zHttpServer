//
// Created by zhangweiwen on 18-1-20.
//

#include "HttpService.h"
#include "HttpConfig.h"
#include "ServerTask.h"

HttpService *HttpService::serviceInstance = nullptr;
std::string HttpService::htdocs;
std::string HttpService::index;

/**
 * 初始化网络服务器
 *
 * @return 是否成功
 */
bool HttpService::init()
{
    Zhs::logger->trace("HttpService::init");

    if (!zMNetService::init()) {
        return false;
    }

    httpTaskPool = new zHttpTaskPool();
    if (nullptr == httpTaskPool || !httpTaskPool->init(Zhs::global["interval"])) {
        return false;
    }

    server_port = (unsigned short) std::stoul(Zhs::global["port"]);
    if (!zMNetService::bind("HttpServer", server_port))
    {
        return false;
    }

    setHtdocs(Zhs::global["htdocs"]);
    setIndex(Zhs::global["index"]);

    // 初始化Lua引擎
    luaVM = zLuaSystem::getInstance()->createLuaVM();
    luaVM->bindModules();
    luaVM->execute(zLuaSystem::getInstance()->createLuaScript(Zhs::global["script"]));
    luaVM->call("initialize");

    Zhs::logger->debug("HttpService initialize done");

    return true;
}

/**
 * 创建一个TCP连接任务
 *
 * @param sockfd 连接套接字
 * @param port   端口号
 */
void HttpService::createTCPTask(const int sockfd, const unsigned short port)
{
    Zhs::logger->trace("HttpService::createTCPTask");

    if (port == server_port) {
        try {
            zHttpTask *task = new ServerTask(httpTaskPool, sockfd);
            if (!httpTaskPool->addHttp(task)) {
                SAFE_DELETE(task);
            }
        } catch (std::bad_alloc &e) {
            TEMP_FAILURE_RETRY(close(sockfd));
        }
    } else {
        TEMP_FAILURE_RETRY(close(sockfd));
    }
}

/**
 * 停止网络服务器
 */
void HttpService::stop()
{
    Zhs::logger->trace("HttpService::stop");

    zMNetService::stop();
}

/**
 * 读取配置文件
 * 是HUP信号的处理函数
 */
void HttpService::loadConfig()
{
    Zhs::logger->trace("HttpService::loadConfig");

    HttpConfig config;
    config.parse("HttpServer");
}

// 定义简短描述信息
char HttpService::doc[] = "\nHttpServer\n";

// 定义命令行参数
struct argp_option HttpService::options[] = {
        {"daemon",		'd',	nullptr,	0,	"Run service as daemon",	0},
        {"log",			'l',	"level",	0,	"Log level",				0},
        {"logfilename",	'f',	"filename",	0,	"Log file name",			0},
        {"ifname",		'i',	"ifname",	0,	"Local network device",		0},
        {"server",		's',	"ip",		0,	"Http server ip address",	0},
        {"port",		'p',	"port",		0,	"Http server port number",	0},
        {"interval",    't',    "interval", 0,  "Thread interval time",     0},
        {"htdocs",      'h',    "htdocs",   0,  "Html files directory",     0},
        {"index",       'n',    "index",    0,  "Default index filename",   0},
        {nullptr,		'\0',	nullptr,	0,	nullptr,					0}
};

// 命令行参数解析器
argp_parser_t HttpService::parser =
        [&](int key, char *arg, struct argp_state *state)->error_t
        {
            switch (key) {
                case 'd':
                    Zhs::global["daemon"] = "true";
                    break;
                case 'p':
                    Zhs::global["port"] = arg;
                    break;
                case 's':
                    Zhs::global["server"] = arg;
                    break;
                case 'l':
                    Zhs::global["log"] = arg;
                    break;
                case 'f':
                    Zhs::global["logfilename"] = arg;
                    break;
                case 'i':
                    Zhs::global["ifname"] = arg;
                    break;
                case 't':
                    Zhs::global["interval"] = arg;
                    break;
                case 'h':
                    Zhs::global["htdocs"] = arg;
                    break;
                case 'n':
                    Zhs::global["index"] = arg;
                    break;
                default:
                    return ARGP_ERR_UNKNOWN;
            }

            return 0;
        };

/**
 * 程序入口
 *
 * @param argc 参数个数
 * @param argv 参数列表
 * @return     退出码
 */
int main(int argc, char **argv)
{
    Zhs::logger = new zLogger("HttpServer");

    // 设置缺省参数
    Zhs::global["interval"] = "50000";
    Zhs::global["port"] = "80";
    Zhs::global["log"] = "debug";
    Zhs::global["logfilename"] = "./log/httpserver.log";
    Zhs::global["htdocs"] = "/htdocs";
    Zhs::global["index"] = "index.html";
    Zhs::global["script"] = "./scripts/autoload.lua";

    // 解析配置文件参数
    HttpConfig config;

    if (!config.parse("HttpServer")) {
        return EXIT_FAILURE;
    }

    // 解析命令行参数
    zArgs::getInstance()->add(HttpService::options, HttpService::parser, nullptr, HttpService::doc);
    zArgs::getInstance()->parse(argc, argv);

    // 设置日志级别
    Zhs::logger->setLevel(Zhs::global["log"]);
    // 设置写本地日志文件
    if (!Zhs::global["logfilename"].empty()) {
        Zhs::logger->addLocalFileLog(Zhs::global["logfilename"]);
    }

    // 是否以守护进程的方式运行
    if (Zhs::global["daemon"] == "true") {
        Zhs::logger->info("Program will be run as a daemon");
        Zhs::logger->removeConsoleLog();

        if (-1 == daemon(1, 1)) {
            return EXIT_FAILURE;
        }
    }

    HttpService::getInstance().run();
    HttpService::destroyInstance();

    SAFE_DELETE(Zhs::logger);

    return EXIT_SUCCESS;
}