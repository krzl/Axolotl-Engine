#pragma once

namespace axlt {
	struct Quaternion {

		union {
			__m128 vector;
			struct {
				float x;
				float y;
				float z;
				float w;
			};
		};

		Quaternion() : Quaternion( 0, 0, 0, 1 ) {}
		Quaternion( float x, float y, float z, float w );
		Quaternion( const Quaternion& v ) = default;
		explicit Quaternion( const __m128& v );

		~Quaternion() = default;

		Quaternion operator*( const Quaternion& q ) const;
		Quaternion& operator=( const __m128& v );
		Quaternion& operator=( const Quaternion& q );
		Quaternion operator-() const;

		Quaternion Quaternion::Inverse() const;
	};
}