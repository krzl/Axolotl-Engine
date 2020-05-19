#include "stdafx.h"
#include "Color.h"

namespace axlt {

	Color::Color() :
		Color( 0.0f, 0.0f, 0.0f ) {}

	Color::Color( const uint32_t color ) {
		const uint32_t r = (color & 0xFF000000) >> 24;
		const uint32_t g = (color & 0x00FF0000) >> 16;
		const uint32_t b = (color & 0x0000FF00) >> 8;
		const uint32_t a = color & 0x000000FF;

		this->r = (float)r / 255.0f;
		this->g = (float)g / 255.0f;
		this->b = (float)b / 255.0f;
		this->a = (float)a / 255.0f;
	}

	Color::Color( const float r, const float g, const float b ) :
	                                                            Color( r, g, b, 1.0f ) {}

	Color::Color( const float r, const float g, const float b, const float a ) :
		r( r ),
		g( g ),
		b( b ),
		a( a ) {}
}
