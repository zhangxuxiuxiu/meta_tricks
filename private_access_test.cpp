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

namespace unpack{ // required by clang that explicit instantiation must be in ns 
#if __cplusplus >= 201703L
	template struct Fields<&Private::data, &Private::str, &Private::data2>;
	template struct Functor<TagSum, &Private::sum>;
	template struct Functor<TagMinus, &Private::minus>;
#else 
//	template<class... Args>
//	constexpr auto fieldDefine(Args... args) -> Fields<SubF<Args,args>...>;
//	decltype( fieldDefine( &Private::data,&Private::str, &Private::data2) );

//	template struct Fields<SubF<int Private::*, &Private::data>, SubF<char Private::*, &Private::str>, SubF<int Private::*, &Private::data2>>;
	template struct Fields<SubFields<decltype(&Private::data), &Private::data>, SubFields<decltype(&Private::str), &Private::str>, SubFields<decltype(&Private::data2), &Private::data2>>;
	template struct Functor<TagSum, int (Private::*)() const, &Private::sum>;
	template struct Functor<TagMinus,int (Private::*)(int ) const, &Private::minus>;
#endif
}

typedef int (Private::*sumFn)() const;


int main(){
	Private obj;

	auto dup = unpack::Unpack(obj);
	dup = std::make_tuple(12, 'c', 23);
	assert( dup == unpack::Unpack(obj) );
	std::cout << std::get<0>(dup) << '\t' << std::get<1>(dup) << '\t' << std::get<2>(dup) << '\n';

	assert( 35 == unpack::TaggedFn<TagSum>(obj));
	assert( -1 == unpack::TaggedFn<TagMinus>(obj, 10));
	// error: 'sum' is a private member of 'Private', can only be used in template parameter
//	sumFn sum = reinterpret_cast<sumFn>(&Private::sum);
//	assert( 35 == (obj.*sum)() );
}
