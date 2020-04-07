#include "SSVXYMatrixText.h"

//
// By Serge Skorodinsky, 03/21/2020
//

boolean UTF8Itterator(String* S, uint8_t &index, uint16_t &UTF8)
{
	//Scan for valid english (one byte) or russian (two bytes) letter in UTF8 String S, starting from index.
	//If found - return TRUE, found letter returned in UTF8, post-foind index returtned in index.
	//Optimized to use in while-loop.
	if (!(S)) return false; //S not initialized
	if (index >= S->length()) return false; //no more characters
	UTF8 = S->charAt(index); 
	if (UTF8 <= 0x7F) 
	  {//english letters
	  index++; 
	  return true;
	  } 
	if ((UTF8 == 0xD0) || (UTF8 == 0xD1))
	  {//two bytes, russian letters
	  index++;
	  if (index >= S->length()) return false; //no more characters
	  UTF8 = UTF8<<8;
	  UTF8 = UTF8 + S->charAt(index); 
	  index++;
	  return true;
	  }
	return false; //two or more bytes, not russian
}


//XYMatrixText Class
XYMatrixText::XYMatrixText(XYMatrix* MatrixCanvas)
{
  _matrixCanvas = MatrixCanvas;
  _s = NULL;
  _charSpace = 2;
  _left_right_swap = false; //is really needed?
  _top_bottom_swap = false; //is really needed?
  _font_char_width = 5;    //char width, depends on font
  _font_char_height = 8;   //char height, depends on font
}

void XYMatrixText::setString(String* S)
{
  _s = S;
}

uint16_t XYMatrixText::getStringWidthPx()
{
if (!(_s)) return 0;
if (_s->length() == 0) return 0;
uint8_t realUTF8length=0; //UTF8 letter counter
uint8_t index=0; //passed by ref., not used here but required by itterator
uint16_t UTF8=0; //passed by ref., not used here but required by itterator	
while ( UTF8Itterator(_s, index, UTF8) ) { realUTF8length++; }
return (realUTF8length * (_font_char_width +_charSpace) -_charSpace);
}

uint16_t XYMatrixText::getStringHeightPx()
{
return _font_char_height;
}

void XYMatrixText::drawString(String* S, int16_t offsetX, int16_t offsetY, CRGB letterColor) //drawString() with String parameter
{
  setString(S);
  drawString(offsetX, offsetY, letterColor);
}

//positioning, offsets. 
//To use these functions BEFORE draw the string - you MUST to use setString(String* S).
//If it is not important - you can use drawString(String* S, ...) function
boolean XYMatrixText::IsStringWidthFit()
{
  return (_matrixCanvas->getMatrixWidth() >= getStringWidthPx());
}

boolean XYMatrixText::IsStringHeightFit()
{
  return (_matrixCanvas->getMatrixHeight() >= getStringHeightPx());
}

//positioning "just hide"
int16_t XYMatrixText::OffsetX_RightStr_To_LeftArea() //offsetX when right text edge at the left edge of area, text is NOT visible 
{
  return  -getStringWidthPx(); //always negative. -(length*charwidth)+((length-1)*charspace)
}

int16_t XYMatrixText::OffsetX_LeftStr_To_RightArea() //offsetX when left text edge at the right edge of area, text is NOT visible 
{
  return _matrixCanvas->getMatrixWidth();
}

int16_t XYMatrixText::OffsetY_TopStr_To_BottomArea() //offsetY when top text edge at the bottom edge of the area, text is NOT visible
{
  return -getStringHeightPx(); //always negative
}

int16_t XYMatrixText::OffsetY_BottomStr_To_TopArea() //offsetY when top text edge at the bottom edge of the area, text is NOT visible
{
  return _matrixCanvas->getMatrixHeight();
}

//positioning "max visibility" 
int16_t XYMatrixText::OffsetX_LeftStr_To_LeftArea() //offsetX when left text edge at the left edge of area, text is max visible 
{
  return 0;
}

int16_t XYMatrixText::OffsetX_RightStr_To_RightArea() //offsetX when right text edge at the right edge of area, text is max visible 
{
  int16_t res = _matrixCanvas->getMatrixWidth() - getStringWidthPx(); //positive, if matrix wider than string
  return res;
}

