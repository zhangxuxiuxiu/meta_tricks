#include "stateful_trans.h"
#include "injector.h"

#include <iostream>
#include <typeinfo>


using namespace traits;

template<class T>
struct test_instantiation{
	using type =  decltype(sizeof(injector::Inject<T,int>), T{});
};   

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

int main(){
	// detect test cases
	static_assert( detect< std::is_void, int>::value);
	static_assert( !detect< std::is_void, int, int>::value);
	static_assert( detect< std::is_same, int, int>::value);
	static_assert( !detect< std::is_same, int>::value);

	// identity test cases
	static_assert( std::is_same< typename identity<int>::type, int>::value);
	static_assert( !std::is_same< typename identity<float>::type, int>::value);

	// all_same test cases
	static_assert( std::is_same< all_same_t<int>, int>::value);
	static_assert( std::is_same< all_same_t<int,int>, int>::value);

//      TODO detect error
	static_assert( detect< all_same_t, int, int>::value);
//	static_assert( !detect< all_same_t, float, int>::value);

	static_assert( std::is_same< typename all_same< type_list<int,int, float>, std::is_integral>::type, int>::value);

	// transform test cases
	// test pure transform
	static_assert( std::is_same< transform_t< std::add_pointer, int,int, float>, type_list<int*, int*, float*>>::value);
	static_assert( std::is_same< typename transform< type_list<int,double, float>,std::add_const, std::add_pointer >::type, type_list<const int*, const double*, const float*>>::value);
	static_assert( std::is_same< typename transform< type_list<int,double, float>, std::add_pointer,std::add_const >::type, type_list<int * const, double* const, float* const>>::value);

	// test pure filter 
	static_assert( std::is_same< typename transform< type_list<int,double, float>,std::is_floating_point>::type, type_list<double, float>>::value);
	static_assert( std::is_same< typename transform< type_list<int, unsigned int, float>,std::is_integral, std::is_unsigned>::type, type_list<unsigned int>>::value);
	// trans + filter
	static_assert( std::is_same< typename transform< type_list<int, const char, long>, std::is_integral>::type, type_list<int, const char, long>>::value);
	static_assert( std::is_same< typename transform< type_list<int, const char, long>, std::is_integral, std::add_pointer, std::is_pointer, std::remove_pointer, std::is_const>::type, type_list<const char>>::value);
	static_assert( std::is_same< typename transform< type_list<int, const char, long>, std::is_integral, std::add_const>::type, type_list<const int, const char, const long>>::value);
	static_assert(okeof<transform_x_state<type_list<>, true, true>>());
	static_assert( std::is_same< typename transform< type_list<int, const char, long>, std::is_integral, std::add_const, std::is_pointer>::type, type_list<>>::value);

	// nth_element
	// simplest
	static_assert( std::is_same< nth_element_t< 0, int, const char, long>, int>::value);
	static_assert( std::is_same< nth_element_t< 1, int, const char, long>, const char>::value);
	static_assert( std::is_same< nth_element_t< 2, int, const char, long>, long>::value);
	static_assert( std::is_same< nth_element_t< 3, int, const char, long, float>, float>::value);

	static_assert( std::is_same< typename nth_element< 3, type_list<int, const char, long, float>, std::add_const, std::add_pointer>::type,const float*>::value);

//	std::cout << typeid(typename stateless_trans_z<>::template fn<const int>::type).name() << '\n';
//	std::cout << typeid(typename stateless_trans_z<>::template fn<const int>::type::type).name() << '\n';
//	static_assert( std::is_same< typename stateless_trans_z<>::template fn<const int>::type::type, const int>::value );
//

	// index list
	static_assert( std::is_same< index_types_t<int,long>, type_list<IndexedType<0,int>, IndexedType<1,long>> >::value );
	static_assert( std::is_same< index_types_t<>, type_list<> >::value );
	static_assert( std::is_same< typename index_types<type_list<int, long, float>, std::is_integral, std::add_const, std::add_pointer>::type, type_list<IndexedType<0,const int*>, IndexedType<1,const long*>> >::value );

}

#ifdef __clang__
# pragma clang diagnostic pop
#endif
