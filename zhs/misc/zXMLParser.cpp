//
// Created by zhangweiwen on 2016/8/23.
//

#include <iconv.h>
#include <cstring>
#include <iostream>

#include "zXMLParser.h"
#include "../base/zCommon.h"
#include "../base/zType.h"

/**
 * 构造函数
 */
zXMLParser::zXMLParser()
{
    doc = nullptr;
}

/**
 * 析构函数
 * 回收分配的空间，关闭的打开文件等。
 */
zXMLParser::~zXMLParser()
{
    freeXML();
}

/**
 * 初始化要解析的XML文件
 *
 * @param xmlFile 文件路径，绝对或者相对的。
 * @return        是否成功
 */
bool zXMLParser::initFile(const std::string &xmlFile)
{
    return initFile(xmlFile.c_str());
}

/**
 * 初始化要解析的xml文件
 *
 * @param xmlFile 文件路径，绝对或者相对的。
 * @return        是否成功
 */
bool zXMLParser::initFile(const char *xmlFile)
{
    freeXML();
    if (xmlFile == nullptr) {
        return false;
    }
    doc = xmlParseFile(xmlFile);

    return doc != nullptr;
}

/**
 * 初始化要解析的XML字符串
 *
 * @param xmlStr 被初始化的xml字符串。
 * @return       是否成功
 */
bool zXMLParser::initStr(const std::string &xmlStr)
{
    return initStr(xmlStr.c_str());
}

/**
 * 初始化要解析的XML字符串
 *
 * @param xmlStr 被初始化的xml字符串。
 * @return       是否成功
 */
bool zXMLParser::initStr(const char *xmlStr)
{
    freeXML();
    if (xmlStr == nullptr) {
        return false;
    }
    doc = xmlParseDoc((xmlChar *) xmlStr);

    return doc != nullptr;
}

/**
 * 初始化XML
 *
 * @return 是否成功
 */
bool zXMLParser::initXML()
{
    freeXML();
    doc = xmlNewDoc((const xmlChar *) "1.0");

    return doc != nullptr;
}

/**
 * 释放被解析的XML文档
 */
void zXMLParser::freeXML()
{
    if (doc) {
        xmlFreeDoc(doc);
        doc = nullptr;
    }
}

/**
 * 转储XML文档
 *
 * @param dest   转储的文档在内存中存放的位置
 * @param format 转储时候是否格式化文档
 * @return       转储的文档
 */
std::string& zXMLParser::dump(std::string &dest, bool format)
{
    if (doc) {
        xmlChar *out = nullptr;
        int size = 0;

        xmlDocDumpFormatMemory(doc, &out, &size, format ? 1 : 0);
        if (out != nullptr) {
            dest = (char *) out;
            xmlFree(out);
        }
    }

    return dest;
}

/**
 * 转储某个节点
 *
 * @param dumpNode 要被转储的节点
 * @param dest     转储的节点在内存中存放的位置
 * @param head     是否添加xml文件头，默认true。
 * @return         转储的文档
 */
std::string& zXMLParser::dump(xmlNodePtr dumpNode, std::string &dest, bool head)
{
    if (dumpNode == nullptr) {
        return dest;
    }
    xmlBufferPtr out = xmlBufferCreate();
    if (xmlNodeDump(out, doc, dumpNode, 1, 1) != -1) {
        unsigned char *cout = charConv(out->content, "UTF-8", (const char *) doc->encoding);
        if (cout) {
            if (head) {
                dest = R"(<?xml version="1.0" encoding=")";
                dest += (char *) doc->encoding;
                dest += "\"?>";
            } else {
                dest = "";
            }
            dest += (char *) cout;
            SAFE_DELETE_VECTOR(cout);
        }
    }
    xmlBufferFree(out);

    return dest;
}

/**
 * 得到XML文档的根节点
 *
 * @param rootName 根节点的名字
 * @return         根节点指针，nullptr表示失败。
 */
