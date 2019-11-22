#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <BLEDevice.h>
#include <BLEServer.h>


#define ONBOARD_LED 2

static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID(   "beb5483e-36e1-4688-b7f5-ea07361b26a8");

const char *ssid = "mehan1";
const char *password = "casamehan";
WebServer server(80);

/**
 * setup
 */
void setup(void) {
  
  Serial.begin(115200);

  //Turn off on board led
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, 0);
    
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.printf ("\nConnected to %s\n",ssid );
  Serial.print  ("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("i8Controller")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/api/action", performAction);
  server.onNotFound(performAction);
  server.begin();

  Serial.println("HTTP server started");

  //initialize BLE Server
  bleServerInit();

}

/**
 * loop
 */
void loop() {
  
  server.handleClient();
  
}

/**
 * performAction
 */
void performAction(){
  
  bool notFound=false;
  
  String url = server.uri();
  //Serial.printf("URL='%s'\n",url.c_str());
  
  if (url.endsWith("toggledriverdoor")){
    toggleDriverDoor();
  }else if(url.endsWith("togglepassengerdoor")){
    togglePassengerDoor();
  }else if(url.endsWith("forward")){
    forward();
  }else if(url.endsWith("back")){
    back();
  }else if (url.endsWith("stop")){
    stop();
  }else if (url.endsWith("right")){
    right();
  }else if (url.endsWith("left")){
    left();
  }else{
    notFound=true;
  }
  
  if(notFound){
    server.send(404, "text/plain", "Not Found");
  }else{
    server.send(200, "text/plain", "ok");
  }  
}


/**
 * Actions
 */
 
void toggleDriverDoor(){
  Serial.println("> driverDoor");
}

void togglePassengerDoor(){
  Serial.println("> passengerDoor");
}

void forward(){
  Serial.println("> forward");
}

void back(){
  Serial.println("> back");
}

void stop(){
  Serial.println("> stop");
}

void right(){
  Serial.println("> right");
}

void left(){
  Serial.println("> left");
}




/**
 * BLE SERVER
 */

class ServerCallbacks: public BLECharacteristicCallbacks {
  
    /**
     * F - Forward          0x46
     * B - Back             0x42
     * R - Right            0x52
     * L - Left             0x4C
     * S - Stop             0x53
     * D - Driver Door      0x44
     * P - Passenger Door   0x50
     */
    void onWrite(BLECharacteristic *pCharacteristic) {
      
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        char cmd = value[0];
//        Serial.printf("> %c\n",cmd); 

        switch(cmd){
          case 'F':
            forward();
            break;
          case 'B':
            back();
            break;  
          case 'R':
            right();
            break;  
          case 'L':
            left();
            break;  
          case 'S':
            stop();
            break;  
          case 'D':
            toggleDriverDoor();
            break;  
          case 'P':
            togglePassengerDoor();
            break;              
        }
      }
    }
};


void bleServerInit(){

  Serial.println("BLE Server - Initializing");
  
  BLEDevice::init("Irdeto I8 Controller");
  BLEServer *pServer = BLEDevice::createServer();
  

  BLEService *pService = pServer->createService(serviceUUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         charUUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new ServerCallbacks());

  pCharacteristic->setValue("I8 Controller");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(pService->getUUID());
  pAdvertising->start();
  Serial.println("BLE Server - Started");
  
}
