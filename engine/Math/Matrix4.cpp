// ReSharper disable CppClangTidyCppcoreguidelinesProTypeMemberInit
// ReSharper disable IdentifierTypo
// ReSharper disable CppJoinDeclarationAndAssignment
// ReSharper disable CppInconsistentNaming
// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#include "stdafx.h"
#include "Matrix4.h"

namespace axlt {
	Matrix4::Matrix4() {
		x = Vector4( 1, 0, 0, 0 );
		y = Vector4( 0, 1, 0, 0 );
		z = Vector4( 0, 0, 1, 0 );
		w = Vector4( 0, 0, 0, 1 );
	}

	Matrix4::Matrix4( float data[16] ) {
		memcpy( this, data, sizeof( Matrix4 ) );  // NOLINT(bugprone-undefined-memory-manipulation)
	}

	Matrix4::Matrix4( const Vector4& x, const Vector4& y, const Vector4& z ) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = Vector4( 0, 0, 0, 1 );
	}

	Matrix4::Matrix4( const Vector4& x, const Vector4& y, const Vector4& z, const Vector4& w ) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Matrix4::Matrix4( const __m256 d0, const __m256 d8 ) {
		Set( d0, d8 );
	}

	Matrix4::Matrix4( const Matrix4& m ) {
		memcpy( this, &data, sizeof( Matrix4 ) );  // NOLINT(bugprone-undefined-memory-manipulation)
	}

	Matrix4 Matrix4::TranslationMatrix( const Vector3& t ) {
		return Matrix4{
			Vector4( 1, 0, 0, 0 ),
			Vector4( 0, 1, 0, 0 ),
			Vector4( 0, 0, 1, 0 ),
			Vector4( t.x, t.y, t.z, 1 ),
		};
	}

	Matrix4 Matrix4::RotationMatrix( const Quaternion& q ) {

		__m128 yzxw = _mm_shuffle_ps( q.vector, q.vector, _MM_SHUFFLE( 3, 0, 2, 1 ) );
		__m128 yzxw2 = _mm_add_ps( yzxw, yzxw );
		__m128 zxyw = _mm_shuffle_ps( q.vector, q.vector, _MM_SHUFFLE( 3, 1, 0, 2 ) );
		__m128 zxyw2 = _mm_add_ps( zxyw, zxyw );
		__m128 wwww = _mm_shuffle_ps( q.vector, q.vector, _MM_SHUFFLE( 3, 3, 3, 3 ) );

		__m128 m0 = _mm_sub_ps(
			_mm_set_ps1( 1.0f ),
			_mm_add_ps( _mm_mul_ps( yzxw, yzxw2 ), _mm_mul_ps( zxyw, zxyw2 )
			) );

		__m128 m1 = _mm_add_ps(
			_mm_mul_ps( q.vector, zxyw2 ),
			_mm_mul_ps( wwww, yzxw2 )
		);

		__m128 m2 = _mm_sub_ps(
			_mm_mul_ps( q.vector, yzxw2 ),
			_mm_mul_ps( wwww, zxyw2 )
		);

		alignas( 16 ) unsigned int select_x[4] = { 0xFFFFFFFF, 0, 0, 0 };
		alignas( 16 ) unsigned int select_z[4] = { 0, 0, 0xFFFFFFFF, 0 };

		__m128 _m_select_x = _mm_load_ps( (float*) select_x );
		__m128 _m_select_z = _mm_load_ps( (float*) select_z );

	#define M128_MASK( a, b, mask ) _mm_or_ps( _mm_and_ps( mask, a ), _mm_andnot_ps( mask, b ) )

		Vector3 r0 = Vector3( M128_MASK( m2, M128_MASK( m0, m1, _m_select_x ), _m_select_z ) );
		Vector3 r1 = Vector3( M128_MASK( m1, M128_MASK( m2, m0, _m_select_x ), _m_select_z ) );
		Vector3 r2 = Vector3( M128_MASK( m0, M128_MASK( m1, m2, _m_select_x ), _m_select_z ) );

		return Matrix4( Vector4( r0 ), Vector4( r1 ), Vector4( r2 ) );
	}

	Matrix4 Matrix4::ScaleMatrix( const Vector3& s ) {
		return Matrix4{
			Vector4( s.x, 0, 0, 0 ),
			Vector4( 0, s.y, 0, 0 ),
			Vector4( 0, 0, s.z, 0 ),
			Vector4( 0, 0, 0, 1 ),
		};
	}

	Matrix4 Matrix4::ScaleMatrix( const float s ) {
		return Matrix4{
			Vector4( s, 0, 0, 0 ),
			Vector4( 0, s, 0, 0 ),
			Vector4( 0, 0, s, 0 ),
			Vector4( 0, 0, 0, 1 ),
		};
	}

	static inline __m256 Twolincomb_AVX_8( const __m256 a01, const Matrix4& b ) {
		__m256 result;
		result = _mm256_mul_ps( _mm256_shuffle_ps( a01, a01, 0x00 ), _mm256_broadcast_ps( &b.vectors[0].vector ) );
		result = _mm256_add_ps( result, _mm256_mul_ps( _mm256_shuffle_ps( a01, a01, 0x55 ), _mm256_broadcast_ps( &b.vectors[1].vector ) ) );
		result = _mm256_add_ps( result, _mm256_mul_ps( _mm256_shuffle_ps( a01, a01, 0xaa ), _mm256_broadcast_ps( &b.vectors[2].vector ) ) );
		result = _mm256_add_ps( result, _mm256_mul_ps( _mm256_shuffle_ps( a01, a01, 0xff ), _mm256_broadcast_ps( &b.vectors[3].vector ) ) );
		return result;
	}

	Matrix4 Matrix4::operator*( const Matrix4& m ) const {
		_mm256_zeroupper();
		const __m256 out01x = Twolincomb_AVX_8( mD0, m );
		const __m256 out23x = Twolincomb_AVX_8( mD8, m );
		return Matrix4( out01x, out23x );
	}

	Matrix4& Matrix4::operator*=( const Matrix4& m ) {
		_mm256_zeroupper();
		const __m256 out01x = Twolincomb_AVX_8( mD0, m );
		const __m256 out23x = Twolincomb_AVX_8( mD8, m );
		Set( out01x, out23x );
		return *this;
	}

	Matrix4& Matrix4::operator=( const Matrix4& m ) {
		if( this == &m ) return *this;
		x = m.mX;
		y = m.mY;
		z = m.mZ;
		w = m.mW;
		return *this;
	}

	Vector4& Matrix4::operator[]( const int index ) {
		FLXASSERT( index < 4, "Index is higher than 3" );
		return vectors[index];
	}

	Matrix4& Matrix4::Set( const __m256 d0, const __m256 d8 ) {
		_mm256_store_ps( &data[0], d0 );
		_mm256_store_ps( &data[8], d8 );
		return *this;
	}

	Matrix4 Matrix4::Transposed() const {
		Matrix4 matrix = Matrix4( *this );
		return matrix.Transpose();
	}

	Matrix4& Matrix4::Transpose() {
		_MM_TRANSPOSE4_PS( x.vector, y.vector, z.vector, w.vector );
		return *this;
	}
}
