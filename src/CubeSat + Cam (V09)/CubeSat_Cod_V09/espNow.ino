#include <esp_now.h>
#include <WiFi.h>

void initEspNow();
void sendEspNow();

uint8_t numImagens = 0;

unsigned long instante_ultimaFoto = 0;

// MAC Address do receptor (ESP32 Cam = 4C:EB:D6:7B:5D:48)
uint8_t broadcastAddress[] = {0x4C, 0xEB, 0xD6, 0x7B, 0x5D, 0x48};

// Define variáveis dos valores a serem enviados via protocolo ESP-NOW
uint8_t CubeSat_systemStatus = 200; // Status 200 = Sistema OK
uint8_t bateria = 1; //originally no value allocated: Assfalk@ufg.br

// Define variáveis que irão armazenar os dados recebidos da ESP32-CAM via protocolo ESP-NOW
uint8_t incoming_cam_systemStatus;
uint8_t incoming_cam_take;

// Variável que registra o sucesso do envio
String success;

//Structure para enviar e receber dados
typedef struct struct_message {
    uint8_t systemStatus; 
    uint8_t info;
} struct_message;

struct_message CubeData; // Dados a serem enviados
struct_message incomingCamMessage; // Dados a serem recebidos

esp_now_peer_info_t peerInfo; // Variável que armazena informações sobre o peer entre o qual informações serão trocadas

// Callback para quando dados são enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nESP-NOW: Last Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// Callback para quando dados são recebidos
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCamMessage, incomingData, sizeof(incomingCamMessage));
  Serial.print(len);
  Serial.print(" bytes recebidos");
  
  //<Assfalk@ufg.br
  Serial.print(" de ");
  Serial.print(mac[1]);
  Serial.print(":");
  Serial.print(mac[2]);
  Serial.print(":");
  Serial.print(mac[3]);
  Serial.print(":");
  Serial.print(mac[4]);
  Serial.print(":");
  Serial.print(mac[5]);
  //>Assfalk@ufg.br
  
  Serial.println(".");
  incoming_cam_systemStatus = incomingCamMessage.systemStatus;
  incoming_cam_take = incomingCamMessage.info;

  //<Assfalk@ufg.br
  Serial.print("Debug: (");
  Serial.print(incoming_cam_systemStatus);
  Serial.print(" , ");
  Serial.print(incoming_cam_take);
  Serial.println(").");
  //>Assfalk@ufg.br
  
  if( incoming_cam_take == 1 ) { // foto tirada
    numImagens++;
    instante_ultimaFoto = millis();
    Serial.println("ESP32-CAM informa: uma captura de imagem foi feita");
  }
  incoming_cam_take = 0;
}
 
void initEspNow() {
  // Dispositivo configurado no modo Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW: Erro inicializando ESP-NOW");
    return;
  }

  // Registro da função de callback referente ao envio de dados
  esp_now_register_send_cb(OnDataSent);
  
  // Registro do peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;  
  peerInfo.encrypt = false;
  
  // Adição do peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("ESP-NOW: Falha ao adicionar peer");
    return;
  }
  // Registro da função de callback referente ao recebimento de dados
  esp_now_register_recv_cb(OnDataRecv);
}
 
void sendEspNow() { 
  // Configuração de valores a serem enviados para o ESP32-CAM
  CubeData.systemStatus = CubeSat_systemStatus;
  CubeData.info = bateria;

  //<Assfalk@ufg.br
  Serial.print("sendEspNow Debug: (");
  Serial.print(CubeData.systemStatus);
  Serial.print(" , ");
  Serial.print(CubeData.info);
  Serial.println(").");
  //>Assfalk@ufg.br
 
  // Envio de dados via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &CubeData, sizeof(CubeData));
   
  if (result == ESP_OK) {
    Serial.println("ESP-NOW: Enviado com sucesso");
  }
  else {
    Serial.println("ESP-NOW: Erro ao enviar");
  }
}
