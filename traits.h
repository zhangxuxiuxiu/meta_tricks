#pragma once

#include <type_traits>
#include <utility>

#include "injector.h"

namespace traits{
	// list operations
	template<typename...>
	struct type_list {};

	template<class Ts>
	struct empty_of;

	template<template<class...> class List, class... Ts>
	struct empty_of<List<Ts...>>{
		using type = List<>;
	};

	template<class Ts>
	using empty_of_t = typename empty_of<Ts>::type;

	template<class Ts>
	struct size_of;

	using std::size_t;

	template<template<class...> class List, class... Ts>
	struct size_of<List<Ts...>> : std::integral_constant< size_t, sizeof...(Ts) >{};

	template<class Ts, class Us>
	struct type_list_concat;

	template<template<class...> class List, typename... Ts, typename... Us>
	struct type_list_concat<List<Ts...>, List<Us...>> {
		using type = List<Ts..., Us...>;
	};

	template<class Ts, class... Args>
	struct type_list_append;

	template<template<class...> class List, class... Ts, class... Args>
	struct type_list_append<List<Ts...>, Args...>{
      		using type = List<Ts..., Args...>;
	};

	template<class Ts>
	struct type_list_pop;

	template<template<class...> class List, class T>
	struct type_list_pop<List<T>>{
		using type = List<>;
	};

	template<template<class...> class List, class U, class... Ts>
	struct type_list_pop<List<U, Ts...>>{
		using type = typename type_list_concat< List<U>, typename type_list_pop<List<Ts...>>::type >::type;
	};


#if !defined(NO_TRANSFORM_X)
	/*
	 * typename Trans::fn<State, T>::type is a State
	 * each State has optional bool value (emit, value, okeof) and an optional type named type
	 * if bool value (emit or value or okeof) is not defined, its value is true by default 
	 * if type is not defined, it means an error intentionally
	 * */
	// transform_x
	template< class Trans, class State, class Ts>
	struct transform_x;

	template< class Trans, class State, template<class...> class List>
	struct transform_x< Trans, State, List<> >{
		// case 1: more search required at end of list
		template<class S>
		static constexpr auto impl(typename std::enable_if<!S::okeof, int>::type){
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
		// case 1: !S::emit => skip T, use old State to Try Us... 
		template<class S>
		static constexpr auto impl(typename std::enable_if< !S::emit, int>::type){
			return typename transform_x< Trans, State , List<Us...> >::type{};
		}

		// case 2: (!defined(S::emit){ie: emit=true} || S::emit) & !S::value => stop searching to return current type 
		template<class S>
		static constexpr auto impl(typename std::enable_if< !S::value, float>::type){
			return typename S::type{};
		}

		// case 3: (!defined(S::emit){ie: emit=true} || S::emit) & (!defined(S::value){ie: value=true} || S::value) => Try Us... 
		template<class S>
		static constexpr auto impl(...){
			return typename transform_x< Trans, S , List<Us...> >::type{};
		}

		using next_state = typename Trans::template fn<State, T>::type;
		using type = decltype(impl<next_state>(0)); 
	};

	template< class Trans, class State, class... Ts>		
	using transform_x_t = typename transform_x< Trans, State, type_list<Ts...> >::type;
	
	template<class Base, bool OkEof = true>
	struct transform_x_base {
		using type = Base;	
		static constexpr bool okeof = OkEof;
	};

	// hof_trans to support chain multiple trans
	template< template<class> class... Trans>
	struct hof_trans;

	template<>
	struct hof_trans<>{
		template<class T>
		struct fn{
			using type = struct X {
				using type = T;
			};
		};
	};

	template<bool B>
	struct emitter { //: std::true_type {
		static constexpr bool emit = B;
	};

	template<template<class> class Tran,  template<class> class... Urans>
	struct hof_trans<Tran, Urans...>{
		template<class T>
		struct fn{
			// Tran is a filter, filter return false
			template<class Tn>
			static constexpr auto impl(typename std::enable_if<!Tn::value,int>::type){
				return emitter<false>{};
			}

			// Tran is a filter, filter return true 
			template<class Tn>
			static constexpr auto impl(typename std::enable_if<Tn::value,float>::type){
				return typename hof_trans<Urans...>::template fn<T>::type{};
			}

			// Tran is a normal transformation
			template<class Tn>
			static constexpr auto impl(...){
				return typename hof_trans<Urans...>::template fn<typename Tn::type>::type{};
			}

			using type = decltype(impl<Tran<T>>(0)); 
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
	
