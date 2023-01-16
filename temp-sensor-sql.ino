


/* Written by Ryan Rivard 4/27/22
    for arduino Mega

    Questions contact ryan.j.rivard@gmail.com

    Maintained by Joe Jalbert
*/

/*
  Known Issues/Intented Future Changes: 
   - Timestamp handling may cause unintended newline skips, string cleansing when pulled from time.nist.gov is likely needed, would be
      more robust to implement a DateTime object to handle timestamps rather than using strings

   - Timestamp date handling on SD write is not complete using epoch

   - SD init could be added to the SD library instead of cluttering the setup function

   - Some form of live reading serial print would be useful for debugging and/or tests that require tuning to achieve setpoints 

*/


// Libraries to include
 #include <Wire.h>
 #include <SD.h>
 #include <SPI.h>
 #include <Ethernet.h>
 #include <IPAddress.h>
 #include <utility/w5100.h>
 #include <EthernetUdp.h>
 #include <Adafruit_MAX31856.h>



// Toggle for debug serial outputs
#define DEBUG 1

// Sampling Rate Control
const uint16_t SAMPLE_RATE = 10000; // Will need to be decreased to achieve actual desired delay, does not account for processing time of function
//
// Ethernet Network Time Protocol
// - Provides timestamps synced to the time.nist.gov server
const uint16_t localPort = 8888;       // local port to listen for UDP packets
const char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server
const byte NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
unsigned int EDT_epoch = 0;
EthernetUDP Udp; // A UDP instance to let us send and receive packets over UDP
byte count = 0;

// Pin definitions
const uint8_t SS_SD_PIN = 4;     // Ethernet Sheild default
const uint8_t SS_W5500_PIN = 10; // Ethernet Shiled default

const uint8_t SPI_PIN_SS = 50;   // Ethernet Shield default
const uint8_t SPI_PIN_1 = 51;    // Ethernet Shield default
const uint8_t SPI_PIN_2 = 52;    // Ethernet Shield default
const uint8_t SPI_PIN_3 = 53;    // Ethernet Shield default

// Logging and Ethernet Connectivity Variables
File logfile;
EthernetClient client;
char server[] = "192.168.0.112"; // IP address of the SQL server on RPi
byte server1[] = { 192, 168, 0, 112 }; // IP address of the SQL server on RPi
IPAddress server2(192,168,0,112);
byte mac[] = {0xA8, 0x61, 0x0A, 0xAE, 0x88, 0x6C}; // Provided MAC address of Ethernet Shield
IPAddress ip1(192,168,0,181); // If UDP failes to assign an IP address to the ethernet shield, instead default to this IP

// State Variables
String cartridgeSerial = "null";
boolean startup = true;

void setup()
{

  SPI.begin();
  Serial.begin(9600);
  Wire.begin();
  delay(300);
  Serial.println("Serial Init1");

  tc_init();
  Serial.println("TC Init");
  delay(300);

  // SD INIT 
  pinMode(SS_W5500_PIN, OUTPUT);
  digitalWrite(SS_W5500_PIN, HIGH); // Close Ethernet communication on SPI bus
  pinMode(SS_SD_PIN, OUTPUT);
  digitalWrite(SS_SD_PIN, LOW); // Open SD communication on SPI bus
  SD.begin(SS_SD_PIN);
  Serial.println("SD Init");
  delay(300);

  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i / 10 + '0';
    filename[7] = i % 10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break;  // leave the loop!
    }
  }

  // Set headers of SD CSV
  logfile.print("Date");
  logfile.print(", ");
  logfile.print("Time");
  logfile.print(", ");
  logfile.print("TC0");
  logfile.print("\n");
  
  #ifdef DEBUG
    Serial.println("logfile Headers");
  #endif

  digitalWrite(SS_SD_PIN, HIGH); // Close communication with SD on SPI bus

  digitalWrite(SS_W5500_PIN, LOW); // Open communication with Ethernet chip
  Ethernet.init(SS_W5500_PIN); 
  delay(5000);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip1);
  }
  Serial.println("Ethernet Init");
  delay(1000);

  Udp.begin(localPort); // Establish UDP connection to enable timestamp pulling from internet

  #ifdef DEBUG
    Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
    Serial.print("IP Address        : ");
    Serial.println(Ethernet.localIP());
    Serial.print("Subnet Mask       : ");
    Serial.println(Ethernet.subnetMask());
    Serial.print("Default Gateway IP: ");
    Serial.println(Ethernet.gatewayIP());
    Serial.print("DNS Server IP     : ");
    Serial.println(Ethernet.dnsServerIP());
  #endif
  digitalWrite(SS_W5500_PIN, HIGH); // Close communication with Ethernet chip
}

void loop()
{
  Serial.print("looping...");
  Serial.println(count);
  count++;
  uint32_t epoch = pullNTP(); // Update epoch timestamp from time.nist.gov
  

  //DateTime timestamp = new DateTime((uint32_t)epoch);
  EDT_epoch = pullNTP();

  #ifdef DEBUG
    Serial.println("Epoch from NTP: ");
    Serial.println(EDT_epoch);
    Serial.print(epochToDate(EDT_epoch));
    Serial.print(" -- ");
    Serial.println(epochToTime(EDT_epoch));
  #endif
  tc_update();
  pushData(EDT_epoch);
  
  Ethernet.maintain(); // Must be periodically called to maintain connction to Apache Server
  delay(SAMPLE_RATE);
}

