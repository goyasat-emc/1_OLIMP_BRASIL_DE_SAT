#include "PION_System.h"
#include <HTTPClient.h>
#include <WiFi.h>

#include "FS.h"

System cubeSat;

// Rede WiFi na qual o CubeSat deve se conectar e o host (serverName) que será procurado para envio de dados
//const char* ssid = "OBSAT_WIFI";
//const char* net_password = "OBSatZenith1000";
//const char* serverName = "http://192.168.0.1/";

const char* ssid = "Menegon_2GHz";
const char* net_password = "mejv1413";
const char* serverName = "https://httpbin.org/post";

unsigned long lastTime = 0;
// Cada mensagem deve ser enviada em um intervalo de 4 minutos
// 4 minutos = 240000ms
unsigned long timerDelay = 240000;

unsigned long timerDelay_SD = 5000; // Intervalo de gravação de dados no cartão micro SD

// Informações obrigatórias de status do satélite
const uint8_t equipe = 41; // Cada equipe receberá um ID, que deverá ser informado, obrigatoriamente, no campo "equipe"
uint8_t bateria; // 1 byte
float temperatura, pressaoAtmosferica, velAngularX, velAngularY, velAngularZ, aceleracaoX, aceleracaoY, aceleracaoZ; // 8*4 bytes = 32 bytes
uint8_t numImagens = 0; // (exemplo de payload) 1 byte

// Informações não obrigatórias de status do satélite
float umidade, nivelCO2, luminosidade, campoMagX, campoMagY, campoMagZ;

String json;

void setup() {
  // Inicializa seu CubeSat, e seus periféricos
  cubeSat.init(true);
  
  cubeSat.createLogOnSD();
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
    // Verifica status da conexão WiFi
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin( serverName );
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST( getCubeSatJson().c_str() ); // c_str() é necessário pois a função espera um vetor de char

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

  // Gravação no cartão SD
  cubeSat.logOnSDFile();
  delay(timerDelay_SD);

}

String getCubeSatJson() {
  // Obtenção dos dados dos sensores
  getSensorsInfo();

  // Criação da string que representa o JSON a ser enviado via HTTP POST
  json = "{\"equipe\":";
  json += String(equipe);
  json += ",\"bateria\":";
  json += String(bateria);
  json += ",\"temperatura\":";
  json += String(temperatura);
  json += ",\"pressao\":";
  json += String(pressaoAtmosferica);
  json += ",\"giroscopio\":[";
  json += String(velAngularX);
  json += ",";
  json += String(velAngularY);
  json += ",";
  json += String(velAngularZ);
  json += "],\"acelerometro\":[";
  json += String(aceleracaoX);
  json += ",";
  json += String(aceleracaoY);
  json += ",";
  json += String(aceleracaoZ);
  json += "],\"payload\":{\"numImagens\":";
  json += String(numImagens);
  json += "}}";

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

// Função Modificada
void createFileFirstLine(fs::FS &fs, const char * path) {
  // Mostra o nome do arquivo
  Serial.printf("Escrevendo em: %s\n", path);

  //Abre o arquivo do SD para a memória RAM
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Falha ao abrir para escrita");
    return;
  }
  // Cria a primeira linha modificada e separada por vírgulas do CSV.
  const char * message = "horario(ms),equipe,bateria(%),temperatura(C),pressao(Pa),velAngularX(graus/s),velAngularY(graus/s),velAngularZ(graus/s),aceleracaoX(m/s2),aceleracaoY(m/s2),aceleracaoZ(m/s2),numImagens";

  // Escreve a mensagem criada anteriormente
  if (file.println(message)) {
    Serial.println("Escrita Começou");
  } else {
    Serial.println("Falha na escrita");
  }
  // Fecha o arquivo
  file.close();
}

// Função modicada para armazenamento no cartão SD
void appendFile(fs::FS &fs, const char * path, TickType_t time) {
  //Abre o arquivo do SD para a memória RAM
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Falha ao abrir para gravacao");
    return;
  }
  // Salva no CSV o dado, seguido de uma vírgula.
  file.print(millis());
  file.write(',');
  file.print(equipe);
  file.write(',');
  file.print(bateria);
  file.write(',');
  file.print(temperatura);
  file.write(',');
  file.print(pressaoAtmosferica);
  file.write(',');
  file.print(velAngularX);
  file.write(',');
  file.print(velAngularY);
  file.write(',');
  file.print(velAngularZ);
  file.write(',');
  file.print(aceleracaoX);
  file.write(',');
  file.print(aceleracaoY);
  file.write(',');
  file.print(aceleracaoZ);
  file.write(',');
  file.print(aceleracaoZ);
  file.write(',');
  file.println(numImagens);
  // Fecha o arquivo
  file.close();
}
