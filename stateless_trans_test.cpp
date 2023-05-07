#include "stateless_trans.h"
#include "traits.h"

using namespace traits;

int main(){

	// zero args
	static_assert( std::is_same< typename stateless_trans_filter_z<>::template fn<int>::type::type, int >::value);
	// one trans
	static_assert( std::is_same< typename stateless_trans_filter_z<identity>::template fn<int>::type::type, int >::value);
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral>::template fn<int>::type::type, int>::value);
	// more trans
	static_assert( std::is_same< typename stateless_trans_filter_z<identity, identity>::template fn<int>::type::type, int >::value);
	static_assert( std::is_same< typename stateless_trans_filter_z<identity, std::add_const, std::add_pointer>::template fn<int>::type::type, const int* >::value);

	// one filter
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral>::template fn<int>::type::type, int>::value);
	static_assert( !stateless_trans_filter_z<std::is_void>::template fn<int>::type::value);
	static_assert( !stateless_trans_filter_z<std::is_integral>::template fn<float>::type::value);

	// more filters
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral, std::is_trivial, std::is_signed>::template fn<int>::type::type, int>::value);
	static_assert( !stateless_trans_filter_z<std::is_void,std::is_floating_point>::template fn<float>::type::value);

	// combine filter and transform
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral, std::make_unsigned, std::is_unsigned>::template fn<int>::type::type, unsigned int>::value);
	static_assert( !stateless_trans_filter_z<std::is_integral, std::make_signed, std::is_unsigned>::template fn<int>::type::value);

}
