#pragma once

#include "ArrayAllocators.h"
#include "../SparseArray.h"

namespace axlt {

	template<typename _ElementArrayAllocator = DefaultSparseArrayAllocator,
		typename _HashAllocator = MixedArrayAllocator<1>,
		uint32_t MinNumberOfElements = 4,
		uint32_t BucketSizeGrowDenominator = 2,
		uint32_t BucketSizeConstant = 8>
	class SetAllocator {

	public:

		static uint32_t CalculateNumberOfHashBuckets( const uint32_t count ) {
			if( count < MinNumberOfElements ) {
				return 1;
			}

			const uint32_t sqrtBucketCount = (uint32_t) ceil( sqrt( count / BucketSizeGrowDenominator + BucketSizeConstant ) );
			return sqrtBucketCount * sqrtBucketCount;
		}

		typedef _ElementArrayAllocator ElementArrayAllocator;
		typedef _HashAllocator HashAllocator;
	};

	typedef SetAllocator<> DefaultSetAllocator;
}