#pragma once

#define PLUGIN_SECTION_NAME abner 

#include <string>
//#include <vector>
#include <boost/config.hpp>
//#include <boost/shared_ptr.hpp>

namespace dynamic{

	class BOOST_SYMBOL_VISIBLE Pluginable{
		public:
			virtual	std::string Load()=0;
	};

	
//	std::vector<boost::shared_ptr<Pluginable>> AllPlugins(std::vector<std::string> const& dylibNames, std::string const& sectName);

}


