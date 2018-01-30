//
// Created by zhangweiwen on 17-2-5.
//

#ifndef ZHS_ZSERVERINFO_H
#define ZHS_ZSERVERINFO_H

#include <unistd.h>
#include <cmath>
#include <sys/sysinfo.h>
#include <vector>
#include <cstring>

#include <zhs/base/zCommon.h>
#include <zhs/misc/zTime.h>

/**
 * 收集服务器信息
 */
class zServerInfo
{
public:

    // 网卡信息
    struct IfInfo
    {
        // 字节数据
        struct BytesInfo
        {
            zTime ts;						// 时间戳
            unsigned long long rx_bytes;	// 接收字节数
            unsigned long long tx_bytes;	// 发送字节数

            /**
             * 默认构造函数
             */
            BytesInfo() : ts()
            {
                rx_bytes = 0;
                tx_bytes = 0;
            }

            /**
             * 复制构造函数
             *
             * @param bi 待复制的数据
             */
            BytesInfo(const BytesInfo &bi)
            {
                ts = bi.ts;
                rx_bytes = bi.rx_bytes;
                tx_bytes = bi.tx_bytes;
            }

            /**
             * 重载=操作符
             *
             * @param bi 待赋值的数据
             * @return   赋值后的数据
             */
            BytesInfo& operator=(const BytesInfo &bi)
            {
                ts = bi.ts;
                rx_bytes = bi.rx_bytes;
                tx_bytes = bi.tx_bytes;

                return *this;
            }
        };

        // 上次扫描网卡信息
        BytesInfo prev;

        // 当前扫描网卡信息
        BytesInfo current;

        // 两次扫描间隔时间
        float delta;

        // 网卡名称
        char ifname[32] {};

        // 当前接收的数据包数
        unsigned long long rx_packets;

        // 当前发送的数据包数
        unsigned long long tx_packets;

        // 接收流量速度
        float rx_speed;

        // 接收流量速度最大值
        float rx_speed_max;

        // 发送流量速度
        float tx_speed;

        // 发送流量速度最大值
        float tx_speed_max;

        /**
         * 默认构造函数
         */
        IfInfo() : prev(), current()
        {
            memset(ifname, 0, sizeof(ifname));
            delta = 0.0;
            rx_packets = 0LL;
            tx_packets = 0LL;
            rx_speed = 0.0;
            rx_speed_max = 0.0;
            tx_speed = 0.0;
            tx_speed_max = 0.0;
        }

        /**
         * 复制构造函数
         *
         * @param ifi 待复制数据
         */
        IfInfo(const IfInfo &ifi)
        {
            strcpy(ifname, ifi.ifname);
            prev = ifi.prev;
            current = ifi.current;
            delta = ifi.delta;
            rx_packets = ifi.rx_packets;
            tx_packets = ifi.tx_packets;
            rx_speed = ifi.rx_speed;
            rx_speed_max = ifi.rx_speed_max;
            tx_speed = ifi.tx_speed;
            tx_speed_max = ifi.tx_speed_max;
        }

        /**
         * 重载=操作符
         *
         * @param ifi 待赋值的数据
         * @return    赋值后的数据
         */
        IfInfo& operator=(const IfInfo &ifi)
        {
            strcpy(ifname, ifi.ifname);
            prev = ifi.prev;
            current = ifi.current;
            delta = ifi.delta;
            rx_packets = ifi.rx_packets;
            tx_packets = ifi.tx_packets;
            rx_speed = ifi.rx_speed;
            rx_speed_max = ifi.rx_speed_max;
            tx_speed = ifi.tx_speed;
            tx_speed_max = ifi.tx_speed_max;

            return *this;
        }
    };

    // CPU信息
    struct CPUInfo
    {
        // 单个CPU信息
        struct CPU
        {
            // 单个CPU的使用情况
            struct TickInfo
            {
                unsigned long long u,       // user: normal processes executing in user mode
                                   n,		// nice: niced processes executing in user mode
                                   s,		// system: processes executing in kernel mode
                                   i,		// idle: twiddling thumbs
                                   w,		// iowait: waiting for I/O to complete
                                   x,		// irq: servicing interrupts
                                   y;		// softirq: servicing softirqs

                // 构造函数
                TickInfo()
                {
                    u = n = s = i = w = x = y = 0;
                }

                // 复制构造函数
                TickInfo(const TickInfo &ti) = default;

                // 产生默认的赋值函数
                TickInfo& operator=(const TickInfo &ti) = default;

                /**
                 * 根据已采集数据计算利用率
                 *
                 * @param prev    上一次采集的数据
                 * @param current 当前采集的数据
                 */
                void calculate(const TickInfo &prev, const TickInfo &current)
                {
                    float scale;
                    long long tz, u_frme, s_frme, n_frme, i_frme, w_frme, x_frme, y_frme, tot_frme;

                    tz = current.i - prev.i;
                    u_frme = current.u - prev.u;
                    s_frme = current.s - prev.s;
                    n_frme = current.n - prev.n;
                    i_frme = static_cast<long long>(fmax(0, tz));
                    w_frme = current.w - prev.w;
                    x_frme = current.x - prev.x;
                    y_frme = current.y - prev.y;
                    tot_frme = static_cast<long long>(fmax(1, u_frme + s_frme + n_frme + i_frme + w_frme + x_frme + y_frme));
                    scale = static_cast<float>(1000.0 / (float) tot_frme);

                    u = (unsigned long long) ((float) u_frme * scale);
                    s = (unsigned long long) ((float) s_frme * scale);
                    n = (unsigned long long) ((float) n_frme * scale);
                    i = (unsigned long long) ((float) i_frme * scale);
                    w = (unsigned long long) ((float) w_frme * scale);
                    x = (unsigned long long) ((float) x_frme * scale);
                    y = (unsigned long long) ((float) y_frme * scale);
                }
            };

            // CPU编号
            const unsigned int id;

            // 上一次采集的数据
            TickInfo prev;

            // 当前采集的数据
            TickInfo current;

            // 计算出来的CPU各项利用率
            TickInfo usage;

            // 构造函数
            explicit CPU(const unsigned int id): id(id), prev(), current(), usage()
            {}

            // 复制构造函数
            CPU(const CPU &cpu): id(cpu.id)
            {
                prev = cpu.prev;
                current = cpu.current;
                usage = cpu.usage;
            }

            // 重载=运算符
            CPU& operator=(const CPU &cpu)
            {
                prev = cpu.prev;
                current = cpu.current;
                usage = cpu.usage;

                return *this;
            }
        };

        // CPU数量
        const unsigned int count;

        // 每个CPU的使用率
        std::vector<CPU> usage;

        /**
         * 构造函数
         *
         * @param count CPU数量
         */
        explicit CPUInfo(unsigned int count = countCPU()) : count(count)
        {
            for (unsigned int i = 0; i <= count; i++) {
                CPU cpu(i);
                usage.push_back(cpu);
            }
        }
    };

    /**
     * 获取可用CPU数量
     *
     * @return CPU数量
     */
    static unsigned int countCPU()
    {
        return static_cast<unsigned int>(sysconf(_SC_NPROCESSORS_ONLN));
    }

    std::vector<IfInfo> ifs;
    CPUInfo cpu;

    int getCPUInfo();
    std::vector<struct IfInfo>::size_type getIfInfo();
    void getServerInfo(std::string &);

private:

    void getCPUInfoLine(FILE *, char const *, unsigned int);
};


#endif //ZHS_ZSERVERINFO_H
