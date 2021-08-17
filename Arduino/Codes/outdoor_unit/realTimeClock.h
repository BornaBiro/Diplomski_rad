#ifndef __REALTIMECLOCK_H__
#define __REALTIMECLOCK_H__

// HAL_RTC_AlarmAEventCallback() function changed to __weak attribute in order to make succ. compile in:
// C:\Users\[user]\AppData\Local\Arduino15\packages\STM32\hardware\stm32\1.9.0\libraries\SrcWrapper\src\stm32\rtc.c

//#include "stm32l0xx_hal_rtc.h"  // STM32 HAL RTC Library
//#include "stm32l0xx_hal_rcc.h"
//#include "stm32l0xx_hal.h"
#include "stm32_def.h"          // Stores Error_Handler(); STM32 HAL Function
#include "time.h"

#include "Arduino.h"
static uint8_t *_alarmFlag = NULL;
static RTC_HandleTypeDef hrtc;

class Rtc {
  public:
    Rtc();
    void begin();
    void setTime(uint8_t _h, uint8_t _m, uint8_t _s);
    void setDate(uint8_t _d, uint8_t _m, uint16_t _y);
    void setEpoch(uint32_t _epoch);

    struct tm getRTCData();
    uint32_t getEpoch();

    // Only Alarm A is used in this library!
    void setAlarm(bool _state, uint32_t _match);
    void setAlarmEpoch(uint32_t _epoch);
    uint32_t getAlarmEpoch();
    void alarmFlag(uint8_t *_f);

    void getTimeAndDate(uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint16_t *_year);
    uint32_t timeAndDateToEpoch(uint8_t _sec, uint8_t _min, uint8_t _hour, uint8_t _day, uint8_t _month, uint16_t _year);
    void epochToTimeAndDate(uint32_t _ep, uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint16_t *_year);

  private:
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    time_t _epoch;
    const time_t* _epochHandler = &_epoch;
    uint32_t _alarmEpoch;
};

#endif
