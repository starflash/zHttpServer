//
// Created by zhangweiwen on 17-3-19.
//

#include "zLuaSystem.h"

/**
 * 创建一个Lua虚拟机
 *
 * @return Lua虚拟机对象
 */
zLuaVM* zLuaSystem::createLuaVM()
{
    auto vm = new zLuaVM();

    vms.push_back(vm);

    return vm;
}

/**
 * 从给定的文件中创建一个Lua脚本文件
 *
 * @param scriptfile Lua脚本文件名
 * @return           Lua脚本文件对象
 */
zLuaScript* zLuaSystem::createLuaScript(const std::string &scriptfile)
{
    auto script = new zLuaScript();

    script->setScriptFile(scriptfile);

    return script;
}

/**
 * 取得一个Lua虚拟机
 *
 * @param index 索引
 * @return      Lua虚拟机对象
 */
zLuaVM* zLuaSystem::getLuaVM(int index)
{
    return vms[index];
}