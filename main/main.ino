#include <Arduino.h>
#include "rcmonitor.h"            // https://github.com/aollin/racechrono-ble-diy-device
#include <TFT_eSPI.h>
#include <cmath>
#include "tft_setup.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
int mainFont = 8;
int secondFont = 4;

//https://racechrono.com/support/equations/identifiers
struct strmon monitors[] = {
    {"Speed", "channel(device(gps), speed)*10.0", 0.36, -1},
    {"Delta lap time", "channel(device(lap), delta_lap_time)*10.0", 0.1, -1},
    {"Best lap time", "channel(device(lap), best_lap_time)*10.0", 0.1, -1},
    {"RPM", "channel(device(obd), rpm)", 1.0, 6800},
    {"Tyre temp fl", "channel(device(wheel), tyre_temp_fl_1)", 1.0, 25},
    {"Tyre temp fr", "channel(device(wheel), tyre_temp_fr_1)", 1.0, 25},
    {"Brake temp fl", "channel(device(canbus), brake_temp_fl)", 1.0, 650},
    {"Brake temp fr", "channel(device(canbus), brake_temp_fr)", 1.0, 650},
    {"Coolant temp", "channel(device(obd), coolant_temp)", 1.0, 98},
    {"Intake temp", "channel(device(obd), intake_temp)", 1.0, -1},
};

#define BUFFSIZE 500
char buff[BUFFSIZE];

std::array<int, 8> convertValues();
void TFT_SET_BL(uint8_t Value);
void onScreen(uint8_t ScreenNumber);

int printAreas[4][2] = {
  {0, 0}, {240, 0}, {0, 160}, {240, 160}
};

#define LeftScreenCS 25
#define CenterScreenCS 26
#define RightScreenCS 27

#define TFT_GREEN_GAIN 0x0400

uint32_t background_color = 0x165d;
uint32_t background_color_backup = 0x165d;

void setup() {
  Serial.begin(115200);

  pinMode(LeftScreenCS,OUTPUT);
  pinMode(CenterScreenCS,OUTPUT);
  pinMode(RightScreenCS,OUTPUT);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  sprite.createSprite(239, 159);
  
  rcmonitorstart();
  TFT_SET_BL(75);
}


