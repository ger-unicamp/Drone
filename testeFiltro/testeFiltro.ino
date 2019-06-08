#include "FiltroFrequencia.cpp"

filtroFrequencia filtro = filtroFrequencia(20,100);
float dadoLido, dadoFiltrado;

void setup() {
  delay(200);
  dadoLido = dadoFiltrado = 0;
  Serial.begin(9600);
}

void loop() {
  dadoLido      = analogRead(A0);
  filtro.entrar(dadoLido);
  dadoFiltrado  = filtro.sair();
  Serial.print(dadoLido);
  Serial.print(" ");
  Serial.println(dadoFiltrado);

}
