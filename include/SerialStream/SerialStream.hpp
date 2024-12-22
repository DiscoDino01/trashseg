#ifndef SERIALSTREAM_H
#include <Arduino.h>
#define SERIALSTREAM_H



class SerialStreamPrint {
public:
    // Constructor to allow specifying which HardwareSerial port to use, defaults to Serial
    SerialStreamPrint(HardwareSerial& hs = Serial) : _serial(hs) {}



    template<typename T>
    SerialStreamPrint& operator<<(const T& value) {
        _serial.print(value);
        return *this;
    }

    // Support for manipulators like '\n'
    SerialStreamPrint& operator<<(const char& value) {
        if (value == '\n') {
            _serial.println();
        } else {
            _serial.print(value);
        }
        return *this;
    }

private:
    HardwareSerial& _serial; // Reference to the selected HardwareSerial port
};


/// Declare (more like define) global instances

// you can now `cout` like in regular c++! -- [[stupid extern wont work]]
/* extern */static SerialStreamPrint cout;

#endif // SERIALSTREAM_H
