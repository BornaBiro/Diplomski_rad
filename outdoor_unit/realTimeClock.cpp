#include "realTimeClock.h"

Rtc::Rtc()
{

}

RTC_HandleTypeDef* Rtc::begin()
{
  __HAL_RCC_RTC_ENABLE();

  // Initialize RTC Only
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  // Initialize RTC and set the Time and Date
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  return &hrtc;
}

void Rtc::setTime(uint8_t _h, uint8_t _m, uint8_t _s)
{
  sTime.Hours = _h;
  sTime.Minutes = _m;
  sTime.Seconds = _s;
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

void Rtc::setDate(uint8_t _d, uint8_t _m, uint16_t _y)
{
  sDate.Date = _d;
  sDate.Month = _m;
  sDate.Year = (_y % 100);
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void Rtc::setEpoch(uint32_t _epoch)
{

}

void Rtc::getTime(uint8_t *_h, uint8_t *_m, uint8_t *_s)
{
  uint8_t dummy1;
  uint16_t dummy2;
  getTimeAndDate(_s, _m, _h, &dummy1, &dummy1, &dummy2);
}

void Rtc::getDate(uint8_t *_d, uint8_t *_m, uint16_t *_y)
{
  uint8_t dummy;
  getTimeAndDate(&dummy, &dummy, &dummy, _d, _m, _y);
}

uint32_t Rtc::getEpoch()
{

}

void Rtc::setAlarm(bool _state, uint32_t _match)
{

}

void Rtc::setAlarmTime(uint8_t _h, uint8_t _m, uint8_t _s)
{

}

void Rtc::setAlarmDate(uint8_t _d, uint8_t _m, uint16_t _y)
{

}

void Rtc::alarmCallback()
{

}

void Rtc::getTimeAndDate(uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint16_t *_year)
{
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  *_hour = sTime.Hours;
  *_min = sTime.Minutes;
  *_sec = sTime.Seconds;
  *_day = sDate.Date;
  *_month = sDate.Month;
  *_year = sDate.Year;
}

uint32_t Rtc::timeAndDateToEpoch(uint8_t _sec, uint8_t _min, uint8_t _hour, uint8_t _day, uint8_t _month, uint16_t _year)
{
  struct tm _t = {0};
  _t.tm_sec = _sec;
  _t.tm_min = _min;
  _t.tm_hour = _hour;
  _t.tm_mday = _day;
  _t.tm_mon = (_month - 1);
  _t.tm_year = 100 + (_year % 100);
  _t.tm_isdst = -1;
  _t.tm_yday = 0;
  _t.tm_wday = 0;
  return mktime(&_t);
}

void Rtc::epochToTimeAndDate(uint32_t _ep, uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint16_t *_year)
{
  struct tm *_t;
  _epoch = _ep;
  _t = gmtime(_epochHandler);
  *_sec = _t->tm_sec;
  *_min = _t->tm_min;
  *_hour = _t->tm_hour;
  *_day = _t->tm_mday;
  *_month = (_t->tm_mon) + 1;
  *_year = (_t->tm_year) + 1900;
}
