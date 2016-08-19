#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include "qtfirmata.h"
//#include "utils.h"

QtFirmata::QtFirmata(QString serialPort, int baud) :
    baud_rate(baud)
{
    parserBuffer=(uint8_t*) malloc(4096);
    connected=false;

    if(serialPort==""){
        QList<QSerialPortInfo> ports=QSerialPortInfo::availablePorts();
        serialPort=ports.value(0).portName();
        qDebug() << "No serial port specified, defaulting to first available: " << serialPort;
    }

    serial=new QSerialPort();
    serial->setBaudRate(baud_rate);
    serial->setPortName(serialPort);

    QObject::connect(serial, SIGNAL(readyRead()), this, SLOT(processSerial()));
    QObject::connect(this, SIGNAL(deviceReady()), this, SLOT(initialise()));
}

bool QtFirmata::connect(){
    connected=serial->open(QIODevice::ReadWrite);
    if(connected) reportFirmware();
    return connected;
}

void QtFirmata::disconnect(){
    serial->close();
    connected=false;
    ready=false;
}

bool QtFirmata::available(){ return connected && ready; }

void QtFirmata::initialise(){
    requestCapabilities();
    reportPins();
}

void QtFirmata::reportFirmware(){
    char buffer[]={
        COMMAND_START_SYSEX,
        COMMAND_REPORT_FIRMWARE,
        COMMAND_END_SYSEX
    };
    QByteArray s(buffer,3);
    serial->write(s);
    serial->flush();
}

void QtFirmata::requestCapabilities(){
    char buffer[]={
        COMMAND_START_SYSEX,
        COMMAND_ANALOG_MAPPING_QUERY,
        COMMAND_END_SYSEX,
        COMMAND_START_SYSEX,
        COMMAND_CAPABILITY_QUERY,
        COMMAND_END_SYSEX
    };
    QByteArray s(buffer,6);
    serial->write(s);
    serial->flush();
}

void QtFirmata::reportPins(){
    for(int i=0; i<16; i++){
        char buffer[]={ COMMAND_REPORT_ANALOG|i, 1 };
        QByteArray s(buffer,2);
        serial->write(s);
        serial->flush();
    }

    for(int i=0; i<2; i++){
        char buffer[]={ COMMAND_REPORT_DIGITAL|i, 1 };
        QByteArray s(buffer,2);
        serial->write(s);
        serial->flush();
    }
}

void QtFirmata::requestInputs(){
    for(int pin=0; pin<128; pin++){
        char buffer[]={
            COMMAND_START_SYSEX,
            COMMAND_PIN_STATE_QUERY,
            pin,
            COMMAND_END_SYSEX
        };

        QByteArray s(buffer,4);
        serial->write(s);
        serial->flush();
    }
}

void QtFirmata::pinMode(int pin, int mode){
    char buffer[]={
        COMMAND_SET_PIN_MODE,
        pin,
        mode
    };
    QByteArray s(buffer,3);
    serial->write(s);
    serial->flush();
}

int QtFirmata::digitalRead(int pin){ return digitalInputData[pin]; }

void QtFirmata::digitalWrite(int pin, int value){
    int portNumber=(pin>>3)&0x0F;
    if(value==0) digitalOutputData[portNumber]&=~(1<<(pin&0x07));
    else digitalOutputData[portNumber]|=(1<<(pin&0x07));

    char buffer[]={
        COMMAND_DIGITAL_MESSAGE|portNumber,
        digitalOutputData[portNumber]&0x7F,
        digitalOutputData[portNumber]>>7
    };
    QByteArray s(buffer,3);
    serial->write(s);
    serial->flush();
}

int QtFirmata::analogRead(int pin){ return analogInputData[pin-14]; }

void QtFirmata::analogWrite(int pin, int value){
    char buffer[]={
        COMMAND_ANALOG_MESSAGE|(pin&0x07),
        value&0x7F,
        value>>7
    };
    QByteArray s(buffer,3);
    serial->write(s);
    serial->flush();
}

void QtFirmata::setServo(int pin, int value){ analogWrite(pin,value); }

