#include <utility>
#include <type_traits>
#include <iostream>
#include <cassert>

#include "private_access_v3.h"

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
	public:
		Private(): data(12),data2(23),str('c'){} 
};

struct TagSum{};
struct TagMinus{};
struct TagData{};

namespace access{ // required by clang that explicit instantiation must be in ns 
#if __cplusplus >= 201703L
	template class MemberTag<TagData>::MemberPtr<&Private::data>;
	template class MemberTag<TagSum>::MemberPtr<&Private::sum>;
	template class MemberTag<TagMinus>::MemberPtr<&Private::minus>;
#else 
	template class MemberTag<TagData>::MemberPtr<decltype(&Private::data),&Private::data>;
	template class MemberTag<TagSum>::MemberPtr<decltype(&Private::sum),&Private::sum>;
	template class MemberTag<TagMinus>::MemberPtr<decltype(&Private::minus),&Private::minus>;
#endif
}

typedef int (Private::*sumFn)() const;


int main(){
	Private obj;

	assert( 35 == access::TagFunctor<TagSum>(obj));
	assert( -1 == access::TagFunctor<TagMinus>(obj, 10));
	assert( 12 == access::TagField<TagData>(obj));	
	access::TagField<TagData>(obj) = 99;	
	assert( 99 == access::TagField<TagData>(obj));	
	
	return 0;
}

