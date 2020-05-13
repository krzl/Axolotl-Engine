// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "stdafx.h"
#include "Vector2Int.h"

namespace axlt {
	Vector2Int::Vector2Int() : Vector2Int( 0 ) {}

	Vector2Int::Vector2Int( const int32_t val ) {
		x = y = val;
	}

	Vector2Int::Vector2Int( const int32_t x, const int32_t y ) {
		this->x = x;
		this->y = y;
	}

	Vector2Int Vector2Int::operator-() const {
		return Vector2Int( -x, -y );
	}

	Vector2Int Vector2Int::operator+( const Vector2Int& v ) const {
		return Vector2Int( x + v.x, y + v.y );
	}

	Vector2Int Vector2Int::operator-( const Vector2Int& v ) const {
		return Vector2Int( x - v.x, y - v.y );
	}

	Vector2Int Vector2Int::operator*( const Vector2Int& v ) const {
		return Vector2Int( x * v.x, y * v.y );
	}

	Vector2Int Vector2Int::operator/( const Vector2Int& v ) const {
		return Vector2Int( x / v.x, y / v.y );
	}

	Vector2Int Vector2Int::operator+( const int32_t v ) const {
		return Vector2Int( x + v, y + v );
	}

	Vector2Int Vector2Int::operator-( const int32_t v ) const {
		return Vector2Int( x - v, y - v );
	}

	Vector2Int Vector2Int::operator*( const int32_t v ) const {
		return Vector2Int( x * v, y * v );
	}

	Vector2Int Vector2Int::operator/( const int32_t v ) const {
		return Vector2Int( x / v, y / v );
	}

	Vector2Int& Vector2Int::operator=( const Vector2Int& v ) {
		if (this == &v) return *this;
		this->x = v.x;
		this->y = v.y;
		return *this;
	}

	Vector2Int& Vector2Int::operator+=( const Vector2Int& v ) {
		this->x += v.x;
		this->y += v.y;
		return *this;
	}

	Vector2Int& Vector2Int::operator-=( const Vector2Int& v ) {
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}

	Vector2Int& Vector2Int::operator*=( const Vector2Int& v ) {
		this->x *= v.x;
		this->y *= v.y;
		return *this;
	}

	Vector2Int& Vector2Int::operator/=( const Vector2Int& v ) {
		this->x /= v.x;
		this->y /= v.y;
		return *this;
	}

	Vector2Int& Vector2Int::operator=( const int32_t v ) {
		x = y = v;
		return *this;
	}

	Vector2Int& Vector2Int::operator+=( const int32_t v ) {
		x += v;
		y += v;
		return *this;
	}

	Vector2Int& Vector2Int::operator-=( const int32_t v ) {
		x -= v;
		y -= v;
		return *this;
	}

	Vector2Int& Vector2Int::operator*=( const int32_t v ) {
		x *= v;
		y *= v;
		return *this;
	}

	Vector2Int& Vector2Int::operator/=( const int32_t v ) {
		x /= v;
		y /= v;
		return *this;
	}

	float Vector2Int::Magnitude() const {
		return sqrt( (float) SqrMagnitude() );
	}

	uint32_t Vector2Int::SqrMagnitude() const {
		return x * x + y * y;
	}
}