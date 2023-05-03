define compile
	@for cmpl in "g++-12" "clang++" ;do for std in 14 17 20; do cmd="$${cmpl} -std=c++$${std} ./$(1)_test.cpp -o $(2)"; echo $${cmd}; eval $${cmd}; done done
endef


injector: ./injector.h ./injector_test.cpp
	$(call compile,injector,injector)

unpack: ./unpack.h ./injector.h ./unpack_test.cpp
	$(call compile,unpack,unpack)

access: ./private_access.h ./traits.h  ./injector.h ./private_access_test.cpp
	$(call compile,private_access,access)

smp: ./smp.h ./injector.h ./smp_test.cpp
	$(call compile,smp,smp)

intern: ./string_intern.h ./string_intern_test.cpp
	$(call compile,string_intern,intern)

clean:
	@for bin in injector unpack access smp intern; do cmd="[[ ! -e ./$${bin} ]] || rm ./$${bin}"; echo $${cmd}; eval $${cmd}; done

all: injector unpack access smp intern clean
