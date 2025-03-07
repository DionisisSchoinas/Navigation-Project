#define LGFX_USE_V1

#include "Arduino.h"

#include "LGFX.cpp"
#include "pins.h"
#include "tone.h"

#include <LovyanGFX.hpp>
#include <lvgl.h>
#include <Preferences.h>

#define FLASH FFat
#define F_NAME "FATFS"
#define buf_size 10

LGFX _lcd;

Preferences prefs;

static const uint32_t screenWidth = WIDTH;
static const uint32_t screenHeight = HEIGHT;

static const uint32_t screenHalfWidth = WIDTH/2;
static const uint32_t screenHalfHeight = HEIGHT/2;

const unsigned int lvBufferSize = screenWidth * buf_size;
uint8_t lvBuffer[2][lvBufferSize];

lv_obj_t *lastActScr;

// bool circular = false;
bool alertSwitch = false;
bool gameActive = false;
bool readIMU = false;
bool updateSeconds = false;
bool hasUpdatedSec = false;
bool navSwitch = false;

String customFacePaths[15];
int customFaceIndex;
static bool transfer = false;

TaskHandle_t gameHandle = NULL;

void helper_setup(void);
void helper_loop(void);
void toneOut(int pitch, int duration);
void screenBrightness(uint8_t value);

void update_faces();
void updateQrLinks();

void flashDrive_cb(lv_event_t *e);
void driveList_cb(lv_event_t *e);

void checkLocal(bool faces = false);
void registerWatchface_cb(const char *name, const lv_image_dsc_t *preview, lv_obj_t **watchface, lv_obj_t **second);
void registerCustomFace(const char *name, const lv_image_dsc_t *preview, lv_obj_t **watchface, String path);

String hexString(uint8_t *arr, size_t len, bool caps = false, String separator = "");

bool loadCustomFace(String file);
bool deleteCustomFace(String file);
bool readDialBytes(const char *path, uint8_t *data, size_t offset, size_t size);
bool isKnown(uint8_t id);
void parseDial(const char *path, bool restart = false);
bool lvImgHeader(uint8_t *byteArray, uint8_t cf, uint16_t w, uint16_t h, uint16_t stride);

void screenBrightness(uint8_t value);

/* Display flushing */
void my_disp_flush(lv_display_t *display, const lv_area_t *area, unsigned char *data)
{

  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  lv_draw_sw_rgb565_swap(data, w * h);

  if (_lcd.getStartCount() == 0)
  {
    _lcd.endWrite();
  }

  _lcd.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)data);
  lv_display_flush_ready(display); /* tell lvgl that flushing is done */
}

#ifdef ELECROW_C3
// ELECROW C3 I2C IO extender
#define PI4IO_I2C_ADDR 0x43

// Extended IO function
void init_IO_extender()
{
  Wire.beginTransmission(PI4IO_I2C_ADDR);
  Wire.write(0x01); // test register
  Wire.endTransmission();
  Wire.requestFrom(PI4IO_I2C_ADDR, 1);
  uint8_t rxdata = Wire.read();
  Serial.print("Device ID: ");
  Serial.println(rxdata, HEX);

  Wire.beginTransmission(PI4IO_I2C_ADDR);
  Wire.write(0x03);                                                 // IO direction register
  Wire.write((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4)); // set pins 0, 1, 2 as outputs
  Wire.endTransmission();

  Wire.beginTransmission(PI4IO_I2C_ADDR);
  Wire.write(0x07);                                                    // Output Hi-Z register
  Wire.write(~((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4))); // set pins 0, 1, 2 low
  Wire.endTransmission();
}

void set_pin_io(uint8_t pin_number, bool value)
{

  Wire.beginTransmission(PI4IO_I2C_ADDR);
  Wire.write(0x05); // test register
  Wire.endTransmission();
  Wire.requestFrom(PI4IO_I2C_ADDR, 1);
  uint8_t rxdata = Wire.read();
  Serial.print("Before the change: ");
  Serial.println(rxdata, HEX);

  Wire.beginTransmission(PI4IO_I2C_ADDR);
  Wire.write(0x05); // Output register

  if (!value)
    Wire.write((~(1 << pin_number)) & rxdata); // set pin low
  else
    Wire.write((1 << pin_number) | rxdata); // set pin high
  Wire.endTransmission();

  Wire.beginTransmission(PI4IO_I2C_ADDR);
  Wire.write(0x05); // test register
  Wire.endTransmission();
  Wire.requestFrom(PI4IO_I2C_ADDR, 1);
  rxdata = Wire.read();
  Serial.print("after the change: ");
  Serial.println(rxdata, HEX);
}
#endif

