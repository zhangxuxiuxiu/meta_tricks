#include <iostream>
#include <cassert>

#include "private_access_v2.h"

using namespace access;

// custom class definition
struct A {
private:
 	int val; 
	void f() {
	  std::cout << "proof!" << std::endl;
	}
public:
 	A():val(3){}
};

struct Tag1 {};
DECLARE_PRIVATE_MEMBER(Tag1, void(A::*)(), &A::f)
struct Tag2 {};
DECLARE_PRIVATE_MEMBER(Tag2, int A::*, &A::val)

int main() {
	A a;
	TagMem<Tag1>(a);
	TagMem<Tag2>(a) = 4;
	
	assert( TagMem<Tag2>(a) == 4);

	assert( std::is_lvalue_reference<decltype(TagMem<Tag2>(a) )>::value );
}
