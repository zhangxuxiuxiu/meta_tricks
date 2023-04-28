#include "injector.h"

int main(){
	static_assert(injector::Inject<int,double>::value);
	static_assert(std::is_same<injector::StateOf<int>, double>::value);

	static_assert(injector::Inject<char,double>::value);
	static_assert(std::is_same< injector::StateOf<char>, double>::value);
	static_assert(!std::is_same< injector::StateOf<char>, float>::value);
	static_assert(injector::Inject<char,double>::value);
	static_assert(!injector::Inject<char,float>::value);

}

