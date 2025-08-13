//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

#pragma once

#include <tuple>
#include <utility> //forward

#include "injector.h"
#include "traits.h"
#include "stateful_trans.h"

namespace unpack{

	// access private fields
#if __cplusplus >= 201703L
	template<auto... F>
	struct Fields{
		using host_type = typename traits::all_same< traits::type_list<decltype(F)...>, traits::member_host>::type;
	
		friend auto unpack_host(host_type& obj, typename injector::Inject<host_type, Fields>::type* ){ 
			return std::forward_as_tuple(obj.*F...); 
		}
	};
#else
	template<class T, T... F>
	struct SubFields {
		using host_type = typename traits::member_host<T>::type;
	
		friend auto FieldsEval(host_type& obj, SubFields* ){
			return std::forward_as_tuple(obj.*F...); 
		}
	};

	template<class T>
	struct sub_fields_host : traits::identity< typename T::host_type > {};

	template<class... SubFieldDefines>
	struct Fields {
		using host_type = typename traits::all_same< traits::type_list<SubFieldDefines...>, sub_fields_host >::type; 
	
		friend auto unpack_host(host_type& obj, typename injector::Inject<host_type, Fields>::type*){ 
			return std::tuple_cat( FieldsEval( obj, static_cast<SubFieldDefines* >(nullptr) )... ); 
		}
	};
#endif

	// why injector::StateOf<Host>*=Fields* here?
	// b' unpack_host can only be name lookuped through Fields
	template<class Host>
	auto Unpack(Host& obj){
		return unpack_host(obj, static_cast<injector::StateOf<Host>*>(nullptr));
	}

	// access private function 
#if __cplusplus >= 201703L
	template<class Tag, auto Fptr>
#else
	template<class Tag, class F, F Fptr>
#endif 
	struct Functor {
		using host_type = typename traits::member_host<decltype(Fptr)>::type; 
	
		template<class... Args>
		friend auto tag_fn(host_type& obj, typename injector::Inject<Tag, Functor>::type*, Args&&... args){
			return (obj.*Fptr)( std::forward<Args>(args)... );
		}
	};
	
	template<class Tag, class Host, class... Args>
	auto TaggedFn(Host& obj, Args&&... args){
		return tag_fn(obj, static_cast<injector::StateOf<Tag>*>(nullptr), std::forward<Args>(args)... );
	}
}
