//
// Created by zhangweiwen on 17-1-12.
//

#ifndef ZHS_ZMATH_H
#define ZHS_ZMATH_H

#include "zhs/zhs.h"

/**
 * 数学扩展
 */
class zMath
{
public:

    /**
     * 产生一定范围内的随机数，包含最大最小值。
     *
     * @tparam T    参数的类型
     * @tparam SEED 随机数种子
     * @param min   最小值
     * @param max   最大值
     * @return      随机数
     */
    template <typename T, unsigned int *SEED = &Zhs::seedp>
    static T rand(T min, T max)
    {
        if (min == max) {
            return min;
        } else if (min > max)
            return max + (T) (((double) min - (double) max + 1.0) * rand_r(SEED) / (RAND_MAX + 1.0));
        else {
            return min + (T) (((double) max - (double) min + 1.0) * rand_r(SEED) / (RAND_MAX + 1.0));
        }
    }

    /**
     * 产生一定范围内的随机数，包含最大最小值。
     *
     * @tparam T1   参数1的类型
     * @tparam T2 　参数2的类型
     * @tparam SEED 随机数种子
     * @param min   最小值
     * @param max   最大值
     * @return      随机数
     */
    template <typename T1, typename T2, unsigned int *SEED = &Zhs::seedp>
    static auto rand(T1 min, T2 max) -> decltype(min + max)
    {
        if (min == max) {
            return min;
        } else if (min > max)
            return max + (T1) (((double) min - (double) max + 1.0) * rand_r(SEED) / (RAND_MAX + 1.0));
        else {
            return min + (T1) (((double) max - (double) min + 1.0) * rand_r(SEED) / (RAND_MAX + 1.0));
        }
    }
};

#endif //ZHS_ZMATH_H
