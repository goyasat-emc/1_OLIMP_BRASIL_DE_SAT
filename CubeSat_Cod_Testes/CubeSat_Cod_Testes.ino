#include "PION_System.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "FS.h"

System cubeSat;

const char* ssid = "OBSAT_WIFI";
const char* net_password = "OBSatZenith1000";
const char* serverName = "http://192.168.0.1/";

unsigned long lastTime = 0;
// Cada mensagem deve ser enviada em um intervalo de 4 minutos
// 4 minutos = 240000ms
unsigned long timerDelay = 240000;

// Informações obrigatórias de status do satélite
const uint8_t equipe = 41; // Cada equipe receberá um ID, que deverá ser informado, obrigatoriamente, no campo "equipe"
uint8_t bateria; // 1 byte
float temperatura, pressaoAtmosferica, velAngularX, velAngularY, velAngularZ, aceleracaoX, aceleracaoY, aceleracaoZ; // 8*4 bytes = 32 bytes
uint8_t numImagens = 0; // (exemplo de payload) 1 byte

// Informações não obrigatórias de status do satélite
float umidade, nivelCO2, luminosidade, campoMagX, campoMagY, campoMagZ;

String json;

void setup(){
  //Serial.begin(115200);
  
  // Inicializa seu CanSat, e seus periféricos 
  cubeSat.init(true);
  cubeSat.createLogOnSD();
  //Serial.println("CubeSat inicializado");

}

void loop(){
  // Loop vazio pois as funções básicas rodam em um nucleo paralelo
}
