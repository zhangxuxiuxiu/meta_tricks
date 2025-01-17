#pragma once

#include <type_traits> 	// enable_if, integral_constant, is_same

#include "type_list.h"
#include "stateless_trans.h"
#include "transform_x.h"
#include "traits.h"

namespace traits{

	//typename Stateless::template fn<T>::type|value 
	//typename Trans::template fn<S, T>::type
	template<class StatefulTrans, class StatelessTrans> 
	struct stateful_trans{
		template<class State, class T>
		struct fn{
			// case 2: Tran is a filter and filter return true, then stateless_trans_generic return Tn that Tn::type==T 
			// case 3: Tran is a normal transformation
			template<class Tn, class = void>
			struct X : StatefulTrans::template fn< State, typename Tn::type >::type{};
			
			// case 1: Tran is a filter, filter return false
			template<class Tn>
			struct X< Tn, typename std::enable_if<!Tn::value>::type > : transform_x_state<void, false, okeof<State>()>{};

			using type = X< typename StatelessTrans::template fn<T> >; 
		};
	};	

	// all same
	struct all_same_trans {
		template<class State, class T>
		struct fn{
			static constexpr bool cont = std::is_same<typename State::type, T>::value || !emit<State>();
			using type = transform_x_state<T, cont, cont, cont>; 
		};
	};

	template<class Ts, template<class> class... Trans>
	using all_same = transform_x< stateful_trans< all_same_trans, stateless_trans_filter_z<Trans...> >, transform_x_state<void, false, false>, Ts >; // false to trigger compilation error on empty list

	template<class... Ts>
	using all_same_t = typename all_same< type_list<Ts...> >::type;

	// transform
	struct transform_trans{
		template< class State, class T>
		struct fn : sub_type < type_list_append< typename State::type, T > > {};	
	};

	template<class Ts, template<class> class... Trans>
	using transform = transform_x< stateful_trans< transform_trans, stateless_trans_filter_z<Trans...> >, transform_x_state< empty_of_t<Ts> >, Ts>; 

	template<template<class> class Trans, class... Ts>
	using transform_t = typename transform< type_list<Ts...>, Trans >::type; 
	
	// nth_element
	template<size_t N>
	struct nth_element_trans{
		template<class State, class T>
		struct fn{
			static constexpr size_t M = State::type::value;
			using type = transform_x_state< typename std::conditional<(M<N), Index<M+1>, T>::type, true, false, (M<N) >;
		};
	};

	template<size_t N, class Ts, template<class> class... Trans>
	using nth_element = transform_x< stateful_trans< nth_element_trans<N>, stateless_trans_filter_z<Trans...> >, transform_x_state<Index<0>, false, false>, Ts >;

	template<size_t N, class... Ts>
	using nth_element_t = typename nth_element< N, type_list<Ts...> >::type;

	// index type list
	template<size_t N, class T>
	struct IndexedType{};

	struct index_trans{
		template< class State, class T>
		struct fn : sub_type < type_list_append< typename State::type, IndexedType< size_of<typename State::type>::value, T > > > {};	
	};

	template<class Ts, template<class> class... Trans>
	using index_types = transform_x< stateful_trans< index_trans, stateless_trans_filter_z<Trans...> >, transform_x_state<empty_of_t<Ts>>, Ts >;

	template<class... Ts>
	using index_types_t = typename index_types< type_list<Ts...> >::type;
}
