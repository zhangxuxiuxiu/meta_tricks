#include "pluginable.h"

#include <boost/dll/shared_library.hpp>
#include <boost/dll/library_info.hpp>
#include <iostream>

namespace dynamic{

	void LoadAllPlugin(std::vector<std::string> const& dylibNames){
		for(auto dylibName : dylibNames){
			// Class `library_info` can extract information from a library
			boost::dll::library_info inf(dylibName);

			auto sectionName = BOOST_PP_STRINGIZE( PLUGIN_SECTION_NAME ) ;
			std::vector<std::string> exports = inf.symbols( sectionName );
			// Loading library and importing symbols from it
			boost::dll::shared_library lib(dylibName);
			for (std::size_t j = 0; j < exports.size(); ++j) {
				std::cout << exports[j] << ".Load() = [ " << lib.get_alias<Pluginable>( exports[j] ).Load() << " ]\n"; 
			}
		}

	}

}


