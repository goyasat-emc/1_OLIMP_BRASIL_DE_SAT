#include "PION_System.h"
#include "FS.h"

System cubeSat;

float campoMagX, campoMagY, campoMagZ;

const unsigned int timerDelay_SD = 6000; // Intervalo de gravação de dados no cartão micro SD

void setup() {
  // Inicializa seu CubeSat, e seus periféricos
  cubeSat.init(true);

  cubeSat.createLogOnSD();
  Serial.println("CubeSat inicializado");
}

void loop() {
  getSensorsInfo();
  // Gravação no cartão SD
  cubeSat.logOnSDFile();
  delay(timerDelay_SD);
}

void getSensorsInfo() {
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
  const char * message = "horario(ms), campoMagX, campoMagY, campoMagZ";

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
  file.print(campoMagX);
  file.write(',');
  file.print(campoMagY);
  file.write(',');
  file.println(campoMagZ);
  // Fecha o arquivo
  file.close();
}
