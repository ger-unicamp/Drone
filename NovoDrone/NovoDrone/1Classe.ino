/// <summary> Realiza o controle de um motor </summary>
/// <remarks> Elton, 14/08/2019. </remarks>
class Motor : Servo		
{
	public:

		/// <summary> Construtor da Classe</summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		Motor() : Servo()
		{
			velocidade = 0;
			minimo = 0;
		}

		/// <summary> Define a velocidade do motor </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="diferenca">Quanto acima do m�nimo para girar o motor dever� girar</param>
		void girar(int diferenca)
		{
			write(minimo + diferenca);

			velocidade = minimo + diferenca;
		}

		/// <summary> Define o valor m�nimo de velocidade para o motor girar </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="minimo"> Valor m�nimo definido </param>
		void setMinimo(int minimo)
		{
			this->minimo = minimo;
		}

		/// <summary> Liga o motor </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void ligar()
		{
			write(17);
		}

		/// <summary> Inicia a rota��o do motor, imprimindo o m�nimo para girar </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void girar()
		{
			write(31);
		}

		/// <summary> Desliga o motor, encerrando a rota��o </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void desligar()
		{
			for (int i = velocidade; i >= 0; i -= 5)
			{
				write(i);
				delay(50);
			}

			velocidade = 0;
		}

	protected:
		friend class Registro;

		int minimo, /// <value> A velocidade m�nima para rota��o do motor </value>
			velocidade; /// <value> A velocidade atual do motor </value>

};

/// <summary> Implementa um filtro de frequ�ncia do tipo "passa baixa"</summary>
/// <remarks>	Elton, 14/08/2019. </remarks>
class PassaBaixa
{
	protected:
		float saidaAnt, /// <value> �ltima sa�da do filtro </value>
			constante, /// <value> Constante do filtro </value>
			saida, /// <value> Sa�da atual do filtro </value>
			freqCorte, /// <value> Frequ�ncia de corte do filtro em [rad/s] </value>
			amostragem; /// <value> Amostragem do sinal </value>

		/// <summary> Calcula a constante utilizada no c�lculo do filtro. </summary>
		/// <remarks>	Elton, 14/08/2019. </remarks>
		void calculaConstante() //Calcula a constante (alpha) do filtro
		{
			constante = 1 - exp(freqCorte * (1 / amostragem));
		}

	public:

		/// <summary> Construtor da classe</summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		PassaBaixa()
		{
			this->freqCorte = freqCorte * 2 * PI * (1 / 60);
			this->amostragem = amostragem;

			calculaConstante();

			saidaAnt = 0;
		}


		/// <summary> Insere uma nova medida no </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="medida"> A nova medida </param>
		void entrar(float medida) //Insere uma nova leitura no filtro e calcula a sa�da
		{
			saida = (constante * medida) + ((1 + constante) * saidaAnt);
			saidaAnt = saida;
		}

		/// <summary> Retorna o �ltimo valor calculado pelo filtro </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <returns> O valor filtrado </returns>
		float sair() //Retorna o valor filtrado
		{
			return -saida;
		}

		/// <summary> Define a frequ�ncia de corte do filtro. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="freqCorte"> A frequ�ncia definida. </param>
		void setFrequencia(float freqCorte) //Altera a frequ�ncia de corte
		{
			this->freqCorte = freqCorte;
			calculaConstante();
		}

		/// <summary> Define a amostragem do filtro. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="amostragem"> A amostragem definida. </param>
		void setAmostragem(float amostragem) //Altera a amostragem
		{
			this->amostragem = amostragem;
			calculaConstante();
		}

};

/// <summary> Classe para controle de uma IMU do modelo "MPU6050" </summary>
/// <remarks>	Elton, 14/08/2019. </remarks>
class Inercial : MPU6050
{
	public:

		/// <summary> Construtor da classe </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="w"> Objeto "Wire" </param>
		Inercial(TwoWire &w) : MPU6050(w)
		{
			roll = 0;
			pitch = 0;
			yaw = 0;
		}

		/// <summary> Efetua uma leitura filtrando os valores lidos </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void leitura()
		{
			update();

			filtroRoll.entrar(getAngleX());
			filtroPitch.entrar(getAngleY());
			filtroYaw.entrar(getAngleZ());

			pitch = filtroPitch.sair() - 6.55 - 0.33;
			roll = filtroRoll.sair();
			yaw = filtroYaw.sair();
		}

