CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa2

etapa2: parser.tab.o lex.yy.o main.o symbols.o
	$(CXX) $(CXXFLAGS) parser.tab.o lex.yy.o main.o symbols.o -o etapa2

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l

parser.tab.cpp: parser.ypp
	bison -d parser.ypp -o parser.tab.cpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c

clean:
	rm -f etapa2 lex.yy.cpp parser.tab.cpp parser.tab.hpp *.o