xmlNodePtr zXMLParser::getRootNode(const char *rootName)
{
    if (doc == nullptr) {
        return nullptr;
    }

    // 得到根节点
    xmlNodePtr cur = xmlDocGetRootElement(doc);

    // 准备起始节点
    if (rootName != nullptr)
        while (cur != nullptr && xmlStrcmp(cur->name, (const xmlChar *) rootName)) {
            cur = cur->next;
        }

    return cur;
}

/**
 * 得到某个节点的子节点
 *
 * @param parent    父节点
 * @param childName 子节点的名称，如果为nullptr，将会得到第一个子节点。
 * @return          子节点指针，nullptr表示失败或者没有相应的节点。
 */
xmlNodePtr zXMLParser::getChildNode(xmlNodePtrConst parent, const char *childName)
{
    if (parent == nullptr) {
        return nullptr;
    }
    xmlNodePtr retval = parent->children;
    if (childName) {
        while (retval) {
            if (!xmlStrcmp(retval->name, (const xmlChar *) childName)) {
                break;
            }
            retval = retval->next;
        }
    } else {
        while (retval) {
            if (!xmlNodeIsText(retval)) {
                break;
            }
            retval = retval->next;
        }
    }

    return retval;
}

/**
 * 得到下一个节点
 *
 * @param node     当前节点
 * @param nextName 下一个节点的名字，如果为nullptr，将会得到相邻的下一个节点。
 * @return         下一个节点指针，nullptr失败或者没有相应的节点。
 */
xmlNodePtr zXMLParser::getNextNode(xmlNodePtrConst node, const char *nextName)
{
    if (node == nullptr) {
        return nullptr;
    }
    xmlNodePtr retval = node->next;
    if (nextName) {
        while (retval) {
            if (!xmlStrcmp(retval->name, (const xmlChar *) nextName)) {
                break;
            }
            retval = retval->next;
        }
    } else {
        while (retval) {
            if (!xmlNodeIsText(retval)) {
                break;
            }
            retval = retval->next;
        }
    }

    return retval;
}

/**
 * 统计子节点的数量
 *
 * @param parent    被统计的父节点
 * @param childName 被统计子节点的名字，如果为nullptr，统计所有子节点的数量
 * @return          子节点的数量
 */
unsigned int zXMLParser::getChildNodeNum(xmlNodePtrConst parent, const char *childName)
{
    unsigned int retval = 0;

    if (parent == nullptr) {
        return retval;
    }
    xmlNodePtr child = parent->children;
    if (childName) {
        while (child) {
            if (!xmlStrcmp(child->name, (const xmlChar *) childName)) {
                retval++;
            }
            child = child->next;
        }
    } else {
        while (child) {
            if (!xmlNodeIsText(child)) {
                retval++;
            }
            child = child->next;
        }
    }

    return retval;
}

/**
 * 为XML文档添加一个根节点
 *
 * @param rootName 根节点名称
 * @return         根节点指针
 */
xmlNodePtr zXMLParser::newRootNode(const char *rootName)
{
    if (nullptr == doc) {
        return nullptr;
    }

    xmlNodePtr root = xmlNewNode(nullptr, (const xmlChar *) rootName);
    xmlDocSetRootElement(doc, root);

    return root;
}

/**
 * 在一个节点下面添加子节点
 *
 * @param parent    父节点
 * @param childName 子节点名称
 * @param content   子节点内容
 * @return          子节点的指针
 */
xmlNodePtr zXMLParser::newChildNode(xmlNodePtrConst parent, const char *childName, const char *content)
{
    if (nullptr == parent) {
        return nullptr;
    }

    return xmlNewChild(parent, nullptr, (const xmlChar *) childName, (const xmlChar *) content);
}

/**
 * 为一个节点添加属性
 *
 * @param node     节点
 * @param propName 属性名称
 * @param prop     属性值
 * @return         是否成功
 */
