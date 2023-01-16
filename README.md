# esp32-enumeration
Automate the enumeration process using an ESP32

Using the ESP32 WROOM dev board\
larger partition: huge app\

Once the device is powered on it will do the following:

1. try to connect to a wifi network it knows
2. Once connected run ping sweep on network.. return a 'target list' of devices
3. Run port scan on all devices on the target list
4. Send results to a discord server

Libraries used:\
AsyncTCP -> by Hristo Gochkov\
Discord_WebHook -> by Usini\
ESPping -> by dvarrel\

Educational purposes only! Running a port scan on devices you do not own is Illegal!
