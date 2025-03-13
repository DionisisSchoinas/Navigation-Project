#include <Arduino.h>
#include <AppHelper.cpp>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BACKGROUND_COLOR TFT_BLACK
#define WRITE_COLOR TFT_WHITE

void connectedLoop(void);
void clearDisplay(void);
void drawBluetooth(void);
void drawBluetoothLoading(uint8_t loadingWheelThickness);

BLEServer *pServer = NULL;
BLECharacteristic *batteryCharacteristic = NULL;
BLECharacteristic *routesCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID                  "52786979-1178-4265-8158-b5b72da65854"
#define BATTERY_CHARACTERISTIC_UUID   "66f8f28a-974e-48ab-b6ee-ad590651899d"
#define ROUTES_CHARACTERISTIC_UUID    "8f7b170e-1bee-4af7-9fc3-127d6548f0ee"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

class CharacteristicCallBack : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic_) override
  {
    clearDisplay();
    _lcd.drawCenterString(characteristic_->getValue().c_str(), screenHalfWidth, screenHalfHeight);
  }
};

void setup() {
  helper_setup();

  // Create the BLE Device
  BLEDevice::init("Navigation ESP32-S3");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  batteryCharacteristic = pService->createCharacteristic(BATTERY_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  batteryCharacteristic->addDescriptor(new BLE2902());

  routesCharacteristic = pService->createCharacteristic(ROUTES_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  routesCharacteristic->addDescriptor(new BLE2902());
  routesCharacteristic->setCallbacks(new CharacteristicCallBack());


  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();

  helper_loop();
  clearDisplay();
  drawBluetooth();
}

void loop() {
  helper_loop();

  // notify changed value
  if (deviceConnected) {
    if (!oldDeviceConnected) {
      /**
       * CONNECTING
       */
      oldDeviceConnected = true;

      clearDisplay();
      _lcd.setTextSize(10);
      _lcd.drawCenterString("Hi!", screenHalfWidth, screenHalfHeight);
    }

    connectedLoop();
  } else if (oldDeviceConnected) {
    /**
     * DISCONNECTED
     */
    delay(500);                   // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();  // restart advertising
    oldDeviceConnected = false;
    
    clearDisplay();
    drawBluetooth();
  }

  if (!oldDeviceConnected) {
    drawBluetoothLoading(3);
  }
}

uint32_t millisBetweenCharacteristicsUpdate = 10000;
uint32_t lastCharacteristicUpdate = -millisBetweenCharacteristicsUpdate;

void connectedLoop() {
  if (millis() - lastCharacteristicUpdate >= millisBetweenCharacteristicsUpdate) {
    batteryCharacteristic->setValue((uint8_t *)&value, 4);
    batteryCharacteristic->indicate();
    value++;
    lastCharacteristicUpdate = millis();
  }
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