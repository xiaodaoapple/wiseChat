#include "filerecvdialog.h"
#include "global.h"
#include <QHboxLayout>
#include <QMessageBox>
FileRecvDialog::FileRecvDialog(QString fileName,QString serverIp,QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->fileName=fileName;
    this->serverIp=serverIp;
    lbShowProgress.setText(tr("已完成:"));
    lbShowInfo.setText(tr("等待接收文件..."));
    btnClose.setText(tr("关闭"));
    btnCancel.setText(tr("取消"));
    recvProgress.setValue(0);
   //   lbFileInfo.setText(tr("要传送的文件为:%1").arg(onlyFileName));
    qDebug()<<serverIp<<" "<<fileName<<endl;
    setWindowTitle(tr("接收来自%1的文件%2").arg(serverIp).arg(fileName));
    QHBoxLayout *hLayout=new QHBoxLayout;

    hLayout->addWidget(&lbShowProgress);
    hLayout->addWidget(&recvProgress);
    QHBoxLayout *bottomLayout=new QHBoxLayout;
    bottomLayout->addWidget(&btnCancel);
    bottomLayout->addWidget(&btnClose);
    QVBoxLayout *mainLayout=new QVBoxLayout;
    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(&lbShowInfo);
    mainLayout->addLayout(bottomLayout);
    this->setLayout(mainLayout);
    setFixedSize(300,107);

    totalBytes=0;
    bytesRecved=0;
    fileNameSize=0;
    connect(&btnCancel,SIGNAL(clicked()),this,SLOT(cancelTransfer()));
    connect(&btnClose,SIGNAL(clicked()),this,SLOT(closeTransfer()));
    connect(&tcpRecv,SIGNAL(readyRead()),this,SLOT(readMsg()));
    connect(&tcpRecv,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayErr(QAbstractSocket::SocketError)));
    tcpRecv.connectToHost(serverIp,TCP_PORT);
}
void FileRecvDialog::readMsg()
{
    QDataStream in(&tcpRecv);
    //第一次读取数据
    if(bytesRecved<=sizeof(qint64)*2)
    {
        if((tcpRecv.bytesAvailable()>=sizeof(qint64)*2) && (fileNameSize==0))
        {
            in>>totalBytes>>fileNameSize;
            bytesRecved+=sizeof(qint64)*2;
        }
        if((tcpRecv.bytesAvailable()>=fileNameSize) && (fileNameSize!=0))
        {
            in>>transfeFileName;
            bytesRecved+=fileNameSize;
            locFile=new QFile(fileName);
            if(!locFile->open(QFile::WriteOnly))
            {
                QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件%1:\n%2.").arg(fileName).arg(locFile->errorString()));
                return;
            }
        }else
        {
            return;
        }
    }
    if(bytesRecved<totalBytes)
    {
        bytesRecved+=tcpRecv.bytesAvailable();
        inBlock=tcpRecv.readAll();
        locFile->write(inBlock);
        inBlock.resize(0);
    }
    recvProgress.setMaximum(totalBytes);
    recvProgress.setValue(bytesRecved);
    lbShowInfo.setText(tr("已接收%1MB").arg(bytesRecved/(1024*1024)));
    if(bytesRecved==totalBytes)
    {
        locFile->close();
        tcpRecv.close();
        lbShowInfo.setText(tr("接收文件%1完毕").arg(fileName));
        btnCancel.setEnabled(false);
    }
}

void FileRecvDialog::displayErr(QAbstractSocket::SocketError sockErr)
{
    switch (sockErr) {
    case QAbstractSocket::RemoteHostClosedError: break;
    default:qDebug()<<tcpRecv.errorString();

    }
}

void FileRecvDialog::cancelTransfer()
{
    tcpRecv.abort();
    if(locFile->isOpen())
        locFile->close();
}

void FileRecvDialog::closeTransfer()
{
    tcpRecv.abort();
    if(locFile->isOpen())
        locFile->close();
    close();
}
