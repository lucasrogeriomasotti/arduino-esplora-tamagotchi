#include <Esplora.h>
#include <TFT.h>

int screenWidth;
int screenHeight;

const int environmentTemperaturePosX = 20;
const int environmentTemperaturePosY = 5;
int environmentTemperature;

const int houseTemperaturePosX = 65;
const int houseTemperaturePosY = 5;
int houseTemperature;

void setup() {
  EsploraTFT.begin();
  EsploraTFT.background(0, 0, 0);
  screenWidth = EsploraTFT.width();
  screenHeight = EsploraTFT.height();

  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text("ENV", (environmentTemperaturePosX - 20), environmentTemperaturePosY);
  EsploraTFT.text("HOU", (houseTemperaturePosX - 20), houseTemperaturePosY);
}

void loop() {  
  EsploraTFT.fill(255,255,255);
  EsploraTFT.circle(screenWidth/2, screenHeight/2, 15);

  /*
  EsploraTFT.noFill();
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.rect(screenWidth/2, screenHeight/2, 80, 80);
  */
  
  environmentTemperature = Esplora.readTemperature(DEGREES_C);
  printTemperature(environmentTemperature, environmentTemperaturePosX, environmentTemperaturePosY);

  houseTemperature =  map(Esplora.readSlider(), 1023, 0, 10, 35);
  printTemperature(houseTemperature, houseTemperaturePosX, houseTemperaturePosY);
    
  delay(1000);
  clearTemperature(environmentTemperature, environmentTemperaturePosX, environmentTemperaturePosY);
  clearTemperature(houseTemperature, houseTemperaturePosX, houseTemperaturePosY);
}

void printTemperature(int temperature, int temperaturePosX, int temperaturePosY) {
  String temperatureText = String(temperature) + "C";
  char printout[5];
  temperatureText.toCharArray(printout, 5);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text(printout, temperaturePosX, temperaturePosY);
}

void clearTemperature(int temperature, int temperaturePosX, int temperaturePosY) {
  String temperatureText = String(temperature) + "C";
  char printout[5];
  temperatureText.toCharArray(printout, 5);
  EsploraTFT.stroke(0, 0, 0);
  EsploraTFT.text(printout, temperaturePosX, temperaturePosY);
}

