#pragma once

#include <emmintrin.h>

namespace axlt {
	struct Vector3;
	struct Matrix4;

	struct Vector4 {
		union {
			__m128 vector;
			float data[4];
			struct {
				float x;
				float y;
				float z;
				float w;
			};
		};

		Vector4() = default;
		explicit Vector4( float val );
		Vector4( float x, float y, float z, float w );
		explicit Vector4( const Vector3& val, float w = 1.0f );
		Vector4( const Vector4& val ) = default;
		explicit Vector4( const __m128& val );

		~Vector4() = default;

		Vector4 operator-() const;
		Vector4 operator+( const Vector4& v ) const;
		Vector4 operator-( const Vector4& v ) const;
		Vector4 operator*( const Vector4& v ) const;
		Vector4 operator/( const Vector4& v ) const;
		Vector4 operator+( float v ) const;
		Vector4 operator-( float v ) const;
		Vector4 operator*( float v ) const;
		Vector4 operator/( float v ) const;
		Vector4 operator*( const Matrix4& m ) const;
		Vector4& operator=( const __m128& v );
		Vector4& operator=( const Vector4& v );
		Vector4& operator+=( const Vector4& v );
		Vector4& operator-=( const Vector4& v );
		Vector4& operator*=( const Vector4& v );
		Vector4& operator/=( const Vector4& v );
		Vector4& operator*=( const Matrix4& m );
		Vector4& operator=( float v );
		Vector4& operator+=( float v );
		Vector4& operator-=( float v );
		Vector4& operator*=( float v );
		Vector4& operator/=( float v );
		float& operator[]( int index );
		float operator[]( int index ) const;
		float Dot( const Vector4& v ) const;
		Vector4 Cross( const Vector4& v ) const;
		float Magnitude() const;
		float SqrMagnitude() const;
		Vector4 Normalized() const;
	};
}
