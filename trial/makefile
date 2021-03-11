flags= \
	-std=c++20
libs= \
	-lstdc++ \
	-lm

debug: main.cpp
	gcc $(flags) -ggdb $^ $(libs) -lsfml-graphics-d -lsfml-window-d -lsfml-system-d -o debug

release: main.cpp
	gcc $(flags) $^ $(libs) -Ilib/Little-Type-Library/include/ -lsfml-graphics -lsfml-window -lsfml-system -o release
