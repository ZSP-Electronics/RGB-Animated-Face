#ifndef RGB_ROBOFACE_H
#define RGB_ROBOFACE_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#ifndef BOARD_HAS_PSRAM
#error "Please turn on PSRAM to OPI !"
#endif

//#define FACE_DEBUG

// For turning things on or off
#define ON 1
#define OFF 0

#define EMOTION_MAX 100
#define LVL OFF
#define VALUE ON
#define VALUE_NEXT 2

// For Identifying eye numbers
#define LEFT 1
#define RIGHT 2
#define LEFT_NEXT 3
#define RIGHT_NEXT 4

// For switch "predefined positions"
#define N 1   // north, top center
#define NE 2  // north-east, top right
#define E 3   // east, middle right
#define SE 4  // south-east, bottom right
#define S 5   // south, bottom center
#define SW 6  // south-west, bottom left
#define W 7   // west, middle left
#define NW 8  // north-west, top left
#define C 9   // for middle center set "DEFAULT"

// For mood type switch
#define DEFAULT 0
#define ANGRY 1
#define HAPPY 2
#define SAD 3
#define TIRED 4
#define SURPRISED 5
#define FEAR 6
#define LOVE  7
#define DISGUST 8


struct FaceSector {
  int x_min;
  int x_max;
  int y_min;
  int y_max;
};

struct gridPoint {
  int x;
  int y;
};

struct EmotionLvls {
  uint8_t happiness;
  uint8_t surprise;
  uint8_t sadness;
  uint8_t tired;
  uint8_t anger;
  uint8_t fear;
  uint8_t disgust;
  uint8_t love;
};

struct eyes {
  int width;
  int height;
  int radius;
  uint16_t color;
  int distanceCenterX;
  int distanceCenterY;
  int heightOffset;
  int cX;
  int cY;
  bool open;
  bool blinking;
};

class RGBroboFace {
public:
  RGBroboFace(int width, int height, bool round_display = false);
  Arduino_Canvas *canvasGfx;

  //*********************************************************************************************
  //  GENERAL METHODS
  //*********************************************************************************************
  void begin(Arduino_GFX *display, uint8_t frameRate, bool dbug=false);
  void setFramerate(uint8_t fps);
  void update();

  //*********************************************************************************************
  //  SETTERS METHODS
  //*********************************************************************************************
  void setEyeColor(uint16_t color);
  void setEyeColor(uint16_t l_color, uint16_t r_color);
  void setBGColor(uint16_t color);

  void setCrosshair(bool active);
  void setSectorLines(bool active);

  // Set automated eye blinking, minimal blink interval in full seconds and blink interval variation range in full seconds
  void setAutoblinker(bool active, int interval, int variation);
  void setAutoblinker(bool active);

  void setWidth(int leftEye, int rightEye);
  void setHeight(int leftEye, int rightEye);
  void setHeightOffset(int leftEye, int rightEye);

  // Set border radius for left and right eye
  void setBorderradius(int leftEye, int rightEye);

  // Set space between the eyes, can also be negative
  void setSpacebetween(int space);

  // Set mood expression
  void setEyeExpression(unsigned char mood, unsigned char level = EMOTION_MAX);
  void setMouthExpression(unsigned char mood);

  void setEmotionLevel(EmotionLvls level);

  void setCuriosity(bool curiousBit, int interval, int variation, uint16_t radius = 0);
  void setCuriosity(bool curiousBit);

  //*********************************************************************************************
  //  GETTERS METHODS
  //*********************************************************************************************
  uint8_t getSector(unsigned int pos_x, unsigned int pos_y);

  //*********************************************************************************************
  //  MOUTH ADDITIONS
  //*********************************************************************************************
  void addMouth(bool active);
  void addMouth(bool active, int padding);
  void setMouthBorderRadius(int mouth);
  void setMouthWidth(int sizeX);
  void setMouthHeight(int sizeY);
  void setMouthPadding(int padding);

  //*********************************************************************************************
  //  TOUCH VARIABLES
  //*********************************************************************************************
  //TouchDrv *_touchDrv;


  //*********************************************************************************************
  //  OTHER FUNCTIONAL METHODS
  //*********************************************************************************************
  void setBrightness(uint8_t level);
  uint8_t getBrightness();
  void setFaceExpression(uint8_t mood);


private:
  Arduino_GFX *_display;

  //*********************************************************************************************
  //  PRE-CALCULATIONS AND ACTUAL DRAWINGS
  //*********************************************************************************************
  void drawFace();
  void drawEyes();
  void drawMouth();
  void debug();
  void close(bool left, bool right);
  void open(bool left, bool right);
  void blink(bool left, bool right);
  void close() {
    _eyes[LEFT_NEXT].height = 1;  // closing left eye
    _eyes[RIGHT_NEXT].height = 1;  // closing right eye
    _eyes[LEFT].open = false;   // left eye not opened (=closed)
    _eyes[RIGHT].open = false;  // right eye not opened (=closed)
    _eyes[LEFT].blinking = true;
    _eyes[RIGHT].blinking = true;
  }

