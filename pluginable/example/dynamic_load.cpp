#include "pluginable.h"
#include "../plugin_loader.h"

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

	for(auto p : dynamic::AllPlugins<dynamic::Pluginable>(dylibs, BOOST_PP_STRINGIZE( PLUGIN_SECTION_NAME ) ) ){
		std::cout << p->Load() << "\n"; 
	}


	for(auto const& dylibName : dylibs){
		boost::dll::library_info inf(dylibName);
		std::vector<std::string> sections= inf.sections(); 
		std::cout << dylibName << " info as follows:\n";
		for(auto& sect : sections){
			std::cout << "sect:\t" << sect << '\n';
			for(auto& sym: inf.symbols(sect)){ 
				std::cout << "sym:\t\t" << sym << '\n';
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	return 0;
}
