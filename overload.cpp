#include <iostream>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <set>
#include <map>

using namespace std;

template<size_t N>
struct OL : OL<N-1>{};

template<>
struct OL<0>{};

/*
    for all $impl that meets enable_if, the highest OL verison is choosen  
*/
template<class T>
struct Dispatcher{

	// lowest priority
	template<class U>
	static constexpr size_t impl(OL<0>){
		return 5;
	}
	
	template<class U>
	static constexpr size_t impl(typename enable_if< is_integral<U>::value, OL<2>>::type ){
		return 4;
	}
	
	template<class U>
	static constexpr size_t impl(typename enable_if< is_same<U, float>::value, OL<4>>::type ){
		return 3;
	}

	template<class U>
	static constexpr size_t impl(typename enable_if< is_floating_point<U>::value, OL<6>>::type ){
		return 2;
	}

	// highest priority
	template<class U>
	static constexpr size_t impl(typename enable_if< is_same<U, int>::value, OL<8>>::type ){
		return 1;
	}

	template<size_t N>
	static constexpr size_t value(){
		return impl<T>(OL<N>{});
	}
};


/*
invoke the highest OL version	
*/
template<class U, class KeyType>
static auto Find(U const & container, KeyType const& key, OL<1>) -> decltype(container.find(key)) {
	cout << "container.find by " << key << '\n';
	return container.find(key);
}

template<class U, class KeyType>
static auto Find(U const& container, KeyType const& key, OL<0>) -> decltype( find(begin(container), end(container), key) ) {
	cout << "std::find by " << key << '\n';
	return find(begin(container), end(container), key);
}

template<class Container,class KeyType>
static auto Find(Container const& container, KeyType const& key){
	return Find(container, key, OL<10>{});
}

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

	//test cases for  Finder 
	// case 1: no find in vector
	vector<int> vec1 = {1,2,3};	
	auto it = Find(vec1, 3);  // ==> std::find
	cout << "vector got 3: " << *it << endl;
	auto it1 = Find(vec1, 3, OL<1>{}); // ==> std::find
	cout << "vector got 3: " << *it1 << endl;

	// case 2: find in map, std::find not support
	map<int,int> m2 = {{1,-1}, {2,-2}};
	auto it2 = Find(m2, 2); // ==> map::find
	cout << "map got -2: " << it2->second << endl;

	// case 3: find in set, std::find support too
	set<int> set1 = {1,2,3};	
	auto it3 = Find(set1, 3); // ==> set::find
	cout << "set got 3: " << *it3 << endl;
	auto it4 = Find(set1, 3, OL<0>{}); // ==> std::find
	cout << "set got 3: " << *it4 << endl;

	return 0;
}
