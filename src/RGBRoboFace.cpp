#include "RGBRoboFace.h"

RGBroboFace::RGBroboFace(int width, int height, bool round_display) {
  _screenWidth = width;
  _screenHeight = height;

  _sector_sizeX = _screenWidth / 5;
  _sector_sizeY = _screenHeight / 5;

  for (uint8_t i = 0; i < 3; i++) {
    for (uint8_t j = 0; j < 3; j++) {
      //Get sector min/max for Width
      _sectors[(3 * i) + j].x_min = _sector_sizeX * (j + 1);
      _sectors[(3 * i) + j].x_max = _sector_sizeX * (j + 2);
      //Get sector min/max for Height
      _sectors[(3 * i) + j].y_min = _sector_sizeY * (i + 1);
      _sectors[(3 * i) + j].y_max = _sector_sizeY * (i + 2);
    }
  }
}

//*********************************************************************************************
//  GENERAL METHODS
//*********************************************************************************************

void RGBroboFace::begin(Arduino_GFX *display, uint8_t frameRate, bool dbug) {
  _debug = dbug;
  _display = display;

  canvasGfx = new Arduino_Canvas(_screenWidth, _screenHeight, _display);

  // Init Display
  canvasGfx->begin();
  canvasGfx->fillScreen(_bgColor);
  canvasGfx->flush();
  setFramerate(frameRate);  // calculate frame interval based on defined frameRate

  // BUILD DEFAULT FACE
  // DEFAULT Position
  _faceCenter_x = _faceCenter_xNext = _screenWidth / 2;
  _faceCenter_y = _faceCenter_yNext = _screenHeight / 2;

  _eyes[DEFAULT].width = eyeWidthDefault;
  _eyes[DEFAULT].height = eyeHeightDefault;
  _eyes[DEFAULT].heightOffset = eyeHeightOffsetDefault;
  _eyes[DEFAULT].radius = eyeBorderRadiusDefault;
  _eyes[DEFAULT].distanceCenterX = spaceBetweenDefault;
  _eyes[DEFAULT].distanceCenterY = 0;
  _eyes[DEFAULT].color = _eyeColorDefault;
  _eyes[DEFAULT].cX = 0;
  _eyes[DEFAULT].cY = 0;
  _eyes[DEFAULT].open = true;
  _eyes[DEFAULT].blinking = true;

  // BUILD EYE USING DEFAULTS
  // Start eyes as closed
  for (uint8_t i = 1; i <= 4; i++) {
    _eyes[i].width = eyeWidthDefault;
    _eyes[i].height = eyeHeightDefault;
    _eyes[i].heightOffset = eyeHeightOffsetDefault;
    _eyes[i].radius = eyeBorderRadiusDefault;
    _eyes[i].distanceCenterX = spaceBetweenDefault;
    if (mouthActive) _eyes[i].distanceCenterY = spaceHeightDefault;
    else _eyes[i].distanceCenterY = 0;
    _eyes[i].color = _eyeColorDefault;
    _eyes[i].open = false;
    _eyes[i].blinking = false;

  }

  // EYE - Coordinates
  // Left
  _eyes[LEFT].cX = _faceCenter_x - (eyeWidthDefault + spaceBetweenDefault);
  _eyes[LEFT].cY = _faceCenter_y;

  // Right
  _eyes[RIGHT].cX = _faceCenter_x + spaceBetweenDefault + 1;
  _eyes[RIGHT].cY = _faceCenter_y;

  // SET Eyes to open
  open();

  // BOTH EYES
  // Eyelid top size
  eyelidRadiusNext = eyelidRadius;
  eyelidPaddingNext = eyelidPadding;

  //Mouth Init
  mouthLx = mouthLx_Next = _faceCenter_x;
  mouthLy = _faceCenter_y;
  mouthLy_Next = _faceCenter_y + mouthPaddingDefault;

  mouthWidth = mouthWidthNext = mouthWidthDefault;
  mouthHeight = mouthHeightNext = mouthHeightDefault;
  mouthRadius = mouthRadiusNext = mouthRadiusDefault;
  mouthPadding = mouthPaddingNext = mouthPaddingDefault;

  // Init Emotions
  _emotions[LVL].happiness = 0;
  _emotions[LVL].anger = 0;
  _emotions[LVL].sadness = 0;
  _emotions[LVL].fear = 0;
  _emotions[LVL].disgust = 0;
  _emotions[LVL].surprise = 0;
  _emotions[LVL].tired = 0;

  _emotions[VALUE].happiness = _emotions[VALUE_NEXT].happiness = _eyes[DEFAULT].height / 2;
  _emotions[VALUE].anger = _emotions[VALUE_NEXT].anger = _eyes[DEFAULT].height / 2;
  _emotions[VALUE].sadness = _emotions[VALUE_NEXT].sadness = _eyes[DEFAULT].height / 2;
  _emotions[VALUE].fear = _emotions[VALUE_NEXT].fear = 0;
  _emotions[VALUE].disgust = _emotions[VALUE_NEXT].disgust = _eyes[DEFAULT].height / 2;
  _emotions[VALUE].surprise = _emotions[VALUE_NEXT].surprise = 0;
  _emotions[VALUE].tired = _emotions[VALUE_NEXT].tired = _eyes[DEFAULT].height / 2;

#ifdef FACE_DEBUG
  debug();
#endif

  // Init Touch
  // if (touch) {
  //   _touchDrv = touch;
  //   _touchDrv->init();
  // }
}

