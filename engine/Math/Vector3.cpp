// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "stdafx.h"

namespace axlt {
	Vector3::Vector3() : Vector3( 0 ) {}

	Vector3::Vector3( const float val ) {
		*this = _mm_set_ps1( val );
	}

	Vector3::Vector3( const float x, const float y, const float z ) {
		vector = _mm_set_ps( 0.0f, z, y, x );
	}

	Vector3::Vector3( const __m128& val ) {
		*this = val;
	}

	Vector3::Vector3( const Vector4& val ) {
		vector = _mm_set_ps( 0.0f, val.z, val.y, val.x );
	}

	Vector3 Vector3::operator-() const {
		return Vector3( _mm_sub_ps( _mm_setzero_ps(), vector ) );
	}

	Vector3 Vector3::operator+( const Vector3& v ) const {
		return Vector3( _mm_add_ps( this->vector, v.vector ) );
	}

	Vector3 Vector3::operator-( const Vector3& v ) const {
		return Vector3( _mm_sub_ps( this->vector, v.vector ) );
	}

	Vector3 Vector3::operator*( const Vector3& v ) const {
		return Vector3( _mm_mul_ps( this->vector, v.vector ) );
	}

	Vector3 Vector3::operator*( const Matrix4& m ) const {
		return Vector3( Vector4( this->x, this->y, this->z, 1.0f ) * m );
	}

	Vector3 Vector3::operator/( const Vector3& v ) const {
		return Vector3( _mm_div_ps( this->vector, v.vector ) );
	}

	Vector3 Vector3::operator+( const float v ) const {
		return Vector3( _mm_add_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector3 Vector3::operator-( const float v ) const {
		return Vector3( _mm_sub_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector3 Vector3::operator*( const float v ) const {
		return Vector3( _mm_mul_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector3 Vector3::operator/( const float v ) const {
		return Vector3( _mm_div_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector3& Vector3::operator=( const __m128& v ) {
		_mm_store_ps( &this->x, v );
		padding = 0;
		return *this;
	}

	Vector3& Vector3::operator=( const Vector3& v ) {
		if( this == &v ) return *this;
		*this = v.vector;
		return *this;
	}

	Vector3& Vector3::operator+=( const Vector3& v ) {
		*this = _mm_add_ps( this->vector, v.vector );
		return *this;
	}

	Vector3& Vector3::operator-=( const Vector3& v ) {
		*this = _mm_sub_ps( this->vector, v.vector );
		return *this;
	}

	Vector3& Vector3::operator*=( const Vector3& v ) {
		*this = _mm_mul_ps( this->vector, v.vector );
		return *this;
	}

	Vector3& Vector3::operator*=( const Matrix4& m ) {
		*this = *this * m;
		return *this;
	}

	Vector3& Vector3::operator/=( const Vector3& v ) {
		*this = _mm_div_ps( this->vector, v.vector );
		return *this;
	}

	Vector3& Vector3::operator=( const float v ) {
		*this = _mm_set_ps1( v );
		return *this;
	}

	Vector3& Vector3::operator+=( const float v ) {
		*this = _mm_add_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector3& Vector3::operator-=( const float v ) {
		*this = _mm_sub_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector3& Vector3::operator*=( const float v ) {
		*this = _mm_mul_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector3& Vector3::operator/=( const float v ) {
		*this = _mm_div_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	float Vector3::Dot( const Vector3& v ) const {
		const __m128 r1 = _mm_mul_ps( this->vector, v.vector );
		__m128 shuf = _mm_shuffle_ps( r1, r1, _MM_SHUFFLE( 2, 3, 0, 1 ) );
		__m128 sums = _mm_add_ps( r1, shuf );
		shuf = _mm_movehl_ps( shuf, sums );
		sums = _mm_add_ss( sums, shuf );
		return _mm_cvtss_f32( sums );
	}

	Vector3 Vector3::Cross( const Vector3& v ) const {
		return Vector3( _mm_sub_ps(
			_mm_mul_ps( _mm_shuffle_ps( this->vector, this->vector, _MM_SHUFFLE( 3, 0, 2, 1 ) ), _mm_shuffle_ps( v.vector, v.vector, _MM_SHUFFLE( 3, 1, 0, 2 ) ) ),
			_mm_mul_ps( _mm_shuffle_ps( this->vector, this->vector, _MM_SHUFFLE( 3, 1, 0, 2 ) ), _mm_shuffle_ps( v.vector, v.vector, _MM_SHUFFLE( 3, 0, 2, 1 ) ) )
		) );
	}

	float Vector3::Magnitude() const {
		const __m128 r1 = _mm_mul_ps( this->vector, this->vector );
		__m128 shuf = _mm_shuffle_ps( r1, r1, _MM_SHUFFLE( 2, 3, 0, 1 ) );
		__m128 sums = _mm_add_ps( r1, shuf );
		shuf = _mm_movehl_ps( shuf, sums );
		sums = _mm_add_ss( sums, shuf );
		return _mm_cvtss_f32( _mm_sqrt_ss( sums ) );
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