int16_t XYMatrixText::OffsetY_BottomStr_To_BottomArea() //offsetY when bottom text edge at the bottom edge of area, text is max visible 
{
  int16_t res = _matrixCanvas->getMatrixHeight() - getStringHeightPx() + 1; //negative, if string is higher than matrix
  if (res>0) res=0; //.. or zero
  return res;
}

int16_t XYMatrixText::OffsetY_TopStr_To_TopArea() //offsetY when top text edge at the top edge of area, text is max visible 
{
  int16_t res = _matrixCanvas->getMatrixHeight() - getStringHeightPx(); //positive, if matrix higher than string
  if (res<0) res=0;  //.. or zero
  return res;
}

//positioning "center", if string is bigger (wider or higher) - beginning and ending of string will be out of matrix
int16_t XYMatrixText::OffsetX_Center() //center offsetX when ...
{
  return (_matrixCanvas->getMatrixWidth() - getStringWidthPx()) / 2;
}

int16_t XYMatrixText::OffsetY_Center() //center offsetY when ...
{
  return (_matrixCanvas->getMatrixHeight() - getStringHeightPx()) / 2;
}

//RUSSIAN UTF8
uint8_t XYMatrixText::getLetterColumn(uint16_t UTF8, uint8_t column) //font column 0..4
{
  //check out-of-limits conditions for both parameters!
  uint8_t index; 
  switch(UTF8)
   {
    case 0 ... 31: //control bytes, replaced with <space>
	  index = 0;    
	  break; 

    case 32 ... 127: //english letters ans signes ' '...'⌂', index 0...95
	  index = UTF8-32;    
	  break; 
	  
	case 0xD090 ... 0xD0BF: //russian letetrs А...п, index 96(0x60)..143(0x8F)
	  index = UTF8-0xD030; 
	  break;
	  
    case 0xD180 ... 0xD18F: //russian letetrs р...я, index 144(0x90)..159(9F)
	  index = UTF8-0xD0F0; 
	  break;
	  
	default: 
      //Serial.printf(" UTF8: %d (%xHex)\n", UTF8, UTF8);   
	  return 0x55;  //to indicate error
	  break; 
   }
return pgm_read_byte(&(fontHEX[index] [column])); 	
}

void XYMatrixText::drawLetter(uint16_t UTF8, int16_t offsetX, int16_t offsetY, CRGB letterColor)
{
  int8_t start_pos; 
  int8_t finish_pos;
  if (offsetX < -_font_char_width || offsetX > _matrixCanvas->getMatrixWidth()) return; //out of area, nothing to draw
  
  if (offsetX < 0) 
	    start_pos = -offsetX; 
    else start_pos=0;
	
  if (offsetX > (_matrixCanvas->getMatrixWidth()-_font_char_width)) 
	     finish_pos=_matrixCanvas->getMatrixWidth()-offsetX; 
    else finish_pos=_font_char_width;

  for (byte x=start_pos; x<finish_pos; x++) 
  {
    int thisByte;
    if (_left_right_swap) thisByte = getLetterColumn(UTF8, _font_char_width-1-x);
                     else thisByte = getLetterColumn(UTF8, x);

    for (byte y=0; y<_font_char_height; y++) 
	{
      boolean thisBit;
      if (_top_bottom_swap) thisBit = thisByte & (1<<y);
                       else thisBit = thisByte & (1<<(_font_char_height-1-y));
      //draw row
        if (thisBit) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, letterColor);
              //else setPixelColor(...) ;// background
    }
  }
}

void XYMatrixText::drawString(int16_t offsetX, int16_t offsetY, CRGB letterColor) //String must be specify in advance by using
{
uint8_t i=0; //UTF8 letter counter
uint8_t index=0; //passed by ref.
uint16_t UTF8=0; //passed by ref.	
while ( UTF8Itterator(_s, index, UTF8) )	
  {
  drawLetter(UTF8, offsetX + i*(_font_char_width +_charSpace), offsetY, letterColor);
  i++;
  }
}

 //End of XYMatrixText Class