bool zXMLParser::newNodeProp(xmlNodePtrConst node, const char *propName, const char *prop)
{
    if (nullptr == node) {
        return false;
    }

    return (nullptr != xmlNewProp(node, (const xmlChar *) propName, (const xmlChar *) prop));
}

/**
 * 得到节点属性，并转化成数字。
 *
 * @param node     对象节点
 * @param propName 属性名称
 * @param prop     返回的属性值
 * @param propSize 属性值所占内存大小
 * @return         是否成功
 */
bool zXMLParser::getNodePropNum(xmlNodePtrConst node, const char *propName, void *prop, const int propSize)
{
    char *temp = nullptr;
    bool ret = true;

    if (node == nullptr || prop == nullptr || propName == nullptr) {
        return false;
    }
    temp = (char *) xmlGetProp(node, (const xmlChar *) propName);
    if (temp == nullptr ) {
        return false;
    }
    switch (propSize) {
        case sizeof(BYTE):
            *((BYTE *) prop) = (BYTE) strtol(temp, nullptr, 10);
            break;
        case sizeof(WORD):
            *((WORD *) prop) = (WORD) strtol(temp, nullptr, 10);
            break;
        case sizeof(DWORD):
            *((DWORD *) prop) = (DWORD) strtol(temp, nullptr, 10);
            break;
        case sizeof(QWORD):
            *((QWORD *) prop) = (QWORD) strtol(temp, nullptr, 10);
            break;
        default:
            ret = false;
    }
    xmlFree(temp);

    return ret;
}

/**
 * 得到节点属性
 *
 * @param node     对象节点
 * @param propName 属性名称
 * @param prop     返回的属性值
 * @param propSize 属性值所占内存大小
 * @return         是否成功
 */
bool zXMLParser::getNodePropStr(xmlNodePtrConst node, const char *propName, void *prop, int propSize)
{
    char *temp = nullptr;
    bool ret = true;

    if (node == nullptr || prop == nullptr || propName == nullptr) {
        return false;
    }
    temp = (char *) xmlGetProp(node, (const xmlChar *) propName);
    if (temp == nullptr ) {
        return false;
    }
    unsigned char *out = charConv((unsigned char *) temp, "UTF-8", (const char *) doc->encoding);
    if (out) {
        memset(prop, 0, (size_t) propSize);
        strncpy((char *) prop, (const char *) out, (size_t) propSize - 1);
        SAFE_DELETE_VECTOR(out);
    }
    xmlFree(temp);

    return ret;
}

/**
 * 得到节点属性
 *
 * @param node     对象节点
 * @param propName 属性名称
 * @param prop     返回的属性值
 * @return         是否成功
 */
bool zXMLParser::getNodePropStr(xmlNodePtrConst node, const char *propName, std::string &prop)
{
    char *temp = nullptr;
    bool ret = true;

    if (node == nullptr || propName == nullptr) {
        return false;
    }
    temp = (char *) xmlGetProp(node, (const xmlChar *) propName);
    if (temp ==nullptr ) {
        return false;
    }
    unsigned char *out = charConv((unsigned char *) temp, "UTF-8", (const char *) doc->encoding);
    if (out) {
        prop = (char *) out;
        SAFE_DELETE_VECTOR(out);
    }
    xmlFree(temp);

    return ret;
}

/**
 * 得到节点内容，并转化成数字。
 *
 * @param node        对象节点
 * @param content     返回的节点内容
 * @param contentSize 节点内容所占内存大小
 * @return            是否成功
 */
