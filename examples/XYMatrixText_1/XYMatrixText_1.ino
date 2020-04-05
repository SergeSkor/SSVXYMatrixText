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

String matrixStr = String("Test String");

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
  FastLED.clear();
  //fill_solid(leds, NUM_LEDS, CRGB::DarkGreen); //background
  matrix.draw_2DGradient(CRGB::Yellow, CRGB::DarkRed, CRGB::DarkGreen, CRGB::DarkBlue ); //background
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  Serial.printf("getStringWidthPx: %d\n", matrixText.getStringWidthPx());
  Serial.printf("getStringHeightPx: %d\n", matrixText.getStringHeightPx());
  matrixText.drawString(2, 2, CRGB::Red);
  FastLED.show();
  WaitForSerial("Text with background at [2,4]");
  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());  Serial.printf("OffsetX_RightStr_To_LeftArea: %d\n", matrixText.OffsetX_RightStr_To_LeftArea());
  Serial.printf("OffsetY_TopStr_To_BottomArea: %d\n", matrixText.OffsetY_TopStr_To_BottomArea());
  matrixText.drawString(matrixText.OffsetX_RightStr_To_LeftArea()+4, matrixText.OffsetY_TopStr_To_BottomArea()+4, CRGB::Red);
  FastLED.show();
  WaitForSerial("Text almost run out to left-bottom corner, only 4x4 points of right-top corner of Text is still visible");
  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  Serial.printf("OffsetX_LeftStr_To_RightArea: %d\n", matrixText.OffsetX_LeftStr_To_RightArea());
  Serial.printf("OffsetY_BottomStr_To_TopArea: %d\n", matrixText.OffsetY_BottomStr_To_TopArea());
  matrixText.drawString(matrixText.OffsetX_LeftStr_To_RightArea()-4, matrixText.OffsetY_BottomStr_To_TopArea()-4, CRGB::Red);
  FastLED.show();
  WaitForSerial("Text almost run out to right-top corner, only 4x4 points of left-bottom corner of Text is still visible");
  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  Serial.printf("OffsetX_LeftStr_To_LeftArea: %d\n", matrixText.OffsetX_LeftStr_To_LeftArea());
  Serial.printf("OffsetY_BottomStr_To_BottomArea: %d\n", matrixText.OffsetY_BottomStr_To_BottomArea());
  matrixText.drawString(matrixText.OffsetX_LeftStr_To_LeftArea(), matrixText.OffsetY_BottomStr_To_BottomArea(), CRGB::Red);
  FastLED.show();
  WaitForSerial("Text stick to Left-Bottom corner of Matrix");
  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  Serial.printf("OffsetX_RightStr_To_RightArea: %d\n", matrixText.OffsetX_RightStr_To_RightArea());
  Serial.printf("OffsetY_TopStr_To_TopArea: %d\n", matrixText.OffsetY_TopStr_To_TopArea());
  matrixText.drawString(matrixText.OffsetX_RightStr_To_RightArea(), matrixText.OffsetY_TopStr_To_TopArea(), CRGB::Red);
  FastLED.show();
  WaitForSerial("Text stick to Right-Top corner of Matrix");
  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  Serial.printf("OffsetX_Center: %d\n", matrixText.OffsetX_Center());
  Serial.printf("OffsetY_Center: %d\n", matrixText.OffsetY_Center());
  if (matrixText.IsStringWidthFit()) {Serial.printf("Width FIT\n");} else {Serial.printf("Width NOT FIT\n");}
  if (matrixText.IsStringHeightFit()) {Serial.printf("Height FIT\n");} else {Serial.printf("Height NOT FIT\n");}
  matrixText.drawString(matrixText.OffsetX_Center(), matrixText.OffsetY_Center(), CRGB::Red);
  FastLED.show();
  WaitForSerial("Text is Centered on Matrix. Only center part of text is visible if Text is too wide/tall.");
  //

  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  OscilateXPosition(  matrixText.OffsetX_RightStr_To_LeftArea(), //X1
                      matrixText.OffsetX_LeftStr_To_RightArea(), //X2
                                                              2, //HowManyTimes
                                                              50 //Delay,mS
                    );
  WaitForSerial("Oscilates Text Left-Right 4 times, moving it out of Area");

  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  OscilateYPosition(  matrixText.OffsetY_TopStr_To_BottomArea(), //Y1
                      matrixText.OffsetY_BottomStr_To_TopArea(), //Y2
                                                              2, //HowManyTimes
                                                              50 //Delay,mS
                    );
  WaitForSerial("Oscilates Text Up-Down 4 times, moving it out of Area");

  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  OscilateXPosition(  matrixText.OffsetX_RightStr_To_RightArea(), //X1
                      matrixText.OffsetX_LeftStr_To_LeftArea(), //X2
                                                             2, //HowManyTimes
                                                             50 //Delay,mS
                    );
  WaitForSerial("Oscilates Text Left-Right 4 times, NOT moving it out of Area");
  
  //
  FastLED.clear();
  matrixText.setString(&matrixStr);
  Serial.printf("Matrix Str: \"%s\"\n", matrixStr.c_str());
  OscilateYPosition(  matrixText.OffsetY_BottomStr_To_BottomArea(), //Y1
                      matrixText.OffsetY_TopStr_To_TopArea(),       //Y2
                                                                 2, //HowManyTimes
                                                                 50 //Delay,mS
                    );
  WaitForSerial("Oscilates Text Up-Down 4 times, NOT moving it out of Area");


  WaitForSerial("end of loop() ");
}


