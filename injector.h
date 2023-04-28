#pragma once

#include <type_traits>

#if !defined(__clang__) && defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wnon-template-friend" 
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
	struct	state_injector{ 
		constexpr friend auto state_fn(state_key<Key>){
			return state_value<State>{};
		}
	};

	template<class Key, class State, class=bool>
	struct Inject : std::integral_constant<bool, sizeof(state_injector<Key,State>) == 1>{
		using type = State;
	};

	template<class Key, class State >
	struct Inject<Key, State, decltype(state_fn(state_key<Key>{}), false)> : std::false_type{}; 

	template<class Key>
	using StateOf = typename decltype(state_fn(state_key<Key>{}))::type;
}

#if !defined(__clang__) && defined(__GNUC__)
# pragma GCC diagnositc pop
#endif
