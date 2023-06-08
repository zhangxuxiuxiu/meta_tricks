#pragma once

#define PLUGIN_SECTION_NAME abner 

#include <string>
#include <boost/config.hpp>

namespace dynamic{

	class BOOST_SYMBOL_VISIBLE Pluginable{
		public:
			virtual	std::string Load()=0;
	};
	
}


