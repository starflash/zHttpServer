//
// Created by zhangweiwen on 2016/8/17.
//

#ifndef ZHS_ZREALTIME_H
#define ZHS_ZREALTIME_H

#include <ctime>
#include <sys/time.h>
#include <sstream>

/**
 * 真实时间类，对timeval结构简单封装。
 * 提供一些常用时间函数，时间精度精确到毫秒。
 */
class zRealTime
{
private:

    /**
     * 真实时间换算为毫秒
     */
    unsigned long long _msecs {};

public:

    /**
     * 构造函数
     *
     * @param offset 相对于现在时间的延时，单位毫秒。
     */
    explicit zRealTime(const int offset = 0)
    {
        now();
        delay(offset);
    }

    /**
     * 复制构造函数
     *
     * @param rt 待复制的对象
     */
    zRealTime(const zRealTime &rt)
    {
        _msecs = rt._msecs;
    }

    /**
     * 得到当前真实时间，单位为毫秒。
     */
    void now()
    {
        unsigned long long retval = 0LL;
        struct timeval tv {};

        gettimeofday(&tv, nullptr);
        retval = (unsigned long long) tv.tv_sec;
        retval *= 1000;
        retval += tv.tv_usec / 1000;

        _msecs = retval;
    }

    /**
     * 返回秒数
     *
     * @return 秒数
     */
    unsigned long sec() const
    {
        return _msecs / 1000;
    }

    /**
     * 返回毫秒数
     *
     * @return 毫秒数
     */
    unsigned long msec() const
    {
        return _msecs % 1000;
    }

    /**
     * 返回总共的毫秒数
     *
     * @return 总共的毫秒数
     */
    unsigned long long msecs() const
    {
        return _msecs;
    }

    /**
     * 加延迟偏移量
     *
     * @param offset 延迟偏移时间，可以为负数，单位毫秒。
     */
    void delay(int offset)
    {
        _msecs += offset;
    }

    /**
     * 重载()运算符
     */
    void operator()()
    {
        now();
    }

    /**
     * 重载=运算符
     */
    zRealTime& operator=(const zRealTime &rt)
    {
        _msecs = rt._msecs;
        return *this;
    }

    /**
     * 重构+运算符
     */
    const zRealTime& operator+(const zRealTime &rt)
    {
        _msecs += rt._msecs;
        return *this;
    }

    /**
     * 重构-运算符
     */
    const zRealTime& operator-(const zRealTime &rt)
    {
        _msecs -= rt._msecs;
        return *this;
    }

    /**
     * 重构>运算符
     */
    bool operator>(const zRealTime &rt) const
    {
        return _msecs > rt._msecs;
    }

    /**
     * 重构>=运算符
     */
    bool operator>=(const zRealTime &rt) const
    {
        return _msecs >= rt._msecs;
    }

    /**
     * 重构<运算符
     */
    bool operator<(const zRealTime &rt) const
    {
        return _msecs < rt._msecs;
    }

    /**
     * 重构<=运算符
     */
    bool operator<=(const zRealTime &rt) const
    {
        return _msecs <= rt._msecs;
    }

    /**
     * 重构==运算符
     */
    bool operator==(const zRealTime &rt) const
    {
        return _msecs == rt._msecs;
    }

    /**
     * 计算目标时间与当前时间的时间差，单位为毫秒。
     * 如果目标时间早于当前时间，则返回零。
     *
     * @param rt 目标时间
     * @return   时间差
     */
    unsigned long long elapse(const zRealTime &rt) const
    {
        return rt._msecs > _msecs ? rt._msecs - _msecs : 0LL;
    }

    /**
     * 得到本地时区字符串
     *
     * @param tz 本地时区
     * @return   本地时区字符串
     */
    static std::string& getLocalTz(std::string &tz)
    {
        std::ostringstream oss;

        tzset();
        oss << tzname[0] << timezone / 3600;
        tz = oss.str();

        return tz;
    }

    /**
     * 把时间戳转换成本地时间（北京时间）
     *
     * @param tv   转换后的本地时间
     * @param time 时间戳
     */
    static void getLocalTime(struct tm &tv, time_t time)
    {
        time += 8 * 60 * 60;
        tv = *gmtime(&time);
    }
};

#endif //ZHS_ZREALTIME_H
