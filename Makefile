all:
	mkdir -p bin
	yacc -d -o ./bin/y.tab.c ./src/compiler.y
	lex -o ./bin/lex.yy.c ./src/compiler.l
	gcc -o ./bin/compiler ./bin/lex.yy.c ./bin/y.tab.c ./src/symbol.c ./src/ast.c -lm -lfl

clean:
	rm -rf ./bin/*