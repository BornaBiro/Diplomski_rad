#ifndef __REALTIMECLOCK_H__
#define __REALTIMECLOCK_H__

#include "stm32l0xx_hal_rtc.h"  // STM32 HAL RTC Library
#include "stm32l0xx_hal_rcc.h"
#include "stm32_def.h"          // Stores Error_Handler(); STM32 HAL Function
#include "time.h"

class Rtc {
  public:
    Rtc();
    RTC_HandleTypeDef* begin();
    void setTime(uint8_t _h, uint8_t _m, uint8_t _s);
    void setDate(uint8_t _d, uint8_t _m, uint16_t _y);
    void setEpoch(uint32_t _epoch);

    void getTime(uint8_t *_h, uint8_t *_m, uint8_t *_s);
    void getDate(uint8_t *_d, uint8_t *_m, uint16_t *_y);
    uint32_t getEpoch();

    void setAlarm(bool _state, uint32_t _match);
    void setAlarmTime(uint8_t _h, uint8_t _m, uint8_t _s);
    void setAlarmDate(uint8_t _d, uint8_t _m, uint16_t _y);

    void alarmCallback();

    void getTimeAndDate(uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint16_t *_year);
    uint32_t timeAndDateToEpoch(uint8_t _sec, uint8_t _min, uint8_t _hour, uint8_t _day, uint8_t _month, uint16_t _year);
    void epochToTimeAndDate(uint32_t _ep, uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint16_t *_year);

  private:
    RTC_HandleTypeDef hrtc;
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    RTC_AlarmTypeDef sAlarm = {0};

    time_t _epoch;
    const time_t* _epochHandler = &_epoch;
};

#endif
