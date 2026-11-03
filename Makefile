build:
	g++ -O3 -march=native -fopenmp main.cpp -o main

clean:
	rm main