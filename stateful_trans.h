#pragma once

#include <type_traits> 	// enable_if, integral_constant, is_same

#include "type_list.h"
#include "stateless_trans.h"
#include "transform_x.h"

namespace traits{

#if !defined(NO_TRANSFORM_X)
	//typename Stateless::template fn<T>::type 
	//typename Trans::template fn<S, T>::type
	template<class StatefulTrans, class StatelessTrans> 
	struct stateful_trans{
		template<class State, class T>
		struct fn{
			// case 1: Tran is a filter, filter return false
			template<class Tn, class = void>
			struct X : StatefulTrans::template fn< State, typename Tn::type >::type{};

			// case 2: Tran is a filter and filter return true, then stateless_trans_generic return Tn that Tn::type==T 
			// case 3: Tran is a normal transformation
			template<class Tn>
			struct X< Tn, typename std::enable_if<!Tn::value>::type > : emitter<false, okeof<State>()>{};

			using type = X< typename StatelessTrans::template fn<T> >; 
		};
	};	
#endif

#ifdef NO_TRANSFORM_X
	// all_same
	template<class Ts>
	struct all_same;

	template<template<class...> class List, class T>
	struct all_same< List<T> >{
		using type = T;
	};
	
	{template< template<class...> class List, class T, class... Us>
	struct all_same< List<T, T, Us...> >{
		using type = typename all_same< List<T, Us...> >::type;
	};

#else
	// all same
	struct all_same_trans {
		template<class State, class T>
		struct fn{
			using next_state = struct X{ 
				using type = T; 
				static constexpr bool cont = std::is_same<typename State::type, type>::value || !emit<State>(); 
			};
			// fail early
			using type = typename std::enable_if< next_state::cont, next_state>::type;
		};
	};

	template<class Ts, template<class> class... Trans>
	using all_same = transform_x< stateful_trans< all_same_trans, stateless_trans_filter_z<Trans...> >, transform_x_base<void, false, false>, Ts >; // false to trigger compilation error on empty list

#endif
	template<class... Ts>
	using all_same_t = typename all_same< type_list<Ts...> >::type;

#ifdef NO_TRANSFORM_X
	// transform 
	template< template< class > class Trans, class Ts>
	struct transform;
	
	template< template< class > class Trans, template<class...> class List, class... Ts >
	struct transform< Trans, List<Ts...> >{
		using type = List<typename Trans<Ts>::type...>;
	};

#else
	// transform
	struct transform_trans{
		template< class State, class T>
		struct fn {
			using type = type_list_append< typename State::type, T >;	
		};
	};

	template<class Ts, template<class> class... Trans>
	using transform = transform_x< stateful_trans< transform_trans, stateless_trans_filter_z<Trans...> >, transform_x_base< empty_of_t<Ts> >, Ts>; 

#endif
	template<template<class> class Trans, class... Ts>
	using transform_t = typename transform< type_list<Ts...>, Trans >::type; 
	
#ifdef NO_TRANSFORM_X
	// nth_element
	template<size_t N, class Ts>
	struct nth_element;

	template<class T, class... Us>
	struct nth_element<0, type_list<T, Us...> >{
		using type = T;
	};

	template<size_t N, class T, class... Us>
	struct nth_element<N, type_list<T, Us...> >{
		using type = typename nth_element< N-1, type_list<Us...> >::type;
	};
	
#else
	// nth_element
	template<size_t N>
	using Index = std::integral_constant<size_t, N>;

	template<size_t N>
	struct nth_element_trans{
		template<class State, class T>
		struct fn{
			template<class, class=void>
			struct Z : std::false_type {
				using type = T;
			};

			template<size_t M>	
			struct Z< Index<M>, typename std::enable_if<(M<N)>::type > : std::true_type{
				using type = Index<M+1>; 	
			};

			using type = struct X {
				using sfinae = Z<typename State::type>;
				using type = typename sfinae::type;
				static constexpr bool cont  = sfinae::value; 
				static constexpr bool okeof = false;
			};
		};
	};

	template<size_t N, class Ts, template<class> class... Trans>
	using nth_element = transform_x< stateful_trans< nth_element_trans<N>, stateless_trans_filter_z<Trans...> >, transform_x_base<Index<0>, false, false>, Ts >;

#endif
	template<size_t N, class... Ts>
	using nth_element_t = typename nth_element< N, type_list<Ts...> >::type;

	// index type list
	template<size_t N, class T>
	struct IndexedType{};

	struct index_trans{
		template< class State, class T>
		struct fn {
			using type = type_list_append< typename State::type, IndexedType< size_of<typename State::type>::value, T > >;	
		};
	};

	template<class Ts, template<class> class... Trans>
	using index_list = transform_x< stateful_trans< index_trans, stateless_trans_filter_z<Trans...> >, transform_x_base<empty_of_t<Ts>>, Ts >;

	template<class... Ts>
	using index_list_t = typename index_list< type_list<Ts...> >::type;
}
