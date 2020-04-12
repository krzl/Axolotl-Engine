#pragma once

namespace axlt {
	template<bool Predicate, typename Result = void>
	struct EnableIf;

	template<typename Result>
	struct EnableIf<true, Result> {
	public:
		typedef Result Type;
	};

	template<typename Result>
	struct EnableIf<false, Result> {};
}
