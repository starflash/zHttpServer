//
// Created by zhangweiwen on 2016/8/23.
//

#ifndef ZHS_ZXMLPARSER_H
#define ZHS_ZXMLPARSER_H

#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

// 定义常量指针
typedef xmlNodePtr const xmlNodePtrConst;

/**
 * XML处理
 */
class zXMLParser
{
public:

    zXMLParser();
    ~zXMLParser();

    bool initFile(const std::string &);
    bool initFile(const char *);
    bool initStr(const std::string &);
    bool initStr(const char *);
    bool initXML();
    void freeXML();
    std::string& dump(std::string &, bool = false);
    std::string& dump(xmlNodePtr, std::string &, bool = true);
    xmlNodePtr getRootNode(const char *);
    xmlNodePtr getChildNode(xmlNodePtrConst, const char *childName);
    xmlNodePtr getNextNode(xmlNodePtrConst, const char *);
    unsigned int getChildNodeNum(xmlNodePtrConst, const char *);

    xmlNodePtr newRootNode(const char *);
    xmlNodePtr newChildNode(xmlNodePtrConst, const char *, const char *);
    bool newNodeProp(xmlNodePtrConst, const char *, const char *);

    bool getNodePropNum(xmlNodePtrConst, const char *, void *, int);
    bool getNodePropStr(xmlNodePtrConst, const char *, void *, int);
    bool getNodePropStr(xmlNodePtrConst, const char *, std::string &);
    bool getNodeContentNum(xmlNodePtrConst, void *, int);
    bool getNodeContentStr(xmlNodePtrConst, void *, int);
    bool getNodeContentStr(xmlNodePtrConst, std::string &);

private:

    unsigned char* charConv(unsigned char *, const char *, const char *);
    xmlDocPtr doc;
};


#endif //ZHS_ZXMLPARSER_H
