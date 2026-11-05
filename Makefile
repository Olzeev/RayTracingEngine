build:
	g++ -O3 -march=native -fopenmp main.cpp -o main

build_app: 
	fltk-config --use-gl --compile app/main.cpp

run:
	./app.o

clean:
	rm main