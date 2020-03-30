#ifndef HANDLEINFO_H
#define HANDLEINFO_H
#include <QObject>
#include "global.h"
#include <QDebug>
class HandleInfo:public QObject
{
    Q_OBJECT
private:
    HandleInfo();
public:
    static HandleInfo *handle;
    static HandleInfo * getInstance();

    ComputerInfo getInfo();
signals:
    void refuseFileHandle(QString);


};

#endif // HANDLEINFO_H
