//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/
//https://github.com/dfrib/accessprivate
//https://dfrib.github.io/a-foliage-of-folly/
//https://accu.org/journals/overload/28/156/harrison%5F2776/

#pragma once

#include<utility>
#include<functional>

namespace access{

	template<class Tag>
	struct MemberTag{
	#if __cplusplus >= 201703L
		template<auto memPtr>
	#else
		template<class MemPtr, MemPtr memPtr>
	#endif 
		struct MemberPtr{
			friend auto TagMemberPtr(MemberTag*){
				return memPtr;
			}
		};
		// declare here so that it can be argument(ie MemberTag) looked up
		friend auto TagMemberPtr(MemberTag*);
	};

	template<class Tag>
	auto MemFn(){
		auto memPtr = TagMemberPtr(static_cast<MemberTag<Tag>*>(nullptr));
		return std::mem_fn(memPtr);
	}
}
