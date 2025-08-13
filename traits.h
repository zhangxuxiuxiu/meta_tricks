#pragma once

namespace traits{

	// detect
	template< template<class...> class Sfinea, class... Args >
	struct detect{
		template< template<class...> class sfinea>
		static constexpr bool impl( sfinea<Args...>* p){
			return true;
		}

		template< template<class...> class sfinea >
		static constexpr bool impl(...){
			return false;
		}

		static constexpr bool value = impl<Sfinea>(nullptr);
	};

	template<class... Ts>
	struct all_the_same : std::true_type{};
	
	template<class T, class U,class ... Ws>
	struct all_the_same<T, U, Ws...> : std::integral_constant<bool, std::is_same<T,U>::value && all_the_same<U,Ws...>::value>{};

	// identity
	template<class T>
	struct identity{
		using type = T;
	};

	// memeber_host
	template<class FPtr>
	struct member_host;
	
	template<class RF, class T>
	struct member_host<RF T::*>{
		using type = T; 
	};

	// sub_type
	template<class T>
	struct sub_type {
		using type = T;
	};
}
