#pragma once

#include <utility>

namespace access{
	template<class Tag, class MemPtr>
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
	
	template<class Tag, class MemPtr>
	MemPtr MemPtrHolder<Tag, MemPtr>::value;

	template<class Tag, class MemPtr>
	template<MemPtr memPtr>
	class MemPtrHolder<Tag,MemPtr>::template Initializer<memPtr> MemPtrHolder<Tag,MemPtr>::Initializer<memPtr>::initializer;
	
	template<class>
	struct TagMemPtr;
	
	template<class Tag, class T, class... Args>
	decltype(auto) TagFunctor(T&& obj, Args&&... args){
		using MemPtr = typename TagMemPtr<Tag>::type;
		auto memPtr = MemPtrHolder<Tag, MemPtr>::value;	
		return (obj.*memPtr) (std::forward<Args>(args)...); 
	}

	template<class Tag, class T, class... Args>
	decltype(auto) TagField(T&& obj, Args&&... args){
		using MemPtr = typename TagMemPtr<Tag>::type;
		auto memPtr = MemPtrHolder<Tag, MemPtr>::value;	
		return obj.*memPtr; 
	}
}


#define DECLARE_PRIVATE_MEMBER(Tag, MemPtr, memPtr) 				\
namespace access{								\
	template class MemPtrHolder<Tag,MemPtr>::template Initializer<memPtr>;	\
	template<>								\
	struct TagMemPtr<Tag>{							\
		using type = MemPtr;						\
	};									\
}
