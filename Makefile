
all: lander test bradenTest snehalTest sb

lander: lander.cpp 
	g++ lander.cpp log.cpp bstitt.cpp bruiz.cpp skumar.cpp timers.cpp -Wall -olander -lX11 -lGL -lGLU -lm

test: lander.cpp
	g++ lander.cpp log.cpp bstitt.cpp bruiz.cpp -Wall -o test -lX11 -lGL -lGLU -lm -D TESTING

bradenTest: bradenTest.cpp global.cpp bstitt.cpp bstitt.h 
	g++ bradenTest.cpp log.cpp global.cpp bstitt.cpp bruiz.cpp skumar.cpp timers.cpp -Wall -o bradenTest -lX11 -lGL -lGLU -lm -D TESTING

snehalTest: snehalTest.cpp 
	g++ snehalTest.cpp log.cpp timers.cpp -Wall -osnehalTest -lX11 -lGL -lGLU -lm

sb: sb.cpp global.cpp bstitt.cpp bstitt.h
	g++ sb.cpp log.cpp timers.cpp global.cpp bstitt.cpp -Wall -osb -lX11 -lGL -lGLU -lm

clean:
	rm -f lander test bradenTest snehalTest sb

