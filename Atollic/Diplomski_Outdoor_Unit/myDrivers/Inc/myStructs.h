#ifndef _MY_STRUCTS_H_
#define _MY_STRUCTS_H_

struct syncStructHandle{
  uint8_t header;
  uint32_t myEpoch;
  uint32_t readInterval;
  uint32_t sendEpoch;
};

struct measruementHandle {
    uint16_t uv;
    int16_t windDir;
    float tempSHT;
    float humidity;
    float pressure;
    float light;
    double solarJ;
    double solarW;
    float windSpeed;
    float battery;
};

struct data1StructHandle {
  uint8_t header;
  float temp;
  float hum;
  float pres;
};

#endif
