#pragma once

#include <utility>

#include "member.h"

namespace access{
	template<typename Tag, typename MemPtr>
	struct MemPtrHolder {
		static MemPtr value;

		template<MemPtr memPtr>
		struct Initiator{
			Initiator(){
				value = memPtr;
			}
			static Initiator initiator;
		};	
	};
	
	template<typename Tag, typename MemPtr>
	MemPtr MemPtrHolder<Tag, MemPtr>::value;

	template<typename Tag, typename MemPtr>
	template<MemPtr memPtr>
	typename MemPtrHolder<Tag, MemPtr>::template Initiator<memPtr> MemPtrHolder<Tag, MemPtr>::Initiator<memPtr>::initiator;
	
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
	template class MemPtrHolder<Tag, MemPtr>::template Initiator<memPtr>;	\
	template<>								\
	struct TagDispatcher<Tag>{						\
		template<typename T, typename... Args>				\
		static decltype(auto) Dispatch(T&& obj, Args&&... args){	\
			return member::Eval(std::forward<T>(obj), MemPtrHolder<Tag,MemPtr>::value, nullptr, std::forward<Args>(args)...); \
		}								\
	};									\
}
