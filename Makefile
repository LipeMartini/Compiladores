CXX = g++
CXXFLAGS = -std=c++11 -Wall

target: etapa5

etapa5: parser.tab.o lex.yy.o main.o symbols.o ast.o tacs.o
	$(CXX) $(CXXFLAGS) parser.tab.o lex.yy.o main.o symbols.o ast.o tacs.o -o etapa5

lex.yy.cpp: scanner.l
	flex -o lex.yy.cpp scanner.l

parser.tab.cpp: parser.ypp
	bison -d parser.ypp -o parser.tab.cpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -c

parser.tab.o: parser.tab.cpp ast.h symbols.hpp
main.o: main.cpp ast.h symbols.hpp tacs.hpp
symbols.o: symbols.cpp symbols.hpp ast.h parser.tab.hpp
ast.o: ast.cpp ast.h symbols.hpp
tacs.o: tacs.cpp tacs.hpp ast.h symbols.hpp
lex.yy.o: lex.yy.cpp parser.tab.hpp ast.h symbols.hpp

clean:
	rm -f etapa5 lex.yy.cpp parser.tab.cpp parser.tab.hpp *.o

test: etapa5
	./etapa5 teste.txt saida.txt

