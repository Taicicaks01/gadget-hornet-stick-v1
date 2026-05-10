#include "SensorManager.h"
#include "config.h"

SensorManager sensorManager;

SensorManager::SensorManager() : currentTemp(0), currentHum(0), currentPres(0), currentRange(0), filteredRange(0), rangeValid(false) {}

bool SensorManager::initBME() {
    return bme.begin(I2C_BME);
}

bool SensorManager::initToF() {
    return lox.begin();
}

void SensorManager::update() {
    // Membaca VL53L0X
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false); 

    if (measure.RangeStatus != 4) {
        // Jika pembacaan baru pertama kali (atau valid pertama kali), set langsung
        if (!rangeValid) {
            filteredRange = measure.RangeMilliMeter;
        } else {
            // EMA Filter (Alpha = 0.3 untuk kelancaran)
            filteredRange = (0.3 * measure.RangeMilliMeter) + (0.7 * filteredRange);
        }
        currentRange = (int)filteredRange;
        rangeValid = true;
    } else {
        rangeValid = false;
    }

    // Membaca BME280
    currentTemp = bme.readTemperature();
    currentHum  = bme.readHumidity();
    currentPres = bme.readPressure() / 100.0F; 
    currentAlt  = bme.readAltitude(1013.25);

    // Membaca Baterai pada pin A0
    int rawA0 = analogRead(A0);
    // V_adc = (rawA0 / 1023.0) * 3.2V.  External divider = 0.5 -> V_batt = V_adc * 2
    float vBatt = (rawA0 / 1023.0) * 6.4; 
    
    // Map voltage to percentage (3.2V = 0%, 4.2V = 100%)
    if (vBatt >= 4.2) currentBattery = 100;
    else if (vBatt <= 3.2) currentBattery = 0;
    else currentBattery = (int)((vBatt - 3.2) * 100.0);
}

float SensorManager::getTemp() const { return currentTemp; }
float SensorManager::getHum() const { return currentHum; }
float SensorManager::getPres() const { return currentPres; }
float SensorManager::getAltitude() const { return currentAlt; }
int SensorManager::getBatteryPercent() const { return currentBattery; }
int SensorManager::getRange() const { return currentRange; }
bool SensorManager::isRangeValid() const { return rangeValid; }
