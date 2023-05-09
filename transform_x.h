#pragma once

#include <type_traits> 	// enable_if

#include "traits.h" 	// detect 

namespace traits{

	/*
	 * typename Trans::fn<State, T>::type is a State
	 * each State has optional bool value (emit, cont, okeof) and an optional type named type
	 * if bool value (emit or value or okeof) is not defined, its value is true by default 
	 * if type is not defined, it means an error intentionally
	 * */
#define TRANS_STATE_VALUE(name) 						\
	template<class S>							\
	using name##_sfinea = typename std::enable_if<!S::name>::type;		\
										\
	template<class S>							\
	static constexpr bool name(){						\
		return !detect<name##_sfinea, S>::value;			\
	}									\

	TRANS_STATE_VALUE(emit)
	TRANS_STATE_VALUE(cont)
	TRANS_STATE_VALUE(okeof)
#undef TRANS_STATE_VALUE	

	// transform_x
	template< class Trans, class State, class Ts>
	struct transform_x;

	template< class Trans, class State, template<class...> class List>
	struct transform_x< Trans, State, List<> > : std::enable_if<okeof<State>(), State>::type{};
	
	template< class Trans, class State, template<class...> class List, class T, class... Us >
	struct transform_x< Trans, State, List<T, Us...> >{
		// case 1: !cont && !emit => return old State 
		template<class S, bool cont = cont<S>(), bool emit = emit<S>()> // false, false
		struct X : transform_x<Trans, State, List<>>{};
		
		// case 2: cont & !S::emit => skip T, use old State to Try Us... 
		template<class S>
		struct X<S, true, false> : transform_x< Trans, State, List<Us...> >{};

		// case 3: !S::cont & emit => stop searching to return current type 
		template<class S>
		struct X<S, false, true> : S{};

		// case 4: cont & emit => Try Us... 
		template<class S>
		struct X<S, true,true> : transform_x< Trans, S, List<Us...> >{};

		using type = typename X<typename Trans::template fn<State, T>::type>::type;
	};

	template< class Trans, class BaseState, class... Ts>		
	using transform_x_t = typename transform_x< Trans, BaseState, type_list<Ts...> >::type;
	
	template<class Base, bool Emit=true, bool OkEof = true, bool Cont = true>
	struct transform_x_state {
		using type = Base;	
		static constexpr bool emit  = Emit;
		static constexpr bool okeof = OkEof;
		static constexpr bool cont  = Cont;
	};
}
