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
  int circleSize;
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
const Position NOISE_POS = {20, 15};

const RGB BACKGROUND_COLOR = { 0, 0, 0 };

const Position STATUS_BASE_POS = {120, 30};
const int STATUS_LINE_HEIGHT = 15;

const int LUMINOSITY_DARK = 10;

const int MAX_HP = 100;
const int MAX_HUNGER = 100;
const int MAX_SLEEP = 100;
const int DEFAULT_SIZE = 10;
Status status = { MAX_HP, MAX_HUNGER, MAX_SLEEP, DEFAULT_SIZE };

const int SLEEP_CYCLE = 5 * 10;
const int HUNGER_CYCLE = 10 * 10;
unsigned long cycles = 0;

const int FOOD_HUNGER_BONUS = 10;
int circleSize = 10;

const int NOISE_MAX = 100;
const int NOISE_MIN = 0;

void setup() {
  EsploraTFT.begin();
  EsploraTFT.background(BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b);
  screenWidth = EsploraTFT.width();
  screenHeight = EsploraTFT.height();

  EsploraTFT.stroke(255, 255, 255);
  EsploraTFT.text("ENV ", (ENVIRONMENT_TEMP_POS.x - 20), ENVIRONMENT_TEMP_POS.y);
  EsploraTFT.text("ARC ", (AR_CONDITIONER_TEMP_POS.x - 20), AR_CONDITIONER_TEMP_POS.y);
  EsploraTFT.text("LUM ", (LUMINOSITY_POS.x - 20), LUMINOSITY_POS.y);
  EsploraTFT.text("NOI ", (NOISE_POS.x - 20), NOISE_POS.y);
  EsploraTFT.text("HP", STATUS_BASE_POS.x, STATUS_BASE_POS.y);
  EsploraTFT.text("HUN", STATUS_BASE_POS.x, STATUS_BASE_POS.y + STATUS_LINE_HEIGHT);
  EsploraTFT.text("SLE", STATUS_BASE_POS.x, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 2) );
  EsploraTFT.text("STA", STATUS_BASE_POS.x, STATUS_BASE_POS.y + (STATUS_LINE_HEIGHT * 3) );

  EsploraTFT.rect(0,30, 115, (screenHeight - 35));
} 

void loop() {  
  EsploraTFT.fill(255,255,255);
  EsploraTFT.circle(screenWidth/2 - 30, screenHeight/2, status.circleSize);

  RGB white = { 255, 255, 255 };
  
  environmentTemperature = Esplora.readTemperature(DEGREES_C);
  printValue(environmentTemperature, ENVIRONMENT_TEMP_POS, white, "C");

  houseTemperature =  map(Esplora.readSlider(), 1023, 0, AR_CONDITIONER_MIN_TEMP, AR_CONDITIONER_MAX_TEMP);
  printValue(houseTemperature, AR_CONDITIONER_TEMP_POS, white, "C");

  int luminosity = map(Esplora.readLightSensor(), 1023, 0, 100, 0);
  printValue(luminosity, LUMINOSITY_POS, white, "%");

  int noise = collectNoise();
  int noiseMapped = map(noise, 1023, 0, NOISE_MAX, NOISE_MIN);
  printValue(noiseMapped, NOISE_POS, white, "%");

  if(noise > 0) {
    Esplora.tone(440, 10);
  }

  if(isDark(luminosity) && noise == 0) {
    printSleepStatus(white);
    status = sleep(status);
  } else {
    clearSleepStatus();
  }

  TemperaturePerception temperaturePerception = getTemperaturePerception(environmentTemperature, houseTemperature);
  printTemperaturePerception(temperaturePerception);
  printStatus(status);

  if(temperaturePerception != regular || status.hunger < 20 || status.sleep < 20) {
    status.hp -= 1;
    Esplora.tone(523, 10); 
  }

  if(cycles % SLEEP_CYCLE == 0 && !isDark(luminosity)) {
    status.sleep -= 2;
  }

  if(cycles % HUNGER_CYCLE == 0) {
    status.hunger -= 1;
  }

  if(status.hunger == MAX_HUNGER) {
    status = heal(status);
  }

  int feedButton = Esplora.readButton(SWITCH_RIGHT);
  if(feedButton == LOW) {
      status = feed(status);
  }
  
  delay(100);
  clearValue(environmentTemperature, ENVIRONMENT_TEMP_POS, "C");
  clearValue(houseTemperature, AR_CONDITIONER_TEMP_POS, "C");
  clearValue(luminosity, LUMINOSITY_POS, "%");
  clearValue(noiseMapped, NOISE_POS, "%");
  cycles++;
}

void clearValue(int value, Position pos, String suffix) {
  printValue(value, pos, BACKGROUND_COLOR, suffix);
}

void printValue(int value, Position pos, RGB color, String suffix) {
  String text = String(value) + suffix;
  char printout[5];
  text.toCharArray(printout, 5);
  EsploraTFT.stroke(color.r, color.g, color.b);
  EsploraTFT.text(printout, pos.x, pos.y);
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
  status.circleSize += 10;
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

int collectNoise() {
  // Adapted From https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
  unsigned long startMillis= millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  unsigned int sample;
   while (millis() - startMillis < 100)
   {
      sample = Esplora.readMicrophone();
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   return peakToPeak;
}

