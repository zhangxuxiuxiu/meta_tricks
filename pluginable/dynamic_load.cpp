#include "pluginable.h"

#include <vector>
#include <iostream>
#include <string>

int main(int argc,char* argv[]){
	std::vector<std::string> dylibs(argc-1);
	for(auto i=1; i< argc; ++i){
		dylibs[i-1] = argv[i];
		std::cout << "libname: " << dylibs[i-1] << '\n';
	}

	dynamic::LoadAllPlugin(dylibs);

	return 0;
}
