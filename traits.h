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
