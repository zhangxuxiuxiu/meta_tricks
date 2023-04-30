#pragma once

namespace traits{

	// all_same
	template< template< class > class Trans, class... Ts>
	struct all_same;
	
	template< template< class > class Trans, class T>
	struct all_same<Trans, T>{
		using type = typename Trans<T>::type;
	};
	
	template< template< class > class Trans, class T, class... Us>
	struct all_same<Trans, T, Us...>{
		using first_type = typename Trans<T>::type;
		using type = typename std::enable_if< std::is_same< first_type, typename all_same< Trans, Us... >::type >::value, first_type >::type;
	};

	// nth_element
	template<size_t N, class...>
	struct nth_element;

	template<class T, class... Us>
	struct nth_element<0, T, Us...>{
		using type = T;
	};

	template<size_t N, class T, class... Us>
	struct nth_element<N, T, Us...>{
		using type = typename nth_element<N-1, Us...>::type;
	};


	// identity
	template<class T>
	struct identity{
		using type = T;
	};
}
