#pragma once

#include <emmintrin.h>

namespace axlt {
	struct Vector3 {
		union {
			__m128 vector;
			struct {
				float x;
				float y;
				float z;
				float padding;
			};
		};

		Vector3() = default;
		explicit Vector3( float val );
		Vector3( float x, float y, float z );
		explicit Vector3( const __m128& val );
		Vector3( const Vector3& val ) = default;

		~Vector3() = default;

		Vector3 operator-() const;
		Vector3 operator+( const Vector3& v ) const;
		Vector3 operator-( const Vector3& v ) const;
		Vector3 operator*( const Vector3& v ) const;
		Vector3 operator/( const Vector3& v ) const;
		Vector3 operator+( float v ) const;
		Vector3 operator-( float v ) const;
		Vector3 operator*( float v ) const;
		Vector3 operator/( float v ) const;
		Vector3& operator=( const __m128& v );
		Vector3& operator=( const Vector3& v );
		Vector3& operator+=( const Vector3& v );
		Vector3& operator-=( const Vector3& v );
		Vector3& operator*=( const Vector3& v );
		Vector3& operator/=( const Vector3& v );
		Vector3& operator=( float v );
		Vector3& operator+=( float v );
		Vector3& operator-=( float v );
		Vector3& operator*=( float v );
		Vector3& operator/=( float v );
		float Dot( const Vector3& v ) const;
		Vector3 Cross( const Vector3& v ) const;
		float Magnitude() const;
		float SqrMagnitude() const;
		Vector3 Normalized() const;
	};
}
