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

template<class A, class B>
struct concat_index;

template<class... As, class... Bs>
struct concat_index< std::tuple<As...>, std::tuple<Bs...> >{
	using type = std::tuple<As..., indexed< (Bs::index+sizeof...(As)), Bs::bit>...>;
};

template<class S>
struct make_index;

template<>
struct make_index<Seq<>>{
	using type = std::tuple<>;
};

template<size_t I, size_t... Js>
struct make_index<Seq<I, Js...>> : concat_index<  std::tuple< indexed<sizeof...(Js),I> >, typename make_index< Seq<Js...> >::type >{};

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
struct Composer : Bases... {};

template<class... Bases>
auto MakeComposer(Bases&&... bases){
	return Composer<Bases...>{ std::forward<Bases>(bases)... };
}

template<class Orders, class... Attrs>
struct ProfileBuilder{
		std::tuple<Attrs...> attrs;

		template<class S=Orders, bool B=(index_of<S,s_age_bit>::value==-1)>
		auto SetAge(int age)-> std::enable_if_t<B, ProfileBuilder<typename push_back<S,s_age_bit>::type, std::tuple<Attrs..., int>>>{
			return {std::tuple_cat(attrs, std::make_tuple(age))};
		}

		template<class S=Orders, bool B=(index_of<S,s_name_bit>::value==-1)>
		auto SetName(std::string name)-> std::enable_if_t<B, ProfileBuilder<typename push_back<S,s_name_bit>::type, std::tuple<Attrs..., std::string>>>{
			return {std::tuple_cat(attrs, std::make_tuple(name))};
		}

		template<class S=Orders, bool B=(index_of<S,s_gender_bit>::value==-1)>
		auto SetGender(bool male)-> std::enable_if_t<B, ProfileBuilder<typename push_back<S,s_gender_bit>::type, std::tuple<Attrs..., bool>>>{
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
			return MakeComposer( buildAttr<Is::bit>(std::get<Is::index>(attrs))... ); 
		}

		auto Build(){ 
			return helpBuild( typename make_index<Orders>::type{} );
		}

};

ProfileBuilder<Seq<>> Profiler(){
	return {{}};
}

int main(){
	auto builder = Profiler();
	auto p1 = builder.SetAge(3).Build();
	return 0;
}
