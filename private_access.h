//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

#pragma once

#include <tuple>
#include <utility> //forward

#include "injector.h"
#include "traits.h"

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
	struct SubF {
		using host_type = typename traits::member_host<T>::type;
	
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
		using host_type = typename traits::all_same< traits::type_list<FieldDefines...>, subs_host >::type; 
	
		friend auto unpack_host(host_type& obj, typename injector::Inject<host_type, Fields>::type*){ 
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
#else
	template<class Tag, class F, F Fptr>
#endif 
	struct Functor {
		using host_type = typename traits::member_host<decltype(Fptr)>::type; 
	
		template<class... Args>
		friend auto tag_fn(host_type& obj, typename injector::Inject< Tag, Functor>::type*, Args&&... args){
			return (obj.*Fptr)( std::forward<Args>(args)... );
		}
	};
	
	template<class Tag, class Host, class... Args>
	auto TaggedFn(Host& obj, Args&&... args){
		return tag_fn(obj, static_cast<injector::StateOf<Tag>*>(nullptr), std::forward<Args>(args)... );
	}
}
