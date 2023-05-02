#pragma once

#include <type_traits>

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

	// nth_element
	using std::size_t;

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

	template<size_t N, class... Ts>
	using nth_element_t = typename nth_element< N, type_list<Ts...> >::type;


	// transform_x
	template< class Trans, class State, class Ts>
	struct transform_x;

	template< class Trans, class State, template<class...> class List>
	struct transform_x< Trans, State, List<> >{
		using type = typename State::type;
	};
	
	template< class Trans, class State, template<class...> class List, class T, class... Us >
	struct transform_x< Trans, State, List<T, Us...> >{
		using next_state = typename Trans::template fn<State, T>::type;
		using type = typename std::conditional< next_state::value,  typename transform_x< Trans, next_state, List<Us...> >::type, typename next_state::type >::type;
	};

	template< class Trans, class State, class... Ts>		
	using transform_x_t = typename transform_x< Trans, State, type_list<Ts...> >::type;
	
	
	template<class Base>
	struct transform_x_base : std::true_type{
		using type = Base;	
	};

	// hof_trans to support chain multiple trans
	template< class... Trans>
	struct hof_trans;

	template<class Tran>
	struct hof_trans<Tran>{
		template<class State, class T>
		struct fn{
			using type = typename Tran::template fn<State, T>::type;
		};
	};

	template<class Tran,  class... Urans>
	struct hof_trans<Tran, Urans...>{
		template<class State, class T>
		struct fn{
			using next_state = typename Tran::template fn<State, T>::type;
			using type = typename std::enable_if< next_state::value, typename hof_trans<Urans...>::template fn<next_state, T>::type  >::type;
		};

	};

	// all_same
//	template<class Ts>
//	struct all_same;
//
//	template<template<class...> class List, class T>
//	struct all_same< List<T> >{
//		using type = T;
//	};
//	
//	template< template<class...> class List, class T, class... Us>
//	struct all_same< List<T, T, Us...> >{
//		using type = typename all_same< List<T, Us...> >::type;
//	};
//
//	template<class... Ts>
//	using all_same_t = typename all_same< type_list<Ts...> >::type;

	// all same
	struct all_same_trans {
		template<class State, class T>
		struct fn{
			using next_state = struct X{ 
				static constexpr bool value =  std::is_same<typename State::type, T>::value || std::is_void<typename State::type>::value;
				using type = T;
			};
			// fail early
			using type = typename std::enable_if< next_state::value, next_state>::type;
		};
	};

	template<class Ts>
	using all_same = transform_x< all_same_trans, transform_x_base<void>, Ts >;

	template<class... Ts>
	using all_same_t = typename all_same< type_list<Ts...> >::type;

	// transform 
//	template< template< class > class Trans, class Ts>
//	struct transform;
//	
//	template< template< class > class Trans, template<class...> class List, class... Ts >
//	struct transform< Trans, List<Ts...> >{
//		using type = List<typename Trans<Ts>::type...>;
//	};
//
//	template< template<class> class Trans, class... Ts>		
//	using transform_t = typename transform< Trans, type_list<Ts...> >::type;

	// transform
	template<template<class> class Trans>
	struct transform_trans{
		template< class State, class T>
		struct fn {
			using type = struct X : std::true_type{
				using type = typename type_list_append< typename State::type, typename Trans<T>::type >::type;	
			};
		};
	};

	template<template<class> class Trans, class Ts>
	using transform = transform_x< transform_trans<Trans>, transform_x_base< empty_of_t<Ts> >, Ts>; 

	template<template<class> class Trans, class... Ts>
	using transform_t = typename transform< Trans, type_list<Ts...> >::type; 
	

	// identity
	template<class T>
	struct identity{
		using type = T;
	};
}
