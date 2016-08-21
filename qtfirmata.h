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

    void pinMode(uint8_t pin, uint8_t mode);
    int digitalRead(uint8_t pin);
    void digitalWrite(uint8_t pin, uint8_t value);
    int analogRead(uint8_t pin);
    void analogWrite(uint8_t pin, int value);
    void setServo(uint8_t pin, int value);
    void I2CRequest(int addr, QVector<int> data, int mode);
    void I2CConfig(int pinState, int delay);

    // Firmata Protocol Consants
    enum {
        MAX_DATA_BYTES                  = 32
    };

    enum{
        PINMODE_INPUT                   = 0,
        PINMODE_OUTPUT                  = 1,
        PINMODE_ANALOG                  = 2,
        PINMODE_PWM                     = 3,
        PINMODE_SERVO                   = 4,
        PINMODE_SHIFT                   = 5,
        PINMODE_I2C                     = 6,
    };

    enum {
        DIGITAL_LOW                     = 0,
        DIGITAL_HIGH                    = 1,
    };

    enum {
        I2C_MODE_7_BITS                 = 0,
        I2C_MODE_10_BITS                = 1,
    };

    enum {
        I2C_MODE_WRITE                  = 0,
        I2C_MODE_READ_ONCE              = 1,
        I2C_MODE_READ_CONSTINOUSLY      = 2,
        I2C_MODE_READ_STOP              = 3,
    };

    enum {
        COMMAND_REPORT_VERSION          = 0xF9, // report firmware version
        COMMAND_SYSTEM_RESET            = 0xFF, // reset from MIDI
        COMMAND_SET_PIN_MODE            = 0xF4, // set a pin to INPUT/OUTPUT/PWM/etc
        COMMAND_DIGITAL_MESSAGE         = 0x90, // send data for a digital port
        COMMAND_ANALOG_MESSAGE          = 0xE0, // send data for an analog pin (or PWM)
        COMMAND_REPORT_ANALOG           = 0xC0, // enable analog input by pin #
        COMMAND_REPORT_DIGITAL          = 0xD0, // enable digital input by port
        COMMAND_SERVO_CONFIG            = 0x70, // set the parameters for a servo pin (max and min pulse, max and min angle)

        COMMAND_START_SYSEX             = 0xF0, // start a MIDI SysEx message
        COMMAND_END_SYSEX               = 0xF7, // end a MIDI SysEx message

        COMMAND_I2C_REQUEST             = 0x76,
        COMMAND_I2C_REPLY               = 0x77,
        COMMAND_I2C_CONFIG              = 0x78,
        COMMAND_I2C_SAMPLING_INTERVAL   = 0x7A,

        COMMAND_PIN_MODE_QUERY          = 0x72, // ask for current and supported pin modes
        COMMAND_PIN_MODE_RESPONSE       = 0x73, // reply with current and supported pin modes
        COMMAND_PIN_STATE_QUERY         = 0x6D,
        COMMAND_PIN_STATE_RESPONSE      = 0x6E,
        COMMAND_CAPABILITY_QUERY        = 0x6B,
        COMMAND_CAPABILITY_RESPONSE     = 0x6C,
        COMMAND_ANALOG_MAPPING_QUERY    = 0x69,
        COMMAND_ANALOG_MAPPING_RESPONSE = 0x6A,
        COMMAND_REPORT_FIRMWARE         = 0x79,
    };

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
