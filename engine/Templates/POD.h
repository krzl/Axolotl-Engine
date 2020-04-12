#pragma once

namespace axlt {
	template<typename T>
	struct POD {
		enum { Value = __is_pod( T ) };
	};
}