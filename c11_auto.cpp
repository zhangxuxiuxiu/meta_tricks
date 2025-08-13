#include <iostream>
#include <type_traits> // integral_constant, false_type, true_type

#if !defined(__clang__) && defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wnon-template-friend" 
#endif

// to resolve that decltype([]{}) will be cached as nested member's default template parameter in clang++ with std=c++20
#if __cplusplus >=202002L
	#define DeclareUniqueTag(Tag) auto Tag = []{}
	#define UniqueTag []{}
	#define UseTag(Tag) decltype(Tag)
#else
	#define DeclareUniqueTag(Tag) class Tag 
	#define UniqueTag std::integral_constant<int, __COUNTER__+28472937> 
	#define UseTag(Tag) Tag
#endif


namespace injector{

template<class Key> 
struct state_key;

template<class State>
struct state_value{
	using type = State;
};

template<class KeyType,class ValueType>
struct key_value{
	constexpr friend state_value<ValueType> mark_key_value(state_key<KeyType>){
		return {}; 
	}
};

template<class KeyType>
struct type_capture{
	template<class ValueType>
	type_capture(ValueType, typename std::enable_if<sizeof(key_value<KeyType,ValueType>)==1>::type* =nullptr){
//		static_assert(sizeof(key_value<KeyType,ValueType>)>0, "defined key_value");
	}
};

template<class Key> 
struct state_key {
	// no auto in c11, use type_capture<Key> here
	constexpr friend type_capture<Key> state_fn(state_key);
};


template<class Key, class State> 
struct state_injector{ 
	// no auto in c11, use type_capture<Key> here
	constexpr friend type_capture<Key> state_fn(state_key<Key>){
		return state_value<State>{};
	}
};


template<class Key, class State, class = bool>
struct Inject : std::integral_constant<bool, sizeof(state_injector<Key,State>) == 1>{
	using type = State;
};

template<class Key, class State >
struct Inject<Key, State, decltype(mark_key_value(state_key<Key>{}), true)> : std::false_type{}; 

// no use "bool=false" to avoid error that type of specialized non-type template argument depends on a template parameter of the partial specialization 
template<class Key, class EvalTag, class = bool>
struct has_state : std::false_type{};

template<class Key, class EvalTag>
struct has_state<Key, EvalTag, decltype(mark_key_value(state_key<Key>{}), false)> : std::true_type{}; 

// indirect to avoid error that default template arguments may not be used in partial specializations
template<class Key, DeclareUniqueTag(EvalTag)>
using HasState = has_state<Key, UseTag(EvalTag)>;

template<class Key>
//using StateOf = typename decltype(state_fn(state_key<Key>{}))::type;
using StateOf = typename decltype(mark_key_value(state_key<Key>{}))::type;

}

#if defined(__clang__) && __cplusplus < 201703L
# pragma clang diagnostic push 
# pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

int main(){
	static_assert(!injector::HasState<char, UniqueTag>::value);
	static_assert(injector::Inject<char,double>::value);
	static_assert(injector::HasState<char, UniqueTag>::value);
//	static_assert(std::is_same< injector::StateOf<char>, double>::value);
//	static_assert(!std::is_same< injector::StateOf<char>, float>::value);
//	static_assert(injector::Inject<char,double>::value);
//	static_assert(!injector::Inject<char,float>::value);
//	static_assert(injector::HasState<char, UniqueTag>::value);

#if __cplusplus >= 202002L
	static_assert(!injector::HasState<int>::value);
	static_assert(injector::Inject<int,double>::value);
	static_assert(injector::HasState<int>::value);
#endif
}

#if defined(__clang__) && __cplusplus < 201703L
# pragma clang diagnostic pop
#endif
