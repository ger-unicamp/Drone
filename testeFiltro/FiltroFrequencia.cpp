#include <math.h>

class filtroFrequencia //Filtra um sinal discreto, atenuando as frequências de ruído
{
  protected:
    double saidaAnt, //Última saída do filtro
          constante, //Constante do filtro
          saida, //Saída atual do filtro
          freqCorte, //Frequência de corte do filtro (rad/s)
          amostragem; //Amostragem do sinal
  
  public:
    filtroFrequencia(double freqCorte, double amostragem)
    { 
      this->freqCorte = freqCorte * 2.0 * 3.14159 / 60;
      this->amostragem = amostragem;
      
      calculaConstante();

      saidaAnt = 0; 
    }

    void calculaConstante() //Calcula a constante (alpha) do filtro
    {
      constante = 1 - exp(freqCorte * 1/amostragem);//exp( freqCorte * amostragem);
    }
    
    void entrar(double medida) //Insere uma nova leitura no filtro e calcula a saída
    {
      saida = (constante * medida) +   ((1+constante) * saidaAnt);
      saidaAnt = saida;
    }

    double sair() //Retorna o valor filtrado
    {
      return (-1)*saida;
    }

    void setFrequencia(double freqCorte) //Altera a frequência de corte
    {
      this-> freqCorte = freqCorte;
      calculaConstante();
    }

    void setAmostragem(double amostragem) //Altera a amostragem
    {
      this-> amostragem = amostragem;
      calculaConstante();
    }
};
