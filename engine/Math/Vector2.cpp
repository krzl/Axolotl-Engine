// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "stdafx.h"
#include "Vector2.h"

namespace axlt {
	Vector2::Vector2() : Vector2( 0 ) {}

	Vector2::Vector2( const float val ) {
		x = y = val;
	}

	Vector2::Vector2( const float x, const float y ) {
		this->x = x;
		this->y = y;
	}

	Vector2::Vector2( const Vector3& val ) {
		this->x = val.x;
		this->y = val.y;
	}

	Vector2 Vector2::operator-() const {
		return Vector2( -x, -y );
	}

	Vector2 Vector2::operator+( const Vector2& v ) const {
		return Vector2( x + v.x, y + v.y );
	}

	Vector2 Vector2::operator-( const Vector2& v ) const {
		return Vector2( x - v.x, y - v.y );
	}

	Vector2 Vector2::operator*( const Vector2& v ) const {
		return Vector2( x * v.x, y * v.y );
	}

	Vector2 Vector2::operator/( const Vector2& v ) const {
		return Vector2( x / v.x, y / v.y );
	}

	Vector2 Vector2::operator+( const float v ) const {
		return Vector2( x + v, y + v );
	}

	Vector2 Vector2::operator-( const float v ) const {
		return Vector2( x - v, y - v );
	}

	Vector2 Vector2::operator*( const float v ) const {
		return Vector2( x * v, y * v );
	}

	Vector2 Vector2::operator/( const float v ) const {
		return Vector2( x / v, y / v );
	}

	Vector2& Vector2::operator=( const Vector2& v ) {
		if (this == &v) return *this;
		this->x = v.x;
		this->y = v.y;
		return *this;
	}

	Vector2& Vector2::operator+=( const Vector2& v ) {
		this->x += v.x;
		this->y += v.y;
		return *this;
	}

	Vector2& Vector2::operator-=( const Vector2& v ) {
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}

	Vector2& Vector2::operator*=( const Vector2& v ) {
		this->x *= v.x;
		this->y *= v.y;
		return *this;
	}

	Vector2& Vector2::operator/=( const Vector2& v ) {
		this->x /= v.x;
		this->y /= v.y;
		return *this;
	}

	Vector2& Vector2::operator=( const float v ) {
		x = y = v;
		return *this;
	}

	Vector2& Vector2::operator+=( const float v ) {
		x += v;
		y += v;
		return *this;
	}

	Vector2& Vector2::operator-=( const float v ) {
		x -= v;
		y -= v;
		return *this;
	}

	Vector2& Vector2::operator*=( const float v ) {
		x *= v;
		y *= v;
		return *this;
	}

	Vector2& Vector2::operator/=( const float v ) {
		x /= v;
		y /= v;
		return *this;
	}

	float Vector2::Magnitude() const {
		return sqrt( SqrMagnitude() );
	}

	float Vector2::SqrMagnitude() const {
		return  x * x + y * y;
	}

	Vector2 Vector2::Normalized() const {
		return *this / Magnitude();
	}
}