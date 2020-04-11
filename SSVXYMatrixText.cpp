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
   //background
  _UpdateBackground = true;
  _BackgroundColor = CRGB(10, 10, 10); //almost black
  
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
uint8_t XYMatrixText::getFontIndex(uint16_t UTF8) //returns index in font
{
  switch(UTF8)
   {
    case 0 ... 31: //control bytes, replaced with <space>
	  return 0;    
	  break; 

    case 32 ... 127: //english letters ans signes ' '...'⌂', index 0...95
	  return UTF8-32;    
	  break; 
	  
	case 0xD090 ... 0xD0BF: //russian letetrs А...п, index 96(0x60)..143(0x8F)
	  return UTF8-0xD030; 
	  break;
	  
    case 0xD180 ... 0xD18F: //russian letetrs р...я, index 144(0x90)..159(9F)
	  return UTF8-0xD0F0; 
	  break;
	  
	default: 
	  return 0xFF;  //to indicate error
	  break; 
   }
}

uint8_t XYMatrixText::getLetterColumn(uint16_t UTF8, uint8_t column) //font column 0..4, from left to right
{
  //check out-of-limits conditions for both parameters?
  if (column >= _font_char_width) return 0xAA;  //to indicate error
  uint8_t index = getFontIndex(UTF8);
  if (index == 0xFF) return 0x55; //to indicate error
  return pgm_read_byte(&(fontHEX[index] [column])); 	
}

/*
//not used yet
uint8_t XYMatrixText::getLetterRow(uint16_t UTF8, uint8_t row) //font row 0..7, from bottom to top, returns 5 significant bits
{
  if (row >= _font_char_height) return 0x0A;  //to indicate error, 5 bits
  uint8_t index = getFontIndex(UTF8);
  if (index == 0xFF) return 0x15; //to indicate error, 5 bits
  uint8_t rowMask = (1<<row);
  uint8_t res=0;
  for (uint8_t i=0; i<_font_char_width; i++) 
	{
	uint8_t b = pgm_read_byte(&(fontHEX[index] [i])); 	//read byte from font
	res = (res<<1); //shift left
	if ((b & rowMask) != 0)  {res |= 0x01;} // lowest bit to "1"
	}
  return res;
}
*/

void XYMatrixText::drawLetter(uint16_t UTF8, int16_t offsetX, int16_t offsetY, CRGB letterColor)
{
  int8_t XMinPos; 
  int8_t XMaxPos;
  if (offsetX < -_font_char_width || offsetX > _matrixCanvas->getMatrixWidth()) return; //out of area, nothing to draw
  
  if (offsetX < 0) 
	     XMinPos = -offsetX; 
    else XMinPos = 0;
	
  if (offsetX > (_matrixCanvas->getMatrixWidth()-_font_char_width)) 
	     XMaxPos = _matrixCanvas->getMatrixWidth()-offsetX; 
    else XMaxPos = _font_char_width;

  for (byte x = XMinPos; x < XMaxPos; x++) 
  {
    int thisByte;
    if (_left_right_swap) thisByte = getLetterColumn(UTF8, _font_char_width-1-x);
                     else thisByte = getLetterColumn(UTF8, x);

    for (byte y = 0; y < _font_char_height; y++) 
	{
      boolean thisBit;
      if (_top_bottom_swap) thisBit = thisByte & (1<<y);
                       else thisBit = thisByte & (1<<(_font_char_height-1-y));
      //draw Column
        if (thisBit) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, letterColor);
          else 	
			if (_UpdateBackground) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, _BackgroundColor); // background
    }
  }
}

void XYMatrixText::drawString(int16_t offsetX, int16_t offsetY, CRGB letterColor) //String must be specify in advance by using
{
  drawString_LeftToRight(offsetX, offsetY, letterColor);
}

void XYMatrixText::drawString_LeftToRight(int16_t offsetX, int16_t offsetY, CRGB letterColor)
{
uint8_t cnt=0; //UTF8 letter counter
uint8_t index=0; //passed by ref.
uint16_t UTF8=0; //passed by ref.	
while ( UTF8Itterator(_s, index, UTF8) )	
  {
  int16_t xpos = offsetX + cnt*(_font_char_width +_charSpace);
  if (xpos > _matrixCanvas->getMatrixWidth()) break; //gets out of range, already too right, never go back
  drawLetter(UTF8, xpos, offsetY, letterColor);
  if ( (_UpdateBackground) && (_charSpace>0) ) 
    {
	if (cnt>0)
	  {
		int16_t x0 = xpos -_charSpace;
		int16_t x1 = xpos - 1;
		int16_t y0 = offsetY; 
		int16_t y1 = offsetY+_font_char_height;
		
		for (int8_t x = x0; x <= x1; x++)
		  {
			//if ( (x<0) || (x>_matrixCanvas->getMatrixWidth()) ) continue;  //skip if not visible
		    _matrixCanvas->draw_line(x, y0, x, y1, _BackgroundColor, false);
		  }
	  }
	}
  cnt++;
  }
}

 //End of XYMatrixText Class
