//
// Created by zhangweiwen on 18-1-16.
//

#include <luabind/luabind.hpp>
#include <zhs/log/zLogger.h>
#include "zLuaBinder.h"

/**
 * 绑定一个Lua虚拟机，使特定的接口对该虚拟机可用。
 *
 * @param vm Lua虚拟机
 */
void zLuaBinder::bind(zLuaVM *vm)
{
    using namespace luabind;

    module(vm->getLuaState())
    [
        class_<std::string>("String"),
        class_<zLogger>("zLogger").def(constructor<const std::string &>())
                                  .def("luainfo", &zLogger::luainfo)
    ];
}