void RGBroboFace::update() {
  // Limit drawing updates to defined max framerate
  if (millis() - fpsTimer >= _frameInterval) {
    drawFace();
    fpsTimer = millis();
  }
  setup_complete = true;
}

void RGBroboFace::setFramerate(uint8_t fps) {
  _frameInterval = 1000 / fps;
}

//*********************************************************************************************
//  SETTERS METHODS
//*********************************************************************************************
// Set automated eye blinking, minimal blink interval in full seconds and blink interval variation range in full seconds
void RGBroboFace::setAutoblinker(bool active, int interval, int variation) {
  autoblinker = active;
  blinkInterval = interval;
  blinkIntervalVariation = variation;
}
void RGBroboFace::setAutoblinker(bool active) {
  autoblinker = active;
}


void RGBroboFace::setWidth(int leftEye, int rightEye) {
  _eyes[LEFT_NEXT].width = leftEye;
  _eyes[RIGHT_NEXT].width = rightEye;
  _eyes[DEFAULT].width = leftEye;

  if (!setup_complete) {
    _eyes[LEFT].width = leftEye;
    _eyes[RIGHT].width = rightEye;
  }
}

void RGBroboFace::setHeight(int leftEye, int rightEye) {
  _eyes[LEFT_NEXT].height = leftEye;
  _eyes[RIGHT_NEXT].height = rightEye;
  _eyes[DEFAULT].height = leftEye;

  if (!setup_complete) {
    _eyes[LEFT].height = leftEye;
    _eyes[RIGHT].height = rightEye;
  }
}

void RGBroboFace::setHeightOffset(int leftEye, int rightEye) {
  _eyes[LEFT_NEXT].heightOffset = leftEye;
  _eyes[RIGHT_NEXT].heightOffset = rightEye;
  _eyes[DEFAULT].heightOffset = leftEye;

  if (!setup_complete) {
    _eyes[LEFT].heightOffset = leftEye;
    _eyes[RIGHT].heightOffset = rightEye;
  }
}

// Set border radius for left and right eye
void RGBroboFace::setBorderradius(int leftEye, int rightEye) {
  _eyes[LEFT_NEXT].radius = leftEye;
  _eyes[RIGHT_NEXT].radius = rightEye;
  _eyes[DEFAULT].radius = leftEye;

  if (!setup_complete) {
    _eyes[LEFT].radius = leftEye;
    _eyes[RIGHT].radius = rightEye;
  }
}

// Set space between the eyes, can also be negative
void RGBroboFace::setSpacebetween(int space) {
  _eyes[LEFT_NEXT].distanceCenterX = space;
  _eyes[RIGHT_NEXT].distanceCenterX = space;
  _eyes[DEFAULT].distanceCenterX = space;

  if (!setup_complete) {
    _eyes[LEFT].distanceCenterX = space;
    _eyes[RIGHT].distanceCenterX = space;
  }
}

// Set mood expression
void RGBroboFace::setEyeExpression(unsigned char mood, unsigned char level) {
  if (level >= 0 && level <= EMOTION_MAX) {
    switch (mood) {
      case ANGRY:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = level;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
      case HAPPY:
        _emotions[LVL].happiness = level;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
      case SAD:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = level;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
      case TIRED:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = level;
        _emotions[LVL].love = 0;
        break;
      case SURPRISED:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = level;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
      case DISGUST:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = level;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
      case FEAR:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = level;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
      case LOVE:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = level;
        break;
      default:
        _emotions[LVL].happiness = 0;
        _emotions[LVL].anger = 0;
        _emotions[LVL].sadness = 0;
        _emotions[LVL].fear = 0;
        _emotions[LVL].disgust = 0;
        _emotions[LVL].surprise = 0;
        _emotions[LVL].tired = 0;
        _emotions[LVL].love = 0;
        break;
    }
  } else {
    _emotions[LVL].happiness = 0;
    _emotions[LVL].anger = 0;
    _emotions[LVL].sadness = 0;
    _emotions[LVL].fear = 0;
    _emotions[LVL].disgust = 0;
    _emotions[LVL].surprise = 0;
    _emotions[LVL].tired = 0;
    _emotions[LVL].love = 0;
  }
}

