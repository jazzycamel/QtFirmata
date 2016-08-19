#ifndef QTFIRMATA_H
#define QTFIRMATA_H

#include <QObject>
#include "qtfirmata_global.h"

class QSerialPort;

class QTFIRMATASHARED_EXPORT QtFirmata : public QObject
{
    Q_OBJECT

public:
    QtFirmata(QString serialPort="", int baud=57600);

    bool connect();
    void disconnect();
    bool available();

    void reportFirmware();
    void requestCapabilities();
    void reportPins();
    void requestInputs();

    void pinMode(int pin, int mode);
    int digitalRead(int pin);
    void digitalWrite(int pin, int value);
    int analogRead(int pin);
    void analogWrite(int pin, int value);
    void setServo(int pin, int value);
    void I2CRequest(int addr, QVector<int> data, int mode);
    void I2CConfig(int pinState, int delay);

    // Firmata Protocol Consants
    static const int MAX_DATA_BYTES                  = 32;

    static const int PINMODE_INPUT                   = 0;
    static const int PINMODE_OUTPUT                  = 1;
    static const int PINMODE_ANALOG                  = 2;
    static const int PINMODE_PWM                     = 3;
    static const int PINMODE_SERVO                   = 4;
    static const int PINMODE_SHIFT                   = 5;
    static const int PINMODE_I2C                     = 6;

    static const int DIGITAL_LOW                     = 0;
    static const int DIGITAL_HIGH                    = 1;

    static const int I2C_MODE_7_BITS                 = 0;
    static const int I2C_MODE_10_BITS                = 1;

    static const int I2C_MODE_WRITE                  = 0;
    static const int I2C_MODE_READ_ONCE              = 1;
    static const int I2C_MODE_READ_CONSTINOUSLY      = 2;
    static const int I2C_MODE_READ_STOP              = 3;

    static const int COMMAND_REPORT_VERSION          = 0xF9; // report firmware version
    static const int COMMAND_SYSTEM_RESET            = 0xFF; // reset from MIDI
    static const int COMMAND_SET_PIN_MODE            = 0xF4; // set a pin to INPUT/OUTPUT/PWM/etc
    static const int COMMAND_DIGITAL_MESSAGE         = 0x90; // send data for a digital port
    static const int COMMAND_ANALOG_MESSAGE          = 0xE0; // send data for an analog pin (or PWM)
    static const int COMMAND_REPORT_ANALOG           = 0xC0; // enable analog input by pin #
    static const int COMMAND_REPORT_DIGITAL          = 0xD0; // enable digital input by port
    static const int COMMAND_SERVO_CONFIG            = 0x70; // set the parameters for a servo pin (max and min pulse, max and min angle)

    static const int COMMAND_START_SYSEX             = 0xF0; // start a MIDI SysEx message
    static const int COMMAND_END_SYSEX               = 0xF7; // end a MIDI SysEx message

    static const int COMMAND_I2C_REQUEST             = 0x76;
    static const int COMMAND_I2C_REPLY               = 0x77;
    static const int COMMAND_I2C_CONFIG              = 0x78;
    static const int COMMAND_I2C_SAMPLING_INTERVAL   = 0x7A;

    static const int COMMAND_PIN_MODE_QUERY          = 0x72; // ask for current and supported pin modes
    static const int COMMAND_PIN_MODE_RESPONSE       = 0x73; // reply with current and supported pin modes
    static const int COMMAND_PIN_STATE_QUERY         = 0x6D;
    static const int COMMAND_PIN_STATE_RESPONSE      = 0x6E;
    static const int COMMAND_CAPABILITY_QUERY        = 0x6B;
    static const int COMMAND_CAPABILITY_RESPONSE     = 0x6C;
    static const int COMMAND_ANALOG_MAPPING_QUERY    = 0x69;
    static const int COMMAND_ANALOG_MAPPING_RESPONSE = 0x6A;
    static const int COMMAND_REPORT_FIRMWARE         = 0x79;

public slots:
    void initialise();
    void processSerial();
    void parseBuffer();

signals:
    void deviceReady();

private:
    int name;

    // Firmata protocol vars
    std::string firmataName;
    bool ready;
    int majorVersion;
    int minorVersion;

    // Inputs / outputs status
    int analogInputData[10];
    int digitalInputData[10];
    int digitalOutputData[10];

    // Serial connection vars
    bool connected;
    QString port;
    int baud_rate;
    QSerialPort* serial;
    uint8_t* parserBuffer;
    int parserCommandLength;
    int parserReceivedCount;
};

#endif // QTFIRMATA_H
