import os
import warnings
import logging

# 1. Configurações de ambiente (C++ e Keras Legado)
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
os.environ['ABSL_LOGGING_MIN_LOG_LEVEL'] = '3'
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'
os.environ['TF_USE_LEGACY_KERAS'] = '1'

# 2. Silencia os filtros padrão do Python
warnings.filterwarnings('ignore', category=DeprecationWarning)
warnings.filterwarnings('ignore', category=UserWarning)

# 3. Força o logger interno do TensorFlow a aceitar apenas ERROS fatais
logging.getLogger('tensorflow').setLevel(logging.ERROR)

# 4. Configura o logger do TF assim que ele for importado
try:
    import tensorflow as tf # type: ignore
    tf.get_logger().setLevel('ERROR')
    tf.autograph.set_verbosity(0)
except Exception:
    pass

import cv2 # type: ignore
import serial # type: ignore
import time
import numpy as np
import requests  # type: ignore
import threading 

from classificador import ClassificadorIA
from interface import TelaCustomizada

from constantes import (
    URL_ESP32, PORTA_SERIAL, URL_FOTO_SEM_FLASH,
    APROVADO, REJEITADO, REINICIAR, INICIAR, RESETAR_SISTEMA,
    AGUARDANDO_SENSOR, PROCESSANDO_IA, MONITORANDO_SAIDA,
    AGUARDANDO_ESTABILIZACAO, BUSCANDO_IMAGEM_HTTP    
)

# Inicialização da Serial com o Arduino Uno
try:
    arduino = serial.Serial(PORTA_SERIAL, 115200, timeout=0.1)
    print("Conectando ao Arduino Uno...")
    time.sleep(3) 
    arduino.reset_input_buffer() 
    print("Sucesso: Arduino Uno conectado!")
except:
    print(f"Erro: Não foi possível abrir a porta {PORTA_SERIAL}")
    exit()

ia = ClassificadorIA()
tela = TelaCustomizada(ia)

cap = cv2.VideoCapture(URL_ESP32)
status_sistema = AGUARDANDO_SENSOR
ultimo_resultado = "" 
geracao_sistema = 0

# Controle do tempo não-bloqueante
tempo_gatilho = 0

print("Sistema de Qualidade IA Ativo e Monitorando (MODO DE OUTSOURCING DE SENSOR)...")

def capturar_foto_thread(geracao_thread):
    global status_sistema
    global ultimo_resultado
    global geracao_sistema

    """Busca a foto em segundo plano e injeta no fluxo da IA"""
    global status_sistema, ultimo_resultado
    try:
        print("[SISTEMA] Capturando foto...")
        resposta = requests.get(URL_FOTO_SEM_FLASH, timeout=10)
        
        if resposta.status_code == 200:
            # Converte os bytes recebidos do HTTP em uma imagem OpenCv (Mat)
            img_array = np.frombuffer(resposta.content, dtype=np.uint8)
            img = cv2.imdecode(img_array, cv2.IMREAD_COLOR)

            if geracao_thread != geracao_sistema:
                print("[SISTEMA] Captura descartada.")
                return
            
            print("[SISTEMA] Foto capturada! Ativando análise da IA...")
            resultado, index = ia.processar_imagem(img)

            if geracao_thread != geracao_sistema:
                print("[SISTEMA] Resultado descartado.")
                return

            ultimo_resultado = resultado

            if resultado == ia.resultado_aprovado:
                arduino.write(bytes(APROVADO, 'utf-8'))
                print(">>> ITEM APROVADO PELO MODELO EXTERNO")
            else:
                arduino.write(bytes(REJEITADO, 'utf-8'))
                print(">>> ITEM REJEITADO PELO MODELO EXTERNO")
            
            status_sistema = MONITORANDO_SAIDA
        else:
            print(f"[ERRO] Erro na requisição HTTP do ESP32: {resposta.status_code}")
            status_sistema = AGUARDANDO_SENSOR
            
    except Exception as e:
        print(f"[ERRO] Falha ao capturar imagem com flash: {e}")
        status_sistema = AGUARDANDO_SENSOR

def reiniciar_sistema_completo():
    global status_sistema
    global ultimo_resultado
    global tempo_gatilho
    global geracao_sistema

    geracao_sistema += 1
    status_sistema = AGUARDANDO_SENSOR
    ultimo_resultado = ""
    tempo_gatilho = 0

    arduino.reset_input_buffer()
    print("[SISTEMA] Reinicialização completa solicitada pelo Arduino.")

while True:
    success, img = cap.read()
    if not success:
        print("Erro: Falha ao receber streaming do ESP32-CAM")
        break

    # 2. Escuta e monitoramento da Serial (Fluxo de Entrada e Saída)
    if arduino.in_waiting > 0:
        try:
            linha_arduino = arduino.readline().decode('utf-8').strip()
            if linha_arduino:
                print(f"[ARDUINO]: {linha_arduino}")

                if linha_arduino == RESETAR_SISTEMA:
                    reiniciar_sistema_completo()
                    continue
                
                # GATILHO DE ENTRADA: Sensor detectou o objeto
                if status_sistema == AGUARDANDO_SENSOR and linha_arduino == INICIAR:
                    print("\n[EVENTO] Carrinho detectado pelo sensor!")
                    print("[SISTEMA] Aguardando estabilização...")
                    tempo_gatilho = time.time()  # Marca o tempo atual
                    status_sistema = AGUARDANDO_ESTABILIZACAO
                
                # GATILHO DE SAÍDA: Arduino avisou que o objeto saiu da frente do sensor
                elif status_sistema == MONITORANDO_SAIDA and linha_arduino == REINICIAR:
                    print("[EVENTO] Arduino reportou que o carrinho saiu. Resetando Python...")
                    status_sistema = AGUARDANDO_SENSOR
                    arduino.write(bytes(REINICIAR, 'utf-8'))
                    arduino.reset_input_buffer()
                    
        except Exception as e:
            pass

    # Lógica de espera temporal não-bloqueante
    if status_sistema == AGUARDANDO_ESTABILIZACAO:
        if time.time() - tempo_gatilho >= 1.5:  # Se passou 1.5 segundos
            print("[SISTEMA] Pista limpa! Preparando para congelar imagem...")
            status_sistema = PROCESSANDO_IA

    # 3. Lógica de Inteligência Artificial
    if status_sistema == PROCESSANDO_IA:
        # Altera temporariamente o estado para evitar múltiplos disparos da Thread
        status_sistema = BUSCANDO_IMAGEM_HTTP
        
        geracao_thread = geracao_sistema
        # Dispara a função de requisição em paralelo para não congelar o monitor principal
        threading.Thread(
            target=capturar_foto_thread,
            args=(geracao_thread,),
            daemon=True
        ).start()

    # Chamada do módulo externo para gerar e exibir o feedback visual
    # Se estiver processando em segundo plano, enviamos o frame atual do streaming apenas para a tela
    img_display = tela.desenhar_monitor(img, status_sistema, ultimo_resultado)
    cv2.imshow("Monitor de Inspecao - Fabrica IA (Modo Teste Real)", img_display)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
arduino.close()