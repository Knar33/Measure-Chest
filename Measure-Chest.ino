#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>

#define SD_ChipSelectPin 10

Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
float angle;
float measurementsPerSecond = 10;
float rotationalThreshold = .05;
TMRpcm tmrpcm;

void setup() {
  Serial.begin(9600);

  angle = 0;
  tmrpcm.speakerPin = 9;
  
  gyro.enableAutoRange(true);
  if(!gyro.begin(GYRO_RANGE_2000DPS))
  {
    Serial.println("Error connecting to gyro");
    while(1);
  }

  if(!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD fail");
    while(1);
  }
  tmrpcm.setVolume(6);
  delay(1000);
}

void loop() {
  sensors_event_t event; 
  gyro.getEvent(&event);
 
  if (!tmrpcm.isPlaying()) {
    tmrpcm.play("opening4.wav");
  }
  
  if (checkRotationalThreshold(rotationalThreshold, event.gyro.z)) {
    //The event.gyro.z is the rotational speed of the gyro at this moment in Radians per second
    //In order to ROUGHLY calculate the angle change during this time frame, we divide by our measurementsPerSecond then multiply by 57.2958 (an approximation of degrees per radian)
    //This will result in drift over time, but the angle will be reset everytime the box is closed.
    angle += event.gyro.z / measurementsPerSecond * 57.2958;
  }

  if (angle > 100) {
    tmrpcm.play("opened3.wav");
  }
  
  Serial.print("Z-Rotation: "); Serial.print(event.gyro.z); Serial.print("; Angle: "); Serial.println(angle);
  delay(1000 / measurementsPerSecond);
}

bool checkRotationalThreshold(float threshold, float rotation) {
  return (rotation > threshold || rotation < (-1 * threshold));
}
