
all: main

main: main.cpp global.cpp bstitt.cpp bstitt.h skumar.h skumar.cpp
	g++ main.cpp log.cpp timers.cpp global.cpp bstitt.cpp skumar.cpp -Wall -omain -lX11 -lGL -lGLU -lm

clean:
	rm -f main

