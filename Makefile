
CC=g++ -g -O0
CCFLAGS=--ansi ${INC} -std=c++20 -Wall ${DEP}

FLEX=flex
FLEX_OPTS=-Platte_cpp_

BISON=bison
BISON_OPTS=-t -platte_cpp_
OBJS = Absyn.o Buffer.o Lexer.o Parser.o Printer.o DataStructure.o IRCoder.o SSARenamer.o LiveAnalyzer.o
OBJS_FILES = ./build/Absyn.o ./build/Buffer.o ./build/Lexer.o ./build/Parser.o ./build/Printer.o ./build/DataStructure.o ./build/IRCoder.o ./build/SSARenamer.o ./build/LiveAnalyzer.o

INC = -I. -I./lib/fmt/include/
DEP = -L./lib/fmt -lfmt

.PHONY : clean distclean

all : CreateDirectory TestLatteCPP

FrontendTests:
	python3 Tests/TestFrontend.py

BackendTests:
	python3 Tests/TestBackend.py

testsClean: cleanArrays cleanExtensions cleanGood cleanMethods cleanMyTests cleanObjects cleanStruct cleanVirtual

cleanMyBlocks:
	rm -rf ./Tests/Blocks/*_intermediate.bc ./Tests/Blocks/*.ll ./Tests/Blocks/*.bc
cleanFrontend:
	rm -rf ./Tests/badOutputs/*

cleanArrays:
	rm -rf ./Tests/arrays/good/*.bc ./Tests/arrays/good/*_intermediate.bc ./Tests/arrays/good/*.ll  ./Tests/arrays/good/*.output \
	./Tests/arrays/arraysAdditional/*.bc ./Tests/arrays/arraysAdditional/*_intermediate.bc ./Tests/arrays/arraysAdditional/*.ll  ./Tests/arrays/arraysAdditional/*.output \
	./Tests/arrays/myArraysGood/*.bc ./Tests/arrays/myArraysGood/*_intermediate.bc ./Tests/arrays/myArraysGood/*.ll ./Tests/arrays/myArraysGood/*.output

cleanExtensions:
	rm -rf ./Tests/extensions/arrays1/*.bc ./Tests/extensions/arrays1/*_intermediate.bc ./Tests/extensions/arrays1/*.ll ./Tests/extensions/arrays1/*.output \
	./Tests/extensions/objects1/*.bc ./Tests/extensions/objects1/*_intermediate.bc ./Tests/extensions/objects1/*.ll ./Tests/extensions/objects1/*.output \
	./Tests/extensions/objects2/*.bc ./Tests/extensions/objects2/*_intermediate.bc ./Tests/extensions/objects2/*.ll ./Tests/extensions/objects2/*.output \
	./Tests/extensions/struct/*.bc ./Tests/extensions/struct/*_intermediate.bc ./Tests/extensions/struct/*.ll ./Tests/extensions/struct/*.output \

cleanGood:
	rm -rf ./Tests/good/basic/*.bc ./Tests/good/basic/*.ll ./Tests/good/basic/*_intermediate.bc  ./Tests/good/basic/*.output \
	./Tests/good/general/*.bc ./Tests/good/general/*.ll ./Tests/good/general/*_intermediate.bc ./Tests/good/general/*.output \
	./Tests/good/gr5/*.bc ./Tests/good/gr5/*.ll ./Tests/good/gr5/*_intermediate.bc ./Tests/good/gr5/*.output

cleanMethods:
	rm -rf ./Tests/methods/*.bc ./Tests/methods/*.ll ./Tests/methods/*_intermediate.bc ./Tests/methods/*.output

cleanMyTests:
	rm -rf ./Tests/myTests/BooleanExpressions/*.bc ./Tests/myTests/BooleanExpressions/*.ll ./Tests/myTests/BooleanExpressions/*_intermediate.bc ./Tests/myTests/BooleanExpressions/*.output \
	./Tests/myTests/PredifinedFunctions/*.bc ./Tests/myTests/PredifinedFunctions/*.ll ./Tests/myTests/PredifinedFunctions/*_intermediate.bc ./Tests/myTests/PredifinedFunctions/*.output \
	./Tests/myTests/Tests/*.bc ./Tests/myTests/Tests/*.ll ./Tests/myTests/Tests/*_intermediate.bc ./Tests/myTests/Tests/*.output

cleanObjects:
	rm -rf ./Tests/objects/*.bc ./Tests/objects/*.ll ./Tests/objects/*_intermediate.bc ./Tests/objects/*.output

cleanStruct:
	rm -rf ./Tests/struct/*.bc ./Tests/struct/*.ll ./Tests/struct/*._intermediate.bc ./Tests/struct/*.output

cleanVirtual:
	rm -rf ./Tests/virtual/*.bc ./Tests/virtual/*.ll ./Tests/virtual/*_intermediate.bc ./Tests/virtual/*.output \
	rm -rf ./Tests/myVirtual/*.bc ./Tests/myVirtual/*.ll ./Tests/myVirtual/*_intermediate.bc ./Tests/myVirtual/*.output

cleanStrings:
	rm -rf ./Tests/strings/*.bc ./Tests/strings/*.ll ./Tests/strings/*_intermediate.bc ./Tests/strings/*.output

CreateDirectory:
	@mkdir -p build

clean :
	rm -rf ./build ./latc_llvm

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

DataStructure.o : ./src/DataStructure.cpp ./src/DataStructure.h ./src/IntermediateProgram.h
	${CC} ${CCFLAGS} -c ./src/DataStructure.cpp -o ./build/DataStructure.o

SSARenamer.o : ./src/SSARenamer.cpp ./src/SSARenamer.h
	${CC} ${CCFLAGS} -c ./src/SSARenamer.cpp -o ./build/SSARenamer.o

LiveAnalyzer.o : ./src/LiveAnalyzer.cpp ./src/LiveAnalyzer.h
	${CC} ${CCFLAGS} -c ./src/LiveAnalyzer.cpp -o ./build/LiveAnalyzer.o
