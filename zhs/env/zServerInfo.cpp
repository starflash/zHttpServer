//
// Created by zhangweiwen on 17-2-5.
//

#include <sys/utsname.h>

#include <zhs/misc/zXMLParser.h>
#include <zhs/net/zSocket.h>
#include "zServerInfo.h"

/**
 * 获取服务器所有信息
 * 包括网卡信息，内存信息，CPU信息，系统信息以及进程信息。
 *
 * @param xml 信息以XML格式返回
 */
void zServerInfo::getServerInfo(std::string &xml)
{
    zXMLParser xmlParser;
    xmlNodePtr root, node, snode;
    char buffer[512];

    if (xmlParser.initXML() && nullptr != (root = xmlParser.newRootNode("Server"))) {
        node = xmlParser.newChildNode(root, "SYS", nullptr);
        if (node) {
            struct utsname uts {};

            if (0 == uname(&uts)) {
                xmlParser.newNodeProp(node, "sysname", uts.sysname);
                xmlParser.newNodeProp(node, "nodename", uts.nodename);
                xmlParser.newNodeProp(node, "release", uts.release);
                xmlParser.newNodeProp(node, "version", uts.version);
                xmlParser.newNodeProp(node, "machine", uts.machine);
                xmlParser.newNodeProp(node, "domainname", uts.domainname);
            }

            struct sysinfo si {};

            if (0 == sysinfo(&si)) {
                snprintf(buffer, sizeof(buffer) - 1, "%lu", time(nullptr));
                xmlParser.newNodeProp(node, "time", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%lu", si.uptime);
                xmlParser.newNodeProp(node, "uptime", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.2f, %.2f, %.2f",
                         (float) si.loads[0] / 65536.0, (float) si.loads[1] / 65536.0, (float) si.loads[2] / 65536.0);
                xmlParser.newNodeProp(node, "loads", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%u", si.procs);
                xmlParser.newNodeProp(node, "processes", buffer);

                snode = xmlParser.newChildNode(node, "MEM", nullptr);
                if (snode) {
                    snprintf(buffer, sizeof(buffer) - 1, "%u", si.mem_unit);
                    xmlParser.newNodeProp(snode, "unit", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.totalram);
                    xmlParser.newNodeProp(snode, "totalram", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.freeram);
                    xmlParser.newNodeProp(snode, "freeram", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.sharedram);
                    xmlParser.newNodeProp(snode, "sharedram", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.bufferram);
                    xmlParser.newNodeProp(snode, "bufferram", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.totalswap);
                    xmlParser.newNodeProp(snode, "totalswap", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.freeswap);
                    xmlParser.newNodeProp(snode, "freeswap", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.totalhigh);
                    xmlParser.newNodeProp(snode, "totalhigh", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%lu", si.freehigh);
                    xmlParser.newNodeProp(snode, "freehigh", buffer);
                }
            }
        }

        getCPUInfo();
        node = xmlParser.newChildNode(root, "CPUTS", nullptr);
        if (node) {
            snprintf(buffer, sizeof(buffer) - 1, "%u", cpu.count);
            xmlParser.newNodeProp(node, "total", buffer);
            snode = xmlParser.newChildNode(node, "cpus", nullptr);
            if (snode) {
                snprintf(buffer, sizeof(buffer) - 1, "%u", cpu.count);
                xmlParser.newNodeProp(snode, "id", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.u / 10.0);
                xmlParser.newNodeProp(snode, "user", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.n / 10.0);
                xmlParser.newNodeProp(snode, "nice", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.s / 10.0);
                xmlParser.newNodeProp(snode, "system", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.i / 10.0);
                xmlParser.newNodeProp(snode, "idle", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.w / 10.0);
                xmlParser.newNodeProp(snode, "iowait", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.x / 10.0);
                xmlParser.newNodeProp(snode, "irq", buffer);
                snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[cpu.count].usage.y / 10.0);
                xmlParser.newNodeProp(snode, "softirq", buffer);
            }
            for (std::vector<CPUInfo::CPU>::size_type i = 0; i < cpu.count; i++) {
                snprintf(buffer, sizeof(buffer) - 1, "%s", "cpu");
                snode = xmlParser.newChildNode(node, buffer, nullptr);
                if (snode) {
                    snprintf(buffer, sizeof(buffer) - 1, "%zu", i);
                    xmlParser.newNodeProp(snode, "id", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.u / 10.0);
                    xmlParser.newNodeProp(snode, "user", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.n / 10.0);
                    xmlParser.newNodeProp(snode, "nice", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.s / 10.0);
                    xmlParser.newNodeProp(snode, "system", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.i / 10.0);
                    xmlParser.newNodeProp(snode, "idle", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.w / 10.0);
                    xmlParser.newNodeProp(snode, "iowait", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.x / 10.0);
                    xmlParser.newNodeProp(snode, "irq", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.1f", (float) cpu.usage[i].usage.y / 10.0);
                    xmlParser.newNodeProp(snode, "softirq", buffer);
                }
            }
        }

        getIfInfo();
        node = xmlParser.newChildNode(root, "NET", nullptr);
        if (node) {
            snprintf(buffer, sizeof(buffer) - 1, "%zu", ifs.size());
            xmlParser.newNodeProp(node, "total", buffer);
            for (auto &i : ifs) {
                snprintf(buffer, sizeof(buffer) - 1, "%s", i.ifname);
                snode = xmlParser.newChildNode(node, buffer, nullptr);
                if (snode)
                {
                    snprintf(buffer, sizeof(buffer) - 1, "%s", zSocket::getIPByIfName(i.ifname));
                    xmlParser.newNodeProp(snode, "addr", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.0f", i.delta);
                    xmlParser.newNodeProp(snode, "delta", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.0f", i.rx_speed);
                    xmlParser.newNodeProp(snode, "recv", buffer);
                    snprintf(buffer, sizeof(buffer) - 1, "%.0f", i.tx_speed);
                    xmlParser.newNodeProp(snode, "transmit", buffer);
                }
            }
        }

        xmlParser.dump(xml, true);
    }
}

/**
 * 获取各CPU的使用率
 *
 * @return CPU数量
 */
int zServerInfo::getCPUInfo()
{
    FILE *fp = fopen("/proc/stat", "r");

    if (fp) {
        getCPUInfoLine(fp, "cpu %llu %llu %llu %llu %llu %llu %llu\n", cpu.count);
        for (unsigned int i = 0; i < cpu.count; i++) {
            getCPUInfoLine(fp, "cpu%*d %llu %llu %llu %llu %llu %llu %llu\n", i);
        }

        fclose(fp);
    }

    return cpu.count;
}

/**
 * 获取CPU的使用率中的一行信息
 *
 * @param fp     文件指针
 * @param format 行格式
 * @param index  存储数据的索引
 */
void zServerInfo::getCPUInfoLine(FILE *fp, char const *format, unsigned int index)
{
    char buff[1024];

    if (fgets(buff, 128, fp) != nullptr) {
        CPUInfo::CPU::TickInfo &cp = cpu.usage[index].prev,
                               &cc = cpu.usage[index].current,
                               &cu = cpu.usage[index].usage;
        cp = cc;
        sscanf(buff, format, &cc.u, &cc.n, &cc.s, &cc.i, &cc.w, &cc.x, &cc.y);
        cu.calculate(cp, cc);
    }
}

/**
 * 获得各网卡流量
 *
 * @return 取得信息的网卡数量
 */
std::vector<struct IfInfo>::size_type zServerInfo::getIfInfo()
{
    char buff[1024];
    FILE *fp = fopen("/proc/net/dev", "r");

    if (fp) {
        // skip the header, two lines.
        if (nullptr == fgets(buff, 256, fp)) {
            return 0;
        }
        if (nullptr == fgets(buff, 256, fp)) {
            return 0;
        }

        std::vector<IfInfo>::size_type i = 0;

        while (true) {
            if (ifs.size() > i) {
                IfInfo &ifi = ifs[i];
                zTime now;

                ifi.delta = (float) ifi.current.ts.elapse(now);
                ifi.prev = ifi.current;
                ifi.current.ts = now;
                if (fgets(buff, 256, fp)) {
                    char *content = buff;
                    int len = 0;

                    while (*content == ' ') {
                        content++;
                    }
                    while (*content != ':' && len < (16 - 1)) {
                        ifi.ifname[len++] = *content++;
                    }
                    content++;
                    sscanf(content, "%llu, %llu, %*d, %*d, %*d, %*d, %*d, %*d, %llu, %llu",
                           &ifi.current.rx_bytes, &ifi.rx_packets, &ifi.current.tx_bytes, &ifi.tx_packets);

                    if (ifi.current.rx_bytes > ifi.prev.rx_bytes && ifi.delta > 0.0) {
                        ifi.rx_speed = (float) (ifi.current.rx_bytes - ifi.prev.rx_bytes) / ifi.delta;
                    } else {
                        ifi.rx_speed = 0.0;
                    }
                    ifi.rx_speed_max = fmaxf(ifi.rx_speed_max, ifi.rx_speed);

                    if (ifi.current.tx_bytes > ifi.prev.tx_bytes && ifi.delta > 0.0) {
                        ifi.tx_speed = (float) (ifi.current.tx_bytes - ifi.prev.tx_bytes) / ifi.delta;
                    } else {
                        ifi.tx_speed = 0.0;
                    }
                    ifi.tx_speed_max = fmaxf(ifi.tx_speed_max, ifi.tx_speed);
                } else {
                    break;
                }
            } else {
                IfInfo ifi;

                if (fgets(buff, 256, fp)) {
                    char *content = buff;
                    int len = 0;

                    while (*content == ' ') {
                        content++;
                    }
                    while (*content != ':' && len < (16 - 1)) {
                        ifi.ifname[len++] = *content++;
                    }
                    content++;
                    sscanf(content, "%llu, %llu, %*d, %*d, %*d, %*d, %*d, %*d, %llu, %llu",
                           &ifi.current.rx_bytes, &ifi.rx_packets, &ifi.current.tx_bytes, &ifi.tx_packets);
                    ifs.push_back(ifi);
                } else {
                    break;
                }
            }
            i++;
        }

        fclose(fp);
    }

    return ifs.size();
}