bool zXMLParser::getNodeContentNum(xmlNodePtrConst node, void *content, int contentSize)
{
    char *temp = nullptr;
    bool ret = true;
    if (node == nullptr || content == nullptr) {
        return false;
    }
    xmlNodePtr text = node->children;
    while (text != nullptr) {
        if (!xmlStrcmp(text->name, (const xmlChar *) "text")) {
            temp = (char *) text->content;
            break;
        }
        text = text->next;
    }
    if (temp == nullptr) {
        return false;
    }
    switch (contentSize) {
        case sizeof(BYTE):
            (*(BYTE *) content) = (BYTE) strtol(temp, nullptr, 10);
            break;
        case sizeof(WORD):
            (*(WORD *) content) = (WORD) strtol(temp, nullptr, 10);
            break;
        case sizeof(DWORD):
            (*(DWORD *) content) = (DWORD) strtol(temp, nullptr, 10);
            break;
        case sizeof(QWORD):
            (*(QWORD *) content) = (QWORD) strtol(temp, nullptr, 10);
            break;
        default:
            ret = false;
    }

    return ret;
}

/**
 * 得到节点内容
 *
 * @param node        对象节点
 * @param content     返回的节点内容
 * @param contentSize 节点内容所占内存大小
 * @return            是否成功
 */
bool zXMLParser::getNodeContentStr(xmlNodePtrConst node, void *content, const int contentSize)
{
    char *temp = nullptr;
    bool ret = true;

    if (node == nullptr || content == nullptr) {
        return false;
    }
    xmlNodePtr text = node->children;
    while (text != nullptr) {
        if (!xmlStrcmp(text->name, (const xmlChar *) "text")) {
            temp = (char *) text->content;
            break;
        }
        text = text->next;
    }
    if (temp == nullptr) {
        return false;
    }
    unsigned char *out = charConv((unsigned char *) temp, "UTF-8", (const char *) doc->encoding);
    if (out) {
        memset(content, 0, (size_t) contentSize);
        strncpy((char *) content, (const char *) out, (size_t) contentSize - 1);
        SAFE_DELETE_VECTOR(out);
    }

    return ret;
}

/**
 * 得到节点内容
 *
 * @param node    对象节点
 * @param content 返回结果的存储位置
 * @return        是否成功
 */
bool zXMLParser::getNodeContentStr(xmlNodePtrConst node, std::string &content) {
    char *temp = nullptr;
    bool ret = true;

    if (node == nullptr) {
        return false;
    }
    xmlNodePtr text = node->children;
    while (text != nullptr) {
        if (!xmlStrcmp(text->name, (const xmlChar*) "text")) {
            temp = (char *) text->content;
            break;
        }
        text = text->next;
    }
    if (temp == nullptr) {
        return false;
    }
    unsigned char *out = charConv((unsigned char *) temp, "UTF-8", (const char *) doc->encoding);
    if (out) {
        content = (char *) out;
        SAFE_DELETE_VECTOR(out);
    }

    return ret;
}

/**
 * 转换XML字符串的编码，返回的内存指针必须手工释放。
 *
 * @param xml          转换前的XML字符串
 * @param fromEncoding 源编码
 * @param toEncoding   目标编码
 * @return             转换后的XML字符串
 */
unsigned char* zXMLParser::charConv(unsigned char *xml, const char *fromEncoding, const char *toEncoding)
{
    unsigned char *out;
    size_t ret, size, out_size;

    size = strlen((char *) xml);
    out_size = size * 2 + 1;
    out = new unsigned char[out_size];
    memset(out, 0, out_size);
    if (fromEncoding != nullptr && toEncoding != nullptr) {
        iconv_t icv = iconv_open(toEncoding, fromEncoding);

        if ((iconv_t) -1 == icv) {
            SAFE_DELETE_VECTOR(out);
            out = nullptr;
        } else {
            auto fromtemp = (char *) xml;
            auto totemp = (char *) out;
            size_t tempout = out_size - 1;

            ret = iconv(icv, &fromtemp, &size, &totemp, &tempout);
            if ((size_t) -1 == ret) {
                SAFE_DELETE_VECTOR(out);
                out = nullptr;
            }
            iconv_close(icv);
        }
    } else {
        strncpy((char *) out, (char *) xml, size);
    }

    return out;
}