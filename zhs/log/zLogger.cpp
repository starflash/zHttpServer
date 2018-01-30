//
// Created by zhangweiwen on 2016/8/18.
//

#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <log4cxx/patternlayout.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/helpers/dateformat.h>
#include <log4cxx/net/syslogappender.h>

#include <zhs/base/zCommon.h>
#include <zhs/misc/zRealTime.h>
#include "zLogger.h"

using namespace log4cxx::net;

#define _T(str) str

const LevelPtr zLogger::zLevel::LEVELALARM(new Level(ALARM_INT, _T("ALARM"), 3));
const LevelPtr zLogger::zLevel::LEVELIFFY(new Level(IFFY_INT, _T("IFFY"), 3));
const LevelPtr zLogger::zLevel::LEVELGBUG(new Level(GBUG_INT, _T("GBUG"), 3));

const zLogger::zLevel*  zLogger::zLevel::OFF = new zLevel(Level::getOff());
const zLogger::zLevel*  zLogger::zLevel::FATAL = new zLevel(Level::getFatal());
const zLogger::zLevel*  zLogger::zLevel::ALARM = new zLevel(LEVELALARM);
const zLogger::zLevel*  zLogger::zLevel::ERROR = new zLevel(Level::getError());
const zLogger::zLevel*  zLogger::zLevel::IFFY = new zLevel(LEVELIFFY);
const zLogger::zLevel*  zLogger::zLevel::WARN = new zLevel(Level::getWarn());
const zLogger::zLevel*  zLogger::zLevel::TRACE = new zLevel(Level::getTrace());
const zLogger::zLevel*  zLogger::zLevel::INFO = new zLevel(Level::getInfo());
const zLogger::zLevel*  zLogger::zLevel::GBUG = new zLevel(LEVELGBUG);
const zLogger::zLevel*  zLogger::zLevel::DEBUG = new zLevel(Level::getDebug());
const zLogger::zLevel*  zLogger::zLevel::ALL = new zLevel(Level::getAll());

//
#define getMessage(message, len, pattern) \
do { \
    va_list ap; \
    memset(message, 0, len); \
    va_start(ap, pattern); \
    vsnprintf(message, ((len) - 1), pattern, ap); \
    va_end(ap); \
} while (false)

// 初始化时区字段
TimeZonePtr zLogger::zLoggerLocalFileAppender::tz(TimeZone::getDefault());

// 构造函数，创建一个本地文件Appender。
zLogger::zLoggerLocalFileAppender::zLoggerLocalFileAppender() = default;

// 析构函数，回收父类中df指向的内存。
zLogger::zLoggerLocalFileAppender::~zLoggerLocalFileAppender() = default;

/**
 * 设置时区
 *
 * @param timeZone 时区字符串
 */
void zLogger::zLoggerLocalFileAppender::setTimeZone(const std::string &timeZone) {
    tz = TimeZone::getTimeZone(timeZone);
}

/**
 * 激活所设置的选项
 *
 * @param pool 内存池
 */
void zLogger::zLoggerLocalFileAppender::activateOptions(Pool &pool)
{
    std::string ltz;
    zRealTime::getLocalTz(ltz);

    char otz[64];
    strncpy(otz, "TZ=", 3);
    strncpy(otz + 3, ltz.c_str(), 60);

    DailyRollingFileAppender::activateOptions(pool);

    // 由于DailyRollingFileAppender::activateOptions会修改时区环境变量，故执行完后再修改回来。
    putenv(otz);
    tzset();
}

/**
 * 添加控制台输出日志
 *
 * @return true
 */
bool zLogger::addConsoleLog()
{
    Pool pool;
    PatternLayoutPtr cpl = new PatternLayout("%d{yyy-MM-dd HH:mm:ss,SSS }%c %5p: %m%n");
    cpl->activateOptions(pool);

    ConsoleAppenderPtr cap = new ConsoleAppender(cpl);
    cap->setName("console");

    _logger->addAppender(cap);

    return true;
}

/**
 * 移除控制台输出日志
 */
void zLogger::removeConsoleLog()
{
    AppenderPtr ap = _logger->getAppender("console");
    _logger->removeAppender(ap);
    ap->close();
}

/**
 * 添加本地文件输出日志
 *
 * @param file 输出的文件名，会自动添加时间后缀。
 * @return     true
 */
bool zLogger::addLocalFileLog(const std::string &file)
{
    Pool pool;
    PatternLayoutPtr fpl = new PatternLayout("%d{yyy-MM-dd HH:mm:ss,SSS }%c %5p: %m%n");
    fpl->activateOptions(pool);

    auto fap = new zLoggerLocalFileAppender();
    fap->setDatePattern(".%Y-%m-%d %H");
    fap->setFile(file);
    fap->setLayout(fpl);
    fap->activateOptions(pool);
    fap->setName("localfile:" + file);

    _logger->addAppender(fap);

    return true;
}

/**
 * 移除本地文件输出日志
 *
 * @param file 要移除的日志文件名
 */
void zLogger::removeLocalFileLog(const std::string &file)
{
    AppenderPtr ap = _logger->getAppender("localfile");
    _logger->removeAppender(ap);
    ap->close();
}

/**
 * 添加系统syslog输出日志
 *
 * @param host 系统的syslong服务器地址，默认为本机。
 * @return     true
 */
bool zLogger::addSysLog(const std::string &host)
{
    Pool pool;
    PatternLayoutPtr spl = new PatternLayout("%c %5p: %m%n");

    SyslogAppenderPtr sap = new SyslogAppender(spl, SyslogAppender::getFacility("user"));
    sap->setName(host);
    sap->setSyslogHost(host);
    sap->activateOptions(pool);
    _logger->addAppender(sap);

    return true;
}

