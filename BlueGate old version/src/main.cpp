//libraries
#include <Arduino.h>
 #include <BLEDevice.h>
 #include <BLEUtils.h>
 #include <BLEServer.h>
 #include <BLE2902.h>
 #include <Wire.h>               // Library to use I2C to display
 #include "SSD1306Wire.h"      // Display library
 #include "esp_gatts_api.h"
#define SERVICE_UUID        "ba10"
#define MAC_UUID            "ba20"
#define Password_UUID       "ba30"
#define Admin_UUID          "ba40"
#define Type_UUID           "ba50"
BLECharacteristic* MAC       = NULL;
BLECharacteristic* Password  = NULL;
BLECharacteristic* Admin     = NULL;
BLECharacteristic* Type      = NULL;
bool deviceConnected         = false;
bool oldDeviceConnected      = false;

const std::string serialKey = "060593";
static std::string key = serialKey; 
bool serialKeyCheck = false;
BLEServer *pServer;
int id;
String mac;

// Display and Scan activities
SSD1306Wire  display(0x3c, 5, 4);

void showit(String text){
  display.clear();
  delay(200);
  // display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  // clear the display
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 120, text);
  display.display();
}


class Check: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic1) {
      std::string rxValue = pCharacteristic1->getValue();
        display.init();
        Serial.println("Checking");
        if(rxValue==key){
            //Password->setValue("");
            Serial.println("Password is correct");
            showit("Password is correct");
            digitalWrite(16, LOW);
            delay(1000);
            Serial.println("Door opened");
            showit("Door opened");
            delay(100);
        }
        else{
            Serial.println("Password is incorrect");
            showit("Password is incorrect");
        }
      //Password->setValue("");
      Serial.println();
      delay(800);
      //mac+="is mac address of a device now i am trying to display some random stuff to generate long text which can be seen from the display of the esp32";      
      showit(mac);
      //pServer->removePeerDevice(id, false);
      //Serial.println("Disconnected");
      digitalWrite(16, HIGH);
      delay(500);
      showit("Connected Home");   
    }
};


class GodMode: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic1) {
      std::string rxValue = pCharacteristic1->getValue();
      display.init();
      for(int i=0;i<6;i++){
           if(serialKey[i]==rxValue[i]){
           serialKeyCheck = true;
         }
      }
        if(serialKeyCheck==true){
          Serial.println("Serial Key is correct");
          showit("Serial Key is correct");
          int l=rxValue.length();
          if(l>6){
          for(int i=6;i<l;i++){
            key[i-6]=rxValue[i];
            }
            Serial.println("Password has changed");
            delay(500);
            std::string changeString = "Password has changed to " + key;
            char *c =(char*) changeString.c_str();
            showit(c); 
          }
          else{
            Serial.println("Password is not changed");
            showit("Password is not changed");
          }
          serialKeyCheck = false;
          }
        else{
          delay(1000);
          showit("Serial key is incorrect");
        }
      //key = rxValue;
      delay(1000);
      showit("Connected Home");   
    }
};

class Case : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic1) {
      std::string rxValue = pCharacteristic1->getValue();
        display.init();
        
      Type->setValue(rxValue);
      Serial.println();
      delay(40);
      showit("Connected Home");   
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t *param) {
      esp_err_t esp_ble_gap_read_rssi(esp_bd_addr_t remote_addr);
      //ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT;
      int8_t  rssi_value;
      //rssi_value = p->scan_rst.rssi;
      //rssi_value = p->read_rssi_cmpl.rssi;
  
      Serial.println("New device connected");
      deviceConnected = true;
      Serial.println("MAC is: ");
      char remoteAddress[18];

      sprintf(
        remoteAddress,
        "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
        param->connect.remote_bda[0],
        param->connect.remote_bda[1],
        param->connect.remote_bda[2],
        param->connect.remote_bda[3],
        param->connect.remote_bda[4],
        param->connect.remote_bda[5]
      );
      mac = remoteAddress;
      Serial.println(remoteAddress);
      display.init();
      showit("User " + String(remoteAddress) + " connected");

      Serial.println(pServer->getConnId());
      id = pServer->getConnId();
      delay(2000);
      BLEDevice::startAdvertising();		
      //pServer->removePeerDevice(param->disconnect.conn_id, false);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      showit("User disconnected");
      delay(1000);
      showit("Connected Home");   
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  pinMode(16,OUTPUT);
  digitalWrite(16,HIGH);
  BLEDevice::init("Door");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  

  MAC      = pService->createCharacteristic(  
                        MAC_UUID,
                        BLECharacteristic::PROPERTY_READ
                      );

  Password = pService->createCharacteristic(
                        Password_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE 
                      );

  Admin    = pService->createCharacteristic(
                        Admin_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE 
                      );

  Type     = pService->createCharacteristic(  
                        Type_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE 
                      );
  
  MAC->setValue(BLEDevice::getAddress().toString().c_str());
  Type->setValue("tap");
  Password ->setCallbacks(new Check());
  Admin    ->setCallbacks(new GodMode());
  Type     ->setCallbacks(new Case());
  /* MAC      ->addDescriptor(new BLE2902());
  Password ->addDescriptor(new BLE2902());
  Admin    ->addDescriptor(new BLE2902());
  Type     ->addDescriptor(new BLE2902());
*/

  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined!");
}

void loop() {
// notify changed value
    if (deviceConnected) {
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}