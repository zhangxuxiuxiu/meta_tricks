//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

#include <type_traits>
#include <iostream>
#include <tuple>
#include <cassert>


template< template< class > class Trans, class... Ts>
struct all_same;

template< template< class > class Trans, class T>
struct all_same<Trans, T>{
	using type = typename Trans<T>::type;
};

template< template< class > class Trans, class T, class... Us>
struct all_same<Trans, T, Us...>{
	using first_type = typename Trans<T>::type;
	using type = typename std::enable_if< std::is_same< first_type, typename all_same< Trans, Us... >::type >::value, first_type >::type;
};

template<class FPtr>
struct field_host;

template<class R, class T>
struct field_host<R T::*>{
	using type = T; 
};

template<class... FPtrs>
using fields_host = all_same< field_host, FPtrs...  >;

template<class T>
struct identity{
	using type = T;
};

// version 1
template<auto... F>
struct Access{
	using field_type = typename all_same<identity, decltype(F)...>::type;
	using host_type = typename field_host<field_type>::type;

	friend auto Eval(field_type, host_type& obj){
		return std::forward_as_tuple(obj.*F...); 
	}
};

// version 2
template<auto... F>
struct SubFields{
	using host_type = typename fields_host<decltype(F)...>::type;

	friend auto Eval(host_type& obj){
		return std::forward_as_tuple(obj.*F...); 
	}
};

template<class T>
using subs_host = typename T::host_type;

//template<class... Subs>
//struct AllFields{
//	using host_type = typename all_same< subs_host, Subs... >::type; 
//
//	static auto Eval(host_type& obj){
//		return std::tuple_cat( Eval); 
//	}
//};

// example

// access private members
class Private {
  int data, data2;
  char str;
};

template struct Access<&Private::data, &Private::data2>;
auto Eval(int Private::*, Private& obj);
// failed that: error: 'int Private::data2' is private within this context
// using int_acc = decltype(Access<&Private::data, &Private::data2>{});


int main(){
	Private obj;
	auto&& data = Eval( static_cast<int Private::*>(nullptr), obj);
	data = std::make_tuple(31,32); 
	assert( data == Eval(static_cast<int Private::*>(nullptr), obj) );

}
