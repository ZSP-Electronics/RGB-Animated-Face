#define GFX_DEV_DEVICE LILYGO_T_RGB
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include "board.h"
#include <RGBroboFace.h>


/*
USING LILYGO TRGB 2.8in TFT Round LCD screen
Look at instructables to learn how to configure Arduino GFX for other RGB screens
https://www.instructables.com/ArduinoGFX/
*/

extern const uint8_t st7701_type9_init_operations2[297];
uint32_t timer = 0;
uint8_t mood = 0;

/* Inilialize the Panel */
Arduino_DataBus *bus = new Arduino_XL9535SWSPI(BOARD_I2C_SDA, BOARD_I2C_SCL, XL_PWD, XL_CS, XL_SCK, XL_MOSI);
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
  BOARD_TFT_DE, BOARD_TFT_VSYNC, BOARD_TFT_HSYNC, BOARD_TFT_PCLK,
  //Red
  BOARD_TFT_R1, BOARD_TFT_R2, BOARD_TFT_R3, BOARD_TFT_R4, BOARD_TFT_R5,
  BOARD_TFT_G0, BOARD_TFT_G1, BOARD_TFT_G2, BOARD_TFT_G3, BOARD_TFT_G4, BOARD_TFT_G5,
  BOARD_TFT_B1, BOARD_TFT_B2, BOARD_TFT_B3, BOARD_TFT_B4, BOARD_TFT_B5,
  1 /* hsync_polarity */, 50 /* hsync_front_porch */, 1 /* hsync_pulse_width */, 30 /* hsync_back_porch */,
  1 /* vsync_polarity */, 20 /* vsync_front_porch */, 1 /* vsync_pulse_width */, 30 /* vsync_back_porch */,
  1 /* pclk_active_neg */);

// ! T-RGB 2.8 inches Uncomment below
Arduino_RGB_Display *display = new Arduino_RGB_Display(BOARD_TFT_WIDTH, BOARD_TFT_HEIGHT, rgbpanel, 0 /* rotation */, true /* auto_flush */, bus, GFX_NOT_DEFINED /* RST */, st7701_type9_init_operations2, sizeof(st7701_type9_init_operations2));
RGBroboFace face(BOARD_TFT_WIDTH, BOARD_TFT_HEIGHT);

void setup() {

  Serial.begin(115200);

  Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL, 400000);
  face.begin(display, 100);

  // Backlight needed high for screen. May not be needed for other screens
  pinMode(BOARD_TFT_BL, OUTPUT);
  digitalWrite(BOARD_TFT_BL, HIGH);


  //face.setSectorLines(ON);
  //face.setCrosshair(ON);

  // Define some automated eyes behaviour
  face.setAutoblinker(ON, 15, 8); // Start auto blinker animation cycle -> bool active, int interval, int variation -> turn on/off, set interval between each blink in full seconds, set range for random interval variation in full seconds
  //face.setAutoblinker(ON, 3, 2);
  //face.setCuriosity(ON, 2, 10, 75); // Start idle animation cycle (eyes looking in random directions) -> turn on/off, set interval between each eye repositioning in full seconds, set range for random time interval variation in full seconds

  // // Define eye shapes, all values in pixels
  face.setWidth(80, 80);         // byte leftEye, byte rightEye
  face.setHeight(100, 100);      // byte leftEye, byte rightEye
  face.setBorderradius(40, 40);  // byte leftEye, byte rightEye
  face.setSpacebetween(15);      // int space -> can also be negative

  // Define mood, curiosity and position
  //face.setEyeExpression(SURPRISED); // mood expressions, can be TIRED, ANGRY, HAPPY, DEFAULT

  // Define Mouth parameters
  face.addMouth(ON);
  //face.setMouthExpression(HAPPY);
  // face.setMouthWidth(120);
  // face.setMouthHeight(120);
  // face.setMouthBorderRadius(60);
  // face.setMouthPadding(-25);

  timer = millis();
}

void loop() {
  face.update();  // update eyes drawings

  if (millis() - timer > 2500) {
    timer = millis();
    face.setFaceExpression(mood);
    mood++;

    if (mood > 5) {
      mood = 0;
    }
  }
}
