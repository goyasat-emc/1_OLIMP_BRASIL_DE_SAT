#include "PION_System.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "FS.h"

const int entrada_sensores_temp = 13;
float temperatura_pion, temperatura_ds18b20; 

OneWire oneWire(entrada_sensores_temp);  /*Protocolo OneWire*/

/********************************************************************/
DallasTemperature sensorTemp(&oneWire); /*encaminha referências OneWire para o sensor*/
System cubeSat;

void setup() {
  // Inicializa seu CubeSat, e seus periféricos
  cubeSat.init(true);
  cubeSat.createLogOnSD();
  pinMode(entrada_sensores_temp, INPUT);
  sensorTemp.begin(); /*inicia biblioteca*/
}

void loop() {
  sensorTemp.requestTemperatures(); /* Envia o comando para leitura da temperatura */


  temperatura_pion = cubeSat.getTemperature();
  temperatura_ds18b20 = sensorTemp.getTempCByIndex(0);

  Serial.print("x: ");
  Serial.println(temperatura_ds18b20);
  Serial.print("y: ");
  Serial.println(temperatura_pion);
  Serial.println();
  
  // Gravação no cartão SD
  cubeSat.logOnSDFile();
  delay(1000*2);
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
  const char * message = "temperatura_ds18b20, temperatura_pion";

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

  file.print(temperatura_ds18b20);
  file.write(',');
  file.println(temperatura_pion);
  // Fecha o arquivo
  file.close();
}
