#ifndef _MY_STRUCTS_H_
#define _MY_STRUCTS_H_

struct syncStructHandle{
  uint8_t header;
  uint32_t myEpoch;
  uint32_t readInterval;
  uint32_t sendEpoch;
};

struct measruementHandle {
    float tempSHT;
    float humidity;
    float pressure;
    uint8_t uv;
    float light;
    float solarJ;
    float solarW;
};

struct data1StructHandle {
  uint8_t header;
  float temp;
  float hum;
  float pres;
};

#endif
