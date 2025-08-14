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
static auto Find(U const & container, KeyType const& key, OL<2>) -> decltype(container.find(key)) {
	cout << "container.find by " << key << '\n';
	return container.find(key);
}

//???: invalid to only return decltype(find(begin(container), end(container), key))
//answer: std::find specify return type as first argument type, so decltype(std::find...) will not trigger substitution error(see FindTry test) 
//template<class U, class KeyType> 
//static auto Find(U const& container, KeyType const& key, OL<1>) ->typename enable_if<is_same<KeyType, typename U::value_type>::value,decltype(find(begin(container), end(container), key) )>::type {
template<class U> 
static auto Find(U const& container, typename U::value_type const& key, OL<1>) ->decltype(begin(container)) {
	cout << "std::find by " << key << '\n';
	return find(begin(container), end(container), key);
}


template<class U, class Predicate>
static auto Find(U const& container, Predicate const& pred, OL<0>) -> decltype( pred(declval<typename U::value_type>()), begin(container) ) {
	cout << "std::find_if pred:" << pred << "\n";
	return find_if(begin(container), end(container), pred);
}

template<class Container,class KeyType>
static auto Find(Container const& container, KeyType const& key){
	return Find(container, key, OL<2>{});
}

struct equal3 {
	bool operator()(int v) const{
		return v==3;
	}	
};

ostream& operator<<( ostream& os, equal3 const& e){
	return os << " equal to 3 ";
}

template<class U, class KeyType>
static auto FindTry(U const& container, KeyType const& key) ->decltype(find(begin(container), end(container), key) ) {
	cout << "std::find by " << key << '\n';
	return find(begin(container), end(container), key);
}

int main(){
	//FindTry(vector<int>{},equal3{}) fail, but following is ok
	cout << typeid( decltype(FindTry(declval<vector<int>>(),equal3{})) ).name() << '\n';

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
	auto it4 = Find(set1, 3, OL<1>{}); // ==> std::find
	cout << "set got 3: " << *it4 << endl;

	// case 4: find_if
	auto it5 = Find(vec1, equal3{});
	cout << "vec find_if v==3: " << *it5 <<'\n';

	return 0;
}
