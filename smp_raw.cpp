//https://mc-deltat.github.io/articles/stateful-metaprogramming-cpp20

#include <type_traits>
#include <concepts>

namespace N3{

	template<class Tag>
	struct DAG{

	// E1
	template<typename...>
	struct type_list {};


	// E2
	template<class TypeList, typename T>
	struct type_list_append;

	template<typename... Ts, typename T>
	struct type_list_append<type_list<Ts...>, T> {
		using type = type_list<Ts..., T>;
	};


	// E3
	template<unsigned N, typename List>
	struct state_t {
		static constexpr unsigned n = N;
		using list = List;
	};

	template<
		unsigned N//,
	>
	struct reader {
		friend auto state_func(reader<N>);
	};


	template<
		unsigned N,
		typename List = type_list<>//,
	>
	struct setter {
	 // E5
		friend auto state_func(reader<N>) {
			return List{};
		}

		static constexpr state_t<N, List> state{};
	};


	// E7
	template<
		auto EvalTag,
		unsigned N = 0
	>
	[[nodiscard]]
	static consteval auto get_state() {
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


	// E8
	template<
		auto EvalTag = []{},
		auto State = get_state<EvalTag>()
	>
	using get_list = typename std::remove_cvref_t<decltype(State)>::list;


	// E9
	template<
		typename T,
		auto EvalTag
	>
	[[nodiscard]]
	static consteval auto append_impl() {
		using cur_state = decltype(get_state<EvalTag>());            // E9.1
		using cur_list = typename cur_state::list;
		using new_list = typename type_list_append<cur_list, T>::type;      // E9.2
		setter<cur_state::n + 1, new_list> s;                        // E9.3
		return s.state;                                                     // E9.4
	}


	// E10
	template<
		typename T,
		auto EvalTag = []{},
		auto State = append_impl<T, EvalTag>()
	>
	using append = decltype(State);

	};

}

int main(){
	{
		using dag = N3::DAG<char>;
		sizeof(dag::setter<0, dag::type_list<>>);     // E6

		static_assert(std::same_as<dag::get_list<>, dag::type_list<>>);       // First usage in this translation unit
		dag::append<int>();
		static_assert(std::same_as<dag::get_list<>, dag::type_list<int>>);
		dag::append<float>();
		static_assert(std::same_as<dag::get_list<>, dag::type_list<int, float>>);
	}

	{
		using dag = N3::DAG<int>;
		sizeof(dag::setter<0, dag::type_list<>>);     // E6

		static_assert(std::same_as<dag::get_list<>, dag::type_list<>>);       // First usage in this translation unit
	}
}
