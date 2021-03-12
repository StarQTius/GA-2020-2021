flag= \
	-std=c++20

lib= \
	-lstdc++ \
	-lm

sfml_lib = \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system

debug_sfml_lib = \
	-lsfml-graphics-d \
	-lsfml-window-d \
	-lsfml-system-d

all: \
	nn \
	gen \
	release

debug: trial/main.cpp
	gcc $(flag) -ggdb $^-I./include  $(lib) $(debug_sfml_lib) -o debug

release: trial/main.cpp
	gcc $(flag) $^ -I./include $(lib) $(sfml_lib) -o release

nn: obj/ut_net.o
	gcc $(flag) $^ $(lib) -lcatch -o ut/$@
	ut/$@ -r compact;

obj/ut_net.o: neural/ut/net.cpp
	gcc $(flag) -ggdb -c $^ -o $@

gen: obj/ut_genetics.o
	gcc $(flag) $^ $(lib) -lcatch -o ut/$@
	ut/$@ -r compact;

obj/ut_genetics.o: genetics/ut/genetics.cpp
	gcc $(flag) -ggdb -c $^ -o $@

clean: \
	cln_nn \
	cln_gen \
	cln_release

mrproper: clean
	rm obj/*.o

cln_nn:
	rm -vf obj/ut_net.o ut/nn

cln_gen:
	rm -vf obj/ut_genetics.o ut/gen

cln_release:
	rm -vf release
