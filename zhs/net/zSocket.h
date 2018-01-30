//
// Created by zhangweiwen on 2016/8/30.
//

#ifndef ZHS_ZSOCKET_H
#define ZHS_ZSOCKET_H

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <ext/pool_allocator.h>
#include <ext/mt_allocator.h>
#include <error.h>
#include <vector>
#include <string>

#include <zhs/zhs.h>
#include <zhs/base/zNoncopyable.h>
#include <zhs/thread/zMutex.h>
#include <zhs/misc/zTime.h>
#include <zhs/misc/zCrypt.h>

// 计算zlib压缩后的长度
#define ZIP_SIZE(zip) ((zip) * 120 / 100 + 12)

// 每个区块的大小
const unsigned int TRUNK_SIZE = 64 * 1024;
// 压缩需要的缓冲
const unsigned int PACKET_ZIP_BUFFER = ZIP_SIZE(TRUNK_SIZE - 1) + sizeof(unsigned int) + 8;

/**
 * 字节缓冲，用于套接字接收和发送数据的缓冲。
 *
 * @tparam T 缓冲区数据类型
 */
template <typename T>
class ByteBuffer
{
public:

    /**
     * 构造函数
     */
    ByteBuffer();

    /**
     * 向缓冲区填入数据
     *
     * @param buff 待填入缓冲区的数据
     * @param size 待填入缓冲区的数据长度
     */
    void put(const BYTE *buff, const unsigned int size)
    {
        // 确认缓冲区的内存是否足够
        wr_reserve(size);

        // 复制数据
        memcpy(&_buffer[current], buff, size);

        // 移动指针
        current += size;
    }

    /**
     * 得到当前缓冲区可写入空间的位置
     * 在调用此函数之前需要保证缓冲区有空间可写入，可以调用wr_reserve函数来分配空间。
     *
     * @return 缓冲区中可写入位置的开始地址
     */
    BYTE* wr_buf()
    {
        return &_buffer[current];
    }

    /**
     * 返回缓冲区中可读取数据的开始地址
     *
     * @return 缓冲区中可读取数据的开始地址
     */
    BYTE* rd_buf()
    {
        return &_buffer[offset];
    }

    /**
     * 得到缓冲区可写入空间的大小
     *
     * @return 可写入空间大小
     */
    unsigned int wr_size() const
    {
        return _size - current;
    }

    /**
     * 得到缓冲区中可读取数据的大小
     *
     * @return 可读取数据大小
     */
    unsigned int rd_size() const
    {
        return current - offset;
    }

    /**
     * 调整缓冲区可写入空间的起始位置
     *
     * @param size 移动量
     */
    void wr_flip(const unsigned int size)
    {
        current += size;
    }

    /**
     * 调整缓冲区可读取数据的起始位置
     * 当可读取数据位置移动后，可写入数据的位置也相应移动。
     *
     * @param size 移动量
     */
    void rd_flip(const unsigned int size)
    {
        offset += size;

        if (rd_ready()) {
            unsigned int rs = rd_size();

            // 当已读取字节数大于未读取字节数的时候，移动未读取数据到头部，同时移动位置指针。
            if (offset >= rs) {
                memmove(&_buffer[0], &_buffer[offset], rs);
                offset = 0;
                current = rs;
            }
        } else {
            reset();
        }
    }

    /**
     * 判断缓冲区中是否有可读取的数据
     *
     * @return 是否可读
     */
    bool rd_ready() const
    {
        return current > offset;
    }

    /**
     * 重值缓冲区中的数据，清空无用的垃圾数据。
     */
    void reset()
    {
        current = 0;
        offset = 0;
    }

    /**
     * 返回缓冲区的大小
     *
     * @return 缓冲区大小
     */
    unsigned int size() const
    {
        return _size;
    }

