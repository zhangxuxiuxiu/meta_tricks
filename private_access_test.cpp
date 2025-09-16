#include <iostream>
#include <cassert>

#include "private_access.h"

// access private members
class Private {
	private:
		int data, data2;
		char str;

		int sum() const{
			return data + data2;
		}

		int minus(int base) const{
			return base + data - data2;
		}
};

struct TagSum{};
struct TagMinus{};
struct TagField{};

namespace access{ // required by clang that explicit instantiation must be in ns 
#if __cplusplus >= 201703L
	template struct Fields<Private,&Private::data, &Private::str, &Private::data2>;
	template struct Member<TagField, &Private::data>;
	template struct Member<TagSum, &Private::sum>;
	template struct Member<TagMinus, &Private::minus>;
#else 
	template struct Fields<Private,SubFields<decltype(&Private::data), &Private::data>, SubFields<decltype(&Private::str), &Private::str>, SubFields<decltype(&Private::data2), &Private::data2>>;
	template struct Member<TagField, decltype(&Private::data), &Private::data>;
	template struct Member<TagSum, int (Private::*)() const, &Private::sum>;
	template struct Member<TagMinus,int (Private::*)(int ) const, &Private::minus>;
#endif
}

typedef int (Private::*sumFn)() const;


int main(){
	Private obj;

	auto dup = access::Unpack(obj);
	auto tpl = std::make_tuple(12, 'c', 23);
	dup = tpl; 
	assert( tpl == access::Unpack(obj) );
	std::cout << std::get<0>(dup) << '\t' << std::get<1>(dup) << '\t' << std::get<2>(dup) << '\n';

	assert( 35 == access::TagMember<TagSum>(obj));
	assert( -1 == access::TagMember<TagMinus>(obj, 10));
	assert( 12 == access::TagMember<TagField>(obj));	
	access::TagMember<TagField>(obj) = 99;	
	assert( 99 == access::TagMember<TagField>(obj));	
	
	return 0;
}
