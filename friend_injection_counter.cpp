//https://ledas.com/post/857-how-to-hack-c-with-templates-and-friends/

// access private members
class Private {
  int data;
};

template<int Private::* Member>
struct Stealer {
  friend int& dataGetter(Private& iObj) {
    return iObj.*Member;
  }
};

template struct Stealer<&Private::data>;
int& dataGetter(Private&);	//redeclare in global ns


// meta counter
template<int>                                         
struct Flag {
	friend constexpr bool flag(Flag);
};

template<int N>
struct Writer {
	friend constexpr bool flag(Flag<N>) { 
		return true; 
	}
	static constexpr int value = N;
};

template<int N = 0, auto EvalTag>
constexpr int reader(float) {
	return Writer<N>::value;
}

template<int N = 0,
	auto EvalTag,
	bool = flag(Flag<N>{})
>
constexpr int reader(int) {
	return reader<N + 1, EvalTag>(int{});
}

template<auto EvalTag = []{}, 
	int R = reader<0, EvalTag>(int{})
>
constexpr int next() {
	return R;
}


int main(){
	Private obj;
	dataGetter(obj) = 31;

	static_assert(next() == 0);
	static_assert(next() == 1);
	static_assert(next() == 2);
}
