#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_L3GD20_U.h>

Adafruit_L3GD20_Unified gyro = Adafruit_L3GD20_Unified(20);
float angle;
float measurementsPerSecond = 10;
float rotationalThreshold = .05;

void setup() {
  Serial.begin(9600);
  angle = 0;
  
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
 
  if (checkRotationalThreshold(rotationalThreshold, event.gyro.z)) {
    //The event.gyro.z is the rotational speed of the gyro at this moment in Radians per second
    //In order to ROUGHLY calculate the angle change during this time frame, we divide by our measurementsPerSecond then multiply by 57.2958 (an approximation of degrees per radian)
    //This will result in drift over time, but the angle will be reset everytime the box is closed.
    angle += event.gyro.z / measurementsPerSecond * 57.2958;
  }
  
  Serial.print("Z-Rotation: "); Serial.print(event.gyro.z); Serial.print("; Angle: "); Serial.println(angle);
  delay(1000 / measurementsPerSecond);
}

bool checkRotationalThreshold(float threshold, float rotation) {
  return (rotation > threshold || rotation < (-1 * threshold));
}
