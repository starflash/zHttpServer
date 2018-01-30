//
// Created by zhangweiwen on 2016/8/17.
//

#ifndef ZHS_ZTIME_H
#define ZHS_ZTIME_H

#include <ctime>
#include <sys/time.h>

#include "zRealTime.h"

/**
 * 时间类，对tm结构简单封装。
 */
class zTime
{
public:

    /**
     * 时间戳转本地的日期时间字符串
     *
     * @param time   时间戳
     * @param out    输换后字符串
     * @param len    输换后字符串长度
     * @param format 格式
     * @return       输换后字符串
     */
    static char* getLocalDatetime(time_t time, char *out, const int len, const char *format)
    {
        struct tm localtime {};

        localtime_r(&time, &localtime);
        strftime(out, (size_t) len, format, &localtime);

        return out;
    }

    /**
     * 构造函数
     */
    zTime()
    {
        time(&secs);
        zRealTime::getLocalTime(tv, secs);
    }

    /**
     * 复制构造函数
     */
    zTime(const zTime &ct)
    {
        secs = ct.secs;
        zRealTime::getLocalTime(tv, secs);
    }

    /**
     * 获取当前时间
     */
    void now()
    {
        time(&secs);
        zRealTime::getLocalTime(tv, secs);
    }

    /**
     * 返回存储的时间
     *
     * @return 秒
     */
    time_t sec() const
    {
        return secs;
    }

    /**
     * 重载=运算符
     */
    zTime& operator=(const zTime &rt)
    {
        secs = rt.secs;
        return *this;
    }

    /**
     * 重载+运算符
     */
    const zTime& operator+(const zTime &rt)
    {
        secs += rt.secs;
        return *this;
    }

    /**
     * 重载-运算符
     */
    const zTime& operator-(const zTime &rt)
    {
        secs -= rt.secs;
        return *this;
    }

    /**
     * 重载-=运算符
     */
    const zTime& operator-=(const time_t s)
    {
        secs -= s;
        return *this;
    }

    /**
     * 重载>运算符
     */
    bool operator>(const zTime &rt) const
    {
        return secs > rt.secs;
    }

    /**
     * 重载>=运算符
     */
    bool operator>=(const zTime &rt) const
    {
        return secs >= rt.secs;
    }

    /**
     * 重载<运算符
     */
    bool operator<(const zTime &rt) const
    {
        return secs < rt.secs;
    }

    /**
     * 重载<=运算符
     */
    bool operator<=(const zTime &rt) const
    {
        return secs <= rt.secs;
    }

    /**
     * 重载==运算符
     */
    bool operator==(const zTime &rt) const
    {
        return secs == rt.secs;
    }

    /**
     * 计算目标时间与对象时间的时间差，单位为秒。
     * 如果目标时间早于对象时间，则返回零。
     *
     * @param rt 目标时间
     * @return   时间差
     */
    time_t elapse(const zTime &rt) const
    {
        return rt.secs > secs ? rt.secs - secs : 0;
    }

    /**
     * 计算对象时间与当前时间的时间差，单位为秒。
     *
     * @return 时间差
     */
    time_t elapse() const
    {
        zTime rt;
        return (rt.secs - secs);
    }

    /**
     * 获取秒，范围0-59。
     *
     * @return 秒
     */
    int getSec()
    {
        return tv.tm_sec;
    }

    /**
     * 获取分钟，范围0-59。
     *
     * @return 分钟
     */
    int getMin()
    {
        return tv.tm_min;
    }

    /**
     * 获取小时，范围0-23。
     *
     * @return 小时
     */
    int getHour()
    {
        return tv.tm_hour;
    }

    /**
     * 获取天数，范围1-31。
     *
     * @return 天
     */
    int getMDay()
    {
        return tv.tm_mday;
    }

    /**
     * 获取星期几，范围1-7。
     *
     * @return 星期几
     */
    int getWDay()
    {
        return tv.tm_wday;
    }

    /**
     * 获取月份，范围1-12。
     *
     * @return 月份
     */
    int getMonth()
    {
        return tv.tm_mon + 1;
    }

    /**
     * 获取年份
     *
     * @return 年份
     */
    int getYear()
    {
        return tv.tm_year + 1900;
    }

private:

    /**
     * 存储时间，单位秒。
     */
    time_t secs {};

    /**
     * tm结构体，方便访问。
     */
    struct tm tv {};
};

#endif //ZHS_ZTIME_H
