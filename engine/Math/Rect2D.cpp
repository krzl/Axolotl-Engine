#include "stdafx.h"
#include "Rect2D.h"

namespace axlt {

	Rect2D::Rect2D() : Rect2D( 0, 1, 0, 1 ) {}

	Rect2D::Rect2D( const Vector2& x, const Vector2& y ) : Rect2D( x.x, x.y, y.x, y.y ) {}

	Rect2D::Rect2D( const float minX, const float maxX, const float minY, const float maxY ) :
		minX( minX ),
		maxX( maxX ),
		minY( minY ),
		maxY( maxY ) {}

	void Rect2D::Set( const Vector2& x, const Vector2& y ) {
		operator=( Rect2D( x, y ) );
	}

	void Rect2D::Set( const float minX, const float maxX, const float minY, const float maxY ) {
		operator=( Rect2D( minX, maxX, minY, maxY ) );
	}

	void Rect2D::SetMinX( const float value ) {
		minX = value > maxX ? maxX : value;
	}

	float Rect2D::GetMinX() const {
		return minX;
	}

	void Rect2D::SetMaxX( const float value ) {
		maxX = value < minX ? minX : value;
	}

	float Rect2D::GetMaxX() const {
		return maxX;
	}

	void Rect2D::SetMinY( const float value ) {
		minY = value > maxY ? maxY : value;
	}

	float Rect2D::GetMinY() const {
		return minY;
	}

	void Rect2D::SetMaxY( const float value ) {
		maxY = value < minY ? minY : value;
	}

	float Rect2D::GetMaxY() const {
		return maxY;
	}

	float Rect2D::Width() const {
		return maxX - minX;
	}

	float Rect2D::Height() const {
		return maxY - minY;
	}

	Vector2 Rect2D::Center() const {
		return Vector2( ( maxX - minX ) / 2.0f, ( maxY - minY ) / 2.0f );
	}

	static bool Overlaps1D( const float aMin, const float aMax, const float bMin, const float bMax ) {
		return !( aMax < bMin && aMin > bMax );
	}

	bool Rect2D::Overlaps( const Rect2D& other ) const {
		return Overlaps1D( minX, maxX, other.minX, other.maxX ) &&
			Overlaps1D( minY, maxY, other.minY, other.maxY );
	}

	bool Rect2D::Contains( const Vector2& other ) const {
		return ( minX <= other.x && other.x <= maxX ) && ( minY <= other.y && other.y <= maxY );
	}

	bool Rect2D::operator==( const Rect2D& other ) const {
		if( this == &other ) return true;
		return minX == other.minX && maxX == other.maxX && minY == other.minY && maxY == other.maxY;
	}

	Rect2D& Rect2D::operator=( const Rect2D& other ) {
		if( this == &other ) return *this;
		Set( other.minX, other.maxX, other.minY, other.maxY );
		return *this;
	}
}
