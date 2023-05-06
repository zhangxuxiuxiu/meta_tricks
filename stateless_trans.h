#pragma once

#include <type_traits> // enable_if, false_type

namespace traits{

	// stateless_trans to support chain multiple stateless trans as a stateless trans
	// typename stateless_trans::template fn<T>::type is a type with either a sub type to indicate a transform or a sub value to indicate a filter 
	template< class... Trans>
	struct stateless_trans_generic;

	template<>
	struct stateless_trans_generic<>{
		template<class T>
		struct fn{
			using type = struct X {
				using type = T;
			};
		};
	};

	template<class Tran, class... Urans>
	struct stateless_trans_generic<Tran, Urans...>{
		template<class T>
		struct fn{
			// Tran is a filter, filter return false
			template<class Tn>
			static constexpr auto impl(typename std::enable_if<!Tn::value,int>::type){
				return std::false_type{};
			}

			// Tran is a filter, filter return true 
			template<class Tn>
			static constexpr auto impl(typename std::enable_if<Tn::value,float>::type){
				return typename stateless_trans_generic<Urans...>::template fn<T>::type{};
			}

			// Tran is a normal transformation
			template<class Tn>
			static constexpr auto impl(...){
				return typename stateless_trans_generic<Urans...>::template fn<typename Tn::type>::type{};
			}

			using type = decltype(impl< typename Tran::template fn<T>::type >(0)); 
		};
	};
	
	template<template<class > class Tran>
	struct make_trans{
		using type = struct X {
			template<class T>
			struct fn {
				using type  = Tran<T>;
			};
		};
	};

	template<template<class> class... Trans> 
	using stateless_trans = stateless_trans_generic< typename make_trans<Trans>::type... >;

}
