build:
	g++ -O3 -march=native -fopenmp main.cpp -o main

build_app: 
	g++ app/main.cpp -lfltk -o app.o

clean:
	rm main