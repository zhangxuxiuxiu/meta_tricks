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



	// transform_x
	template< class Trans, class State, class Ts>
	struct transform_x;

	template< class Trans, class State, template<class...> class List>
	struct transform_x< Trans, State, List<> >{
		// case 1: more search required at end of list
		// case 2: no more search required at end of list
		using type = typename std::enable_if< State::ok_eof, typename State::type >::type;
	};
	
	template< class Trans, class State, template<class...> class List, class T, class... Us >
	struct transform_x< Trans, State, List<T, Us...> >{
		template<class S>
		static constexpr auto impl(...){
			return typename S::type{};
		}

		template<class S>
		static constexpr auto impl(typename std::enable_if< S::value, int>::type){
			return typename transform_x< Trans, typename Trans::template fn<S, T>::type, List<Us...> >::type{};
		}

		using type = decltype(impl<State>(0)); 
	};

	template< class Trans, class State, class... Ts>		
	using transform_x_t = typename transform_x< Trans, State, type_list<Ts...> >::type;
	
	
	template<class Base, bool cont = true, bool OkEof = true>
	struct transform_x_base : std::integral_constant<bool, cont>{
		using type = Base;	
		static constexpr bool ok_eof = OkEof;
	};

	// hof_trans to support chain multiple trans
	template< template<class> class... Trans>
	struct hof_trans;

	template<>
	struct hof_trans<>{
		template<class T>
		struct fn{
			using type = T; 
		};
	};

	template<template<class> class Tran,  template<class> class... Urans>
	struct hof_trans<Tran, Urans...>{
		template<class T>
		struct fn{
			using next_state = typename Tran<T>::type;
			using type = typename hof_trans<Urans...>::template fn<next_state>::type;
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

	// all same
	template<template<class> class... Trans>
	struct all_same_trans {
		template<class State, class T>
		struct fn{
			using next_state = struct X{ 
				using type = typename hof_trans<Trans...>::template fn<T>::type;
				static constexpr bool value =  std::is_same<typename State::type, type>::value || std::is_void<typename State::type>::value;
				static constexpr bool ok_eof = true;
			};
			// fail early
			using type = typename std::enable_if< next_state::value, next_state>::type;
		};
	};

	template<class Ts, template<class> class... Trans>
	using all_same = transform_x< all_same_trans<Trans...>, transform_x_base<void>, Ts >;

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

	// transform
	template<template<class> class... Trans>
	struct transform_trans{
		template< class State, class T>
		struct fn {
			using type = struct X : std::true_type{
				using type = typename type_list_append< typename State::type, typename hof_trans<Trans...>::template fn<T>::type >::type;	
				static constexpr bool ok_eof = true;
			};
		};
	};

	template<template<class> class Trans, class Ts>
	using transform = transform_x< transform_trans<Trans>, transform_x_base< empty_of_t<Ts> >, Ts>; 

	template<template<class> class Trans, class... Ts>
	using transform_t = typename transform< Trans, type_list<Ts...> >::type; 
	
	// nth_element

//	template<size_t N, class Ts>
//	struct nth_element;
//
//	template<class T, class... Us>
//	struct nth_element<0, type_list<T, Us...> >{
//		using type = T;
//	};
//
//	template<size_t N, class T, class... Us>
//	struct nth_element<N, type_list<T, Us...> >{
//		using type = typename nth_element< N-1, type_list<Us...> >::type;
//	};
	
	// nth_element

	template<size_t N>
	using Index = std::integral_constant<size_t, N>;

	template<size_t N, template<class> class... Trans>
	struct nth_element_trans{
		template<class State, class T>
		struct fn{
			template<size_t M>	
			static constexpr auto impl(Index<M>)  ->typename std::enable_if< (M<=N), Index<M+1> >::type  {
				return Index<M+1>{}; 	
			}

			static constexpr auto impl(...) { 
				return typename hof_trans<Trans...>::template fn<T>::type{};	
			}

			using type = struct X {
				static constexpr bool value = !std::is_same< typename State::type, Index<N+1> >::value; 
				using type = decltype( impl( typename State::type{} ) );
				static constexpr bool ok_eof = false;
			};
		};

	};
	
	template<size_t N, class Ts, template<class> class... Trans>
	using nth_element = transform_x< nth_element_trans<N, Trans...>, transform_x_base<Index<0>, true, false>, Ts >;

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
				using type = typename type_list_append< typename State::type, IndexedType< size_of<typename State::type>::value, typename hof_trans<Trans...>::template fn<T>::type > >::type;	
				static constexpr bool ok_eof = true;
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
}
