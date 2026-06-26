import os

# Força o TensorFlow a usar o formato antigo apenas para carregar o arquivo
os.environ["TF_USE_LEGACY_KERAS"] = "1"

import tensorflow as tf # type: ignore

# Caminhos dos arquivos (ajuste se os nomes forem diferentes)
modelo_antigo = "keras_model.h5"
modelo_novo = "modelo_ia_atualizado.keras"

print("Corrigindo a estrutura do cérebro da IA para o Python 3.13...")

if not os.path.exists(modelo_antigo):
    print(f"[ERRO] O arquivo '{modelo_antigo}' não foi encontrado nesta pasta!")
    exit()

try:
    # Carrega usando o motor legado interno do TensorFlow
    model = tf.keras.models.load_model(modelo_antigo, compile=False)
    
    # Salva no formato moderno universal (.keras)
    model.save(modelo_novo)
    print("\n=============================================")
    print("       SINDROME DE VERSÃO CORRIGIDA!         ")
    print("=============================================")
    print(f" ✅ Novo arquivo gerado: {modelo_novo}")
    print(" Ele está pronto para rodar no seu código novo.")
    print("=============================================")
except Exception as e:
    print(f"\n[ERRO CRÍTICO AO CONVERTER]: {e}")