  // Open both eyes
  void open() {
    _eyes[LEFT].open = true;   // left eye opened - if true, drawEyes() will take care of opening eyes again
    _eyes[RIGHT].open = true;  // right eye opened
  }

  // Trigger eyeblink animation
  void blink() {
    close();
    open();
  }

  // For general setup - screen size and max. frame rate
  int _screenWidth;            // OLED display width, in pixels
  int _screenHeight;           // OLED display height, in pixels
  int _frameInterval = 20;     // default value for 50 frames per second (1000/50 = 20 milliseconds)
  unsigned long fpsTimer = 0;  // for timing the frames per second
  uint8_t _framedivisor = 4;

  FaceSector _sectors[9];
  eyes _eyes[5];  //Create default structure for eyes

  float _faceCenter_x, _faceCenter_xNext = 0;
  float _faceCenter_y, _faceCenter_yNext = 0;
  uint16_t _eyeColorDefault = WHITE;
  uint16_t _bgColor = BLACK;
  uint8_t _padding = 0;
  uint16_t _sector_sizeX;
  uint16_t _sector_sizeY;

  EmotionLvls _emotions[3];

  bool cyclops = false;  // if true, draw only one eye
  bool idle_center = false;
  bool cross_hair = false;
  bool sector_lines = false;


  //*********************************************************************************************
  //  Eyes Geometry
  //*********************************************************************************************

  //DEFAULTS
  int eyeWidthDefault = 50;
  int eyeHeightDefault = 50;
  int eyeHeightClosedDefault = 1;
  int eyeHeightOffsetDefault = 0;
  int eyeBorderRadiusDefault = 8;
  int spaceBetweenDefault = 5;
  int spaceHeightDefault = 10;

  // EYELIDS
  int eyelidRadius = 0;
  int eyelidRadiusNext;
  int eyelidPadding = 0;
  int eyelidPaddingNext;

  //*********************************************************************************************
  //  Mouth Geometry
  //*********************************************************************************************

  // Mouth Defaults
  int mouthLx, mouthLx_Next = 0;
  int mouthLy, mouthLy_Next = 0;

  bool mouthActive = false;
  
  int mouthPaddingDefault = -15;
  int mouthWidthDefault = 50;
  int mouthHeightDefault = 50;
  int mouthRadiusDefault = 25;
  uint16_t _mouthColor = WHITE;

  int mouthWidth, mouthWidthNext;
  int mouthHeight, mouthHeightNext;
  int mouthRadius, mouthRadiusNext;
  int mouthPadding, mouthPaddingNext;

  bool mouth_happy = 0;
  bool mouth_sad = 0;
  bool mouth_surprised = 0;

  int topMouthLip = 0;
  int topMouthLipNext;
  int topMouthRadius = 0;

  int bottomMouthLip = 0;
  int bottomMouthLipNext;

  int surprisedPadding = 0;
  int surprisedPaddingNext;


  //*********************************************************************************************
  //  Macro Animations
  //*********************************************************************************************

  // Animation - horizontal flicker/shiver
  bool hFlicker = 0;
  bool hFlickerAlternate = 0;
  int hFlickerAmplitude = 2;

  // Animation - vertical flicker/shiver
  bool vFlicker = 0;
  bool vFlickerAlternate = 0;
  int vFlickerAmplitude = 10;

  // Animation - auto blinking
  bool autoblinker = 0;            // activate auto blink animation
  int blinkInterval = 1;           // basic interval between each blink in full seconds
  int blinkIntervalVariation = 4;  // interval variaton range in full seconds, random number inside of given range will be add to the basic blinkInterval, set to 0 for no variation
  unsigned long blinktimer = 0;    // for organising eyeblink timing

  // Animation - idle mode: eyes looking in random directions
  bool idle = 0;
  int idleInterval = 1;                  // basic interval between each eye repositioning in full seconds
  int idleIntervalVariation = 3;         // interval variaton range in full seconds, random number inside of given range will be add to the basic idleInterval, set to 0 for no variation
  unsigned long idleAnimationTimer = 0;  // for organising eyeblink timing
  uint16_t idleRadius = 0;

  // Animation - eyes confused: eyes shaking left and right
  bool confused = 0;
  unsigned long confusedAnimationTimer = 0;
  int confusedAnimationDuration = 500;
  bool confusedToggle = 1;

  // Animation - eyes laughing: eyes shaking up and down
  bool laugh = 0;
  unsigned long laughAnimationTimer = 0;
  int laughAnimationDuration = 500;
  bool laughToggle = 1;

protected:
  bool setup_complete = false;
  bool _debug = false;
  uint16_t _FakebgColor = RED;
};
#endif