#include <esp_now.h>
#include <WiFi.h>

void initEspNow();
void sendEspNow();

// MAC Address of your receiver (CubeSat = 3C:61:05:02:93:BC)
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x02, 0x93, 0xBC};

// Define variables to be sent
uint8_t cam_systemStatus;
uint8_t take;

// Define variables to store incoming data
uint8_t incoming_CubeSat_systemStatus;
uint8_t incoming_CubeSat_bateria;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    uint8_t systemStatus;
    uint8_t info;
} struct_message;


struct_message camMessage;
struct_message incomingCubeData;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCubeData, incomingData, sizeof(incomingCubeData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  incoming_CubeSat_systemStatus = incomingCubeData.systemStatus;
  incoming_CubeSat_bateria = incomingCubeData.info;

  if( incoming_CubeSat_bateria < 2 ){ // Alerta vermelho
    esp_deep_sleep_start(); // "Dormir" para economizar bateria
  }
}
 
void initEspNow() {
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void sendEspNow() { 
  // Set values to send
  camMessage.systemStatus = cam_systemStatus;
  camMessage.info = take; 
 
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &camMessage, sizeof(camMessage));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}
