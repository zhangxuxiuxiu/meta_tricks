#pragma once

#include <utility>

namespace access{
	template<typename Tag, typename MemPtr>
	struct MemPtrHolder {
		static MemPtr value;

		template<MemPtr ptrValue>
		struct Initiator{
			Initiator(){
				value = ptrValue;
			}
			static Initiator initiator;
		};	
	};
	
	template<typename Tag, typename MemPtr>
	MemPtr MemPtrHolder<Tag, MemPtr>::value;

	template<typename Tag, typename MemPtr>
	template<MemPtr ptrValue>
	typename MemPtrHolder<Tag, MemPtr>::template Initiator<ptrValue> MemPtrHolder<Tag, MemPtr>::Initiator<ptrValue>::initiator;
	
	template<typename Tag>
	struct TagDispatcher{
		template<typename T, typename... Args>
		static auto Dispatch(T&& obj, Args&&... args);
	};
	
	template<typename Tag, typename T, typename... Args>
	auto TagMem(T&& obj, Args&&... args){
		return TagDispatcher<Tag>::Dispatch(std::forward<T>(obj), std::forward<Args>(args)...);
	}
}

#define DECLARE_PRIVATE_FUNCTION( Tag, Sig, Ptr) 				\
namespace access{								\
	template class MemPtrHolder< Tag, Sig>::template Initiator<Ptr>;	\
	template<>								\
	struct TagDispatcher<Tag>{						\
		template<typename T, typename... Args>				\
		static auto Dispatch(T&& obj, Args&&... args){			\
			return (obj.*MemPtrHolder<Tag,Sig>::value) (std::forward<Args>(args)...); \
		}								\
	};									\
}

#define DECLARE_PRIVATE_FIELD( Tag, Sig, Ptr) 					\
namespace access{								\
	template class MemPtrHolder< Tag, Sig>::template Initiator<Ptr>;	\
	template<>								\
	struct TagDispatcher<Tag>{						\
		template<typename T, typename... Args>				\
		static auto Dispatch(T&& obj, Args&&... args){			\
			return obj.*MemPtrHolder<Tag,Sig>::value; 		\
		}								\
	};									\
}

