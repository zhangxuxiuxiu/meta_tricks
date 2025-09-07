//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/
//http://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html
//https://gist.github.com/altamic/d3778f906b63c6983ef074635eb36c85
//https://github.com/dfrib/accessprivate

#pragma once

namespace access{

	template<class Tag>
	struct TagMember{
	#if __cplusplus >= 201703L
		template<auto memPtr>
	#else
		template<class MemPtr, MemPtr memPtr>
	#endif 
		struct MemberPtr{
			friend auto TagMemberPtr(TagMember*){
				return memPtr;
			}
		};
		friend auto TagMemberPtr(TagMember*);
	};

	template<class Tag, class Host, class... Args>
	decltype(auto) TagFunctor(Host& obj, Args&&... args){
		auto memPtr = TagMemberPtr(static_cast<TagMember<Tag>*>(nullptr));
		return (obj.*memPtr) (std::forward<Args>(args)...); 
	}

	template<class Tag, class Host>
	decltype(auto) TagField(Host& obj){
		auto memPtr = TagMemberPtr(static_cast<TagMember<Tag>*>(nullptr));
		return obj.*memPtr; 
	}
}