void RGBroboFace::setEyeColor(uint16_t color) {
  _eyes[LEFT].color = _eyes[RIGHT].color = color;
}

void RGBroboFace::setEyeColor(uint16_t l_color, uint16_t r_color) {
  _eyes[LEFT].color = l_color;
  _eyes[RIGHT].color = r_color;
}

void RGBroboFace::setBGColor(uint16_t color) {
  _bgColor = color;
}

void RGBroboFace::setCuriosity(bool curiousBit, int interval, int variation, uint16_t radius) {
  idle = curiousBit;
  idleInterval = interval;
  idleIntervalVariation = variation;

  if (radius <= _screenWidth)
    idleRadius = radius;
  else
    idleRadius = 0;
}

void RGBroboFace::setCuriosity(bool curiousBit) {
  idle = curiousBit;
}

//*********************************************************************************************
//  MOUTH ADDITIONS
//*********************************************************************************************
void RGBroboFace::addMouth(bool active) {
  mouthActive = active;
}

void RGBroboFace::addMouth(bool active, int padding) {
  mouthActive = active;
  mouthPaddingNext = padding;
  mouthPaddingDefault = padding;
}

void RGBroboFace::setMouthBorderRadius(int mouth) {
  mouthRadiusNext = mouth;
  mouthRadiusDefault = mouth;
}

void RGBroboFace::setMouthWidth(int sizeX) {
  mouthWidthNext = sizeX;
  mouthWidthDefault = sizeX;
}

void RGBroboFace::setMouthHeight(int sizeY) {
  mouthHeightNext = sizeY;
  mouthHeightDefault = sizeY;
}

void RGBroboFace::setMouthPadding(int padding) {
  mouthPaddingNext = padding;
  mouthPaddingDefault = padding;
}

//*********************************************************************************************
//  GETTERS METHODS
//*********************************************************************************************
uint8_t RGBroboFace::getSector(unsigned int pos_x, unsigned int pos_y) {
  for (uint8_t i = 0; i < 9; i++) {
    if (pos_x >= _sectors[i].x_min && pos_x <= _sectors[i].x_max) {
      if (pos_y >= _sectors[i].y_min && pos_y <= _sectors[i].y_max) {
        return i + 1;
      }
    }
  }
}

//*********************************************************************************************
//  PRE-CALCULATIONS AND ACTUAL DRAWINGS
//*********************************************************************************************
void RGBroboFace::close(bool left, bool right) {
  if (left) {
    _eyes[LEFT_NEXT].height = 1;  // blinking left eye
    _eyes[LEFT].open = false;     // left eye not opened (=closed)
    _eyes[LEFT].blinking = true;
  }
  if (right) {
    _eyes[RIGHT_NEXT].height = 1;  // blinking right eye
    _eyes[RIGHT].open = false;     // right eye not opened (=closed)
    _eyes[RIGHT].blinking = true;
  }
}

// Open eye(s)
void RGBroboFace::open(bool left, bool right) {
  if (left) {
    _eyes[LEFT].open = true;  // left eye opened - if true, drawEyes() will take care of opening eyes again
    _eyes[LEFT].blinking = true;
  }
  if (right) {
    _eyes[RIGHT].open = true;  // right eye opened
    _eyes[RIGHT].blinking = true;
  }
}

// Trigger eyeblink(s) animation
void RGBroboFace::blink(bool left, bool right) {
  close(left, right);
  open(left, right);
}

