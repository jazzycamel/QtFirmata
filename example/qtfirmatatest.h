#ifndef QTFIRMATATEST_H
#define QTFIRMATATEST_H

#include <QObject>

class QTimer;
class QtFirmata;

class QtFirmataTest : public QObject
{
    Q_OBJECT
public:
    explicit QtFirmataTest(QString port, QObject *parent=0);

public slots:
    void initialise();
    void start();
    void step();
    void printInputs();

private:
    QString port;
    QtFirmata *arduino;
    QTimer *activeTimer;
    QTimer *printTimer;
    int d, i, a, s, a_ud, s_ud;
};

#endif // QTFIRMATATEST_H
