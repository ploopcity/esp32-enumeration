#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BLEDevice.h>
#include <ESPping.h>
#include <AsyncTCP.h>
#include <lwip/sockets.h>
#include <vector>
#include <Discord_WebHook.h>

const String device_name = "Device1"; // name the device whatever you want..
String DISCORD_WEBHOOK = "YOUR DISCORD WEBHOOK URL"; //change with your discord webhook URL
Discord_Webhook discord;

bool currentlySending = false;
bool wifiState = false;

//put in some known wifi credentials  {"SSID":"PASSWORD"}
const int num_networks = 2;
const char* networks[num_networks][2] = {
  {"wifi_name 1", "password 1"},
  {"wifi_name 2", "password 2"}
};

const int PORT_COUNT = 32; // Number of ports to scan
const int PORT_NUMBERS[] = {
    20, // FTP data transfer
    21, // File Transfer Protocol (FTP)
    22, // Secure Shell (SSH)
    23, // Telnet
    25, // Simple Mail Transfer Protocol (SMTP)
    53, // Domain Name System (DNS)
    67, // DHCP
    68, // DHCP
    80, // Hypertext Transfer Protocol (HTTP)
    110, // Post Office Protocol (POP3)
    123, // NTP
    135, // Remote Procedure Call (RPC) endpoint mapper
    139, // NetBIOS Session Service
    143, // Internet Message Access Protocol (IMAP)
    161, // SNMP
    179, // BGP
    389, // Lightweight Directory Access Protocol (LDAP)
    443, // HTTPS
    445, // Microsoft-DS (SMB over TCP)
    465, // SMTP over SSL
    548, // Apple Filing Protocol (AFP)
    587, // SMTP (submission)
    993,  // IMAP over SSL
    995,  // POP3 over SSL
    1433, // Microsoft SQL Server
    1723, // PPTP
    3306, // MySQL
    3389, // Remote Desktop Protocol (RDP)
    5900, // Virtual Network Computing (VNC)
    6667, // Internet Relay Chat (IRC)
    8080, // HTTP Alternate (HTTP proxy)
    8443 // HTTPS Alternate (HTTPS proxy)
};

std::vector<String> targetList;
int sweepArraySize = 0;

void sendToDiscord(String the_message)
{
  discord.disableDebug();
  discord.begin(DISCORD_WEBHOOK);
  delay(100);
  discord.send(the_message);
  delay(100);
  return;
}

void port_scanner(String target_ip) {
  
  IPAddress subnet = WiFi.subnetMask();
  IPAddress baseIP = WiFi.localIP() & subnet;
  IPAddress ip;

  Serial.print("Starting port scan on ");
  Serial.println(target_ip);
  String allResults = "";
  

  for (int port = 0; port < PORT_COUNT; port++) {
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {
      Serial.println("Error creating socket");
      return;
    }

    sockaddr_in sock_addr;
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = ip.fromString(target_ip);
    sock_addr.sin_port = htons(PORT_NUMBERS[port]);
    
    try{
      int result = connect(sock, (sockaddr*)&sock_addr, sizeof(sock_addr));
      if (result == 0) {
        Serial.print("Port ");
        Serial.print(PORT_NUMBERS[port]);
        Serial.print(" is open on ");
        Serial.println(target_ip);  

        String discordMessage = "Port "+String(PORT_NUMBERS[port])+" is open on "+target_ip;
        sendToDiscord(discordMessage);
        
      }else{
        //Serial.print("Port ");
        //Serial.print(PORT_NUMBERS[port]);
        //Serial.println(" is closed");
      }
    }catch(...)
    {
      Serial.println("something went bad");
      return;
    }
    close(sock);
    delay(10);
  }
  delay(100);  
  return;
}




String pingSweep() {
  Serial.println("Starting ping sweep");
  int devices_found = 0;
  String devices = "";
  String allResults = "Ping Sweep Results:\n";
  IPAddress localIP = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();
  IPAddress baseIP = WiFi.localIP() & subnet;    
  for (int i = 1; i < 255; i++) {
    IPAddress targetIP(baseIP[0], baseIP[1], baseIP[2], i);
    if(targetIP.toString() != localIP.toString())
    {
      try{
        if (Ping.ping(targetIP,1)) {
          if(Ping.averageTime() > 0)
          {
            String theIPstring = targetIP.toString();
            targetList.push_back(theIPstring);            
            devices += targetIP.toString() + "\n";
            Serial.printf("IP %s is alive, response time: %.2f", targetIP.toString().c_str(), Ping.averageTime());
            Serial.println("ms");            
            sendToDiscord("IP "+theIPstring+ " is alive, response time "+Ping.averageTime()+"ms");            
            devices_found++;
          }else{
            //Serial.printf("IP %s is Dead", targetIP.toString().c_str());          
            //Serial.println(".");
          }    
          
          delay(10);
        }
      }catch(...){
        return devices;
      }          
    }
  }
  delay(10);
        
  Serial.print("Done with ping sweep.. ");  
  Serial.print(devices_found);
  Serial.println(" devices found");
  delay(10);  
  
  return devices;
}


