#ifndef WISECHAT_H
#define WISECHAT_H

#include <QWidget>
#include <QToolButton>
#include <QVector>
#include <QUdpSocket>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTcpServer>
#include "global.h"
#include "chatdialog.h"
QT_BEGIN_NAMESPACE
namespace Ui { class WiseChat; }
QT_END_NAMESPACE
using namespace std;
class WiseChat : public QWidget
{
    Q_OBJECT

public:
    WiseChat(QWidget *parent = nullptr);
    ~WiseChat();
public:
    QVector<QToolButton *> friendVector; //存储好友图形列表
    QVector<ComputerInfo> friends; //存储好友列表
    QVector<ChatDialog *> chatVector;

private:
    Ui::WiseChat *ui;
    void initInfo();
    QUdpSocket udpReceive;  //接收消息的UDP
    QUdpSocket udpSendMsg;  //发送消息的UDP
    QGroupBox *group1;
    QVBoxLayout *layout;
signals:
    void infoToChatDialog(QString msg);
    void refuseFile(QString ip);  //拒绝接收ip的文件，将其通过单例模式传递给文件对话框
public slots:
    void udpDeal();  //处理udp报文
    /* type 消息类型
       msg  消息内容
       ip   接收ip*/
    void udpSend(int type,QString msg,QString recvIp);  //发送udp报文
    void chatTo();   //打开聊天对象
    void chatClose(QString ip); //关闭聊天窗口

    void dealClientMessage(QString);
    void infoShowInChat(QString);
    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};
#endif // WISECHAT_H