    /**
     * 根据传入的空间大小，计算合适的区块数。
     *
     * @param size 空间大小
     * @return     区块数
     */
    unsigned int trunks(const unsigned int size) const
    {
        return (size + TRUNK_SIZE - 1) / TRUNK_SIZE;
    }

    /**
     * 对缓冲区空间进行调整，如果缓冲区空间不足，重新调整缓冲区大小，大小调整原则按照TRUNK_SIZE的整数倍进行增加。
     *
     * @param size 新的空间大小
     */
    void wr_reserve(unsigned int size);

private:

    // 缓冲区大小
    unsigned int _size;

    // 指向缓冲区可读取数据的地址
    unsigned int offset {0};

    // 指向缓冲区可写入数据的地址
    unsigned int current {0};

    // 缓冲区
    T _buffer;
};

// 动态缓冲区，可以动态扩展缓冲区大小。
typedef ByteBuffer<std::vector<BYTE, __gnu_cxx::__pool_alloc<BYTE>>> t_BufferCmdQueue;

/**
 * 构造函数
 * 动态分配内存的缓冲区，大小可以随时扩展。
 */
template <>
inline t_BufferCmdQueue::ByteBuffer() : _size(TRUNK_SIZE), _buffer(_size)
{}

/**
 * 调整缓冲区空间大小
 *
 * @param size 新的空间大小
 */
template <>
inline void t_BufferCmdQueue::wr_reserve(const unsigned int size)
{
    if (wr_size() < size) {
        _size += (TRUNK_SIZE * trunks(size));
        _buffer.resize(_size);
    }
}

// 静态缓冲区，以栈空间数组的方式来分配内存，用于一些临时变量的获取。
typedef ByteBuffer<BYTE[PACKET_ZIP_BUFFER]> t_StackCmdQueue;

/**
 * 构造函数
 * 静态数组的缓冲区，大小不能随时改变。
 */
template <>
inline t_StackCmdQueue::ByteBuffer() : _size(PACKET_ZIP_BUFFER), _buffer()
{}

/**
 * 调整缓冲区空间大小
 *
 * @param size 新的空间大小
 */
template <>
inline void t_StackCmdQueue::wr_reserve(const unsigned int size)
{
    // 静态缓冲区无法调整大小
}

/**
 * 封装套接字底层函数
 */
class zSocket : private zNoncopyable
{
public:

    // 读取超时的毫秒数
    static const int T_RD_MSEG = 2100;

    // 发送超时的毫秒数
    static const int T_WR_MSEG = 5100;

    // 数据包包头长度
    static constexpr unsigned int PACKET_HEAD_LEN = sizeof(unsigned int);

    // 数据包压缩最小长度
    static const unsigned int PACKET_ZIP_MIN = 32;

    // 数据包压缩标志
    static const unsigned int PACKET_ZIP = 0x40000000;

    // 上次对套接字进行读取操作没有读取完全的标志
    static const unsigned int INCOMPLETE_READ = 0x00000001;

    // 上次对套接字进行写入操作没有写入完毕的标志
    static const unsigned int INCOMPLETE_WRITE = 0x00000002;

    // 最大数据包长度掩码
    static constexpr unsigned int PACKET_MASK = TRUNK_SIZE - 1;

    // 数据包最大长度，包括包头4字节。
    static constexpr unsigned int MAX_DATABUFFERSIZE = PACKET_MASK;

    // 数据包最大长度
    static constexpr unsigned int MAX_DATASIZE = (MAX_DATABUFFERSIZE - PACKET_HEAD_LEN);

    // 用户数据包最大长度
    static constexpr unsigned int MAX_USERDATASIZE = (MAX_DATASIZE - 128);

    static const char *getIPByIfName(const char *);

    explicit zSocket(int, struct sockaddr_in const * = nullptr, bool = false);
    ~zSocket() override ;

    int recvCmd(void *, int, bool);
    bool sendCmd(const void *, unsigned int, bool = false);
    bool sendCmdWithoutPack(const void *, unsigned int, bool = false);
    bool sync();
    void sync_force();

