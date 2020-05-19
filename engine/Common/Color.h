#pragma once

namespace axlt {
	struct Color {
		float r, g, b, a;

		Color();
		explicit Color( uint32_t color );
		Color( float r, float g, float b );
		Color( float r, float g, float b, float a );
	};
}