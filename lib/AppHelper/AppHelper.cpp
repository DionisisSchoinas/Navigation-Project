#define LGFX_USE_V1

#include "Arduino.h"
#include "AppHelper.h"

#include <LovyanGFX.hpp>
#include <lvgl.h>
#include <Preferences.h>

#include "pins.h"
#include "tone.h"

#define FLASH FFat
#define F_NAME "FATFS"
#define buf_size 10

class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Touch_CST816S _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();

      // SPI bus settings
      cfg.spi_host = SPI; // Select the SPI to use ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
      // * Due to the ESP-IDF version upgrade, VSPI_HOST, The HSPI_HOST specification is deprecated, so if you get an error, use SPI2_HOST or SPI3_HOST instead.
      cfg.spi_mode = 0;                       // Set SPI communication mode (0 ~ 3)
      cfg.freq_write = 80000000;              // SPI time (up to 80MHz, four or five inputs divided by 80MHz to get an integer)
      cfg.freq_read = 20000000;               // SPI time when connected cfg.spi_3wire = true; // Set true if receiving is done via MOSI pin
      cfg.use_lock = true;                    // Usage lock time setting true
      cfg.dma_channel = SPI_DMA_CH_AUTO;      // Set the DMA channel to use (0=DMA not used / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=automatic setting) // * Due to the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting) is now recommended for the DMA channel. Specifying 1ch or 2ch is no longer recommended.
      cfg.pin_sclk = SCLK;                    // Set the SPI SCLK pin number
      cfg.pin_mosi = MOSI;                    // Set the SPI CLK pin number
      cfg.pin_miso = MISO;                    // Set the SPI MISO pin number (-1 = disable)
      cfg.pin_dc = DC;                        // Set the SPI D/C pin number (-1 = disable)
      _bus_instance.config(cfg);              // Reflect the setting value to the bus.
      _panel_instance.setBus(&_bus_instance); // Set the bus to the panel.
    }

    {                                      // Set the display panel control.
      auto cfg = _panel_instance.config(); // Get the structure for display panel settings.

      cfg.pin_cs = CS;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = RST; // Pin number to which RST is connected (-1 = disable)
      cfg.pin_busy = -1; // Pin number to which BUSY is connected (-1 = disable)

      /* The following settings are set to general initial values ​​for each panel, so try commenting out any items you are unsure of. */

      cfg.memory_width = WIDTH;   // Maximum width supported by driver IC
      cfg.memory_height = HEIGHT; // Maximum height supported by driver IC
      cfg.panel_width = WIDTH;    // Actual displayable width
      cfg.panel_height = HEIGHT;  // Actual displayable height
      cfg.offset_x = OFFSET_X;    // Panel offset in X direction
      cfg.offset_y = OFFSET_Y;    // Panel offset in Y direction
      cfg.offset_rotation = 0;    // Value 0~7 in rotation direction (4~7 is inverted)
      cfg.dummy_read_pixel = 8;   // Virtual number of positions read before reading image
      cfg.dummy_read_bits = 1;    // The number of imaginary words other than the image element
      cfg.readable = false;       // As long as the number of acquisitions is as high as possible, the setting is true
      cfg.invert = true;          // As a result, the brightness and darkness of the face plate is reversed, and the setting is true
      cfg.rgb_order = RGB_ORDER;  // As a result, the red color and the blue color are replaced on the face plate, and the setting is true
      cfg.dlen_16bit = false;     // From 16th position to 16th position, the length of the number of transfers is set to true
      cfg.bus_shared = false;     // How to use drawJpgFile (e.g. summary control)

      _panel_instance.config(cfg);
    }

    { // Set backlight control. (delete if not necessary)

      auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

      cfg.pin_bl = BL;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 1; // PWM channel number to use

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
    }

    { // Sets touchscreen control. (Delete if not needed)

      auto cfg = _touch_instance.config();
      cfg.x_min = 0;        // Minimum X value obtained from touch screen (raw value)
      cfg.x_max = WIDTH;    // Maximum X value obtained from touch screen (raw value)
      cfg.y_min = 0;        // Minimum Y value obtained from touch screen (raw value)
      cfg.y_max = HEIGHT;   // Maximum Y value obtained from touch screen (raw value)
      cfg.pin_int = TP_INT; // Pin number to which INT is connected
      cfg.pin_rst = TP_RST;
      cfg.bus_shared = true;   // Set true if using a common bus with the screen
      cfg.offset_rotation = 0; // Adjust if display and touch orientation do not match. Set to a value between 0 and 7

      cfg.i2c_port = 0;      // Select the I2C to use (0 or 1)
      cfg.i2c_addr = 0x15;   // I2C device address number
      cfg.pin_sda = I2C_SDA; // Pin number to which SDA is connected
      cfg.pin_scl = I2C_SCL; // Pin number to which SCL is connected
      cfg.freq = 400000;     // Set the I2C clock
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance); // Set the touch screen to the panel.
    }

    setPanel(&_panel_instance); // Set the panel to use.
  }
};

LGFX tft;

Preferences prefs;

static const uint32_t screenWidth = WIDTH;
static const uint32_t screenHeight = HEIGHT;

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

  if (tft.getStartCount() == 0)
  {
    tft.endWrite();
  }

  tft.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t *)data);
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

  tft.init();
  tft.initDMA();
  tft.startWrite();
  tft.fillScreen(TFT_BLUE);
  tft.setRotation(prefs.getInt("rotate", 0));

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
  screenBrightness(prefs.getInt("brightness", 100));

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

void screenBrightness(uint8_t value)
{
  tft.setBrightness(value);
  #ifdef ELECROW_C3
    set_pin_io(2, value > 0); // ELECROW C3, no brightness control
  #endif
}