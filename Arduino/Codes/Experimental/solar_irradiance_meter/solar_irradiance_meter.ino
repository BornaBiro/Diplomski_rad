#define SC_CURRENT    50E-3
#define TIME_PERIOD   600
#define ADC_MAX       4095.0
#define OPAMP_OFFSET  0.0795

void setup()
{
  Serial.begin(115200);
  pinMode(A0, INPUT_ANALOG);
  analogReadResolution(12);
}

uint16_t rawAdc;
double current;
double energy;
double energyJ;

void loop()
{
  rawAdc = analogRead(A0);
  current = ((rawAdc / ADC_MAX * 3.3) - OPAMP_OFFSET) / (220/4.7);
  energyJ = (1.0 / 1E4) * (current / SC_CURRENT) * (1000 * TIME_PERIOD);
  energy = current / SC_CURRENT * 1000;
  //Serial.print("ADC:"); Serial.println(rawAdc);
  Serial.print("Current:"); Serial.print(current * 1000, 2); Serial.println("mA");
  Serial.print("Energy:"); Serial.print(energyJ, 2); Serial.println("J/cm^2");
  Serial.print("Energy:"); Serial.print(energy, 2); Serial.println("W/m^2");
  delay(500);
}
