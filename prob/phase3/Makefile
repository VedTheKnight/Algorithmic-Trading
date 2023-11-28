#------------------------
# Do not modify this file
#------------------------

all: iml.cpp trader.cpp
	g++-11 -std=c++20 iml.cpp  -lpthread -o iml
	g++-11 -std=c++20 trader.cpp maxheap.cpp minheap.cpp  -lpthread -o trader
	./trader 3 > output.txt &
	sleep 3
	./iml 3

clean:
	rm -rf trader iml output.txt
	rm -rf *~
