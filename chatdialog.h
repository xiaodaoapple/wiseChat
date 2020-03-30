#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QFontComboBox>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QTcpSocket>
#include <QUdpSocket>
class ChatDialog:public QWidget
{
    Q_OBJECT
public:
    ChatDialog(QString ip,QWidget *parent=nullptr);
public:
    QTextBrowser tbMsgBrowser;
    QTextEdit teMsgEdit;
    QFontComboBox fcbFontChoose;
    QComboBox cbSize;
    QPushButton btnSend;
    QPushButton btnExit;
    QToolButton tbBold;
    QToolButton tbItalic;
    QToolButton tbUnderline;
    QToolButton tbColor;
    QToolButton tbSend;


    bool isOpened=false; //窗口是否已经打开
    bool hasConnectServer=false; //是否连接到服务器
private:
   //QTcpSocket *tcpClient;
    QUdpSocket  udpSend;
    QColor color;
signals:
    void chatClose(QString);
    void sendMessage(int type,QString msg,QString revIp);
public slots:

    void sendMessage();  //发送消息
    void clientHasConnected();
    void changeFont(QFont);
    void changeSize(QString);
    void boldClicked(bool);
    void italicClicked(bool);
    void underClicked(bool);
    void colorClicked();
    void sendFileClicked();
    void sendFileInfo(QString);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
public:
    QString ip;
};

#endif // CHATDIALOG_H
