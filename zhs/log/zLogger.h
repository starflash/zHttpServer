//
// Created by zhangweiwen on 2016/8/18.
//

#ifndef ZHS_ZLOGGER_H
#define ZHS_ZLOGGER_H

#include <cstring>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/helpers/timezone.h>
#include <log4cxx/dailyrollingfileappender.h>

#include <zhs/thread/zMutex.h>

using namespace log4cxx;
using log4cxx::helpers::TimeZonePtr;
using log4cxx::helpers::TimeZone;
using log4cxx::helpers::Pool;

/**
 * 日志类，以Log4cxx基础构建的。
 * 目前实现了三种写日志方式，即控制台、本地文件和syslog系统，Syslog的等级为USER。
 * 默认日志级别是DEBUG
 * 此类为线程安全类。
 */
class zLogger
{
public:

    /**
     * zLevel声明了几个日志等级
     * 除了用log4cxx提供的标准日志等级作为日志等级外，还自定义了服务器日志等级。
     * 程序日志等级关系为 OFF > FATAL > ERROR > WARN > INFO > DEBUG > ALL
     * 服务器日志等级关系为 ALARM > IFFY > GBUG
     * 服务器日志等级与程序日志等级关系: ALARM=ERROR, IFFY=WARN, GBUG=DEBUG
     */
    class zLevel
    {
    private:

        // 友元类
        friend class zLogger;

        // 日志等级
        LevelPtr _level;

        /**
         * 日志等级数字定义
         */
        enum zLevelInt {
            ALARM_INT = Level::ERROR_INT,
            IFFY_INT  = Level::WARN_INT,
            GBUG_INT  = Level::DEBUG_INT
        };

        static const LevelPtr LEVELALARM;
        static const LevelPtr LEVELIFFY;
        static const LevelPtr LEVELGBUG;

        /**
         * 构造函数，初始化日志等级。
         *
         * @param LevelPtr level
         */
        explicit zLevel(const LevelPtr &level): _level(level)
        {}

    public:

        /**
         * 当zLogger等级设置为OFF，除了用调forceLog函数，否则不会输出任何日志。
         */
        static const zLevel *OFF;

        /**
         * 当zLogger等级设置为FATAL，只输出FATAL等级的日志。
         * 程序致命错误，已经无法提供正常的服务功能。
         */
        static const zLevel *FATAL;

        /**
         * 当zLogger等级设置为ERROR，只输出大于等于此等级的日志。
         * 错误，可能不能提供某种服务，但可以保证程序正确运行。
         */
        static const zLevel *ERROR;

        /**
         * 当zLogger等级设置为ALARM，与ERROR同一级别。
         * 报警，服务器数据发生错误，比如检测到有非法连接，服务器数据异常等等。与ERROR同一级别。
         */
        static const zLevel *ALARM;

        /**
         * 当zLogger等级设置为WARN，只输出大于等于此等级的日志。
         * 警告，某些地方需要引起注意，比如没有配置文件，但程序用默认选项可以使用。
         */
        static const zLevel *WARN;

        /**
         * 当zLogger等级设置为IFFY。
         * 可疑的，需要追查的一些服务器数据，比如说一个变动的数据超出某种范围。与WARN同一级别。
         */
        static const zLevel *IFFY;

        /**
         * 当zLogger等级设置为INFO，只输出大于等于此等级的日志。
         * 信息，提供一般信息记录，多用于一些程序状态的记录。
         */
        static const zLevel *INFO;

        /**
         * 当zLogger等级设置为TRACE，只输出大于等于此等级的日志。
         * 服务器数据跟踪，对服务器中所有的关键数据跟踪，便于日后查找问题。
         * 与INFO同一级别。
         */
        static const zLevel *TRACE;

        /**
         * 当zLogger等级设置为DEBUG，输出所有等级的日志。
         */
        static const zLevel *DEBUG;

        /**
         * 当zLogger等级设置为GBUG，只输出大于等于此等级的日志。
         * 调试用的服务器数据。与DEBUG同一级别。
         */
        static const zLevel *GBUG;

        /**
         * 当zLogger等级设置为ALL，输出所有等级的日志。
         */
        static const zLevel *ALL;
    };

    /**
     * 构造函数，创建一个log4cxx日志对象。
     *
     * @param name  日志的名字，将会出现在日志中的每一行。
     * @param level 日志等级
     */
    explicit zLogger(const std::string &name, const std::string &level = "debug");

    /**
     * 析构函数
     */
    ~zLogger() = default;

    /**
     * 返回日志的名字
     *
     * @return 日志名字
     */
    const std::string getName() { return _logger->getName(); }

    /**
     * 设置日志的名字
     *
     * @param name 日志名字
     */
    void setName(const std::string &name) { ((zLoggerExPtr) _logger)->setName(name); }

    bool addConsoleLog();
    void removeConsoleLog();
    bool addLocalFileLog(const std::string &);
    void removeLocalFileLog(const std::string &);
    bool addSysLog(const std::string &);
    void removeSysLog(const std::string &);

    const zLevel *getLevel(const std::string &);
    void setLevel(const zLevel *);
    void setLevel(const std::string &);
    bool forceLog(const zLevel *, const char *, ...);
    bool log(const zLevel *, const char *, ...);
    bool debug(const char *, ...);
    bool error(const char *, ...);
    bool info(const char *, ...);
    bool fatal(const char *, ...);
    bool warn(const char *, ...);
    bool alarm(const char *, ...);
    bool iffy(const char *, ...);
    bool trace(const char *, ...);
    bool gbug(const char *, ...);
    bool luainfo(const char *);

private:

    /**
     * 本类只为了能以正确的时间写文件名，而从log4cxx::DailyRollingFileAppender继承实现的。
     * 增加了setTimeZone函数和重构了activateOptions函数，其他的用法请参见log4cxx手册。
     */
    class zLoggerLocalFileAppender : public DailyRollingFileAppender
    {
    private:

        /**
         * 写日志文件名所用的时区
         */
        static TimeZonePtr tz;

    public:

        zLoggerLocalFileAppender();
        ~zLoggerLocalFileAppender() override ;

        void setTimeZone(const std::string &);
        void activateOptions(Pool &) override ;
    };

    /**
     * 扩展Logger的功能
     */
    class zLoggerEx;
    LOG4CXX_PTR_DEF(zLoggerEx);

    /**
     *
     */
    class zLoggerEx : public Logger
    {
    public:

        /**
         * 设置日志的名字
         *
         * @param log_name 日志名字
         */
        void setName(const std::string &log_name)
        {
            name = log_name;
        }

    protected:

        /**
         * 构造函数
         *
         * @param name 日志名字
         */
        zLoggerEx(Pool &pool, const std::string &name) : Logger(pool, name)
        {}
    };

    static const int MESSAGE_SIZE = 4096;
    char _message[MESSAGE_SIZE] {};

    LoggerPtr _logger;
    zMutex _mutex;
};

#endif //ZHS_ZLOGGER_H
