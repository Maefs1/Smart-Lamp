#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//Tempo em que o sistema ficará ligado depois de interação com sensores
#define tempoSegundos 10 

//Chave de autenticação do Blynk com o celular
char auth[] = "HKWs6TsHt-uwIQhjGoDu0w9gHiTe18Ln";

//Credenciais do Wifi
char ssid[] = "VIVOFIBRA-49B7";
char pass[] = "72233E49B7";

//Definindo variáveis para controlar o tempo
unsigned long tempoAtual = millis();
unsigned long ultimoAcionamento = 0;
boolean iniciaTimer = false;

//Definindo portas utilizados do ESP32
const int portaLDR = 35;
const int sensorMovimento = 27;
const int pinoVermelho = 13;
const int pinoVerde = 12;
const int pinoAzul = 14;

//Separando os canais PWM do RGB
const int canalLed1 = 0;
const int canalLed2 = 1;
const int canalLed3 = 2;

//Frequência utilizada no PWM
const int freq = 5000;

//Quantidade de BITS a ser utilizado no LED RGB
const int quantidadeBits = 8;

//Variáveis globais para controle da cor RGB
float r = 255;
float g = 255;
float b = 255;

//Variáveis globais para controle das diferentes entradas
int acionado = 0;
int programado = 0;
int luzAmbiente = 0;

//Botão ZeRBGa que controla a cor do LED
BLYNK_WRITE(V0){
  //Cada cor é recebida por parâmetro diretamente do Blynk
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();

  //Caso o sistema esteja ligado, é enviado a cor para o LED
  if (acionado == 1 or programado == 1){
    //Escrevendo o valor no canal
    ledcWrite(canalLed1, r);
    ledcWrite(canalLed2, g);
    ledcWrite(canalLed3, b);
  }
}

//Botão ON/OFF
BLYNK_WRITE(V1){
  //Recebe valor 0 ou 1 diretamente do Blynk
  acionado = param[0].asInt();

  //Caso seja 0, é desligado o LED
  if(acionado == 0){
    ledcWrite(canalLed1, 0);
    ledcWrite(canalLed2, 0);
    ledcWrite(canalLed3, 0);
  }
  //Caso seja 1, LED é ligado
  if(acionado == 1){
    ledcWrite(canalLed1, r);
    ledcWrite(canalLed2, g);
    ledcWrite(canalLed3, b);
  }    
}

//Botão de funcionamento programado
BLYNK_WRITE(V2){
  /*Quando é atingido o tempo de entrada é retornado valor 1,
    ao atingir o valor de saída é retornado valor 0*/
  programado = param.asInt();
  
  //Caso seja 0, é desligado o LED
  if(programado == 0){
    ledcWrite(canalLed1, 0);
    ledcWrite(canalLed2, 0);
    ledcWrite(canalLed3, 0);
  }
  //Caso seja 1, LED é ligado
  if(programado == 1){
    ledcWrite(canalLed1, r);
    ledcWrite(canalLed2, g);
    ledcWrite(canalLed3, b);
  }
}

//Botão de controle de intensidade luminosa
BLYNK_WRITE(V3){
  //Botão de rolagem que retorna valores 0~100
  float intensidade = param.asInt();

  //Utilizado o valor retornado como porcentagem de intensidade luminosa
  float rN = (r/100) * intensidade;
  float gN = (g/100) * intensidade;
  float bN = (b/100) * intensidade;

  //Caso esteja acionado de alguma forma, é enviado a nova luminosidade
  if (acionado == 1 or programado == 1){
    ledcWrite(canalLed1, rN);
    ledcWrite(canalLed2, gN);
    ledcWrite(canalLed3, bN);
  }
}

//Função que detecta a luminosidade através do LDR
void detectaLuz(){
  //É recebido o valor do LDR através de um conversor analógico-digital
  luzAmbiente = analogRead(portaLDR);
  delay(10);
}

//Função de acionamento automático
void detectaMovimento() {
  //Caso a luminosidade esteja baixa (utilizado threshold = 1000)
  if (luzAmbiente < 1000){
    //Seta o sistema como acionado
    acionado = 1;
    //Envia a cor para o LED
    ledcWrite(canalLed1, r);
    ledcWrite(canalLed2, g);
    ledcWrite(canalLed3, b);
    //Marcando o início do TIMER
    iniciaTimer = true;
    //Marcando o tempo iniciado em milissegundos
    ultimoAcionamento = millis();
  }    
}

//Configuração do sistema
void setup()
{
  //Setando a Upload Speed em 115200
  Serial.begin(115200);
  //Autenticando o Blynk
  Blynk.begin(auth, ssid, pass);
  //Setando o PIR em modo PullUp
  pinMode(sensorMovimento, INPUT_PULLUP);
  //Setando o PIR como interrupção, atribuindo a função a ser executada e colocando em borda de subida
  attachInterrupt(digitalPinToInterrupt(sensorMovimento), detectaMovimento, RISING);

  //Configurando o LED para as funcionalidades PWM
  ledcSetup(canalLed1, freq, quantidadeBits);
  ledcSetup(canalLed2, freq, quantidadeBits);
  ledcSetup(canalLed3, freq, quantidadeBits);

  //Conectando o canal ao GPIO para ser controlado
  ledcAttachPin(pinoVermelho,canalLed1);
  ledcAttachPin(pinoVerde,canalLed2);
  ledcAttachPin(pinoAzul,canalLed3);
}

void loop()
{
  //Iniciando servidor Blynk
  Blynk.run();
  //Chamando a função do LDR para verificar o ambiente
  detectaLuz();
  //Verificando o tempo atual
  tempoAtual = millis();

  //Caso o timer esteja ligado e o tempo exceda o determinado
  if(iniciaTimer && (tempoAtual - ultimoAcionamento > (tempoSegundos * 1000))){
    //Acionamento desativado
    acionado = 0;
    //Desliga o LED
    ledcWrite(canalLed1, 0);
    ledcWrite(canalLed2, 0);
    ledcWrite(canalLed3, 0);
    //Marca o Timer como desligado
    iniciaTimer = false;
    }
}
