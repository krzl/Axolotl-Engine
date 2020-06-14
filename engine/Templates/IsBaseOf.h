#pragma once

namespace axlt {
	template<typename Base, typename Derived>
	struct IsBaseOf {
		enum { Value = __is_base_of( Base, Derived ) };
	};
}