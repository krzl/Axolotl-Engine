// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
#include "stdafx.h"

namespace axlt {

	Quaternion Quaternion::identity = Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );
	
	Quaternion::Quaternion( const float x, const float y, const float z, const float w ) {
		*this = _mm_set_ps( w, z, y, x );
	}

	Quaternion::Quaternion( const float x, const float y, const float z ) {
		*this =
			Quaternion( sin( x / 2.0f * degToRad ), 0, 0, cos( x / 2.0f * degToRad ) ) *
			Quaternion( 0, sin( y / 2.0f * degToRad ), 0, cos( y / 2.0f * degToRad ) ) *
			Quaternion( 0, 0, sin( z / 2.0f * degToRad ), cos( z / 2.0f * degToRad ) );
	}

	Quaternion::Quaternion( const __m128& v ) {
		*this = v;
	}

	Quaternion Quaternion::operator*( const Quaternion & q ) const {

		__m128 r1 = _mm_mul_ps( this->vector, _mm_set_ps( q.x, -q.y, q.z, q.w ) );
		__m128 r2 = _mm_mul_ps( this->vector, _mm_set_ps( q.y, q.x, q.w, -q.z ) );

		const __m128 res1 = _mm_hadd_ps( r1, r2 );

		r1 = _mm_mul_ps( this->vector, _mm_set_ps( q.z, q.w, -q.x, q.y ) );
		r2 = _mm_mul_ps( this->vector, _mm_set_ps( q.w, -q.z, -q.y, -q.x ) );

		const __m128 res2 = _mm_hadd_ps( r1, r2 );

		return Quaternion( _mm_hadd_ps( res1, res2 ) );
	}

	Quaternion& Quaternion::operator=( const __m128& v ) {
		_mm_store_ps( &this->x, v );
		return *this;
	}
	
	Quaternion& Quaternion::operator*=( const Quaternion& q ) {
		__m128 r1 = _mm_mul_ps( this->vector, _mm_set_ps( q.x, -q.y, q.z, q.w ) );
		__m128 r2 = _mm_mul_ps( this->vector, _mm_set_ps( q.y, q.x, q.w, -q.z ) );

		const __m128 res1 = _mm_hadd_ps( r1, r2 );

		r1 = _mm_mul_ps( this->vector, _mm_set_ps( q.z, q.w, -q.x, q.y ) );
		r2 = _mm_mul_ps( this->vector, _mm_set_ps( q.w, -q.z, -q.y, -q.x ) );

		const __m128 res2 = _mm_hadd_ps( r1, r2 );

		*this = _mm_hadd_ps( res1, res2 );
		return *this;
	}

	Quaternion Quaternion::operator-() const {
		return Quaternion( -x, -y, -z, w );
	}

	Quaternion& Quaternion::operator=( const Quaternion& q ) {
		if (this == &q) return *this;
		*this = q.vector;
		return *this;
	}

	Quaternion Quaternion::Inverse() const {
		return Quaternion( -x, -y, -z, w );
	}
}