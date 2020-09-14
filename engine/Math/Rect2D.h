#pragma once

namespace axlt {
	struct Rect2D {

		Rect2D();
		Rect2D( const Vector2& x, const Vector2& y );
		Rect2D( float minX, float maxX, float minY, float maxY );
		
		void Set( const Vector2& x, const Vector2& y );
		void Set( float minX, float maxX, float minY, float maxY );

		void SetMinX( float value );
		float GetMinX() const;
		void SetMaxX( float value );
		float GetMaxX() const;
		void SetMinY( float value );
		float GetMinY() const;
		void SetMaxY( float value );
		float GetMaxY() const;

		float Width() const;
		float Height() const;
		Vector2 Center() const;

		bool Overlaps( const Rect2D& other ) const;
		bool Contains( const Vector2& other ) const;

		bool operator==( const Rect2D& other ) const;
		Rect2D& operator=( const Rect2D& other );

	private:
		
		float minX;
		float maxX;
		float minY;
		float maxY;
	};
}