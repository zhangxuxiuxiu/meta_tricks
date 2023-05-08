#pragma once

#include <type_traits> // enable_if, false_type

namespace traits{

	// stateless_trans to support chain multiple stateless trans as a stateless trans
	// typename stateless_trans::template fn<T> is a type with either a sub type to indicate a transform or a sub value to indicate a filter 
	template< class... Trans>
	struct stateless_trans_filter;

	template<>
	struct stateless_trans_filter<>{
		template<class T>
		struct fn{
			using type = T;
		};
	};

	template<class Tran, class... Urans>
	struct stateless_trans_filter<Tran, Urans...>{
		// Tran is a normal transformation
		template<class T, class Tn =typename Tran::template fn<T>, class = bool>
		struct fn : stateless_trans_filter<Urans...>::template fn<typename Tn::type>{};

		// Tran is a filter, filter return true 
		template<class T, class Tn>
		struct fn<T, Tn, typename std::enable_if<Tn::value,bool>::type> : stateless_trans_filter<Urans...>::template fn<T>{};

		// Tran is a filter, filter return false 
		template<class T, class Tn>
		struct fn<T, Tn, typename std::enable_if<!Tn::value,bool>::type> : std::false_type{};
	};

	template<template<class > class Tran>
	struct make_trans_filter{
		using type = struct X {
			template<class T>
			struct fn : Tran<T>{};
		};
	};

	template<template<class> class... Trans> 
	using stateless_trans_filter_z = stateless_trans_filter< typename make_trans_filter<Trans>::type... >;
}
