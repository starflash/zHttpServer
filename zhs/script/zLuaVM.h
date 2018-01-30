//
// Created by zhangweiwen on 17-3-19.
//

#ifndef ZHS_ZLUAVM_H
#define ZHS_ZLUAVM_H

#include <string>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <luabind/luabind.hpp>

#include <zhs/base/zType.h>
#include "zLuaScript.h"

const DWORD LUALIB_BASE	   = 0x00000001;
const DWORD LUALIB_TABLE   = 0x00000002;
const DWORD LUALIB_IO      = 0x00000004;
const DWORD LUALIB_STRING  = 0x00000008;
const DWORD LUALIB_MATH    = 0x00000010;
const DWORD LUALIB_DEBUG   = 0x00000020;
const DWORD LUALIB_PACKAGE = 0x00000040;
const DWORD LUALIB_OS      = 0x00000080;

struct lua_State;

/**
 * Lua虚拟机，用来执行Lua脚本。
 */
class zLuaVM
{
public:

    explicit zLuaVM(DWORD = LUALIB_BASE | LUALIB_TABLE | LUALIB_IO | LUALIB_STRING | LUALIB_MATH | LUALIB_PACKAGE);
    virtual ~zLuaVM();

    virtual void execute(const std::string &);
    virtual void execute(zLuaScript *);
    void bindModules();

    /**
     * 调用Lua函数
     *
     * @param func 函数名
     */
    void call(const char *func)
    {
        try {
            luabind::call_function<void>(getLuaState(), func);
        }
        catch (luabind::error &e)
        {
            return;
        }
    }

    /**
     * 调用Lua函数
     *
     * @tparam T   函数参数类型
     * @param func 函数名
     * @param arg  函数参数
     */
    template <typename T>
    void call(const char *func, T arg)
    {
        try {
            luabind::call_function<void>(getLuaState(), func, arg);
        }
        catch (luabind::error &e)
        {
            return;
        }
    }

    /**
     * 返回luabind的脚本上下文
     *
     * @return luabind的脚本上下文
     */
    lua_State *getLuaState() const
    {
        return luaState;
    }

protected:

    /**
     * luabind的脚本上下文
     */
    lua_State *luaState;
};


#endif //ZHS_ZLUAVM_H
