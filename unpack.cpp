#include <tuple>
#include <type_traits>
#include <iostream>

#ifdef __clang__
#elif defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wnon-template-friend" 
#endif

namespace unpack{

	using std::size_t;

	template<class, class>
	struct index_concat;

	template<size_t... Is, size_t... Js>
	struct index_concat<std::index_sequence<Is...>, std::index_sequence<Js...>>{
		using type = std::index_sequence<Is..., Js...>;
	};

	template<class Seq>
	struct index_push;

	template<size_t... Is>
	struct index_push<std::index_sequence<Is...>>{
		using type = std::index_sequence<Is..., sizeof...(Is)>;
	};

	template<class Seq>
	struct index_pop;

	template<size_t J>
	struct index_pop<std::index_sequence<J>>{
		using type = std::index_sequence<>;
	};

	template<size_t J, size_t... Is>
	struct index_pop<std::index_sequence<J, Is...>>{
		using type = typename index_concat<std::index_sequence<J>, typename index_pop<std::index_sequence<Is...>>::type >::type;
	};

	template <class T>
	constexpr T unsafe_declval() noexcept {
//	    report_if_you_see_link_error_with_this_function();
	
	    typename std::remove_reference<T>::type* ptr = 0;
	    ptr += 42; // suppresses 'null pointer dereference' warnings
	    return static_cast<T>(*ptr);
	}

	template<class T>
	struct Unpacker{

		template<size_t N> 
		struct field_id : std::integral_constant<size_t,N>{ 
			constexpr friend auto field_type(field_id<N>);
		};

		template<size_t N, class U> 
		struct	field_marker{ 
			constexpr friend auto field_type(field_id<N>){
				return U{};
			}
		};

		template <size_t N>
		struct type_detector {
		    template<class U, std::size_t = sizeof(field_marker<N,U>)>
		    constexpr operator U() const&& noexcept{
			return unsafe_declval<U>();
		    }; 
		};

		// deduct tuple type

		template<size_t... Is>
		static constexpr auto as_tuple_help(std::index_sequence<Is...>){
			return std::tuple<decltype(field_type(field_id<Is>{}))... > {};
		}

#if __cplusplus >= 202002L

		template<size_t... Is>
		static constexpr auto as_tuple() {
			constexpr auto succ = requires{ T{ type_detector<Is>{}... }; };
			if constexpr (succ){ 
				return as_tuple<Is..., sizeof...(Is)>();
			} else {
				return as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{});
			}
		}
		
		using tuple_type = decltype(as_tuple<>());

#elif __cplusplus >= 201703L

		template<size_t... Is>
		struct is_constructible{
			template<size_t... Js>
			static constexpr auto test(float){return false;} 
			template<size_t... Js>
			static constexpr auto test(int) -> decltype(T{ type_detector<Js>{}...}, true) {
				return true;
			}

			static constexpr bool value = test<Is...>(0);
		};

		template<size_t... Is>
		static constexpr auto as_tuple() {
			if constexpr (is_constructible<Is...>::value){ 
				return as_tuple<Is..., sizeof...(Is)>();
			} else {
				return as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{});
			}
		}
		
		using tuple_type = decltype(as_tuple<>());

#elif __cplusplus >= 201402L

		// TRY ONE: enable if
//		template<class Seq>
//		struct is_constructible{
//			template<size_t... Js>
//			static constexpr auto test(std::index_sequence<Js...>, float){return false;} 
//			template<size_t... Js>
//			static constexpr auto test(std::index_sequence<Js...>, int) -> decltype(T{ type_detector<Js>{}...}, true) {
//				return true;
//			}
//
//			static constexpr bool value = test(Seq{}, 0);
//		};
//
//		template<class Seq, class PrevSeq=typename index_pop<Seq>::type>
//		static constexpr auto as_tuple() -> typename std::enable_if<!is_constructible<Seq>::value, decltype(as_tuple_help(PrevSeq{}))>::type{ 
//			return as_tuple_help(PrevSeq{});
//		}
//
//		template<class Seq, class NextSeq=typename index_push<Seq>::type>
//		static constexpr auto as_tuple() -> typename std::enable_if<is_constructible<Seq>::value, decltype(as_tuple<NextSeq>())>::type{ 
//			return as_tuple<NextSeq>();
//		}
//
//		using tuple_type = decltype(as_tuple<std::index_sequence<>>());

		// TRY TWO: all tuple_type is tuple<> 
