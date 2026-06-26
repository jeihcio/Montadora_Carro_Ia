# Projeto de IA para Montadora de Carros - Maquete de Feira de Ciências

## 📋 Descrição do Projeto

Este projeto é uma demonstração prática de como integrar **Inteligência Artificial** em uma montadora de carros em formato de maquete para feira de ciências. O sistema utiliza visão computacional para verificar a qualidade dos carrinhos em uma esteira de produção automatizada.

### Objetivo Principal
Treinar uma IA para verificar e validar os carrinhos produzidos na maquete, utilizando a plataforma **Google Teachable Machine** (https://teachablemachine.withgoogle.com/) para o treinamento do modelo de visão.

---

## 🤖 Treinamento da IA com Google Teachable Machine

### Como Treinar o Modelo:

1. Acesse https://teachablemachine.withgoogle.com/
2. Clique em **"Get Started"** e selecione **"Image Project"**
3. Crie duas classes:
   - **Classe 1**: "APROVADOS" (carrinhos sem defeitos)
   - **Classe 2**: "REJEITADOS" (carrinhos com problemas)
4. Capture ou faça upload de imagens dos carrinhos para cada classe
5. Clique em **"Train Model"** e aguarde o treinamento
6. Após o treinamento, exporte o modelo para uso offline

### Conversão do Modelo

Na pasta `python/ia/consertar_modelo`, há um script de conversão que transforma os arquivos exportados do Teachable Machine para o formato compatível com nosso projeto. Essa conversão é necessária porque o Teachable Machine exporta modelos em formato específico que precisa ser adaptado para uso em Python com as bibliotecas de visão computacional que utilizamos.

---

## 📁 Estrutura do Projeto

```
Projeto/
├── arduino/
│   └── arduino.ino (Controlador da esteira, controle remoto, leds, som e botões)
├── esp32-cam/
│   ├── esp32-cam.ino
│   └── config.h (Configurações personalizadas)
├── python/
│   ├── main.py
│   ├── config.ini (Configurações personalizadas)
│   └── ia/
│       └── consertar_modelo/ (Conversão do modelo Teachable Machine)
└── assets/
    └── drivers/ (Driver para ESP32 funcionar no PC)
```

---

## ⚙️ Configurações Personalizadas

### Arduino (arduino.ino)

No arquivo `arduino.ino` você pode personalizar:
- **Velocidade da esteira**: Ajuste o PWM enviado ao motor
- **Pinos de conexão**: Defina os pinos para sensores, botões e motor
- **Botões do controle remoto**: Configure os botões do controle remoto
- **Comunicação Serial**: use `Serial.begin(115200);` no sketch e configure o Monitor Serial para 115200 para visualizar as mensagens

**Exemplo de configurações:**
```cpp
#define MOTOR_PIN 9
#define BOTAO_INICIAR 2
#define BOTAO_PARAR 3
#define VELOCIDADE_ESTEIRA 200  // 0-255
```

### ESP32-CAM (config.h)

No arquivo `config.h` da pasta `esp32-cam/`, configure:
- Credenciais WiFi (SSID e senha)
- Resolução da câmera
- Qualidade da transmissão
- IP estático (opcional)

### Python (config.ini)

No arquivo `python/config.ini`, personalize:
- Porta serial de comunicação com Arduino
- Velocidade de transmissão (baud rate)
- Caminhos dos modelos de IA
- Parâmetros de detecção (threshold, etc.)

---

## 🔧 Configuração do ESP32 no PC

### Passo 1: Instalar o Driver
1. Procure a pasta **`assets/drivers/`** neste repositório
2. Dentro dela, encontre o driver do chipset CH340 (ou similar)
3. Instale o driver seguindo as instruções fornecidas

### Passo 2: Adicionar Suporte ESP32 no Arduino IDE
1. Abra **Arduino IDE** → **Arquivo** → **Preferências**
2. Em "URLs adicionais de Gerenciador de Placas", adicione:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Vá em **Ferramentas** → **Placa** → **Gerenciador de Placas**
4. Procure por "ESP32" e instale a versão mais recente

### Passo 3: Selecionar a Placa
- **Ferramentas** → **Placa** → Selecione **"AI Thinker ESP32-CAM"** (ou seu modelo específico)
- Defina a porta COM correta

---

## 📚 Bibliotecas Necessárias para Arduino

Instale as seguintes bibliotecas via **Gerenciador de Bibliotecas** (Sketch → Incluir Biblioteca → Gerenciar Bibliotecas):

1. **ESP32 by Espressif Systems** - Suporte básico do ESP32
2. **ArduinoJson** - Para processamento JSON
3. **WiFi** - Comunicação por rede (já incluída no ESP32)

**Para Arduino tradicional (não ESP32):**
- Biblioteca padrão do Arduino (já vem instalada)

---

## 🔌 Esquema de Montagem - Arduino

### Componentes Necessários:
- 1x Arduino Uno/Nano
- 1x Motor DC 12V
- 1x Módulo relé para controlar o motor
- 2x Botões (Start/Stop)
- 3x LEDs (opcional - para indicadores)
- Resistores, fios e jumpers

### Diagrama de Pinos:

```
Arduino PIN LAYOUT:
┌─────────────────────────────┐
│       Arduino Uno           │
│                             │
│  D2  ────→ Botão Iniciar    │
│  D3  ────→ Botão Parar      │
│  D9  ────→ Motor (PWM)      │
│  D13 ────→ LED Status       │
│           (opcional)        │
│  GND ────→ GND comum        │
│  5V  ────→ VCC              │
│                             │
│  A0  ────→ Sensor (opt.)    │
└─────────────────────────────┘

Módulo Relé:
IN  ←── D9 (Arduino)
GND ←── GND (Arduino)
VCC ←── 5V (Arduino)

Motor DC:
+12V ←── Relé
GND  ←── GND comum
```

---

## 🚀 Como Usar o Projeto

1. **Treinar o modelo**: Use Google Teachable Machine conforme descrito acima
2. **Converter o modelo**: Execute o script em `python/ia/consertar_modelo/`
3. **Configurar hardware**: Monte o Arduino com os pinos conforme o diagrama
4. **Instalar driver ESP32**: Siga os passos da seção "Configuração do ESP32"
5. **Carregar código**: Faça upload do código Arduino e ESP32 usando Arduino IDE
6. **Executar programa Python**: 
   ```bash
   python python/main.py
   ```

---

## 📝 Notas Importantes

- Garanta que todos os pinos estejam conectados corretamente antes de energizar
- O modelo de IA precisa ser treinado com imagens diversas para melhor precisão
- A conversão do modelo (consertar_modelo) é essencial para compatibilidade
- Sempre teste em ambiente seguro antes de operação contínua

---

## 👨‍💼 Autor

Projeto desenvolvido como demonstração de IA aplicada em processos industriais para feira de ciências.
