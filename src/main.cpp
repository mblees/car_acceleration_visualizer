#include <Arduino.h>
#include <LIS3DH.h>
#include <Wire.h>

struct Vector
{
  float x;
  float y;
  float z;
};

Vector calibrateSensor(uint16_t cycles);
Vector readSensorDataCalibrated(Vector calibration);

LIS3DH lis3dh;
static Vector calibration;

void setup()
{
  Serial.begin(9600);
  // Wait briefly for serial monitor to attach on some boards
  while (!Serial)
    ;
  delay(200);
  Serial.println("Serial connected!");

  Wire.begin(21, 22);

  if (!lis3dh.begin())
  {
    Serial.println("LIS3DH not found. Check wiring.");
    while (1)
      delay(1000);
  }

  calibration = calibrateSensor(100);
}

void loop()
{
  Vector calibratedAcc = readSensorDataCalibrated(calibration);

  Serial.print(" | Calibrated -> X: ");
  Serial.print(calibratedAcc.x);
  Serial.print(" Y: ");
  Serial.print(calibratedAcc.y);
  Serial.print(" Z: ");
  Serial.print(calibratedAcc.z);

  delay(250);
}

Vector calibrateSensor(uint16_t cycles)
{
  float x, y, z;
  float av_x = 0.0f;
  float av_y = 0.0f;
  float av_z = 0.0f;

  for (uint16_t i = 0; i < cycles; i++)
  {
    lis3dh.readAcceleration(x, y, z);

    av_x += x;
    av_y += y;
    av_z += z;
  }

  Vector result;
  result.x = av_x / cycles;
  result.y = av_y / cycles;
  result.z = av_z / cycles;
  return result;
}

Vector readSensorDataCalibrated(Vector calibration)
{
  float x, y, z;
  lis3dh.readAcceleration(x, y, z);

  Vector result;
  return result;
}
