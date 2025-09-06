#pragma once

#include <utility>

#include "member.h"

namespace access{
	template<typename Tag, typename MemPtr>
	struct MemPtrHolder {
		static MemPtr value;

		template<MemPtr memPtr>
		struct Initializer{
			Initializer(){
				value = memPtr;
			}
			static Initializer initializer;
		};	
	};
	
	template<typename Tag, typename MemPtr>
	MemPtr MemPtrHolder<Tag, MemPtr>::value;

	template<typename Tag, typename MemPtr>
	template<MemPtr memPtr>
	typename MemPtrHolder<Tag, MemPtr>::template Initializer<memPtr> MemPtrHolder<Tag, MemPtr>::Initializer<memPtr>::initializer;
	
	template<typename Tag>
	struct TagDispatcher{
		template<typename T, typename... Args>
		static decltype(auto) Dispatch(T&& obj, Args&&... args);
	};
	
	template<typename Tag, typename T, typename... Args>
	decltype(auto) TagMem(T&& obj, Args&&... args){
		return TagDispatcher<Tag>::Dispatch(std::forward<T>(obj), std::forward<Args>(args)...);
	}

}


#define DECLARE_PRIVATE_MEMBER(Tag, MemPtr, memPtr) 				\
namespace access{								\
	template class MemPtrHolder<Tag, MemPtr>::template Initializer<memPtr>;	\
	template<>								\
	struct TagDispatcher<Tag>{						\
		template<typename T, typename... Args>				\
		static decltype(auto) Dispatch(T&& obj, Args&&... args){	\
			return member::Eval<MemPtr>(std::forward<T>(obj), MemPtrHolder<Tag,MemPtr>::value, std::forward<Args>(args)...); \
		}								\
	};									\
}