void RGBroboFace::drawFace() {
#ifdef FACE_DEBUG
  if (_debug)
    canvasGfx->fillScreen(_FakebgColor);  // start with a blank screen
  else
#endif
    canvasGfx->fillScreen(_bgColor);  // start with a blank screen


  if (idle) {
    if (millis() >= idleAnimationTimer) {
      bool sel = random(2);
      if (sel) {
        _faceCenter_xNext = _screenWidth / 2;
        _faceCenter_yNext = _screenHeight / 2;
      } else {
        if (idleRadius > 0) {
          _faceCenter_xNext = random((_screenWidth / 2) - idleRadius, (_screenWidth / 2) + idleRadius);
          _faceCenter_yNext = random((_screenHeight / 2) - idleRadius, (_screenHeight / 2) + idleRadius);
        } else {
          uint8_t sector = random(9);
          _faceCenter_xNext = random(_sectors[sector].x_min, _sectors[sector].x_max);
          _faceCenter_yNext = random(_sectors[sector].y_min, _sectors[sector].y_max);
        }
      }
      idleAnimationTimer = millis() + (idleInterval * 1000) + (random(idleIntervalVariation) * 1000);  // calculate next time for eyes repositioning
    }
  }

  // _faceCenter_x = (_faceCenter_x + _faceCenter_xNext) / 2;
  // _faceCenter_y = (_faceCenter_y + _faceCenter_yNext) / 2;

  _faceCenter_x = (_faceCenter_x * 0.9) + (_faceCenter_xNext * 0.1);
  _faceCenter_y = (_faceCenter_y * 0.9) + (_faceCenter_yNext * 0.1);

  if (mouthActive)
    drawMouth();

  drawEyes();

  // Draw sector lines if enabled
  if (sector_lines) {
    //Draw Vertical Lines
    for (uint8_t i = 0; i <= 5; i++) {
      canvasGfx->drawLine(_sector_sizeX * i, 0, _sector_sizeX * i, _screenHeight, RGB565(242, 242, 33));
    }
    //Draw Horizontal Lines
    for (uint8_t j = 0; j <= 5; j++) {

      canvasGfx->drawLine(0, _sector_sizeY * j, _screenWidth, _sector_sizeY * j, RGB565(242, 242, 33));
    }

    if (idleRadius > 0)
      canvasGfx->drawCircle((_screenWidth / 2), (_screenHeight / 2), idleRadius, RGB565(255, 0, 255));
  }

  //Draw center point of face
  if (cross_hair) {
    canvasGfx->drawLine(_faceCenter_x - 5, _faceCenter_y, _faceCenter_x + 5, _faceCenter_y, RGB565(242, 242, 33));
    canvasGfx->drawLine(_faceCenter_x, _faceCenter_y - 5, _faceCenter_x, _faceCenter_y + 5, RGB565(242, 242, 33));
  }


  canvasGfx->flush();
}

