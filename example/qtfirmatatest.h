#ifndef QTFIRMATATEST_H
#define QTFIRMATATEST_H

#include <QObject>

class QTimer;
class QtFirmata;

class QtFirmataTest : public QObject
{
    Q_OBJECT
public:
    explicit QtFirmataTest(QObject *parent=0);

    QtFirmata *arduino;
    QTimer *activeTimer;
    int d, i, a, s, a_ud, s_ud;

public slots:
    void initialise();
    void start();
    void step();
    void printInputs();
};

#endif // QTFIRMATATEST_H
