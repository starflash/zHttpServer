//
// Created by zhangweiwen on 17-3-19.
//

#ifndef ZHS_ZLUASCRIPT_H
#define ZHS_ZLUASCRIPT_H

#include <string>

/**
 * Lua脚本文件
 */
class zLuaScript
{
public:

    // 构造函数
    zLuaScript() = default;

    // 析构函数
    virtual ~zLuaScript() = default;

    /**
     * 设置Lua脚本文件
     *
     * @param sciptfile 脚本文件
     */
    virtual void setScriptFile(const std::string &sciptfile)
    {
        filename = sciptfile;
    }

    /**
     * 返回Lua脚本文件
     *
     * @return 脚本文件
     */
    const std::string &getScriptFile() const
    {
        return filename;
    }

private:

    //
    std::string filename;
};

#endif //ZHS_ZLUASCRIPT_H
