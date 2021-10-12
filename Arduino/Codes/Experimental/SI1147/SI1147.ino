#include <Wire.h>
#define SI1147_ADDR                   0x60

#define SI1147_PART_ID                0x00
#define SI1147_REV_ID                 0x01
#define SI1147_SEQ_ID                 0x02
#define SI1147_INT_CFG                0x03
#define SI1147_IRQ_ENABLE             0x04
#define SI1147_HW_KEY                 0x07
#define SI1147_MEAS_RATE0             0x08
#define SI1147_MEAS_RATE1             0x09
#define SI1147_PS_LED21               0x0F
#define SI1147_PS_LED3                0x10
#define SI1147_UCOEF0                 0x13
#define SI1147_UCOEF1                 0x14
#define SI1147_UCOEF2                 0x15
#define SI1147_UCOEF3                 0x16
#define SI1147_PARAM_WR               0x17
#define SI1147_COMMAND                0x18
#define SI1147_RESPONSE               0x20
#define SI1147_IRQ_STATUS             0x21
#define SI1147_ALS_VIS_DATA0          0x22
#define SI1147_ALS_VIS_DATA1          0x23
#define SI1147_ALS_IR_DATA0           0x24
#define SI1147_ALS_IR_DATA1           0x25
#define SI1147_PS1_DATA0              0x26
#define SI1147_PS1_DATA1              0x27
#define SI1147_PS2_DATA0              0x28
#define SI1147_PS2_DATA1              0x29
#define SI1147_PS3_DATA0              0x2A
#define SI1147_PS3_DATA1              0x2B
#define SI1147_AUX_DATA0              0x2C
#define SI1147_AUX_DATA1              0x2D
#define SI1147_PARAM_RD               0x2E
#define SI1147_CHIP_STATUS            0x30
#define SI1147_ANA_IN_KEY             0x3B

// Command Register Summary
#define SI1147_PARAM_QUERY            0b10000000
#define SI1147_PARAM_SET              0b10100000
#define SI1147_NOP                    0b00000000
#define SI1147_RESET                  0b00000001
#define SI1147_BUSADDR                0b00000010
#define SI1147_PS_FORCE               0b00000101
#define SI1147_GET_CAL                0b00010010
#define SI1147_ALS_FORCE              0b00000110
#define SI1147_PSALS_FORCE            0b00000111
#define SI1147_PS_PAUSE               0b00001001
#define SI1147_ALS_PAUSE              0b00001010
#define SI1147_PSALS_PAUSE            0b00001011
#define SI1147_PA_AUTO                0b00001101
#define SI1147_ALS_AUTO               0b00001110
#define SI1147_PSALS_AUTO             0b00001111

/*
   Parameter RAM Summary Table
   Parameters are located in internal memory and are not directly addressable over I2C. They must be indirectly
   accessed using the PARAM_QUERY and PARAM_SET commands described in "4.2. Command Protocol" on
   page 22.
*/
#define SI1147_I2C_ADDR               0x00
#define SI1147_CHLIST                 0x01
#define SI1147_PSLED12_SELECT         0x02
#define SI1147_PSLED3_SELECT          0x03
#define SI1147_PS_ENCODING            0x05
#define SI1174_ALS_ENCODING           0x06
#define SI1147_PS1_ADCMUX             0x07
#define SI1147_PS2_ADCMUX             0x08
#define SI1147_PS3_ADCMUX             0x09
#define SI1147_PS_ADC_COUNTER         0x0A
#define SI1147_PS_ADC_GAIN            0x0B
#define SI1147_PS_ADC_MISC            0x0c
#define SI1147_ALS_IR_ADCMUX          0x0E
#define SI1147_AUX_ADCMUX             0x0F
#define SI1147_ALS_VIS_ADC_COUNTER    0x10
#define SI1147_ALS_VIS_ADC_GAIN       0x11
#define SI1147_ALS_VIS_ADC_MISC       0x12
#define SI1147_LED_REC                0x1C
#define SI1147_ALS_IR_ADC_COUNTER     0x1D
#define SI1147_ALS_IR_ADC_GAIN        0x1E
#define SI1147_ALS_IR_ADC_MISC        0x1F

void setup() {
  Wire.setSDA(PB9);
  Wire.setSCL(PB8);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Code has started.");

  if (!si1147_init(D4))
  {
    Serial.println("No chip found!");
    while (true);
  }

  setUV();
}

void loop() {
  forceUV();
  
  Serial.print("UV Index: ");
  Serial.println(getUV() / 100.0, 1);
  Serial.print("Visable: ");
  Serial.println(getVis() * 0.282 * 16.5, 2);
  Serial.print("IR: ");
  Serial.println(getIR(), DEC);
  delay(1000);
}

uint8_t readReg(uint8_t _reg)
{
  Wire.beginTransmission(SI1147_ADDR);
  Wire.write(_reg);
  Wire.endTransmission(false);

  Wire.requestFrom(SI1147_ADDR, 1);
  return Wire.read();
}

void writeReg(uint8_t _reg, uint8_t _data)
{
  Wire.beginTransmission(SI1147_ADDR);
  Wire.write(_reg);
  Wire.write(_data);
  Wire.endTransmission();
}

void writeRegs(uint8_t *_regs, uint8_t _n)
{
  Wire.beginTransmission(SI1147_ADDR);
  Wire.write(_regs, _n);
  Wire.endTransmission();
}

