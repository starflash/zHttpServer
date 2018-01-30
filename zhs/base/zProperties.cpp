//
// Created by zhangweiwen on 2016/8/23.
//

#include <vector>

#include "zProperties.h"
#include "zString.h"

unsigned int zProperties::parseCmdLine(const std::string &cmdLine)
{
    std::vector<std::string> lines;

    Zhs::string_token(lines, cmdLine);
    for (std::vector<std::string>::const_iterator iter = lines.begin(); iter != lines.end(); iter++) {
        std::vector<std::string> kv;

        Zhs::string_token(kv, *iter, "=", 1);
        if (kv.size() == 2) {
            _properties[kv[0]] = kv[1];
        }
    }

    return static_cast<unsigned int>(_properties.size());
}

unsigned int zProperties::parseCmdLine(const char *cmdLine)
{
    std::vector<std::string> lines;

    Zhs::string_token(lines, cmdLine);
    for (std::vector<std::string>::const_iterator iter = lines.begin(); iter != lines.end(); iter++) {
        std::vector<std::string> kv;

        Zhs::string_token(kv, *iter, "=", 1);
        if (kv.size() == 2) {
            _properties[kv[0]] = kv[1];
        }
    }

    return static_cast<unsigned int>(_properties.size());
}