#include "wisechat.h"
#include "ui_wisechat.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QGroupBox>
#include <QtAlgorithms>
#include "handleinfo.h"
#include "global.h"
#include "chatdialog.h"
#include "filerecvdialog.h"
#include <algorithm>
WiseChat::WiseChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WiseChat)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("WiseChat 1.0"));
    this->setWindowIcon(QPixmap(":/head/images/qq.png"));

    //删除自带的2个page
    this->ui->tbShow->setItemText(0,"");
    this->ui->tbShow->removeItem(0);
    this->ui->tbShow->setItemText(0,"");
    this->ui->tbShow->removeItem(0);

    //add myself
    QToolButton *tbTemp=new QToolButton;
    tbTemp->setObjectName("127.0.0.1");
    tbTemp->setText(tr("我自己\n\n")+HandleInfo::handle->getInfo().computerIp);
    tbTemp->setIcon(QPixmap(":/head/images/spqy.png"));
    tbTemp->setIconSize(QPixmap(":/head/images/spqy.png").size());
    tbTemp->setAutoRaise(true);
    tbTemp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(tbTemp,SIGNAL(clicked()),this,SLOT(chatTo()));

    friendVector.append(tbTemp);

    group1=new QGroupBox;
    layout=new QVBoxLayout(group1);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(tbTemp);

    setFixedSize(230,800);
    this->ui->tbShow->addItem((QWidget *)group1,"我的好友");

    initInfo();

}
void WiseChat::initInfo()
{

    ComputerInfo temp=HandleInfo::getInstance()->getInfo();
    friends.append(temp);
    //监听udp信息
    udpReceive.bind(UDP_PORT);
    this->connect(&udpReceive,SIGNAL(readyRead()),this,SLOT(udpDeal()));


    //发送本机信息上线通知
    QString s;
    s=HandleInfo::getInstance()->getInfo().computerName+"#"+HandleInfo::getInstance()->getInfo().computerMac+"#"+HandleInfo::getInstance()->getInfo().computerIp;
    udpSend(WC_USER_ENTRY,s,"");  //启动时候将自己信息广播出去

    connect(this,SIGNAL(refuseFile(QString)),HandleInfo::getInstance(),SIGNAL(refuseFileHandle(QString)));
    //客户端发来的消息都用槽函数进行处理

}
void WiseChat::dealClientMessage(QString msg)
{
   qDebug()<<"主窗口收到消息";
   emit infoToChatDialog(msg);
   qDebug()<<msg;
}
void WiseChat::udpSend(int flag,QString msg,QString recvIp)
{

    QString s;
    s=QString::number(flag)+"#"+msg;
    if(recvIp=="")
    {
        qDebug()<<"发出UDP消息"<<s;
        udpSendMsg.writeDatagram(s.toUtf8(),QHostAddress::Broadcast,UDP_PORT);
    }
    else
    {
        qDebug()<<"发出UDP消息"<<s;
        udpSendMsg.writeDatagram(s.toUtf8(),QHostAddress(recvIp),UDP_PORT);
    }

}

/* 点击头像图标，打开聊天窗口 */
void WiseChat::chatTo()
{
    QString mName=sender()->objectName();

    //判断聊天窗口里是否有这个ip，如果有，则退出
    for(int i=0;i<chatVector.count();++i)
    {
        ChatDialog *temp=chatVector.at(i);
        if(temp->ip==mName)
            return;
    }
    ChatDialog* cd=new ChatDialog(mName);
    connect(cd,SIGNAL(chatClose(QString)),this,SLOT(chatClose(QString)));
    connect(cd,SIGNAL(sendMessage(int,QString,QString)),this,SLOT(udpSend(int,QString,QString)));
    chatVector.append(cd);
    cd->show();
}
/*  处理收到的信息，如果接收信息的聊天窗口已经打开，则传递过去，如果没有打开，则打开对应的聊天窗口，并显示信息 */
void WiseChat::infoShowInChat(QString msg)
{
    //  QString chatIp=msg.left(msg.indexOf('#'),msg.indexOf('#',msg.indexOf('#'))); //得到发过来消息的用户ip,在第二个#
      QStringList info=msg.split('#');
      QString chatIp=info.at(1);
      QString s_show=info.at(1)+" "+info.at(2)+"\n"+info.at(3)+"\n";
      //判断已经打开的聊天窗口里是否有这个ip，如果有，则退出
      for(int i=0;i<chatVector.count();++i)
      {
          ChatDialog *temp=chatVector.at(i);
          if(temp->ip==chatIp)
          {
              temp->tbMsgBrowser.append(s_show);
              return;
          }

      }
      ChatDialog* cd=new ChatDialog(chatIp);
      connect(cd,SIGNAL(chatClose(QString)),this,SLOT(chatClose(QString)));
      connect(cd,SIGNAL(sendMessage(int,QString,QString)),this,SLOT(udpSend(int,QString,QString)));
      chatVector.append(cd);
      cd->tbMsgBrowser.append(s_show);
      cd->show();
}
void WiseChat::chatClose(QString ip)
{
   // 如果数组里存储了聊天窗口，就将其删除掉
    for(int i=0;i<chatVector.count();++i)
    {
        ChatDialog *temp=chatVector.at(i);
        if(temp->ip==ip)
        {
            chatVector.removeAt(i);
            delete  temp;
        }
    }
   // qDebug()<<ip;
}

