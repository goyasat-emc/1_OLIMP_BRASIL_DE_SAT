#include "PION_System.h"
#include <OneWire.h>
#include <DallasTemperature.h>

const int entrada_sensores_temp = 13;

OneWire oneWire(entrada_sensores_temp);  /*Protocolo OneWire*/

/********************************************************************/
DallasTemperature sensorTemp(&oneWire); /*encaminha referências OneWire para o sensor*/
System cubeSat;

void setup() {
  // Inicializa seu CubeSat, e seus periféricos
  cubeSat.init(true);
  pinMode(entrada_sensores_temp, INPUT);

  sensorTemp.begin(); /*inicia biblioteca*/
}

void loop() {
  sensorTemp.requestTemperatures(); /* Envia o comando para leitura da temperatura */

  for(int n=0; n<5; n++){
    Serial.print("Sensor ");
    Serial.print(n);
    Serial.print(": ");
    Serial.println(sensorTemp.getTempCByIndex(n)); /* Endereço do sensor */
  }
  
  Serial.println();
  delay(300);
}
