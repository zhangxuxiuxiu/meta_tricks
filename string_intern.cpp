#include <iostream>

#include "string_intern.h"

template<class, int>
struct Uniquer{};

#define Uq Uniquer(__FILE__,__COUNTER__)
#define Uniquer(F,N) Uniquer<$(F),N>

template<const char*, int>
struct Uniquer2{};

#define Uq2 Uniquer2(__FILE__,__COUNTER__)

int main(){
	std::cout  << typeid(Uq).name() << '\n' ;
	std::cout  << typeid(Uq).name() << '\n' ;

	std::cout  << typeid(Uq2).name() << '\n' ;
	std::cout  << typeid(Uq2).name() << '\n' ;

}
