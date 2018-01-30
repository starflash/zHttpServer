//
// Created by zhangweiwen on 17-3-5.
//

#ifndef ZHS_ZUNIQID_H
#define ZHS_ZUNIQID_H

#include <list>
#include <algorithm>
#include <ext/pool_allocator.h>

#include "zMutex.h"

/**
 * 唯一编号生成器模板
 * 本模板实现了唯一编号生成器，并保证线程安全。
 *
 * @tparam T 各类长度的无符号整型
 */
template <typename T>
class zUniqid : private zNoncopyable
{
public:

    /**
     * 默认构造函数
     */
    zUniqid()
    {
        init(1, static_cast<T> (-1));
    }

    /**
     * 构造函数
     *
     * @param start 起始编号
     */
    explicit zUniqid(T start)
    {
        init(start, static_cast<T> (-1));
    }

    /**
     * 构造函数
     *
     * @param start 起始编号
     * @param max   最大无效编号
     */
    zUniqid(T start, T max)
    {
        init(start, max);
    }

    /**
     * 析构函数
     */
    ~zUniqid() override
    {
        mutex.lock();
        idlist.clear();
        mutex.unlock();
    }

    /**
     * 返回最大编号
     *
     * @return 最大编号
     */
    T getMaxId()
    {
        return max_id;
    }

    /**
     * 返回最大有效编号
     *
     * @return 最大有效编号
     */
    T getMaxValidId()
    {
        return max_id - 1;
    }

    /**
     * 编号是否被分配出去
     *
     * @param id 编号
     * @return   是否被分配
     */
    bool isAssigned(T id)
    {
        bool assigned = false;

        mutex.lock();
        if (id >= min_id && id < max_id) {
            for (auto iter = idlist.begin(); iter != idlist.end(); iter++) {
                if (*iter == id) {
                    goto found;
                }
            }
            assigned = true;
        }
    found:
        mutex.unlock();

        return assigned;
    }

    /**
     * 返回一个唯一编号
     * 如果返回的是最大编号，表示所有编号都被使用。
     *
     * @return 唯一编号
     */
    T get()
    {
        T id;

        mutex.lock();
        if (max_id > top_id) {
            id = top_id++;
        } else {
            if (!idlist.empty()) {
                id = idlist.back();
                idlist.pop_back();
            } else {
                id = max_id;
            }
        }
        mutex.unlock();

        return id;
    }

    /**
     * 返回多个唯一编号
     * 编号都是相邻的
     * 如果返回的是最大编号，表示所有编号都被使用。
     *
     * @param size  需要分配的个数
     * @param count 实际分配的个数
     * @return      第一个编号
     */
    T get(size_t size, size_t &count)
    {
        T id;

        mutex.lock();
        if (max_id > top_id) {
            count = std::min(max_id - top_id, size);
            id = top_id;
            top_id += size;
        } else {
            id = max_id;
            count = 0;
        }
        mutex.unlock();

        return id;
    }

    /**
     * 将编号放回生成器
     * 放回的编号必须是由get函数得到的
     * 必须保证放回的编号，没有被其他线程使用，以免出现编号重复现象。
     *
     * @param id 编号
     */
    void put(T id)
    {
        mutex.lock();
        if (id >= min_id && id < max_id) {
            bool found = false;
            for (auto iter = idlist.begin(); iter != idlist.end(); iter++) {
                if (*iter == id) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                idlist.push_front(id);
            }
        }
        mutex.unlock();
    }

private:

    // 最大编号
    T max_id;

    // 最小编号
    T min_id;

    // 可用最大编号
    T top_id;

    // 互斥量
    zMutex mutex {};

    //
    std::list<T, __gnu_cxx::__pool_alloc<T> > idlist;

    //
    void init(T min, T max)
    {
        min_id = min;
        max_id = max;
        top_id = min_id;
    }
};

#endif //ZHS_ZUNIQID_H
