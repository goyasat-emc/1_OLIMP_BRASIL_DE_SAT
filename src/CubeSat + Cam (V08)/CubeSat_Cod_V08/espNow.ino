#include <esp_now.h>
#include <WiFi.h>

void initEspNow();
void sendEspNow();

uint8_t numImagens = 0;

unsigned long instante_ultimaFoto = 0;

// MAC Address of your receiver (ESP32 Cam = )
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Define variables to be sent
uint8_t CubeSat_systemStatus = 200; // Status OK
uint8_t bateria;

// Define variables to store incoming data
uint8_t incoming_cam_systemStatus;
uint8_t incoming_cam_take;

// Variable to store if sending data was successful
String success;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    uint8_t systemStatus;
    uint8_t info;
} struct_message;

struct_message CubeData; // Dados a serem enviados
struct_message incomingCamMessage; // Dados a serem recebidos

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
  memcpy(&incomingCamMessage, incomingData, sizeof(incomingCamMessage));
  Serial.print("Bytes recebidos: ");
  Serial.println(len);
  incoming_cam_systemStatus = incomingCamMessage.systemStatus;
  incoming_cam_take = incomingCamMessage.info;

  if( incoming_cam_take == 1 ) { // foto tirada
    numImagens++;
    instante_ultimaFoto = millis();
    Serial.println("ESP32-CAM informa: uma captura de imagem foi feita");
  }
  incoming_cam_take = 0;
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
  CubeData.systemStatus = CubeSat_systemStatus;
  CubeData.info = bateria;
 
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &CubeData, sizeof(CubeData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}
