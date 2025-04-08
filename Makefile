CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa2

etapa2: lex.yy.o main.o symbols.o
	$(CXX) $(CXXFLAGS) lex.yy.o main.o symbols.o -o etapa2

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $< -c

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l 

clean:
	rm etapa1 lex.yy.cpp *.o

