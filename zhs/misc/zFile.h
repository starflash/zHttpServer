//
// Created by zhangweiwen on 18-1-24.
//

#ifndef ZHS_ZFILE_H
#define ZHS_ZFILE_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * 文件操作
 */
class zFile
{
public:

    /**
     * 判断文件名是不是一个目录
     *
     * @param filename 文件名
     * @return         是否目录
     */
    static bool isDirectory(std::string &filename)
    {
        struct stat statinfo {};
        stat(filename.c_str(), &statinfo);

        return S_ISDIR(statinfo.st_mode);
    }
};

#endif //ZHS_ZFILE_H
