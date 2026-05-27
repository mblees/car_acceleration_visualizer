#include <Arduino.h>
#include <LIS3DH.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#include "vectorTransform.h"
#include "vector.h"

#define LED_MATRIX_Size 5
#define LED_PIN 26
#define LIS3DH_EN 25
#define LIS3DH_I2C_EN 33

Vector calibrateSensor(uint16_t cycles);
Vector readSensorDataCalibrated(Vector calibration);
static float mapAxisQuadratic(float v);
void setMatrixByAcceleration(Vector acc);
void setLEDMatrix(void);

LIS3DH lis3dh;
static Vector calibration;
static bool ledMatrix[LED_MATRIX_Size][LED_MATRIX_Size] = {0};
Adafruit_NeoPixel leds(LED_MATRIX_Size *LED_MATRIX_Size, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  delay(200);
  Serial.println("Serial connected!");

  leds.begin();
  leds.setBrightness(50);
  leds.setPixelColor(0, 255, 0, 0);
  leds.show();

  pinMode(LIS3DH_EN, OUTPUT);
  digitalWrite(LIS3DH_EN, HIGH);
  pinMode(LIS3DH_I2C_EN, OUTPUT);
  digitalWrite(LIS3DH_I2C_EN, LOW);

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
  setLEDMatrix();
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

void setLEDMatrix(void)
{
  for (uint8_t i = 0; i < LED_MATRIX_Size; i++)
  {
    for (uint8_t j = 0; j < LED_MATRIX_Size; j++)
    {
      if (ledMatrix[i][j])
      {
        leds.setPixelColor(5 * i + j, 0, 255, 0);
      }
      else
      {
        leds.setPixelColor(5 * i + j, 0, 0, 0);
      }
    }
  }
  leds.show();
}