void loop() {
  if (rcmonitor()) {

    bool flash = millis() / 500 % 2; // toggles every 500 ms if the value exceed the alarm threshold

    onScreen(1);
    for (int index = 8; index < 10; index++) {
      int monitorValue = (int)monitorValues[index] * monitorMultipliers[index];

      sprite.setTextSize(1);

      if (monitorValue > 10000) {
        monitorValue = 0;
      }

      if ( monitors[index].alertLevel != -1 && monitorValue >+ monitors[index].alertLevel ) {
        background_color = flash ? TFT_RED : background_color_backup;
      } else {
        background_color = background_color_backup;
      }
      sprite.fillSprite(background_color);

      sprite.setTextDatum(MC_DATUM);// middle center

      sprite.setTextColor(TFT_BLACK, background_color);
      sprite.drawNumber((int)monitorValue, 
                              109, 74, mainFont);// center of 218x148
      sprite.setTextSize(1);
      sprite.setTextDatum(BL_DATUM);
      sprite.drawString(monitors[index].nam, 
                            5, 155, secondFont);

      sprite.pushSprite(printAreas[index-8][0] + 1, 
                            printAreas[index-8][1] + 1);
    }

    onScreen(2);
    for (int index = 0; index < 4; index++) {
      float monitorValue = monitorValues[index] * monitorMultipliers[index];

      sprite.setTextSize(1);

      if (monitorValue > 10000) {
        monitorValue = 0;
      }

      if ( monitors[index].alertLevel != -1 && monitorValue >= monitors[index].alertLevel ) {
        background_color = flash ? TFT_RED : background_color_backup;
      } else {
        background_color = background_color_backup;
      }
      sprite.fillSprite(background_color);

      sprite.setTextDatum(MC_DATUM);// middle center

      if (strcmp(monitors[index].nam, "Delta lap time") == 0) {
        if (monitorValue <= 0) {
          sprite.setTextColor(TFT_GREEN_GAIN, background_color);
        } else {
          sprite.setTextColor(TFT_RED, background_color);
        }
        sprite.drawFloat(monitorValue, 2, 
                              120, 74, mainFont);// center of 218x148
        sprite.setTextColor(TFT_BLACK, background_color);

      } else if (strcmp(monitors[index].nam, "RPM") == 0){
          if (monitorValue >= 4000 && monitorValue < 6800) {
            sprite.setTextColor(TFT_GREEN_GAIN, background_color);
        } else if (monitorValue < 4000) {
          sprite.setTextColor(TFT_YELLOW, background_color);
        }
        sprite.drawNumber((int)monitorValue,
                              120, 74, mainFont);// center of 218x148
        sprite.setTextColor(TFT_BLACK, background_color);

      } else {
        sprite.setTextColor(TFT_BLACK, background_color);
        sprite.drawNumber((int)monitorValue, 
                              120, 74, mainFont);// center of 218x148
      }
      sprite.setTextSize(1);
      sprite.setTextDatum(BL_DATUM);
      sprite.drawString(monitors[index].nam, 
                            5, 155, secondFont);

      sprite.pushSprite(printAreas[index][0] + 1, 
                            printAreas[index][1] + 1);
    }

    onScreen(3);
    for (int index = 4; index < 8; index++) {
      int monitorValue = (int)monitorValues[index] * monitorMultipliers[index];

      sprite.setTextSize(1);

      if (monitorValue > 10000) {
        monitorValue = 0;
      }

      if ( monitors[index].alertLevel != -1 && monitorValue >= monitors[index].alertLevel ) {
        background_color = flash ? TFT_RED : background_color_backup;
      } else {
        background_color = background_color_backup;
      }
      sprite.fillSprite(background_color);

      sprite.setTextDatum(MC_DATUM);// middle center

      if (strcmp(monitors[index].nam, "Brake temp fl") == 0 || strcmp(monitors[index].nam, "Brake temp fr") == 0){
        if (monitorValue >= 200 && monitorValue < 650) {
          sprite.setTextColor(TFT_GREEN_GAIN, background_color);
        } else if (monitorValue < 200) {
          sprite.setTextColor(TFT_YELLOW, background_color);
        }
        sprite.drawNumber((int)monitorValue,
                              109, 74, mainFont);// center of 218x148
        sprite.setTextColor(TFT_BLACK, background_color);

      } else {
        sprite.setTextColor(TFT_BLACK, background_color);
        sprite.drawNumber((int)monitorValue, 
                              109, 74, mainFont);// center of 218x148
      }
      sprite.setTextSize(1);
      sprite.setTextDatum(BL_DATUM);
      sprite.drawString(monitors[index].nam, 
                            5, 155, secondFont);

      sprite.pushSprite(printAreas[index-4][0] + 1, 
                            printAreas[index-4][1] + 1);
    }
  }
  delay(100);
}

void TFT_SET_BL(uint8_t Value) {
  if (Value < 0 || Value > 100) {
    printf("TFT_SET_BL Error \r\n");
  } else {
    analogWrite(TFT_BL, Value * 2.55);
  }
}

void onScreen(uint8_t ScreenNumber) {
  if(ScreenNumber == 0) {
    digitalWrite(LeftScreenCS,0);
    digitalWrite(CenterScreenCS,0);
    digitalWrite(RightScreenCS,0);
  } else if(ScreenNumber == 1) {
    digitalWrite(LeftScreenCS,0);
    digitalWrite(CenterScreenCS,1);
    digitalWrite(RightScreenCS,1);
  } else if (ScreenNumber == 2) {
    digitalWrite(LeftScreenCS,1);
    digitalWrite(CenterScreenCS,0);
    digitalWrite(RightScreenCS,1);
  } else if (ScreenNumber == 3) {
    digitalWrite(LeftScreenCS,1);
    digitalWrite(CenterScreenCS,1);
    digitalWrite(RightScreenCS,0);
  } else {
    Serial.println("Wrong screen number selected");
  }
}