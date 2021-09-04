#ifndef __RTC_H__
#define __RTC_H__

#include "stdint.h"
#include "stm32l0xx_hal.h"
#include "time.h"

uint8_t RTC_SetTime(uint8_t _h, uint8_t _m, uint8_t _s);
uint32_t RTC_GetEpoch();
struct tm RTC_GetData();
void RTC_SetAlarmEpoch(uint32_t _alarmEpoch, uint32_t _mask);
void RTC_epochToTimeAndDate(uint32_t _ep, uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint8_t *_year);

#endif
