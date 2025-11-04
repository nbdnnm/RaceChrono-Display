#include <Arduino.h>
#include "rcmonitor.h"            // https://github.com/aollin/racechrono-ble-diy-device
#include <TFT_eSPI.h>
#include <cmath>
#include "tft_setup.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

struct strmon monitors[] = {
    {"speed", "channel(device(gps), speed)*10.0", 0.36, -1},
    {"delta_lap_time", "channel(device(lap), delta_lap_time)*10.0", 0.1, -1},
    {"delta_speed", "channel(device(gps), delta_speed)*10.0", 0.36, -1},
    {"rpm", "channel(device(obd), rpm)", 1.0, 6800},
    {"tyre_temp_fl", "channel(device(wheel), tyre_temp_fl)", 1.0, 25},
    {"tyre_temp_fr", "channel(device(wheel), tyre_temp_fr)", 1.0, 25},
    {"brake_temp_fl", "channel(device(canbus), brake_temp_fl)", 1.0, 650},
    {"brake_temp_fr", "channel(device(canbus), brake_temp_fr)", 1.0, 650},
    // {"coolant_temp", "channel(device(obd), coolant_temp)", 1.0, 97},
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

uint32_t background_color = 0x165d;
uint32_t background_color_backup = 0x165d;

void setup() {
  Serial.begin(115200);

  pinMode(LeftScreenCS,OUTPUT);
  pinMode(CenterScreenCS,OUTPUT);
  pinMode(RightScreenCS,OUTPUT);

  // onScreen(3);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(background_color
);
  sprite.createSprite(239, 159);
  for(int i = 0; i<4; i++) {
    tft.drawRect(printAreas[i][0], printAreas[i][1], 239, 159, TFT_BLACK);
  }
  rcmonitorstart();
  TFT_SET_BL(100);
}


void loop() {
  if (rcmonitor()) {
    std::array<int, 8> convertedValues = convertValues();

    for (int i = 0; i < 4; i++) {
      sprite.fillSprite(background_color);// clear sprite
      onScreen(1);

      sprite.setTextSize(2);

      bool flash = millis() / 500 % 2; // toggles every 500 ms
      if ( monitors[i].alertLevel != -1 && convertedValues[i] > monitors[i].alertLevel && convertedValues[i] < INT16_MAX ) {
        background_color = flash ? TFT_RED : background_color_backup;
      } else {
        background_color = background_color_backup;
      }
      sprite.fillSprite(background_color);

      sprite.setTextColor(TFT_BLACK, background_color);// set text color
      sprite.setTextDatum(MC_DATUM);// middle center
      sprite.drawNumber(convertedValues[i], 
                            109, 74, 7);// center of 218x148

      sprite.setTextDatum(BL_DATUM);
      sprite.drawString(monitors[i].nam, 
                            5, 155, 1);

      sprite.pushSprite(printAreas[i][0] + 1, 
                            printAreas[i][1] + 1);
    }

    for (int i = 0; i < 4; i++) {
      sprite.fillSprite(background_color);// clear sprite
      onScreen(3);

      sprite.setTextSize(2);

      bool flash = millis() / 500 % 2; // toggles every 500 ms
      if ( monitors[i+4].alertLevel != -1 && convertedValues[i+4] > monitors[i+4].alertLevel && convertedValues[i+4] < INT16_MAX ) {
        background_color = flash ? TFT_RED : background_color_backup;
      } else {
        background_color = background_color_backup;
      }
      sprite.fillSprite(background_color);

      sprite.setTextColor(TFT_BLACK, background_color);// set text color
      sprite.setTextDatum(MC_DATUM);// middle center
      sprite.drawNumber(convertedValues[i+4], 
                            109, 74, 7);// center of 218x148

      sprite.setTextDatum(BL_DATUM);
      sprite.drawString(monitors[i+4].nam, 
                            5, 155, 1);

      sprite.pushSprite(printAreas[i][0] + 1, 
                            printAreas[i][1] + 1);
    }

    snprintf(buff, BUFFSIZE,
             "speed=%d delta_lap_time=%d delta_speed=%d rpm=%d tyre_temp_fl=%d tyre_temp_fr=%d brake_temp_fl=%d brake_temp_fl=%d",
             convertedValues[0],
             convertedValues[1],
             convertedValues[2],
             convertedValues[3],
             convertedValues[4],
             convertedValues[5],
             convertedValues[6],
             convertedValues[7]);
    Serial.println(buff);
  }
  delay(50);
}

void TFT_SET_BL(uint8_t Value) {
  if (Value < 0 || Value > 100) {
    printf("TFT_SET_BL Error \r\n");
  } else {
    analogWrite(TFT_BL, Value * 2.55);
  }
}


std::array<int, 8> convertValues() {
    int speed = (int)monitorValues[0] * monitorMultipliers[0];
    int delta_lap_time = (int)monitorValues[1] * monitorMultipliers[1];
    int rpm = (int) monitorValues[2] * monitorMultipliers[2];
    int delta_speed = (int) monitorValues[3] * monitorMultipliers[3];
    int tyre_temp_fl = (int)monitorValues[4] * monitorMultipliers[4];
    int tyre_temp_fr = (int)monitorValues[5] * monitorMultipliers[5];
    int brake_temp_fl = (int) monitorValues[6] * monitorMultipliers[6];
    int brake_temp_fr = (int) monitorValues[7] * monitorMultipliers[7];
    std::array<int, 8> arr;
    arr[0] = speed;
    arr[1] = delta_lap_time;
    arr[2] = rpm;
    arr[3] = delta_speed;
    arr[4] = tyre_temp_fl;
    arr[5] = tyre_temp_fr;
    arr[6] = brake_temp_fl;
    arr[7] = brake_temp_fr;
    return arr;
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