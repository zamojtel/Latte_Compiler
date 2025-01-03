
CC=g++ -g -O0
CCFLAGS=--ansi ${INC} -std=c++20 -Wall ${DEP}

FLEX=flex
FLEX_OPTS=-Platte_cpp_

BISON=bison
BISON_OPTS=-t -platte_cpp_
OBJS = Absyn.o Buffer.o Lexer.o Parser.o Printer.o DataStructure.o IRCoder.o
OBJS_FILES = ./build/Absyn.o ./build/Buffer.o ./build/Lexer.o ./build/Parser.o ./build/Printer.o ./build/DataStructure.o ./build/IRCoder.o

INC = -I. -I./lib/fmt/include/
DEP = -L./lib/fmt -lfmt 

.PHONY : clean distclean

all : CreateDirectory TestLatteCPP

FrontendTests:
	python3 Tests/TestFrontend.py

BackendTests:
	python3 Tests/TestBackend.py

testsClean:
	rm -rf ./Tests/badOutputs ./Tests/goodOutputs  ./Tests/myTests/someTestsOutputs ./Tests/myTests/BasicPredifinedFunctionsFunctionalitiesOutputs

CreateDirectory:
	@mkdir -p build

clean :
	rm -rf ./build ./latc_llvm ./Tests/myBadOutputs ./Tests/myGoodOutputs ./Tests/goodOutputs ./Tests/badOutputs \
	./Tests/myGood/*.bc ./Tests/myGood/*.ll \
	./Tests/good/*.bc ./Tests/good/*.ll	\
	./Ready/good/* \
	./Tests/myTests/NewTestsOutputs


cleanBison:
	rm -f ./src/Parser.C ./src/Bison.H

distclean : clean
	rm -f Absyn.C Absyn.H Buffer.C Buffer.H Test.C Bison.H Parser.C Parser.H ParserError.H LatteCPP.y Lexer.C LatteCPP.l Skeleton.C Skeleton.H Printer.C Printer.H Makefile LatteCPP.tex

TestLatteCPP : ${OBJS} Test.o
	@echo "Linking TestLatteCPP..."
	${CC} ${CCFLAGS} ${OBJS_FILES} ./build/Test.o -o latc_llvm

Absyn.o : ./src/Absyn.C ./src/Absyn.H
	${CC} ${CCFLAGS} -c ./src/Absyn.C -o ./build/Absyn.o 

Buffer.o : ./src/Buffer.C ./src/Buffer.H
	${CC} ${CCFLAGS} -c ./src/Buffer.C -o ./build/Buffer.o

Lexer.C : ./src/LatteCPP.l
	${FLEX} ${FLEX_OPTS} -oLexer.C LatteCPP.l

Parser.C Bison.H : ./src/LatteCPP.y
	${BISON} ${BISON_OPTS} ./src/LatteCPP.y -o ./src/Parser.C

Lexer.o : CCFLAGS+=-Wno-sign-conversion

Lexer.o : ./src/Lexer.C ./src/Bison.H
	${CC} ${CCFLAGS} -c ./src/Lexer.C -o ./build/Lexer.o

Parser.o : 
	${CC} ${CCFLAGS} -c ./src/Parser.C -o ./build/Parser.o

Printer.o : ./src/Printer.C ./src/Printer.H ./src/Absyn.H
	${CC} ${CCFLAGS} -r ./src/Printer.C -o ./build/Printer.o

Skeleton.o : ./src/Skeleton.C ./src/Skeleton.H ./src/Absyn.H
	${CC} ${CCFLAGS} -c Skeleton.C -o ./build/Skeleton.o

Test.o : ./src/Test.C ./src/Parser.H ./src/Printer.H ./src/Absyn.H
	${CC} ${CCFLAGS} -c ./src/Test.C -o ./build/Test.o

IRCoder.o : ./src/IRCoder.cpp ./src/IRCoder.h
	${CC} ${CCFLAGS} -c ./src/IRCoder.cpp -o ./build/IRCoder.o

DataStructure.o : ./src/DataStructure.cpp ./src/DataStructure.h
	${CC} ${CCFLAGS} -c ./src/DataStructure.cpp -o ./build/DataStructure.o