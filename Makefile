build:
	g++ -O3 -march=native -fopenmp main.cpp -o main

build_app: 
	g++ app/main.cpp `fltk-config --ldstaticflags` -o app.o

run:
	./app.o

clean:
	rm main