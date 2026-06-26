#include "esp_camera.h"
#include <WiFi.h>
#include "config.h"

// Definição dos pinos da câmera (Modelo AI-Thinker)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

void startCameraServer();

void setup() {
  Serial.begin(115200);
  delay(1000);  // Pequeno delay para estabilizar o monitor serial

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
  config.xclk_freq_hz = 15000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Ajuste de qualidade e memória
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;  // 640x480 (perfeito para o Python)
    config.jpeg_quality = 14;           // Ajuste fino de compressão
    config.fb_count = 3;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 14;
    config.fb_count = 1;
  }

  // Inicializa a Câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Erro na câmera: 0x%x\n", err);
    return;
  }

  // CONEXÃO INTELIGENTE DE WI-FI
  IPAddress ip;
  ip.fromString(ENDERECO_IP_DO_SISTEMA);

  IPAddress gateway;
  gateway.fromString(GATEWAY_DO_SISTEMA);

  IPAddress subnet;
  subnet.fromString(MASCARA_DE_REDE);

  if (CRIAR_REDE_PROPRIA == true) {
    Serial.println("Configurando Ponto de Acesso...");

    if (USAR_IP_FIXO) {
      gateway = ip;
      IPAddress subnet(255, 255, 255, 0);

      if (!WiFi.softAPConfig(ip, gateway, subnet)) {
        Serial.println("Erro ao configurar IP fixo do Access Point!");
      }
    }

    WiFi.softAP(NOME_DA_REDE_DO_ESP32, SENHA_DA_REDE_DO_ESP32);

    Serial.println("\n[SUCESSO] Rede Wi-Fi Criada!");
    Serial.print("Conecte seu computador na rede: ");
    Serial.println(NOME_DA_REDE_DO_ESP32);

    Serial.print("IP para colocar no Python: http://");

    if (USAR_IP_FIXO)
      Serial.print(ip);
    else
      Serial.print(WiFi.softAPIP());

    Serial.println(":81/stream");

  } else {
    Serial.print("Conectando na rede: ");
    Serial.println(NOME_DO_WIFI_DE_CASA);

    if (USAR_IP_FIXO) {
      IPAddress dns(8, 8, 8, 8);

      if (!WiFi.config(ip, gateway, subnet, dns)) {
        Serial.println("Falha ao configurar IP fixo!");
      }
    }

    WiFi.begin(NOME_DO_WIFI_DE_CASA, SENHA_DO_WIFI_DE_CASA);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("\n[SUCESSO] Wi-Fi Conectado!");

    Serial.print("IP para colocar no Python: http://");

    if (USAR_IP_FIXO)
      Serial.print(ip);
    else
      Serial.print(WiFi.localIP());

    Serial.println(":81/stream");
  }

  // Inicia o servidor de streaming
  startCameraServer();
}

void loop() {
  delay(10000);  // O streaming roda em segundo plano
}