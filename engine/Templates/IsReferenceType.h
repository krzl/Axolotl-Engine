#pragma once

namespace axlt {
	template<typename T>
	struct IsReferenceType {
		enum { Value = false };
	};

	template<typename T>
	struct IsReferenceType<T&> {
		enum { Value = true };
	};
}