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

struct Status {
  int hp;
  int hunger;
  int sleep;
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

const Position STATUS_BASE_POS = {120, 30};
const int STATUS_LINE_HEIGHT = 15;

const int LUMINOSITY_DARK = 10;

const int MAX_HP = 100;
const int MAX_HUNGER = 100;
const int MAX_SLEEP = 100;
Status status = { MAX_HP, MAX_HUNGER, MAX_SLEEP};

const int SLEEP_CYCLE = 5;
const int HUNGER_CYCLE = 10;
unsigned long cycles = 0;

const int FOOD_HUNGER_BONUS = 10;

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
    status = sleep(status);
  } else {
    clearSleepStatus();
  }

  TemperaturePerception temperaturePerception = getTemperaturePerception(environmentTemperature, houseTemperature);
  printTemperaturePerception(temperaturePerception);
  printStatus(status);

  if(temperaturePerception != regular) {
    status.hp -= 1;
  }

  if(cycles % SLEEP_CYCLE == 0) {
    status.sleep -= 2;
  }

  if(cycles % HUNGER_CYCLE == 0) {
    status.hunger -= 1;
  }

  if(status.hunger == MAX_HUNGER) {
    status = heal(status);
  }

  int button = Esplora.readButton(SWITCH_RIGHT);
  if(button == LOW) {
      status = feed(status);
  }
  
  delay(500);
  clearTemperature(environmentTemperature, ENVIRONMENT_TEMP_POS);
  clearTemperature(houseTemperature, AR_CONDITIONER_TEMP_POS);
  clearLight(luminosity, LUMINOSITY_POS);
  cycles++;
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
  EsploraTFT.text("SLE", STATUS_BASE_POS.x + 10, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 5) );
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
  EsploraTFT.stroke(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  EsploraTFT.rect(STATUS_BASE_POS.x + 10, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 4), 20, 10);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text(printout, STATUS_BASE_POS.x + 10, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 4) );
}

void printStatus(Status status) {
  char hpPrintout[5];
  String(status.hp).toCharArray(hpPrintout, 5);
  char hungerPrintout[5];
  String(status.hunger).toCharArray(hungerPrintout, 5);
  char sleepPrintout[5];
  String(status.sleep).toCharArray(sleepPrintout, 5);
  
  EsploraTFT.stroke(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  EsploraTFT.fill(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  EsploraTFT.rect(STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y, 40, 40);
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text(hpPrintout, STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y);
  EsploraTFT.text(hungerPrintout, STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y + STATUS_LINE_HEIGHT);
  EsploraTFT.text(sleepPrintout, STATUS_BASE_POS.x + 20, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 2) );
}

Status sleep(Status s) {
  if(s.sleep < MAX_SLEEP) {
    s.sleep += 1;
  }
  return s;
}

Status feed(Status s) {
  int newHunger = s.hunger + FOOD_HUNGER_BONUS;
  if(newHunger > MAX_HUNGER) {
    newHunger = 100;
  }
  status.hunger = newHunger;
  printFoodMessage();
  return status;
}

Status heal(Status s) {
  if(s.hp < MAX_HP) {
    s.hp += 1;
  }
  return s;
}

void printFoodMessage() {
  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text("FOOD!", screenWidth/2 - 40, screenHeight/2 - 20);
  delay(200);
  EsploraTFT.stroke(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  EsploraTFT.text("FOOD!", screenWidth/2 - 40, screenHeight/2 - 20);
}

