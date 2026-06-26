# constantes.py
import configparser

config = configparser.ConfigParser()
config.read('config.ini', encoding='utf-8')

# CONFIGURAÇÕES DE CONEXÃO
IP_REDE = config['ESP32-CAM']['ip']
URL_ESP32 = f'http://{IP_REDE}:81/stream'
PORTA_SERIAL = config['ARDUINO']['porta_serial']

# STATUS DE COMUNICAÇÃO SERIAL (Arduino <-> Python)
APROVADO  = 'G'
REJEITADO = 'R'
REINICIAR = 'X'
INICIAR   = 'S'
RESETAR_SISTEMA = 'Z'

# STATUS DE FLUXO DO SISTEMA PYTHON
AGUARDANDO_SENSOR = "AGUARDANDO_SENSOR"
PROCESSANDO_IA    = "PROCESSANDO_IA"
MONITORANDO_SAIDA = "MONITORANDO_SAIDA"

# Configuração do IP do ESP32-CAM
IP_ESP32 = URL_ESP32.split("//")[1].split(":")[0] 
URL_FOTO_SEM_FLASH = f"http://{IP_ESP32}/capture"
URL_FOTO_FLASH = f"http://{IP_ESP32}/capturar_com_flash"

# Variáveis para controle do tempo não-bloqueante
AGUARDANDO_ESTABILIZACAO = "AGUARDANDO_ESTABILIZACAO"
BUSCANDO_IMAGEM_HTTP = "BUSCANDO_IMAGEM_HTTP"