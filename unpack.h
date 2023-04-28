#pragma once

#include <tuple>
#include <type_traits>
#include "friend_injector.h"

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
	struct TupleLike{

		template<size_t N>
		struct field_id{};

		template <size_t N>
		struct type_detector {
		    template<class U, bool = injector::Inject<field_id<N>, U>::value>
		    constexpr operator U() const&& noexcept{
			return unsafe_declval<U>();
		    }; 
		};

		template<class Seq, class = T>
		struct is_constructible : std::false_type {};

		template<size_t... Is>
		struct is_constructible<std::index_sequence<Is...>, decltype(T{ type_detector<Is>{}...})> : std::true_type {};

		//template<size_t... Is>
		//inline constexpr bool is_constructible_v =typename is_constructible<std::index_sequence<Is...>>::value;

		// deduct tuple type

		template<size_t... Is>
		static constexpr auto as_tuple_help(std::index_sequence<Is...>){
			return std::tuple<injector::StateOf<field_id<Is>>... > {};
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
		
		using type = decltype(as_tuple<>());

#elif __cplusplus >= 201703L

		template<size_t... Is>
		static constexpr auto as_tuple() {
			if constexpr (is_constructible<std::index_sequence<Is...>>::value){ 
				return as_tuple<Is..., sizeof...(Is)>();
			} else {
				return as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{});
			}
		}
		
		using type = decltype(as_tuple<>());

#elif __cplusplus >= 201402L

		// TRY ONE: enable if on parameter type 
//
//		template<class Seq, class PrevSeq=typename index_pop<Seq>::type>
//		static constexpr auto as_tuple(typename std::enable_if<!is_constructible<Seq>::value, int>::type = 1) { 
//			return as_tuple_help(PrevSeq{});
//		}
//
//		template<class Seq, class NextSeq=typename index_push<Seq>::type>
//		static constexpr auto as_tuple(typename std::enable_if<is_constructible<Seq>::value, int>::type = 1){ 
//			return as_tuple<NextSeq>();
//		}
//
//		using type = decltype(as_tuple<std::index_sequence<>>());

		// TRY TWO: enable_if  on default template type
//		template<class Seq, class PrevSeq=typename index_pop<Seq>::type>
//		static constexpr auto as_tuple(float) { 
//			return as_tuple_help(PrevSeq{});
//		}
//	
//		template<class Seq, class NextSeq=typename index_push<Seq>::type, class=typename std::enable_if<is_constructible<Seq>::value>::type >
//		static constexpr auto as_tuple(int){ 
//			return as_tuple<NextSeq>(0);
//		}
//	
//		using type = decltype(as_tuple<std::index_sequence<>>(0));
//

		// TRY THREE: overload function & SFINE on parameter type 
		template<size_t... Is>
		static constexpr auto as_tuple(float){ 
			return as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{});
		}

		template<size_t... Is>
		static constexpr auto as_tuple( decltype( T{ type_detector<Is>{}...}, 0) ) {
			return as_tuple<Is..., sizeof...(Is)>(0);
		}

		using type = decltype(as_tuple<>(0));

		//TRY FOUR: overload function, SFINE on return type and default template parameter using sizeof
//		template<class Seq>
//		static constexpr auto as_tuple(float){ 
//			return as_tuple_help(typename index_pop<Seq>::type{});
//		}
//
//		template<size_t... Is> // fail without decltype(T{ type_detector<Is>{}...})
//		static constexpr auto construct(std::index_sequence<Is...>) -> decltype(T{ type_detector<Is>{}...}){
//			return T{ type_detector<Is>{}...};
//		}
//
//		template<class Seq, class = decltype(construct(Seq{}))>
//		static constexpr auto as_tuple(int) { 
//			return as_tuple<typename index_push<Seq>::type>(0);
//		}
//
//		using type = decltype(as_tuple<std::index_sequence<>>(0));

		// TRY FIVE: specialize type
//		template<class Seq, bool = is_constructible<Seq>::value >
//		struct tuple_expand{
//			using type = typename tuple_expand< typename index_push<Seq>::type >::type;	
//		};
//
//		template<size_t... Is>
//		struct tuple_expand<std::index_sequence<Is...>, false>{
//			using type =  decltype(as_tuple_help(typename index_pop<std::index_sequence<Is...>>::type{}));
//		};
//
//		using type = typename tuple_expand<std::index_sequence<>>::type;
//
		// TRY SIX: specialize type with default template parameter 
//		template<class Seq, class = T>
//		struct tuple_expand{
//			using type =  decltype(as_tuple_help(typename index_pop<Seq>::type{}));
//		};
//
//		template<size_t... Is>
//		struct tuple_expand<std::index_sequence<Is...>, decltype(T{ type_detector<Is>{}...})>{
//			using type = typename tuple_expand<std::index_sequence<Is..., sizeof...(Is)> >::type;	
//		};
//
//		using type = typename tuple_expand<std::index_sequence<>>::type;


#else 
#	error __cplusplus must be at least 201402L
#endif

	};

}
