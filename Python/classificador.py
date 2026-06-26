import time
import random
import configparser
import os
import sys
import numpy as np

class ClassificadorIA:
    def __init__(self):
        self.config = configparser.ConfigParser()
        self.config_file = 'config.ini'
        
        # Valores iniciais padrão
        self.usar_simulador = True
        self.salvar_log_ia = True
        self.modelo_path = "IA/modelo_ia_atualizado.keras"
        self.labels_path = "IA/labels.txt"
        
        self._carregar_configuracoes()

        self.resultado_aprovado = "APROVADO"
        self.resultado_rejeitado = "REJEITADO"
        self.labels_ia = [self.resultado_aprovado, self.resultado_rejeitado]
        
        if self.usar_simulador:
            print("[MÓDULO IA] Classificador inicializado em modo de SIMULAÇÃO.")
        else:
            print("[MÓDULO IA] Inicializando IA REAL com arquivos locais...")
            self._inicializar_ia_real()

    def _carregar_configuracoes(self):
        if os.path.exists(self.config_file):
            try:
                self.config.read(self.config_file)
                self.usar_simulador = self.config.getboolean('CONFIGURACOES', 'usar_simulador', fallback=self.usar_simulador)
                self.salvar_log_ia = self.config.getboolean('CONFIGURACOES', 'salvar_log_ia', fallback=self.salvar_log_ia)
                
                self.modelo_path = self.config.get('MODELO_IA', 'modelo_path', fallback=self.modelo_path)
                self.labels_path = self.config.get('MODELO_IA', 'labels_path', fallback=self.labels_path)
            except Exception as e:
                print(f"[ERRO CRÍTICO] Falha ao ler o arquivo {self.config_file}: {e}")
                sys.exit(1)
        else:
            print(f"[AVISO] Arquivo {self.config_file} não encontrado. Criando um padrão...")
            self.config['CONFIGURACOES'] = {
                'usar_simulador': str(self.usar_simulador),
                'salvar_log_ia': str(self.salvar_log_ia)
            }
            self.config['MODELO_IA'] = {
                'modelo_path': self.modelo_path, 
                'labels_path': self.labels_path
            }
            with open(self.config_file, 'w') as configfile:
                self.config.write(configfile)

    def _inicializar_ia_real(self):
        try:
            import tensorflow as tf # type: ignore
            
            if os.path.exists(self.modelo_path) and os.path.exists(self.labels_path):
                self.classificador_real = tf.keras.models.load_model(self.modelo_path, compile=False)
                
                with open(self.labels_path, "r") as f:
                    self.classes_txt = [linha.strip().split(" ", 1)[-1].upper() for linha in f.readlines()]
                
                print("[MÓDULO IA] IA REAL carregada com sucesso nativamente via TensorFlow!")
            else:
                print(f"[ERRO CRÍTICO] Arquivos da IA não encontrados!")
                sys.exit(1) 
                
        except ImportError:
            print("[ERRO CRÍTICO] Bibliotecas necessárias não instaladas!")
            sys.exit(1) 

    def processar_imagem(self, img):
        # --- MODO IA REAL ---
        if not self.usar_simulador:
            import cv2 # type: ignore
            
            # 1. Redimensiona o frame VGA para 224x224 (Padrão Teachable Machine)
            img_redimensionada = cv2.resize(img, (224, 224), interpolation=cv2.INTER_AREA)
            
            # 2. Converte para RGB e normaliza a imagem
            img_rgb = cv2.cvtColor(img_redimensionada, cv2.COLOR_BGR2RGB)
            image_array = np.asarray(img_rgb)
            normalized_image_array = (image_array.astype(np.float32) / 127.5) - 1
            
            # 3. Prepara o shape para o TensorFlow
            data = np.ndarray(shape=(1, 224, 224, 3), dtype=np.float32)
            data[0] = normalized_image_array
            
            # 4. Faz a previsão matemática
            prediction = self.classificador_real.predict(data, verbose=0)
            index_real = np.argmax(prediction)
            resultado = self.labels_ia[index_real]

            # Log na tela
            print("\n [DIAGNÓSTICO DA IA] Nível de certeza do cérebro:")
            for i, confianca in enumerate(prediction[0]):
                nome_classe = self.classes_txt[i] if hasattr(self, 'classes_txt') else f"Classe {i}"
                print(f"   -> {nome_classe}: {confianca * 100:.2f}%")
            
            # SALVAR FOTOS DA IA
            if self.salvar_log_ia:              
                pasta_erros = "IA/Erros_Diagnostico"
                os.makedirs(pasta_erros, exist_ok=True)
                nome_foto_erro = f"{pasta_erros}/ia_viu_{resultado}_{int(time.time())}.jpg"
                cv2.imwrite(nome_foto_erro, img)
                print(f" [DIAGNÓSTICO] Foto guardada em: {nome_foto_erro}\n")
            # ------------------------------------------------------------------

            return resultado, index_real

        # --- MODO SIMULADOR ---
        time.sleep(0.5) 
        index_simulado = random.choice([0, 1])
        resultado = self.labels_ia[index_simulado]
        
        return resultado, index_simulado