void QtFirmata::I2CRequest(int addr, QVector<int> data, int mode){
    int addr_mode=0x20, pos=0;
    char buffer[5+data.length()];

    buffer[pos++]=COMMAND_START_SYSEX;
    buffer[pos++]=COMMAND_I2C_REQUEST;
    buffer[pos++]=addr&0xFF;
    buffer[pos]=(addr&0x380)>>7;
    buffer[pos]|=addr_mode;
    buffer[pos]|=(mode<<2);
    pos++;

    for(int i=0; i<data.length(); i+=2){
        buffer[pos++]=data[i]&0x0F;
        buffer[pos++]=(data[i+1]&0xF0)>>8;
    }

    buffer[pos++]=COMMAND_END_SYSEX;

    QByteArray s(buffer,pos);
    serial->write(s);
    serial->flush();
}

void QtFirmata::I2CConfig(int pinState, int delay){
    char buffer[]={
        COMMAND_START_SYSEX,
        COMMAND_I2C_CONFIG,
        pinState,
        delay&0xFF,
        delay>>7,
        COMMAND_END_SYSEX
    };
    QByteArray s(buffer,6);
    serial->write(s);
    serial->flush();
}

void QtFirmata::processSerial(){
    QByteArray r=serial->readAll();

    for(int i=0; i<r.length(); i++){
        uint8_t c=r[i];
        uint8_t msn=c&0xF0;

        if(msn==COMMAND_ANALOG_MESSAGE || msn==COMMAND_DIGITAL_MESSAGE || c==COMMAND_REPORT_VERSION){
            parserCommandLength=3;
            parserReceivedCount=0;
        } else if (msn==COMMAND_REPORT_ANALOG || msn==COMMAND_REPORT_DIGITAL){
            parserCommandLength=2;
            parserReceivedCount=0;
        } else if (c==COMMAND_START_SYSEX){
            parserCommandLength=4096;
            parserReceivedCount=0;
        } else if (c==COMMAND_END_SYSEX){
            parserCommandLength=parserReceivedCount+1;
        } else if (c&0x80){
            parserCommandLength=1;
            parserReceivedCount=0;
        }

        if(parserReceivedCount<=parserCommandLength){
            parserBuffer[parserReceivedCount]=c;
            parserReceivedCount++;

            if(parserReceivedCount==parserCommandLength){
                parseBuffer();
                parserReceivedCount=parserCommandLength=0;
            }
        } else if (parserReceivedCount>=parserCommandLength-1){
            parserReceivedCount=parserCommandLength=0;
        }
    }
}

void QtFirmata::parseBuffer(){
    uint8_t cmd=(parserBuffer[0]&0xF0);

    if(cmd==COMMAND_ANALOG_MESSAGE && parserReceivedCount==3){
        int analog_ch=(parserBuffer[0]&0x0F);
        int analog_val=parserBuffer[1]|(parserBuffer[2]<<7);
        analogInputData[analog_ch]=analog_val;
    } else if(cmd==COMMAND_DIGITAL_MESSAGE && parserReceivedCount==3){
        int port_num=(parserBuffer[0]&0x0F);
        int port_val=parserBuffer[1]|(parserBuffer[2]<<7);
        int pin=port_num*8;

        for(int mask=1; mask&0xFF; mask<<=1, pin++)
            digitalInputData[pin]=(uint32_t)(port_val&mask)?1:0;
    } else if(parserBuffer[0]==COMMAND_START_SYSEX && parserBuffer[parserReceivedCount-1]==COMMAND_END_SYSEX){
        qDebug() << "SYSEX Command";

        if(parserBuffer[1]==COMMAND_REPORT_FIRMWARE){
            char name[140];
            int len=0;
            for(int i=4; i<parserReceivedCount-2; i+=2)
                name[len++]=(parserBuffer[i]&0x7F)|((parserBuffer[i+1]&0x7F)<<7);

            name[len++]='-';
            name[len++]=parserBuffer[2]+'0';
            name[len++]='.';
            name[len++]=parserBuffer[3]+'0';
            name[len++]=0;
            firmataName=name;
            qDebug() << "Firmata version:" << QString::fromStdString(firmataName);

            ready=true;
            emit deviceReady();
        } else if(parserBuffer[1]==COMMAND_CAPABILITY_RESPONSE){
        } else if(parserBuffer[1]==COMMAND_ANALOG_MAPPING_RESPONSE){
        } else if(parserBuffer[1]==COMMAND_PIN_STATE_RESPONSE && parserReceivedCount>=6){
        } else qDebug() << "SYSEX command not recognised!:" << hex << parserBuffer[1];
    }
}
