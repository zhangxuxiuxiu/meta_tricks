dll1: plugin_one.h plugin_one.cpp 
	g++ -std=c++11  -dynamiclib -undefined suppress -flat_namespace -I/usr/local/Cellar/boost/1.78.0_1/include plugin_one.cpp -o libplugin_one.dylib  
dll2: plugin_two.h plugin_two.cpp plugin_four.h plugin_four.cpp 
	g++ -std=c++11  -dynamiclib -undefined suppress -flat_namespace -I/usr/local/Cellar/boost/1.78.0_1/include plugin_two.cpp plugin_four.cpp -o libplugin_two.dylib  
all: dll1 dll2 
	g++ -std=c++11  pluginable.cpp dynamic_load.cpp -lboost_filesystem -lboost_system -o dyload  -I/usr/local/Cellar/boost/1.78.0_1/include -L/usr/local/Cellar/boost/1.78.0_1/lib
	./dyload ./*.dylib

clear:
	rm -r *.dylib *.dSYM ./dyload