void RGBroboFace::drawEyes() {

  // Left eye width
  _eyes[LEFT].width = (_eyes[LEFT].width + _eyes[LEFT_NEXT].width) / 2;
  _eyes[LEFT].height = (_eyes[LEFT].height + _eyes[LEFT_NEXT].height + _eyes[LEFT].heightOffset) / 2;
  _eyes[LEFT].distanceCenterX = (_eyes[LEFT].distanceCenterX + _eyes[LEFT_NEXT].distanceCenterX) / 2;
  _eyes[LEFT].distanceCenterY = (_eyes[LEFT].distanceCenterY + _eyes[LEFT_NEXT].distanceCenterY) / 2;
  _eyes[LEFT].radius = (_eyes[LEFT].radius + _eyes[LEFT_NEXT].radius) / 2;

  // Right eye width
  _eyes[RIGHT].width = (_eyes[RIGHT].width + _eyes[RIGHT_NEXT].width) / 2;
  _eyes[RIGHT].height = (_eyes[RIGHT].height + _eyes[RIGHT_NEXT].height + _eyes[RIGHT].heightOffset) / 2;
  _eyes[RIGHT].distanceCenterX = (_eyes[RIGHT].distanceCenterX + _eyes[RIGHT_NEXT].distanceCenterX) / 2;
  _eyes[RIGHT].distanceCenterY = (_eyes[RIGHT].distanceCenterY + _eyes[RIGHT_NEXT].distanceCenterY) / 2;
  _eyes[RIGHT].radius = (_eyes[RIGHT].radius + _eyes[RIGHT_NEXT].radius) / 2;


  //EYE Coordinates Default prior to modifications
  if (!mouthActive) {
    // Left eye
    _eyes[LEFT].cX = _faceCenter_x - (_eyes[LEFT].width + _eyes[LEFT].distanceCenterX);
    _eyes[LEFT].cY = _faceCenter_y - (_eyes[LEFT].height / 2);
    // Right eye
    _eyes[RIGHT].cX = _faceCenter_x + _eyes[RIGHT].distanceCenterX + 1;
    _eyes[RIGHT].cY = _faceCenter_y - (_eyes[LEFT].height / 2);
  } else {
    _eyes[LEFT].cX = _faceCenter_x - (_eyes[LEFT].width + _eyes[LEFT].distanceCenterX);
    _eyes[LEFT].cY = _faceCenter_y - _eyes[LEFT].height - _padding;
    // Right eye
    _eyes[RIGHT].cX = _faceCenter_x + (_eyes[LEFT].distanceCenterX + 1);
    _eyes[RIGHT].cY = _faceCenter_y - _eyes[LEFT].height - _padding;
  }

  // Open eyes again after closing them
  if (_eyes[LEFT].open && _eyes[LEFT].blinking) {
    if (_eyes[LEFT].height <= 1 + _eyes[LEFT].heightOffset) {
      _eyes[LEFT_NEXT].height = _eyes[DEFAULT].height;
      _eyes[LEFT].blinking = false;
    }
  }
  if (_eyes[RIGHT].open && _eyes[RIGHT].blinking) {
    if (_eyes[RIGHT].height <= 1 + _eyes[RIGHT].heightOffset) {
      _eyes[RIGHT_NEXT].height = _eyes[DEFAULT].height;
      _eyes[RIGHT].blinking = false;
    }
  }

  _emotions[VALUE].happiness = (_emotions[VALUE].happiness + _emotions[VALUE_NEXT].happiness) / 2;
  _emotions[VALUE].anger = (_emotions[VALUE].anger + _emotions[VALUE_NEXT].anger) / 2;
  _emotions[VALUE].sadness = (_emotions[VALUE].sadness + _emotions[VALUE_NEXT].sadness) / 2;
  //_emotions[VALUE].fear = (_emotions[VALUE].fear + _emotions[VALUE_NEXT].fear) / 2;
  _emotions[VALUE].disgust = (_emotions[VALUE].disgust + _emotions[VALUE_NEXT].disgust) / 2;
  //_emotions[VALUE].surprise = (_emotions[VALUE].surprise + _emotions[VALUE_NEXT].surprise) / 2;
  _emotions[VALUE].tired = (_emotions[VALUE].tired + _emotions[VALUE_NEXT].tired) / 2;

  eyelidRadius = (eyelidRadius + eyelidRadiusNext) / 2;

  if (autoblinker) {
    if (millis() >= blinktimer) {
      blink();
      blinktimer = millis() + (blinkInterval * 1000) + (random(blinkIntervalVariation) * 1000);  // calculate next time for blinking
    }
  }

  // Prepare mood type transitions
  if (_emotions[LVL].happiness > 0 || _emotions[LVL].sadness > 0) {
    eyelidRadiusNext = _eyes[DEFAULT].radius;
  } else eyelidRadiusNext = 0;

  // Prepare mood type transitions
  if (_emotions[LVL].happiness > 0) {
    _emotions[VALUE_NEXT].happiness = map(_emotions[LVL].happiness, 0, EMOTION_MAX, 0, (_eyes[DEFAULT].height / 2));
  } else
    _emotions[VALUE_NEXT].happiness = 0;

  if (_emotions[LVL].anger > 0) {
    _emotions[VALUE_NEXT].anger = map(_emotions[LVL].anger, 0, EMOTION_MAX, 0, _eyes[DEFAULT].height / 2);
  } else
    _emotions[VALUE_NEXT].anger = 0;

  if (_emotions[LVL].sadness > 0) {
    _emotions[VALUE_NEXT].sadness = map(_emotions[LVL].sadness, 0, EMOTION_MAX, 0, _eyes[DEFAULT].height / 2);
    eyelidPaddingNext = _eyes[DEFAULT].height / 3;
  } else {
    _emotions[VALUE_NEXT].sadness = 0;
    eyelidPaddingNext = 0;
  }

  if (_emotions[LVL].tired > 0) {
    _emotions[VALUE_NEXT].tired = map(_emotions[LVL].tired, 0, EMOTION_MAX, 0, (_eyes[DEFAULT].height / 2));
  } else
    _emotions[VALUE_NEXT].tired = 0;

  // Setup for surprise
  if (_emotions[LVL].surprise > 0) {
    _emotions[VALUE_NEXT].surprise = map(_emotions[LVL].surprise, 0, EMOTION_MAX, 0, 8);
    _eyes[LEFT_NEXT].width = _eyes[DEFAULT].width + (2 * _emotions[VALUE_NEXT].surprise);
    if (!_eyes[LEFT].blinking)
      _eyes[LEFT_NEXT].height = _eyes[DEFAULT].height + (2 * _emotions[VALUE_NEXT].surprise);
    if (!cyclops) {
      _eyes[RIGHT_NEXT].width = _eyes[DEFAULT].width + (2 * _emotions[VALUE_NEXT].surprise);
      if (!_eyes[RIGHT].blinking)
        _eyes[RIGHT_NEXT].height = _eyes[DEFAULT].height + (2 * _emotions[VALUE_NEXT].surprise);
    }
  } else {
    _emotions[VALUE_NEXT].surprise = 0;
    if (!_eyes[LEFT].blinking || !_eyes[RIGHT].blinking) {
      _eyes[LEFT_NEXT].width = _eyes[DEFAULT].width;
      _eyes[LEFT_NEXT].height = _eyes[DEFAULT].height;
      if (!cyclops) {
        _eyes[RIGHT_NEXT].width = _eyes[DEFAULT].width;
        _eyes[RIGHT_NEXT].height = _eyes[DEFAULT].height;
      }
    }
  }

  // Setup for fear
  // if (_emotions[LVL].fear > 0) {
  //   _emotions[VALUE_NEXT].fear = map(_emotions[LVL].fear, 0, EMOTION_MAX, 0, 60 / 2);
  //   _eyes[LEFT_NEXT].height = 0;
  //   if (!cyclops) {
  //     _eyes[RIGHT_NEXT].height = 0;
  //   }
  // } else {
  //   _emotions[VALUE_NEXT].fear = 0;
  //   // if (_emotions[LVL].fear > 0) {
  //   //   _emotions[VALUE].fear = (_emotions[VALUE].fear + _emotions[VALUE_NEXT].fear) / 2;
  //   // } else {
  //   _eyes[LEFT_NEXT].height = _eyes[DEFAULT].height;
  //   if (!cyclops) {
  //     _eyes[RIGHT_NEXT].height = _eyes[DEFAULT].height;
  //   }
  //   // }
  // }


  // Draw basic eye shapes
  canvasGfx->fillRoundRect(_eyes[LEFT].cX, _eyes[LEFT].cY, _eyes[LEFT].width, _eyes[LEFT].height, _eyes[LEFT].radius, _eyes[LEFT].color);  // left eye
  if (!cyclops) {
    canvasGfx->fillRoundRect(_eyes[RIGHT].cX, _eyes[RIGHT].cY, _eyes[RIGHT].width, _eyes[RIGHT].height, _eyes[RIGHT].radius, _eyes[RIGHT].color);  // right eye
  }

  // if (_emotions[LVL].fear > 0 && (_eyes[LEFT].height == 0 || _eyes[RIGHT].height == 0)) {
  //   _emotions[VALUE].fear = (_emotions[VALUE].fear + _emotions[VALUE_NEXT].fear) / 2;
  //   int theta2 = (90 - _emotions[VALUE].fear);
  //   int yd = (_eyes[LEFT].width * sin(_emotions[VALUE].fear * DEG_TO_RAD)) / sin(theta2 * DEG_TO_RAD);
  //   // Serial.printf("Fear Theta: %d   Theta2: %d   Yd: %d\t", _emotions[VALUE].fear, theta2, yd);
  //   // Serial.printf("Cx: %d   Cy: %d\t", _eyes[LEFT].cX, _eyes[LEFT].cY);
  //   // Serial.printf("X0: %d   Y0: %d   X1: %d   Y1: %d\n", _eyes[LEFT].cX, _eyes[LEFT].cY + ((_eyes[DEFAULT].height / 2) - yd), _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY + (_eyes[DEFAULT].height / 2));
  //   for (uint8_t i = 0; i < 20; i++) {
  //     canvasGfx->drawLine(_eyes[LEFT].cX - i, _eyes[LEFT].cY - yd, _eyes[LEFT].cX + _eyes[LEFT].width - i, _eyes[LEFT].cY, _eyes[LEFT].color);
  //     canvasGfx->drawLine(_eyes[LEFT].cX - i, _eyes[LEFT].cY + yd, _eyes[LEFT].cX + _eyes[LEFT].width - i, _eyes[LEFT].cY, _eyes[LEFT].color);
  //     canvasGfx->drawLine(_eyes[RIGHT].cX + i, _eyes[RIGHT].cY, _eyes[RIGHT].cX + _eyes[RIGHT].width + i, _eyes[RIGHT].cY - yd, _eyes[RIGHT].color);
  //     canvasGfx->drawLine(_eyes[RIGHT].cX + i, _eyes[RIGHT].cY, _eyes[RIGHT].cX + _eyes[RIGHT].width + i, _eyes[RIGHT].cY + yd, _eyes[RIGHT].color);
  //   }
  // }


  // Draw tired top eyelids
  // eyelidsTiredHeight = (eyelidsTiredHeight + eyelidsTiredHeightNext) / 2;
  if (!_eyes[LEFT].blinking)
    canvasGfx->fillRoundRect(_eyes[LEFT].cX - 1, _eyes[LEFT].cY - 1, _eyes[LEFT].width + 2, _emotions[VALUE].tired, _eyes[LEFT].radius, _bgColor);
  if (!cyclops) {
    if (!_eyes[RIGHT].blinking)
      canvasGfx->fillRoundRect(_eyes[RIGHT].cX - 1, _eyes[RIGHT].cY - 1, _eyes[RIGHT].width + 2, _emotions[VALUE].tired, _eyes[RIGHT].radius, _bgColor);  // right eye
  }

  // Draw angry eyelids
  //eyelidsAngryHeight = (eyelidsAngryHeight + eyelidsAngryHeightNext) / 2;
  if (_emotions[LVL].anger > 0) {
    if (!cyclops) {
      if (!_eyes[LEFT].blinking)
        canvasGfx->fillTriangle(_eyes[LEFT].cX, _eyes[LEFT].cY - 1, _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY - 1, _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY + _emotions[VALUE].anger - 1, _bgColor);  // left eye
      if (!_eyes[RIGHT].blinking)
        canvasGfx->fillTriangle(_eyes[RIGHT].cX, _eyes[RIGHT].cY - 1, _eyes[RIGHT].cX + _eyes[RIGHT].width, _eyes[RIGHT].cY - 1, _eyes[RIGHT].cX, _eyes[RIGHT].cY + _emotions[VALUE].anger - 1, _bgColor);  // right eye
    } else {
      // Cyclops angry eyelids
      canvasGfx->fillTriangle(_eyes[LEFT].cX, _eyes[LEFT].cY - 1, _eyes[LEFT].cX + (_eyes[LEFT].width / 2), _eyes[LEFT].cY - 1, _eyes[LEFT].cX + (_eyes[LEFT].width / 2), _eyes[LEFT].cY + _emotions[VALUE].anger - 1, _bgColor);                      // left eyelid half
      canvasGfx->fillTriangle(_eyes[LEFT].cX + (_eyes[LEFT].width / 2), _eyes[LEFT].cY - 1, _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY - 1, _eyes[LEFT].cX + (_eyes[LEFT].width / 2), _eyes[LEFT].cY + _emotions[VALUE].anger - 1, _bgColor);  // right eyelid half
    }
  }

  // Draw sad eyelids
  //eyelidsSadHeight = (eyelidsSadHeight + eyelidsSadHeightNext) / 2;
  eyelidPadding = (eyelidPadding + eyelidPaddingNext) / 2;
  if (_emotions[LVL].sadness > 0) {
    if (!cyclops) {
      if (!_eyes[LEFT].blinking) {
        canvasGfx->fillRoundRect(_eyes[LEFT].cX, _eyes[LEFT].cY - 1, _eyes[LEFT].width + 2, eyelidPadding, 0, _bgColor);
        canvasGfx->fillTriangle(_eyes[LEFT].cX, _eyes[LEFT].cY + eyelidPadding - 1, _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY + eyelidPadding - 1, _eyes[LEFT].cX, _eyes[LEFT].cY + _emotions[VALUE].sadness - 1, _bgColor);
      }
      if (!_eyes[RIGHT].blinking) {
        canvasGfx->fillRoundRect(_eyes[RIGHT].cX, _eyes[RIGHT].cY - 1, _eyes[RIGHT].width + 2, eyelidPadding, 0, _bgColor);                                                                                                                                        // left eye
        canvasGfx->fillTriangle(_eyes[RIGHT].cX, _eyes[RIGHT].cY + eyelidPadding - 1, _eyes[RIGHT].cX + _eyes[RIGHT].width, _eyes[RIGHT].cY + eyelidPadding - 1, _eyes[RIGHT].cX + _eyes[RIGHT].width, _eyes[RIGHT].cY + _emotions[VALUE].sadness - 1, _bgColor);  // right eye
      }
    } else {
      canvasGfx->fillTriangle(_eyes[LEFT].cX, _eyes[LEFT].cY - 1, _eyes[LEFT].cX + (_eyes[LEFT].width / 2), _eyes[LEFT].cY - 1, _eyes[LEFT].cX, _eyes[LEFT].cY + _emotions[VALUE].sadness - 1, _bgColor);                                          // left eyelid half
      canvasGfx->fillTriangle(_eyes[LEFT].cX + (_eyes[LEFT].width / 2), _eyes[LEFT].cY - 1, _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY - 1, _eyes[LEFT].cX + _eyes[LEFT].width, _eyes[LEFT].cY + _emotions[VALUE].sadness - 1, _bgColor);  // right eyelid half
    }
  }

  // Draw happy  eyelids
  //canvasGfx->fillRoundRect(_eyes[LEFT].cX - 1, _eyes[LEFT].cY, _eyes[LEFT].width + 2, _eyes[LEFT].eyeLidTop, eyelidRadius, _bgColor);
  if (!_eyes[LEFT].blinking)                                                                                                                                                                      // left eye
    canvasGfx->fillRoundRect(_eyes[LEFT].cX - 1, _eyes[LEFT].cY + (_eyes[LEFT].height - _emotions[VALUE].happiness), _eyes[LEFT].width + 2, _emotions[VALUE].happiness, eyelidRadius, _bgColor);  // left eye
  if (!cyclops) {
    //canvasGfx->fillRoundRect(_eyes[RIGHT].cX - 1, _eyes[RIGHT].cY, _eyes[RIGHT].width + 2, _eyes[RIGHT].eyeLidTop, eyelidRadius, _bgColor);  // right eye
    if (!_eyes[RIGHT].blinking)
      canvasGfx->fillRoundRect(_eyes[RIGHT].cX - 1, _eyes[RIGHT].cY + (_eyes[RIGHT].height - _emotions[VALUE].happiness), _eyes[RIGHT].width + 2, _emotions[VALUE].happiness, eyelidRadius, _bgColor);
  }
}


