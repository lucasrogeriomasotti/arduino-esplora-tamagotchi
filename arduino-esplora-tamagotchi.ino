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

enum TemperaturePerception {
  cold,
  regular,
  hot
};

int screenWidth;
int screenHeight;

const Position ENVIRONMENT_TEMP_POS = {20, 5};
int environmentTemperature;

const Position AR_CONDITIONER_TEMP_POS = {65, 5};
int houseTemperature;
const int AR_CONDITIONER_MIN_TEMP = 5;
const int AR_CONDITIONER_MAX_TEMP = 30;

const Position LUMINOSITY_POS = {110, 5};

const RGB BACKGROUND_COLOR = { 0, 0, 0 };

const Position STATUS_BASE_POS = {100, 30};
const int STATUS_LINE_HEIGHT = 15;

const int LUMINOSITY_DARK = 10;

void setup() {
  EsploraTFT.begin();
  EsploraTFT.background(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  screenWidth = EsploraTFT.width();
  screenHeight = EsploraTFT.height();

  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text("ENV ", (ENVIRONMENT_TEMP_POS.x - 20), ENVIRONMENT_TEMP_POS.y);
  EsploraTFT.text("ARC ", (AR_CONDITIONER_TEMP_POS.x - 20), AR_CONDITIONER_TEMP_POS.y);
  EsploraTFT.text("LUM ", (LUMINOSITY_POS.x - 20), LUMINOSITY_POS.y);
  EsploraTFT.text("HP", STATUS_BASE_POS.x, STATUS_BASE_POS.y);
  EsploraTFT.text("HUN", STATUS_BASE_POS.x, STATUS_BASE_POS.y + STATUS_LINE_HEIGHT);
  EsploraTFT.text("SLE", STATUS_BASE_POS.x, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 2) );
  EsploraTFT.text("STA", STATUS_BASE_POS.x, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 3) );
} 

void loop() {  
  EsploraTFT.fill(255,255,255);
  EsploraTFT.circle(screenWidth/2 - 30, screenHeight/2, 10);

  RGB white = { 255, 255, 255 };
  
  environmentTemperature = Esplora.readTemperature(DEGREES_C);
  printTemperature(environmentTemperature, ENVIRONMENT_TEMP_POS, white);

  houseTemperature =  map(Esplora.readSlider(), 1023, 0, AR_CONDITIONER_MIN_TEMP, AR_CONDITIONER_MAX_TEMP);
  printTemperature(houseTemperature, AR_CONDITIONER_TEMP_POS, white);

  int luminosity = map(Esplora.readLightSensor(), 1023, 0, 100, 0);
  printLuminosity(luminosity, LUMINOSITY_POS, white);

  if(isDark(luminosity)) {
    printSleepStatus(white);
  } else {
    clearSleepStatus();
  }

  TemperaturePerception temperaturePerception = getTemperaturePerception(environmentTemperature, houseTemperature);
  printTemperaturePerception(temperaturePerception);
  
  delay(1000);
  clearTemperature(environmentTemperature, ENVIRONMENT_TEMP_POS);
  clearTemperature(houseTemperature, AR_CONDITIONER_TEMP_POS);
  clearLight(luminosity, LUMINOSITY_POS);
}

void clearLight(int luminosity, Position pos) {
  printLuminosity(luminosity, pos, BACKGROUND_COLOR);
}

void printLuminosity(int luminosity, Position pos, RGB color) {
  String luminosityText = String(luminosity) + "%";
  char printout[4];
  luminosityText.toCharArray(printout, 4);
  EsploraTFT.stroke(color.r, color.g, color.b);
  EsploraTFT.text(printout, pos.x, pos.y);
}

void printTemperature(int temperature, Position pos, RGB color) {
  String temperatureText = String(temperature) + "C";
  char printout[4];
  temperatureText.toCharArray(printout, 4);
  EsploraTFT.stroke(color.r, color.g, color.b);
  EsploraTFT.text(printout, pos.x, pos.y);
}

void clearTemperature(int temperature, Position pos) {
  printTemperature(temperature, pos, BACKGROUND_COLOR);
}

boolean isDark(int luminosity) {
  return luminosity < LUMINOSITY_DARK;
}

void printSleepStatus(RGB color) {
  EsploraTFT.stroke(color.r, color.g, color.b);
  EsploraTFT.text("SLE", STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 3) );
}
void clearSleepStatus() {
  printSleepStatus(BACKGROUND_COLOR);
}

TemperaturePerception getTemperaturePerception(int environmentTemperature, int houseTemperature) {
  int mediumTemperature = (environmentTemperature + houseTemperature) / 2;
  if(mediumTemperature < 16) {
    return cold;
  } else if(mediumTemperature >= 16 && mediumTemperature < 26) {
    return regular;
  } else {
    return hot;
  }
}

void printTemperaturePerception(TemperaturePerception temperature) {
  char printout[4];
  String temperatureText = String(temperature) + "C";
  RGB color;
  switch(temperature) {
    case cold:
      temperatureText = "COL";
      Esplora.writeRGB(0, 0, 100);
    break;
    case regular:
      temperatureText = "REG";
      Esplora.writeRGB(0, 100, 0);
    break;
    case hot:
      temperatureText = "HOT";
      Esplora.writeRGB(100, 0, 0);
    break;
  }
  temperatureText.toCharArray(printout, 4);
  EsploraTFT.fill(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  EsploraTFT.rect(STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 4), 20, 10);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text(printout, STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 4) );
}

