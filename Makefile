define compile
	@for cmpl in "g++" "clang++" ;do for std in 14 17 20; do cmd="$${cmpl} -std=c++$${std} ./$(1)_test.cpp -o $(2)"; echo $${cmd}; eval $${cmd}; done done
endef


injector: ./injector.h ./injector_test.cpp
	$(call compile,injector,injector)

unpack: ./unpack.h ./injector.h ./type_list.h ./unpack_test.cpp
	$(call compile,unpack,unpack)

stateful: ./traits.h ./stateless_trans.h ./transform_x.h ./stateful_trans.h ./injector.h ./stateful_trans_test.cpp
	$(call compile,stateful_trans,stateful)

stateless: ./stateless_trans.h ./injector.h ./stateless_trans_test.cpp
	$(call compile,stateless_trans,stateless)

access: ./member.h ./private_access.h ./private_access_v2.h ./private_access_v3.h ./injector.h ./private_access_test.cpp ./private_access_v2_test.cpp ./private_access_v3_test.cpp
	$(call compile,private_access,access)
	$(call compile,private_access_v2,access_v2)
	$(call compile,private_access_v3,access_v3)

smp: ./smp.h ./injector.h ./type_list.h ./smp_test.cpp
	$(call compile,smp,smp)

intern: ./string_intern.h ./string_intern_test.cpp
	$(call compile,string_intern,intern)

composer: ./type_list.h ./static_composer.cpp
	clang++ -std=c++11 static_composer.cpp -o sc
	g++ -std=c++11 static_composer.cpp -o sc

clean:
	@for bin in injector unpack stateful stateless access access_v2 smp intern sc; do cmd="[[ ! -e ./$${bin} ]] || rm ./$${bin}"; echo $${cmd}; eval $${cmd}; done

all: injector unpack stateful stateless access smp intern clean
