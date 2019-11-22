#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#define WIFI_SSID                   "i8Controller"
#define WIFI_SSID_PASS              ""
#define I8HOST                      "192.168.4.1"
#define I8PORT                      80
#define URL_TOGGLE_DRIVER_DOOR      "/api/action/toggledriverdoor"
#define URL_TOGGLE_PASSENGER_DOOR   "/api/action/toggledriverdoor"

WiFiMulti wifiMulti;
long   lastTx = 0;

/**
 * setup
 */ 
void setup() {

    Serial.begin(115200);
    Serial.println("");
    Serial.println("");
    Serial.println("WebClient Starting");

    Serial.printf("Connecting to %s ",WIFI_SSID);
    
    wifiMulti.addAP(WIFI_SSID, WIFI_SSID_PASS);
}

/**
 * loop
 */ 
void loop() {
  
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {

      //send the commmand every 30 seconds
      if (millis() - lastTx > 30000){
        sendRequest(URL_TOGGLE_DRIVER_DOOR);
        lastTx = millis();
      }
        
    }else{
      //connecting... 
      Serial.print(".");
    }
    
    processSerial();
    delay(10);
}

/**
 * sendRequest
 */
void sendRequest(const char *action){
  
  HTTPClient http;
  
  Serial.println("");
  Serial.printf("%s - ",action);
  
  http.begin(I8HOST, I8PORT, action);
  int httpCode = http.GET();

  if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
          Serial.println("SUCCESS");          
//          String payload = http.getString();
//          Serial.println(payload);
      }            
  } else {
      Serial.printf("FAILED: '%s'\n", http.errorToString(httpCode).c_str());
  }
  http.end();
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
