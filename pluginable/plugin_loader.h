#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/library_info.hpp>

namespace dynamic{

	template<class PluginApi>
	struct shared_library_deleter{
		boost::shared_ptr<boost::dll::shared_library> shared_lib;

		void operator()(PluginApi* plugin) const{}
	};

	template<class PluginApi>
	std::vector<boost::shared_ptr<PluginApi>>  AllPlugins(std::vector<std::string> const& dylibNames, std::string const& sectName){
		std::vector<boost::shared_ptr<PluginApi>> plugins;
		for(auto dylibName : dylibNames){
			// Class `library_info` can extract information from a library
			boost::dll::library_info inf(dylibName);

			std::vector<std::string> exports = inf.symbols( sectName );
			if( exports.empty() ){
				continue;
			}
			// Loading library and importing symbols from it
			auto lib = boost::make_shared<boost::dll::shared_library>( dylibName );
			for (std::size_t j = 0; j < exports.size(); ++j) {
				plugins.push_back( boost::shared_ptr<PluginApi>( &(lib->get_alias<PluginApi>( exports[j] ) ), shared_library_deleter<PluginApi>{lib} ) );	
			}
		}

		return plugins;
	}

}

