//
// Created by zhangweiwen on 2016/8/17.
//

#ifndef ZHS_ZNONCOPYABLE_H
#define ZHS_ZNONCOPYABLE_H

/**
 * 所有继承的子类禁止复制对象
 */
class zNoncopyable
{
protected:

    // 默认构造函数
    zNoncopyable() = default;

    // 默认析构函数
    virtual ~zNoncopyable() = default;

public:

    // 禁止用传参方式来复制对象
    zNoncopyable(const zNoncopyable &) = delete;

    // 禁止用赋值方式来复制对象
    zNoncopyable& operator=(zNoncopyable const &) = delete;

    // 禁止用传参方式来移动对象
    zNoncopyable(zNoncopyable &&) = delete;

    // 禁止用赋值方式来移动对象
    zNoncopyable&operator=(zNoncopyable &&) = delete;
};

#endif //ZHS_ZNONCOPYABLE_H