//		template<size_t... Is>
//		static constexpr auto as_tuple(float){ 
//			return as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{});
//		}
//
//		template<size_t... Is>
//		static constexpr auto as_tuple(int) -> decltype(T{ type_detector<Is>{}...}, as_tuple<Is..., sizeof...(Is)>(0)){ 
//			return as_tuple<Is..., sizeof...(Is)>(0);
//		}
//
//		using tuple_type = decltype(as_tuple<>(0));

		//TRY THREE : default template parameter like get_state in smp
		//problem: int = sizeof(construct(Seq{})) fail to work
//		template<class Seq>
//		static constexpr auto as_tuple(float){ 
//			return as_tuple_help(typename index_pop<Seq>::type{});
//		}
//
//		template<size_t... Is>
//		static constexpr auto construct(std::index_sequence<Is...>){
//			return T{ type_detector<Is>{}...};
//		}
//
//		template<class Seq, class = decltype(construct(Seq{}))>
//		static constexpr auto as_tuple(int) { //
//			return as_tuple<typename index_push<Seq>::type>(0);
//		}
//
//		using tuple_type = decltype(as_tuple<std::index_sequence<>>(0));
//

//		// TRY FOUR
		template<class Seq>
		struct is_constructible{
			template<size_t... Js>
			static constexpr auto test(std::index_sequence<Js...>, float){return false;} 
			template<size_t... Js>
			static constexpr auto test(std::index_sequence<Js...>, int) -> decltype(T{ type_detector<Js>{}...}, true) {
				return true;
			}

			static constexpr bool value = test(Seq{}, 0);
		};

		template<class Seq, bool = is_constructible<Seq>::value >
		struct tuple_expand{
			using type = typename tuple_expand< typename index_push<Seq>::type >::type;	
		};

		template<size_t... Is>
		struct tuple_expand<std::index_sequence<Is...>, false>{
			using type =  decltype(as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{}));
		};

		using tuple_type = typename tuple_expand<std::index_sequence<>>::type;

#else 
#	error __cplusplus must be at least 201402L
#endif

	};

}

#ifdef __clang__
#elif defined(__GNUC__)
# pragma GCC diagnostic pop
#endif 

struct empty{};
struct one{ int x; };
struct two{ long x; float y; };
struct three{ char x; double y; float z;};

int main(){
//	static_assert(is_constructible<empty,std::index_sequence<>>::value);
//	static_assert(is_constructible<one,std::index_sequence<0>>::value);
	//static_assert(unpack::Unpacker<one>::is_contructiable<std::index_sequence<0>>::value);
//	sizeof(typename unpack::Unpacker<empty>::template type_detector<0>{});
//	sizeof(unpack::Unpacker<empty>::construct(std::index_sequence<>{}));
//	sizeof(unpack::Unpacker<one>::construct(std::index_sequence<0>{}));
//	using next_seq = typename unpack::index_push<std::index_sequence<>>::type;
//	static_assert(std::is_same<std::index_sequence<>, typename unpack::index_pop<next_seq>::type>::value);

	static_assert(std::is_same<std::tuple<>, typename unpack::Unpacker<empty>::tuple_type>::value);
	static_assert(std::is_same<std::tuple<int>, typename unpack::Unpacker<one>::tuple_type>::value);
	static_assert(std::is_same<std::tuple<long,float>, typename unpack::Unpacker<two>::tuple_type>::value);
	static_assert(std::is_same<std::tuple<char,double, float>, typename unpack::Unpacker<three>::tuple_type>::value);
	std::cout << typeid(typename unpack::Unpacker<one>::tuple_type).name() << '\n';
	std::cout << typeid(typename unpack::Unpacker<two>::tuple_type).name() << '\n';
	std::cout << typeid(typename unpack::Unpacker<two>::tuple_type).name() << '\n';
	std::cout << typeid(typename unpack::Unpacker<three>::tuple_type).name() << '\n';
}
