
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdbool.h"
#include "screens.h"


typedef enum {
    UnitsKph, UnitsMph, UnitsCelsius, UnitsFahrenheit
 } UnitsT;

typedef struct ConfigSettings {
  UnitsT SpeedUnits;
  UnitsT TempUnits;
  uint16_t WheelCirc; // millimeters
  float GearRatio;
  IdScreenT Screen1;
  IdScreenT Screen2;
  IdScreenT Screen3;
  IdScreenT Screen4;
  bool HallOnWheel;
  bool ShowLogo;
} ConfigSettingsT;


extern ConfigSettingsT config;


#endif /* _SETTINGS_H_ */