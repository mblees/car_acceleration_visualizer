#include <Arduino.h>
#include <LIS3DH.h>
#include <Wire.h>

LIS3DH lis3dh;

void setup()
{
  Serial.begin(9600);
  // Wait briefly for serial monitor to attach on some boards
  while (!Serial);
  delay(200);
  Serial.println("Serial connected!");
  
  Wire.begin(21, 22);

  if (!lis3dh.begin())
  {
    Serial.println("LIS3DH not found. Check wiring.");
    while (1)
      delay(1000);
  }
}

void loop()
{
  float x, y, z;
  lis3dh.readAcceleration(x, y, z);

  float rel = lis3dh.getRelativeAcceleration();
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.print(" | Relative: ");
  Serial.print(rel);
  Serial.println(" m/s^2");

  if (lis3dh.isMoving(2.0))
  {
    Serial.println("Device is moving (threshold 2.0 m/s^2)");
  }

  delay(250);
}