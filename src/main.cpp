#include <Arduino.h>
#include <AppHelper.cpp>

#define BACKGROUND_COLOR TFT_BLACK
#define WRITE_COLOR TFT_WHITE

void clearDisplay(void);
void drawBluetooth();
void drawBluetoothLoading(uint8_t loadingWheelThickness);

void setup() {
  helper_setup();

  helper_loop();
  clearDisplay();
  drawBluetooth();
}

void loop() {
  helper_loop();

  drawBluetoothLoading(3);
}

void clearDisplay() {
  _lcd.fillScreen(BACKGROUND_COLOR);
  _lcd.setColor(WRITE_COLOR);
}

uint8_t bluetoothMargin = 80;
uint8_t halfBluetoothHeight = (HEIGHT-bluetoothMargin*2)/2;
uint8_t quarterBluetoothHeight = (HEIGHT-bluetoothMargin*2)/4;
float loadingAngle = 0;
boolean clearColor = false;

/**
 * Draw bluetooth symbol
 * 
 * @param loadingWheelThickness the thickness of the loading wheel
 */
void drawBluetooth() {
  _lcd.drawWideLine(screenHalfWidth, bluetoothMargin, screenHalfWidth, HEIGHT-bluetoothMargin, 3, WRITE_COLOR);
  _lcd.drawWideLine(screenHalfWidth-quarterBluetoothHeight, bluetoothMargin+quarterBluetoothHeight, screenHalfWidth+quarterBluetoothHeight, HEIGHT-bluetoothMargin-quarterBluetoothHeight, 3, WRITE_COLOR);
  _lcd.drawWideLine(screenHalfWidth-quarterBluetoothHeight, HEIGHT-bluetoothMargin-quarterBluetoothHeight, screenHalfWidth+quarterBluetoothHeight, bluetoothMargin+quarterBluetoothHeight, 3, WRITE_COLOR);
  _lcd.drawWideLine(screenHalfWidth, bluetoothMargin, screenHalfWidth+quarterBluetoothHeight, bluetoothMargin+quarterBluetoothHeight, 3, WRITE_COLOR);
  _lcd.drawWideLine(screenHalfWidth, HEIGHT-bluetoothMargin, screenHalfWidth+quarterBluetoothHeight, HEIGHT-bluetoothMargin-quarterBluetoothHeight, 3, WRITE_COLOR);
}

/**
 * Draw loading circle over bluetooth symbol
 */
void drawBluetoothLoading(uint8_t loadingWheelThickness) {
  if (clearColor) {
    _lcd.setColor(BACKGROUND_COLOR);
  } else {
    _lcd.setColor(WRITE_COLOR);
  }

  for (uint8_t i=0; i<loadingWheelThickness; i++) {
    _lcd.drawArc(screenHalfWidth, screenHalfHeight, halfBluetoothHeight+19+i, halfBluetoothHeight+19+i, 0, loadingAngle);
  }
  
  loadingAngle += 4;
  if (loadingAngle > 360) {
    clearColor = !clearColor;
    loadingAngle = 0;
  }
}