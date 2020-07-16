#pragma once

namespace axlt {
	template<typename... Types>
	struct LogicOr;

	template<bool Value, typename... Types>
	struct LogicOrValue {
		enum { Value = LogicOr<Types...>::Value };
	};

	template<typename... Types>
	struct LogicOrValue<true, Types...> {
		enum { Value = true };
	};

	template<typename Left, typename... Types>
	struct LogicOr<Left, Types...> : LogicOrValue<Left::Value, Types...> {};

	template<>
	struct LogicOr<> {
		enum { Value = false };
	};


	template<typename... Types>
	struct LogicAnd;

	template<bool Value, typename... Types>
	struct LogicAndValue {
		enum { Value = LogicAnd<Types...>::Value };
	};

	template<typename... Types>
	struct LogicAndValue<false, Types...> {
		enum { Value = false };
	};

	template<typename Left, typename... Types>
	struct LogicAnd<Left, Types...> : LogicAndValue<Left::Value, Types...> {};

	template<>
	struct LogicAnd<> {
		enum { Value = true };
	};
}