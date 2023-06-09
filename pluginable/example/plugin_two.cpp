//https://www.boost.org/doc/libs/master/doc/html/boost_dll/tutorial.html#boost_dll.tutorial.querying_libraries_for_symbols
#include "pluginable.h"

#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS_SECTIONED

namespace dynamic{

	class PluginTwo: public Pluginable{
		public:
			virtual std::string Load() override{
				return __PRETTY_FUNCTION__;
			}
	};

	PluginTwo plugin_two;
	BOOST_DLL_ALIAS_SECTIONED(dynamic::plugin_two, plugin_two, PLUGIN_SECTION_NAME)
}
