#include <QCoreApplication>
#include "qtfirmatatest.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if(argc!=2){
        qDebug() << "Usage: QtFirmataTest <port>";
        return -1;
    }

    QtFirmataTest test(argv[1]);
    test.initialise();

    return a.exec();
}
