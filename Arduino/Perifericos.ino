void controlarLed(int cor, bool acao) {
  digitalWrite(cor, acao);
}

void emitirSom(bool somLigado, int frequencia, int duracao) {
  if (!somLigado) return;

  // Calcula o tempo de espera para a frequência desejada
  long atraso = 1000000 / frequencia / 2;
  long ciclos = (long)frequencia * duracao / 1000;

  for (long i = 0; i < ciclos; i++) {
    digitalWrite(pinoBuzzer, HIGH);
    delayMicroseconds(atraso);
    digitalWrite(pinoBuzzer, LOW);
    delayMicroseconds(atraso);
  }
}

bool verificarObjetoPresente() {
  return !digitalRead(pinoSensorIR);
}

void ligarEsteira(int velocidadeEsteira, int direcaoEsteira) {
  motorEsteira.setSpeed(velocidadeEsteira * direcaoEsteira);
  motorEsteira.runSpeed();
}

void desligarEsteira() {
  motorEsteira.disableOutputs();
}