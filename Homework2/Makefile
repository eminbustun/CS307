CC = g++
CFLAGS = -I. -std=c++20 
DEPS = 
LIB = -pthread

all: 

%: %.cpp
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)

clean:
	rm -f *~
	rm -f ./sample1Level
	rm -f ./sampleMultiLevel
	rm -f ./sampleQueue
