#include <Tone.h>
#include "SD.h"
#define SD_ChipSelectPin 10
#include "TMRpcm.h"
#include "SPI.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>

#define SD_ChipSelectPin 10
#define speakerPin 9
#define xModifier 1
#define yModifier 1
#define zModifier .5

int scale[] = {NOTE_B2, NOTE_CS3, NOTE_D3, NOTE_E3, NOTE_FS3, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_CS4, NOTE_D4, NOTE_E4, NOTE_FS4, NOTE_G4, NOTE_A4,NOTE_B4, NOTE_CS5, NOTE_D5, NOTE_E5, NOTE_FS5, NOTE_G5, NOTE_A5,NOTE_B5, NOTE_CS6, NOTE_D6, NOTE_E6, NOTE_FS6, NOTE_G6, NOTE_A6,NOTE_B6, NOTE_CS7, NOTE_D7, NOTE_E7, NOTE_FS7, NOTE_G7, NOTE_A7,NOTE_B7, NOTE_CS8, NOTE_D8 };
int timeSignatures[] = { 1, 2, 3, 4, 6, 8, 9, 12, 16 };

Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
float xAngle;
float yAngle;
float zAngle;
float measurementsPerSecond = 50;
float rotationalThreshold = .05;
int delaySeconds = 0;
int baseDelay = 1000;
TMRpcm tmrpcm;
Tone musicTone;

void setup() {
  Serial.begin(9600);
  
  musicTone.begin(speakerPin); 
  
  xAngle = 210;
  yAngle = 100;
  zAngle = 200;
  
  gyro.enableAutoRange(true);
  if(!gyro.begin(GYRO_RANGE_2000DPS))
  {
    Serial.println("Error connecting to gyro");
    while(1);
  }
  delay(1000);
}

void loop() {
    sensors_event_t event; 
    gyro.getEvent(&event);
    
    if (checkRotationalThreshold(rotationalThreshold, event.gyro.x)) {
      xAngle += (event.gyro.x / measurementsPerSecond * 57.2958) * xModifier;
      if (xAngle < 0) {
        xAngle = 0;
      }
    }
    if (checkRotationalThreshold(rotationalThreshold, event.gyro.y)) {
      yAngle += (event.gyro.y / measurementsPerSecond * 57.2958) * yModifier;
      if (yAngle < 50) {
        yAngle = 50;
      }
    }
    if (checkRotationalThreshold(rotationalThreshold, event.gyro.z)) {
      zAngle += (event.gyro.z / measurementsPerSecond * 57.2958) * zModifier;
      if (zAngle < 0) {
        zAngle = 0;
      }
    }

    if (delaySeconds <= 0) {
      int scaleAccessor = xAngle / 30;
      if (scaleAccessor > 37) {
        scaleAccessor = 37;
      }
      musicTone.play(scale[scaleAccessor], (yAngle));
      
      int scaleIndex = floor(zAngle / 45);
      if (scaleIndex > 8) {
        scaleIndex = 8;
      }
      delaySeconds = baseDelay / timeSignatures[scaleIndex];
    }
    else {
      delaySeconds -= (1000 / measurementsPerSecond);
    }
    delay(1000 / measurementsPerSecond);
}

bool checkRotationalThreshold(float threshold, float rotation) {
  return (rotation > threshold || rotation < (-1 * threshold));
}
