// Bibliotecas para o funcionamento do protocolo ESP-NOW
#include <esp_now.h>
#include <WiFi.h>

// MAC Address of your receiver (ESP32 Cam = )
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x02, 0x93, 0xBC};

// Define variables to store data to be sent
uint8_t pictureNumber = 0;
uint8_t systemStatus_CAM;

// Define variables to store incoming data
uint8_t bateria;
uint8_t systemStatus_CUBESAT;

// Variable to store if sending data was successful
String success;

//Must match the receiver structure
typedef struct struct_message {
    uint8_t info;
    uint8_t systemStatus;
} struct_message;

// Create a struct_message called camMessage to hold message to be sent
struct_message camMessage;

// Create a struct_message to hold incoming data from cubesat
struct_message cubesatMessage;

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
  memcpy(&cubesatMessage, incomingData, sizeof(cubesatMessage));
  Serial.print("Bytes received: ");
  Serial.println(len);
  bateria = cubesatMessage.info;
  systemStatus_CUBESAT = cubesatMessage.systemStatus;
}
