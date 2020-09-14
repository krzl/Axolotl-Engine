#pragma once

namespace axlt {
	template<typename T>
	struct IsEnum {
		enum { Value = __is_enum( T ) };
	};
}