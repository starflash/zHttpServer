//
// Created by zhangweiwen on 17-3-12.
//

#ifndef ZHS_ZMESSAGEQUEUE_H
#define ZHS_ZMESSAGEQUEUE_H

#include <utility>
#include <ext/mt_allocator.h>
#include <ext/pool_allocator.h>
#include <deque>
#include <queue>
#include "zCmd.h"

/**
 * 消息队列 (环形队列)
 */
class zMessageQueue
{
public:

    /**
     * 定义消息队列容器
     *
     * @tparam QSIZE 消息队列大小
     */
    template <int QSIZE = 102400>
    class zMQ
    {
    public:

        // 消息队列单元
        typedef std::pair<unsigned int, unsigned char *> t_MQChunk;

        // 构造函数
        zMQ()
        {
            queue_read = queue_write = 0;
        }

        // 析构函数
        ~zMQ()
        {
            clear();
        }

        /**
         * 从消息队列中取当前数据
         *
         * @return 数据
         */
        t_MQChunk* get()
        {
            if (array[queue_read].first) {
                return &array[queue_read].second;
            }

            return nullptr;
        }

        /**
         * 从消息队列中删除当前数据，并指向下一条数据。
         */
        void erase()
        {
            allocator.deallocate(array[queue_read].second.second, array[queue_read].second.first);
            array[queue_read].first = false;
            next(queue_read);
        }

        /**
         * 把数据放入消息队列
         * 如果消息队列已满，则放入临时队列。
         *
         * @param data 数据
         * @param len  数据长度
         * @return     是否成功
         */
        bool put(const void *data, const unsigned int len)
        {
            unsigned char *buf = allocator.allocate(len);

            if (buf) {
                memcpy(buf, data, len);
                if (!raise() && !array[queue_write].first) {
                    array[queue_write].first = true;
                    array[queue_write].second = std::make_pair(len, buf);
                    next(queue_write);
                } else {
                    queue.push(std::make_pair(len, buf));
                }

                return true;
            }

            return false;
        }

    private:

        /**
         * 多线程的内存分配器
         *
         * 该分配器可以通过调用_M_set_options方法设置以下参数：
         * struct __gnu_cxx::__pool_base::_Tune
         * {
         *     // 字节对齐
         *     // 默认值: 8
         *     size_t _M_align;
         *
         *     // 多少字节以上的内存直接用new分配
         *     // 默认值: 128
         *     size_t _M_max_bytes;
         *
         *     // 可分配的最小的内存块大小
         *     // 默认值: 8
         *     size_t _M_min_bin;
         *
         *     // 每次从系统申请的内存块的大小
         *     // 默认值: 4080
         *     size_t _M_chunk_size;
         *
         *     // 可支持的最多线程数
         *     // 默认值: 4096
         *     size_t _M_max_threads;
         *
         *     // 单个线程能保存的空闲块的百分比（超过的空闲块会归还给全局空闲链表）
         *     // 默认值: 10
         *     size_t _M_freelist_headroom;
         *
         *     // 是否直接使用new来分配
         *     // 默认值: 根据环境变量GLIBCXX_FORCE_NEW
         *     bool  _M_force_new;
         * }
         */
        __gnu_cxx::__mt_alloc<unsigned char> allocator;

        // 正式队列数组，参数一表示当前单元是否有数据，参数二存放消息数据。
        std::pair<volatile bool, t_MQChunk> array[QSIZE];

        // 临时存放队列
        std::queue<t_MQChunk, std::deque<t_MQChunk, __gnu_cxx::__pool_alloc<t_MQChunk> > > queue;

        // 读标志
        unsigned int queue_read;

        // 写标志
        unsigned int queue_write;

        /**
         * 把临时队列中的数据移动到正式队列
         *
         * @return 临时队列中是否仍存在数据
         */
        bool raise()
        {
            while (!queue.empty()) {
                if (!array[queue_write].first) {
                    array[queue_write].second = queue.front();
                    array[queue_write].first = true;
                    next(queue_write);
                    queue.pop();
                } else {
                    return true;
                }
            }

            return false;
        }

        /**
         * 清空消息队列
         */
        void clear()
        {
            while (raise()) {
                while (get()) {
                    erase();
                }
            }

            while (get()) {
                erase();
            }
        }

        /**
         * 消息队列读或写标记往后移一位
         * 如果读写标记已经是最后一位，则移动到头部（变为零）。
         *
         * @param queue_rw 读写标记
         */
        void next(unsigned int &queue_rw)
        {
            queue_rw = (queue_rw + 1) % QSIZE;
        }
    };

    /**
     * 将指令放入消息队列
     *
     * @param cmd 指令
     * @param len 长度
     * @return    是否成功
     */
    bool putQueue(const Zhs::t_CmdNull *cmd, const unsigned int len)
    {
        return mq.put((void *) cmd, len);
    }

    /**
     * 处理消息队列
     */
    void parseQueue()
    {
        zMQ<>::t_MQChunk *chunk = nullptr;

        while ((chunk = mq.get()) != nullptr) {
            parseCmd(reinterpret_cast<const Zhs::t_CmdNull *> (chunk->second), chunk->first);
            mq.erase();
        }
    }

    virtual bool parseCmd(const Zhs::t_CmdNull *, unsigned int) = 0;

protected:

    // 析构函数
    virtual ~zMessageQueue() = default;

private:

    // 消息队列容器
    zMQ<> mq {};
};

#endif //ZHS_ZMESSAGEQUEUE_H
