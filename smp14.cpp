#include "smp14.h"
#include "string_intern.h"

#include <iostream>

struct Tag0{};
struct Tag1{};
struct Tag2{};
struct Tag4{};
struct Tag8{};

#define Uq std::integral_constant<int, __COUNTER__>

#define Current2(dag) dag::Current<Uq>()
#define Next2(dag) dag::Next<Uq>()

int main(){
	// test different explicit MetaList tags and macro Pop&Append tags
	{
		using dag = list::MetaList<Tag0, list::type_list<char>>;
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<char>>::value);
		dag::Pop<Uq>();
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<>>::value);
	}

	{

		using dag = list::MetaList<Tag1>;

		static_assert(std::is_same<dag::Current<Uq>, list::type_list<>>::value);  
		dag::Append<int,Uq>();
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<int>>::value);
		dag::Append<float,Uq>();
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<int, float>>::value);
		dag::Append<list::type_list<float, double>,Uq>();
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<int, float, float, double>>::value);
		dag::Pop<Uq>();
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<int, float, float >>::value);
	}

	{
		using dag = list::MetaList<Tag2, list::type_list<char>>;
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<char>>::value);    
	}

	// test macro MetaList tag
	{
		using dag = list::MetaList<Uq, list::type_list<char>>;
		static_assert(std::is_same<dag::Current<Uq>, list::type_list<char>>::value);    
	}

	
	// test same MetaList tag
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<Uq>()==0);
		static_assert(dag::Next<Uq>::value==1);
		static_assert(dag::Next<Uq>()==2);
		static_assert(dag::Next<Uq>()==3);
		static_assert(dag::Next<Uq>()==4);
	}
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<Uq>()==4);
		static_assert(dag::Next<Uq>::value==5);
		static_assert(dag::Next<Uq>()==6);
		static_assert(dag::Next<Uq>()==7);
		static_assert(dag::Next<Uq>()==8);
	}
	{
		using dag = counter::Counter<Tag4, 3>;
		static_assert(Current2(dag)==3);
		static_assert(Next2(dag)==4);
		static_assert(Current2(dag)==4);
		static_assert(Next2(dag)==5);
		static_assert(Next2(dag)==6);
		static_assert(Next2(dag)==7);
	}

	// test tag captures intermediate state 
	{
		using dag = counter::Counter<Uq>;
		static_assert(dag::Current<Uq>()==0);
		using tag1 = Uq;
		static_assert(dag::Next<tag1>::value==1);
		static_assert(dag::Current<tag1>()==0); // last state
		static_assert(dag::Next<Uq>()==2);
		using tag2 = Uq;
		static_assert(dag::Next<tag2>()==3);
		static_assert(dag::Current<tag2>()==2); // last state
		static_assert(dag::Next<tag1>::value==1);
		static_assert(dag::Current<tag1>()==0); // last state
		static_assert(dag::Next<Uq>()==4);
		static_assert(dag::Next<tag2>()==3);
	}

}
