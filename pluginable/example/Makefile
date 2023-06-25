dll1: plugin_one.h plugin_one.cpp 
	g++ -std=c++11  -dynamiclib -undefined suppress -flat_namespace -I/opt/homebrew/Cellar/boost/1.81.0_1/include -I/usr/local/Cellar/boost/1.78.0_1/include plugin_one.cpp -o libplugin_one.dylib  
dll2: plugin_two.cpp  plugin_four.cpp 
	g++ -std=c++11  -dynamiclib -undefined suppress -flat_namespace -I/opt/homebrew/Cellar/boost/1.81.0_1/include -I/usr/local/Cellar/boost/1.78.0_1/include plugin_two.cpp plugin_four.cpp -o libplugin_two.dylib  
all: dll1 dll2 dynamic_load.cpp ../plugin_loader.h
	g++ -std=c++11  dynamic_load.cpp -lboost_filesystem -lboost_system -o dyload -I/opt/homebrew/Cellar/boost/1.81.0_1/include -I/usr/local/Cellar/boost/1.78.0_1/include -L/opt/homebrew/Cellar/boost/1.81.0_1/lib -L/usr/local/Cellar/boost/1.78.0_1/lib
	./dyload ./*.dylib

clear:
	[[ ! -f ./libplugin_one.dylib ]]  || rm libplugin_one.dylib 
	[[ ! -f ./libplugin_two.dylib ]]  || rm libplugin_two.dylib 
	[[ ! -f ./dyload ]]  || rm ./dyload 
	[[ ! -d ./dyload.dSYM ]]  || rm -r ./dyload.dSYM 