void readRegs(uint8_t _reg, uint8_t *_data, uint8_t _n)
{
  Wire.beginTransmission(SI1147_ADDR);
  Wire.write(_reg);
  Wire.endTransmission(false);

  Wire.requestFrom(SI1147_ADDR, _n);
  for (int i = 0; i < _n; i++)
  {
    _data[i] = Wire.read();
  }
}

void clearResponseReg()
{
  Wire.beginTransmission(SI1147_ADDR);
  Wire.write(SI1147_COMMAND);
  Wire.write(0);
  Wire.endTransmission();
}

uint8_t getResponse()
{
  Wire.beginTransmission(SI1147_ADDR);
  Wire.write(SI1147_RESPONSE);
  Wire.endTransmission(false);

  Wire.requestFrom(SI1147_ADDR, 1);
  return Wire.read();
}

bool si1147_init(uint8_t _intPin)
{
  uint8_t tempRegs[2];
  uint8_t _res = 0;

  // INT pin must be used! Especialy while power up, it must be on HIGH logic level!
  pinMode(_intPin, INPUT_PULLUP);

  // Read PART_ID register (it shound return 0b01000111 for Si1147)
  _res = readReg(0);
  if (_res != 0b01000111) return false;

  // Reset IC
  writeReg(SI1147_COMMAND, 0x01);
  _res = getResponse();
  if (_res != 0) return false;

  // "Unlock IC"
  writeReg(0x07, 0x17);

  // Get the status of Si1147 (0 - sleep, 1 - suspend, 2 - running
  // _res = readReg(SI1147_CHIP_STATUS);
  // if (_res != 2) return false;

  // Disable drive for LED1, LED2 and LED3 (here we are not using proximity feature, just ALS & UV)
  writeReg(SI1147_PARAM_WR, 0);
  tempRegs[0] = SI1147_COMMAND;
  tempRegs[1] = SI1147_PARAM_SET | SI1147_PSLED12_SELECT;
  writeRegs(tempRegs, 2);

  writeReg(SI1147_PARAM_WR, 0);
  tempRegs[0] = SI1147_COMMAND;
  tempRegs[1] = SI1147_PARAM_SET | SI1147_PSLED3_SELECT;
  writeRegs(tempRegs, 2);

  // Set no current @ IR LEDs outputs
  writeReg(SI1147_PS_LED21, 0);
  writeReg(SI1147_PS_LED3, 0);

  // Enable INT on ALS complete
  writeReg(SI1147_IRQ_ENABLE, 1);

  // Set INT on ALS complete
  writeReg(SI1147_IRQ_STATUS, 1);

  // Enable Interrupt on INT pin of Si1147
  writeReg(SI1147_INT_CFG, 1);

  return true;
}

void setUV()
{
  uint8_t tempRegs[5];
  
  // Enable UV meas. (1 << 7), ALS IR (1 << 5) and ALS VIS (1 << 4)  in CH list
  writeReg(SI1147_PARAM_WR, (1 << 7) | (1 << 5) | (1 << 4));
  tempRegs[0] = SI1147_COMMAND;
  tempRegs[1] = SI1147_PARAM_SET | SI1147_CHLIST;
  writeRegs(tempRegs, 2);

  // Configure UCOEF
  tempRegs[0] = SI1147_UCOEF0;
  tempRegs[1] = 0x7B;
  tempRegs[2] = 0x6B;
  tempRegs[3] = 0x01;
  tempRegs[4] = 0x00;
  writeRegs(tempRegs, 5);

  // Set the VIS_RANGE and IR_RANGE bits
  writeReg(SI1147_PARAM_WR, 1 << 5);
  //writeReg(SI1147_PARAM_WR, 0);
  tempRegs[0] = SI1147_COMMAND;
  tempRegs[1] = SI1147_PARAM_SET | SI1147_ALS_VIS_ADC_MISC;
  writeRegs(tempRegs, 2);

  writeReg(SI1147_PARAM_WR, 1 << 5);
  //writeReg(SI1147_PARAM_WR, 0);
  tempRegs[0] = SI1147_COMMAND;
  tempRegs[1] = SI1147_PARAM_SET | SI1147_ALS_IR_ADC_MISC;
  writeRegs(tempRegs, 2);
}

void forceUV()
{
  // Force one UV meas.
  writeReg(SI1147_COMMAND, SI1147_ALS_FORCE);

  // Wait for interrupt event
  while (digitalRead(D4));

  // Clear interrupt by sending 1 to corresponding interrupt
  writeReg(SI1147_IRQ_STATUS, 1);
}

uint16_t getUV()
{
  uint8_t regData[2];
  
  // Get the UV data
  readRegs(SI1147_AUX_DATA0, regData, 2);

  return (uint16_t)((regData[1] << 8) | regData[0]);
}

int16_t getVis()
{
  uint8_t regData[2];
  
  // Get ambient light visable spectrum data
  readRegs(SI1147_ALS_VIS_DATA0, regData, 2);
  return (int16_t)(((regData[1] << 8) | regData[0]) - 256);
}

int16_t getIR()
{
  uint8_t regData[2];
  
  // Get ambient light IR spectrum data
  readRegs(SI1147_ALS_IR_DATA0, regData, 2);
  return (int16_t)(((regData[1] << 8) | regData[0]) - 256);
}
