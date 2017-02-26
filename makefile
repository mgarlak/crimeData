## Simple makefile for crimeParse

run: crime2database.cpp
	g++ crime2database.cpp -o crime2database -std=c++11 -l sqlite3
	./crime2database
	rm crime2database

clean:
	rm *.db
