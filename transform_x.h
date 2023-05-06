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
	using emit_sfinea = std::enable_if<!S::emit>;

	template<class S>
	static constexpr bool emit(){
		return !detect<emit_sfinea, S>::value;
	}

	template<class S>
	using cont_sfinea = std::enable_if<!S::cont>;

	template<class S>
	static constexpr bool cont(){
		return !detect<cont_sfinea, S>::value;
	}

	template<class S>
	using okeof_sfinea = std::enable_if<!S::okeof>;

	template<class S>
	static constexpr bool okeof(){
		return !detect<okeof_sfinea, S>::value;
	}
	
	// transform_x
	template< class Trans, class State, class Ts>
	struct transform_x;

	template< class Trans, class State, template<class...> class List>
	struct transform_x< Trans, State, List<> >{
		// case 1: more search required at end of list
		template<class S>
		static constexpr auto impl(typename std::enable_if<!okeof<S>(), int>::type){
			return typename S::nonexist_type{}; // trigger error intentionally
		}

		// case 2: no more search required at end of list
		template<class S>
		static constexpr auto impl(...){
			return typename State::type{};
		}

		using type = decltype(impl<State>(0));
	};
	
	template< class Trans, class State, template<class...> class List, class T, class... Us >
	struct transform_x< Trans, State, List<T, Us...> >{
		// case 1: !value && !emit => return old State 
		template<class S>
		static constexpr auto impl(typename std::enable_if< !cont<S>() && !emit<S>(), int>::type){
			return typename transform_x<Trans, State, List<>>::type{};
		}
		
		// case 2: value & !S::emit => skip T, use old State to Try Us... 
		template<class S>
		static constexpr auto impl(typename std::enable_if< cont<S>() && !emit<S>(), int>::type){
			return typename transform_x< Trans, State, List<Us...> >::type{};
		}

		// case 3: !S::value & emit => stop searching to return current type 
		template<class S>
		static constexpr auto impl(typename std::enable_if< !cont<S>() && emit<S>(), int>::type){
			return typename S::type{};
		}

		// case 4: value & emit => Try Us... 
		template<class S>
		static constexpr auto impl(...){
			return typename transform_x< Trans, S, List<Us...> >::type{};
		}

		using next_state = typename Trans::template fn<State, T>::type;
		using type = decltype(impl<next_state>(0)); 
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
