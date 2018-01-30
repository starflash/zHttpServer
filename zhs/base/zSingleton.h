//
// Created by zhangweiwen on 2016/8/17.
//

#ifndef ZHS_ZSINGLETON_H
#define ZHS_ZSINGLETON_H

#include <cstddef>

#include "zCommon.h"

/**
 * 单例工厂模板
 */
template <typename T>
class zSingletonFactory
{
public:

    // 创建类的实例
    static T* newInstance() {
        return new T();
    }

    // 创建类的实例
    template <typename... P>
    static T* newInstance(P... args) {
        return new T(args...);
    }
};

/**
 * 单例模板
 */
template <typename T, typename F = zSingletonFactory<T> >
class zSingleton
{
public:

    // 禁止用传参方式来复制对象
    zSingleton(const zSingleton &) = delete;

    // 禁止用赋值方式来复制对象
    zSingleton& operator=(zSingleton const &) = delete;

    // 禁止用传参方式来移动对象
    zSingleton(zSingleton &&) = delete;

    // 禁止用赋值方式来移动对象
    zSingleton& operator=(zSingleton &&) = delete;

protected:

    // 指向类实例的指针。
    static T *instance;

    // 构造函数
    zSingleton() = default;

    // 析构函数
    virtual ~zSingleton() = default;

public:

    // 创建新的实例
    static void newInstance()
    {
        instance = F::newInstance();
    }

    // 创建新的实例
    template <typename... P>
    static void newInstance(P... args)
    {
        instance = F::newInstance(args...);
    }

    // 返回指向类的实例的指针
    static T* getInstance()
    {
        if (!hasInstance()) {
            newInstance();
        }

        return instance;
    }

    // 返回类的实例对象
    static T& getInstanceObject()
    {
        return *getInstance();
    }

    // 删除类的实例
    static void destroyInstance()
    {
        SAFE_DELETE(instance);
    }
    
    // 类的实例是否存在
    static bool hasInstance()
    {
        return instance != nullptr;
    }
};

template <typename T, typename F>
T* zSingleton<T, F>::instance = nullptr;

/**
 * 简化的单例模板
 */
template <typename T>
class zSingletonDef
{
public:

    typedef T ObjectType;

    static ObjectType instance;

    static ObjectType& getInstance()
    {
        return instance;
    }

    zSingletonDef() = delete;
};

template <typename T>
typename zSingletonDef<T>::ObjectType zSingletonDef<T>::instance;

#endif //ZHS_ZSINGLETON_H