    int checkIORead();
    int checkIOWrite();
    int recvBufPolled();
    int recvCmdPolled(void *, int);
    int recvCharPolled(char *, int);

    template <typename BufferType>
    static unsigned int packPacketZip(const void *, unsigned int, BufferType&, bool = false);

    /**
     * 获取对端套接字的IP地址（点分十进制）
     *
     * @return IP地址
     */
    const char* getPeerIP() const
    {
        return inet_ntop(AF_INET, (void *) &(peer_addr.sin_addr), peer_ip, sizeof(peer_ip));
    }

    /**
     * 获取对端套接字的IP地址
     *
     * @return IP地址
     */
    const unsigned int getPeerAddr() const
    {
        return peer_addr.sin_addr.s_addr;
    }

    /**
     * 获取对端套接字的端口号
     *
     * @return 端口号
     */
    const unsigned short getPeerPort() const
    {
        return ntohs(peer_addr.sin_port);
    }

    /**
     * 获取本地套接字的IP地址（点分十进制）
     *
     * @return IP地址
     */
    const char* getLocalIP() const
    {
        return inet_ntop(AF_INET, (void *) &(local_addr.sin_addr), local_ip, sizeof(local_ip));
    }

    /**
     * 获取本地套接字的IP地址
     *
     * @return IP地址
     */
    const unsigned int getLocalAddr() const
    {
        return local_addr.sin_addr.s_addr;
    }

    /**
     * 获取对端套接字的端口号
     *
     * @return 端口号
     */
    const unsigned short getLocalPort() const
    {
        return ntohs(local_addr.sin_port);
    }

    /**
     * 设置读取超时时间
     *
     * @param msec 超时时间，单位：毫秒。
     */
    void setReadTimeout(const int msec)
    {
        rd_msec = msec;
    }

    /**
     * 设置写入超时时间
     *
     * @param msec 超时时间，单位：毫秒。
     */
    void setWriteTimeout(const int msec)
    {
        wr_msec = msec;
    }

    /**
     * 设置加解密方式
     *
     * @param method 加解密方式
     */
    void setCryptMethod(zCrypt::CryptMethod method)
    {
        crypt.setCryptMethod(method);
    }

    /**
     * 设置DES密钥
     *
     * @param key 密钥
     */
    void setCryptDesKey(const char *key)
    {
        crypt.setDesKey(key);
    }

    /**
     * 设置DES密钥
     *
     * @param key 密钥
     */
    void setCryptDesKey(const unsigned char *key)
    {
        crypt.setDesKey((const char *) key);
    }

    /**
     * 设置RC5密钥
     *
     * @param key 密钥
     */
    void setCryptRc5Key(const char *key)
    {
        crypt.setRc5Key(key);
    }

    /**
     * 设置RC5密钥
     *
     * @param key 密钥
     */
    void setCryptRc5Key(const unsigned char *key)
    {
        crypt.setRc5Key((const char *) key);
    }

    /**
     * 添加事件到pollfd
     *
     * @param fds    pollfd
     * @param events 事件
     */
    void addPoll(struct pollfd &fds, short events)
    {
        fds.fd = sockfd;
        fds.events = events;
        fds.revents = 0;
    }

