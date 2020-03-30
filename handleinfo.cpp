#include "handleinfo.h"
#include <QMutex>
#include <QHostInfo>
#include <QNetworkInterface>
HandleInfo *HandleInfo::handle=nullptr;
HandleInfo::HandleInfo()
{

}

HandleInfo *HandleInfo::getInstance()
{
    static QMutex mutex;
    if(!handle)
    {
        QMutexLocker locker(&mutex);
        if(!handle)
        {
            handle=new HandleInfo;

        }
    }
    return handle;
}

ComputerInfo HandleInfo::getInfo()
{
    ComputerInfo info;
    info.computerName=QHostInfo::localHostName();
    info.computerMac=QNetworkInterface::allInterfaces().at(0).hardwareAddress();
   // info.computerIp=QNetworkInterface::allInterfaces().at(0).addressEntries().at(1).ip().toString();

    QHostInfo info1 = QHostInfo::fromName(QHostInfo::localHostName());
    foreach(QHostAddress address,info1.addresses())
        {
           if(address.protocol() == QAbstractSocket::IPv4Protocol)
           {
               info.computerIp=address.toString();
               break;
           }

         }
    return info;
}
