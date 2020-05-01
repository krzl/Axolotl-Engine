#pragma once

namespace axlt {
	struct Vector3 {
		float x;
		float y;
		float z;

		Vector3();
		explicit Vector3( float val );
		Vector3( float x, float y, float z );
		Vector3( const Vector3& val ) = default;
		// ReSharper disable once CppNonExplicitConvertingConstructor
		Vector3( const Vector4& val );

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

		static Vector3 forward;
		static Vector3 right;
		static Vector3 up;
		static Vector3 one;
		static Vector3 zero;
	};
}
