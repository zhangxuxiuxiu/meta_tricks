#include "smp20.h"
#include "string_intern.h"

#include <iostream>

struct Tag0{};
struct Tag1{};
struct Tag2{};
struct Tag4{};
struct Tag8{};

namespace counter{
	template<class Tag, size_t N=0>
	struct Counter2: smp::DAG<Tag, Index<N>>{ 
		using base = smp::DAG<Tag, Index<N>>;

		template<auto EvalTag>
		using Next = typename base::Transform<next, list::type_list<>, EvalTag>::state;
	};
}

#define Next2(dag) dag::Next<Uq{}>()

int main(){
	{
		using dag = list::MetaList<Tag0, list::type_list<char>>;
		static_assert(std::same_as<dag::Current<>, list::type_list<char>>);
		dag::Pop<>();
		static_assert(std::same_as<dag::Current<>, list::type_list<>>);
	}

	{

		using dag = list::MetaList<Tag1>;

		static_assert(std::same_as<dag::Current<>, list::type_list<>>);  
		dag::Append<int>();
		static_assert(std::same_as<dag::Current<>, list::type_list<int>>);
		dag::Append<float>();
		static_assert(std::same_as<dag::Current<>, list::type_list<int, float>>);
		dag::Append<list::type_list<float, double>>();
		static_assert(std::same_as<dag::Current<>, list::type_list<int, float, float, double>>);
		dag::Pop<>();
		static_assert(std::same_as<dag::Current<>, list::type_list<int, float, float >>);
	}

	{
		using dag = list::MetaList<Tag2, list::type_list<char>>;

		static_assert(std::same_as<dag::Current<>, list::type_list<char>>);    
	}
	
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<>()==0);
		static_assert(dag::Next<>::value==1);
		static_assert(dag::Next<>()==2);
		static_assert(dag::Next<>()==3);
		static_assert(Next2(dag)==4);
		
	}

	{
		using dag = counter::Counter2<Tag8>;
		std::cout << Next2(dag) << '\t' 
			<< Next2(dag) << '\n';
		static_assert(Next2(dag)==3);
		static_assert(Next2(dag)==4);
	}
}
