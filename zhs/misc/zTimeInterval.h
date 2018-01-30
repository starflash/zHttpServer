//
// Created by zhangweiwen on 16-12-23.
//

#ifndef ZHS_ZTIMEINTERVAL_H
#define ZHS_ZTIMEINTERVAL_H

#include "zRealTime.h"

/**
 * 时间间隔工具类
 */
class zTimeInterval
{
public:

    /**
     * 构造函数
     *
     * @param delay  延迟时间，秒。
     * @param offset 相对于当前时间的偏移时间，秒。
     */
    explicit zTimeInterval(const float delay, const int offset = 0) : interval(static_cast<int> (1000 * delay)), realTime(1000 * offset)
    {}

    /**
     * 构造函数
     *
     * @param delay 延迟时间，毫秒。
     * @param rt    真实时间对象
     */
    zTimeInterval(const float delay, const zRealTime rt) : interval(static_cast<int> (1000 * delay)), realTime(rt)
    {
        realTime.delay(interval);
    }

    /**
     * 重置对象时间
     *
     * @param rt 目标时间
     */
    void replace(const zRealTime &rt)
    {
        realTime = rt;
        realTime.delay(interval);
    }

    /**
     * 重载()运算符，比较目标时间是否晚于对象时间，并重置对象时间。
     *
     * @param rt 目标时间
     * @return   是否比对象时间晚
     */
    bool operator()(const zRealTime &rt)
    {
        if (rt >= realTime) {
            replace(rt);

            return true;
        }

        return false;
    }

private:

    // 间隔时间，毫秒。
    int interval;

    // 真实时间
    zRealTime realTime {};
};

#endif //ZHS_ZTIMEINTERVAL_H
