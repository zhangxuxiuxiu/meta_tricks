#include <iostream>

#include "unpack.h"

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

struct empty{};
struct one{ int x; };
struct two{ long x; float y; };
struct three{ char x; double y; float z;};

int main(){
	static_assert(std::is_same<std::tuple<>, typename unpack::TupleLike<empty>::type>::value);
	static_assert(std::is_same<std::tuple<int>, typename unpack::TupleLike<one>::type>::value);
	static_assert(std::is_same<std::tuple<long,float>, typename unpack::TupleLike<two>::type>::value);
	static_assert(std::is_same<std::tuple<char,double, float>, typename unpack::TupleLike<three>::type>::value);
	std::cout << typeid(typename unpack::TupleLike<one>::type).name() << '\n';
	std::cout << typeid(typename unpack::TupleLike<two>::type).name() << '\n';
	std::cout << typeid(typename unpack::TupleLike<two>::type).name() << '\n';
	std::cout << typeid(typename unpack::TupleLike<three>::type).name() << '\n';
}

#ifdef __clang__
# pragma clang diagnostic pop
#endif
