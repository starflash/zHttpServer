//
// Created by zhangweiwen on 2016/8/30.
//

#include <cmath>
#include <algorithm>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <zlib.h>

#include "zSocket.h"

/**
 * 根据网卡名称编号获取指定网卡的IP地址(非线程安全)
 *
 * @param ifName 网卡名称
 * @return       IP地址
 */
const char* zSocket::getIPByIfName(const char *ifName)
{
    Zhs::logger->trace("zSocket::getIPByIfName");

    static const char *ip = "0.0.0.0";

    if (nullptr == ifName || strlen(ifName) == 0) {
        Zhs::logger->warn("网卡名字为空");

        return ip;
    }

    int fd;
    if (-1 == (fd = socket(AF_INET, SOCK_DGRAM, 0))) {
        return ip;
    }

    struct ifreq ir {};
    memset(ir.ifr_name, 0, sizeof(ir.ifr_name));
    strncpy(ir.ifr_name, ifName, strlen(ifName));
    if (-1 != ioctl(fd, SIOCGIFADDR, &ir)) {
        ip = inet_ntoa((reinterpret_cast<struct sockaddr_in *> (&ir.ifr_addr))->sin_addr);
    } else {
        char error_buff[100] = {0};
        strerror_rx(errno, error_buff, sizeof(error_buff));

        Zhs::logger->debug("获取网卡(%s)IP地址错误: %s", ifName, error_buff);
    }
    TEMP_FAILURE_RETRY(close(fd));

    return ip;
}

/**
 * 构造函数
 *
 * @param sockfd      连接套接字
 * @param addr        地址
 * @param compressed  底层数据传输是否支持压缩
 */
zSocket::zSocket(const int sockfd, const sockaddr_in *addr, const bool compressed)
{
    Zhs::logger->trace("zSocket::zSocket");

    assert(-1 != sockfd);

    this->sockfd = sockfd;

    socklen_t addrlen = sizeof(struct sockaddr); // 该变量必须初始化，且值够大。
    memset(&this->peer_addr, 0, sizeof(struct sockaddr_in));
    if (nullptr == addr) {
        getpeername(this->sockfd, (struct sockaddr *) &this->peer_addr, &addrlen);
    } else {
        memcpy(&this->peer_addr, addr, sizeof(struct sockaddr_in));
    }
    memset(&this->local_addr, 0, sizeof(struct sockaddr_in));
    getsockname(this->sockfd, (struct sockaddr *) &this->local_addr, &addrlen);

    fcntl(this->sockfd, F_SETFD, fcntl(this->sockfd, F_GETFD) | FD_CLOEXEC);

    setNonblock();

    rd_msec = T_RD_MSEG;
    wr_msec = T_WR_MSEG;
    rcv_raw_size = snd_current = 0;

    setFlag(INCOMPLETE_READ | INCOMPLETE_WRITE);
    if (compressed) {
        setFlag(PACKET_ZIP);
    }
}

/**
 * 析构函数
 */
zSocket::~zSocket()
{
    Zhs::logger->trace("zSocket::~zSocket");

    shutdown(sockfd, SHUT_RDWR);
    TEMP_FAILURE_RETRY(close(sockfd));

    sockfd = -1;
}

/**
 * 把缓冲数据解压
 *
 * @param cmd 解压后的数据
 * @return    解压后的数据长度
 */
inline int zSocket::unpack(void *data)
{
    int retval = 0;

    if (rcv_raw_size >= packetMinSize()) {
        unsigned int len = packetSize(rcv_queue.rd_buf());

        if (rcv_raw_size >= len) {
            retval = unpackPacket(rcv_queue.rd_buf(), len, static_cast<BYTE *> (data));
            rcv_queue.rd_flip(len);
            rcv_raw_size -= len;
        }
    }

    return retval;
}

/**
 * 对数据进行解压
 *
 * @param data 待解压数据
 * @param len  待解压数据长度，包括包头。
 * @param out  解压后数据
 * @return     解压后数据长度
 */
