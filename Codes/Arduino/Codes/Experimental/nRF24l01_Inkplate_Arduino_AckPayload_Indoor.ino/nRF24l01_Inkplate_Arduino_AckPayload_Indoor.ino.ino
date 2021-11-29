#include <SPI.h>
#include <time.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "sys/time.h"
#include "Inkplate.h"
#include "RF24_Inkplate.h"

#define SYNC_HEADER     0b00110101
#define DATA1_HEADER    0b00010001

Inkplate display(INKPLATE_1BIT);
SdFile file;
RF24_Inkplate radio(14, 15, 2000000);
byte addresses[][6] = {"1Node", "2Node"};

const char* ssid       = "Biro_WLAN";
const char* password   = "CaVex250_H2sH11";

struct tm tNow;
timeval tm;
const timeval* tmPtr = &tm;
time_t alarmTime;

uint64_t addr[2] = {0x65646F4E31, 0x65646F4E32};

struct syncStructHandle {
  uint8_t header;
  uint32_t myEpoch;
  uint32_t readInterval;
  uint32_t sendEpoch;
} syncStruct = {SYNC_HEADER};

struct data1StructHandle {
  uint8_t header;
  float temp;
  float hum;
  float pres;
} data1Struct = {DATA1_HEADER};

void setup() {
  Serial.begin(115200);
  display.begin();
  display.display();
  SPIClass *mySpi = display.getSPIptr();
  mySpi->begin(14, 12, 13, 15);
  if (!radio.begin(mySpi, &display))
  {
    Serial.println("Radio init ERROR!");
    while (1);
  }
  if (!display.sdCardInit())
  {
    display.setCursor(0, 0);
    display.print("SD Card Error");
    display.display();
    while (true);
  }
  radioSetup();
  Serial.println("Radio Init OK");

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  if (readNTP(&tm.tv_sec))
  {
    settimeofday(tmPtr, NULL);
  }

  // Wait for sync
  bool syncOk = false;
  uint8_t syncTimeout = 120;
  while (!syncOk && syncTimeout != 0)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Timeout: ");
    display.print(syncTimeout--, DEC);
    display.partialUpdate(false, true);
    if (radio.available())
    {
      char temp[32];
      while (radio.available())
      {
        radio.read(temp, 32);
      }
      if (temp[0] == SYNC_HEADER)
      {
        gettimeofday(&tm, NULL);
        syncStruct.myEpoch = tm.tv_sec;
        alarmTime = newWakeupTime(tm.tv_sec);
        syncStruct.sendEpoch = alarmTime;
        radio.writeAckPayload(1, &syncStruct, sizeof(syncStruct));
        syncOk = true;
      }
    }
    delay(1000);
  }
  display.clearDisplay();

  if (syncOk)
  {
    display.print("Sync succ!");
  }
  else
  {
    display.print("Sync failed!");
  }
  display.display();
  display.setCursor(0, 0);
  display.clearDisplay();
  radio.flush_tx();
  radio.flush_rx();
}
int n = 0;
void loop()
{
  gettimeofday(&tm, NULL);
  esp_sleep_enable_timer_wakeup((alarmTime - tm.tv_sec) * 1000000ULL);
  esp_light_sleep_start();

  radio.powerUp();
  radioSetup();

  bool dataRx = false;
  uint8_t rxTimeout = 10;
  while (!dataRx && rxTimeout != 0)
  {
    //display.print("Timeout: ");
    //display.print(rxTimeout, DEC);
    //display.partialUpdate(false, true);
    if (radio.available())
    {
      char temp[32];
      while (radio.available())
      {
        radio.read(temp, 32);
      }
      if (temp[0] == DATA1_HEADER)
      {
        memcpy(&data1Struct, temp, sizeof(data1Struct));
        gettimeofday(&tm, NULL);
        syncStruct.myEpoch = tm.tv_sec;
        alarmTime = newWakeupTime(tm.tv_sec);
        syncStruct.sendEpoch = alarmTime;
        radio.writeAckPayload(1, &syncStruct, sizeof(syncStruct));
        dataRx = true;
        Serial.println(syncStruct.myEpoch);
        Serial.println(syncStruct.sendEpoch);
        Serial.println("----------------------");
        Serial.flush();
      }
    }
    delay(1000);
    rxTimeout--;
  }

  if (dataRx)
  {
    char temp[70];
    struct tm mt;
    mt = epochToHuman(tm.tv_sec);
    sprintf(temp, "  %2d:%02d:%02d %2d/%02d/%04d %.1f, %.1f, %.1f", mt.tm_hour, mt.tm_min, mt.tm_sec, mt.tm_mday, mt.tm_mon + 1, mt.tm_year + 1900, data1Struct.temp, data1Struct.hum, data1Struct.pres);
    display.println(temp);
    if (file.open("myWeatherData.csv", O_RDWR | O_CREAT | O_APPEND))
    {
      file.timestamp(T_ACCESS | T_CREATE | T_WRITE, mt.tm_year + 1900, mt.tm_mon + 1, mt.tm_mday, mt.tm_hour, mt.tm_min, mt.tm_sec);
      char sdCardTemp[100];
      sprintf(sdCardTemp, "%ld; %2d:%02d:%02d %2d/%02d/%04d; %.1f; %.1f; %.1f;", tm.tv_sec, mt.tm_hour, mt.tm_min, mt.tm_sec, mt.tm_mday, mt.tm_mon + 1, mt.tm_year + 1900, data1Struct.temp, data1Struct.hum, data1Struct.pres);
      file.println(sdCardTemp);
      file.close();
    }

  }
  else
  {
    display.println("[no data]");
    gettimeofday(&tm, NULL);
    alarmTime = newWakeupTime(tm.tv_sec);
  }
  display.display();
  n++;
  if (n > 30)
  {
    n = 0;
    display.clearDisplay();
    display.setCursor(0,0);
  }
  radio.flush_tx();
  radio.flush_rx();
  radio.powerDown();
}

