all: compila clean

compila:
	g++ -c tinyxml2.cpp
	g++ -c misc.cpp -lm -lGL -lGLU -lglut
	g++ -c rect.cpp -lGL -lGLU -lglut
	g++ -c circle.cpp -lGL -lGLU -lglut -lm
	g++ -c bullet.cpp -lGL -lGLU -lglut -lm
	g++ -c car.cpp -lGL -lGLU -lglut
	g++ -c input.cpp
	g++ main.cpp -o trabalhocg tinyxml2.o misc.o rect.o circle.o bullet.o car.o input.o -lGL -lGLU -lglut -lm

clean:
	@rm -f *.o
