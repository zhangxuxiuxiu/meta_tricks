#pragma once
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS_SECTIONED

#include "pluginable.h"

namespace dynamic{

	class PluginOne : public Pluginable{
		public:
			virtual std::string Load();
	};
}
