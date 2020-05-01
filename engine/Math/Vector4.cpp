// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "stdafx.h"

namespace axlt {
	Vector4::Vector4( const float val ) {
		*this = _mm_set1_ps( val );
	}

	Vector4::Vector4( const float x, const float y, const float z, const float w ) {
		*this = _mm_set_ps( w, z, y, x );
	}

	Vector4::Vector4( const Vector3& val, const float w ) {
		*this = _mm_set_ps( w, val.z, val.y, val.x );
	}

	Vector4::Vector4( const __m128& val ) {
		*this = val;
	}

	Vector4 Vector4::operator-() const {
		return Vector4( _mm_sub_ps( _mm_setzero_ps(), vector ) );
	}

	Vector4 Vector4::operator+( const Vector4& v ) const {
		return Vector4( _mm_add_ps( this->vector, v.vector ) );
	}

	Vector4 Vector4::operator-( const Vector4& v ) const {
		return Vector4( _mm_sub_ps( this->vector, v.vector ) );
	}

	Vector4 Vector4::operator*( const Vector4& v ) const {
		return Vector4( _mm_mul_ps( this->vector, v.vector ) );
	}

	Vector4 Vector4::operator/( const Vector4& v ) const {
		return Vector4( _mm_div_ps( this->vector, v.vector ) );
	}

	Vector4 Vector4::operator+( const float v ) const {
		return Vector4( _mm_add_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector4 Vector4::operator-( const float v ) const {
		return Vector4( _mm_sub_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector4 Vector4::operator*( const float v ) const {
		return Vector4( _mm_mul_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector4 Vector4::operator/( const float v ) const {
		return Vector4( _mm_div_ps( this->vector, _mm_set_ps1( v ) ) );
	}

	Vector4 Vector4::operator*( const Matrix4& m ) const {
		__m128 res = _mm_mul_ps( m.x.vector, _mm_broadcast_ss( &x ) );
		res = _mm_add_ps( res, _mm_mul_ps( m.y.vector, _mm_broadcast_ss( &y ) ) );
		res = _mm_add_ps( res, _mm_mul_ps( m.z.vector, _mm_broadcast_ss( &z ) ) );
		res = _mm_add_ps( res, _mm_mul_ps( m.w.vector, _mm_broadcast_ss( &w ) ) );
		return Vector4( res );
	}

	Vector4& Vector4::operator=( const __m128& v ) {
		_mm_store_ps( &this->x, v );
		return *this;
	}

	Vector4& Vector4::operator=( const Vector4& v ) {
		if( this == &v ) return *this;
		*this = v.vector;
		return *this;
	}

	Vector4& Vector4::operator+=( const Vector4& v ) {
		*this = _mm_add_ps( this->vector, v.vector );
		return *this;
	}

	Vector4& Vector4::operator-=( const Vector4& v ) {
		*this = _mm_sub_ps( this->vector, v.vector );
		return *this;
	}

	Vector4& Vector4::operator*=( const Vector4& v ) {
		*this = _mm_mul_ps( this->vector, v.vector );
		return *this;
	}

	Vector4& Vector4::operator/=( const Vector4& v ) {
		*this = _mm_div_ps( this->vector, v.vector );
		return *this;
	}

	Vector4& Vector4::operator=( const float v ) {
		*this = _mm_set_ps1( v );
		return *this;
	}

	Vector4& Vector4::operator+=( const float v ) {
		*this = _mm_add_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector4& Vector4::operator-=( const float v ) {
		*this = _mm_sub_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector4& Vector4::operator*=( const float v ) {
		*this = _mm_mul_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector4& Vector4::operator/=( const float v ) {
		*this = _mm_div_ps( this->vector, _mm_set_ps1( v ) );
		return *this;
	}

	Vector4& Vector4::operator*=( const Matrix4& m ) {
		__m128 res = _mm_mul_ps( m.x.vector, _mm_broadcast_ss( &x ) );
		res = _mm_add_ps( res, _mm_mul_ps( m.y.vector, _mm_broadcast_ss( &y ) ) );
		res = _mm_add_ps( res, _mm_mul_ps( m.z.vector, _mm_broadcast_ss( &z ) ) );
		res = _mm_add_ps( res, _mm_mul_ps( m.w.vector, _mm_broadcast_ss( &w ) ) );
		*this = res;
		return *this;
	}

	float& Vector4::operator[]( const int index ) {
		FLXASSERT( index < 4, "Index is higher than 3" );
		return data[index];
	}

	float Vector4::operator[]( const int index ) const {
		FLXASSERT( index < 4, "Index is higher than 3" );
		return data[index];
	}

	float Vector4::Dot( const Vector4& v ) const {
		const __m128 r1 = _mm_mul_ps( this->vector, v.vector );
		__m128 shuf = _mm_shuffle_ps( r1, r1, _MM_SHUFFLE( 2, 3, 0, 1 ) );
		__m128 sums = _mm_add_ps( r1, shuf );
		shuf = _mm_movehl_ps( shuf, sums );
		sums = _mm_add_ss( sums, shuf );
		return _mm_cvtss_f32( sums );
	}

	Vector4 Vector4::Cross( const Vector4& v ) const {
		return Vector4( _mm_sub_ps(
			_mm_mul_ps( _mm_shuffle_ps( this->vector, this->vector, _MM_SHUFFLE( 3, 0, 2, 1 ) ), _mm_shuffle_ps( v.vector, v.vector, _MM_SHUFFLE( 3, 1, 0, 2 ) ) ),
			_mm_mul_ps( _mm_shuffle_ps( this->vector, this->vector, _MM_SHUFFLE( 3, 1, 0, 2 ) ), _mm_shuffle_ps( v.vector, v.vector, _MM_SHUFFLE( 3, 0, 2, 1 ) ) )
		) );
	}

	float Vector4::Magnitude() const {
		const __m128 r1 = _mm_mul_ps( this->vector, this->vector );
		__m128 shuf = _mm_shuffle_ps( r1, r1, _MM_SHUFFLE( 2, 3, 0, 1 ) );
		__m128 sums = _mm_add_ps( r1, shuf );
		shuf = _mm_movehl_ps( shuf, sums );
		sums = _mm_add_ss( sums, shuf );
		return _mm_cvtss_f32( _mm_sqrt_ss( sums ) );
	}

	float Vector4::SqrMagnitude() const {
		return Dot( *this );
	}

	Vector4 Vector4::Normalized() const {
		return *this / Magnitude();
	}
}