inline unsigned int zSocket::unpackPacket(BYTE *data, const unsigned int len, BYTE *out)
{
    Zhs::logger->trace("zSocket::unpackPacket");

    unsigned int packet_size = len - PACKET_HEAD_LEN;
    ulong unpacket_size = MAX_DATASIZE;

    if (isZip(*packetHead(data))) {
        switch(uncompress(out, &unpacket_size, &data[PACKET_HEAD_LEN], packet_size)) {
            case Z_OK:
                break;
            case Z_MEM_ERROR:
                Zhs::logger->fatal("%s: Z_MEM_ERROR", __PRETTY_FUNCTION__);
                break;
            case Z_DATA_ERROR:
                Zhs::logger->fatal("%s: Z_DATA_ERROR", __PRETTY_FUNCTION__);
                break;
            case Z_BUF_ERROR:
                Zhs::logger->fatal("%s: Z_BUF_ERROR", __PRETTY_FUNCTION__);
                break;
            default:
                break;
        }
    } else {
        memcpy(out, &data[PACKET_HEAD_LEN], packet_size);
        unpacket_size = packet_size;
    }

    return static_cast<unsigned int> (unpacket_size);
}

// 分配静态常量地址。
constexpr unsigned int zSocket::MAX_DATASIZE;

/**
 * 对数据进行压缩，由调用者判断是否需要加密，这里只负责加密不作判断。
 *
 * @tparam BufferType  数据缓冲区类型
 * @param data         待压缩的数据
 * @param len          待压缩的数据长度
 * @param packet       压缩以后的数据
 * @param compressed   是否压缩
 * @return             加密以后的数据长度
 */
template <typename BufferType>
unsigned int zSocket::packPacketZip(const void *data, const unsigned int len, BufferType &packet, bool compressed)
{
    unsigned int mask = 0, datasize = std::min(len, MAX_DATASIZE);

    if (datasize > PACKET_ZIP_MIN && compressed) { // 数据包足够大且带压缩标记
        ulong zipsize = compressBound(datasize);
        packet.wr_reserve(PACKET_HEAD_LEN + zipsize);
        switch(compress(&(packet.wr_buf()[PACKET_HEAD_LEN]), &zipsize, static_cast<const Bytef *> (data), datasize)) {
            case Z_OK:
                break;
            case Z_BUF_ERROR:
                Zhs::logger->fatal("%s: Z_BUF_ERROR", __PRETTY_FUNCTION__);
                break;
            case Z_MEM_ERROR:
                Zhs::logger->fatal("%s: Z_MEM_ERROR", __PRETTY_FUNCTION__);
                break;
            default:
                break;
        }
        datasize = static_cast<unsigned int>(zipsize);
        mask |= PACKET_ZIP;
    } else {
        packet.wr_reserve(PACKET_HEAD_LEN + datasize);
        memcpy(&(packet.wr_buf()[PACKET_HEAD_LEN]), data, datasize);
    }
    *(packetHead(packet.wr_buf())) = datasize | mask;
    packet.wr_flip(PACKET_HEAD_LEN + datasize);

    return PACKET_HEAD_LEN + datasize;
}

/**
 * 对数据进行加密
 *
 * @tparam BufferType 数据缓冲区类型
 * @param data        待加密的数据
 * @param size        待加密的数据中最后一条指令的长度
 * @param offset      待加密的数据的相对偏移量
 * @return            加密以后的数据大小
 */
template <typename BufferType>
inline unsigned int zSocket::packPacketCrypt(BufferType &data, const unsigned int size, const unsigned int offset)
{
    auto remainder = (data.rd_size() - offset) % 8;

    if (0 != remainder) { // 加密需要数据长度是8的倍数
        // 计算不足的字节数
        remainder = 8 - remainder;

        // 补齐最后一条指令的长度为8的倍数
        *packetHead(&(data.rd_buf()[data.rd_size() - size])) += remainder;

        // 使得可读数据长度为8的倍数
        data.wr_flip(remainder);
    }

    // 加密
    crypt.encrypt(&data.rd_buf()[offset], data.rd_size() - offset);

    return data.rd_size();
}

/**
 * 对数据进行打包，必要时压缩，不加密。
 *
 * @tparam BufferType 数据缓冲区类型
 * @param data        原始数据
 * @param len         原始数据长度
 * @param packet      打包后的数据
 * @return            打包后的数据长度
 */
template <typename BufferType>
inline unsigned int zSocket::appendPacketWithoutCrypt(const void *data, const unsigned int len, BufferType &packet)
{
    return packPacketZip(data, len, packet, isCompress());
}

