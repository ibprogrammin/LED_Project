#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>

class Settings {
  public:
    Settings();
    void load();
    void save();
    int getBrightness();
    void setBrightness(int brightness);
    int getTimeout();
    void setTimeout(int timeout);

  private:
    struct SettingsData {
      int brightness;
      int timeout;
    };

    SettingsData data;
    const int EEPROM_ADDRESS = 0;
};

#endif  // SETTINGS_H