#ifndef _MY_STRUCTS_H_
#define _MY_STRUCTS_H_

struct syncStructHandle{
  uint8_t header;
  uint32_t myEpoch;
  uint32_t readInterval;
  uint32_t sendInterval;
};

#endif
