//https://www.boost.org/doc/libs/master/doc/html/boost_dll/tutorial.html#boost_dll.tutorial.querying_libraries_for_symbols
#include "plugin_one.h"

#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS_SECTIONED
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

namespace dynamic{
	std::string PluginOne::Load(){
		return __PRETTY_FUNCTION__;
	}

	PluginOne plugin_one;
	BOOST_DLL_ALIAS_SECTIONED(dynamic::plugin_one, plugin_one, PLUGIN_SECTION_NAME)
}
