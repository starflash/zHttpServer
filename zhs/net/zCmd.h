//
// Created by zhangweiwen on 16-12-22.
//

#ifndef ZHS_ZCMD_H
#define ZHS_ZCMD_H

#include <zhs/base/zType.h>
#include <zhs/base/zCommon.h>

#pragma pack(1)

/**
 * 定义指令，责服务器内部交换使用，和客户端交互的指令需要另外定义。
 */
namespace Zhs {
    // 空的指令代码
    const BYTE CMD_NULL = 0;

    // 空的指令参数
    const BYTE PARAM_NULL = 0;

    /**
     * 空操作指令
     */
    struct t_CmdNull {
        BYTE code;  // 指令代码
        BYTE param; // 指令参数

        /**
         * 构造函数
         */
        explicit t_CmdNull(const BYTE code = CMD_NULL, const BYTE param = PARAM_NULL) : code(code), param(param) {}
    };
}

#pragma pack()

#endif //ZHS_ZCMD_H
