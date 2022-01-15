#include "Log.h"
#include "Mousr.h"
#include "utility.h"

#include <Wire.h>
//#include <SPI.h>
#include <U8g2lib.h>

// BLE includes
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// seesaw includes
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <Adafruit_seesaw.h>

// OLED views
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8_status = U8X8_SSD1306_128X64_NONAME_HW_I2C(U8X8_PIN_NONE); // Status mode
