#include <Arduino.h>
#include <AppHelper.cpp>

void drawBluetooth(uint8_t loadingWheelThickness);
void drawBluetoothLoading(uint8_t loadingWheelThickness);

void setup() {
  helper_setup();
}

void loop() {
  helper_loop();
  _lcd.setColor(TFT_BLACK);

  drawBluetooth(3);
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
void drawBluetooth(uint8_t loadingWheelThickness) {
  _lcd.drawWideLine(screenHalfWidth, bluetoothMargin, screenHalfWidth, HEIGHT-bluetoothMargin, 3, TFT_BLACK);
  _lcd.drawWideLine(screenHalfWidth-quarterBluetoothHeight, bluetoothMargin+quarterBluetoothHeight, screenHalfWidth+quarterBluetoothHeight, HEIGHT-bluetoothMargin-quarterBluetoothHeight, 3, TFT_BLACK);
  _lcd.drawWideLine(screenHalfWidth-quarterBluetoothHeight, HEIGHT-bluetoothMargin-quarterBluetoothHeight, screenHalfWidth+quarterBluetoothHeight, bluetoothMargin+quarterBluetoothHeight, 3, TFT_BLACK);
  _lcd.drawWideLine(screenHalfWidth, bluetoothMargin, screenHalfWidth+quarterBluetoothHeight, bluetoothMargin+quarterBluetoothHeight, 3, TFT_BLACK);
  _lcd.drawWideLine(screenHalfWidth, HEIGHT-bluetoothMargin, screenHalfWidth+quarterBluetoothHeight, HEIGHT-bluetoothMargin-quarterBluetoothHeight, 3, TFT_BLACK);
  drawBluetoothLoading(loadingWheelThickness);
}

/**
 * Draw loading circle over bluetooth symbol
 */
void drawBluetoothLoading(uint8_t loadingWheelThickness) {
  if (clearColor) {
    _lcd.setColor(TFT_WHITE);
  } else {
    _lcd.setColor(TFT_BLACK);
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