// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "stdafx.h"

namespace axlt {
	Vector3::Vector3() : Vector3( 0 ) {}

	Vector3::Vector3( const float val ) {
		x = y = z = val;
	}

	Vector3::Vector3( const float x, const float y, const float z ) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	Vector3::Vector3( const Vector4& val ) {
		this->x = val.x;
		this->y = val.y;
		this->z = val.z;
	}

	Vector3 Vector3::operator-() const {
		return Vector3( -x, -y, -z );
	}

	Vector3 Vector3::operator+( const Vector3& v ) const {
		return Vector3( x + v.x, y + v.y, z + v.z );
	}

	Vector3 Vector3::operator-( const Vector3& v ) const {
		return Vector3( x - v.x, y - v.y, z - v.z );
	}

	Vector3 Vector3::operator*( const Vector3& v ) const {
		return Vector3( x * v.x, y * v.y, z * v.z );
	}

	Vector3 Vector3::operator/( const Vector3& v ) const {
		return Vector3( x / v.x, y / v.y, z / v.z );
	}

	Vector3 Vector3::operator+( const float v ) const {
		return Vector3( x + v, y + v, z + v );
	}

	Vector3 Vector3::operator-( const float v ) const {
		return Vector3( x - v, y - v, z - v );
	}

	Vector3 Vector3::operator*( const float v ) const {
		return Vector3( x * v, y * v, z * v );
	}

	Vector3 Vector3::operator/( const float v ) const {
		return Vector3( x / v, y / v, z / v );
	}

	Vector3& Vector3::operator=( const Vector3& v ) {
		if( this == &v ) return *this;
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		return *this;
	}

	Vector3& Vector3::operator+=( const Vector3& v ) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}

	Vector3& Vector3::operator-=( const Vector3& v ) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}

	Vector3& Vector3::operator*=( const Vector3& v ) {
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		return *this;
	}

	Vector3& Vector3::operator/=( const Vector3& v ) {
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		return *this;
	}

	Vector3& Vector3::operator=( const float v ) {
		x = y = z = v;
		return *this;
	}

	Vector3& Vector3::operator+=( const float v ) {
		x += v;
		y += v;
		z += v;
		return *this;
	}

	Vector3& Vector3::operator-=( const float v ) {
		x -= v;
		y -= v;
		z -= v;
		return *this;
	}

	Vector3& Vector3::operator*=( const float v ) {
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	Vector3& Vector3::operator/=( const float v ) {
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	float Vector3::Dot( const Vector3& v ) const {
		return x * v.x + y * v.y + z * v.z;
	}
	
	Vector3 Vector3::Cross( const Vector3& v ) const {
		return Vector3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
	}

	float Vector3::Magnitude() const {
		return sqrt( Dot( *this ) );
	}

	float Vector3::SqrMagnitude() const {
		return Dot( *this );
	}

	Vector3 Vector3::Normalized() const {
		return *this / Magnitude();
	}

	Vector3 Vector3::right = Vector3( 1.0f, 0.0f, 0.0f );
	Vector3 Vector3::forward = Vector3( 0.0f, 1.0f, 0.0f );
	Vector3 Vector3::up = Vector3( 0.0f, 0.0f, 1.0f );
	Vector3 Vector3::one = Vector3( 1.0f, 1.0f, 1.0f );
	Vector3 Vector3::zero = Vector3( 0.0f, 0.0f, 0.0f );
}