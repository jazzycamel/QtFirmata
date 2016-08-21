#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

#include "qtfirmata.h"
#include "qtfirmatatest.h"

QtFirmataTest::QtFirmataTest(QString port, QObject *parent)
    : QObject(parent), port(port)
{
    qDebug() << "Starting...";
    d=i=a=s=a_ud=s_ud=0;
}

void QtFirmataTest::initialise(){
    arduino=new QtFirmata(port);
    connect(arduino, SIGNAL(deviceReady()), this, SLOT(start()));

    qDebug("Connecting...");
    if(arduino->connect()) qDebug("Successfully connected!");
    else {
        qDebug("Error connecting!");
        qApp->quit();
    }
}

void QtFirmataTest::start(){
    activeTimer= new QTimer(this);
    activeTimer->setInterval(1000);
    activeTimer->setSingleShot(true);
    connect(activeTimer, SIGNAL(timeout()), this, SLOT(step()));

    printTimer= new QTimer(this);
    printTimer->setInterval(100);
    printTimer->setSingleShot(true);
    connect(printTimer, SIGNAL(timeout()), this, SLOT(printInputs()));

    qDebug("Initialising ports...");

    arduino->pinMode(13, QtFirmata::PINMODE_OUTPUT);

    arduino->pinMode(3, QtFirmata::PINMODE_PWM);
    arduino->pinMode(2, QtFirmata::PINMODE_SERVO);
    arduino->pinMode(8, QtFirmata::PINMODE_INPUT);
    arduino->pinMode(9, QtFirmata::PINMODE_INPUT);
    arduino->pinMode(14, QtFirmata::PINMODE_ANALOG);

//    arduino->pinMode(18, QtFirmata::PINMODE_I2C);
//    arduino->pinMode(19, QtFirmata::PINMODE_I2C);

//    QVector<int> readCmd(2,0);
//    arduino->I2CConfig(0,100);
//    arduino->I2CRequest(0x70, readCmd, QtFirmata::I2C_MODE_READ_ONCE);

    qDebug("Successfully Initialised!");
    qDebug("Starting loop...");

    activeTimer->start();
}

void QtFirmataTest::step(){
    if(!arduino->available()){
        qDebug("Device not ready, or device connection lost!");
        return;
    }

    i++;
    qDebug("");
    qDebug("----------------------------------------------------------");
    qDebug("");
    qDebug() << "Iteration: " << dec << i;

    d=i%2==0 ? QtFirmata::DIGITAL_HIGH : QtFirmata::DIGITAL_LOW;
    arduino->digitalWrite(13,d);

    if(a>=250) a_ud=1;
    else if (a<=0) a_ud=0;

    if (a_ud==0) a+=50;
    else a-=50;

    arduino->analogWrite(3,a);

    if(s>=180) s_ud=1;
    else if (s<=0) s_ud=0;

    if(s_ud==0) s+=20;
    else s-=20;

    arduino->setServo(2,s);

    qDebug() << "Digital Out (13): " << d << ", Analog Out (3): " << a << ", Servo Out (2): " << s;

    printTimer->start();
}

void QtFirmataTest::printInputs(){
    QString analog;
    for(int i=14; i<=19; i++)
        analog+=QString(" %1").arg(arduino->analogRead(i));

    qDebug() << "Digital In (8): " << arduino->digitalRead(8)
             << ", Analog In (14, 15, 16, 17, 18, 19): " << analog;

    activeTimer->start();
}
