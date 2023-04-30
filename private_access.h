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
	
	template<auto... F>
	struct Fields : std::index_sequence< sizeof( injector::Inject<
				typename traits::nth_element<0, decltype(F)...>::type,
				Fields<F...>
			>) >{
		using field_type = typename traits::nth_element<0, decltype(F)...>::type;
		using host_type = typename field_host<field_type>::type;
	
		friend auto FieldsEval(host_type& obj, Fields* ){
			return std::forward_as_tuple(obj.*F...); 
		}
	
	};
	
	template<class T>
	struct subs_host{
		using type = typename T::host_type;
	};
	
	template<class... FieldDefines>
	struct AllFields : std::index_sequence<sizeof(injector::Inject<
			typename traits::all_same< subs_host, FieldDefines... >::type,
			AllFields<FieldDefines...>
			>)>{
	
		using host_type = typename traits::all_same< subs_host, FieldDefines... >::type; 
	
		friend auto unpack_host(host_type& obj, AllFields* =nullptr){
			return std::tuple_cat( FieldsEval( obj, static_cast< injector::StateOf< typename FieldDefines::field_type >* >(nullptr) )... ); 
		}
	};
	
	template<class Host>
	auto Unpack(Host& obj, injector::StateOf<Host>* p=nullptr){
		return unpack_host(obj, p);
	}

//	template<class... FieldDefines>
//	struct DefineFields : std::index_sequence< sizeof(FieldDefines)... >{};
}
