#pragma once

#include "TemplateBoolLogic.h"
#include "TriviallyCopyable.h"
#include "TriviallyDestructible.h"
#include "EqualTypes.h"

namespace axlt {
	template< typename T, typename... Types >
	struct Relocatable {};
	
	template<typename Dest, typename Src>
	struct Relocatable<Dest, Src> :
		LogicOr<
			EqualTypes<Dest, Src>,
			LogicAnd<
				TriviallyCopyable<Dest,Src>,
				TriviallyDestructible<Src>
			>
		>
	{};

	template<typename Type>
	struct Relocatable<Type, Type> :
		LogicAnd<
			TriviallyCopyable<Type,Type>,
			TriviallyDestructible<Type>
		>
	{};
}