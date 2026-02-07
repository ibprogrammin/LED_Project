#include "Settings.h"

Settings::Settings() {
  load();
}

void Settings::load() {
  EEPROM.get(EEPROM_ADDRESS, data);
  
  // If settings are invalid, set defaults
  if (data.brightness < 0 || data.brightness > 255) {
    data.brightness = 128;
    data.timeout = 30;
    save();
  }
}

void Settings::save() {
  EEPROM.put(EEPROM_ADDRESS, data);
}

int Settings::getBrightness() {
  return data.brightness;
}

void Settings::setBrightness(int brightness) {
  data.brightness = brightness;
  save();
}

int Settings::getTimeout() {
  return data.timeout;
}

void Settings::setTimeout(int timeout) {
  data.timeout = timeout;
  save();
}