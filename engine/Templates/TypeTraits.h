#pragma once

namespace axlt {

	template<typename T, bool PassByValue>
	struct TypeTraitsParamHelper {
		typedef const T& Type;
		typedef const T& ConstType;
	};

	template<typename T>
	struct TypeTraitsParamHelper<T, true> {
		typedef const T Type;
		typedef const T ConstType;
	};

	template<typename T>
	struct TypeTraitsParamHelper<T*, true> {
		typedef T* Type;
		typedef const T* ConstType;
	};

	template<typename Ret, typename... Args>
	struct TypeTraitsParamHelper<Ret(*)(Args...), true> {
		using FunctionType = Ret(*)( Args... );
		typedef FunctionType Type;
		typedef FunctionType ConstType;
	};

	template<typename T>
	struct TypeTraits {

		enum { PassByValue = LogicAndValue< sizeof( T ) <= sizeof( void* ), POD<T>>::Value };
		
		typedef T Value;
		typedef T& Reference;
		typedef typename TypeTraitsParamHelper<T, PassByValue>::Type ParamType;
		typedef typename TypeTraitsParamHelper<T, PassByValue>::ConstType ConstParamType;
	};
}