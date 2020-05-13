#pragma once

namespace axlt {
	struct Vector2Int {
		int32_t x;
		int32_t y;

		Vector2Int();
		explicit Vector2Int( int32_t val );
		Vector2Int( int32_t x, int32_t y );
		Vector2Int( const Vector2Int& val ) = default;

		~Vector2Int() = default;

		Vector2Int operator-() const;
		Vector2Int operator+( const Vector2Int& v ) const;
		Vector2Int operator-( const Vector2Int& v ) const;
		Vector2Int operator*( const Vector2Int& v ) const;
		Vector2Int operator/( const Vector2Int& v ) const;
		Vector2Int operator+( int32_t v ) const;
		Vector2Int operator-( int32_t v ) const;
		Vector2Int operator*( int32_t v ) const;
		Vector2Int operator/( int32_t v ) const;
		Vector2Int& operator=( const Vector2Int& v );
		Vector2Int& operator+=( const Vector2Int& v );
		Vector2Int& operator-=( const Vector2Int& v );
		Vector2Int& operator*=( const Vector2Int& v );
		Vector2Int& operator/=( const Vector2Int& v );
		Vector2Int& operator=( int32_t v );
		Vector2Int& operator+=( int32_t v );
		Vector2Int& operator-=( int32_t v );
		Vector2Int& operator*=( int32_t v );
		Vector2Int& operator/=( int32_t v );
		float Magnitude() const;
		uint32_t SqrMagnitude() const;
	};
}
