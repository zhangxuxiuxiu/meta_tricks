#pragma once

#include <type_traits>
#include <concepts>

#ifdef __GNUC__
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

		// 2: Current State 
		template<
			auto EvalTag,
			unsigned N = 0,
			// Instantiation of initial state is triggered for sure, because $Current&$Transform both invoke $get_state
			auto Trigger = sizeof(setter<0, InitialState>)
		>
		[[nodiscard]]
		static constexpr auto get_state() {
			constexpr bool counted_past_n = requires(reader<N> r) {
				state_func(r);
			};

			if constexpr (counted_past_n) {
				return get_state<EvalTag, N + 1>();
			} else {
				constexpr reader<N - 1> r;
				return state_t<N - 1, decltype(state_func(r))>{};
			}
		}


		template<
			auto EvalTag = []{},
			auto State = get_state<EvalTag>()
		>
		using Current = typename std::remove_cvref_t<decltype(State)>::state;

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
			auto EvalTag
		>
		[[nodiscard]]
		static consteval auto transform_impl() {
			using cur_state_t = decltype(get_state<EvalTag>());            // E9.1
			using cur_state = typename cur_state_t::state;
			using next_state = typename unpack_transform<cur_state, Trans, Args>::type;      // E9.2
			setter<cur_state_t::n + 1, next_state> s;                        // E9.3
			return s.state;                                                     // E9.4
		}

		template<
			template<class, class... > class Trans,
			class Args,
			auto EvalTag //= []{}
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

	template<class Tag = decltype([]{}), class InitialState = type_list<>>
	struct MetaList : smp::DAG<Tag, InitialState>{ 
		using base = smp::DAG<Tag, InitialState>;

		template<class Args, 
			 auto EvalTag= []{}>
		using Append = typename base::template Transform<type_list_append, typename to_list<Args>::type, EvalTag>;

		template<auto EvalTag = []{}>
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

	template<class Tag = decltype([]{}), size_t N=0>
	struct Counter: smp::DAG<Tag, Index<N>>{ 
		using base = smp::DAG<Tag, Index<N>>;

		template<auto EvalTag= []{}>
		using Next = typename base::template Transform<next, list::type_list<>, EvalTag>::state;
	};
}

#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
