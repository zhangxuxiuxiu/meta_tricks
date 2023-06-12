#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/library_info.hpp>

namespace dynamic{

	template<class PluginApi>
	std::vector<boost::shared_ptr<PluginApi>>  AllPlugins(std::vector<std::string> const& dylibNames, std::string const& sectName){
		std::vector<boost::shared_ptr<PluginApi>> plugins;
		for(auto const& dylibName : dylibNames){
			// Class `library_info` can extract information from a library
			boost::dll::library_info inf(dylibName);

			std::vector<std::string> symbols = inf.symbols( sectName );
			if( symbols.empty() ){
				continue;
			}
			// Loading library and importing symbols from it
			auto lib = boost::make_shared<boost::dll::shared_library>( dylibName );
			for (auto& sym :  symbols ) {
				// each plugin adds one reference cnt to $lib to keep $lib from being unloaded so that the plugin is valid
				plugins.push_back( boost::shared_ptr<PluginApi>( lib, lib->get<PluginApi*>( sym )) );
			}
		}

		return std::move(plugins);
	}

}

