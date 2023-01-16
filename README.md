# esp32-enumeration
Use an ESP32 to automate the noisey parts of the Enumeration process.

-- I'm using the ESP32 WROOM dev board.. It should work on most ESP32 boards tho\
-- Make sure to update partition scheme to allocate more space.

Arduino IDE you can change your partition by selecting: Tools -> Partition Scheme -> Huge App

Platform.io IDE you can change your partition scheme by updating the platformio.ini file add add this line to your board settings:\
"board_build.partitions = min_spiffs.csv"

Once the device is powered on it will do the following:

1. Connect to a wifi network it knows about. Once connected:
2. Scan the area for Bluetooth devices nearby
3. Run a ping sweep on network
4. Run a port scan on every device that responded to the ping sweep
5. Log results to a discord channel

Libraries used:\
AsyncTCP -> by Hristo Gochkov\
Discord_WebHook -> by Usini\
ESPping -> by dvarrel

Educational purposes only! Running a port scan on devices you do not own is Illegal!
