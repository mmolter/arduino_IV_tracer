#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <Adafruit_INA219.h>

// Declare object variables.
Adafruit_MCP4725 dac;
Adafruit_INA219  ina219;

// Set physical constants.
const float max_voltage =  1.200;
const float min_voltage = -1.800;
const float gnd_voltage =  2.498 + 0.70;

void setup() {
  // Initialize DAC
  dac.begin(0x62);

  // Initialize current sensor
  ina219.begin();

  // Set to 0 - 400 mA mode.
  ina219.setCalibration_16V_400mA();

  // Initialize serial communications.
  Serial.begin(115200);
}

float set_voltage(float voltage){
  // Sets the DAC to produce a `voltage` across the device-under-test. //
  
  // Check that set voltage is within allowable range. 
  if ((voltage > max_voltage) || (voltage < min_voltage)){
    Serial.println("ERROR: Voltage out of allowable range.");
    return 0;
  }

  // Convert voltage setting to bits.
  float dac_voltage = voltage + gnd_voltage;
  float dac_setting = imap(dac_voltage, 0.000, 5.000, 0.00, 4096.00);

  // Send setting to DAC. 
  dac.setVoltage((int) dac_setting, false);
}

float imap(float x, float in_min, float in_max, float out_min, float out_max){
  // Scale `x` from range `in_min`-`in_max` to range `out_min`-`out-max`. //
  
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  for (float v = min_voltage; v < max_voltage; v += 0.001){

    // Set the voltage on the DUT.
    set_voltage(v);                                  delay(50);

    // Measure the current. 
    float c = ina219.getCurrent_mA();                delay(5);
    float s = ina219.getShuntVoltage_mV() / 1000.0;  delay(5);
    float b = ina219.getBusVoltage_V();

    // Plot.
    Serial.print(s + b - gnd_voltage);    
    Serial.print(',');
    Serial.print(c);
    Serial.print('\n');
  }
}
