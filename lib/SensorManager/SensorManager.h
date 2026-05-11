#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VL53L0X.h>

class SensorManager {
private:
    Adafruit_BME280 bme;
    Adafruit_VL53L0X lox;

    float currentTemp;
    float currentHum;
    float currentPres;
    float currentAlt;
    int currentBattery;
    
    int currentRange;
    float filteredRange;
    bool rangeValid;

public:
    SensorManager();
    bool initBME();
    bool initToF();
    void update(); // Reads sensors and updates internal state
    void debugDump(); // Print direct sensor readings to Serial

    float getTemp() const;
    float getHum() const;
    float getPres() const;
    float getAltitude() const;
    int getBatteryPercent() const;
    
    int getRange() const;
    bool isRangeValid() const;
};

extern SensorManager sensorManager;

#endif // SENSOR_MANAGER_H
