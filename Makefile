injector: ./injector.h ./injector_test.cpp
	g++-12 -std=c++14 ./injector_test.cpp -o ijc  
	g++-12 -std=c++17 ./injector_test.cpp -o ijc 
	g++-12 -std=c++20 ./injector_test.cpp -o ijc 
	clang++ -std=c++14 ./injector_test.cpp -o ijc  
	clang++ -std=c++17 ./injector_test.cpp -o ijc 
	clang++ -std=c++20 ./injector_test.cpp -o ijc 

unpack: ./unpack.h ./injector.h ./unpack_test.cpp
	g++-12 -std=c++14 ./unpack_test.cpp -o upk  
	g++-12 -std=c++17 ./unpack_test.cpp -o upk 
	g++-12 -std=c++20 ./unpack_test.cpp -o upk 
	clang++ -std=c++14 ./unpack_test.cpp -o upk  
	clang++ -std=c++17 ./unpack_test.cpp -o upk 
	clang++ -std=c++20 ./unpack_test.cpp -o upk 

access: ./private_access.h ./traits.h  ./injector.h ./private_access_test.cpp
	g++-12 -std=c++20 ./private_access_test.cpp -o pa 
	g++-12 -std=c++17 ./private_access_test.cpp -o pa 
	g++-12 -std=c++14 ./private_access_test.cpp -o pa 
	clang++ -std=c++20 ./private_access_test.cpp -o pa 
	clang++ -std=c++17 ./private_access_test.cpp -o pa 
	clang++ -std=c++14 ./private_access_test.cpp -o pa 

smp: ./smp.h ./injector.h ./smp_test.cpp
	g++-12 -std=c++14 ./smp_test.cpp -o sall
	g++-12 -std=c++17 ./smp_test.cpp -o sall
	g++-12 -std=c++20 ./smp_test.cpp -o sall
	clang++ -std=c++14 ./smp_test.cpp -o sall
	clang++ -std=c++17 ./smp_test.cpp -o sall
	clang++ -std=c++20 ./smp_test.cpp -o sall

intern: ./string_intern.h ./string_intern_test.cpp
	g++-12 -std=c++14 ./string_intern_test.cpp -o sit 
	g++-12 -std=c++17 ./string_intern_test.cpp -o sit
	g++-12 -std=c++20 ./string_intern_test.cpp -o sit
	clang++ -std=c++14 ./string_intern_test.cpp -o sit
	clang++ -std=c++17 ./string_intern_test.cpp -o sit
	clang++ -std=c++20 ./string_intern_test.cpp -o sit

clean:
	[[ ! -e ./ijc ]] || rm ./ijc
	[[ ! -e ./upk ]] || rm ./upk
	[[ ! -e ./sall ]] || rm ./sall
	[[ ! -e ./sit ]] || rm ./sit
	[[ ! -e ./pa ]] || rm ./pa

all: injector unpack access smp intern clean
