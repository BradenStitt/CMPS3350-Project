
all: main

main: main.cpp global.cpp bstitt.cpp bstitt.h skumar.h skumar.cpp jrivera2.h jrivera2.cpp bruiz.cpp bruiz.h
	g++ main.cpp log.cpp timers.cpp global.cpp bstitt.cpp skumar.cpp -Wall -omain -lX11 -lGL -lGLU -lm

clean:
	rm -f main

