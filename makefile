COMPILER=g++
FLAGS=-Wall -pedantic -O3
OPTIMIZE=-O3
TARGET=calculator

all:
	$(COMPILER) $(FLAGS) $(OPTIMIZE) main.cpp -o $(TARGET)

clean:
	rm $(TARGET)