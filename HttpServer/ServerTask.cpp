//
// Created by zhangweiwen on 18-1-21.
//

#include <fstream>
#include <zhs/misc/zFile.h>
#include "ServerTask.h"
#include "HttpService.h"

/**
 * 任务主处理函数
 *
 * @return 1: 成功
 *         0: 还要继续等待
 *        -1: 失败
 */
int ServerTask::httpCore()
{
    Zhs::logger->trace("ServerTask::httpCore");

    auto retcode = socket.recvBufPolled();
    if (retcode > 0) {
        char buf[8192];

        if (socket.recvCharPolled(buf, sizeof(buf)) <= 0) {
            // 这里只是从缓冲取数据包，所以不会出错，没有数据则直接返回。
            retcode = 0;
        } else {
            extract_request(buf);

            if (request_line.find("GET ") == 0) {
                Zhs::logger->debug("HTTP Request: %s", request_line.c_str());

                if (request_line[4] == '/') {
                    auto end = request_line.find(' ', 4);

                    if (end != std::string::npos) {
                        std::string filename = HttpService::getHtdocs() + request_line.substr(4, end - 4);
                        std::ifstream infile;

                        if (zFile::isDirectory(filename)) {
                            filename += HttpService::getIndex();
                        }

                        if (infile.open(filename.data()), infile.is_open()) {
                            response_body.emplace_back(std::string(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()));
                            response_line = RESPONSE_LINE_OK;
                        } else {
                            Zhs::logger->error("File not found: %s", filename.c_str());

                            response_line = RESPONSE_LINE_NOTFOUND;
                            response_body.emplace_back("Page Not Found");
                        }
                    }
                }

                if (response_body.empty()) {
                    response_body.emplace_back(request_line);
                }

                response_header.emplace_back(RESPONSE_HEADER_CONTENTTYPE);

                std::string http_response = build_response();

                sendResponse(http_response);
            }

            retcode = zHttpTask::httpCore();
        }
    }

    return retcode;
}