/**
 * 对数据进行打包，必要时压缩，并加密。
 *
 * @tparam BufferType 数据缓冲区类型
 * @param data        原始数据
 * @param len         原始数据长度
 * @param packet      打包后的数据
 * @return            打包后的数据长度
 */
template <typename BufferType>
inline unsigned int zSocket::appendPacket(const void *data, const unsigned int len, BufferType &packet)
{
    unsigned int packet_size;

    packet_size = packPacketZip(data, len, packet, isCompress());
    if (isCrypt()) {
        packet_size = packPacketCrypt(packet, packet.rd_size());
    }

    return packet_size;
}

/**
 * 接收数据到缓冲区
 * 如果是加密包需要解密到解密缓冲区
 *
 * @return -1：错误
 *          0：超时
 *         >0：实际接受字节数，如果是加密数据，则返回实际可用字节数。
 */
inline int zSocket::recvBuf()
{
    Zhs::logger->trace("zSocket::recvBuf");

    int retcode = 0;

    if (issetFlag(INCOMPLETE_READ)) {
        clearFlag(INCOMPLETE_READ);

        retcode = recv_poll();
    } else {
        rcv_queue.wr_reserve(MAX_DATABUFFERSIZE);
        retcode = (int) TEMP_FAILURE_RETRY(recv(sockfd, rcv_queue.wr_buf(), rcv_queue.wr_size(), 0));
        if (0 == retcode) {
            retcode = -1;

            Zhs::logger->debug("服务器端主动关闭了连接");
        } else if (-1 == retcode && (EAGAIN == errno || EWOULDBLOCK == errno)) {
            retcode = recv_poll();
        }
    }

    if (retcode > 0) {
        recv_success(static_cast<unsigned int> (retcode));
    }

    return retcode;
}

/**
 * 接收指令到缓冲区
 *
 * @param cmd  指令缓冲区
 * @param len  指令缓冲区长度
 * @param wait 当套接字数据没有准备好的时候，是否需要等待。
 * @retur      实际接收的指令长度 -1：错误
 *                              0：超时
 *                             >0：实际长度
 */
int zSocket::recvCmd(void *cmd, const int len, const bool wait)
{
    Zhs::logger->trace("zSocket::recvCmd");

    int cmdlen, recvlen;

    while (0 == (cmdlen = unpack(cmd))) {
        recvlen = recvBuf();
        if (-1 == recvlen || (0 == recvlen && !wait)) {
            return recvlen;
        }
    }

    return cmdlen;
}

/**
 * 接收数据到缓冲区
 * 如果是加密包需要先解密到解密缓冲区
 * 需要保证在调用本函数之前套接字准备好被写入，也就是使用poll轮询过。
 *
 * @return -1：错误
 *          0：超时
 *         >0：实际接受字节数
 */
int zSocket::recvBufPolled()
{
    Zhs::logger->trace("zSocket::recvBufPolled");

    rcv_queue.wr_reserve(MAX_DATABUFFERSIZE);

    auto retcode = (int) TEMP_FAILURE_RETRY(recv(sockfd, rcv_queue.wr_buf(), rcv_queue.wr_size(), 0));
    if (retcode > 0) {
        recv_success((unsigned int) retcode);
    } else if (retcode == -1 && (EAGAIN == errno || EWOULDBLOCK == errno)) {
        retcode = 0;
    } else if (retcode == 0) {
        retcode = -1;
    }

    return retcode;
}

/**
 * 接收指令到缓冲区
 * 不从套接字接收指令，只是把接收缓冲区的数据解包。
 *
 * @param cmd 指令缓冲区
 * @param len 指令缓冲区的长度
 * @return    -1：错误
 *             0：超时
 *            >0：实际接受字节数
 */
int zSocket::recvCmdPolled(void *cmd, const int len)
{
    Zhs::logger->trace("zSocket::recvCmdPolled");

    return unpack(cmd);
}

/**
 * 接受字符数据到缓冲区
 * 不从套接字接收指令，只是把接收缓冲区的数据解包。
 *
 * @param data 数据缓冲区
 * @param len  数据缓冲区长度
 * @return     -1：错误
 *              0：超时
 *             >0：实际接受字节数
 */
