//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

#pragma once

#include <type_traits>
#include <tuple>

#include "injector.h"
#include "traits.h"

namespace unpack{
	
	template<class FPtr>
	struct field_host;
	
	template<class R, class T>
	struct field_host<R T::*>{
		using type = T; 
	};

	template<size_t Index, class FieldType>
	struct IndexedFields{};
	
	template<size_t Index, auto... F>
	struct FieldsImpl {
		using field_type = typename traits::nth_element<0, decltype(F)...>::type;
		using host_type = typename field_host<field_type>::type;
	
		friend auto FieldsEval(host_type& obj, FieldsImpl* ){
			return std::forward_as_tuple(obj.*F...); 
		}
	
	};
	
	template<auto... Fs>
	struct Fields{
		using field_type = typename traits::nth_element<0, decltype(Fs)...>::type;
		using host_type = typename field_host<field_type>::type;
	}; // used to declare field access 

	template<class T>
	struct subs_host{
		using type = typename T::host_type;
	};

	// concat type_list
	template<class...>
	struct type_list{};

	template<class A, class B>
	struct concat;

	template<class... Ts, class... Us>
	struct concat< type_list<Ts...>, type_list<Us...> >{
		using type = type_list< Ts..., Us...  >;
	};

	// Index Subs: each Sub is a Fields
	template<size_t N, class... Subs>
	struct IndexFields;

	template<size_t N>
	struct IndexFields<N>{
		using type = type_list<>; 
	};

	template<size_t N, auto... Fs,  class... Subs>
	struct IndexFields< N, Fields<Fs...>, Subs... >{
		using type = typename concat< type_list< FieldsImpl<N,Fs...> >, 
			typename IndexFields< N+1, Subs... >::type
		      >::type;
	};
	
	template<class... FieldDefines>
	struct AllFields : std::index_sequence<sizeof(injector::Inject<
			typename traits::all_same< subs_host, FieldDefines... >::type,
			AllFields<FieldDefines...>
			>)>{
	
		using host_type = typename traits::all_same< subs_host, FieldDefines... >::type; 
	
		template<class... Subs>
		static auto unpack_host_impl(host_type& obj, type_list<Subs...>){
			return std::tuple_cat( FieldsEval( obj, static_cast<Subs* >(nullptr) )... ); 
		}

		friend auto unpack_host(host_type& obj, AllFields* =nullptr){
			return unpack_host_impl(obj, typename IndexFields<0, FieldDefines...>::type{} );
		}
	};
	
	template<class Host>
	auto Unpack(Host& obj, injector::StateOf<Host>* p=nullptr){
		return unpack_host(obj, p);
	}

}
