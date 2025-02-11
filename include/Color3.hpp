#pragma once
#include <math.h>
#include <Adafruit_TCS34725.h>
#include <stdint.h>

class Color3
{
public:
  Color3() {}
  Color3(uint16_t r, uint16_t g, uint16_t b, uint16_t c)
  {
    this->r = r;
    this->g = g;
    this->b = b;
    this->clear_channel = c;
  }

  void from_tcs(Adafruit_TCS34725 &dev)
  {
    // dev.begin();
    dev.enable();
    dev.getRawData(&this->r, &this->g, &this->b, &this->clear_channel);
  }

  uint16_t getLux()
  {
    float illuminance;
    illuminance = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);

    return static_cast<uint16_t>(illuminance);
  };

  uint16_t getColorTemp()
  {
    float X, Y, Z; /* RGB to XYZ correlation      */
    float xc, yc;  /* Chromaticity co-ordinates   */
    float n;       /* McCamy's formula            */
    float cct;

    if (r == 0 && g == 0 && b == 0)
    {
      return 0;
    }

    /* 1. Map RGB values to their XYZ counterparts.    */
    /* Based on 6500K fluorescent, 3000K fluorescent   */
    /* and 60W incandescent values for a wide range.   */
    /* Note: Y = Illuminance or lux                    */
    X = (-0.14282F * r) + (1.54924F * g) + (-0.95641F * b);
    Y = (-0.32466F * r) + (1.57837F * g) + (-0.73191F * b);
    Z = (-0.68202F * r) + (0.77073F * g) + (0.56332F * b);

    /* 2. Calculate the chromaticity co-ordinates      */
    xc = (X) / (X + Y + Z);
    yc = (Y) / (X + Y + Z);

    /* 3. Use McCamy's formula to determine the CCT    */
    n = (xc - 0.3320F) / (0.1858F - yc);

    /* Calculate the final CCT */
    cct = (449.0F * powf(n, 3)) + (3525.0F * powf(n, 2)) + (6823.3F * n) +
          5520.33F;

    /* Return the results in degrees Kelvin */
    return static_cast<uint16_t>(cct);
  };

  uint16_t r, g, b, clear_channel;
};