String scanBluetoothDevices() {
    String deviceList = "";
    BLEDevice::init("PoopyPants"); // Initialize the BLE device
    BLEScan* pBLEScan = BLEDevice::getScan(); // Create a scan object
    pBLEScan->setActiveScan(true); // Active scanning set
    BLEScanResults foundDevices = pBLEScan->start(5); // Start the scan and set the timeout to 5 seconds
    deviceList += foundDevices.getCount();
    deviceList += " Bluetooth Devices nearby:\n\n";
    sendToDiscord(String(foundDevices.getCount())+" Bluetooth Devices nearby:");
    delay(200);
    for (int i = 0; i < foundDevices.getCount(); i++) {
        BLEAdvertisedDevice d = foundDevices.getDevice(i);
        String deviceName = "No Name";
        String mfgData = d.getManufacturerData().c_str();
        if(d.haveName())
        {
          deviceName = d.getName().c_str();         
        }                     
        deviceList += deviceName;
        deviceList += "->";        
        deviceList += d.getAddress().toString().c_str();
        deviceList += "\n";

        String discord_message = deviceName+" -> "+d.getAddress().toString().c_str(); 
        sendToDiscord(discord_message);
        delay(200);
    }

    BLEDevice::deinit(); // Deinitialize the BLE device
    Serial.println(deviceList);
    return deviceList;
}


String scan_wifi() {
    String wifi_list = "";
    int n = WiFi.scanNetworks();
    if (n == 0) {
        wifi_list = "No WiFi networks found";
    } else {
        wifi_list += n;
        wifi_list += " Access Points nearby:\n\n";
        for (int i = 0; i < n; i++) {
            wifi_list += String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*") + ((WiFi.SSID(i) == WiFi.SSID()) ? "@" : "") + '\n';
            delay(10);
        }
    }
    return wifi_list;
}


void setup(void) {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Boot Success!");
  Serial.println("Scanning the area");

  //scan the area for wifi networks.. auto connect to familiar networks
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("No WiFi networks found");
  } else {
    Serial.print("There are ");    
    Serial.print(n);
    Serial.println(" wifi networks nearby");

    for (int i = 0; i < n; i++) {
      for(int j=0;j<num_networks;j++){
        if(WiFi.SSID(i) == networks[j][0]){
          Serial.println("Found a familiar network!");
          WiFi.begin(networks[j][0], networks[j][1]);
          Serial.print("Connecting to WiFi...");
          while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
          }
          Serial.println(" ");
          Serial.println("Connected to " + WiFi.SSID());        
          wifiState = true;          
          break;
        }
      }
    }
  }

  if(!wifiState)
  {
    return;
  }
  
  delay(100);

  // Get the ESP32's local IP address
  IPAddress localIP = WiFi.localIP();

  // Get the ESP32's global IP address
  HTTPClient http;
  http.begin("http://checkip.dyndns.org");
  int httpCode = http.GET();
  String payload = http.getString();
  int first = payload.indexOf("Address: ") + 9;
  int last = payload.indexOf("</body>");
  String globalIP = payload.substring(first, last);

  Serial.print("My Local IP: ");
  Serial.println(localIP.toString());  

  Serial.print("My Global IP: ");
  Serial.println(globalIP);  
  
  //Send wake up message to discord
  delay(10);
  Serial.println("Sending my wake_up details to discord");
  delay(100);
  sendToDiscord(device_name+" just woke up!");
  delay(100);
  sendToDiscord("Wifi SSID: "+WiFi.SSID());
  delay(100);
  sendToDiscord("Local IP: "+localIP.toString());
  delay(100);
  sendToDiscord("Global IP: "+globalIP);
  delay(200);

  //Bluetooth Scan
  scanBluetoothDevices();
  delay(200);

  //Ping Sweep
  sendToDiscord("Starting Ping Sweep");  
  delay(200);
  pingSweep();
  delay(200);
  
  //Port Scan
  if(targetList.size() > 0){
    sendToDiscord("Ping sweep completed, "+String(targetList.size())+" devices found. Starting port scan on those devices");
    delay(200);
    for (int i = 0; i < targetList.size(); i++){   
    port_scanner(targetList[i]);
    delay(100);
    }
  }  
  
  delay(500);
  sendToDiscord(device_name +" finished all scans... Thanks for playing!"); //All done notification for discord
  Serial.println("All scans are complete. Standing by");
}

void loop(void) {  
  delay(2);  
}