	template< template<class...> class List, class T, class... Us>
	struct all_same< List<T, T, Us...> >{
		using type = typename all_same< List<T, Us...> >::type;
	};

#else
	// all same
	using all_same_default_base = UseTag(UniqueTag);

	template<template<class> class... Trans>
	struct all_same_trans {
		template<class State, class T>
		struct fn{
			using next_state = struct X{ 
				using type = typename hof_trans<Trans...>::template fn<T>::type::type;
				static constexpr bool value =  std::is_same<typename State::type, type>::value || std::is_same<typename State::type, all_same_default_base>::value;
				static constexpr bool okeof = true;
			};
			// fail early
			using type = typename std::enable_if< next_state::value, next_state>::type;
		};
	};

	template<class Ts, template<class> class... Trans>
	using all_same = transform_x< all_same_trans<Trans...>, transform_x_base<all_same_default_base, false>, Ts >; // false to trigger compilation error on empty list

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
	template<template<class> class... Trans>
	struct transform_trans{
		template< class State, class T>
		struct fn {
			using type = struct X : std::true_type{
				using type = typename type_list_append< typename State::type, typename hof_trans<Trans...>::template fn<T>::type::type >::type;	
				static constexpr bool okeof = true;
			};
		};
	};

	template<template<class> class Trans, class Ts>
	using transform = transform_x< transform_trans<Trans>, transform_x_base< empty_of_t<Ts> >, Ts>; 

#endif
	template<template<class> class Trans, class... Ts>
	using transform_t = typename transform< Trans, type_list<Ts...> >::type; 
	
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

	template<size_t N, template<class> class... Trans>
	struct nth_element_trans{
		template<class State, class T>
		struct fn{
			template<size_t M>	
			static constexpr auto impl(typename std::enable_if< (M<=N), Index<M>>::type) {
				return std::make_pair(true,Index<M+1>{}); 	
			}

			static constexpr auto impl(...) { 
				return std::make_pair(false, typename hof_trans<Trans...>::template fn<T>::type::type{});	
			}

			using type = struct X {
				static constexpr auto btype = impl( typename State::type{} );
				using type = decltype(btype.second);
				static constexpr bool value = btype.first; 
				static constexpr bool okeof = false;
			};
		};

	};
	
	template<size_t N, class Ts, template<class> class... Trans>
	using nth_element = transform_x< nth_element_trans<N, Trans...>, transform_x_base<Index<0>, false>, Ts >;

#endif
	template<size_t N, class... Ts>
	using nth_element_t = typename nth_element< N, type_list<Ts...> >::type;

	// index type list
	template<size_t N, class T>
	struct IndexedType{};

	template<template<class> class... Trans>
	struct index_trans{
		template< class State, class T>
		struct fn {
			using type = struct X : std::true_type{
				using type = typename type_list_append< typename State::type, IndexedType< size_of<typename State::type>::value, typename hof_trans<Trans...>::template fn<T>::type::type > >::type;	
				static constexpr bool okeof = true;
			};
		};
	};

	template<class Ts, template<class> class... Trans>
	using index_list = transform_x< index_trans<Trans...>, transform_x_base<type_list<>>, Ts >;

	template<class... Ts>
	using index_list_t = typename index_list< type_list<Ts...> >::type;

	// identity
	template<class T>
	struct identity{
		using type = T;
	};

	template<class FPtr>
	struct member_host;
	
	template<class R, class T>
	struct member_host<R T::*>{
		using type = T; 
	};

	template<class R, class T, class... Args>
	struct member_host<R T::*(Args...)>{
		using type = T; 
	};

	// index sequence operations
	template<class, class>
	struct index_concat;

	template<size_t... Is, size_t... Js>
	struct index_concat<std::index_sequence<Is...>, std::index_sequence<Js...>>{
		using type = std::index_sequence<Is..., Js...>;
	};

	template<class Seq>
	struct index_push;

	template<size_t... Is>
	struct index_push<std::index_sequence<Is...>>{
		using type = std::index_sequence<Is..., sizeof...(Is)>;
	};

	template<class Seq>
	struct index_pop;

	template<size_t J>
	struct index_pop<std::index_sequence<J>>{
		using type = std::index_sequence<>;
	};

	template<size_t J, size_t... Is>
	struct index_pop<std::index_sequence<J, Is...>>{
		using type = typename index_concat<std::index_sequence<J>, typename index_pop<std::index_sequence<Is...>>::type >::type;
	};
}
