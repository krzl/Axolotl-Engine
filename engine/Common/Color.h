#pragma once

namespace axlt {
	struct Color {
		float r, g, b, a;

		Color();
		Color( float r, float g, float b );
		Color( float r, float g, float b, float a );
	};
}