void OscilateXPosition(int16_t X1, int16_t X2, byte HowManyTimes, int16_t StepDelay_mS)
{
if (X1 == X2) return;
int16_t Xmin = min(X1, X2);
int16_t Xmax = max(X1, X2);
for (byte n = 0; n < HowManyTimes; n++)
  {
    //from max to min
    for (int16_t i=Xmax; i>=Xmin; i--)
    {
    FastLED.clear ();
    matrixText.drawString(&matrixStr, i, 4, CRGB::Blue);
    delay(StepDelay_mS);
    FastLED.show();
    }
    //from min to max
    for (int16_t i=Xmin; i<=Xmax; i++)
    {
    FastLED.clear ();
    matrixText.drawString(&matrixStr, i, 4, CRGB::Blue);
    delay(StepDelay_mS);
    FastLED.show();
    }
  }
}

void OscilateYPosition(int16_t Y1, int16_t Y2, byte HowManyTimes, int16_t StepDelay_mS)
{
if (Y1 == Y2) return;
int16_t Ymin = min(Y1, Y2);
int16_t Ymax = max(Y1, Y2);
for (byte n = 0; n < HowManyTimes; n++)
  {
    //from max to min
    for (int16_t i=Ymax; i>=Ymin; i--)
    {
    FastLED.clear ();
    matrixText.drawString(&matrixStr, 2, i, CRGB::Blue);
    delay(StepDelay_mS);
    FastLED.show();
    }
    //from min to max
    for (int16_t i=Ymin; i<=Ymax; i++)
    {
    FastLED.clear ();
    matrixText.drawString(&matrixStr, 2, i, CRGB::Blue);
    delay(StepDelay_mS);
    FastLED.show();
    }
  }
}


void WaitForSerial(const char *serial_message)
{
  char endMarker = '\n';
  char rc;
  String Stmp = String("");

Serial.println();
Serial.println(serial_message);
Serial.println("\r    --> Type New String <Enter> to replace MatrixStr or just <Enter> to continue...\n");


if (Serial.available()) {rc = Serial.read();} else {rc = (char) 0xff;}
while (rc != endMarker)
  {
    if ( (rc!=char(0xff)) && (rc!=endMarker) )
      {
      Stmp += rc;
      }
    if (Serial.available()) {rc = Serial.read();} else {rc = (char) 0xff;}
  }
if (Stmp.length() > 0) 
  {
    Serial.printf("\r    --> Matrix Str \"%s\" replaced with \"%s\"\n", matrixStr.c_str(), Stmp.c_str() );
    matrixStr = String(Stmp);
  }  
  
//Serial.println(matrixStr);
}