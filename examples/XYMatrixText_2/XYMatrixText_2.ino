//
// By Serge Skorodinsky, 11/15/2019
//

#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>

#include "SSVXYMatrix.h"
#include "SSVXYMatrixText.h"

FASTLED_USING_NAMESPACE


#define LED_PIN  D8 //15 //ESP8266 Lolin ESP-12E module

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 32

// Params for width and height
const uint8_t MatrixWidth = 16;
const uint8_t MatrixHeight = 16;

#define NUM_LEDS (MatrixWidth * MatrixHeight)

CRGB leds[NUM_LEDS];

XYMatrix matrix(leds, MatrixWidth, MatrixHeight, true);
XYMatrixText matrixText(&matrix);

String Str = String("Hello World! Привет всем...");  //english -russian



void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.clear ();
  FastLED.show();
  delay(100); //for stability
  Serial.println();
  Serial.println();
} //end of setup()

void loop()
{
uint8_t index=0; //passed by ref.
uint16_t UTF8=0; //passed by ref.  
CRGB color = CHSV(random8(), 255, 255);
while ( UTF8Itterator(&Str, index, UTF8) )  
  {
  FastLED.clear();
  matrixText.drawLetter(UTF8, 4, 4, color);
  FastLED.show();
  delay(600);
  }

FastLED.clear();
FastLED.show();
delay(5000);
WaitForSerial("end of loop() ");
}


void WaitForSerial(const char *serial_message)
{
//
}