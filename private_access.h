//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

#pragma once

#include <type_traits>
#include <tuple>

#include "injector.h"

namespace unpack{

	template< template< class > class Trans, class... Ts>
	struct all_same;
	
	template< template< class > class Trans, class T>
	struct all_same<Trans, T>{
		using type = typename Trans<T>::type;
	};
	
	template< template< class > class Trans, class T, class... Us>
	struct all_same<Trans, T, Us...>{
		using first_type = typename Trans<T>::type;
		using type = typename std::enable_if< std::is_same< first_type, typename all_same< Trans, Us... >::type >::value, first_type >::type;
	};
	
	template<class FPtr>
	struct field_host;
	
	template<class R, class T>
	struct field_host<R T::*>{
		using type = T; 
	};
	
	template<class... FPtrs>
	using fields_host = all_same< field_host, FPtrs...  >;
	
	template<class T>
	struct identity{
		using type = T;
	};
	
	
	template<auto... F>
	struct Access : std::index_sequence< sizeof( injector::Inject<
				typename all_same<identity, decltype(F)...>::type,
				Access<F...>
			>) >{
		using field_type = typename all_same<identity, decltype(F)...>::type;
		using host_type = typename field_host<field_type>::type;
	
		friend auto Eval(host_type& obj, Access* ){
			return std::forward_as_tuple(obj.*F...); 
		}
	
	};
	
	template<class T>
	struct subs_host{
		using type = typename T::host_type;
	};
	
	
	template<class... Subs>
	struct AllFields : std::index_sequence<sizeof(injector::Inject<
			typename all_same< subs_host, Subs... >::type,
			AllFields<Subs...>
			>)>{
	
		using host_type = typename all_same< subs_host, Subs... >::type; 
	
		friend auto Fields(host_type& obj, AllFields* =nullptr){
			return std::tuple_cat( Eval( obj, static_cast< injector::StateOf< typename Subs::field_type >* >(nullptr) )... ); 
		}
	};
	
	template<class Host>
	auto Unpack(Host& obj, injector::StateOf<Host>* p=nullptr){
		return Fields(obj, p);
	}

}
