#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>

class ComputerInfo {
public:
    QString computerName;
    QString computerIp;
    QString computerMac;
    friend bool operator ==(const ComputerInfo &info1 ,const ComputerInfo &info2){

        if((info1.computerName==info2.computerName) && (info1.computerMac==info2.computerMac) && (info1.computerIp==info2.computerIp))
            return true;
        return false;
    }

};

#define SENDLOCAL 0  //发送本机信息UDP报文
#define SENDCLOSE 1  //退出时发送本机信息报文
#define UDP_PORT 5555 //udp报文 端口号
#define TCP_PORT 6666 //tcp端口接听

/*  以下为UDP报文头设计 用户上线，用户退出，用户在线，发出消息，收到消息 */
#define WC_USER_ENTRY 1100
// WC_USER_ENTRY#计算机名#计算机MAC#计算机IP

#define WC_USER_EXIT 1101
//WC_USER_EXIT#计算名#计算机MAC#计算机IP
#define WC_USER_ONLINE 1102
//WC_USER_ONLINE#计算名#计算机MAC#计算机IP
#define WC_SENDMSG 1103
//WC_SENDMSG#发消息的IP#消息时间#消息内容
#define WC_RECVMSG 1104

//WC_SENDFILE#发消息的IP#文件名称
#define WC_SENDFILE 1105
//WC_REFUSE#发消息的IP     拒绝接收文件
#define WC_REFUSE 1106
//用户在线
//
#endif // GLOBAL_H