    /**
     * 添加事件到epoll描述符
     *
     * @param epfd   epoll描述符
     * @param events 待添加的事件
     * @param ptr    额外参数
     */
    void addEpoll(int epfd, uint32_t events, void *ptr)
    {
        struct epoll_event ev {};

        ev.events = events;
        ev.data.ptr = ptr;
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev)) {
            char error_buff[100] = {0};
            strerror_rx(errno, error_buff, sizeof(error_buff));

            Zhs::logger->fatal("%s: %s", __PRETTY_FUNCTION__, error_buff);
        }
    }

    /**
     * 从epoll描述符中删除事件
     *
     * @param epfd   epoll描述符
     * @param events 待删除的事件
     */
    void delEpoll(int epfd, uint32_t events)
    {
        struct epoll_event ev {};

        ev.events = events;
        ev.data.ptr = nullptr;
        if (-1 == epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev)) {
            char error_buff[100] = {0};
            strerror_rx(errno, error_buff, sizeof(error_buff));

            Zhs::logger->fatal("%s: %s", __PRETTY_FUNCTION__, error_buff);
        }
    }

    /**
     * 返回队列大小（接收队列 + 发送队列）
     *
     * @return 队列大小
     */
    unsigned int getQueueSize() const
    {
        return rcv_queue.size() + snd_queue.size();
    }

    // 定义用于网络传输的数据缓冲类型
    typedef BYTE t_DataBuffer[MAX_DATASIZE];

