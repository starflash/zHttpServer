//
// Created by zhangweiwen on 17-3-19.
//

#include <algorithm>

#include "zLuaQuest.h"

/**
 * 在全局索引中添加一项
 *
 * @param type 事件类型
 * @param id   事件编号
 */
void zLuaQuest::add(int type, int id)
{
    if (!has(type, id)) {
        lqs.insert(hash(type, id));
    }
}

/**
 * 判断事件是否在全局索引中存在
 *
 * @param type 事件类型
 * @param id   事件编号
 * @return     是否存在
 */
bool zLuaQuest::has(int type, int id) const
{
    return lqs.find(hash(type, id)) != lqs.end();
}

/**
 * 根据事件类型和编号计算哈希值
 *
 * @param type 事件类型
 * @param id   事件编号
 * @return     哈希值
 */
int zLuaQuest::hash(int type, int id) const
{
    return ((type & 0xff) << 24) | (id & 0x00ffffff);
}

/**
 * 排序函数
 * 用来加速查找
 */
void zLuaQuest::sort()
{
    //std::stable_sort(lqs.begin(), lqs.end());
}