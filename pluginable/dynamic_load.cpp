#include "pluginable.h"

#include <vector>
#include <iostream>
#include <string>

#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS_SECTIONED

namespace dynamic{

	class PluginThree: public Pluginable{
		public:
			std::string Load() override {
				return __PRETTY_FUNCTION__;
			}
	};

	PluginThree plugin_three;
	BOOST_DLL_ALIAS_SECTIONED(dynamic::plugin_three, plugin_three, PLUGIN_SECTION_NAME)
}


int main(int argc,char* argv[]){
	std::vector<std::string> dylibs(argc);
	for(auto i=0; i< argc; ++i){
		dylibs[i] = argv[i];
		std::cout << "libname: " << dylibs[i] << '\n';
	}

	dynamic::LoadAllPlugin(dylibs);

	return 0;
}
