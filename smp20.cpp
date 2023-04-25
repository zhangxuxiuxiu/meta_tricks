#include "smp20.h"
#include "string_intern.h"

#include <iostream>

struct Tag0{};
struct Tag1{};
struct Tag2{};
struct Tag4{};
struct Tag8{};
//
//namespace counter{
//	template<class Tag, size_t N=0>
//	struct Counter2: smp::DAG<Tag, Index<N>>{ 
//		using base = smp::DAG<Tag, Index<N>>;
//
//		template<auto EvalTag>
//		using Next = typename base::Transform<next, list::type_list<>, EvalTag>::state;
//	};
//}

#define Uq std::integral_constant<int, __COUNTER__>
#define Next2(dag) dag::Next<Uq{}>()

int main(){
	// test different tags and pop&append
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

	// test default tag
	{
		using dag = list::MetaList<>;
		dag::Append<char>();
		static_assert(std::same_as<dag::Current<>, list::type_list<char>>);    
	}
	{
		using dag = list::MetaList<>;
		dag::Append<double>();
		static_assert(std::same_as<dag::Current<>, list::type_list<double>>);    
	}
	
	// test same tag: Tag4
	{
		using dag = counter::Counter<Tag4>;
		static_assert(dag::Current<>()==0);
		static_assert(dag::Next<>::value==1);
		static_assert(dag::Next<>()==2);
		static_assert(dag::Next<>()==3);
		
	}
	{
		using dag = counter::Counter<Tag4>;
		static_assert(Next2(dag)==4);
		static_assert(Next2(dag)==5);
		static_assert(Next2(dag)==6);
	}
	{
		using dag = counter::Counter<Tag4, 1>;
		static_assert(Next2(dag)==2);
		static_assert(Next2(dag)==3);
		static_assert(Next2(dag)==4);
	}

	// test tag captures intermediate state 
	{
		using dag = counter::Counter<>;
		static_assert(dag::Current<>()==0);
		constexpr auto tag1 = Uq{};
		static_assert(dag::Next<tag1>::value==1);
		static_assert(dag::Current<>()==1);
		static_assert(dag::Current<tag1>()==0); // tag1 in Next mark lastin Current
		static_assert(dag::Next<>()==2);
		constexpr auto tag2 = []{};
		static_assert(dag::Next<tag2>()==3);
		static_assert(dag::Current<tag2>()==2); // tag2 in Next mark last state in Current
		static_assert(dag::Next<tag1>::value==1);
		static_assert(dag::Current<tag1>()==0); // tag1 in Next mark last in Current
		static_assert(dag::Next<>()==4);
		static_assert(dag::Next<tag2>()==3);
	}
}
