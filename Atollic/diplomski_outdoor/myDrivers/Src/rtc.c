#include "rtc.h"

extern RTC_HandleTypeDef hrtc;
static time_t _epoch;
static const time_t* _epochHandler = &_epoch;

uint8_t RTC_SetTime(uint8_t _h, uint8_t _m, uint8_t _s)
{
	RTC_TimeTypeDef sTime = {0};
	sTime.Hours = _h;
	sTime.Minutes = _m;
	sTime.Seconds = _s;
	return HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}

uint32_t RTC_GetEpoch()
{
	struct tm _t = RTC_GetData();
	return mktime(&_t);
}

struct tm RTC_GetData()
{
  struct tm _t = {0};
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  _t.tm_sec = sTime.Seconds;;
  _t.tm_min = sTime.Minutes;
  _t.tm_hour = sTime.Hours;
  _t.tm_mday = sDate.Date;
  _t.tm_mon = (sDate.Month - 1);
  _t.tm_year = 100 + (sDate.Year % 100);
  _t.tm_isdst = -1;
  _t.tm_yday = 0;
  _t.tm_wday = 0;
  return _t;
}

void RTC_SetAlarmEpoch(uint32_t _alarmEpoch, uint32_t _mask)
{
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
	RTC_AlarmTypeDef _sAlarm = {0};
	RTC_TimeTypeDef _sTime = {0};
	uint8_t _dummy;
	RTC_epochToTimeAndDate(_alarmEpoch, &_sTime.Seconds, &_sTime.Minutes, &_sTime.Hours, &_sAlarm.AlarmDateWeekDay, &_dummy, &_dummy);
    _sAlarm.AlarmTime = _sTime;
    _sAlarm.AlarmMask = _mask;
    _sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    _sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    _sAlarm.Alarm = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&hrtc, &_sAlarm, RTC_FORMAT_BIN);
}

void RTC_epochToTimeAndDate(uint32_t _ep, uint8_t *_sec, uint8_t *_min, uint8_t *_hour, uint8_t *_day, uint8_t *_month, uint8_t *_year)
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
