import cv2 # type: ignore

from constantes import AGUARDANDO_SENSOR, PROCESSANDO_IA

class TelaCustomizada:
    def __init__(self, ia_referencia):
        # Armazena a referência para checar as strings de aprovado/rejeitado
        self.ia = ia_referencia

    def desenhar_monitor(self, img, status_sistema, ultimo_resultado):
        """Desenha a barra superior e o texto com base no estado do sistema."""
        img_display = img.copy()
        
        # Determina o texto e a cor com base no status do sistema
        if status_sistema == AGUARDANDO_SENSOR:
            cor_txt = (0, 255, 0) # Verde
            texto_exibir = f"SISTEMA: {AGUARDANDO_SENSOR}"
            
        elif status_sistema == PROCESSANDO_IA:
            cor_txt = (0, 255, 255) # Amarelo
            texto_exibir = f"SISTEMA: {PROCESSANDO_IA}"
            
        else:
            # Estado MONITORANDO_SAIDA: usa o resultado definitivo da IA
            if ultimo_resultado == self.ia.resultado_aprovado:
                cor_txt = (0, 255, 0) # Verde
            else:
                cor_txt = (0, 0, 255) # Vermelho
            texto_exibir = f"SISTEMA: {ultimo_resultado}"
            
        # Substitui os underlines por espaços apenas na string que vai para a tela
        texto_exibir = texto_exibir.replace("_", " ")
        
        # Renderiza a barra preta superior e o texto
        cv2.rectangle(img_display, (0, 0), (550, 50), (0, 0, 0), -1)
        cv2.putText(img_display, texto_exibir, (10, 35), cv2.FONT_HERSHEY_SIMPLEX, 0.7, cor_txt, 2)
        
        return img_display