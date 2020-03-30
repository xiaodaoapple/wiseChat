#include "wisechat.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WiseChat w;
    w.show();
    return a.exec();
}
