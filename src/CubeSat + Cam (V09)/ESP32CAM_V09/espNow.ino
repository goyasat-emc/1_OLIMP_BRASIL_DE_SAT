#include <esp_now.h>
#include <WiFi.h>

void initEspNow();
void sendEspNow();

unsigned int numTentativas = 0; // Número de tentativas de envio

uint8_t numImagens = 0;
unsigned long instante_ultimaFoto = 0;

// MAC Address do receptor (CubeSat = 3C:61:05:02:93:BC)
uint8_t broadcastAddress[] = {0x3C, 0x61, 0x05, 0x02, 0x93, 0xBC};

// Define variáveis dos valores a serem enviados via protocolo ESP-NOW
uint8_t cam_systemStatus = 200; // Status 200 = Sistema OK
uint8_t take = 0;

// Define variáveis que irão armazenar os dados recebidos do CubeSat via protocolo ESP-NOW
uint8_t incoming_CubeSat_systemStatus;
uint8_t incoming_CubeSat_bateria;

// Variável que registra o sucesso do envio
String success;

//Structure para enviar e receber dados
typedef struct struct_message {
    uint8_t systemStatus;
    uint8_t info;
} struct_message;

struct_message camMessage; // Dados a serem enviados
struct_message incomingCubeData; // Dados a serem recebidos

esp_now_peer_info_t peerInfo; // Variável que armazena informações sobre o peer entre o qual informações serão trocadas

// Callback para quando dados são enviados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  delay(200);
  if (status ==0){
    success = "Delivery Success";
    numTentativas = 0;
  }
  else{
    success = "Delivery Fail";
    // Tentar novamente?
    if(numTentativas < 5){
      numTentativas++;
      sendEspNow();
    }
  }
}

// Callback para quando dados são recebidos
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incomingCubeData, incomingData, sizeof(incomingCubeData));
  Serial.print("ESP-NOW: ");
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
  incoming_CubeSat_systemStatus = incomingCubeData.systemStatus;
  incoming_CubeSat_bateria = incomingCubeData.info;

  Serial.print("Debug: (");
  Serial.print(incoming_CubeSat_systemStatus);
  Serial.print(" , ");
  Serial.print(incoming_CubeSat_bateria);
  Serial.println(").");  
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
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Adição do peer          
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("ESP-NOW: Failed to add peer");
    return;
  }
  // Registro da função de callback referente ao recebimento de dados
  esp_now_register_recv_cb(OnDataRecv);
}
 
void sendEspNow() { 
  // Configuração de valores a serem enviados para o ESP32-CAM
  camMessage.systemStatus = cam_systemStatus;
  camMessage.info = take; 

  //<Assfalk@ufg.br
  Serial.print("sendEspNow Debug: (");
  Serial.print(camMessage.systemStatus);
  Serial.print(" , ");
  Serial.print(camMessage.info);
  Serial.println(").");
  //>Assfalk@ufg.br
      
  // Envio de dados via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &camMessage, sizeof(camMessage));
   
  if (result == ESP_OK) {
    Serial.println("ESP-NOW: Enviado com sucesso");
  }
  else {
    Serial.println("ESP-NOW: Erro ao enviar dados");
  }
}
