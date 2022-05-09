#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <EEPROM.h>            // read and write from flash memory

//<Assfalk@ufg.br
#include <WiFi.h>

// Rede WiFi na qual o CubeSat deve se conectar e o host (serverName) que será procurado para envio de dados
//const char* ssid = "OBSAT_WIFI";
//const char* net_password = "OBSatZenith1000";
//const char* serverName = "http://192.168.0.1/";

const char* ssid = "";
const char* net_password = "";
//>Assfalk@ufg.br

extern void initEspNow();
extern void sendEspNow();

extern uint8_t take;

unsigned long lastTime = 0;
// Cada foto deve ser tirada em um intervalo específico
// 4 minutos = 240000ms
unsigned long timerDelay = 15000;

// Define o número de bytes que deseja-se acessar
#define EEPROM_SIZE 1

// Definição de pinos para o modelo CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

extern int numeroImg = 0;
camera_fb_t * fb;

/* ----------Trecho necessário para o bom funcionamento do esp_now---------- */
#include <esp_now.h>
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
/* ------------------------------------------------------------------------- */

void setup() {
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  //Serial.println();

  Serial.println("Iniciando câmera");
  initCamera();

  //<Assfalk@ufg.br
  // Configura WiFi no modo station
  Serial.println("Iniciando conexão Wifi");
  WiFi.begin(ssid, net_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("Conexão concluída");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  //>Assfalk@ufg.br

  Serial.println("Iniciando ESP-NOW");
  initEspNow();

  Serial.println("Setup terminado");
  lastTime = millis();
}

void initCamera() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //desabilita detector de brownout

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Inicialização da câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //Serial.println("Starting SD Card");
  if (!SD_MMC.begin()) {
    Serial.println("SD Card Mount Failed");
    return;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return;
  }

  camera_fb_t * fb = NULL;
}

void takePhoto() {
  // Captura imagem com a câmera
  Serial.println("Início de captura");
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  // Inicializa EEPROM com o tamanho definido anteriormente
  EEPROM.begin(EEPROM_SIZE);
  numeroImg = EEPROM.read(0) + 1;

  // Caminho dentro do cartão de memório, no qual a imagem será salva
  String path = "/picture" + String(numeroImg) + ".jpg";

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());

  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Arquivo salvo no caminho: %s\n", path.c_str());
    EEPROM.write(0, numeroImg);
    EEPROM.commit();
    take = 1; // Indicação de foto tirada
    sendEspNow();
  }
  take = 0; // Retorno para o status padrão
  file.close();
  esp_camera_fb_return(fb);

  delay(2000);
}


void loop() {
  // O tempo entre cada foto é definido pela variável 'timerDelay'
  if ((millis() - lastTime) > timerDelay) {
    takePhoto(); // Neste método, uma foto é tirada, salva e informações são enviadas para o CubeSat
    lastTime = millis();
    delay(1000);     
  }
 
}
