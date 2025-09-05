//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/
//http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html
//https://gist.github.com/altamic/d3778f906b63c6983ef074635eb36c85
//https://github.com/dfrib/accessprivate

#pragma once

#include <tuple>
#include <utility> //forward

#include "member.h"
#include "injector.h"

namespace access{


// access private fields
#if __cplusplus >= 201703L
	template<class Tag, auto memPtr>
#else
	template<class Tag, class MemPtr, MemPtr memPtr>
#endif 
	struct Member{
		template<class Host, class... Args>
		friend decltype(auto) tag_mem(typename injector::Inject<Tag, Member>::type*, Host& obj, Args&&... args){
			return member::Eval(obj, memPtr, nullptr, std::forward<Args>(args)...);
		}
	};

	template<class Tag, class Host, class... Args>
	decltype(auto) TagMem(Host& obj, Args&&... args){
		return tag_mem(static_cast<injector::StateOf<Tag>*>(nullptr), obj, std::forward<Args>(args)... );
	}

// access private fields
#if __cplusplus >= 201703L
	template<class Host, auto... memPtrs>
	struct Fields{
		friend auto unpack_host(typename injector::Inject<Host, Fields>::type*, Host& obj ){ 
			return std::forward_as_tuple(obj.*memPtrs...); 
		}
	};
#else
	template<class MemPtr, MemPtr... memPtrs>
	struct SubFields {
		template<class Host>
		friend auto eval_fields(SubFields*, Host& obj){
			return std::forward_as_tuple(obj.*memPtrs...); 
		}
	};

	template<class Host, class... SubFieldDefines>
	struct Fields {
		friend auto unpack_host(typename injector::Inject<Host, Fields>::type*, Host& obj){ 
			return std::tuple_cat( eval_fields(static_cast<SubFieldDefines* >(nullptr), obj)... ); 
		}
	};
#endif

	// why injector::StateOf<Host>*=Fields* here?
	// b' unpack_host can only be name lookuped through Fields
	template<class Host>
	auto Unpack(Host& obj){
		return unpack_host(static_cast<injector::StateOf<Host>*>(nullptr), obj);
	}

}
