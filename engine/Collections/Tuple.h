#pragma once

#include <cstdint>

#include "Templates/IntegerSequence.h"

namespace axlt {
	namespace Hidden_Tuple {

		template<uint32_t Index, typename T>
		struct TupleElement {

			template<typename... ArgsType>
			explicit TupleElement( ArgsType&&... args ) : element( Forward<ArgsType>( args )... ) {}

			TupleElement() = default;
			TupleElement( const TupleElement& ) = default;
			TupleElement( TupleElement&& ) = default;
			TupleElement& operator=( const TupleElement& ) = default;
			TupleElement& operator=( TupleElement&& ) = default;
			~TupleElement() = default;

			T element;
		};

		template<uint32_t CurrIndex, uint32_t Index, typename... Types>
		struct TupleElementExtractorImpl;

		template<uint32_t CurrIndex, uint32_t Index, typename ElementType, typename... Types>
		struct TupleElementExtractorImpl<CurrIndex, Index, ElementType, Types...> : TupleElementExtractorImpl<CurrIndex + 1, Index, Types...> {};

		template<uint32_t Index, typename ElementType, typename... Types>
		struct TupleElementExtractorImpl<Index, Index, ElementType, Types...> {

			typedef ElementType Type;

			template<typename TupleType>
			static ElementType& Get( TupleType& tuple ) {
				return static_cast<TupleElement<Index, ElementType>&>( tuple ).element;
			}

			template<typename TupleType>
			static const ElementType& Get( const TupleType& tuple ) {
				return static_cast<TupleElement<Index, ElementType>&>( tuple ).element;
			}
		};

		template<uint32_t Index, typename... Types>
		struct TupleElementExtractor : TupleElementExtractorImpl<0, Index, Types...> {};

		template<uint32_t Index, typename Pair>
		struct PairElementExtractor;

		template<typename Pair>
		struct PairElementExtractor<0, Pair> {

			typedef typename Pair::KeyType Type;

			static Type& Get( Pair& tuple ) {
				return tuple.key;
			}

			static const Type& Get( const Pair& tuple ) {
				return tuple.key;
			}
		};

		template<typename Pair>
		struct PairElementExtractor<1, Pair> {

			typedef typename Pair::ValueType Type;

			static Type& Get( Pair& tuple ) {
				return tuple.value;
			}

			static const Type& Get( const Pair& tuple ) {
				return tuple.value;
			}
		};

		template<typename Indices, typename... Types>
		struct TupleImpl;

		template<uint32_t... Indices, typename... Types>
		struct TupleImpl<IntegerSequence<uint32_t, Indices...>, Types...> : TupleElement<Indices, Types>... {

			template<typename... ArgsType>
			explicit TupleImpl( ArgsType&&... args ) : TupleElement<Indices, Types>( Forward<ArgsType>( args ) )... {}

			TupleImpl() = default;
			TupleImpl( const TupleImpl& ) = default;
			TupleImpl( TupleImpl&& ) = default;
			TupleImpl& operator=( const TupleImpl& ) = default;
			TupleImpl& operator=( TupleImpl&& ) = default;
			~TupleImpl() = default;

			template<uint32_t Index>
			typename TupleElementExtractor<Index, Types...>::Type& Get() {
				return TupleElementExtractor<Index, Types...>::Get( *this );
			}

			template<uint32_t Index>
			const typename TupleElementExtractor<Index, Types...>::Type& Get() const {
				return TupleElementExtractor<Index, Types...>::Get( *this );
			}

			template<typename F>
			void ForEach( F func ) {
				ForEachImpl( *this, func, MakeIntegerSequence<uint32_t, sizeof...( Types )>() );
			}

		public:

			// STRUCTURIZED BINDING
			// 
			// ReSharper disable once CppInconsistentNaming
			template<uint32_t Index>
			typename TupleElementExtractor<Index, Types...>::Type& get() {
				return TupleElementExtractor<Index, Types...>::Get( *this );
			}

			// ReSharper disable once CppInconsistentNaming
			template<uint32_t Index>
			const typename TupleElementExtractor<Index, Types...>::Type& get() const {
				return TupleElementExtractor<Index, Types...>::Get( *this );
			}
		};

