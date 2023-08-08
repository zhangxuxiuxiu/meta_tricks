#pragma once

#include <type_traits> 	// integral_constant
#include <utility>	// index_sequence

#include "traits.h"

namespace traits{

	// list operations
	template<typename...>
	struct type_list {};

	// empty_of
	template<class Ts>
	struct empty_of;

	template<template<class...> class List, class... Ts>
	struct empty_of<List<Ts...>> : sub_type< List<> >{};

	template<class Ts>
	using empty_of_t = typename empty_of<Ts>::type;

	// size_of
	template<class Ts>
	struct size_of;

	using std::size_t;

	template<template<class...> class List, class... Ts>
	struct size_of<List<Ts...>> : std::integral_constant< size_t, sizeof...(Ts) >{};

	// type_list_concat
	template<class Ts, class Us>
	struct type_list_concat;

	template<template<class...> class List1,template<class...> class List2, typename... Ts, typename... Us>
	struct type_list_concat<List1<Ts...>, List2<Us...>> : sub_type < List1<Ts..., Us...> > {};

	// type_list_append
	template<class Ts, class... Args>
	using type_list_append = type_list_concat<Ts, type_list<Args...>>;

	// type_list_pop
	template<class Ts>
	struct type_list_pop;

	template<template<class...> class List, class T>
	struct type_list_pop< List<T> > : sub_type < List<> >{};

	template<template<class...> class List, class U, class... Ts>
	struct type_list_pop<List<U, Ts...>> : type_list_concat< List<U>, typename type_list_pop<List<Ts...>>::type >{};

	// index_of
	template<class Seq, class T>
	struct index_of;
	
	template<template<class...> class List, class T>
	struct index_of<List<>,T> : std::integral_constant<int, -1>{};
	
	template<template<class...> class List, class T, class U, class... Ks>
	struct index_of<List<U, Ks...>,T> : std::integral_constant<int, std::is_same<T,U>::value?0:(index_of<List<Ks...>,T>::value==-1?-1:1+index_of<List<Ks...>,T>::value)>{};

	// index_list
	template<size_t Index, class T>
	struct indexed_type{
		static constexpr size_t index= Index;
		using type = T;
	};

	template<class List, size_t Start=0>
	struct index_list;	
	
	template<template<class...>class List, size_t Start>	
	struct index_list<List<>, Start> : sub_type<List<>>{};

	template<template<class...>class List, size_t Start, class T, class... Us >	
	struct index_list<List<T, Us...>, Start> : type_list_concat< List<indexed_type<Start,T>>, typename index_list<List<Us...>, Start+1>::type >{};

	// list2seq
	template<size_t N>
	using Index = std::integral_constant<size_t, N>;

	template<class Seq>
	struct to_list;

	template<template<class T, T... > class Seq, class U, U... Is>
	struct to_list<Seq<U, Is...>>  : sub_type< type_list<Index<Is>...> > {};	

	template<class List>
	struct to_seq;

	template<template<class...> class List, class... Is>
	struct to_seq<List<Is...>> : sub_type < std::index_sequence<Is::value...> > {};	

	template<template<class...> class ListOp, class... Args>
	struct list2seq : to_seq< typename ListOp< typename to_list<Args>::type... >::type >{};

	// index_concat
	template<class A, class B>
	using index_concat = list2seq<type_list_concat, A, B>;

	// empty_of
	template<template<size_t...> class Seq, size_t... Is>
	struct empty_of<Seq<Is...>> : list2seq<empty_of, Seq<Is...>>{};

	// size_of
	template<template<size_t...> class Seq, size_t... Is>
	struct size_of<Seq<Is...>> : Index< sizeof...(Is) >{};

	// index_push
	template<class Seq, size_t V=size_of<Seq>::value>
	using index_push = list2seq< type_list_concat, Seq, std::index_sequence<V> >;

	// index_pop
	template<class Seq>
	using index_pop = list2seq<type_list_pop, Seq>;
}
