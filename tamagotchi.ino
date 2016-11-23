#include <Esplora.h>
#include <TFT.h>

struct RGB {
  byte r;
  byte g;
  byte b;
};

struct Position {
  int x;
  int y;
};

int screenWidth;
int screenHeight;

const Position ENVIRONMENT_TEMP_POS = {20, 5};
int environmentTemperature;

const Position AR_CONDITIONER_TEMP_POS = {65, 5};
int houseTemperature;

const RGB BACKGROUND_COLOR = { 0, 0, 0 };

void setup() {
  EsploraTFT.begin();
  EsploraTFT.background(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  screenWidth = EsploraTFT.width();
  screenHeight = EsploraTFT.height();

  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text("ENV", (ENVIRONMENT_TEMP_POS.x - 20), ENVIRONMENT_TEMP_POS.y);
  EsploraTFT.text("HOU", (AR_CONDITIONER_TEMP_POS.x - 20), AR_CONDITIONER_TEMP_POS.y);
}

void loop() {  
  EsploraTFT.fill(255,255,255);
  EsploraTFT.circle(screenWidth/2, screenHeight/2, 15);

  /*
  EsploraTFT.noFill();
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.rect(screenWidth/2, screenHeight/2, 80, 80);
  */

  RGB white = { 255, 255, 255 };
  
  environmentTemperature = Esplora.readTemperature(DEGREES_C);
  printTemperature(environmentTemperature, ENVIRONMENT_TEMP_POS, white);

  houseTemperature =  map(Esplora.readSlider(), 1023, 0, 10, 35);
  printTemperature(houseTemperature, AR_CONDITIONER_TEMP_POS, white);
    
  delay(1000);
  clearTemperature(environmentTemperature, ENVIRONMENT_TEMP_POS);
  clearTemperature(houseTemperature, AR_CONDITIONER_TEMP_POS);
}

void printTemperature(int temperature, Position pos, RGB color) {
  String temperatureText = String(temperature) + "C";
  char printout[5];
  temperatureText.toCharArray(printout, 5);
  EsploraTFT.stroke(color.r, color.g, color.b);
  EsploraTFT.text(printout, pos.x, pos.y);
}

void clearTemperature(int temperature, Position pos) {
  printTemperature(temperature, pos, BACKGROUND_COLOR);
}
