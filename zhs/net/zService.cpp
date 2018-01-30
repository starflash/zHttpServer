//
// Created by zhangweiwen on 2016/8/26.
//

#include <vector>
#include <string>
#include <csignal>
#include <unistd.h>
#include <ctime>

#include <zhs/base/zString.h>
#include <zhs/zhs.h>
#include "zService.h"

zService *zService::serviceInstance = nullptr;

/**
 * CTRL + C 等信号的处理函数：结束程序。
 *
 * @param signo 信号
 */
static void ctrlc_handler(int signo)
{
    Zhs::logger->trace("Ctrl + C Handler");

    zService::getInstance()->terminate();
}

/**
 * HUP信号的处理函数：重新读入配置文件。
 *
 * @param signo 信号
 */
static void hup_handler(int signo)
{
    Zhs::logger->trace("HUP Handler");

    zService::getInstance()->loadConfig();
}

/**
 * 初始化服务器
 *
 * １.保存环境变量 2.设置信号量处理程序 3.设置随机数种子
 *
 * @return 初始化是否成功
 */
bool zService::init()
{
    Zhs::logger->trace("zService::init");

    int i = 0;

    // 存储环境变量
    while (environ[i]) {
        std::string s(environ[i++]);
        std::vector<std::string> v;

        Zhs::string_token(v, s, "=", 1);
        if (!v.empty() && v.size() == 2) {
            env[v[0]] = v[1];
        }
    }

    // 设置信号处理程序
    struct sigaction act {};

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = ctrlc_handler;
    sigaction(SIGINT, &act, nullptr);
    sigaction(SIGQUIT, &act, nullptr);
    sigaction(SIGABRT, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);

    act.sa_handler = hup_handler;
    sigaction(SIGHUP, &act, nullptr);

    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &act, nullptr);

    srand(static_cast<unsigned int>(time(nullptr)));
    Zhs::seedp = static_cast<unsigned int>(time(nullptr));

    return true;
}

/**
 * 启动服务器程序
 */
void zService::run()
{
    Zhs::logger->trace("zService::run");

    // 初始化程序，并确认服务器启动成功。
    if (this->init() && this->validate()) {

        while (!this->isTerminated()) {

            if (!this->callback()) {
                break;
            }
        }

        Zhs::logger->debug("zService terminate");
    }
    this->stop(); // 结束程序，释放相应的资源。
}