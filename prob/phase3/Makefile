#------------------------
# Do not modify this file
#------------------------

all: iml.cpp trader.cpp
	g++-11 -std=c++20 trader.cpp maxheap.cpp minheap.cpp -o trader
	./trader < input.txt > output.txt 

	

clean:
	rm -rf trader iml 
	rm -rf *~
