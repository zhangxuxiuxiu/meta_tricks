#pragma once

#include <tuple>

#include "type_list.h"
#include "injector.h"

namespace unpack{

	template <class T>
	constexpr T unsafe_declval() noexcept {
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
				return as_tuple_help(typename traits::index_pop<std::index_sequence<Is...>>::type{});
			}
		}
		
		using type = decltype(as_tuple<>());

#elif __cplusplus >= 201703L
		template<class Seq, class = T>
		struct is_constructible : std::false_type {};

		template<size_t... Is>
		struct is_constructible<std::index_sequence<Is...>, decltype(T{ type_detector<Is>{}...})> : std::true_type {};

		template<size_t... Is>
		static constexpr auto as_tuple() {
			if constexpr (is_constructible<std::index_sequence<Is...>>::value){ 
				return as_tuple<Is..., sizeof...(Is)>();
			} else {
				return as_tuple_help(typename traits::index_pop<std::index_sequence<Is...>>::type{});
			}
		}
		
		using type = decltype(as_tuple<>());

#elif __cplusplus >= 201402L
		template<size_t... Is>
		static constexpr auto as_tuple(float){ 
			return as_tuple_help(typename traits::index_pop<std::index_sequence<Is...>>::type{});
		}

		template<size_t... Is>
		static constexpr auto as_tuple( decltype( T{ type_detector<Is>{}...}, 0) ) {
			return as_tuple<Is..., sizeof...(Is)>(0);
		}

		using type = decltype(as_tuple<>(0));
#else 
#	error __cplusplus must be at least 201402L
#endif
	};
}