/**
 * 移除系统syslog输出日志
 *
 * @param host 要移除的syslong服务器地址，默认为本机。
 */
void zLogger::removeSysLog(const std::string &host)
{
    AppenderPtr ap = _logger->getAppender(host);
    _logger->removeAppender(ap);
    ap->close();
}

/**
 * 返回日志等级对象
 *
 * @param level 日志等级字符串
 * @return      日志等级对象指针
 */
const zLogger::zLevel* zLogger::getLevel(const std::string &level)
{
    if ("off" == level) {
        return zLevel::OFF;
    } else if ("fatal" == level) {
        return zLevel::FATAL;
    } else if ("alarm" == level) {
        return zLevel::ALARM;
    } else if ("error" == level) {
        return zLevel::ERROR;
    } else if ("iffy" == level) {
        return zLevel::IFFY;
    } else if ("warn" == level) {
        return zLevel::WARN;
    } else if ("info" == level) {
        return zLevel::INFO;
    } else if ("gbug" == level) {
        return zLevel::GBUG;
    } else if ("debug" == level) {
        return zLevel::DEBUG;
    } else if ("trace" == level) {
        return zLevel::TRACE;
    } else if ("all" == level) {
        return zLevel::ALL;
    }

    return nullptr;
}

/**
 * 设置日志等级
 *
 * @param zLevelPtr 指向日志等级对象
 */
void zLogger::setLevel(const zLevel *zLevelPtr)
{
    _logger->setLevel(zLevelPtr->_level);
}

/**
 * 设置日志等级
 *
 * @param level 日志等级字符串
 */
void zLogger::setLevel(const std::string &level)
{
    setLevel(getLevel(level));
}

/**
 * 写日志
 * 
 * @param zLevelPtr 指向日志等级对象
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::log(const zLevel *zLevelPtr, const char *pattern, ...)
{
	_mutex.lock();
	getMessage(_message, MESSAGE_SIZE, pattern);
	_logger->log(zLevelPtr->_level, _message);
	_mutex.unlock();

	return true;
}

/**
 * 强制写日志，不受日志等级限制。
 * 
 * @param zLevelPtr 指向日志等级对象
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::forceLog(const zLevel *zLevelPtr, const char *pattern, ...)
{
	_mutex.lock();
	getMessage(_message, MESSAGE_SIZE, pattern);
	_logger->forcedLog(zLevelPtr->_level, _message);
	_mutex.unlock();

	return true;
}

/**
 * 写fatal程序日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::fatal(const char *pattern, ...)
{
    _mutex.lock();
    getMessage(_message, MESSAGE_SIZE, pattern);
    _logger->fatal(_message);
    _mutex.unlock();

    return true;
}

/**
 * 写error程序日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::error(const char *pattern, ...)
{
    _mutex.lock();
    getMessage(_message, MESSAGE_SIZE, pattern);
    _logger->error(_message);
    _mutex.unlock();

    return true;
}

/**
 * 写warn程序日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::warn(const char *pattern, ...)
{
    _mutex.lock();
    getMessage(_message, MESSAGE_SIZE, pattern);
    _logger->warn(_message);
    _mutex.unlock();

    return true;
}

/**
 * 写info程序日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::info(const char *pattern, ...)
{
    _mutex.lock();
    getMessage(_message, MESSAGE_SIZE, pattern);
    _logger->info(_message);
    _mutex.unlock();

    return true;
}

/**
 * 写debug程序日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::debug(const char *pattern, ...)
{
    _mutex.lock();
    getMessage(_message, MESSAGE_SIZE, pattern);
    _logger->debug(_message);
    _mutex.unlock();

    return true;
}

/**
 * 写alarm日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::alarm(const char *pattern, ...)
{
	_mutex.lock();
	getMessage(_message, MESSAGE_SIZE, pattern);
	_logger->log(zLevel::ALARM->_level, _message);
	_mutex.unlock();

	return true;
}

/**
 * 写iffy日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::iffy(const char *pattern, ...)
{
	_mutex.lock();
	getMessage(_message, MESSAGE_SIZE, pattern);
	_logger->log(zLevel::IFFY->_level, _message);
	_mutex.unlock();

	return true;
}

/**
 * 写trace日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::trace(const char *pattern, ...)
{
    _mutex.lock();
    getMessage(_message, MESSAGE_SIZE, pattern);
    _logger->trace(_message);
    _mutex.unlock();

    return true;
}

/**
 * 写gbug日志
 *
 * @param pattern   输出格式
 * @param ...       输出格式需要的参数
 * @return          true
 */
bool zLogger::gbug(const char *pattern, ...)
{
	_mutex.lock();
	getMessage(_message, MESSAGE_SIZE, pattern);
	_logger->log(zLevel::GBUG->_level, _message);
	_mutex.unlock();

	return true;
}

/**
 * 写lua日志
 *
 * @param pattern 输出格式
 * @return        true
 */
bool zLogger::luainfo(const char *pattern)
{
    _logger->info(pattern);

    return true;
}

/**
 * 构造函数
 *
 * @param name  日志名称
 * @param level 日志等级
 */
zLogger::zLogger(const std::string &name, const std::string &level)
{
    memset(_message, 0, sizeof(_message));

    _logger = Logger::getLogger(name);
    _logger->setLevel(getLevel(level)->_level);

    addConsoleLog();
}