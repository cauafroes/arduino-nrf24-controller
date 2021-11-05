/* Lembre de instalar a biblioteca NRF24 em seu IDE, link: https://github.com/nRF24/RF24
 * Conexões:

        CE     ->   D9
       CSN     ->   D10
      CLK/SCK  ->   D13
       MOSI    ->   D11
       MISO    ->   D12

Esse código transmite informação em 4 canais a partir de sinais deste arduino.
*/

#include <SPI.h>         //Necessária para comunicação Serial.
#include <nRF24L01.h>    //Necessária para uso do módulo NRF24.
#include <RF24.h>        //Necessária para uso do módulo NRF24.

const uint64_t CanoSaida = 00001; // Endereço de comunicação entre o transmissor e receptor, usar o mesmo para ambos.

RF24 radio(9, 10);  //Define os pinos CE e CSN

//Cria uma estrutura chamada Dados_a_enviar, que vai conter os bytes que serão "associados" aos canais do NRF24

struct Dados_a_enviar {
  byte j1pot;
  byte j2pot;
};

//Cria uma variável com a estrutura acima e a nomeia dados.

Dados_a_enviar dados;

void setup()
{
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(false);           //Configura a transmissão
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(CanoSaida);  //Abre o canal de comunicação

 //Reseta os valores armazenados em cada canal.
  dados.j2pot = 127;  //O valor é 127, pois é o centro entre 0 e 255
  dados.j1pot = 127;  //O intervalo de possíveis valores para um byte vai de 0 até 255
}


void loop()
{

  dados.j1pot = map( analogRead(A0), 0, 1023, 0, 255);
  dados.j2pot = map( analogRead(A3), 0, 1023, 0, 255);
  
      //Para testes:
    Serial.print(dados.j1pot);
    Serial.print("  ");
    Serial.println(dados.j2pot);

  radio.write(&dados, sizeof(Dados_a_enviar));    //Envia os dados
}
