#pragma once

#include <type_traits> 	// integral_constant
#include <utility>	// index_sequence

namespace traits{

	// list operations
	template<typename...>
	struct type_list {};

	// empty_of
	template<class Ts>
	struct empty_of;

	template<template<class...> class List, class... Ts>
	struct empty_of<List<Ts...>>{
		using type = List<>;
	};

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

	template<template<class...> class List, typename... Ts, typename... Us>
	struct type_list_concat<List<Ts...>, List<Us...>> {
		using type = List<Ts..., Us...>;
	};

	// type_list_append
	template<class Ts, class... Args>
	struct type_list_append;

	template<template<class...> class List, class... Ts, class... Args>
	struct type_list_append<List<Ts...>, Args...>{
      		using type = List<Ts..., Args...>;
	};

	// type_list_pop
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

	// index_concat
	template<class, class>
	struct index_concat;

	template<size_t... Is, size_t... Js>
	struct index_concat<std::index_sequence<Is...>, std::index_sequence<Js...>>{
		using type = std::index_sequence<Is..., Js...>;
	};

	// index_push
	template<class Seq>
	struct index_push;

	template<size_t... Is>
	struct index_push<std::index_sequence<Is...>>{
		using type = std::index_sequence<Is..., sizeof...(Is)>;
	};

	// index_pop
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

	// empty_of
	template<template<size_t...> class Seq, size_t... Is>
	struct empty_of<Seq<Is...>>{
		using type = Seq<>;
	};

	// size_of
	template<template<size_t...> class Seq, size_t... Is>
	struct size_of<Seq<Is...>> : std::integral_constant< size_t, sizeof...(Is) >{};
}
