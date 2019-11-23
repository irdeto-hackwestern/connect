#include <Arduino.h>
#include <BLEDevice.h>


static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID(   "beb5483e-36e1-4688-b7f5-ea07361b26a8");

/**
 * F - Forward                      0x46
 * B - Back                         0x42
 * R - Right                        0x52
 * L - Left                         0x4C
 * S - Stop                         0x53
 * T - Turn Tires Right             0x54
 * U - Turn Tires Left              0x55
 * D - Driver Door                  0x44
 * P - Passenger Door               0x50
 */

byte ACTION_FORWARD[1]            = {0x46};
byte ACTION_BACK[1]               = {0x42};
byte ACTION_RIGHT[1]              = {0x52};
byte ACTION_LEFT[1]               = {0x4C};
byte ACTION_STOP[1]               = {0x53};
byte ACTION_TURN_TIRES_RIGHT[1]   = {0x54};
byte ACTION_TURN_TIRES_LEFT[1]    = {0x55};
byte ACTION_DRIVER_DOOR[1]        = {0x44};
byte ACTION_PASSENGER_DOOR[1]     = {0x50};

static boolean doConnect = false;
static boolean connected = false;

static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
long   lastCheck = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\nbleClient - start\n");
  BLEDevice::init("Irdeto I8 Client");
  bleClientInit();
  
}

void loop() {

  connectToI8Controller();

  if (millis() - lastCheck > 30000){
    sendBleCommand(ACTION_DRIVER_DOOR);
    delay(1000);
    sendBleCommand(ACTION_PASSENGER_DOOR);
    delay(1000);
    sendBleCommand(ACTION_DRIVER_DOOR);
    delay(1000);
    sendBleCommand(ACTION_PASSENGER_DOOR);
    delay(1000);

    sendBleCommand(ACTION_TURN_TIRES_RIGHT);
    delay(1000);
    sendBleCommand(ACTION_FORWARD);
    delay(2000);
    sendBleCommand(ACTION_BACK);
    delay(2000);
    sendBleCommand(ACTION_STOP);
    delay(1000);
    
    sendBleCommand(ACTION_DRIVER_DOOR);
    delay(1000);
    sendBleCommand(ACTION_PASSENGER_DOOR);
    delay(1000);
    sendBleCommand(ACTION_DRIVER_DOOR);
    delay(1000);
    sendBleCommand(ACTION_PASSENGER_DOOR);

    lastCheck = millis();
  }

  processSerial();

  delay(10); 
    
} 

/**
 * processSerial
 */
void processSerial(){
  
  if (Serial.available() > 0) {    
    String input = Serial.readString();

    //Serial.printf("> '%s'\n");
    
    //reboot
    if(input.startsWith("/reboot")){      
      Serial.println("Rebooting....");
      ESP.restart();
    }      
  }
  
}

void sendBleCommand(byte *action){
  if (connected) {      
      Serial.println("Sending action");
    
      // send action 
      pRemoteCharacteristic->writeValue(action,1);

      
    }else if(doScan){
      Serial.println("Failed to send, not connected.");
    }
  
}
/**
 * MyAdvertisedDeviceCallbacks
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
//    Serial.print("BLE Advertised Device found: ");
//    Serial.println(advertisedDevice.toString().c_str());
    
    if (advertisedDevice.haveServiceUUID()) {

      // We have found a device, let us now see if it contains the service we are looking for.
      if (advertisedDevice.isAdvertisingService(serviceUUID)) {
  
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;
        Serial.println("Found Irdeto I8 Controller");
      }else{
        Serial.print("Service Data: ");
        Serial.println(advertisedDevice.getServiceData().c_str());
      }
    }
  } // onResult
}; // MyAdvertisedDeviceCallbacks


/**
 * MyClientCallback
 */
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
  }
};

/**
 * setupClient
 */
void bleClientInit(){
  Serial.println("Connecting to i8 Controller over BLE...");
  
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

}

/**
 * connectToI8
 */
void connectToI8Controller(){
 
 // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println(" - Connected");
    } else {
      Serial.println(" - **** Failed to Connect!");
    }

    doConnect = false;

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    if (connected) {      
      Serial.println(" - Done");      
    }else if(doScan){
      BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
    }
  }
  
}

/*
 * connectToServer
 */
bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    connected = true;
    return true;
}
