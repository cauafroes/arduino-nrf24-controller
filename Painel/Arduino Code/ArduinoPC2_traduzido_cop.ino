const char inicioChar = '<';
const char fimChar = '>';

const byte bufferTamanho = 40;
char entradaBuffer[bufferTamanho];
char mensagemDoPC[bufferTamanho] = {0};

byte bytesRecuper = 0;

boolean leituraEmProgresso = false;
boolean novasInfo = false;

unsigned long ultimRespostPCmillis = 0;
unsigned long respProPCintervalo = 1000;
unsigned long atualMillis;

int valorRecebidoUm = 0;
int valorRecebidoDois = 0;

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("<Arduino esta pronto>");
}

void loop() {
  atualMillis = millis();
  pegarDadosPC();
}

void pegarDadosPC(){
  if (Serial.available() > 0){
    char x = Serial.read();

    if (x == fimChar){
      leituraEmProgresso = false;
      novasInfo = true;
      entradaBuffer[bytesRecuper] = 0;
      analisarDados();
    }
    if (leituraEmProgresso){
      entradaBuffer[bytesRecuper] = x;
      bytesRecuper ++;
      if (bytesRecuper == bufferTamanho){
        bytesRecuper = bufferTamanho - 1;
      }
    }
    if (x == inicioChar){
      bytesRecuper = 0;
      leituraEmProgresso = true;
    }
  }
}


void analisarDados(){  // processa a mensagem do pc
  
  char * strtokIndex; // o strtok() usa isso como index

  strtokIndex = strtok(entradaBuffer,",");
  strcpy(mensagemDoPC, strtokIndex);

  strtokIndex = strtok(NULL, ",");
  valorRecebidoUm = atoi(strtokIndex);

  strtokIndex = strtok(NULL, ",");
  valorRecebidoDois = atoi(strtokIndex);
  
}

void responderAoPc(){
  
  if (novasInfo){
    novasInfo = false;
    Serial.print("<Valores: ");
    Serial.print(mensagemDoPC);
    Serial.print(" Valor recebido 1: ");
    Serial.print(valorRecebidoUm);
    Serial.print(" Valor recebido 2: ");
    Serial.print(valorRecebidoDois);
    Serial.println(">");
    
  }
}
