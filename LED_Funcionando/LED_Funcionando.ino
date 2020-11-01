/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "HKWs6TsHt-uwIQhjGoDu0w9gHiTe18Ln";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "VIVOFIBRA-49B7";
char pass[] = "72233E49B7";

const int pinoVermelho = 13;
const int pinoVerde = 12;
const int pinoAzul = 14;

const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;

const int freq = 5000;
const int resolution = 8;

BLYNK_WRITE(V0){
  int r = param[0].asInt();
  int g = param[1].asInt();
  int b = param[2].asInt();

  Serial.print("Cores(r,g,b): ");
  Serial.println(r);
  Serial.println(g);
  Serial.println(b);

  ledcWrite(ledChannel1, r);
  ledcWrite(ledChannel2, g);
  ledcWrite(ledChannel3, b);

}
void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

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
}
