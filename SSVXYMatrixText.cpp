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
  _charSpace = 0;
  //_left_right_swap = false; //is really needed?
  //_top_bottom_swap = false; //is really needed?
  _charFlip = Flip_None;
  _charRotation = Rotate_None;
  _stringDir = Dir_LeftToRight;
  
  _font_char_width = 5;    //char width, depends on font
  _font_char_height = 8;   //char height, depends on font
   //background
  _UpdateBackground = false;
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

//make similar function, but to blend color of font top color of background.
void XYMatrixText::drawChar(uint16_t UTF8, int16_t offsetX, int16_t offsetY, CRGB letterColor)
{
switch(_charRotation)
   {
    case Rotate_None: 
	  drawChar_Vert(UTF8, offsetX, offsetY, letterColor, (_charFlip == Flip_Hor), (_charFlip == Flip_Vert) );
	  break; 
	case Rotate_CW90:
      drawChar_Hor(UTF8, offsetX, offsetY, letterColor, (_charFlip == Flip_Hor), (_charFlip == Flip_Vert) );
	  break;
    case Rotate_CCW90:
	  drawChar_Hor(UTF8, offsetX, offsetY, letterColor, (_charFlip != Flip_Hor), (_charFlip != Flip_Vert) );
	  break;
	case Rotate_180:
	  drawChar_Vert(UTF8, offsetX, offsetY, letterColor, (_charFlip != Flip_Hor), (_charFlip != Flip_Vert) );
	  break;
	default: return;  
    }
}

void XYMatrixText::drawChar_Hor(uint16_t UTF8, int16_t offsetX, int16_t offsetY, CRGB letterColor, boolean swapLeftRight, boolean swapTopBottom)
{
//  int8_t XMinPos; 
//  int8_t XMaxPos;
    int8_t YMinPos; 
    int8_t YMaxPos;
//  if (offsetX < -_font_char_width || offsetX > _matrixCanvas->getMatrixWidth()) return; //out of area, nothing to draw
    if (offsetY < -_font_char_width || offsetY > _matrixCanvas->getMatrixWidth()) return; //out of area, nothing to draw

//  if (offsetX < 0) XMinPos = -offsetX; else XMinPos = 0;
  if (offsetY < 0) YMinPos = -offsetY; else YMinPos = 0;
	
//  if (offsetX > (_matrixCanvas->getMatrixWidth()-_font_char_width)) 
//	     XMaxPos = _matrixCanvas->getMatrixWidth()-offsetX; 
//    else XMaxPos = _font_char_width;
  if (offsetY > (_matrixCanvas->getMatrixWidth()-_font_char_width)) 
	     YMaxPos = _matrixCanvas->getMatrixWidth()-offsetX; 
    else YMaxPos = _font_char_width;



  for (byte y = YMinPos; y < YMaxPos; y++) 
  {
    int thisByte;
	if (swapLeftRight) 
		   thisByte = getLetterColumn(UTF8, y);  //NO flip   
	  else thisByte = getLetterColumn(UTF8, _font_char_width-1-y); //flip
	  
    for (byte x = 0; x < _font_char_height; x++) 
	{
      boolean thisBit;
	  if (swapTopBottom) 	  
		     thisBit = thisByte & (1<<x); //not flip
        else thisBit = thisByte & (1<<(_font_char_height-1-x)); //flip
      
	  //draw Column
        if (thisBit) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, letterColor);
          else 	
			if (_UpdateBackground) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, _BackgroundColor); // background
    }
  }
}


void XYMatrixText::drawChar_Vert(uint16_t UTF8, int16_t offsetX, int16_t offsetY, CRGB letterColor, boolean swapLeftRight, boolean swapTopBottom)
{
  int8_t XMinPos; 
  int8_t XMaxPos;
  if (offsetX < -_font_char_width || offsetX > _matrixCanvas->getMatrixWidth()) return; //out of area, nothing to draw
  
  if (offsetX < 0) XMinPos = -offsetX; else XMinPos = 0;
	
  if (offsetX > (_matrixCanvas->getMatrixWidth()-_font_char_width)) 
	     XMaxPos = _matrixCanvas->getMatrixWidth()-offsetX; 
    else XMaxPos = _font_char_width;

  for (byte x = XMinPos; x < XMaxPos; x++) 
  {
    int thisByte;
 	if (swapLeftRight) 
		   thisByte = getLetterColumn(UTF8, _font_char_width-1-x); //not flip
      else thisByte = getLetterColumn(UTF8, x);  //flip
	  
    for (byte y = 0; y < _font_char_height; y++) 
	{
      boolean thisBit;
	  if (swapTopBottom)   
		     thisBit = thisByte & (1<<y); //not flip
        else thisBit = thisByte & (1<<(_font_char_height-1-y)); //flip
      
	  //draw Column
        if (thisBit) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, letterColor);
          else 	
			if (_UpdateBackground) _matrixCanvas->setPixelColor(offsetX+x, offsetY+y, _BackgroundColor); // background
    }
  }
}