		template<typename T, typename F, uint32_t... Is>
		void ForEachImpl( T& tuple, F func, IntegerSequence<uint32_t, Is...> seq ) {
			auto l = { ( func( tuple.template Get<Is>() ), 0 )... };
		}

		template<typename _KeyType, typename _ValueType>
		struct TupleImpl<IntegerSequence<uint32_t, 0, 1>, _KeyType, _ValueType> {

			typedef _KeyType KeyType;
			typedef _ValueType ValueType;

			template<typename KeyParamType, typename ValueParamType>
			TupleImpl( KeyParamType key, ValueParamType value ) : key( Forward<KeyParamType>( key ) ), value( Forward<ValueParamType>( value ) ) {}

			TupleImpl() = default;
			TupleImpl( const TupleImpl& ) = default;
			TupleImpl( TupleImpl&& ) = default;
			TupleImpl& operator=( const TupleImpl& ) = default;
			TupleImpl& operator=( TupleImpl&& ) = default;
			~TupleImpl() = default;

			template<uint32_t Index>
			typename PairElementExtractor<Index, TupleImpl>::Type& Get() {
				return PairElementExtractor<Index, TupleImpl>::Get( *this );
			}

			template<uint32_t Index>
			const typename PairElementExtractor<Index, TupleImpl>::Type& Get() const {
				return PairElementExtractor<Index, TupleImpl>::Get( *this );
			}

			template<typename F>
			void ForEach( F func ) {
				func( key );
				func( value );
			}

			KeyType key;
			ValueType value;

			// STRUCTURIZED BINDING
			// 
			// ReSharper disable once CppInconsistentNaming
			template<uint32_t Index>
			typename PairElementExtractor<Index, TupleImpl>::Type& get() {
				return PairElementExtractor<Index, TupleImpl>::Get( *this );
			}

			// ReSharper disable once CppInconsistentNaming
			template<uint32_t Index>
			const typename PairElementExtractor<Index, TupleImpl>::Type& get() const {
				return PairElementExtractor<Index, TupleImpl>::Get( *this );
			}
		};
	}

	template<typename... Types>
	struct Tuple : Hidden_Tuple::TupleImpl<MakeIntegerSequence<uint32_t, sizeof...( Types )>, Types...> {

		typedef Hidden_Tuple::TupleImpl<MakeIntegerSequence<uint32_t, sizeof...( Types )>, Types...> BaseType;

		template<typename... ArgsType>
		explicit Tuple( ArgsType&&... args ) : BaseType( Forward<ArgsType>( args )... ) {}

		Tuple() = default;
		Tuple( const Tuple& ) = default;
		Tuple( Tuple&& ) = default;
		Tuple& operator=( const Tuple& ) = default;
		Tuple& operator=( Tuple&& ) = default;
		~Tuple() = default;
	};

	template<typename Key, typename Value>
	using Pair = Tuple<Key, Value>;

	template<typename... Types>
	inline uint32_t GetHash( const Tuple<Types...>& tuple ) {
		uint32_t seed = 0;
		Tuple<Types...>& tupleNoConst = const_cast<Tuple<Types...>&>( tuple );
		tupleNoConst.ForEach( [ &seed ]( auto& x ) { HashCombine( seed, x ); } );
		return seed;
	}
}


// STRUCTURIZED BINDING

// ReSharper disable once CppInconsistentNaming
namespace std {  // NOLINT(cert-dcl58-cpp)
	template<typename Tuple>
	struct tuple_size;

	template<typename... Types>
	struct tuple_size<axlt::Tuple<Types...>> {
		static const std::size_t value = sizeof...( Types );
	};

	template<uint64_t I, typename Tuple>
	struct tuple_element;

	template<uint64_t I, typename... Types>
	struct tuple_element<I, axlt::Tuple<Types...>> {
		// ReSharper disable once CppInconsistentNaming
		using type = typename axlt::Hidden_Tuple::TupleElementExtractor<I, Types...>::Type;

	};
}

