class Motor : Servo		
{
	public:
		Motor() : Servo()
		{
			velocidade = 0;
			minimo = 0;
		}

		void girar(int diferenca)
		{
			write(minimo + diferenca);

			velocidade = minimo + diferenca;
		}
	
		void setMinimo(int minimo)
		{
			this->minimo = minimo;
		}

		void ligar()
		{
			write(17);
		}

		void girar()
		{
			write(31);
		}

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
		int minimo,
			velocidade;

};

class PassaBaixa
{
	protected:
		float saidaAnt, //Última saída do filtro
			constante, //Constante do filtro
			saida, //Saída atual do filtro
			freqCorte, //Frequência de corte do filtro (rad/s)
			amostragem; //Amostragem do sinal

	public:
		PassaBaixa()
		{
			this->freqCorte = freqCorte * 2 * PI * (1 / 60);
			this->amostragem = amostragem;

			calculaConstante();

			saidaAnt = 0;
		}

		void calculaConstante() //Calcula a constante (alpha) do filtro
		{
			constante = 1 - exp(freqCorte * (1/amostragem));
		}

		void entrar(float medida) //Insere uma nova leitura no filtro e calcula a saída
		{
			saida = (constante * medida) + ((1 + constante) * saidaAnt);
			saidaAnt = saida;
		}

		float sair() //Retorna o valor filtrado
		{
			return -saida;
		}

		void setFrequencia(float freqCorte) //Altera a frequência de corte
		{
			this->freqCorte = freqCorte;
			calculaConstante();
		}

		void setAmostragem(float amostragem) //Altera a amostragem
		{
			this->amostragem = amostragem;
			calculaConstante();
		}

};

class Inercial : MPU6050
{
	public:
		Inercial(TwoWire &w) : MPU6050(w)
		{
			roll = 0;
			pitch = 0;
			yaw = 0;
		}

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
		
		float getRoll()
		{
			return roll;
		}

		float getYaw()
		{
			return yaw;
		}

		float getPitch()
		{
			return pitch;
		}

	protected:
		float roll, pitch, yaw; //
		PassaBaixa filtroRoll,
				   filtroPitch,
				   filtroYaw;
};

class Voltimetro
{
	public:
		Voltimetro()
		{
		}

		void setPorta(int porta)
		{
			this->porta = porta;
		}

		int leitura()
		{
			tensao = float(analogRead(porta)) / 67.2727;

			return tensao;
		}

	protected:
		int porta;
		float tensao;
};

class ControleSerial
{
	public:
		ControleSerial()
		{
		}


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

		void setSerial(SoftwareSerial *serial2)
		{
			this->serial2 = serial2;
		}

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
		SoftwareSerial *serial2;

		int usando;
};

class Registro
{
	public:
		Registro(ControleSerial *controle)
		{
			this->controle = controle;

			imprimeAngulo = false;
			imprimeVelocidade = false;
			imprimePID = false;
		}

		void setTensao(float *tensao)
		{
			this->tensao == tensao;
		}

		void setAngulo(float *roll, float *pitch, float *yaw)
		{
			this->roll = roll;
			this->pitch = pitch;
			this->yaw = yaw;
		}

		void setVelocidade(float *vel0, float *vel1, float *vel2, float *vel3)
		{
			this->vel0 = vel0;
			this->vel1 = vel1;
			this->vel2 = vel2;
			this->vel3 = vel3;
		}

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

	private:
		ControleSerial *controle;
		float *tensao,
			*roll,
			*pitch,
			*yaw,
			*vel0,
			*vel1,
			*vel2,
			*vel3;

		bool imprimeAngulo,
			imprimeVelocidade,
			imprimePID;

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

		void printTensao()
		{
			controle->print("tensao= ");
			controle->print(*tensao);
			controle->print(" ");
		}

		void printPID()
		{

		}

};

struct VariaveisPID
{
	double entrada,
		saida,
		setPoint,
		kp,
		ki,
		kd;	
};

class Drone
{
	public:
		Drone() : pidPitch(&varPIDPitch.entrada, &varPIDPitch.saida, &varPIDPitch.setPoint, 0.0, 0.0, 0.0, DIRECT), inercial(Wire), registro(&controle)
		{

		}

		void setMotor(int indice, int porta)
		{
			//motor[indice].attach(porta);
		}

		void irPara(float angulo)
		{
			varPIDPitch.setPoint = angulo;
		}

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

			registro.inicia();
		}

		void desligar()
		{
			for (int i = 0; i < 4; i++)
			{
				motor[i].desligar();
			}
			while (1) {}
		}

		void pousar()
		{
			desligar();
		}

	protected:
		VariaveisPID varPIDPitch;
		
		Inercial inercial;
		PID pidPitch;
		Motor motor[4];
		Registro registro;
		ControleSerial controle;
		Voltimetro voltimetro;


		void atualizaEntradas()
		{
			varPIDPitch.entrada = inercial.getPitch();
		}

		void atualizaPID()
		{
			atualizaEntradas();
			pidPitch.Compute();
		}

		void atualizaMotor()
		{
			motor[0].girar(varPIDPitch.saida);
			motor[1].girar(-varPIDPitch.saida);
			motor[2].girar(-varPIDPitch.saida);
			motor[3].girar(varPIDPitch.saida);
		}

		void leitura()
		{
			inercial.leitura();
		}
};
