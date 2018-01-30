--
-- Created by IntelliJ IDEA.
-- User: zhangweiwen
-- Date: 18-1-28
-- Time: 下午4:09
-- To change this template use File | Settings | File Templates.
--

function log(message)
    logger = zLogger("LuaScripts");
    logger:luainfo(message);
end

function initialize()
    log("Lua脚本引擎初始化成功");
end