void RGBroboFace::setMouthExpression(unsigned char mood) {
  switch (mood) {
    case HAPPY:
      mouth_happy = 1;
      mouth_sad = 0;
      mouth_surprised = 0;
      break;
    case SAD:
      mouth_happy = 0;
      mouth_sad = 1;
      mouth_surprised = 0;
      break;
    case SURPRISED:
      mouth_happy = 0;
      mouth_sad = 0;
      mouth_surprised = 1;
      break;
    default:
      mouth_happy = 0;
      mouth_sad = 0;
      mouth_surprised = 0;
      break;
  }
}

void RGBroboFace::drawMouth() {

  mouthLx = _faceCenter_x;
  mouthLy = _faceCenter_y + mouthPaddingDefault;

  mouthWidth = (mouthWidth + mouthWidthNext) / 2;
  mouthHeight = (mouthHeight + mouthHeightNext) / 2;
  mouthRadius = (mouthRadius + mouthRadiusNext) / 2;
  mouthPadding = (mouthPadding + mouthPaddingNext) / 2;

  // mouth_sad = 0;
  // mouth_surprised = 0;

  if (mouth_surprised) {
    surprisedPaddingNext = 8;
    mouthRadiusNext = mouthRadiusDefault + 10;
  } else {
    surprisedPaddingNext = 0;
    mouthRadiusNext = mouthRadiusDefault;
  }

  if (mouth_happy) {
    topMouthRadius = 0;
    topMouthLipNext = mouthHeight / 2;
  } else {
    topMouthLipNext = 0;
    topMouthRadius = mouthRadiusDefault;
  }

  if (mouth_sad) {
    bottomMouthLipNext = ((4 * mouthHeight) / 5) + 2;
  } else {
    bottomMouthLipNext = 0;
  }

  if (!mouth_happy && !mouth_surprised && !mouth_sad) {
    topMouthLipNext = (4 * mouthHeight) / 5;
  }


  surprisedPadding = (surprisedPadding + surprisedPaddingNext) / 2;
  topMouthLip = (topMouthLip + topMouthLipNext) / 2;
  bottomMouthLip = (bottomMouthLip + bottomMouthLipNext) / 2;

  // Default mouth
  canvasGfx->fillRoundRect(mouthLx - (mouthWidth / 2), mouthLy - surprisedPaddingNext, mouthWidth, mouthHeight + (2 * surprisedPaddingNext), mouthRadius, _mouthColor);

  // Top Expression
  canvasGfx->fillRoundRect(mouthLx - (mouthWidth / 2) - 1, mouthLy, mouthWidth + 2, topMouthLip, topMouthRadius, _bgColor);
  // Bottom Expression
  canvasGfx->fillRoundRect(mouthLx - (mouthWidth / 2) - 1, mouthLy + (mouthHeight / 5), mouthWidth + 2, bottomMouthLip, topMouthRadius, _bgColor);
}


//*********************************************************************************************
//  ADDITIONAL TEST FUNCTIONS
//*********************************************************************************************
void RGBroboFace::setCrosshair(bool active) {
  cross_hair = active;
}

void RGBroboFace::setSectorLines(bool active) {
  sector_lines = active;
}

void RGBroboFace::setFaceExpression(uint8_t mood) {
  setEyeExpression(mood);
  setMouthExpression(mood);
}

void RGBroboFace::debug() {
#ifdef FACE_DEBUG
  setSectorLines(ON);
  setCrosshair(ON);
#endif
}

//*********************************************************************************************
//  TOUCH FUNCTIONS
//*********************************************************************************************
