#include <Wire.h>

#include "realTimeClock.h"
#include "glassLCD.h"
//#define USE_SLEEP

Rtc myRtc;
Lcd myLcd;

uint8_t alarm = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Code started");
  pinMode(LED_GREEN, OUTPUT);
  Wire.setSDA(PB9);
  Wire.setSCL(PB8);
  Wire.begin();
  myRtc.begin();
  myLcd.begin(&Wire);
  delay(1000);
  myRtc.setTime(10, 11, 12);
  myRtc.setDate(14, 5, 2021);
  myRtc.setAlarmEpoch(myRtc.getEpoch() + 10);
  myRtc.alarmFlag(&alarm);
  myRtc.setAlarm(true, RTC_ALARMMASK_DATEWEEKDAY);

#ifdef USE_SLEEP
  // Prepare to enter "light sleep" mode
  HAL_PWR_DisablePVD();
  HAL_PWREx_EnableUltraLowPower();
  HAL_SuspendTick();

  // Enter "light sleep" mode
  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

  // Recover from "light sleep" mode
  SystemClock_ConfigFromStop();
  HAL_ResumeTick();
#endif
}

void loop() {
  struct tm t;
  char temp[50];
  t = myRtc.getRTCData();
  //sprintf(temp, "%02d:%02d:%02d - %d/%02d/%04d", t.tm_hour, t.tm_min, t.tm_sec, t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
  //Serial.println(temp);
  //Serial.println(mktime(&t));
  sprintf(temp, "%2d%02d%02d", t.tm_hour, t.tm_min, t.tm_sec);
  myLcd.setDot(1, true);
  myLcd.setDot(3, true);
  myLcd.writeData(temp);
  digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
  delay(1000);
  if (alarm != 0) {
    Serial.println("ALARM!!!!!");
    alarm = 0;
  }
}


extern "C" void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_3;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
