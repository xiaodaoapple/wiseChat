#include "chatdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QColorDialog>
#include "global.h"
#include "handleinfo.h"
#include <QDebug>
#include "filesenddialog.h"
ChatDialog::ChatDialog(QString ip,QWidget *parent):QWidget(parent)
{
    this->setWindowTitle(ip);
    this->ip=ip;
    isOpened=true;
    for(int i=8;i<+50;++i)
        cbSize.addItem(QString::number(i));
    tbBold.setIcon(QPixmap(":/operate/images/bold.png"));
    tbBold.setCheckable(true);
    tbBold.setToolTip("加粗");
    tbItalic.setIcon(QPixmap(":/operate/images/italic.png"));
    tbItalic.setCheckable(true);
    tbItalic.setToolTip("倾斜");
    tbUnderline.setIcon(QPixmap(":/operate/images/under.png"));
    tbUnderline.setCheckable(true);
    tbUnderline.setToolTip("下划线");
    tbColor.setIcon(QPixmap(":/operate/images/color.png"));
    tbColor.setToolTip("更改字体颜色");
    tbSend.setIcon(QPixmap(":/operate/images/send.png"));
    tbSend.setToolTip("传输文件");

    cbSize.setCurrentIndex(4);
    btnSend.setText(tr("发送"));
    btnExit.setText(tr("退出"));
    QHBoxLayout *hLayout=new QHBoxLayout;

    hLayout->addWidget(&fcbFontChoose);
    hLayout->addWidget(&cbSize);
    hLayout->addWidget(&tbBold);
    hLayout->addWidget(&tbItalic);
    hLayout->addWidget(&tbUnderline);
    hLayout->addWidget(&tbColor);
    hLayout->addWidget(&tbSend);


    QHBoxLayout *bottomLayout=new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->addWidget(&btnSend);
    bottomLayout->addStretch();
    bottomLayout->addWidget(&btnExit);
    bottomLayout->addStretch();

    QVBoxLayout *mainLayout=new QVBoxLayout();
    mainLayout->addWidget(&tbMsgBrowser);
    mainLayout->addLayout(hLayout);
    mainLayout->addWidget(&teMsgEdit);
    mainLayout->addLayout(bottomLayout);

    mainLayout->setStretchFactor(&tbMsgBrowser,7);
    mainLayout->setStretchFactor(hLayout,1);
    mainLayout->setStretchFactor(&teMsgEdit,2);
    mainLayout->setStretchFactor(bottomLayout,1);
    this->setLayout(mainLayout);
    setFixedSize(400,400);
    connect(&btnSend,SIGNAL(clicked()),this,SLOT(sendMessage()));
    connect(&btnExit,SIGNAL(clicked()),this,SLOT(close()));

    /*
    tcpClient=new QTcpSocket(this);
  //  tcpClient=new TcpChatClient(this);
    connect(tcpClient,SIGNAL(connected()),this,SLOT(clientHasConnected()));
    tcpClient->connectToHost(ip,TCP_PORT);
    */
    connect(&fcbFontChoose,SIGNAL(currentFontChanged(QFont)),this,SLOT(changeFont(QFont)));
    connect(&cbSize,SIGNAL(currentIndexChanged(QString)),this,SLOT(changeSize(QString)));
    connect(&tbBold,SIGNAL(clicked(bool)),this,SLOT(boldClicked(bool)));
    connect(&tbItalic,SIGNAL(clicked(bool)),this,SLOT(italicClicked(bool)));
    connect(&tbUnderline,SIGNAL(clicked(bool)),this,SLOT(underClicked(bool)));
    connect(&tbColor,SIGNAL(clicked()),this,SLOT(colorClicked()));
    connect(&tbSend,SIGNAL(clicked()),this,SLOT(sendFileClicked()));
}

void ChatDialog::sendMessage()
{

    if(teMsgEdit.toPlainText()=="")
    {
        QMessageBox::warning(this,"警告"," 不能发空消息","确定");
        return;
    }
    QString msg;
    msg=HandleInfo::handle->getInfo().computerIp;
    msg+="#"+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"#";
    msg+=teMsgEdit.toPlainText();
  //  msg+=teMsgEdit.toHtml();
  //  qDebug()<<teMsgEdit.toHtml();
   /*
    int length=0;
    tcpClient->write(msg.toUtf8());
    */
    emit sendMessage(WC_SENDMSG,msg,ip);
   /*
    this->tbMsgBrowser.append(tr("自己 ")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+tr("\n"));
    this->tbMsgBrowser.seth
    this->tbMsgBrowser.append(this->tbMsgBrowser.setHtml(teMsgEdit.toHtml()));
    */
    this->tbMsgBrowser.append(tr("自己 ")+QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")+"\n"+teMsgEdit.toPlainText());
    this->tbMsgBrowser.append(("\n"));
    /*
    if((length=tcpClient->write(msg.toUtf8(),msg.length()))!=msg.length())
    {
        QMessageBox::warning(this,"警告"," 消息发送失败","确定");
        return;
    }
    */
    //tcpClient.write(msg.toUtf8(),msg.length());

    teMsgEdit.setText("");
}

void ChatDialog::clientHasConnected()
{

    qDebug()<<"连接server成功"<<endl;
    hasConnectServer=true;
}

void ChatDialog::changeFont(QFont f)
{
    this->teMsgEdit.setCurrentFont(f);
    this->teMsgEdit.setFocus();
}

void ChatDialog::changeSize(QString s)
{
    this->teMsgEdit.setFontPointSize(s.toDouble());
    this->teMsgEdit.setFocus();
}

void ChatDialog::boldClicked(bool checked)
{
    if(checked)
        this->teMsgEdit.setFontWeight(QFont::Bold);
    else
        this->teMsgEdit.setFontWeight(QFont::Normal);
}

void ChatDialog::italicClicked(bool checked)
{
    if(checked)
        this->teMsgEdit.setFontItalic(checked);
    this->teMsgEdit.setFocus();
}

void ChatDialog::underClicked(bool checked)
{
    this->teMsgEdit.setFontUnderline(checked);
    this->teMsgEdit.setFocus();
}

void ChatDialog::colorClicked()
{
    color=QColorDialog::getColor(color,this);
    if(color.isValid())
    {
        this->teMsgEdit.setTextColor(color);
        this->teMsgEdit.setFocus();
    }
}

void ChatDialog::sendFileClicked()
{
    FileSendDialog *fs=new FileSendDialog();
    //将发送文件窗体的udp文件广播绑定到发送文件信息槽函数
    connect(fs,SIGNAL(udpFileName(QString)),this,SLOT(sendFileInfo(QString)));
    fs->ip=ip;
    fs->show();

}

void ChatDialog::sendFileInfo(QString fileInfo)
{
    QString msg;
    msg=HandleInfo::handle->getInfo().computerIp+"#"+fileInfo;
    emit sendMessage(WC_SENDFILE,msg,ip);
}

void ChatDialog::closeEvent(QCloseEvent *event)
{
 //   tcpClient->disconnectFromHost();

    //发送关闭聊天窗口的信号
    emit chatClose(ip);

}
