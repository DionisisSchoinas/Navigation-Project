#include "Arduino.h"

#include <LovyanGFX.hpp>
#include "pins.h"

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_GC9A01 _panel_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Bus_SPI _bus_instance;

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

        // cfg.invert = true;          // As a result, the brightness and darkness of the face plate is reversed, and the setting is true

        cfg.rgb_order = RGB_ORDER;  // As a result, the red color and the blue color are replaced on the face plate, and the setting is true
        cfg.dlen_16bit = false;     // From 16th position to 16th position, the length of the number of transfers is set to true
        cfg.bus_shared = false;     // How to use drawJpgFile (e.g. summary control)

        _panel_instance.config(cfg);
      }

      { // Set backlight control. (delete if not necessary)

        auto cfg = _light_instance.config(); // Get the structure for backlight configuration.

        cfg.pin_bl = BL;     // pin number to which the backlight is connected
        cfg.freq = 44100;    // backlight PWM frequency
        cfg.pwm_channel = 1; // PWM channel number to use

        // cfg.invert = true;

        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance); // Sets the backlight to the panel.
      }

      setPanel(&_panel_instance); // Set the panel to use.
    }
};