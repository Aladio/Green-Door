
#include <Adafruit_NeoPixel.h>
#include <elapsedMillis.h>
//#include <Metro.h>
#include <AltSoftSerial.h>    // Arduino build environment requires this
#include <wavTrigger.h>
#include <Bounce2.h>
//-----------------------------------------------------------------------------------
// WAVTrigger
#define LED 13 // our LED
#define trigPin 8
wavTrigger wTrig;             // Our WAV Trigger object
//Metro gLedMetro(500);         // LED blink interval timer
//Metro gWTrigMetro(6000);      // WAV Trigger state machine interval timer

byte gLedState = 0;           // LED State
//int  gWTrigState = 0;         // WAV Trigger state
//int  gRateOffset = 0;         // WAV Trigger sample-rate offset

//-----------------------------------------------------------------------------------
// Sliding Switch
#define SWITCH_PIN 2
bool doorOpen = false;  // Keep track of door state
// Digital IO pin connected to the door switch.  This will be
// driven with a pull-down resistor so the door switch should
// pull the pin to ground LOW when door is opened.  Then go back to HIGH when the
// door is closed.

//-----------------------------------------------------------------------------------
// NeoPixels
#define PIXEL_PIN    6    // Digital IO pin connected to the NeoPixels.
#define PIXEL_COUNT 60    // The pixels has 60 pixels


// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Set up variables cooresponding to pixel placement.
int rFloor1 = 2;
int rFloor2 = 6;
int lFloor1 = 7;
int lFloor2 = 11;
int mirror1 = 12;
int mirror2 = 16;
int lWall1 = 17;
int lWall2 = 21;
int rrWall1 = 22;
int rrWall2 = 25;
int rWall1 = 26;
int rWall2 = 30;
int ceiling1 = 31;
int ceiling2 = 49;
int back1 = 50;
int back2 = 59;

//-----------------------------------------------------------------------------------
// Instantiate a timer object
elapsedMillis timeElapsed;
// Set up variables for time intervals
float tickTock  =  3500;
float verse1 =    19000;
float chorus1 =   38500;
float verse2 =    56000;
float chorus2 =   75000;
float piano =     97000;
float verse3 =   120800;
float offEnd =   120800;

//-----------------------------------------------------------------------------------
// Instantiate a Bounce object
Bounce debouncer = Bounce();

//-----------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);   // Start serial communication for debugging
  pinMode(SWITCH_PIN, INPUT);  //  Set up pin 2 the door pin

  // After setting up the button, setup the Bounce instance :
  debouncer.attach(SWITCH_PIN);
  debouncer.interval(20); // interval in ms

  pixels.begin();  //  Initialize the pixels object
  pixels.show();   // Initialize all pixels to 'off'

  // Initialize the LED pin
  pinMode(LED, OUTPUT);
  digitalWrite(LED, gLedState);
  elapsedMillis timeElapsed;

  // WAV Trigger startup at 57600
  wTrig.start();

  // If the Uno is powering the WAV Trigger, we should wait for the WAV Trigger
  //  to finish reset before trying to send commands.
  delay(1000);

  // If we're not powering the WAV Trigger, send a stop-all command in case it
  //  was already playing tracks. If we are powering the WAV Trigger, it doesn't
  //  hurt to do this.
  wTrig.stopAllTracks();
}

//-----------------------------------------------------------------------------------
void loop() {
  // Update the Bounce instance :
  debouncer.update();

  // Get the updated value :
  int value = debouncer.read();
  Serial.println(value);
  // Turn on or off the show as determined by the state :
  if ( value == LOW && doorOpen == false ) {
    startShow();
  }
  if (value == HIGH && doorOpen == true) {
    wTrig.trackStop(4);
    delay(50);
    wTrig.trackStop(4);
    delay(50);
    for (int i = 0; i < PIXEL_COUNT; i++) {
      pixels.setPixelColor(i, 0, 0, 0);
    }
    pixels.show();
    doorOpen = false;
    timeElapsed = 0;
    software_Reset();
  }
}

//-----Functions---------------------------------------------------------------------
//------ Reset ---------------------------------------------------------------------
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
{
asm volatile ("  jmp 0");  
} 
//-----Start Show -------------------------------------------------------------------

