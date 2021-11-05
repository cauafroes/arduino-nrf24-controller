#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

/* Lembre de instalar a biblioteca NRF24 em seu Arduino IDE, link: https://github.com/nRF24/RF24
 * Conexões:

        CE     ->   D9
       CSN     ->   D10
      CLK/SCK  ->   D13
       MOSI    ->   D11
       MISO    ->   D12

Esse código recebe informações de outro arduino e cria uma saída PWM nos pinos da placa.
*/

#include <SPI.h>         //Necessária para comunicação Serial.
#include <nRF24L01.h>    //Necessária para uso do módulo NRF24.
#include <RF24.h>        //Necessária para uso do módulo NRF24.
#include <Servo.h>       //Necessária para criação de sinais PWM.

#define motor1a 3    //Associa "motor1a" ao pino D3 no Arduino
#define motor1b 4    //Associa "motor1b" ao pino D4 no Arduino
#define motor2a 5    //Associa "motor2a" ao pino D5 no Arduino
#define motor2b 6    //Associa "motor2b" ao pino D6 no Arduino
#define en1 7        //Associa "en1" ao pino D7 no Arduino
#define en2 8        //Associa "en2" ao pino D8 no Arduino

RF24 radio(9, 10);  //Define os pinos CE e CSN

Servo direcao;

const uint64_t CanoEntrada = 00001;   //Endereço de comunicação entre o transmissor e receptor, usar o mesmo para ambos.
unsigned long lastRecvTime = 0;
int j1pot_valor, j2pot_valor = 0;   //Cria as variáveis que serão os valores recebidos pelo outro Arduino.
int velocidademotor1, velocidademotor2 = 0;                       //Cria os valores PWM que serão enviados para a ponte H, depois do tratamento (map e if).


struct Dados_recebidos {      //Cria uma estrutura chamada Dados_recebidos, que vai conter os bytes que serão "associados" aos canais do NRF24
  byte j1pot;
  byte j2pot;
};

Dados_recebidos dados_recebidos;     //Cria uma variável com a estrutura acima e a nomeia 'dados_recebidos'.


void reseta_os_valores()    //Criamos uma função que irá centralizar os valores para o veículo não se mover sozinho enquanto a comunicação não é estabelecida.
{
  dados_recebidos.j1pot = 127;
  dados_recebidos.j2pot = 127;
}


void setup()
{
  
  pinMode(en1, OUTPUT);    //Determina os pinos como saída.
  pinMode(en2, OUTPUT);
  pinMode(motor1a, OUTPUT);
  pinMode(motor1b, OUTPUT);
  pinMode(motor2a, OUTPUT);
  pinMode(motor2b, OUTPUT);
  Serial.begin(9600);
  
  direcao.attach(2);      //Atrela o pino D2 à 'direcao'

  reseta_os_valores();    //Reseta os valores recebidos.

  radio.begin();              //Abre o canal de comunicação, faz o NRF24 iniciar, configura e inicia a conexão.
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1,CanoEntrada);
  radio.startListening();

}


//Criamos uma função que irá receber os dados de tempo em tempo.
void receber_os_dados()  {
  while (radio.available()) {
  radio.read(&dados_recebidos, sizeof(Dados_recebidos));
  lastRecvTime = millis(); //Recepção de dados ocorre aqui
      }
}


void loop()
{
    //Recebe os dados
  receber_os_dados();

  if (dados_recebidos.j1pot < 117) {
    
    // Faz o motor 1 rodar para "trás"
    digitalWrite(motor1a, HIGH);
    digitalWrite(motor1b, LOW);
    
    // Faz o motor 2 rodar para "trás"
    digitalWrite(motor2a, HIGH);
    digitalWrite(motor2b, LOW);
    
    //Aumenta a velocidade do motor
    velocidademotor1 = map(dados_recebidos.j1pot, 117, 0, 0, 255);
    velocidademotor2 = map(dados_recebidos.j1pot, 117, 0, 0, 255);
  }
  else if (dados_recebidos.j1pot > 137) {
    
    // Faz o motor 1 rodar para "frente"
    digitalWrite(motor1a, LOW);
    digitalWrite(motor1b, HIGH);
    
    // Faz o motor 1 rodar para "frente"
    digitalWrite(motor2a, LOW);
    digitalWrite(motor2b, HIGH);
    
    // Aumenta a velocidade do motor
    velocidademotor1 = map(dados_recebidos.j1pot, 137, 255, 0, 255);
    velocidademotor2 = map(dados_recebidos.j1pot, 137, 255, 0, 255);
  }
    //Faz com que os motores fiquem parados caso o joystick esteje centrado
    else {
    velocidademotor1 = 0;
    velocidademotor2 = 0;
  }

    //Para testes:
    Serial.print("L:");
    Serial.print(velocidademotor1);
    Serial.print("  ");
    Serial.print("R:");
    Serial.print(velocidademotor2);
    Serial.print("  ");
    Serial.print("Servo: ");
    Serial.println(j2pot_valor);

  analogWrite(en1, velocidademotor1); // Envia um sinal PWM para o motor1
  analogWrite(en2, velocidademotor2); // Envia um sinal PWM para o motor2
  
  j2pot_valor = map(dados_recebidos.j2pot,0,255,45,135);

     //Faz o servo virar
  direcao.write(j2pot_valor);    
  
}
