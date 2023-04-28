#include "injector.h"

#if defined(__clang__) && __cplusplus < 201703L
# pragma clang diagnostic push 
# pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

int main(){
	static_assert(!injector::HasState<char, UniqueTag>::value);
	static_assert(injector::Inject<char,double>::value);
	static_assert(std::is_same< injector::StateOf<char>, double>::value);
	static_assert(!std::is_same< injector::StateOf<char>, float>::value);
	static_assert(injector::Inject<char,double>::value);
	static_assert(!injector::Inject<char,float>::value);
	static_assert(injector::HasState<char, UniqueTag>::value);

#if __cplusplus >= 202002L
	static_assert(!injector::HasState<int>::value);
	static_assert(injector::Inject<int,double>::value);
	static_assert(injector::HasState<int>::value);
#endif
}

#if defined(__clang__) && __cplusplus < 201703L
# pragma clang diagnostic pop
#endif
