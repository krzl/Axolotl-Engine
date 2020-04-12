#pragma once

#include "Vector4.h"
#include <immintrin.h>

namespace axlt {
	struct Quaternion;

	// COLUMN MAJOR
	struct Matrix4 {
		union {
			Vector4 vectors[4];
			float data[16];
			struct {
				Vector4 x;
				Vector4 y;
				Vector4 z;
				Vector4 w;
			};
			struct {
				__m128 mX;
				__m128 mY;
				__m128 mZ;
				__m128 mW;
			};
			struct {
				__m256 mD0;
				__m256 mD8;
			};
		};

		Matrix4();
		explicit Matrix4( float data[16] );
		Matrix4( const Vector4& x, const Vector4& y, const Vector4& z );
		Matrix4( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w );
		Matrix4( __m256 d0, __m256 d8 );
		Matrix4( const Matrix4& m );
		
		Matrix4& Set( __m256 d0, __m256 d8 );

		static Matrix4 TranslationMatrix( const Vector3& t );
		static Matrix4 RotationMatrix( const Quaternion& q );
		static Matrix4 ScaleMatrix( const Vector3& s );
		static Matrix4 ScaleMatrix( float s );

		Matrix4 operator*( const Matrix4& m ) const;
		Matrix4& operator*=( const Matrix4& m );
		Matrix4& operator=( const Matrix4& m );
		Vector4& operator[]( int index );

		Matrix4 Transposed() const;
		Matrix4& Transpose();
	};
}
