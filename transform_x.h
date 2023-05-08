#pragma once

#include <type_traits> 	// enable_if

#include "traits.h" 	// detect 

namespace traits{

#if !defined(NO_TRANSFORM_X)
	/*
	 * typename Trans::fn<State, T>::type is a State
	 * each State has optional bool value (emit, cont, okeof) and an optional type named type
	 * if bool value (emit or value or okeof) is not defined, its value is true by default 
	 * if type is not defined, it means an error intentionally
	 * */
	template<class S>
	using emit_sfinea = typename std::enable_if<!S::emit>::type;

	template<class S>
	static constexpr bool emit(){
		return !detect<emit_sfinea, S>::value;
	}

	template<class S>
	using cont_sfinea = typename std::enable_if<!S::cont>::type;

	template<class S>
	static constexpr bool cont(){
		return !detect<cont_sfinea, S>::value;
	}

	template<class S>
	using okeof_sfinea = typename std::enable_if<!S::okeof>::type;

	template<class S>
	static constexpr bool okeof(){
		return !detect<okeof_sfinea, S>::value;
	}
	
	// transform_x
	template< class Trans, class State, class Ts>
	struct transform_x;

	template< class Trans, class State, template<class...> class List>
	struct transform_x< Trans, State, List<> >{
		template<class S, bool = !okeof<S>()>
		struct X{
			using type = typename S::nonexist_type;
		};

		template<class S>
		struct X<S, false>{
			using type = typename S::type;
		};

		using type = typename X<State>::type;
	};
	
	template< class Trans, class State, template<class...> class List, class T, class... Us >
	struct transform_x< Trans, State, List<T, Us...> >{
		// case 1: !cont && !emit => return old State 
		template<class S, bool cont = cont<S>(), bool emit = emit<S>()> // false, false
		struct X{
			using type = typename transform_x<Trans, State, List<>>::type;
		};
		
		// case 2: cont & !S::emit => skip T, use old State to Try Us... 
		template<class S>
		struct X<S, true, false>{
			using type = typename transform_x< Trans, State, List<Us...> >::type;
		};

		// case 3: !S::cont & emit => stop searching to return current type 
		template<class S>
		struct X<S, false, true>{
			using type = typename S::type;
		};

		// case 4: cont & emit => Try Us... 
		template<class S>
		struct X<S, true,true>{
			using type = typename transform_x< Trans, S, List<Us...> >::type;
		};

		using type = typename X<typename Trans::template fn<State, T>::type>::type;
	};

	template< class Trans, class BaseState, class... Ts>		
	using transform_x_t = typename transform_x< Trans, BaseState, type_list<Ts...> >::type;
	
	template<class Base, bool Emit=true, bool OkEof = true>
	struct transform_x_base {
		using type = Base;	
		static constexpr bool emit  = Emit;
		static constexpr bool okeof = OkEof;
	};

	template<bool Emit=false, bool Okeof=true>
	struct emitter { 
		static constexpr bool emit  = Emit;
		static constexpr bool okeof = Okeof;
	};
#endif

}