		/// <summary> Retorna o valor filtrado no eixo Roll. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <returns> Valor filtrado no eixo Roll em [m/s^2]. </returns>
		float getRoll()
		{
			return roll;
		}

		/// <summary> Retorna o valor filtrado no eixo Yaw. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <returns> Valor filtrado no eixo Yaw em [m/s^2]. </returns>
		float getYaw()
		{
			return yaw;
		}

		/// <summary> Retorna o valor filtrado no eixo Pitch. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <returns> Valor filtrado no eixo Pitch em [m/s^2]. </returns>
		float getPitch()
		{
			return pitch;
		}

	protected:
		friend class Registro;

		float roll, /// <value> �ltimo valor lido e filtrado no eixo roll. </value> 
			  pitch, /// <value> �ltimo valor lido e filtrado no eixo roll. </value> 
			  yaw; /// <value> �ltimo valor lido e filtrado no eixo roll. </value>
		PassaBaixa filtroRoll, /// <value> Filtro passa baixa para o eixo Roll. </value>
				   filtroPitch, /// <value> Filtro passa baixa para o eixo Pitch. </value>
				   filtroYaw; /// <value> Filtro passa baixa para o eixo Yaw. </value>
};

/// <summary> Implementa um volt�metro conectado a uma porta anal�gica do Arduino </summary>
/// <remarks>	Elton, 14/08/2019. </remarks>
class Voltimetro
{
	public:

		/// <summary> Construtor da classe </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		Voltimetro()
		{
		}

		/// <summary> Define a porta de leitura </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name = "porta"> Porta da leitura </param>
		void setPorta(int porta)
		{
			this->porta = porta;
		}

		/// <summary> Efetua a leitura anal�gica e a converte para a tens�o em [V] </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <returns> Tens�o lida </returns>
		int leitura()
		{
			tensao = float(analogRead(porta)) / 67.2727;

			return tensao;
		}

	protected:
		friend class Registro;

		int porta; /// <value> Porta de leitura. </value>
		float tensao; /// <value> �ltimo valor de tens�o lido. </value>
};


/// <summary> Realiza o envio e recebimento de dados da porta serial, que pode ser definida </summary>
/// <remarks> Elton, 14/08/2019 </remarks>
class ControleSerial
{
	public:

		/// <summary> Construtor da classe </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		ControleSerial()
		{
		}

		/// <summary> Inicia a comunica��o serial na porta desejada. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="uso"> 
		/// <para> Porta desejada: </para>
		/// <para> 0 = Padr�o do Arduino </para>
		/// <para> 1 = Porta definada pelo usu�rio </para> 
		///	</param>
		void iniciar(int uso)
		{
			usando = uso;

			if (usando == 0)
			{
				Serial.begin(9600);
			}
			else
			{
				serial2->begin(9600);
			}
		}

		/// <summary> Define uma outra porta serial. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="serial2"> Ponteiro para o objeto "SoftwareSerial" da nova porta. </param>
		void setSerial(SoftwareSerial *serial2)
		{
			this->serial2 = serial2;
		}

		/// <summary> Imprime um valor dado. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <typeparam name="tipo">	Tipo do valor. </typeparam>
		/// <param name="texto"> O valor a ser impresso. </param>
		template <class tipo> void print(tipo texto)
		{
			if (usando == 0)
			{
				Serial.print(texto);
			}
			else if (usando == 1)
			{
				serial2->print(texto);
			}
		}

		/// <summary> Imprime um valor dado, com uma quebra de linha. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <typeparam name="tipo">	Tipo do valor. </typeparam>
		/// <param name="texto"> O valor a ser impresso. </param>
		template <class tipo> void println(tipo texto)
		{
			if (usando == 0)
			{
				Serial.println(texto);
			}
			else if (usando == 1)
			{
				serial2->println(texto);
			}
		}

		/// <summary> Imprime uma quebra de linha </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void println()
		{
			if (usando == 0)
			{
				Serial.println();
			}
			else if (usando == 1)
			{
				serial2->println();
			}
		}

	protected:

		/// <value> Ponteiro para o objeto serial definido pelo usu�rio. </value>
		SoftwareSerial *serial2;

		/// <value>	
		/// <para> Cont�m o �ndice da porta sendo usada. </para>
		/// <para> 0 = Padr�o do Arduino </para>
		/// <para> 1 = Porta definada pelo usu�rio </para>
		///	</value>
		int usando;
};

