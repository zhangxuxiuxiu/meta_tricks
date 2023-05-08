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
		template<class T>
		struct fn_impl{
			// Tran is a normal transformation
			template<class Tn, class = bool>
			struct X : stateless_trans_filter<Urans...>::template fn<typename Tn::type>{};

			// Tran is a filter, filter return true 
			template<class Tn>
			struct X<Tn, typename std::enable_if<Tn::value,bool>::type> : stateless_trans_filter<Urans...>::template fn<T>{};

			// Tran is a filter, filter return false 
			template<class Tn>
			struct X<Tn, typename std::enable_if<!Tn::value,bool>::type> : std::false_type{};

			using type = X<typename Tran::template fn<T>>; 
		};

		template<class T>
		struct fn : fn_impl<T>::type{};
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
	

	// only transforms
	template< class... Trans>
	struct stateless_trans;

	template<>
	struct stateless_trans<>{
		template<class T>
		struct fn{
			using type = T;
		};
	};

	template<class Tran, class... Urans>
	struct stateless_trans<Tran, Urans...>{
		template<class T>
		struct fn : stateless_trans<Urans...>::template fn< typename Tran::template fn<T>::type >{};
	};

	template<template<class > class Tran>
	struct make_trans{
		using type = struct X {
			template<class T>
			struct fn : Tran<T>{};
		};
	};

	template<template<class> class... Trans> 
	using stateless_trans_z = stateless_trans< typename make_trans<Trans>::type... >;
}
