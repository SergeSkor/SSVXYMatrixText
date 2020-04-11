#ifndef XYMatrixText_h
#define XYMatrixText_h

//
// By Serge Skorodinsky, 11/15/2019
//

#include <FastLED.h>
#include "SSVXYMatrix.h"
#include "fonts.h"

boolean UTF8Itterator(String* S, uint8_t &index, uint16_t &UTF8);

//XYMatrixText Class
class XYMatrixText
{
  public:
    //constructors
    XYMatrixText(XYMatrix* MatrixCanvas); 
	//
	void setString(String* S); //preliminary set the String to be able to calculate framing info without real drawing the string. 
	                           //If it is not needed - drawString() with String parameter may be used.
	uint16_t getStringWidthPx();
	uint16_t getStringHeightPx();
	uint8_t getFontIndex(uint16_t UTF8); //returns index in font
	uint8_t getLetterColumn(uint16_t UTF8, uint8_t column); //font colunmn 0..4, from left to right
	
	//not used
	//uint8_t getLetterRow(uint16_t UTF8, uint8_t row); //font row 0..7, from bottom to top
	
	void drawLetter(uint16_t UTF8, int16_t offsetX, int16_t offsetY, CRGB letterColor);
	void drawString(int16_t offsetX, int16_t offsetY, CRGB letterColor); //String must be specify in advance by using .setString();
	void drawString(String* S, int16_t offsetX, int16_t offsetY, CRGB letterColor); //drawString() with String parameter
	
	//positioning, offsets
	boolean IsStringWidthFit();
	boolean IsStringHeightFit();
	//positioning "just hide"
	int16_t OffsetX_RightStr_To_LeftArea(); //offsetX when right text edge at the left edge of area, text is NOT visible 
	int16_t OffsetX_LeftStr_To_RightArea(); //offsetX when left text edge at the right edge of area, text is NOT visible 
	int16_t OffsetY_TopStr_To_BottomArea(); //offsetY when top text edge at the bottom edge of the area, text is NOT visible
	int16_t OffsetY_BottomStr_To_TopArea(); //offsetY when bottom text edge at the top edge of the area, text is NOT visible
    //positioning "max visibility" 
    int16_t OffsetX_LeftStr_To_LeftArea(); //offsetX when left text edge at the left edge of area, text is max visible 
	int16_t OffsetX_RightStr_To_RightArea(); //offsetX when right text edge at the right edge of area, text is max visible 
	int16_t OffsetY_BottomStr_To_BottomArea(); //offsetY when bottom text edge at the bottom edge of area, text is max visible 
	int16_t OffsetY_TopStr_To_TopArea(); //offsetY when top text edge at the top edge of area, text is max visible 
	//positioning "center", if string is bigger (wider or higher) - beginning and ending of string will be out of matrix
	int16_t OffsetX_Center(); //center offsetX when ...
	int16_t OffsetY_Center(); //center offsetY when ...
	
  protected:
    XYMatrix*  _matrixCanvas;
    String* _s;
    uint8_t    _charSpace;
    boolean _left_right_swap; 
    boolean _top_bottom_swap;
	uint8_t _font_char_width;  //5   //char width, depends on font
    uint8_t _font_char_height; //8   //char height, depends on font
	//background
	boolean _UpdateBackground;
	CRGB _BackgroundColor;
	//functions
	void drawString_LeftToRight(int16_t offsetX, int16_t offsetY, CRGB letterColor);

}; //End of XYMatrixText Class

#endif
//END OF FILE