int zSocket::recvCharPolled(char *data, const int len)
{
    Zhs::logger->trace("zSocket::recvCharPolled");

    if (rcv_raw_size > 0) {
        auto *buf = (char *) rcv_queue.rd_buf();
        auto size = static_cast<unsigned int>(strlen(buf));

        if (size < len) {
            strcpy(data, buf);
            rcv_queue.rd_flip(size);
            rcv_raw_size -= size;

            return size;
        } else {
            return -1;
        }
    }

    return 0;
}

/**
 * 向套接字发送原始数据
 * 如果是没有打包的数据，一般发送数据的时候需要加入额外的包头。
 *
 * @param buffer 待发送的原始数据
 * @param size   待发送的原始数据大小
 * @return       实际发送的字节数，-1：错误
 *                               0：超时
 *                              >0：实际发送的字节数
 */
inline int zSocket::sendRawData(const void *buffer, const unsigned int size)
{
    Zhs::logger->trace("zSocket::sendRawData(%s:%d)", getPeerIP(), getPeerPort());

    int retcode = 0;

    if (issetFlag(INCOMPLETE_WRITE)) {
        clearFlag(INCOMPLETE_WRITE);

        if (1 == (retcode = waitIOWrite())) {
            retcode = (int) TEMP_FAILURE_RETRY(send(sockfd, buffer, size, MSG_NOSIGNAL));
        }
    } else {
        retcode = (int) TEMP_FAILURE_RETRY(send(sockfd, buffer, size, MSG_NOSIGNAL));
        if (-1 == retcode && (EAGAIN == errno || EWOULDBLOCK == errno))
        {
            // 当设置O_NONBLOCK属性后，如果发送缓存被占满，send函数就会返回EAGAIN或EWOULDBLOCK的错误。
            if (1 == (retcode = waitIOWrite())) {
                retcode = (int) TEMP_FAILURE_RETRY(send(sockfd, buffer, size, MSG_NOSIGNAL));
            }
        }
    }

    if (retcode > 0 && retcode < size) {
        setFlag(INCOMPLETE_WRITE);
    }

    Zhs::logger->trace("zSocket::sendRawData 发送了%u字节数据", retcode);

    return retcode;
}

/**
 * 发送指定字节数的原始数据，忽略超时，直到发送完毕或者发送失败。
 *
 * @param buffer 待发送的原始数据
 * @param size   待发送的原始数据大小
 * @return       发送数据是否成功
 */
inline bool zSocket::sendRawDataIM(const void *buffer, const unsigned int size)
{
    Zhs::logger->trace("zSocket::sendRawDataIM");

    if (nullptr == buffer || size <= 0) {
        return false;
    }

    int offset = 0, sent;

    do {
        sent = sendRawData(&(static_cast<const char *> (buffer))[offset], size - offset);
        if (-1 == sent) {
            return false;
        } else {
            offset += sent;
        }
    } while (offset < size);

    return offset == size;
}

/**
 * 向套接字发送原始数据
 * 如果是没有打包的数据，一般发送数据的时候需要加入额外的包头。
 * 需要确保在调用本函数之前套接字准备好被写入，也就是使用poll轮询过。
 *
 * @param buffer 待发送的原始数据
 * @param size   待发送的原始数据大小
 * @retu         实际发送的字节数，-1：错误
 *                               0：超时
 *                              >0：实际发送的字节数
 */
inline int zSocket::sendRawDataPolled(const void *buffer, const unsigned int size)
{
    Zhs::logger->trace("zSocket::sendRawDataPolled");

    int retcode;

    retcode = (int) TEMP_FAILURE_RETRY(send(sockfd, buffer, size, MSG_NOSIGNAL));
    if (-1 == retcode && (EAGAIN == errno || EWOULDBLOCK == errno)) {
        retcode = 0;
    } else if (retcode > 0 && retcode < size) {
        setFlag(INCOMPLETE_WRITE);
    }

    return retcode;
}

/**
 * 发送指令
 *
 * @param cmd    待发送的指令
 * @param len    待发送的指令长度
 * @param buffer 是否需要缓冲
 * @return       是否发送成功
 */
