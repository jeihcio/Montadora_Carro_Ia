#include <IRremote.hpp>
#include <AccelStepper.h>

// Configurações e Constantes
const int VelocidadePadraoEsteira = 300;
const int VelocidadeMaximaEsteira = 800;
const int VelocidadeMinimaEsteira = 50;

// Botão do controle remoto
unsigned long BTN_LIGAR_DESLIGAR = 0xBC43FF00;
unsigned long BTN_LIGAR_DESLIGAR_SOM = 0xE916FF00;
unsigned long BTN_LIGAR_DESLIGAR_ESTEIRA = 0xF609FF00;

unsigned long BTN_ESTEIRA_FRENTE = 0xBB44FF00;
unsigned long BTN_ESTEIRA_TRAS = 0xBF40FF00;
unsigned long BTN_AUMENTAR_VELOCIDADE_ESTEIRA = 0xEA15FF00;
unsigned long BTN_DIMINUIR_VELOCIDADE_ESTEIRA = 0xF807FF00;

// Status de Comunicação
const char FazerNada = 'N';
const char Aprovado = 'G';
const char Rejeitado = 'R';
const char Reiniciar = 'X';
const char Iniciar = 'S';
const char ResetarSistema = 'Z';

// Pinos
const int pinoSensorIR = 6;  // Sensor Infravermelho de obstáculo
const int pinoLedVerde = 8;
const int pinoLedVermelho = 9;
const int pinoLedAzul = 11;
const int pinoBuzzer = 10;  // Buzina
const int pinoIR = 2;       // Controle remoto

const int pinoMotorIN1 = 3;   // Driver ULN2003
const int pinoMotorIN2 = 4;   // Driver ULN2003
const int pinoMotorIN3 = 5;   // Driver ULN2003
const int pinoMotorIN4 = 12;  // Driver ULN2003

// Definições para os métodos
const bool ACENDER = HIGH;
const bool APAGAR = LOW;

const int VERDE = pinoLedVerde;
const int VERMELHO = pinoLedVermelho;
const int AZUL = pinoLedAzul;

// Direção da esteira
const int ESTEIRA_FRENTE = -1;
const int ESTEIRA_TRAS = 1;

// Variáveis de Controle
bool sistemaLigado = false;
bool esteiraLigada = false;

bool somLigado = true;

bool emProcessamento = false;
bool envioComandoReiniciarRealizado = false;
char statusAtual = FazerNada;
static char ultimoStatus = 0;

int direcaoPadraoEsteira = ESTEIRA_FRENTE;
int direcaoEsteira = direcaoPadraoEsteira;
int velocidadeEsteira = VelocidadePadraoEsteira;

// Filtro estável para evitar bouncing físico
int leiturasObjetoPresente = 0;
int leiturasObjetoAusente = 0;
const int CONVENCAO_FILTRO = 5;

// Configurações
AccelStepper motorEsteira(AccelStepper::FULL4WIRE, pinoMotorIN1, pinoMotorIN3, pinoMotorIN2, pinoMotorIN4);

void setup() {
  Serial.begin(115200);

  pinMode(pinoSensorIR, INPUT);
  pinMode(pinoLedVerde, OUTPUT);
  pinMode(pinoLedVermelho, OUTPUT);
  pinMode(pinoLedAzul, OUTPUT);
  pinMode(pinoBuzzer, OUTPUT);

  controlarLed(AZUL, APAGAR);
  IrReceiver.begin(pinoIR, ENABLE_LED_FEEDBACK);

  motorEsteira.setMaxSpeed(VelocidadeMaximaEsteira);
  motorEsteira.setAcceleration(1000);
  motorEsteira.setSpeed(VelocidadePadraoEsteira);

  Serial.println("Sistema iniciado com ele desligado!");
}

