#include "stateless_trans.h"
#include "traits.h"
#include "injector.h"

#include <iostream>
#include <typeinfo>
using namespace traits;

template<class T>
//struct test_instantiation : std::integral_constant<int, sizeof(injector::Inject<T,int>)>{
struct test_instantiation{ 
	using type = decltype(sizeof(injector::Inject<T,int>), T{});
};   

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

int main(){

	// zero args
	static_assert( std::is_same< typename stateless_trans_filter_z<>::template fn<int>::type, int >::value);
	// one trans
	static_assert( std::is_same< typename stateless_trans_filter_z<identity>::template fn<int>::type, int >::value);
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral>::template fn<const int>::type, const int>::value);
	// more trans
	static_assert( std::is_same< typename stateless_trans_filter_z<identity, identity>::template fn<int>::type, int >::value);
	static_assert( std::is_same< typename stateless_trans_filter_z<identity, std::add_const, std::add_pointer>::template fn<int>::type, const int* >::value);

	// one filter
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral>::template fn<int>::type, int>::value);
	static_assert( !stateless_trans_filter_z<std::is_void>::template fn<int>::value);
	static_assert( !stateless_trans_filter_z<std::is_integral>::template fn<float>::value);

	// more filters
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral, std::is_trivial, std::is_signed>::template fn<int>::type, int>::value);
	static_assert( !stateless_trans_filter_z<std::is_void,std::is_floating_point>::template fn<float>::value);

	// combine filter and transform
	static_assert( std::is_same< typename stateless_trans_filter_z<std::is_integral, std::make_unsigned, std::is_unsigned>::template fn<int>::type, unsigned int>::value);
	static_assert( !stateless_trans_filter_z<std::is_integral, std::make_signed, std::is_unsigned>::template fn<int>::value);
}

#ifdef __clang__
# pragma clang diagnostic pop
#endif