/// <summary> Implementa um registro/log para dados do drone. </summary>
/// <remarks>	Elton, 14/08/2019. </remarks>
class Registro
{
	public:

		/// <summary> Construtor da Classe </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="controle"> Ponteiro para um objeto "ControleSerial" </param>
		Registro(ControleSerial *controle)
		{
			this->controle = controle;

			imprimeAngulo = false;
			imprimeVelocidade = false;
			imprimePID = false;
		}

		/// <summary> Define o endere�o para o valor de tens�o. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="tensao"> Ponteiro para a vari�vel que cont�m a tens�o lida. </param>
		void setTensao(float *tensao)
		{
			this->tensao == tensao;
		}

		/// <summary> Define os endere�os para as posi��es �ngulares do rob�. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="roll"> Ponteiro para a vari�vel que cont�m a leitura do eixo "roll". </param>
		/// <param name="pitch"> Ponteiro para a vari�vel que cont�m a leitura do eixo "pitch". </param>
		/// <param name="yaw"> Ponteiro para a vari�vel que cont�m a leitura do eixo "yaw".</param>
		void setAngulo(float *roll, float *pitch, float *yaw)
		{
			this->roll = roll;
			this->pitch = pitch;
			this->yaw = yaw;
		}

		/// <summary> Define os endere�os para as velocidades dos motores do rob�. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="vel0"> Ponteiro para a vari�vel que cont�m a leitura do motor 0. </param>
		/// <param name="vel1">	Ponteiro para a vari�vel que cont�m a leitura do motor 1. </param>
		/// <param name="vel2">	Ponteiro para a vari�vel que cont�m a leitura do motor 2. </param>
		/// <param name="vel3">	Ponteiro para a vari�vel que cont�m a leitura do motor 3. </param>
		void setVelocidade(float *vel0, float *vel1, float *vel2, float *vel3)
		{
			this->vel0 = vel0;
			this->vel1 = vel1;
			this->vel2 = vel2;
			this->vel3 = vel3;
		}

		/// <summary> Imprime os valores desejados na porta serial. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void print()
		{
			printTensao();

			if (imprimeAngulo)
			{
				printAngulo();
			}

			if (imprimeVelocidade)
			{
				printVelocidade();
			}

			if (imprimePID)
			{
				printPID();
			}

			controle->println();
		}

	protected:

		ControleSerial *controle; /// <value> Ponteiro para o objeto "ControleSerial" para o envio de dados. </value>
		float *tensao, /// <value> Ponteiro para o endere�o da vari�vel com o valor de tens�o. </value>
			*roll, /// <value> Ponteiro para o endere�o da vari�vel com o valor de inclina��o no eixo roll. </value>
			*pitch, /// <value>	Ponteiro para o endere�o da vari�vel com o valor de inclina��o no eixo pitch. </value>
			*yaw, /// <value> Ponteiro para o endere�o da vari�vel com o valor de inclina��o no eixo yaw. </value>
			*vel0, /// <value> Ponteiro para o endere�o da vari�vel com a velocidade do motor 0. </value>
			*vel1, /// <value> Ponteiro para o endere�o da vari�vel com a velocidade do motor 1. </value>
			*vel2, /// <value> Ponteiro para o endere�o da vari�vel com a velocidade do motor 2. </value>
			*vel3; /// <value> Ponteiro para o endere�o da vari�vel com a velocidade do motor 3. </value>

		bool imprimeAngulo, /// <value> True = Deve imprimir as inclina��es. </value>
			imprimeVelocidade, /// <value> True = Deve imprimir as velocidades. </value>
			imprimePID; /// <value> True = Deve imprimir os dados dos controladores PID. </value>

		/// <summary> Imprime as inclina��es. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void printAngulo()
		{
			controle->print("pitch= ");
			controle->print(*pitch);
			controle->print(" ");
			controle->print("roll= ");
			controle->print(*roll);
			controle->print(" ");
			controle->print("yaw= ");
			controle->print(*yaw);
			controle->print(" ");
		}

		/// <summary> Imprime as velocidades dos motores. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void printVelocidade()
		{
			controle->print("vel0= ");
			controle->print(*vel0);
			controle->print(" ");
			controle->print("vel1= ");
			controle->print(*vel1);
			controle->print(" ");
			controle->print("vel2= ");
			controle->print(*vel2);
			controle->print(" ");
			controle->print("vel3= ");
			controle->print(*vel3);
			controle->print(" ");
		}

