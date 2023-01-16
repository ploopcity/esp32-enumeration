# esp32-enumeration
Automate the 'noisy' parts of a pen-test; Enumeration.. Use the ESP32 to run a ping sweep on current network, then run a port scan on all the devices that responded to the ping sweep.. Log results to a discord channel of your choice.

-- I'm using the ESP32 WROOM dev board.. It should work on most ESP32 boards tho\
-- Make sure to update partition scheme to allocate more space.

Arduino IDE you can change your partition by selecting: Tools -> Partition Scheme -> Huge App

Platform.io IDE you can change your partition scheme by updating the platformio.ini file add add this line to your board settings:\
"board_build.partitions = min_spiffs.csv"

Once the device is powered on it will do the following:

1. Connect to a wifi network it knows about. Once connected:
2. Scan the area for Bluetooth devices nearby
3. Run a ping sweep on subnet
4. Run a port scan on every device that responded to the ping sweep
5. Log results to a discord channel

Libraries used:\
AsyncTCP -> by Hristo Gochkov\
Discord_WebHook -> by Usini\
ESPping -> by dvarrel

Educational purposes only! Running a port scan on devices you do not own is Illegal!

"Stay safe in cyber space"
