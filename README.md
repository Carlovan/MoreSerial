# MoreSerial
This little library tries to give you some more tool for serial communication.

## Usage
The library is all contained inside the `MoreSerial` namespace.

### HIDDevice

    File: "hidserial.h"

This class allows you to communicate _serially_ with an HID device. 
You can find an example in the `main.cpp` file.

### Compiling
This library uses the [`HIDAPI`](http://www.signal11.us/oss/hidapi/) so you need to install it. 
On Linux you could find it in your distro repositories.
#### Example
On Arch Linux I installed the `hidapi` package from the `community` repository. 
Then to compile:

    g++ -lhidapi-libusb -I/usr/include/hidapi -o main main.cpp

It will probably be different for your situation.
