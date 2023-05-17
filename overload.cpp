#include <iostream>
#include <type_traits>


template<size_t N>
struct OL : OL<N-1>{};

template<>
struct OL<0>{};

template<class T>
struct Dispatcher{

	// lowest priority
	template<class U>
	static constexpr size_t impl(OL<0>){
		return 5;
	}
	
	template<class U>
	static constexpr size_t impl(typename std::enable_if< std::is_integral<U>::value, OL<2>>::type ){
		return 4;
	}
	
	template<class U>
	static constexpr size_t impl(typename std::enable_if< std::is_same<U, float>::value, OL<4>>::type ){
		return 3;
	}

	template<class U>
	static constexpr size_t impl(typename std::enable_if< std::is_floating_point<U>::value, OL<6>>::type ){
		return 2;
	}

	// highest priority
	template<class U>
	static constexpr size_t impl(typename std::enable_if< std::is_same<U, int>::value, OL<8>>::type ){
		return 1;
	}

	template<size_t N>
	static constexpr size_t value(){
		return impl<T>(OL<N>{});
	}
};

int main(){
	static_assert( Dispatcher<int>::value<0>() == 5);
	static_assert( Dispatcher<int>::value<1>() == 5);
	static_assert( Dispatcher<int>::value<2>() == 4);
	static_assert( Dispatcher<int>::value<3>() == 4);
	static_assert( Dispatcher<int>::value<4>() == 4);
	static_assert( Dispatcher<int>::value<5>() == 4);
	static_assert( Dispatcher<int>::value<6>() == 4);
	static_assert( Dispatcher<int>::value<7>() == 4);
	static_assert( Dispatcher<int>::value<8>() == 1);

	static_assert( Dispatcher<long>::value<0>() == 5);
	static_assert( Dispatcher<long>::value<1>() == 5);
	static_assert( Dispatcher<long>::value<2>() == 4);
	static_assert( Dispatcher<long>::value<3>() == 4);
	static_assert( Dispatcher<long>::value<4>() == 4);
	static_assert( Dispatcher<long>::value<5>() == 4);
	static_assert( Dispatcher<long>::value<6>() == 4);
	static_assert( Dispatcher<long>::value<7>() == 4);
	static_assert( Dispatcher<long>::value<8>() == 4);

	static_assert( Dispatcher<float>::value<0>() == 5);
	static_assert( Dispatcher<float>::value<1>() == 5);
	static_assert( Dispatcher<float>::value<2>() == 5);
	static_assert( Dispatcher<float>::value<3>() == 5);
	static_assert( Dispatcher<float>::value<4>() == 3);
	static_assert( Dispatcher<float>::value<5>() == 3);
	static_assert( Dispatcher<float>::value<6>() == 2);
	static_assert( Dispatcher<float>::value<7>() == 2);
	static_assert( Dispatcher<float>::value<8>() == 2);

	static_assert( Dispatcher<double>::value<0>() == 5);
	static_assert( Dispatcher<double>::value<1>() == 5);
	static_assert( Dispatcher<double>::value<2>() == 5);
	static_assert( Dispatcher<double>::value<3>() == 5);
	static_assert( Dispatcher<double>::value<4>() == 5);
	static_assert( Dispatcher<double>::value<5>() == 5);
	static_assert( Dispatcher<double>::value<6>() == 2);
	static_assert( Dispatcher<double>::value<7>() == 2);
	static_assert( Dispatcher<double>::value<8>() == 2);

}
