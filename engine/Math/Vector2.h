#pragma once

namespace axlt {
	struct Vector2 {
		float x;
		float y;

		Vector2();
		explicit Vector2( float val );
		Vector2( float x, float y );
		Vector2( const Vector2& val ) = default;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		Vector2( const Vector3& val );

		~Vector2() = default;

		Vector2 operator-() const;
		Vector2 operator+( const Vector2& v ) const;
		Vector2 operator-( const Vector2& v ) const;
		Vector2 operator*( const Vector2& v ) const;
		Vector2 operator/( const Vector2& v ) const;
		Vector2 operator+( float v ) const;
		Vector2 operator-( float v ) const;
		Vector2 operator*( float v ) const;
		Vector2 operator/( float v ) const;
		Vector2& operator=( const Vector2& v );
		Vector2& operator+=( const Vector2& v );
		Vector2& operator-=( const Vector2& v );
		Vector2& operator*=( const Vector2& v );
		Vector2& operator/=( const Vector2& v );
		Vector2& operator=( float v );
		Vector2& operator+=( float v );
		Vector2& operator-=( float v );
		Vector2& operator*=( float v );
		Vector2& operator/=( float v );
		float Magnitude() const;
		float SqrMagnitude() const;
		Vector2 Normalized() const;
	};
}
