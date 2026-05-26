#include <Arduino.h>
#include <LIS3DH.h>
#include <Wire.h>

#include "vectorTransform.h"
#include "vector.h"

#define LED_MATRIX_Size 7

Vector calibrateSensor(uint16_t cycles);
Vector readSensorDataCalibrated(Vector calibration);
static float mapAxisQuadratic(float v);
void setMatrixByAcceleration(Vector acc);

LIS3DH lis3dh;
static Vector calibration;
static bool ledMatrix[LED_MATRIX_Size][LED_MATRIX_Size] = {0};

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  delay(200);
  Serial.println("Serial connected!");

  pinMode(25, OUTPUT);
  digitalWrite(25, HIGH);
  Wire.begin(21, 22);

  delay(200);

  while (!lis3dh.begin())
  {
    delay(1000);
  }

  calibration = calibrateSensor(100);
}

void loop()
{
  Vector calibratedAcc = readSensorDataCalibrated(calibration);
  setMatrixByAcceleration(calibratedAcc);

  for (int y = 0; y < LED_MATRIX_Size; y++)
  {
    for (int x = 0; x < LED_MATRIX_Size; x++)
    {
      if (ledMatrix[y][x])
      {
        Serial.print("■ "); // true = black square
      }
      else
      {
        Serial.print("□ "); // false = white square
      }
    }
    Serial.println();
  }

  Serial.print("X: ");
  Serial.print(calibratedAcc.x, 3);

  Serial.print(" | Y: ");
  Serial.print(calibratedAcc.y, 3);

  Serial.print(" | Z: ");
  Serial.println(calibratedAcc.z, 3);

  Serial.println();

  delay(300);
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

static float mapAxisQuadratic(float v)
{
  // Acceleration needed to move one LED near the center.
  constexpr float gPerLed = 2.0f;

  float sign = (v >= 0.0f) ? 1.0f : -1.0f;
  float a = fabs(v);

  // Quadratic distance mapping:
  // small accelerations move less, large accelerations move more.
  return sign * ((a * a) / (gPerLed * gPerLed));
}

void setMatrixByAcceleration(Vector acc)
{
  const int size = LED_MATRIX_Size;
  const int center = size / 2;

  for (int y = 0; y < size; y++)
  {
    for (int x = 0; x < size; x++)
    {
      ledMatrix[y][x] = false;
    }
  }

  int ledX = center + (int)round(mapAxisQuadratic(acc.x));
  int ledY = center - (int)round(mapAxisQuadratic(acc.y));

  if (ledX < 0)
    ledX = 0;
  if (ledX >= size)
    ledX = size - 1;

  if (ledY < 0)
    ledY = 0;
  if (ledY >= size)
    ledY = size - 1;

  ledMatrix[ledY][ledX] = true;
}