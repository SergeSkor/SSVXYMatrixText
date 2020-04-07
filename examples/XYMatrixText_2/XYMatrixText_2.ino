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

String Str = String("Hello World! Привет всем..."); //english -russian



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

NewSerialStringProcess();

}


String Stmp = "";         // a String to hold incoming data

void NewSerialStringProcess()
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the Stmp:
    if ((inChar != '\r') && (inChar != '\n') ) 
      Stmp += inChar;
      else
      {
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (Stmp.length() > 0) 
          {
          Serial.printf("\r    --> Str \"%s\" replaced with \"%s\"\n", Str.c_str(), Stmp.c_str() );
          Str = String(Stmp);
          Stmp = String("");
          }  
      }
  }
}