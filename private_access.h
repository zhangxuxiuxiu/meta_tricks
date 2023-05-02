//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

#pragma once

#include <type_traits>
#include <tuple>

#include "injector.h"
#include "traits.h"

namespace unpack{
	
	template<class FPtr>
	struct member_host;
	
	template<class R, class T>
	struct member_host<R T::*>{
		using type = T; 
	};

	template<class R, class T, class... Args>
	struct member_host<R T::*(Args...)>{
		using type = T; 
	};

	// access private fields
#if __cplusplus >= 201703L
	template<auto... F>
	struct Fields{
		using field_type = typename traits::nth_element<0, decltype(F)...>::type;
		using host_type = typename member_host<field_type>::type;
	
		friend auto unpack_host(host_type& obj, Fields* ) 
			-> decltype( sizeof(injector::Inject<host_type, Fields>), std::forward_as_tuple(obj.*F...) ){
			return std::forward_as_tuple(obj.*F...); 
		}
	
	};
#else
	template<class T, T... F>
	struct SubF {
		using field_type = T;
		using host_type = typename member_host<field_type>::type;
	
		friend auto FieldsEval(host_type& obj, SubF* ){
			return std::forward_as_tuple(obj.*F...); 
		}
	
	};

	template<class T>
	struct subs_host {
		using type = typename T::host_type;
	};

	template<class... FieldDefines>
	struct Fields {
		using host_type = typename traits::all_same< subs_host, FieldDefines... >::type; 
	
		friend auto unpack_host(host_type& obj, Fields* ) 
			-> decltype( sizeof(injector::Inject<host_type, Fields>), std::tuple_cat( FieldsEval( obj, static_cast<FieldDefines*>(nullptr) )... ) ) {
			return std::tuple_cat( FieldsEval( obj, static_cast<FieldDefines* >(nullptr) )... ); 
		}
	};
	
#endif

	template<class Host>
	auto Unpack(Host& obj, injector::StateOf<Host>* p=nullptr){
		return unpack_host(obj, p);
	}

	// access private function 
#if __cplusplus >= 201703L
	template<class Tag, auto Fptr>
	struct Functor: std::index_sequence< sizeof( injector::Inject< Tag, Functor<Tag, Fptr> > ) >
#else
	template<class Tag, class F, F Fptr>
	struct Functor: std::index_sequence< sizeof( injector::Inject< Tag, Functor<Tag, F, Fptr> > ) >
#endif 
	{
	
		using host_type = typename member_host<decltype(Fptr)>::type; 
	
		template<class... Args>
		friend auto tag_fn(host_type& obj, Functor*, Args&&... args){
			return (obj.*Fptr)( std::forward<Args>(args)... );
		}
	};
	
	template<class Tag, class Host, class... Args>
	auto TaggedFn(Host& obj, Args&&... args){
		return tag_fn(obj, static_cast<injector::StateOf<Tag>*>(nullptr), std::forward<Args>(args)... );
	}
}
