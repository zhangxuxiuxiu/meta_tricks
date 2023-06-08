//https://www.boost.org/doc/libs/master/doc/html/boost_dll/tutorial.html#boost_dll.tutorial.querying_libraries_for_symbols
#include "plugin_four.h"

#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS_SECTIONED
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

namespace dynamic{
	std::string PluginFour::Load(){
		return __PRETTY_FUNCTION__;
	}

	PluginFour plugin_four;
	BOOST_DLL_ALIAS_SECTIONED(dynamic::plugin_four, plugin_four, PLUGIN_SECTION_NAME)
}
