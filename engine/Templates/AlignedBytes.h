#pragma once

namespace axlt {
	template<uint32_t Size, uint32_t Alignment>
	struct AlignedBytes {

	protected:

		struct __declspec( align( Alignment ) ) AlignedData {
			uint8_t data[Size];
		};

		AlignedData aligned;
	};
}