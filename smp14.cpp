#include "smp14.h"
#include "string_intern.h"

#include <iostream>

struct Tag0{};
struct Tag1{};
struct Tag2{};
struct Tag4{};
struct Tag8{};

#define Next2(dag) dag::Next<Uq>()

int main(){
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
	
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<Uq>()==0);
		static_assert(dag::Next<Uq>::value==1);
		static_assert(dag::Next<Uq>()==2);
		static_assert(dag::Next<Uq>()==3);
		static_assert(Next2(dag)==4);
		
	}

}