void WiseChat::closeEvent(QCloseEvent *)
{
    //关闭所有已经打开的窗口
    for(int i=0;i<chatVector.count();++i)
    {
        ChatDialog *temp=chatVector.at(i);
        if(temp->ip!="")
        {
            temp->close();
        }

    }
    QString s;
    s=HandleInfo::getInstance()->getInfo().computerName+"#"+HandleInfo::getInstance()->getInfo().computerMac+"#"+HandleInfo::getInstance()->getInfo().computerIp;
    //发送用户下线
    udpSend(WC_USER_EXIT,s,"");

}
void WiseChat::udpDeal()
{


    while(udpReceive.hasPendingDatagrams())
    {
        //QString msg=QString::fromUtf8(udpReceive.readAll()); //不能这样接收消息，查看QUdpSocket的帮助


        QByteArray datagram;
        datagram.resize(udpReceive.pendingDatagramSize());
        udpReceive.readDatagram(datagram.data(),datagram.size());

        QString msg=datagram.data();
        qDebug()<<"收到UDP消息"<<msg<<endl;

        QStringList s=msg.split('#');
        int flag=s.at(0).toInt();
        ComputerInfo infoTemp;
        if(flag==WC_USER_ENTRY || flag==WC_USER_ONLINE) //新用户上线,或者收到上线通知的用户的反馈消息
        {
            //如果收到的是自己的上线或者在线的信息，则忽略掉
            QString s_ip=s.at(3);
            if(s_ip==HandleInfo::getInstance()->getInfo().computerIp)
                return;
            if(flag==WC_USER_ENTRY) //收到的是新用户上线的通知
            {
                //给新用户反馈我在线
                QString msg;
                msg=HandleInfo::getInstance()->getInfo().computerName+"#"+HandleInfo::getInstance()->getInfo().computerMac+"#"+HandleInfo::getInstance()->getInfo().computerIp;
                udpSend(WC_USER_ONLINE,msg,s.at(3));

            }
            infoTemp.computerName=s.at(1);
            infoTemp.computerMac=s.at(2);
            infoTemp.computerIp=s.at(3);
            if(!friends.contains(infoTemp)) //假如好友列表没有这个好友
            {
                friends.append(infoTemp); //添加到好友列表

                QToolButton *tbTemp=new QToolButton;
                tbTemp->setObjectName(infoTemp.computerIp);
                tbTemp->setText(infoTemp.computerName+"\n\n"+infoTemp.computerIp);
                tbTemp->setIcon(QPixmap(":/head/images/qq.png"));
                tbTemp->setIconSize(QPixmap(":/head/images/qq.png").size());
                tbTemp->setAutoRaise(true);
                tbTemp->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
                connect(tbTemp,SIGNAL(clicked()),this,SLOT(chatTo()));
                layout->addWidget(tbTemp);
                friendVector.append(tbTemp);

            }
        }

       if(flag==WC_USER_EXIT) //用户下线
        {
           //如果收到的是自己的下线的信息，则忽略掉
           QString s_ip=s.at(3);
           if(s_ip==HandleInfo::getInstance()->getInfo().computerIp)
               return;
           infoTemp.computerName=s.at(1);
           infoTemp.computerMac=s.at(2);
           infoTemp.computerIp=s.at(3);
           QVector<ComputerInfo>::iterator i_find= qFind(friends.begin(),friends.end(),infoTemp);
           int i=distance(friends.begin(),i_find);
           friends.removeOne(infoTemp);
           QToolButton *temp=friendVector.at(i);
           layout->removeWidget(temp); //删除用户图标
           friendVector.removeOne(temp);
           delete temp;
           qDebug()<<"有好友下线";
        }
       if(flag==WC_SENDMSG) //收到用户发来的消息,并进行处理
       {
           infoShowInChat(msg);
       }
       if(flag==WC_SENDFILE) //收到用户发来传输文件的请求
       {
            qDebug()<<"收到传输文件请求"<<endl;
            int btn=QMessageBox::information(this,"接收文件",tr("接收来自%1的文件%2？").arg(s.at(1)).arg(s.at(2)),QMessageBox::Yes,QMessageBox::No);
            if(btn==QMessageBox::Yes)
            {
                QString name=QFileDialog::getSaveFileName(0,tr("保存文件"),s.at(2));
                //打开文件接收窗口
                FileRecvDialog *frd=new FileRecvDialog(name,s.at(1));
                frd->show();
            }else //拒绝接收文件
            {
                udpSend(WC_REFUSE,HandleInfo::getInstance()->getInfo().computerIp,s.at(1));
            }
       }
       if(flag==WC_REFUSE)
       {
           emit refuseFile(s.at(1));
       }

    }
}
WiseChat::~WiseChat()
{

    while(friendVector.count())
    {
        QToolButton *temp=friendVector.back();
        friendVector.removeLast();
        delete temp;
    }

    while(chatVector.count())
    {
        ChatDialog *temp=chatVector.back();
        chatVector.removeLast();
        delete temp;
    }

  //  delete tcpServer;

    delete layout;
    delete group1;
    delete ui;
    delete HandleInfo::getInstance();

}


