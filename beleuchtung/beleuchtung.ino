#include "DHTesp.h"

#define DHT22_PIN 13
#define BUTTON 35
#define LED1 12
#define LED2 32

bool zustandTaster    = HIGH;        // aktueller  Status des Tasters
bool zustandTasterAlt = HIGH;        // letzter  Status des Tasters
bool zustandLed       = LOW;         // Status der LED (an oder aus)

constexpr long entprellZeit = 30;     // in ms
unsigned long startEntprellen = 0;   // Zeitpunkt, wenn das Enprellen beginnt

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
constexpr long interval = 1000;       // in 10 s
bool retMeas = false;
DHTesp dht;

bool readDHT22()
{
  Serial.println("-----------------------------------");
  float temperatur = dht.getTemperature();
  float luftFeuchte = dht.getHumidity();  
  //TempAndHumidity dhtData = dht.getTempAndHumidity();
  if (dht.getStatus() != 0) 
  {
    Serial.println("Error while reading DHT data!");
    return false;
  }
  
  float hIndex = dht.computeHeatIndex(temperatur, luftFeuchte);
  // float tPunkt = dht.computeDewPoint(temperatur, luftFeuchte);

  Serial.println("Temperatur:       " + String (temperatur) + " °C" );
  Serial.println("  gefühlt:        " + String (hIndex) + " °C" );
  Serial.println("Luftfeuchtigkeit: " + String (luftFeuchte)  + " %");
  // Serial.println("Taupunkt:         " + String (tPunkt)  + " °C");
  return true;
}

void ledCtrl()
{
  zustandTaster = digitalRead(BUTTON);

  // Wurde der Taster gedrückt?
  if ((zustandTaster == 0) && (zustandTasterAlt == 1)) 
  {
    startEntprellen = millis();
  }

  if (startEntprellen != 0)
  {
      if ( millis()-startEntprellen > entprellZeit)
      {
        if (zustandTaster == 0)
        {
          zustandLed = !zustandLed;
          startEntprellen = 0;
        }
      }
  }
  zustandTasterAlt = zustandTaster;     // alten Tasterzustand speichern
  digitalWrite(LED1, zustandLed);
  digitalWrite(LED2, zustandLed);
}

void setup() 
{
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);     // interner Pull-Up-Widerstand
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  dht.setup(DHT22_PIN, DHTesp::AUTO_DETECT);   // DHTesp::DHT22
}

void loop() 
{
  ledCtrl();

  currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    retMeas = readDHT22();
    previousMillis = currentMillis;
  }
}