private:

    // 连接套接字
    int sockfd;

    // 对端套接字地址
    struct sockaddr_in peer_addr;

    // 对端套接字IP
    mutable char peer_ip[16] {};

    // 本地套接字地址
    struct sockaddr_in local_addr;

    // 本地套接字IP
    mutable char local_ip[16] {};

    // 读取超时，毫秒。
    int rd_msec;

    // 写入超时，毫秒。
    int wr_msec;

    // 接收缓冲已解密数据大小
    unsigned int rcv_raw_size;

    // 接收缓冲指令队列
    t_BufferCmdQueue rcv_queue;

    // 发送缓冲指令队列
    t_BufferCmdQueue snd_queue;

    // 发送缓冲指令队列(加密)
    t_BufferCmdQueue enc_queue;

    // 发送缓冲指令队列中，最后一个指令的长度。
    unsigned int snd_current;

    // 标志掩码
    unsigned int bitmask;

    // 互斥锁
    zMutex mutex;

    // 最后一次检测时间
    zTime last_check_time;

    // 加解密对象
    zCrypt crypt;

    /**
     * 对套接字进行poll的读操作轮询
     *
     * @param timeout 延迟时间
     * @return        -1：失败
     *                 0：超时
     *                 1：套接字有数据能被读取
     */
    int poll_io_read(int timeout)
    {
        struct pollfd pfd {};
        addPoll(pfd, POLLIN | POLLERR | POLLPRI);

        auto retcode = (int) TEMP_FAILURE_RETRY(poll(&pfd, 1, timeout));
        if (retcode > 0 && (pfd.revents & POLLIN) == 0) {
            retcode = -1;
        }

        return retcode;
    };

    /**
     * 对套接字进行poll的写操作轮询
     *
     * @param timeout 延迟时间
     * @return        -1：失败
     *                 0：超时
     *                 1：套接字有数据能被写入
     */
    int poll_io_write(int timeout)
    {
        struct pollfd pfd {};
        addPoll(pfd, POLLOUT | POLLERR | POLLPRI);

        auto retcode = (int) TEMP_FAILURE_RETRY(poll(&pfd, 1, timeout));
        if (retcode > 0 && (pfd.revents & POLLOUT) == 0) {
            retcode = -1;
        }

        return retcode;
    };

    /**
     * 接受经过poll轮询后的套接字数据
     *
     * @return -1：错误
     *          0：超时
     *         >0：实际接受字节数
     */
    int recv_poll()
    {
        auto retcode = waitIORead();

        if (1 == retcode) {
            retcode = (int) TEMP_FAILURE_RETRY(recv(sockfd, rcv_queue.wr_buf(), rcv_queue.wr_size(), 0));
        }

        return retcode;
    }

    /**
     * 成功接受数据后，调整接受缓冲指令队列。
     *
     * @param rcv_size 数据大小
     */
    void recv_success(unsigned int rcv_size)
    {
        if (rcv_size < rcv_queue.wr_size()) {
            setFlag(INCOMPLETE_READ);
        }

        rcv_queue.wr_flip(rcv_size);

        if (isCrypt()) {
            // 计算未解密的数据段长度
            rcv_size = rcv_queue.rd_size() - rcv_raw_size;
            // 调整未解密的数据段长度为８的倍数
            rcv_size -= rcv_size % 8;
            if (rcv_size) {
                // 对未解密的数据段进行解密
                crypt.decrypt(&rcv_queue.rd_buf()[rcv_raw_size], rcv_size);
            }
        }

        rcv_raw_size += rcv_size;
    }

    /**
     * 同步发送指令队列中的数据到加密指令队列
     */
    void sync_enc()
    {
        unsigned int size = 0, offset = 0, current = 0;

        mutex.lock();
        if (snd_queue.rd_ready()) {
            size = snd_queue.rd_size();
            offset = enc_queue.rd_size();
            current = snd_current;

            enc_queue.put(snd_queue.rd_buf(), size);
            snd_queue.rd_flip(size);
            if (isCrypt()) {
                snd_current = 0;
            }
        }
        mutex.unlock();

        if (isCrypt() && size > 0) {
            packPacketCrypt(enc_queue, current, offset);
        }
    }

    int  sendRawData(const void *, unsigned int);
    bool sendRawDataIM(const void *, unsigned int);
    int  sendRawDataPolled(const void *, unsigned int);
    bool setNonblock();
    int  recvBuf();
    int  waitIORead();
    int  waitIOWrite();
    int  unpack(void *);

    unsigned int unpackPacket(BYTE *, unsigned int, BYTE *);

    template<typename BufferType>
    unsigned int appendPacket(const void *, unsigned int, BufferType &);
    template<typename BufferType>
    unsigned int appendPacketWithoutCrypt(const void *, unsigned int, BufferType &);
    template<typename BufferType>
    unsigned int packPacketCrypt(BufferType &, unsigned int, unsigned int = 0);

    void setFlag(unsigned int flag) { bitmask |= flag; }
    void clearFlag(unsigned int flag) { bitmask &= ~flag; }
    bool issetFlag(unsigned int flag) const { return (bitmask & flag) != 0; }

    /**
     * 是否加密
     */
    bool isCrypt() const
    {
        return crypt.getCryptMethod() != zCrypt::CRYPT_NONE;
    }

    /**
     * 是否设置了压缩标志
     *
     * @param head 数据头
     */
    bool isZip(const unsigned int head) const
    {
        return PACKET_ZIP == (head & PACKET_ZIP);
    }

    /**
     * 是否压缩
     */
    bool isCompress() const
    {
        return isZip(bitmask);
    }

    /**
     * 取得数据包的头部
     *
     * @param data 数据包
     * @return     指向头部的指针
     */
    static unsigned int* packetHead(void *data)
    {
        return reinterpret_cast<unsigned int *> (data);
    }

    /**
     * 取得数据包的头部
     *
     * @param data 数据包
     * @return     指向头部的指针
     */
    static unsigned int* packetHead(BYTE *data)
    {
        return reinterpret_cast<unsigned int *> (data);
    }

    /**
     * 取得数据包的头部
     *
     * @param data 数据包
     * @return     指向头部的指针
     */
    static const unsigned int* packetHead(const BYTE *data)
    {
        return reinterpret_cast<const unsigned int *> (data);
    }

    /**
     * 返回数据包包头最小长度
     *
     * @return 最小长度
     */
    unsigned int packetMinSize() const
    {
        return PACKET_HEAD_LEN;
    }

    /**
     * 返回整个数据包的长度
     *
     * @param data 数据包
     * @return     数据包长度
     */
    unsigned int packetSize(const BYTE *data) const
    {
        unsigned int head = *packetHead(data);

        return PACKET_HEAD_LEN + (head & PACKET_MASK);
    }
};


#endif //ZHS_ZSOCKET_H
