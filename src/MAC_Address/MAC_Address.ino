#include "WiFi.h"

// After uploading the code, press the RST/EN button, and the MAC address should be displayed on the Serial Monitor.
 
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
