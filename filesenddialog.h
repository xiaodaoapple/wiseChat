#ifndef FILESENDDIALOG_H
#define FILESENDDIALOG_H

#include <QFile>
#include <QLabel>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>
class FileSendDialog :public QWidget
{
    Q_OBJECT
public:
    FileSendDialog(QWidget *parent=nullptr);
private:
    QProgressBar fileProgress;
    QLabel lbFileInfo;
    QPushButton btnOpen;
    QPushButton btnStart;
    QPushButton btnExit;

    QTcpServer *fileServer; //文件传输服务器
    QTcpSocket *clientSocket; // 连接上来的socket

    QString fileName;
    QString onlyFileName;
    QFile *locFile; //打开的文件指针
    qint64 totalBytes; //总共需要发送的字节数
    qint64 bytesWritten; // 已发送字节数
    qint64 bytesTobeWrite; //待发送字节数
    qint64 payloadSize; //初始化的一个常量
    QByteArray outBlock; //缓存一次发送的数据
public:
    QString ip;  //关联的聊天窗口的远端用户IP
signals:
    void udpFileName(QString);
public slots:
    void chooseFile();
    void sendFile();
    void closeAll();
    void fileRefused(QString); //对方拒绝接收文件
    void fileTransfer();
    void updateProgress(qint64 numBytes);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // FILESENDDIALOG_H
