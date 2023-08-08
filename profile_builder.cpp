#include <iostream>
#include <type_traits>
#include <string>
#include <tuple>

// meta fn
template<size_t... Is>
struct Seq{};

template<class Seq, size_t I>
struct index_of;

template<size_t I>
struct index_of<Seq<>,I> : std::integral_constant<int, -1>{};

template<size_t I, size_t J, size_t... Ks>
struct index_of<Seq<J, Ks...>,I> : std::integral_constant<int, I==J?0:(index_of<Seq<Ks...>,I>::value==-1?-1:1+index_of<Seq<Ks...>,I>::value)>{};

template<class Seq, size_t I>
struct push_back;

template<template<size_t...>class Seq, size_t I, size_t... Js>
struct push_back<Seq<Js...>, I>{
	using type = Seq<Js..., I>;
};

template<size_t Index, size_t Bit>
struct indexed{
	static constexpr size_t index = Index;
	static constexpr size_t bit = Bit;
};

//template<class A, class B>
//struct concat_index;
//
//template<class... As, class... Bs>
//struct concat_index< std::tuple<As...>, std::tuple<Bs...> >{
//	using type = std::tuple<As..., indexed< (Bs::index+sizeof...(As)), Bs::bit>...>;
//};

template<class A, class B>
struct concat;

template<template<class...> class Seq, class... As, class... Bs>
struct concat< Seq<As...>, Seq<Bs...> >{
	using type = Seq<As..., Bs...>;
};

template<class S, size_t Start=0>
struct make_index;

template<template<size_t...> class ISeq, size_t Start>
struct make_index<ISeq<>,Start>{
	using type = std::tuple<>;
};

template<template<size_t...> class ISeq, size_t Start, size_t I, size_t... Js>
struct make_index<ISeq<I, Js...>, Start> : concat<  std::tuple< indexed<Start,I> >, typename make_index< Seq<Js...>, Start+1 >::type >{};

// start of profiler
constexpr static size_t s_age_bit	= 0x1 << 0; // adult or children 
constexpr static size_t s_name_bit	= 0x1 << 1; // anonymous or named 
constexpr static size_t s_gender_bit	= 0x1 << 2; // job differs
constexpr static size_t s_id_bit	= 0x7; 	// set all attributes 
						
struct Anonymous{
	std::string Name(){
		return "anonymous";
	}
};

struct Named{
	std::string name;
	std::string Name(){
		return name;
	}
};

struct Aged{
	int age;
	std::string Age(){
		return std::to_string(age);
	}
};


struct Gendered{
	bool m;
	std::string Personality(){
		return m?"hard":"soft";
	}
};

template<class... Bases>
struct Composer : public Bases... {};

template<class... Bases>
auto MakeComposer(Bases&&... bases){
	return Composer<Bases...>{ std::forward<Bases>(bases)... };
}

template<class Orders, class... Attrs>
struct ProfileBuilder{
		std::tuple<Attrs...> attrs;

		template<class S=Orders, bool B=(index_of<S,s_age_bit>::value==-1)>
		auto SetAge(int age)-> std::enable_if_t<B, ProfileBuilder<typename push_back<S,s_age_bit>::type, Attrs..., int>>{
			return {std::tuple_cat(attrs, std::make_tuple(age))};
		}

		template<class S=Orders, bool B=(index_of<S,s_name_bit>::value==-1)>
		auto SetName(std::string name)-> std::enable_if_t<B, ProfileBuilder<typename push_back<S,s_name_bit>::type, Attrs..., std::string>>{
			return {std::tuple_cat(attrs, std::make_tuple(name))};
		}

		template<class S=Orders, bool B=(index_of<S,s_gender_bit>::value==-1)>
		auto SetGender(bool male)-> std::enable_if_t<B, ProfileBuilder<typename push_back<S,s_gender_bit>::type, Attrs..., bool>>{
			return {std::tuple_cat(attrs, std::make_tuple(male))};
		}

		template<class S=Orders, bool B=std::is_same<S,Seq<>>::value>
		auto SetId(std::string idNo)-> std::enable_if_t<B, ProfileBuilder<Seq<s_age_bit,s_name_bit,s_gender_bit>, std::tuple<int,std::string,bool>>>{
			return {18,"id_name", true};
		}

		template<size_t Bit, class Attr>
		auto buildAttr(Attr attr){
			if constexpr(Bit == s_age_bit){
				return Aged{ .age = attr}; 
			} else if constexpr(Bit == s_name_bit){
				return Named{ .name = attr};
			} else {
				return Gendered{.m = attr};
			}
		}

		template<class... Is>
		auto helpBuild(std::tuple<Is...>){ 
			if constexpr(index_of<Orders,s_name_bit>::value==-1){
				return MakeComposer( Anonymous{}, buildAttr<Is::bit>(std::get<Is::index>(attrs))... ); 
			} else {
				return MakeComposer( buildAttr<Is::bit>(std::get<Is::index>(attrs))... ); 
			}
		}

		auto Build(){ 
			return helpBuild( typename make_index<Orders>::type{} );
		}

};

ProfileBuilder<Seq<>> InitProfile(){
	return {{}};
}

int main(){
	auto builder = InitProfile();
	{
		auto p1 = builder.SetAge(3).Build();
		static_assert(std::is_same<decltype(p1), Composer<Anonymous, Aged>>::value);
		std::cout <<p1.Name() << '\t' << p1.Age() << '\n';
	}

	{
		auto p1 = builder.SetName("only-name").Build();
		static_assert(std::is_same<decltype(p1), Composer<Named>>::value);
		std::cout << p1.Name() << '\n';
	}

	{
		auto p1 = builder.SetGender(true).Build();
		static_assert(std::is_same<decltype(p1), Composer<Anonymous, Gendered>>::value);
		std::cout << p1.Name() << '\t' << p1.Personality() << '\n';
	}

	{
		auto p2 = builder.SetAge(3).SetName("age-name").Build();
		static_assert(std::is_same<decltype(p2), Composer<Aged,Named>>::value);
		std::cout << p2.Age() << '\t' <<  p2.Name() << '\n';
	}

	{
		auto p2 = builder.SetAge(13).SetGender(true).Build();
		static_assert(std::is_same<decltype(p2), Composer<Anonymous, Aged,Gendered>>::value);
		std::cout << p2.Name() << '\t' << p2.Age() << '\t' << p2.Personality() << '\n';
	}

	{
		auto p2 = builder.SetName("name-gender").SetGender(true).Build();
		static_assert(std::is_same<decltype(p2), Composer<Named, Gendered>>::value);
		std::cout << p2.Name() << '\t' <<  p2.Personality() << '\n';
	}

	{
		auto p3 = builder.SetAge(13).SetName("age-name-gender").SetGender(false).Build();
		static_assert(std::is_same<decltype(p3), Composer<Aged,Named,Gendered>>::value);
		std::cout << p3.Age() << '\t' <<  p3.Name() << '\t' << p3.Personality() << '\n';

		auto p4 = builder.SetName("age-name-gender").SetAge(13).SetGender(false).Build();
		static_assert(std::is_same<decltype(p4), Composer<Named,Aged,Gendered>>::value);
		std::cout << p4.Age() << '\t' <<  p4.Name() << '\t' << p4.Personality() << '\n';

		auto p5 = builder.SetGender(false).SetAge(13).SetName("age-name-gender").Build();
		static_assert(std::is_same<decltype(p5), Composer<Gendered,Aged,Named>>::value);
		std::cout << p5.Age() << '\t' <<  p5.Name() << '\t' << p5.Personality() << '\n';
	}


	return 0;
}
