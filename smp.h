#pragma once

#include <type_traits>

#include "injector.h"


namespace smp{

	template<class Tag, class InitialState>
	struct Msm{
		// 1: preparations 
		template<unsigned N>
		struct step{};

		template<unsigned N, typename State>
		struct state_t {
			static constexpr unsigned n = N;
			using state = State;
		};

	#if __cplusplus < 202002L
		// 2: Current State 
		template<
			class EvalTag,
			unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			int Trigger = sizeof(typename injector::Inject<step<0>, state_t<0,InitialState>>::type) + sizeof(injector::StateOf<step<N>>) 
		>
		static constexpr auto get_state(int) {
			return get_state<EvalTag, N + 1>(1);
		}

		template<
			class EvalTag,
			unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			int Trigger = sizeof( typename injector::Inject<step<0>, state_t<0,InitialState>>::type )
		>
		static constexpr auto get_state(float) {
			return injector::StateOf< step<N-1> >{};
		}

	#else
		// 2: Current State 
		template<
			class EvalTag,
			unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			auto Trigger = sizeof( typename injector::Inject<step<0>, state_t<0,InitialState>>::type )
		>
		[[nodiscard]]
		static constexpr auto get_state(int ) {
			constexpr bool counted_past_n = requires {
				injector::StateOf<step<N>>{};
			};

			if constexpr (counted_past_n) {
				return get_state<EvalTag, N + 1>(0);
			} else {
				return injector::StateOf< step<N-1> >{};
			}
		}

	#endif

		template<
			class DeclareUniqueType(EvalTag),
			class State = decltype(get_state<EvalTag>(0))
		>
		using Current = typename State::state;

		// 3: Transform to Next State
		template<class CurState, 
			 template<class , class... > class Trans,
			 class Args>
		struct unpack_transform;

		template<class CurState, 
			 template<class , class... > class Trans,
			 template<class... > class List,
			 class... Args>
		struct unpack_transform<CurState, Trans, List<Args...>>{
			using type = typename Trans<CurState, Args...>::type;
		};

		template<
			template<class, class... > class Trans,
			class Args,
			class EvalTag
		>
		static constexpr auto transform_impl() {
			using cur_state_t = decltype(get_state<EvalTag>(0));
			using cur_state = typename cur_state_t::state;
			using next_state = typename unpack_transform<cur_state, Trans, Args>::type;      // E9.2
			return typename injector::Inject< step<cur_state_t::n + 1>, state_t<cur_state_t::n+1, next_state> >::type{};                        // E9.3
		}

		template<
			template<class, class... > class Trans,
			class Args,
			class EvalTag 
		>
		using Transform =  decltype(transform_impl<Trans, Args, EvalTag>());

	};

} // end of smp


namespace list{

	template<typename...>
	struct type_list {};

	template<class TypeList1, class Typelist2>
	struct type_list_concat;

	template<typename... Ts, typename... U>
	struct type_list_concat<type_list<Ts...>, type_list<U...>> {
		using type = type_list<Ts..., U...>;
	};

	template<class TypeList1, class... Args>
	struct type_list_append{
      		using type = typename type_list_concat<TypeList1, type_list<Args...>>::type;
	};

	template<class T>
	struct to_list{
		using type  = type_list<T>;
	};

	template<class... Args>
	struct to_list<type_list<Args...>>{
		using type = type_list<Args...>;
	};


	template<class TypeList>
	struct type_list_pop;

	template<class T>
	struct type_list_pop<type_list<T>>{
		using type = type_list<>;
	};

	template<class U, class... Ts>
	struct type_list_pop<type_list<U, Ts...>>{
		using type = typename type_list_concat< type_list<U>, typename type_list_pop<type_list<Ts...>>::type >::type;
	};

//	template<>
//	struct type_list_pop<type_list<>>{
//		static_assert(false, "type_list_pop must have at least one element");
//	};


	template<class DeclareUniqueType(Tag), class InitialState = type_list<>>
	struct MetaList : smp::Msm<Tag, InitialState>{ 
		using base = smp::Msm<Tag, InitialState>;

		template<class Args, 
			 class DeclareUniqueType(EvalTag)>
		using Append = typename base::template Transform<type_list_append, typename to_list<Args>::type, EvalTag>;

		template<class DeclareUniqueType(EvalTag)>
		using Pop = typename base::template Transform<type_list_pop, type_list<>, EvalTag>;
	};

}

namespace counter{
	using std::size_t;

	template<size_t N>
	using Index = std::integral_constant<size_t, N>;
		
	template<class Cur>	
	struct next;
	
	template<size_t N>
	struct next<Index<N>>{
		using type = Index<N+1>;
	};

	template<class DeclareUniqueType(Tag), size_t N=0>
	struct Counter: smp::Msm<Tag, Index<N>>{ 
		using base = smp::Msm<Tag, Index<N>>;

		template<class DeclareUniqueType(EvalTag)>
		using Next = typename base::template Transform<next, list::type_list<>, EvalTag>::state;
	};
}
