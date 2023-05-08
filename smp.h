//inspired by https://mc-deltat.github.io/articles/stateful-metaprogramming-cpp20

#pragma once

#include <type_traits>	// integral_constant

#include "injector.h"
#include "type_list.h"

namespace smp{

	template<DeclareUniqueTag(Tag), class InitialState = void>
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
		template< class EvalTag, unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			int Trigger = sizeof(typename injector::Inject<step<0>, state_t<0,InitialState>>::type) + sizeof(injector::StateOf<step<N>>) >
		static constexpr auto get_state(int) {
			return get_state<EvalTag, N + 1>(1);
		}

		template< class EvalTag, unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			int Trigger = sizeof( typename injector::Inject<step<0>, state_t<0,InitialState>>::type ) >
		static constexpr auto get_state(float) {
			return injector::StateOf< step<N-1> >{};
		}

	#else
		// 2: Current State 
		template< class EvalTag, unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			auto Trigger = sizeof( typename injector::Inject<step<0>, state_t<0,InitialState>>::type ) >
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
		template< DeclareUniqueTag(EvalTag),
			  class State = decltype(get_state<UseTag(EvalTag)>(0)) >
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

		template< template<class, class... > class Trans,
			  class Args, class EvalTag >
		static constexpr auto transform_impl() {
			using cur_state_t = decltype(get_state<EvalTag>(0));
			using cur_state = typename cur_state_t::state;
			using next_state = typename unpack_transform<cur_state, Trans, Args>::type;      // E9.2
			return typename injector::Inject< step<cur_state_t::n + 1>, state_t<cur_state_t::n+1, next_state> >::type{};                        // E9.3
		}

		template< template<class, class... > class Trans,
			  class Args, DeclareUniqueTag(EvalTag) >
		using Transform =  decltype(transform_impl<Trans, Args, UseTag(EvalTag)>());

	};

} // end of smp


namespace list{


	template<class T>
	struct to_list{
		using type  = traits::type_list<T>;
	};

	template<class... Args>
	struct to_list<traits::type_list<Args...>>{
		using type = traits::type_list<Args...>;
	};

	template<DeclareUniqueTag(Tag), class InitialState = traits::type_list<>>
	struct MetaList : smp::Msm<Tag, InitialState>{ 
		using base = smp::Msm<Tag, InitialState>;

		template<class Args, 
			 DeclareUniqueTag(EvalTag)>
		using Append = typename base::template Transform<traits::type_list_append, typename to_list<Args>::type, EvalTag>;

		template<DeclareUniqueTag(EvalTag)>
		using Pop = typename base::template Transform<traits::type_list_pop, traits::type_list<>, EvalTag>;
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

	template<DeclareUniqueTag(Tag), size_t N=0>
	struct Counter: smp::Msm<Tag, Index<N>>{ 
		using base = smp::Msm<Tag, Index<N>>;

		template<DeclareUniqueTag(EvalTag)>
		using Next = typename base::template Transform<next, traits::type_list<>, EvalTag>::state;
	};
}