		/// <summary> Imprime a tens�o. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void printTensao()
		{
			controle->print("tensao= ");
			controle->print(*tensao);
			controle->print(" ");
		}

		/// <summary> Imprime as inclina��es. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void printPID()
		{

		}

};

/// <summary> Estrutura para armazenar dados de um controlador PID. </summary>
/// <remarks> Elton, 14/08/2019. </remarks>
struct VariaveisPID
{
	double entrada, /// <value> �ltima entrada do PID. </value>
		   saida, /// <value> �ltima sa�da do PID. </value>
		   setPoint, /// <value> SetPoint do PID. </value>
		   kp, /// <value> Coeficiente proporcional do PID </value>
		   ki, /// <value> Coeficiente integrativo do PID </value>
		   kd; /// <value> Coeficiente derivativo do PID </value>
};

/// <summary> Implementa o controle de um drone. </summary>
/// <remarks> Elton, 14/08/2019. </remarks>
class Drone
{
	public:
		Drone() : pidPitch(&varPIDPitch.entrada, &varPIDPitch.saida, &varPIDPitch.setPoint, 0.0, 0.0, 0.0, DIRECT), inercial(Wire), registro(&controle)
		{
		}

		/// <summary> Define a porta do motor. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="indice"> �ndice do motor a ser definido. </param>
		/// <param name="porta">  Porta desejada. </param>
		void setMotor(int indice, int porta)
		{
			//motor[indice].attach(porta);
		}

		/// <summary> Define o �ngulo para o eixo pitch. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		/// <param name="angulo"> O �ngulo desejado em [�]. </param>
		void irPara(float angulo)
		{
			varPIDPitch.setPoint = angulo;
		}

		/// <summary> Atualiza os dados do drone e o mat�m na posi��o desejada. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void atualiza()
		{
			if (voltimetro.leitura() < 10)
			{
				pousar();
			}

			leitura();
			atualizaPID();
			atualizaMotor();

			registro.print();
		}

		/// <summary> Liga os motores do drone. </summary>
		/// <remarks> Elton, 14/08/2019.</remarks>
		void ligar()
		{
			delay(1000);

			for (int i = 0; i < 4; i++)
			{
				motor[i].ligar();
			}

			delay(2000);

			for (int i = 0; i < 4; i++)
			{
				motor[i].girar();
			}

			//registro.inicia();
		}

		/// <summary> Desliga os motores. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void desligar()
		{
			for (int i = 0; i < 4; i++)
			{
				motor[i].desligar();
			}
			while (1) {}
		}

		/// <summary> Pousa o drone. </summary>
		/// <remarks> 
		/// <param> N�O IMPLEMENTADO. </param>
		/// <param> Elton, 14/08/2019. </param>
		/// </remarks>
		void pousar()
		{
			desligar();
		}

	protected:
		VariaveisPID varPIDPitch; /// <value> Vari�veis do PID para controle do eixo pitch </value>
		
		Inercial inercial; /// <value> Objeto para controle da IMU. </value>
		PID pidPitch; /// <value> PID para controle da inclina��o no eixo pitch. </value>
		Motor motor[4]; /// <value> Objetos para controle dos motores. </value>
		Registro registro; /// <value> Objeto para log do drone. </value>
		ControleSerial controle; /// <value> Objeto para controle da comunica��o do drone. </value>
		Voltimetro voltimetro;/// <value> Objeto para leitura da tens�o da bateria </value>

		/// <summary> Atualiza entradas dos PIDs. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void atualizaEntradas()
		{
			varPIDPitch.entrada = inercial.getPitch();
		}

		/// <summary> Atualiza os PIDs. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void atualizaPID()
		{
			atualizaEntradas();
			pidPitch.Compute();
		}

		/// <summary> Define as novas velocidades dos motores. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void atualizaMotor()
		{
			motor[0].girar(varPIDPitch.saida);
			motor[1].girar(-varPIDPitch.saida);
			motor[2].girar(-varPIDPitch.saida);
			motor[3].girar(varPIDPitch.saida);
		}

		/// <summary> Efetua as leituras dos sensores. </summary>
		/// <remarks> Elton, 14/08/2019. </remarks>
		void leitura()
		{
			inercial.leitura();
		}
};