void pushData(uint32_t  timestamp) {
  
  #ifdef DEBUG
    ShowSockStatus();
  #endif

  if (client.connect(server1, 80) == 1)
  { 
    digitalWrite(SS_W5500_PIN, LOW); // Open communication with ethernet chip
    Serial.println("-> Connected");
    // Make a HTTP request:
    // http://http://192.168.100.206//testserver/arduino_temperatures/add_data.php?serial=288884820500006X&temperature=12.3
    client.print("GET /updateDB.php?");
    
    client.print("id=");
    client.print(EDT_epoch);
    client.print("&&");

    client.print(" Temperature=");
    client.print(count);
    client.print("&&");

    client.print(" Humidity=");
    client.print("55%");
    client.print("&&");

    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
    client.println();
    client.stop();
    digitalWrite(SS_W5500_PIN, HIGH); // Close communication with ethernet chip
    Serial.println("Data pushed to Server");
  }
  else
  {
    // you didn't get a connection to the server:
    Serial.println("--> connection failed/n");
  }

  // SD Write
  digitalWrite(SS_SD_PIN, LOW); // Open communication SPI bus with SD
  if (!SD.begin(SS_SD_PIN)) { //ensure SD card is present
      Serial.println("Card failed, or not present");
    }
    else {
      //Serial.println("Card initialized.");
    }
 
    if (!logfile) {
      Serial.println("Couldn't create file!");
    }

    logfile.print(epochToDate(EDT_epoch));
    logfile.print(",");
    logfile.print(epochToTime(EDT_epoch));
    logfile.print(",");
    logfile.print(count);
    logfile.print('\n');

    logfile.flush();
    digitalWrite(SS_SD_PIN, HIGH); // Close communication SPI bus with SD

    Serial.println("Data written to SD");
}

// TODO: Finish Epoch Conversion to datestamp
String epochToDate(uint32_t timestamp) {
  char created_date[] = "00/00/00";
  created_date[0] = (char)(((EDT_epoch  % 86400) / 60) / 10) + '0';
  created_date[1] = (char)((EDT_epoch  % 86400) / 60) + '0';
  created_date[2] = "/";
  created_date[3] = (char)(EDT_epoch  % 86400) + '0';
  created_date[4] = (char)((EDT_epoch  % 86400) % 10) + '0';
  created_date[5] = "/";
  created_date[6] = (char)((EDT_epoch % 31556926) / 10) + '0';
  created_date[7] = (char)((EDT_epoch % 31556926) % 10) + '0';
  return created_date;
}

String epochToTime(uint32_t  timestamp) {
  char created_time[] = "00:00:00";
  created_time[0] = (char)(((EDT_epoch  % 86400L) / 3600) / 10) + '0';
  created_time[1] = (char)(((EDT_epoch  % 86400L) / 3600)) + '0';
  created_time[2] = "-";
  created_time[3] = (char)(((EDT_epoch  % 3600) / 60) / 10) + '0';
  created_time[4] = (char)(((EDT_epoch  % 3600) / 60) % 10) + '0';
  created_time[5] = "-";
  created_time[6] = (char)((EDT_epoch % 60) / 10) + '0';
  created_time[7] = (char)((EDT_epoch % 60) % 10) + '0';
  return created_time;
}

// A debugging tool that shows active sockets for ethernet connection and any errors they're facing
void ShowSockStatus()
{
	for (int i = 0; i < MAX_SOCK_NUM; i++) {
		Serial.print("Socket#");
		Serial.print(i);
		uint8_t s = W5100.readSnSR(i);
		Serial.print(":0x");
		Serial.print(s,16);
		Serial.print(" ");
		Serial.print(W5100.readSnPORT(i));
		Serial.print(" D:");
		uint8_t dip[4];
		W5100.readSnDIPR(i, dip);
		for (int j=0; j<4; j++) {
			Serial.print(dip[j],10);
			if (j<3) Serial.print(".");
		}
		Serial.print("(");
		Serial.print(W5100.readSnDPORT(i));
		Serial.println(")");
	}
}

// From arduino Example code 
// https://docs.arduino.cc/tutorials/ethernet-shield-rev2/UdpNtpClient
uint32_t pullNTP() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket()) {
    // We've received a packet, read the data from it
    #ifdef DEBUG
      Serial.println("Packets!");
    #endif
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    uint32_t highWord = word(packetBuffer[40], packetBuffer[41]);
    uint32_t lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    uint32_t secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = ");
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const uint32_t seventyYears = 2208988800;
    // subtract seventy years:
    uint32_t epoch = secsSince1900 - seventyYears;
    return epoch-14400; // offset to EDT
    // print Unix time:
    //Serial.println(epoch);
  }
}

// send an NTP request to the time server at the given address
void sendNTPpacket(const char * address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
