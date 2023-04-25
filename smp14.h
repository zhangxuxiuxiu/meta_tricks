#pragma once

#include <type_traits>

#ifdef __clang__

#elif defined( __GNUC__ )
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wnon-template-friend" 
#endif

namespace smp{

	template<class Tag, class InitialState>
	struct DAG{

		// 1: preparations 
		template<unsigned N, typename State>
		struct state_t {
			static constexpr unsigned n = N;
			using state = State;
		};

		template<
			unsigned N
		>
		struct reader {
			friend auto state_func(reader<N>);
		};


		template<
			unsigned N,
			typename State
		>
		struct setter {
		 // E5
			friend auto state_func(reader<N>) {
				return State{};
			}

			static constexpr state_t<N, State> state{};
		};

		// add EvalTag here to trigger each invocation rather than cached value
	//	template<class EvalTag, unsigned N>
	//	struct is_state_fn_defined{
	//		template<unsigned M>
	//		static constexpr bool test(float){return false;} 
	//		template<unsigned M, int= sizeof(state_func(reader<M>{}))>
	//		static constexpr bool test(int) {return true;}

	//		static constexpr bool value = test<N>(0);
	//	};

		// 2: Current State 
		template<
			class EvalTag,
			unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			int Trigger = sizeof(setter<0, InitialState>) + sizeof(state_func(reader<N>{}))
		>
		static constexpr auto get_state(int) {
			return get_state<EvalTag, N + 1>(1);
		}

		template<
			class EvalTag,
			unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			int Trigger = sizeof(setter<0, InitialState>)
		>
		static constexpr auto get_state(float) {
			return state_t<N - 1, decltype(state_func(reader<N-1>{}))>{};
		}


		template<
			class EvalTag,
			class State = decltype(get_state<EvalTag>(1))
		>
		//using Current = typename std::remove_cvref_t<State>::state;
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
			using cur_state_t = decltype(get_state<EvalTag>(1));            // E9.1
			using cur_state = typename cur_state_t::state;
			using next_state = typename unpack_transform<cur_state, Trans, Args>::type;      // E9.2
			return setter<cur_state_t::n + 1, next_state>{}.state;                        // E9.3
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

	template<class Tag, class InitialState = type_list<>>
	struct MetaList : smp::DAG<Tag, InitialState>{ 
		using base = smp::DAG<Tag, InitialState>;

		template<class Args, 
			 class EvalTag>
		using Append = typename base::template Transform<type_list_append, typename to_list<Args>::type, EvalTag>;

		template<class EvalTag>
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

	template<class Tag, size_t N=0>
	struct Counter: smp::DAG<Tag, Index<N>>{ 
		using base = smp::DAG<Tag, Index<N>>;

		template<class EvalTag>
		using Next = typename base::template Transform<next, list::type_list<>, EvalTag>::state;
	};
}

#ifdef __GNUC__
# pragma __GNUC__ diagnostic pop 
#endif
