#pragma once

namespace axlt {
	template<typename T>
	struct TriviallyDestructible {
		enum { Value = __has_trivial_destructor( T ) };
	};
}