#ifdef ENABLE_RTC
bool wasError(const char *errorTopic = "")
{
  uint8_t error = Rtc.LastError();
  if (error != 0)
  {
    // we have a communications error
    // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
    // for what the number means
    Serial.print("[");
    Serial.print(errorTopic);
    Serial.print("] WIRE communications error (");
    Serial.print(error);
    Serial.print(") : ");

    switch (error)
    {
    case Rtc_Wire_Error_None:
      Serial.println("(none?!)");
      break;
    case Rtc_Wire_Error_TxBufferOverflow:
      Serial.println("transmit buffer overflow");
      break;
    case Rtc_Wire_Error_NoAddressableDevice:
      Serial.println("no device responded");
      break;
    case Rtc_Wire_Error_UnsupportedRequest:
      Serial.println("device doesn't support request");
      break;
    case Rtc_Wire_Error_Unspecific:
      Serial.println("unspecified error");
      break;
    case Rtc_Wire_Error_CommunicationTimeout:
      Serial.println("communications timed out");
      break;
    }
    return true;
  }
  return false;
}
#endif

static uint32_t my_tick(void)
{
  return millis();
}

void toneOut(int pitch, int duration)
{ // pitch in Hz, duration in ms
#if defined(BUZZER) && (BUZZER != -1)
  int delayPeriod;
  long cycles, i;

  pinMode(BUZZER, OUTPUT);                        // turn on output pin
  delayPeriod = (500000 / pitch) - 7;             // calc 1/2 period in us -7 for overhead
  cycles = ((long)pitch * (long)duration) / 1000; // calc. number of cycles for loop

  for (i = 0; i <= cycles; i++)
  { // play note for duration ms
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(delayPeriod);
    digitalWrite(BUZZER, LOW);
    delayMicroseconds(delayPeriod - 1); // - 1 to make up for digitaWrite overhead
  }
  pinMode(BUZZER, INPUT); // shut off pin to avoid noise from other operations
#endif
}

/**
* Screen brightness range [0,255]
*/
void screenBrightness(uint8_t value)
{
  if (value > 255) {
    _lcd.setBrightness(255);
    return;
  }
  if (value < 0) {
    _lcd.setBrightness(0);
    return;
  }
  _lcd.setBrightness(value);
}

void drawline(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t linePixelWidth) {
  // _lcd.drawWideLine
}

void helper_setup()
{
  prefs.begin("my-app");

  #ifdef ELECROW_C3
    Wire.begin(4, 5);
    init_IO_extender();
    delay(100);
    set_pin_io(2, true);
    set_pin_io(3, true);
    set_pin_io(4, true);
  #endif

  _lcd.init();
  _lcd.initDMA();
  _lcd.startWrite();
  _lcd.setRotation(prefs.getInt("rotate", 0));
  _lcd.setRawColor(TFT_BLACK);
  _lcd.endWrite();

  toneOut(TONE_EN * 2, 170);
  toneOut(TONE_FS * 2, 170);
  toneOut(TONE_GN * 2, 170);

  lv_init();

  lv_tick_set_cb(my_tick);

  static auto *lvDisplay = lv_display_create(screenWidth, screenHeight);
  lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(lvDisplay, my_disp_flush);

  lv_display_set_buffers(lvDisplay, lvBuffer[0], lvBuffer[1], lvBufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // load saved preferences
  screenBrightness(prefs.getInt("brightness", 150));

  if (lv_fs_is_ready('S'))
  {
    Serial.println("Drive S is ready");
  }
  else
  {
    Serial.println("Drive S is not ready");
  }

  lv_rand_set_seed(my_tick());
}

void helper_loop()
{
    lv_timer_handler(); // Update the UI
    delay(5);
}