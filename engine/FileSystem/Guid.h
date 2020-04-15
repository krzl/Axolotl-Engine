#pragma once

namespace axlt {
	class Guid {

		friend class ResourceData;
		friend uint32_t GetHash( const Guid& );

	public:

		static Guid GenerateGuid();
		static Guid FromString( const String& string );

		String ToString() const;

	private:

		Guid( uint64_t high, uint64_t low );

		uint64_t high, low;
	};

	uint32_t GetHash( const Guid& guid );
}