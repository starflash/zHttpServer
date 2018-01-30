//
// Created by zhangweiwen on 17-3-19.
//

#include "zLuaVM.h"
#include "zLuaBinder.h"

/**
 * 构造函数
 * 创建一个虚拟机上下文用于执行脚本
 *
 * @param libs 虚拟机内可用的Lua库
 */
zLuaVM::zLuaVM(DWORD libs)
{
    auto isset = [&libs](const DWORD mask)->bool {
        return (libs & mask) == mask;
    };

    luaState = lua_open();
    if (isset(LUALIB_BASE)) {
        lua_cpcall(luaState, luaopen_base, nullptr);
    }
    if (isset(LUALIB_TABLE)) {
        lua_cpcall(luaState, luaopen_table, nullptr);
    }
    if (isset(LUALIB_IO)) {
        lua_cpcall(luaState, luaopen_io, nullptr);
    }
    if (isset(LUALIB_STRING)) {
        lua_cpcall(luaState, luaopen_string, nullptr);
    }
    if (isset(LUALIB_MATH)) {
        lua_cpcall(luaState, luaopen_math, nullptr);
    }
    if (isset(LUALIB_DEBUG)) {
        lua_cpcall(luaState, luaopen_debug, nullptr);
    }
    if (isset(LUALIB_PACKAGE)) {
        lua_cpcall(luaState, luaopen_package, nullptr);
    }
    if (isset(LUALIB_OS)) {
        lua_cpcall(luaState, luaopen_os, nullptr);
    }
    luabind::open(luaState);
}

/**
 * 析构函数
 * 关闭虚拟机
 */
zLuaVM::~zLuaVM()
{
    if (luaState) {
        lua_close(luaState);
        luaState = nullptr;
    }
}

/**
 * 执行Lua脚本
 *
 * @param lua Lua脚本字符串
 */
void zLuaVM::execute(const std::string &lua)
{
    if (luaState) {
        luaL_dostring(luaState, lua.c_str());
    }
}

/**
 * 执行Lua脚本
 *
 * @param luaScript Lua脚本文件
 */
void zLuaVM::execute(zLuaScript *luaScript)
{
    if (luaState) {
        const std::string &luaFile = luaScript->getScriptFile();

        luaL_dofile(luaState, luaFile.c_str());
    }
}

/**
 * 绑定需要的模块
 */
void zLuaVM::bindModules()
{
    zLuaBinder::bind(this);
}