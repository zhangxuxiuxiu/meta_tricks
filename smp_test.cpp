#include "smp.h"

#include <iostream>

struct Tag0{};
struct Tag1{};
struct Tag2{};
struct Tag4{};
struct Tag8{};


#define Current2(dag) dag::Current<UniqueType>()
#define Next2(dag) dag::Next<UniqueType>()

#if defined(__clang__) && __cplusplus < 201703L
# pragma clang diagnostic push 
# pragma clang diagnostic ignored "-Wc++17-extensions"
#endif

int main(){
	// test different explicit MetaList tags and macro Pop&Append tags
	{
		using dag = list::MetaList<Tag0, list::type_list<char>>;
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<char>>::value);
		dag::Pop<UniqueType>();
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<>>::value);
	}

	{

		using dag = list::MetaList<Tag1>;

		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<>>::value);  
		dag::Append<int,UniqueType>();
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<int>>::value);
		dag::Append<float,UniqueType>();
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<int, float>>::value);
		dag::Append<list::type_list<float, double>,UniqueType>();
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<int, float, float, double>>::value);
		dag::Pop<UniqueType>();
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<int, float, float >>::value);
	}

	{
		using dag = list::MetaList<Tag2, list::type_list<char>>;
		static_assert(std::is_same<dag::Current<UniqueType>, list::type_list<char>>::value);    
	}

	
	// test same MetaList tag
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<UniqueType>()==0);
		static_assert(dag::Next<UniqueType>::value==1);
		static_assert(dag::Next<UniqueType>()==2);
		static_assert(dag::Next<UniqueType>()==3);
		static_assert(dag::Next<UniqueType>()==4);
	}
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<UniqueType>()==4);
		static_assert(dag::Next<UniqueType>::value==5);
		static_assert(dag::Next<UniqueType>()==6);
	}
	{
		using dag = counter::Counter<Tag4, 3>;
		static_assert(Current2(dag)==3);
		static_assert(Next2(dag)==4);
		static_assert(Current2(dag)==4);
		static_assert(Next2(dag)==5);
	}

	// test tag captures intermediate state 
	{
		using dag = counter::Counter<UniqueType>;
		static_assert(dag::Current<UniqueType>()==0);
		using tag1 = UniqueType;
		static_assert(dag::Next<tag1>::value==1);
		static_assert(dag::Current<tag1>()==0); // last state
		static_assert(dag::Next<UniqueType>()==2);
		using tag2 = UniqueType;
		static_assert(dag::Next<tag2>()==3);
		static_assert(dag::Current<tag2>()==2); // last state
		static_assert(dag::Next<tag1>::value==1);
		static_assert(dag::Current<tag1>()==0); // last state
		static_assert(dag::Next<UniqueType>()==4);
		static_assert(dag::Next<tag2>()==3);
	}

#if !defined(__clang__) && __cplusplus >= 202002L
	// test default tag
	{
		using dag = list::MetaList<>;
		dag::Append<char>();
		static_assert(std::same_as<dag::Current<>, list::type_list<char>>);    
		dag::Append<char>();
		static_assert(std::same_as<dag::Current<>, list::type_list<char,char>>);    
		dag::Pop<>();
		static_assert(std::same_as<dag::Current<>, list::type_list<char>>);    
	}
	{
		using dag = counter::Counter<>;
		static_assert(dag::Current<>()==0);
		static_assert(dag::Next<>()==1);
		static_assert(dag::Current<>()==1);
	}
#endif
}

#if defined(__clang__) && __cplusplus < 201703L
# pragma clang diagnostic pop
#endif
