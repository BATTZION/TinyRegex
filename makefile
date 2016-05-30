#test_parser : test_parser.o parser.o
#	g++ -o test_parser test_parser.o parser.o -std=c++11

test : parser.o nfa.o nfa2dfa.o test.o
	g++ -o test parser.o nfa.o nfa2dfa.o test.o -std=c++11

#parser
parser.o : parser.h parser.cpp
	g++ -c parser.cpp -std=c++11

#test_parser
test_parser.o : parser.h test_parser.cpp
	g++ -c test_parser.cpp  -std=c++11

#nfa
nfa.o : nfa.h parser.h nfa.cpp parser.cpp
	g++ -c nfa.cpp  -std=c++11

nfa2dfa.o : nfa2dfa.h nfa.h nfa2dfa.cpp
	g++ -c nfa2dfa.cpp -std=c++11

test.o : nfa.h nfa2dfa.h parser.h test.cpp
	g++ -c test.cpp -std=c++11

#clean
clean:
	rm test.o parser.o nfa.o test_parser test nfa2dfa.o
