//
// Created by zhangweiwen on 2016/8/23.
//

#ifndef ZHS_ZPROPERTIES_H
#define ZHS_ZPROPERTIES_H

#include <string>
#include <iostream>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <cstring>

#include "zString.h"

/**
 * 属性关联类容器，所有属性关键字和值都使用字符串代表，关键字不区分大小写。
 */
class zProperties
{
public:

    /**
     * 获取一个属性
     *
     * @param key 关键字
     * @return    属性值
     */
    const std::string& getProperity(const std::string &key)
    {
        return _properties[key];
    }

    /**
     * 设置一个属性
     *
     * @param key 关键字
     * @param value 属性值
     */
    void setProperty(const std::string &key, const std::string &value)
    {
        _properties[key] = value;
    }

    /**
     * 重载[]操作符，获取一个属性。
     *
     * @param key 关键字
     * @return    属性值
     */
    std::string& operator[](const std::string &key)
    {
        return _properties[key];
    }

    /**
     * 输出存储的所有属性值
     *
     * @param out 输出目的地
     */
    void dump(std::ostream &out)
    {
        property_hashmap::const_iterator citer;

        for (citer = _properties.begin(); citer != _properties.end(); citer++) {
            out << citer->first << " = " << citer->second << std::endl;
        }
    }

    unsigned int parseCmdLine(const std::string &);
    unsigned int parseCmdLine(const char *);

protected:

    /**
     * 哈希函数
     */
    struct key_hash : public std::function<size_t (const std::string)>
    {
        size_t operator()(const std::string &key) const
        {
            std::string k;
            std::hash<std::string> h;

            // 转化字符串为小写
            k.assign(key);
            Zhs::strtolower(k);

            return h(k);
        }
    };

    /**
     * 比较函数
     */
    struct key_equal : public std::function<bool (const std::string , const std::string)>
    {
        bool operator()(const std::string &s1, const std::string &s2) const
        {
            return strcasecmp(s1.c_str(), s2.c_str()) == 0;
        }
    };


    // 字符串的哈希表
    typedef std::unordered_map<std::string, std::string, key_hash, key_equal> property_hashmap;

    // 保存属性的键值对
    property_hashmap _properties;
};

#endif //ZHS_ZPROPERTIES_H
