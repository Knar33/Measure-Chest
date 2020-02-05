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
bool opened = false;
bool opening = false;
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
    
    if (checkRotationalThreshold(rotationalThreshold, event.gyro.x)) {
      //The event.gyro.x is the rotational speed of the gyro at this moment in Radians per second
      //In order to ROUGHLY calculate the angle change during this time frame, we divide by our measurementsPerSecond then multiply by 57.2958 (an approximation of degrees per radian)
      //This will result in drift over time, but the angle will be reset everytime the box is closed.
      angle += event.gyro.x / measurementsPerSecond * 57.2958;
    }
    
    if (angle < 5 && opening) {
      angle = 0;
      opening = false;
      opened = false;
      tmrpcm.stopPlayback(); 
    }
    if (angle > 5 && !opening) {
      opening = true;
      tmrpcm.play("opening5.wav");
    }
  
    if (angle > 100 && !opened) {
      opened = true;
      tmrpcm.play("opened3.wav");
    }

    if (opened && !tmrpcm.isPlaying()) {
      tmrpcm.disable();
      tmrpcm.stopPlayback(); 
    }
    
    delay(1000 / measurementsPerSecond);
}

bool checkRotationalThreshold(float threshold, float rotation) {
  return (rotation > threshold || rotation < (-1 * threshold));
}
