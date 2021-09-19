#include "communication.h"

uint8_t rfBuffer[32];
uint64_t addr[2] = {0x65646F4E31, 0x65646F4E32};
const char syncStr[] = {"SYNC %3d"};

void communication_Setup()
{
    RF24_setAutoAck(1);
    RF24_enableAckPayload();
    RF24_setChannel(0);
    RF24_setDataRate(RF24_250KBPS);
    RF24_setPALevel(RF24_PA_MAX, 1);
    RF24_openWritingPipe(addr[0]);
    RF24_openReadingPipe(1, addr[1]);
    RF24_stopListening();
}

uint8_t communication_Sync(struct syncStructHandle *_s)
{
    uint8_t syncOk = 0;
    uint8_t syncTimeout = 120;
    uint32_t time1 = HAL_GetTick();
    while (!syncOk && syncTimeout != 0)
    {
        if ((HAL_GetTick() - time1) > 1000)
        {
            time1 = HAL_GetTick();
            char lcdTemp[9];
            sprintf(lcdTemp, syncStr, syncTimeout--);
            glassLCD_Clear();
            glassLCD_WriteData(lcdTemp);
            glassLCD_Update();

            _s->myEpoch = RTC_GetEpoch();
            RF24_write(_s, sizeof(struct syncStructHandle), 0);
            if (RF24_isAckPayloadAvailable())
            {
                syncOk = 1;
                while (RF24_available(NULL))
                {
                    RF24_read(rfBuffer, 32);
                }
                if (rfBuffer[0] == SYNC_HEADER)
                {
                    memcpy(_s, rfBuffer, sizeof(struct syncStructHandle));
                    // Just for debug, remove it later!
                    char mymy[9];
                    sprintf(mymy, "%d", _s->myEpoch);
                    glassLCD_Clear();
                    glassLCD_WriteData(mymy);
                    glassLCD_Update();
                    HAL_Delay(1000);
                    // ----------------------------------
                    syncOk = 1;
                }
            }
        }
    }
    return syncOk;
}
