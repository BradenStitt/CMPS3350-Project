# I removed skumar.cpp from the list of files to compile because it was causing errors
# Add it back to fix compiler warnings and errors snehal. :)
all: lander test bradenTest

lander: lander.cpp 
	g++ lander.cpp log.cpp bstitt.cpp bruiz.cpp -Wall -olander -lX11 -lGL -lGLU -lm

test: lander.cpp
	g++ lander.cpp log.cpp bstitt.cpp bruiz.cpp -Wall -o test -lX11 -lGL -lGLU -lm -D TESTING

bradenTest: bradenTest.cpp global.cpp bstitt.cpp bstitt.h 
	g++ bradenTest.cpp log.cpp global.cpp bstitt.cpp bruiz.cpp -Wall -o bradenTest -lX11 -lGL -lGLU -lm -D TESTING

clean:
	rm -f lander test bradenTest

