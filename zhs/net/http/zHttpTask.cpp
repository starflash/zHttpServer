//
// Created by zhangweiwen on 17-2-6.
//

#include "zHttpTask.h"

/**
 * 向套接字发送指令
 *
 * @param cmd 指令
 * @param len 指令长度
 * @return    是否发送成功
 */
bool zHttpTask::sendCmd(const void *cmd, const unsigned int len)
{
    return socket.sendCmdWithoutPack(cmd, len);
}

/**
 * 向套接字发送Http响应
 *
 * @param response  Http响应
 * @param urlencode 是否需要编码
 * @return          是否成功
 */
bool zHttpTask::sendResponse(std::string &response, bool urlencode)
{
    if (urlencode) {
        Zhs::urlencode(response);
    }

    return sendCmd(response.c_str(), static_cast<unsigned int>(response.size()));
}