#include "PION_System.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "OBSAT_WIFI";
const char* net_password = "OBSatZenith1000";
const char* serverName = "http://192.168.0.1/";

unsigned long lastTime = 0;
// Cada mensagem deve ser enviada em um intervalo de 4 minutos
// 4 minutos = 240000ms
unsigned long timerDelay = 240000;

System cubeSat;

// Variáveis utilizadas na criação do JSON
StaticJsonBuffer<300> JSONbuffer;
JsonObject& JSONencoder = JSONbuffer.createObject();

// Informações obrigatórias de status do satélite
const uint8_t equipe = 41; // Cada equipe receberá um ID, que deverá ser informado, obrigatoriamente, no campo "equipe"
uint8_t bateria; // 1 byte
float temperatura, pressaoAtmosferica, velAngularX, velAngularY, velAngularZ, aceleracaoX, aceleracaoY, aceleracaoZ; // 8*4 bytes = 32 bytes
uint8_t numImagens = 0; // (exemplo de payload) 1 byte

float umidade, nivelCO2, luminosidade, campoMagX, campoMagY, campoMagZ;

String json;

void setup() {
  Serial.begin(115200);

  // Inicializa seu CubeSat, e seus periféricos
  cubeSat.init(true);
  Serial.println("CubeSat inicializado");

  // Configura WiFi no modo station
  Serial.println("Iniciando conexão Wifi");
  WiFi.begin(ssid, net_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conexão concluída");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("Configuração inicial (setup) terminada");
}

void loop() {
  // Cada mensagem deve ser enviada em um intervalo de 4 minutos.
  if ((millis() - lastTime) > timerDelay) {
    // Vrifica status da conexão WiFi
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      // c_str() é necessário pois a função espera um vetor de char
      http.begin( serverName.c_str() );
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST( getCubeSatJson().c_str() );

      Serial.print("Código de resposta HTTP: ");
      Serial.println(httpResponseCode);
      if ( httpResponseCode > 0 ) { // Caso a requisição não chegue ao servidor, o valor de retorno do método será 0
        Serial.println( http.getString() );
      }

      // Liberação de recurso
      http.end();
    }
    else {
      Serial.println("WiFi desconectado");
    }
    lastTime = millis();
  }

}

String getCubeSatJson() {
  // Obtenção dos dados dos sensores
  getSensorsInfo();

  // Criação do JSON a ser enviado via HTTP POST
  JSONencoder["equipe"] = equipe;
  JSONencoder["bateria"] = bateria;
  JSONencoder["temperatura"] = temperatura;
  JSONencoder["pressao"] = pressaoAtmosferica;
  JSONencoder["equipe"] = equipe;

  JsonArray& velAngular = JSONencoder.createNestedArray("giroscopio");
  velAngular.add(velAngularX);
  velAngular.add(velAngularY);
  velAngular.add(velAngularZ);

  JsonArray& aceleracao = JSONencoder.createNestedArray("acelerometro");
  aceleracao.add(aceleracaoX);
  aceleracao.add(aceleracaoY);
  aceleracao.add(aceleracaoZ);

  return json;
}

void getSensorsInfo() {
  // Valor da bateria (0 a 100%)
  bateria = cubeSat.getBattery();

  // Realiza a leitura de Temperatura, em °C, e armazena em uma variável do tipo float
  temperatura = cubeSat.getTemperature();

  // Realiza a leitura de Pressão, em Pa, e armazena em uma variável do tipo float.
  pressaoAtmosferica = cubeSat.getPressure();

  // Realiza a leitura de velocidade angular no eixo X, em graus/s, e armazena em uma variável do tipo float.
  velAngularX = cubeSat.getGyroscope(0);
  // Realiza a leitura de velocidade angular no eixo Y, em graus/s, e armazena em uma variável do tipo float.
  velAngularY = cubeSat.getGyroscope(1);
  // Realiza a leitura de velocidade angular no eixo Z, em graus/s, e armazena em uma variável do tipo float.
  velAngularZ = cubeSat.getGyroscope(2);

  // Realiza a leitura de aceleração no eixo X, em m/s², e armazena em uma variável do tipo float.
  aceleracaoX = cubeSat.getAccelerometer(0);
  // Realiza a leitura de aceleração no eixo Y, em m/s², e armazena em uma variável do tipo float.
  aceleracaoY = cubeSat.getAccelerometer(1);
  // Realiza a leitura de aceleração no eixo Z, em m/s², e armazena em uma variável do tipo float.
  aceleracaoZ = cubeSat.getAccelerometer(2);

  // ---------------------------------------------------------------------------------------------------------------------- //
  // Dados não obrigatórios

  // Realiza a leitura de Umidade Relativa , em %, e armazena em uma variável do tipo float.
  umidade = cubeSat.getHumidity();

  // Realiza a leitura de concentração de CO2, em ppm, e armazena em uma variável do tipo float.
  nivelCO2 = cubeSat.getCO2Level();

  // Realiza a leitura de incidência de luz, em %, e armazena em uma variável do tipo float.
  luminosidade = cubeSat.getLuminosity();

  // Realiza a leitura de intensidade do campo magnético no eixo X, em μT, e armazena em uma variável do tipo float.
  campoMagX = cubeSat.getMagnetometer(0);
  // Realiza a leitura de intensidade do campo magnético no eixo Y, em μT, e armazena em uma variável do tipo float.
  campoMagY = cubeSat.getMagnetometer(1);
  // Realiza a leitura de intensidade do campo magnético no eixo Z, em μT, e armazena em uma variável do tipo float.
  campoMagZ = cubeSat.getMagnetometer(2);
}
