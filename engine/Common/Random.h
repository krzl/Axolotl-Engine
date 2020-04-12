#pragma once

namespace axlt {

	template<typename T>
	T RandomNumber();

	template uint64_t RandomNumber();
	template uint32_t RandomNumber();
	template uint16_t RandomNumber();
	template uint8_t RandomNumber();
	
	template int64_t RandomNumber();
	template int32_t RandomNumber();
	template int16_t RandomNumber();
	template int8_t RandomNumber();
}