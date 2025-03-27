CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa1

etapa1: lex.yy.o main.o
	$(CXX) lex.yy.o main.o -o etapa1

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $< -c

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l 

clean:
	rm etapa1 lex.yy.cpp *.o

