#include <iostream>

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
DECLARE_PRIVATE_FUNCTION(Tag1, void(A::*)(), &A::f)
struct Tag2 {};
DECLARE_PRIVATE_FIELD(Tag2, int A::*, &A::val)

int main() {
	A a;
	TagMem<Tag1>(a);
//	TODO return auto&&
//	auto& v = TagMem<Tag2>(2);// = 4;
	std::cout << TagMem<Tag2>(a) << '\n';
}