bool readNTP(time_t *_epoch)
{
  IPAddress ntpIp;
  WiFiUDP udp;
  const char* NTPServer = "hr.pool.ntp.org";
  uint16_t ntpPort = 123;
  uint8_t ntpPacket[48];

  udp.begin(8888);
  if (!WiFi.hostByName(NTPServer, ntpIp)) return 0;

  ntpPacket[0] = B11100011; //Clock is unsync, NTP version 4, Symmetric passive
  ntpPacket[1] = 0;     // Stratum, or type of clock
  ntpPacket[2] = 60;     // Polling Interval
  ntpPacket[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntpPacket[12]  = 49;
  ntpPacket[13]  = 0x4E;
  ntpPacket[14]  = 49;
  ntpPacket[15]  = 52;
  udp.beginPacket(ntpIp, 123);
  udp.write(ntpPacket, 48);
  udp.endPacket();
  delay(1500);
  if (udp.parsePacket())
  {
    udp.read(ntpPacket, 48);
    uint32_t unix = ntpPacket[40] << 24 | ntpPacket[41] << 16 | ntpPacket[42] << 8 | ntpPacket[43];
    *_epoch = unix - 2208988800UL + 7200;
    return true;
  }
  return false;
}

time_t newWakeupTime(time_t _current)
{
  struct tm _myTime;
  time_t _alarmEpoch;
  memcpy (&_myTime, localtime((const time_t *) &_current), sizeof (_myTime));

  int minutes = (int)(ceil(((double) (_myTime.tm_min) + ((double) (_myTime.tm_sec) / 60)) / 2) * 2);
  _myTime.tm_min = minutes % 60;
  _myTime.tm_sec = 0;
  _alarmEpoch = mktime(&_myTime);

  if (minutes >= 60) _alarmEpoch += 3600;
  return _alarmEpoch;
}

struct tm epochToHuman(time_t _t)
{
  struct tm *_timePtr;
  struct tm _time;
  _timePtr = localtime(&_t);
  memcpy(&_time, _timePtr, sizeof(_time));
  return _time;
}

void radioSetup()
{
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setChannel(0);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX, 1);
  radio.openWritingPipe(addr[1]);
  radio.openReadingPipe(1, addr[0]);
  radio.startListening();
}
