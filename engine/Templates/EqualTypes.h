#pragma once

namespace axlt {
	template<typename A, typename B>
	struct EqualTypes;

	template<typename A, typename B>
	struct EqualTypes {
		enum { Value = false };
	};

	template<typename A>
	struct EqualTypes<A, A> {
		enum { Value = true };
	};
}