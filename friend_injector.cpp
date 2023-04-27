#include <type_traits>

struct StateRepo{
	template<class UKey> 
	struct StateKey {
		constexpr friend auto state_fn(StateKey);
	};

	template<class Key, class State> 
	struct	StateWriter{ 
		constexpr friend auto state_fn(Key){
			return State{};
		}
	};

	template<class Key, class State>
	struct Save {
		static constexpr bool value = (sizeof(StateWriter<StateKey<Key>,State>), true);		
	};

	template<class Key>
	struct Get{
		using type = decltype(state_fn(StateKey<Key>{}));
	}; 
};

int main(){
	StateRepo::Save<int,double>::value;
	static_assert(std::is_same<typename StateRepo::Get<int>::type, double>::value);

	StateRepo::Save<char,double>::value;
	static_assert(std::is_same<typename StateRepo::Get<char>::type, double>::value);
	static_assert(!std::is_same<typename StateRepo::Get<char>::type, float>::value);

	static_assert(!std::is_same<typename StateRepo::Get<float>::type, float>::value);
}