void startShow() {
  doorOpen = true;
  ceilingBack(40, 40, 40);
  timeElapsed = 0;
  playWav();
  delay(50);
  lightShow();
}
//---------------Play Wav File ------------------------------------------------------------
void playWav() {
  Serial.print("Door Open - ");
  Serial.println(doorOpen);
  wTrig.masterGain(0);
  wTrig.trackPlayPoly(4);
}
//--------------Light Show -------------------------------------------------------------------
void lightShow() {
  while (timeElapsed < tickTock) {
    Serial.print("tickTock-");
    Serial.println(timeElapsed);
    pixelSingleColorFadeInOut(rFloor1, rWall2, 150 , 0 , 0);
  }
  while (timeElapsed > tickTock && timeElapsed < verse1) {
    Serial.print("verse1-");
    Serial.println(timeElapsed);
    pixelSingleColorFadeInOut(rFloor1, rWall2, 0, 150, 0);
    //theaterChase(pixels.Color(127,   0,   0), 50);
    //pixelScan(rFloor1, rWall2, 255, 0, 0);
  }
    while (timeElapsed > verse1 && timeElapsed < chorus1) {
      Serial.print("chorus1-");
      Serial.println(timeElapsed);
     // pixelChase(rFloor1, rWall2, 255, 0, 0);
     //theaterChaseRainbow(30);
     pixelScan(rFloor1, rWall2, 150, 0, 0);
    }
    while (timeElapsed > chorus1 && timeElapsed < verse2) {
      Serial.print("verse2-");
      Serial.println(timeElapsed);
      //pixelChase(rFloor1, rWall2, 0, 255, 0);
      pixelScan(rFloor1, rWall2, 0, 150, 0);
    }
    while (timeElapsed > verse2 && timeElapsed < chorus2) {
      Serial.print("chorus2-");
      Serial.println(timeElapsed);
      pixelTrail(rFloor1, rWall2);
    }
    while (timeElapsed > chorus2 && timeElapsed < piano) {
      Serial.print("piano-");
      Serial.println(timeElapsed);
      //pixelScan(rFloor1, rWall2, 150, 0, 0);
      theaterChase(pixels.Color(127,   0,   0), 50);
    }
    while (timeElapsed > piano && timeElapsed < verse3) {
      Serial.print("verse3-");
      Serial.println(timeElapsed);
      pixelBlink(rFloor1, back2, 150, 0, 0);
    }
    while (timeElapsed > verse3) {
      Serial.print("OFF-");
      Serial.println(timeElapsed);
      pixelOff(0, 60);
      delay (5000);
      timeElapsed = 0;
    }
}
//----------------------------------------------------------------------------------------------
//--- Stop Everything -------------------
void checkStop() {
  // Update the Bounce instance :
  debouncer.update();

  // Get the updated value :
  int value = debouncer.read();

  if ( value == HIGH ) {
    wTrig.trackStop(4);
    delay(50);
    wTrig.trackStop(4);
    delay(50);
    for (int i = 0; i < PIXEL_COUNT; i++) {
      pixels.setPixelColor(i, 0, 0, 0);
    }
    pixels.show();
    doorOpen = false;
    timeElapsed = 0;
    software_Reset();
  }
}

//-----------------------------------------------------------------------------------
// Cyclonic Knights
void pixelScan(int start, int end, int G, int R, int B) {

  for (int i = start; i <= end; i++) {
    checkStop();
    pixels.setPixelColor(i, G, R, B);
    pixels.show();
    delay(50);
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
  }
  for (int i = rWall2; i >= rFloor1; i--) {
    checkStop();
    pixels.setPixelColor(i, G, R, B);
    pixels.show();
    delay(50);
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
  }
}

//-----------------------------------------------------------------------------------
// Blink
void pixelBlink(int start, int end, int G, int R, int B) {
  checkStop();
  pixelOn(start, end, G, R, B);
  delay(50);
  pixelOff(start, end);
  delay(50);
}

//-----------------------------------------------------------------------------------
// -- Turn neopixels ON at one time using inputted color values ------------------
void pixelOn(int start, int end, int G, int R, int B) {
  for (int i = start; i <= end; i++) {
    checkStop();
    pixels.setPixelColor(i, G, R, B);
  }
  pixels.show();
}

//-----------------------------------------------------------------------------------
// ------- Turn OFF neopixels at once (black) ------------------
void pixelOff(int start, int end) {
  for (int i = start; i <= end; i++) {
    checkStop();
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();
}
//-----------------------------------------------------------------------------------
// Turn on the ceiling and back lights
void ceilingBack(int G, int R, int B) {
  for (int i = ceiling1; i <= back2; i++) {
    checkStop();
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(G, R, B)); // low white.

    pixels.show(); // This sends the updated pixel color to the hardware.

    //delay(delayval); // Delay for a period of time (in milliseconds).
  }
}

//----------------------------------------------------------------------------------------------
// Fade lights in and out
void pixelSingleColorFadeInOut(int start, int end, int G, int R, int B) {
  for (int i = 0; i < 256; i++) {
    for (int p = start; p <= end; p++) {
      checkStop();
      if (G > 0) {
        pixels.setPixelColor(p, i, R, B);
      } else {
        pixels.setPixelColor(p, G, i, B);
      }
    }
    pixels.show();
    delay(1);
  }

  for (int i = 255; i >= 0; i--) {
    for (int p = start; p <= end; p++) {
      checkStop();
      if (G > 0) {
        pixels.setPixelColor(p, i, R, B);
      } else {
        pixels.setPixelColor(p, G, i, B);
      }
    }
    pixels.show();
    delay(1);
  }
}

//----------------------------------------------------------------------------------------------
//--- Move fading trail around ring -------------------
void pixelTrail(int start, int end) {

  for (int i = start; i <= end; i++) {
    checkStop();
    pixels.setPixelColor(i, 255, 0, 0);
    pixels.setPixelColor(i - 1, 210, 0, 0);
    pixels.setPixelColor(i - 2, 70, 0, 8);
    pixels.setPixelColor(i - 3, 10, 0, 5);
    pixels.setPixelColor(i - 4, 1, 0, 1);
    pixels.setPixelColor(i - 5, 0, 0, 0);
    pixels.show();
    delay(50);
  }
  for (int i = start + 5; i >= end; i--) {
    checkStop();
    pixels.setPixelColor(i - 5, 255, 0, 0);
    pixels.setPixelColor(i - 4, 210, 0, 0);
    pixels.setPixelColor(i - 3, 70, 33, 0);
    pixels.setPixelColor(i - 2, 10, 5, 0);
    pixels.setPixelColor(i - 1, 1, 3, 0);
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
    delay(50);
  }
}

//----------------------------------------------------------------------------------------------
//--- Move single pixel around ring -------------------
void pixelChase(int start, int end, int G, int R, int B) {

  for (int i = start; i <= end; i++) {
    checkStop();
    pixels.setPixelColor(i, G, R, B);
    pixels.show();
    delay(50);
    pixels.setPixelColor(i, 0, 0, 0);
    pixels.show();
  }
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) {  //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, c);    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, Wheel((i + j) % 255));    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i = 0; i < pixels.numPixels(); i = i + 3) {
        pixels.setPixelColor(i + q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}




