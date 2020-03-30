#include "filesenddialog.h"

#include <QHboxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include "global.h"
#include "handleinfo.h"
FileSendDialog::FileSendDialog(QWidget *parent):QWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout *hLayout=new QHBoxLayout;
    btnStart.setText(tr("开始"));
    btnOpen.setText(tr("选择文件"));
    btnExit.setText(tr("退出"));
    hLayout->addWidget(&btnStart);
    hLayout->addWidget(&btnOpen);
    hLayout->addWidget(&btnExit);
    QVBoxLayout *mainLayout=new QVBoxLayout;
    mainLayout->addWidget(&fileProgress);
    lbFileInfo.setText(tr("请选择文件进行传输"));
    mainLayout->addWidget(&lbFileInfo);
    mainLayout->addLayout(hLayout);
    this->setLayout(mainLayout);
    fileProgress.reset();
    connect(&btnStart,SIGNAL(clicked()),this,SLOT(sendFile()));
    connect(&btnOpen,SIGNAL(clicked()),this,SLOT(chooseFile()));
    connect(&btnExit,SIGNAL(clicked()),this,SLOT(closeAll()));
    btnStart.setEnabled(false);
    setFixedSize(300,107);

    //建立QTcpServer对象，用来传输文件
    fileServer=new QTcpServer(this);
    connect(fileServer,SIGNAL(newConnection()),this,SLOT(fileTransfer()));
    fileServer->close();

    connect(HandleInfo::getInstance(),SIGNAL(refuseFileHandle(QString)),this,SLOT(fileRefused(QString)));

    payloadSize=64*1024; //一次读取64k
}
void FileSendDialog::chooseFile()
{
    fileName=QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())
    {
        onlyFileName=fileName.right(fileName.size()-fileName.lastIndexOf('/')-1); //得到文件名称
        lbFileInfo.setText(tr("要传送的文件为:%1").arg(onlyFileName));
        btnStart.setEnabled(true);
    }
}

void FileSendDialog::sendFile()
{
    if(!fileServer->listen(QHostAddress::Any,TCP_PORT))
    {
        qDebug()<<fileServer->errorString()<<endl;
        close();
        return;
    }
    qDebug()<<"建立TCP文件接收服务器"<<endl;
    lbFileInfo.setText("等待对方接收");
    emit udpFileName(onlyFileName);

}

void FileSendDialog::closeAll()
{
    if(fileServer->isListening())
    {
        fileServer->close();
        if(locFile!=NULL)
        {
            if(locFile->isOpen())
            locFile->close();
        }
  //     clientSocket->abort();
    }
    close();
}

void FileSendDialog::fileRefused(QString ip)
{
    qDebug()<<"收到拒绝接收文件的ip"<<ip;
    //如果是拒绝接收我发来的文件,因为有可能打开多个聊天窗口用来发送文件，只要有一个窗口拒绝接收文件，那么这个窗口ip所有的窗口都会接收到
    //所以要进行判断
    if(ip==this->ip)
    {
        fileServer->close();
        lbFileInfo.setText("对方拒绝接收文件");
    }

}

void FileSendDialog::fileTransfer()
{
    qDebug()<<"开始文件传输"<<endl;
    btnStart.setEnabled(false);
    clientSocket=fileServer->nextPendingConnection();  //得到连接上的信息
    connect(clientSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(updateProgress(qint64)));
    lbFileInfo.setText(tr("开始传送文件%1").arg(fileName));
    locFile=new QFile(fileName);
    //打开文件失败
    if(!locFile->open(QFile::ReadOnly))
    {
        QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件%1:\n%2").arg(fileName).arg(locFile->errorString()));
        return;
    }
    totalBytes=locFile->size();
    QDataStream  sendOut(&outBlock,QIODevice::WriteOnly);
    QString curFile=fileName.right(fileName.size()-fileName.lastIndexOf('/')-1);
    sendOut<<qint64(0)<<qint64(0)<<curFile;  //预先定义文件头
    totalBytes+=outBlock.size();
    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64(outBlock.size()-sizeof(qint64) *2);
    bytesTobeWrite=totalBytes-clientSocket->write(outBlock);
    outBlock.resize(0);


}

void FileSendDialog::updateProgress(qint64 numBytes)
{
    bytesWritten+=(int)numBytes;
    if(bytesTobeWrite>0)
    {
        outBlock=locFile->read(qMin(bytesTobeWrite,payloadSize));
        bytesTobeWrite-=(int)clientSocket->write(outBlock);
        outBlock.resize(0);
    }else
    {
        locFile->close();
    }
    fileProgress.setMaximum(totalBytes);
    fileProgress.setValue(bytesWritten);
    lbFileInfo.setText(tr("已发送%1MB").arg(bytesWritten/(1024*1024)));
}

void FileSendDialog::closeEvent(QCloseEvent *)
{
    closeAll();
}
