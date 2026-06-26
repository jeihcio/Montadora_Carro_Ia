// config.h - PAINEL DE CONFIGURAÇÕES DO SISTEMA

#ifndef CONFIG_H
#define CONFIG_H

// ====================================================================
// ESCOVA DE MODO DE REDE (Como o ESP32 vai se comportar)
// ====================================================================

// Se você deixar como "true", o ESP32 vai criar a sua própria rede Wi-Fi (Modo AP).
// Se você mudar para "false", ele vai tentar se conectar no roteador da sua casa (Modo Estação).
#define CRIAR_REDE_PROPRIA true


// ====================================================================
// CONFIGURAÇÃO DA REDE PRÓPRIA (Se a opção acima for "true")
// ====================================================================

// O nome da rede Wi-Fi que o ESP32 vai criar no seu celular/notebook
#define NOME_DA_REDE_DO_ESP32 "Fabrica_IA"

// A senha para conseguir conectar na rede que o ESP32 criou (mínimo 8 caracteres)
#define SENHA_DA_REDE_DO_ESP32 "senhadificil"


// ====================================================================
// CONFIGURAÇÃO DO WI-FI DE CASA (Se a opção lá de cima for "false")
// ====================================================================

// O nome exato da rede Wi-Fi da sua casa ou do laboratório
#define NOME_DO_WIFI_DE_CASA ""

// A senha do Wi-Fi da sua casa para o ESP32 conseguir internet
#define SENHA_DO_WIFI_DE_CASA ""


// ====================================================================
// ENDEREÇO DO SISTEMA (IP Fixo para facilitar o Python)
// ====================================================================

// true = usa o IP definido abaixo
// false = deixa o WiFi obter o IP automaticamente
#define USAR_IP_FIXO true
#define ENDERECO_IP_DO_SISTEMA "192.168.4.1"
#define GATEWAY_DO_SISTEMA "192.168.1.1"
#define MASCARA_DE_REDE "255.255.255.0"

#endif