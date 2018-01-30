//
// Created by zhangweiwen on 18-1-21.
//

#ifndef ZHS_SERVERTASK_H
#define ZHS_SERVERTASK_H


#include <zhs/net/http/zHttpTask.h>

/**
 * 处理Http任务
 */
class ServerTask : public zHttpTask
{
public:

    /**
     * 构造函数
     *
     * @param pool   线程池
     * @param sockfd 连接套接字
     */
    ServerTask(zHttpTaskPool *pool, const int sockfd) : zHttpTask(pool, sockfd)
    {}

    /**
     * 析构函数
     */
    ~ServerTask() override = default;

    int httpCore() override ;

protected:

    /**
     * 解析Http原始请求
     *
     * @param raw       原始请求
     * @param urldecode 是否需要解码
     */
    void extract_request(const char *raw, bool urldecode = true)
    {
        std::istringstream http_request(raw);
        std::string line;

        while (getline(http_request, line) && request_line.empty()) {
            if (urldecode) {
                Zhs::urldecode(line);
            }
            request_line = line;
        }

        while (getline(http_request, line) && line != "\r") {
            if (urldecode) {
                Zhs::urldecode(line);
            }
            request_header.push_back(line);
        }

        while (getline(http_request, line)) {
            if (urldecode) {
                Zhs::urldecode(line);
            }
            request_body.push_back(line);
        }
    }

    /**
     * 生成Http响应
     *
     * @return 响应字符串
     */
    std::string build_response()
    {
        std::ostringstream http_response;

        http_response << response_line << "\r\n";

        for (const auto &header : response_header) {
            http_response << header << "\r\n";
        }

        http_response << "\r\n";

        for (const auto &body : response_body) {
            http_response << body << "\r\n";
        }

        return http_response.str();
    }

private:

    /**
     *
     */
    const std::string RESPONSE_LINE_OK {"HTTP/1.1 200 OK"};
    const std::string RESPONSE_LINE_NOTFOUND {"HTTP/1.1 404 Not Found"};
    const std::string RESPONSE_LINE_INTERNAL_SERVER_ERROR {"HTTP/1.1 500 Internal Server Error"};
    const std::string RESPONSE_HEADER_CONTENTTYPE {"Content-Type: text/html; charset=utf-8"};

    /**
     * 请求行
     */
    std::string request_line;

    /**
     * 请求头
     */
    std::vector<std::string> request_header;

    /**
     * 请求体
     */
    std::vector<std::string> request_body;

    /**
     * 响应行
     */
    std::string response_line;

    /**
     * 响应头
     */
    std::vector<std::string> response_header;

    /**
     * 响应体
     */
    std::vector<std::string> response_body;
};


#endif //ZHS_SERVERTASK_H
