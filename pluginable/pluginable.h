#pragma once

#define PLUGIN_SECTION_NAME abner 

#include <string>
#include <vector>
#include <boost/config.hpp>

namespace dynamic{

	class BOOST_SYMBOL_VISIBLE Pluginable{
		public:
			virtual	std::string Load()=0;
	};

	void LoadAllPlugin(std::vector<std::string> const& dylibNames);

}


