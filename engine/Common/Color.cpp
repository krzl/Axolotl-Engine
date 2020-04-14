#include "stdafx.h"
#include "Color.h"

namespace axlt {

	Color::Color() :
		Color( 0.0f, 0.0f, 0.0f ) {}

	Color::Color( const float r, const float g, const float b ) :
		Color( r, g, b, 1.0f ) {}

	Color::Color( const float r, const float g, const float b, const float a ) :
		r( r ),
		g( g ),
		b( b ),
		a( a ) {}
}
