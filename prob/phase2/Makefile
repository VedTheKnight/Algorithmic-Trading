#------------------------
# Do not modify this file
#------------------------

all: trader

# For part1: marked with a blank trader
market: main.cpp market.cpp worker.cpp blank_trader.cpp 
	g++ -std=c++20 main.cpp market.cpp worker.cpp blank_trader.cpp -lpthread -o market
	./market 50 > output.txt

# For part2
trader: main.cpp market.cpp worker.cpp trader.cpp 
	g++ -std=c++20 main.cpp market.cpp worker.cpp trader.cpp -lpthread -o trader
	./trader 50 > output.txt

clean:
	rm -rf market trader 
	rm -rf *~
