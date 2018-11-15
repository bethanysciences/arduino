//     Shift Register LED Matrix Project - Glyph
//     Copyright (C) 2017 Michael Kamprath
//
//     This file is part of Shift Register LED Matrix Project.
// 
//     Shift Register LED Matrix Project is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
// 
//     Shift Register LED Matrix Project is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
// 
//     You should have received a copy of the GNU General Public License
//     along with Shift Register LED Matrix Project.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __GLYPH_H__
#define __GLYPH_H__

/***
 * Glyph
 *
 * A Glyph is N x M array of bits that indicates pixels in an image devoid of color.
 * Glyphs are generally used to represent the image of letters in a font, but can be
 * used for any pixel image.
 *
 */

#include "LEDImage.h"

class Glyph : public LEDImage<bool> {
public:
	static bool* generateBitBoolArray( 
			int rows,
			int columns,
			const unsigned char* data,
			bool isFromProgramSpace
		);

	Glyph(
			int rows,
			int columns,
			const bool* data,
			bool isFromProgramSpace = false
		);
	Glyph(const LEDImageBase<bool>& other);
	virtual ~Glyph();

};

class MutableGlyph : public MutableLEDImage<bool,false,0x80> {
public:
	MutableGlyph(int rows, int columns);
	MutableGlyph(
			int rows,
			int columns,
			const bool* data,
			bool isFromProgramSpace = false
		);
	MutableGlyph(const LEDImageBase<bool>& other);
	virtual ~MutableGlyph();
	
	void setBit( int row, int column )			{ this->setPixel( row, column, true); }
	void clearBit( int row, int column )		{ this->setPixel( row, column, false); }

	void placeGlyphAt( const LEDImageBase<bool>& glyph, int row, int column );

};

#endif
