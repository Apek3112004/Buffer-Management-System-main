CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Iinclude

SRC = src
BIN = bin
FILES = files

all: table join select

table: $(SRC)/table.cpp include/Tables.hpp
	$(CXX) $(CXXFLAGS) $(SRC)/table.cpp -o $(BIN)/table

join: $(SRC)/join.cpp include/Tables.hpp
	$(CXX) $(CXXFLAGS) $(SRC)/join.cpp -o $(BIN)/join

select: $(SRC)/select.cpp include/Tables.hpp
	$(CXX) $(CXXFLAGS) $(SRC)/select.cpp -o $(BIN)/select

run-table: table
	./$(BIN)/table

run-join: join
	./$(BIN)/join

run-select: select
	./$(BIN)/select

clean:
	rm -f $(BIN)/*