bool zSocket::sendCmd(const void *cmd, const unsigned int len, const bool buffer)
{
    Zhs::logger->trace("zSocket::sendCmd buffer=(%d)", buffer);

    if (nullptr == cmd || len <= 0) {
        return false;
    }

    bool retval = true;
    t_StackCmdQueue raw_queue;

    if (buffer) {
        appendPacketWithoutCrypt(cmd, len, raw_queue);
        mutex.lock();
        snd_queue.put(raw_queue.rd_buf(), raw_queue.rd_size());
        snd_current = raw_queue.rd_size();
        mutex.unlock();
    } else {
        appendPacket(cmd, len, raw_queue);
        mutex.lock();
        retval = sendRawDataIM(raw_queue.rd_buf(), raw_queue.rd_size());
        mutex.unlock();
    }

    return retval;
}

/**
 * 发送原始指令数据，不打包数据。
 *
 * @param cmd    待发送的指令
 * @param len    待发送的指令长度
 * @param buffer 是否需要缓冲
 * @return       是否发送成功
 */
bool zSocket::sendCmdWithoutPack(const void *cmd, const unsigned int len, const bool buffer)
{
    Zhs::logger->trace("zSocket::sendCmdWithoutPack");

    if (nullptr == cmd || len <= 0) {
        return false;
    }

    bool retval = true;

    if (buffer) {
        mutex.lock();
        snd_queue.put(static_cast<const BYTE *> (cmd), len);
        snd_current = len;
        mutex.unlock();
    } else {
        if (isCrypt()) {
            t_StackCmdQueue raw_queue;

            raw_queue.put(static_cast<const BYTE *> (cmd), len);
            packPacketCrypt(raw_queue, raw_queue.rd_size());
            mutex.lock();
            retval = sendRawDataIM(raw_queue.rd_buf(), raw_queue.rd_size());
            mutex.unlock();
        } else {
            mutex.lock();
            retval = sendRawDataIM(cmd, len);
            mutex.unlock();
        }
    }

    return retval;
}

/**
 * 同步发送指令队列中的数据到加密指令队列，然后发送加密数据。
 *
 * @return 发送数据是否成功
 */
bool zSocket::sync()
{
    Zhs::logger->trace("zSocket::sync");

    sync_enc();

    if (enc_queue.rd_ready()) {
        int retcode = sendRawDataPolled(enc_queue.rd_buf(), enc_queue.rd_size());
        if (retcode > 0) {
            enc_queue.rd_flip(static_cast<unsigned int>(retcode));
        } else if (retcode == -1) {
            return false;
        }
    }

    return true;
}

/**
 * 同步发送指令队列中的数据到加密指令队列，然后发送加密数据，直到数据发送完毕。
 */
void zSocket::sync_force()
{
    Zhs::logger->trace("zSocket::sync_force");

    sync_enc();

    if (enc_queue.rd_ready()) {
        sendRawDataIM(enc_queue.rd_buf(), enc_queue.rd_size());
        enc_queue.reset();
    }
}

/**
 * 检查套接字是否可以读取操作
 *
 * @return -1：失败
 *          0：超时
 *          1：套接字有数据能被读取
 */
int zSocket::checkIORead()
{
    Zhs::logger->trace("zSocket::checkIORead");

    return poll_io_read(0);
}

/**
 * 检查套接字是否可以写入操作
 *
 * @return -1：失败
 *          0：超时
 *          1：套接字能被写入数据
 */
int zSocket::checkIOWrite()
{
    Zhs::logger->trace("zSocket::checkIOWrite");

    return poll_io_write(0);
}

/**
 * 等待套接字准备好读取操作
 *
 * @return -1：失败
 *          0：超时
 *          1：套接字有数据能被读取
 */
inline int zSocket::waitIORead()
{
    Zhs::logger->trace("zSocket::waitIORead");

    return poll_io_read(rd_msec);
}

/**
 * 等待套接字准备好写入操作
 *
 * @return -1：失败
 *          0：超时
 *          1：套接字能被写入数据
 */
inline int zSocket::waitIOWrite()
{
    Zhs::logger->trace("zSocket::waitIOWrite");

    return poll_io_write(wr_msec);
}

/**
 * 设置套接字为非阻塞模式
 *
 * @return 是否设置成功
 */
inline bool zSocket::setNonblock()
{
    Zhs::logger->trace("zSocket::setNonblock");

    int optval = 1;
    if (0 != setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &optval, sizeof(optval))) {
        return false;
    }

    return -1 != fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);

}