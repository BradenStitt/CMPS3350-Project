
all: main

main: main.cpp global.cpp bstitt.cpp bstitt.h skumar.h jrivera2.h jrivera2.cpp bruiz.cpp bruiz.h fonts.h
	g++ main.cpp log.cpp timers.cpp global.cpp skumar.cpp bstitt.cpp libggfonts.a -Wall -omain -lX11 -lGL -lGLU -lm

clean:
	rm -f main

