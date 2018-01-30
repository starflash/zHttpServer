//
// Created by zhangweiwen on 17-3-19.
//

#ifndef ZHS_ZLUASYSTEM_H
#define ZHS_ZLUASYSTEM_H


#include <zhs/base/zSingleton.h>
#include <vector>
#include "zLuaVM.h"

/**
 * Lua脚本系统
 * 用来创建Lua虚拟机和Lua脚本
 */
class zLuaSystem : public zSingleton<zLuaSystem>
{
public:

    // 友元类
    friend class zSingleton<zLuaSystem>;
    friend class zSingletonFactory<zLuaSystem>;

    virtual zLuaScript *createLuaScript(const std::string &);
    virtual zLuaVM *createLuaVM();
    zLuaVM *getLuaVM(int);

private:

    // Lua虚拟机
    std::vector<zLuaVM *> vms;

    // 构造函数
    zLuaSystem() = default;

    // 析构函数
    ~zLuaSystem() override
    {
        vms.clear();
    }
};


#endif //ZHS_ZLUASYSTEM_H
