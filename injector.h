#pragma once

#include <type_traits>

#if !defined(__clang__) && defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wnon-template-friend" 
#endif

// to resolve that decltype([]{}) will be cached as nested member's default template parameter in clang++ with std=c++20
#if __cplusplus >=202002L
	#define DeclareUniqueTag(Tag) auto Tag = []{}
	#define UniqueTag []{}
	#define UseTag(Tag) decltype(Tag)
#else
	#define DeclareUniqueTag(Tag) class Tag 
	#define UniqueTag std::integral_constant<int, __COUNTER__> 
	#define UseTag(Tag) Tag
#endif

namespace injector{

	template<class Key> 
	struct state_key {
		constexpr friend auto state_fn(state_key);
	};

	template<class State>
	struct state_value{
		using type = State;
	};

	template<class Key, class State> 
	struct state_injector{ 
		constexpr friend auto state_fn(state_key<Key>){
			return state_value<State>{};
		}
	};

	template<class Key, class State, class = bool>
	struct Inject : std::integral_constant<bool, sizeof(state_injector<Key,State>) == 1>{
		using type = State;
	};

	template<class Key, class State >
	struct Inject<Key, State, decltype(state_fn(state_key<Key>{}), true)> : std::false_type{}; 

	// no use "bool=false" to avoid error that type of specialized non-type template argument depends on a template parameter of the partial specialization 
	template<class Key, class EvalTag, class = bool>
	struct has_state : std::false_type{};

	template<class Key, class EvalTag>
	struct has_state<Key, EvalTag, decltype(state_fn(state_key<Key>{}), false)> : std::true_type{}; 

	// indirect to avoid error that default template arguments may not be used in partial specializations
	template<class Key, DeclareUniqueTag(EvalTag)>
	using HasState = has_state<Key, UseTag(EvalTag)>;

	template<class Key>
	using StateOf = typename decltype(state_fn(state_key<Key>{}))::type;
}

#if !defined(__clang__) && defined(__GNUC__)
# pragma GCC diagnositc pop
#endif
