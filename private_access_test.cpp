#include <iostream>
#include <cassert>

#include "private_access.h"

// access private members
class Private {
  int data, data2;
  char str;
};

namespace unpack{ // required by clang that explicit instantiation must be in ns 
	template struct AllFields<Fields<&Private::data>, Fields<&Private::str>, Fields<&Private::data2>>;
}


int main(){
	Private obj;
//	auto int_ft = static_cast< injector::StateOf<int Private::*>*>(nullptr); 
//	auto&& data = FieldsEval(obj, int_ft);	
//	data = std::make_tuple(31,32); 
//	assert( data == FieldsEval(obj, int_ft) );
//
//	auto char_ft = static_cast< injector::StateOf<char Private::*>*>(nullptr); 
//	auto&& str= FieldsEval( obj, char_ft);
//	str = std::make_tuple('a'); 
//	assert( str == FieldsEval(obj, char_ft) );

	auto dup = unpack::Unpack(obj);
	dup = std::make_tuple(12, 'c', 23);
	assert( dup == unpack::Unpack(obj) );
	std::cout << std::get<0>(dup) << '\t' << std::get<1>(dup) << '\t' << std::get<2>(dup) << '\n';

}
