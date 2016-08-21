# QtFirmata
A Qt5 implementation of a [Firmata](https://github.com/firmata/arduino) client library.

More information on the protocol can be found [here](http://firmata.org/wiki/V2.3ProtocolDetails).

## Building the library
To build the library:

```
$ qmake
$ make
```

This should generate a shared library suitable for your OS.

## Using the library
First, upload the [StandardFirmata](https://github.com/firmata/arduino/blob/master/examples/StandardFirmata/StandardFirmata.ino) sketch to your Arduino of choice.

An example of using this library is included in the 'example' directory. This project assumes that you have built the library and placed the output (dylib, so etc.) in the source directory (i.e. path/to/QtFirmata/libQtFirmata.dylib).

## Discalimer(s)
This library has been tested with an official Arduino Uno running the linked firmware (V2.4 at time of writing). The library was built and tested with Qt5.5.1 and Qt5.6.0 on Mac OSX 10.11.4 El Capitan. There is no known reason why it won't work with other Arduino models or other OSs, but no guarentee is given that it will.

This library is an extension/port/reimplementation of this [project](https://code.google.com/archive/p/firmatator/). There may well still be gaps/errors in the implementation.

All suggestions, comments, (constructive) criticism and bug fixes gratefully received!
