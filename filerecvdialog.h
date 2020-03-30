#ifndef FILERECVDIALOG_H
#define FILERECVDIALOG_H

#include <QFile>
#include <QLabel>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>
class FileRecvDialog : public QWidget
{
    Q_OBJECT
public:
    explicit FileRecvDialog(QString fileName,QString serverIp,QWidget *parent = nullptr);

public:
    QString fileName; //所选择的保存文件的路径和文件名称
    QString transfeFileName; //传输过来的 文件名称
    QString serverIp;
private:
    QLabel lbShowProgress;
    QProgressBar recvProgress;
    QLabel lbShowInfo;
    QPushButton btnCancel;
    QPushButton btnClose;
    QTcpSocket tcpRecv;

    qint64 totalBytes;   //总共需要接收的字节数
    qint64 bytesRecved;  //已经接收的字节数
    qint64 fileNameSize;
    QByteArray inBlock;  //缓存一次接收的字节数
    QFile *locFile;

    QTimer time;
private slots:
    void readMsg();
    void displayErr(QAbstractSocket::SocketError sockErr);
    void cancelTransfer();
    void closeTransfer();
signals:

};

#endif // FILERECVDIALOG_H
