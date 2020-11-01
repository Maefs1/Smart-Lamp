/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define timeSeconds 5

//Chave de autenticação do Blynk com o celular
char auth[] = "HKWs6TsHt-uwIQhjGoDu0w9gHiTe18Ln";

//Credenciais do Wifi
char ssid[] = "VIVOFIBRA-49B7";
char pass[] = "72233E49B7";

unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

const int portaLDR = 35;
const int sensorMovimento = 27;

const int pinoVermelho = 13;
const int pinoVerde = 12;
const int pinoAzul = 14;

const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;

const int freq = 5000;
const int resolution = 8;

int r = 255;
int g = 255;
int b = 255;

int acionado = 0;

int luzAmbiente = 0;

BLYNK_WRITE(V0){
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();

  if (acionado == 1){
    ledcWrite(ledChannel1, r);
    ledcWrite(ledChannel2, g);
    ledcWrite(ledChannel3, b);
  }
}

BLYNK_WRITE(V1){
  acionado = param[0].asInt();
  
  if(acionado == 0){
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);
    ledcWrite(ledChannel3, 0);
  }

  if(acionado == 1){
    ledcWrite(ledChannel1, r);
    ledcWrite(ledChannel2, g);
    ledcWrite(ledChannel3, b);
  }    
}

void detectaLuz(){
  luzAmbiente = analogRead(portaLDR);
  //delay(10);
  //Serial.println("Leitura do Sensor de LDR:");
  //Serial.println(luzAmbiente);
}

void detectaMovimento() {
  if (luzAmbiente < 1000){
    acionado = 1;
    ledcWrite(ledChannel1, 255);
    ledcWrite(ledChannel2, 255);
    ledcWrite(ledChannel3, 255);
    Serial.println("MOVIMENTOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
    startTimer = true;
    lastTrigger = millis();
  }    
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(sensorMovimento, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sensorMovimento), detectaMovimento, RISING);
  
  
  
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  ledcSetup(ledChannel3, freq, resolution);

  ledcAttachPin(pinoVermelho,ledChannel1);
  ledcAttachPin(pinoVerde,ledChannel2);
  ledcAttachPin(pinoAzul,ledChannel3);
}

void loop()
{
  Blynk.run();
  detectaLuz();
  now = millis();
  
  if(startTimer && (now - lastTrigger > (timeSeconds*1000))){
    acionado = 0;
    ledcWrite(ledChannel1, 0);
    ledcWrite(ledChannel2, 0);
    ledcWrite(ledChannel3, 0);
    startTimer = false;
    }
}