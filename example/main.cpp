#include <QCoreApplication>
#include "qtfirmatatest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QtFirmataTest test;
    test.initialise();

    return a.exec();
}
