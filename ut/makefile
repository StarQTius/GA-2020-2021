COMPILER_FLAG = \
  -Wall \
  -std=c++20 \
  -ggdb

SFML_LIB = \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system

state_compilation = echo \\033[93mCompiling to build object \\033[95m$@\\033[93m...\\033[0m
state_linking     = echo \\033[93mLinking to build unit test \\033[95m$@\\033[93m...\\033[0m

utest = \
echo \\033[93mRunning unit test \\033[95m$(1)...\\033[93m\\033[0m; \
./$(1) -r compact;

all: \
	nn \
	gen \
	ent \
	pg

nn: ../obj/ut_neural_network.o
	g++ $^ -lcatch -o $@
	./$@ -r compact;

../obj/ut_neural_network.o: neural_network.cpp
	g++ -c $^ $(COMPILER_FLAG) -o $@

gen: ../obj/ut_genetics.o
	g++ $^ -lcatch -o $@
	./$@ -r compact;

../obj/ut_genetics.o: genetics.cpp
	g++ -c $^ $(COMPILER_FLAG) -o $@

ent: ../obj/ut_entity.o
	g++ $^ -lcatch $(SFML_LIB) -o $@
	./$@ -r compact;

../obj/ut_entity.o: entity.cpp
	g++ -c $^ $(COMPILATION_FLAG) -o $@

pg: ../obj/ut_playground.o
	g++ $^ -lcatch $(SFML_LIB) -o $@
	./$@ -r compact;

../obj/ut_playground.o: playground.cpp
	g++ -c $^ $(COMPILATION_FLAG) -o $@

clean: \
	cln_nn \
	cln_gen \
	cln_ent \
	cln_pg

cln_nn:
	rm -vf ../obj/ut_neural_network.o nn

cln_gen:
	rm -vf ../obj/ut_genetics.o gen

cln_ent:
	rm -vf ../obj/ut_entity.o ent

cln_pg:
	rm -vf ../obj/ut_playground.o pg
