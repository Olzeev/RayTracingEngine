run: build
	./sfml-app

build:
	g++ -c main.cpp
	g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system

clean:
	rm main.o
