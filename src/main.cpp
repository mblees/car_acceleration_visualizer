#include <Arduino.h>
#include <LIS3DH.h>
#include <Wire.h>

#include "vectorTransform.h"
#include "vector.h"

#define LED_MATRIX_ROWS 3
#define LED_MATRIX_COLUMNS 3

Vector calibrateSensor(uint16_t cycles);
Vector readSensorDataCalibrated(Vector calibration);

LIS3DH lis3dh;
static Vector calibration;
static bool ledMatrix[LED_MATRIX_ROWS][LED_MATRIX_COLUMNS];

void setup()
{
  Serial.begin(9600);
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
  Serial.println(calibratedAcc.z);

  delay(50);
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
  Vector data;
  lis3dh.readAcceleration(data.x, data.y, data.z);

  return getTransformedVector(calibration, data);
}
