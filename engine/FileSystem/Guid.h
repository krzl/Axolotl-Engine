#pragma once

namespace axlt {
	class String;

	class Guid {
		
		friend uint32_t GetHash( const Guid& );

	public:

		Guid( uint64_t high, uint64_t low );

		static Guid invalidGuid;
		
		static Guid GenerateGuid();
		static Guid FromString( const String& string );

		String ToString() const;

	private:

		uint64_t high, low;
	};

	uint32_t GetHash( const Guid& guid );
}