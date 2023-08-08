#include <iostream>
#include <type_traits>
#include <string>
#include <tuple>

#include "type_list.h"

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

using namespace traits;

template<class Orders, class... Attrs>
struct ProfileBuilder{
		std::tuple<Attrs...> attrs;

		template<class S=Orders, bool B=(index_of<S,Index<s_age_bit>>::value==-1)>
		auto SetAge(int age) const -> std::enable_if_t<B, ProfileBuilder<typename type_list_append<S,Index<s_age_bit>>::type, Attrs..., int>> {
			return {std::tuple_cat(attrs, std::make_tuple(age))};
		}

		template<class S=Orders, bool B=(index_of<S,Index<s_name_bit>>::value==-1)>
		auto SetName(std::string name) const -> std::enable_if_t<B, ProfileBuilder<typename type_list_append<S,Index<s_name_bit>>::type, Attrs..., std::string>>{
			return {std::tuple_cat(attrs, std::make_tuple(name))};
		}

		template<class S=Orders, bool B=(index_of<S,Index<s_gender_bit>>::value==-1)>
		auto SetGender(bool male) const -> std::enable_if_t<B, ProfileBuilder<typename type_list_append<S,Index<s_gender_bit>>::type, Attrs..., bool>> {
			return {std::tuple_cat(attrs, std::make_tuple(male))};
		}

		template<class S=Orders, bool B=size_of<S>::value==0>
		auto SetId(std::string idNo) const -> std::enable_if_t<B, ProfileBuilder<type_list<Index<s_age_bit>,Index<s_name_bit>,Index<s_gender_bit>>, std::tuple<int,std::string,bool>>> {
			return {18,"id_name", true};
		}

		template<size_t Bit, class Attr>
		auto buildAttr(Attr attr) const{
			if constexpr(Bit == s_age_bit){
				return Aged{ .age = attr}; 
			} else if constexpr(Bit == s_name_bit){
				return Named{ .name = attr};
			} else {
				return Gendered{.m = attr};
			}
		}

		template<class... Is>
		auto helpBuild(type_list<Is...>) const{ 
			if constexpr(index_of<Orders,Index<s_name_bit>>::value==-1){
				return MakeComposer( Anonymous{}, buildAttr<Is::type::value>(std::get<Is::index>(attrs))... ); 
			} else {
				return MakeComposer( buildAttr<Is::type::value>(std::get<Is::index>(attrs))... ); 
			}
		}

		auto Build() const{ 
			return helpBuild( typename index_list<Orders>::type{} );
		}

};

static constexpr ProfileBuilder<type_list<>> builder;

int main(){
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