void XYMatrixText::drawString(int16_t offsetX, int16_t offsetY, CRGB letterColor)
{
uint8_t charCnt=0; //UTF8 letter counter
uint8_t index=0; //passed by ref.
uint16_t UTF8=0; //passed by ref.	
//starting position
int16_t xpos=offsetX; //accumulator for X position, start position
int16_t ypos=offsetY; //accumulator for Y position, start position
//"change position" logic...
boolean char_v   = ((_charRotation == Rotate_None) || (_charRotation == Rotate_180)); //true if char is vertical, false if horisontal
boolean string_v = ((_stringDir == Dir_LeftToRight)  || (_stringDir == Dir_RightToLeft)); //true if string is vertical, false if horisontal
int16_t IncrementPos;
if (char_v == string_v) IncrementPos = (_font_char_width +_charSpace);  
                   else IncrementPos = (_font_char_height +_charSpace);
if ((_stringDir == Dir_RightToLeft)  || (_stringDir == Dir_TopToBottom)) 
  {
  IncrementPos = - IncrementPos; //negative 
  //correcting starting position
  
  if (string_v) xpos = xpos + IncrementPos +1 +_charSpace; //decrease, with no space
           else ypos = ypos + IncrementPos +1 +_charSpace; //decrease, with no space
  }
//end of "change position" logic...
//main loop
while ( UTF8Itterator(_s, index, UTF8) )	
  {
  if (xpos > _matrixCanvas->getMatrixWidth()) break; //gets out of range, already too right, never go back
  drawChar(UTF8, xpos, ypos, letterColor);
  
  //fillout Spaces, if needed
  if ( (_UpdateBackground) && (_charSpace>0) ) 
    {
	if (charCnt>0)
	  {
       int16_t x0;
       int16_t y0;
       int16_t x1;
       int16_t y1;
	   int16_t delta1;
	   int16_t delta2;
	   if (char_v == string_v) 
	     {
	     delta1 = _font_char_width; 
	     delta2 = _font_char_height-1;
	     }
	    else 
	     {
	     delta1 = _font_char_height;
	     delta2 = _font_char_width-1; 
         }
       switch (_stringDir)
	     {
         case Dir_LeftToRight: x0=xpos-_charSpace; y0=ypos;            x1=x0+_charSpace-1;  y1=y0+delta2;       break;
         case Dir_RightToLeft: x0=xpos+delta1;     y0=ypos;            x1=x0+_charSpace-1;  y1=y0+delta2;       break;
         case Dir_TopToBottom: x0=xpos;            y0=ypos+delta1;     x1=x0+delta2;        y1=y0+_charSpace-1; break;
         case Dir_BottomToTop: x0=xpos;            y0=ypos-_charSpace; x1=x0+delta2;        y1=y0+_charSpace-1; break;
         default: break;
	     }
 	   _matrixCanvas->draw_fillrect(x0, y0, x1, y1, _BackgroundColor);
	   }
	}
	
  charCnt++;
  //change position.
  if (string_v) xpos = xpos + IncrementPos; 
           else ypos = ypos + IncrementPos; 
  }
}

//char flips
TCharFlip XYMatrixText::getCharFlip()
{return _charFlip;}

void XYMatrixText::setCharFlip(TCharFlip CharFlip)
{_charFlip = CharFlip;}

//char rotation
TCharRotation XYMatrixText::getCharRotation()
{return _charRotation;}

void XYMatrixText::setCharRotation(TCharRotation CharRotation)
{_charRotation = CharRotation;}

//Sting Direction
TStringDirection XYMatrixText::getStringDir()
{return _stringDir;}

void XYMatrixText::setStringDir(TStringDirection StringDir)
{_stringDir = StringDir;}

//background color
boolean XYMatrixText::getUpdateBackground()
{return _UpdateBackground;}

void XYMatrixText::setUpdateBackground(boolean UpdateBackground)
{_UpdateBackground = UpdateBackground;}

CRGB XYMatrixText::getBackgroundColor()
{return _BackgroundColor;}

void XYMatrixText::setBackgroundColor(CRGB Color)
{_BackgroundColor = Color;}

//char space
uint8_t XYMatrixText::getCharSpace()
{return _charSpace;}

void XYMatrixText::setCharSpace(uint8_t charSpace)
{_charSpace = charSpace;}

 //End of XYMatrixText Class
