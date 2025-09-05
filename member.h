#pragma once

#include <utility>

namespace member{
	template<class T, class MemPtr, class... Args>
	decltype(auto) Eval(T&& obj, MemPtr memPtr, typename std::enable_if<std::is_member_function_pointer<MemPtr>::value>::type*, Args&&... args){
		return (std::forward<T>(obj).*memPtr) (std::forward<Args>(args)...); 
	}

	template<class T, class MemPtr, class... Args>
	decltype(auto) Eval(T&& obj, MemPtr memPtr, typename std::enable_if<!std::is_member_function_pointer<MemPtr>::value>::type*, Args&&... args){
		return std::forward<T>(obj).*memPtr;
	}
}