void loop() {
  // Controle Remoto
  if (IrReceiver.decode()) {
    // Ignora repetição do botão
    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      unsigned long codigo = IrReceiver.decodedIRData.decodedRawData;

      //Serial.print("Codigo emitido pelo o controle remoto: ");
      //Serial.println(codigo, HEX);

      if (codigo == BTN_LIGAR_DESLIGAR) {
        sistemaLigado = !sistemaLigado;

        // Desliga tudo
        controlarLed(AZUL, APAGAR);
        controlarLed(VERDE, APAGAR);
        controlarLed(VERMELHO, APAGAR);

        // restaurar esteira
        esteiraLigada = false;
        direcaoEsteira = direcaoPadraoEsteira;
        velocidadeEsteira = VelocidadePadraoEsteira;

        // Limpa Serial
        while (Serial.available() > 0) {
          Serial.read();
        }

        if (sistemaLigado) {
          esteiraLigada = true;
          Serial.println("Sistema LIGADO");
          controlarLed(AZUL, ACENDER);
        } else {
          Serial.println("Sistema DESLIGADO");
          Serial.println(ResetarSistema);

          emProcessamento = false;
          envioComandoReiniciarRealizado = false;
          esteiraLigada = false;
          somLigado = true;
          statusAtual = FazerNada;
          leiturasObjetoPresente = 0;
          leiturasObjetoAusente = 0;
        }
      } else {
        if (sistemaLigado) {
          // Bloqueio de comandos do motor em processamento
          if (emProcessamento && statusAtual == FazerNada) {
            Serial.println("Nao eh possivel alterar o motor quando tiver em processamento!");
          } else if (codigo == BTN_ESTEIRA_FRENTE) {

            direcaoEsteira = ESTEIRA_FRENTE;
            Serial.println("Direcao da esteira: FRENTE");

          } else if (codigo == BTN_ESTEIRA_TRAS) {

            direcaoEsteira = ESTEIRA_TRAS;
            Serial.println("Direcao da esteira: TRAS");

          } else if (codigo == BTN_AUMENTAR_VELOCIDADE_ESTEIRA) {

            velocidadeEsteira += 50;
            if (velocidadeEsteira > VelocidadeMaximaEsteira) {
              velocidadeEsteira = VelocidadeMaximaEsteira;
            }
            Serial.print("Velocidade da esteira: ");
            Serial.println(velocidadeEsteira);

          } else if (codigo == BTN_DIMINUIR_VELOCIDADE_ESTEIRA) {

            velocidadeEsteira -= 50;
            if (velocidadeEsteira < VelocidadeMinimaEsteira) {
              velocidadeEsteira = VelocidadeMinimaEsteira;
            }
            Serial.print("Velocidade da esteira: ");
            Serial.println(velocidadeEsteira);

          } else if (codigo == BTN_LIGAR_DESLIGAR_ESTEIRA) {

            esteiraLigada = !esteiraLigada;
            if (esteiraLigada) {
              Serial.println("Esteira LIGADA");
            } else {
              Serial.println("Esteira DESLIGADA");
            }

          } else if (codigo == BTN_LIGAR_DESLIGAR_SOM) {
            somLigado = !somLigado;
            if (somLigado) {
              Serial.println("Som do sistema: LIGADO!");
            } else {
              Serial.println("Som do sistema: DESLIGADO!");
            }
          }
        }
      }
    }
    IrReceiver.resume();
  }

  if (!sistemaLigado) {
    delay(50);
    return;
  }

  // CONTROLE DA ESTEIRA: Ela obedece ao estado do fluxo
  if (esteiraLigada) {
    ligarEsteira(velocidadeEsteira, direcaoEsteira);
  } else {
    desligarEsteira();
  }

  // RECEBE COMANDO DO PYTHON PRIMEIRO
  if (Serial.available() > 0) {
    char comando = Serial.read();

    if (comando == '\n' || comando == '\r') {
      return;
    }

    if (comando == FazerNada || comando == Aprovado || comando == Rejeitado || comando == Reiniciar || comando == Iniciar) {
      statusAtual = comando;
      Serial.print("Recebemos um comando valido: ");
      Serial.println(statusAtual);
    } else {
      Serial.print("Recebemos um comando invalido: ");
      Serial.println(comando);
    }
  }

  bool objetoDetectado = verificarObjetoPresente();

  // Filtro de estabilização do sensor IR
  if (objetoDetectado) {
    leiturasObjetoPresente++;
    leiturasObjetoAusente = 0;
  } else {
    leiturasObjetoAusente++;
    leiturasObjetoPresente = 0;
  }

  // GATILHO DE ENTRADA: Só aceita se NÃO estiver em processamento E se o status não for G ou R latente
  if (leiturasObjetoPresente >= CONVENCAO_FILTRO && !emProcessamento && statusAtual == FazerNada) {
    Serial.println("Sensor detectou a entrada do objeto!");
    Serial.println(Iniciar);

    emProcessamento = true;
    envioComandoReiniciarRealizado = false;
    esteiraLigada = false;  // TRAVA E PARA A ESTEIRA IMEDIATAMENTE
    leiturasObjetoPresente = 0;
  }

  // CONTROLE DE SAÍDA ASSÍNCRONO: Monitora quando o objeto sai e avisa o Python
  if (leiturasObjetoAusente >= CONVENCAO_FILTRO && emProcessamento) {
    if (!envioComandoReiniciarRealizado && (statusAtual == Aprovado || statusAtual == Rejeitado)) {
      Serial.println("Sensor detectou a saida do objeto! Avisando Python...");
      Serial.println(Reiniciar);

      envioComandoReiniciarRealizado = true;
      leiturasObjetoAusente = 0;
    }
  }

  // Printar status na mudança
  if (statusAtual != ultimoStatus) {
    ultimoStatus = statusAtual;

    if (statusAtual == Aprovado) {
      Serial.println("Status -> Aprovado!");
    } else if (statusAtual == Rejeitado) {
      Serial.println("Status -> Reprovado!");
    } else if (statusAtual == Reiniciar) {
      Serial.println("Status -> Reiniciar as verificacoes!");
    }
  }

  // Executa os efeitos visuais/sonoros da IA (enquanto o objeto ainda estiver na estação)
  if (statusAtual == Aprovado) {

    controlarLed(VERDE, ACENDER);
    controlarLed(VERMELHO, APAGAR);

  } else if (statusAtual == Rejeitado) {

    bool piscarLed = (millis() / 500) % 2;

    controlarLed(VERDE, APAGAR);
    controlarLed(VERMELHO, piscarLed);

    if (piscarLed) {
      emitirSom(somLigado, 600, 100);
    }

  } else if (statusAtual == Reiniciar) {

    // REINICIALIZAÇÃO CONFIGURADA: Só acontece aqui quando o Python de fato envia o caractere 'X'
    controlarLed(VERDE, APAGAR);
    controlarLed(VERMELHO, APAGAR);

    esteiraLigada = true;
    emProcessamento = false;
    envioComandoReiniciarRealizado = false;
    statusAtual = FazerNada;

    delay(1000);
    leiturasObjetoPresente = 0;
    leiturasObjetoAusente = 0;
  }
}