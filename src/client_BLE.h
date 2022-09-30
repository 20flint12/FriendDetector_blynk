/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include <ArduinoJson.h>
#include "BLEDevice.h"


// // The remote service we wish to connect to.
// static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// // The characteristic of the remote service we are interested in.
// static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

static BLEUUID serviceUUID(SERVICE_UUID);
static BLEUUID characteristicUUID_RX(CHARACTERISTIC_UUID_RX);
static BLEUUID characteristicUUID_TX(CHARACTERISTIC_UUID_TX);


static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteTxCharacteristic;
static BLERemoteCharacteristic* pRemoteRxCharacteristic;  // here to write 
static BLEAdvertisedDevice* myDevice;



static void notifyCallback(
                            BLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData,
                            size_t length,
                            bool isNotify) 
{
    // Serial.print("Notify ");
    // Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    // Serial.print(" len ");
    // Serial.print(length);
    // Serial.print("data: ");
    // DEBUG_PRINTF(BLYNK_F("data: "), (char*)pData); // prints only "data"
    // DEBUG_PRINT((char*)pData);
    DEBUG_PRINT2(BLYNK_F("data: "), (char*)pData);
    
    DynamicJsonDocument doc(20);
    // StaticJsonDocument<30> doc;

    DeserializationError err = deserializeJson(doc, pData);
    DEBUG_PRINT1(err.f_str());

    JsonObject obj = doc.as<JsonObject>();

    if (obj.containsKey("p0")) {
      p0 = obj[String("p0")]; //Serial.println(p0); 
      return;
    }
    if (obj.containsKey("p1")) {
      p1 = obj[String("p1")]; //Serial.println(p1); 
      return;
    }
    if (obj.containsKey("p2")) {
      p2 = obj[String("p2")]; //Serial.println(p2); 
      return;
    }
    if (obj.containsKey("p3")) {
      p3 = obj[String("p3")]; //Serial.println(p3); 
      return;
    }
    if (obj.containsKey("p4")) {
      p4 = obj[String("p4")]; //Serial.println(p4); 
      return;
    }
    if (obj.containsKey("p5")) {
      p5 = obj[String("p5")]; //Serial.println(p5); 
      return;
    }

    timer_30s.setTimeout(300, timer_BLE_message);
}


class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    DEBUG_PRINT1(BLYNK_F("onConnect"));
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    DEBUG_PRINT1(BLYNK_F("onDisconnect"));
  }
};


bool connectToServer() {

    DEBUG_PRINT2(BLYNK_F("Forming a connection to "), myDevice->getAddress().toString().c_str());

    BLEClient*  pClient  = BLEDevice::createClient();
    DEBUG_PRINT1(BLYNK_F(" - Created client"));

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    DEBUG_PRINT1(BLYNK_F(" - Connected to server"));

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      DEBUG_PRINT2(BLYNK_F("Failed to find our service UUID: "), serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    DEBUG_PRINT1(BLYNK_F(" - Found our service"));
    

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteTxCharacteristic = pRemoteService->getCharacteristic(characteristicUUID_RX);
    
    if (pRemoteTxCharacteristic == nullptr) {
      DEBUG_PRINT2(BLYNK_F("Failed to find our characteristic UUID: "), characteristicUUID_RX.toString().c_str());
      pClient->disconnect();
      return false;
    }
    DEBUG_PRINT1(BLYNK_F(" - Found our characteristic"));

    // Read the value of the characteristic.
    if(pRemoteTxCharacteristic->canRead()) {
      std::string value = pRemoteTxCharacteristic->readValue();
      DEBUG_PRINT2(BLYNK_F("The characteristic value was: "), value.c_str());
    }

    if(pRemoteTxCharacteristic->canNotify()) {
      pRemoteTxCharacteristic->registerForNotify(notifyCallback);
    }


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteRxCharacteristic = pRemoteService->getCharacteristic(characteristicUUID_TX);


    connected = true;
    return true;
}


/*
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  /*
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    DEBUG_PRINT2(BLYNK_F("BLE found: "), advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      DEBUG_PRINT1(BLYNK_F("\n----------------------------------------------------"));
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup_client_BLE() {
  // Serial.println("Starting Arduino BLE Client application...");
  DEBUG_PRINT1(BLYNK_F("Starting Arduino BLE Client application..."));
  BLEDevice::init("ESP32");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.


// This is the Arduino main loop function.
void loop_client_BLE() {

  DEBUG_PRINT4(BLYNK_F("| "), doConnect, connected, doScan);
  
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      // Serial.println("We are now connected to the BLE Server.");
      DEBUG_PRINT1(BLYNK_F("We are now connected to the BLE Server."));
    } else {
      // Serial.println("We have failed to connect to the server; there is nothin more we will do.");
      DEBUG_PRINT1(BLYNK_F("We have failed to connect to the server; there is nothin more we will do."));
      ESP.restart();
    }
    doConnect = false;
  }


  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    // String newValue = "Time since boot: " + String(millis()/1000);
    // Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    // Serial.print(".");
    // Set the characteristic's value to be the array of bytes that is actually a string.
    // pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  
  //sdelay(1000); // Delay a second between loops.
} // End of loop


void send2Ble_server(String mess)
{
    // Serial.println(mess);
    DEBUG_PRINT1(mess);
    pRemoteRxCharacteristic->writeValue((uint8_t*)mess.c_str(), mess.length());
}
