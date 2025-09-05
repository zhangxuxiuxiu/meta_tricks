#pragma once

#include <utility>

namespace member{
	template<class MemPtr, class T, class... Args>
	auto Eval(T&& obj, MemPtr memPtr, Args&&... args)
	-> typename std::enable_if<std::is_member_function_pointer<MemPtr>::value, decltype( (std::forward<T>(obj).*memPtr) (std::forward<Args>(args)...) ) >::type {
		return (std::forward<T>(obj).*memPtr) (std::forward<Args>(args)...); 
	}

	template<class MemPtr, class T, class... Args>
	auto Eval(T&& obj, MemPtr memPtr, Args&&... args)
	-> typename std::enable_if<!std::is_member_function_pointer<MemPtr>::value, decltype( std::forward<T>(obj).*memPtr ) >::type {
		return std::forward<T>(obj).*memPtr;
	}
}
