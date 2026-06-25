CC      = gcc
CFLAGS  = -Wall -Isrc
SRC     = src
BIN     = compilador
LEXBIN  = analizador_lexico

GEN_C   = $(SRC)/parser.tab.c $(SRC)/lex.yy.c
GEN_H   = $(SRC)/parser.tab.h

.PHONY: all clean run lex gui

all: $(BIN)

# Bison genera parser.tab.c y parser.tab.h
$(SRC)/parser.tab.c $(SRC)/parser.tab.h: $(SRC)/parser.y $(SRC)/ast.h $(SRC)/simbolos.h $(SRC)/semantico.h
	bison -d $(SRC)/parser.y -o $(SRC)/parser.tab.c

# Flex genera lex.yy.c (depende de parser.tab.h para los codigos de token)
$(SRC)/lex.yy.c: $(SRC)/lexer.l $(SRC)/parser.tab.h
	flex -o $(SRC)/lex.yy.c $(SRC)/lexer.l

# Compilador completo: lexico + sintactico + AST + semantico + salida GUI
$(BIN): $(GEN_C) $(SRC)/ast.c $(SRC)/simbolos.c $(SRC)/semantico.c $(SRC)/gui_salida.c $(SRC)/tokens.c
	$(CC) $(CFLAGS) $(SRC)/parser.tab.c $(SRC)/lex.yy.c $(SRC)/ast.c $(SRC)/simbolos.c $(SRC)/semantico.c $(SRC)/gui_salida.c $(SRC)/tokens.c -o $(BIN)

# Solo el analizador lexico (sin AST/semantico)
lex: $(SRC)/lex.yy.c $(SRC)/tokens.c
	$(CC) $(CFLAGS) $(SRC)/main_lexer.c $(SRC)/lex.yy.c $(SRC)/tokens.c -o $(LEXBIN)

run: $(BIN)
	./$(BIN) ejemplos/ejemplo1.g5z80

clean:
	rm -f $(GEN_C) $(GEN_H) $(BIN) $(LEXBIN)

# Interfaz grafica (servidor Python stdlib, sin dependencias)
gui: all lex
	@echo "Lanzando interfaz en http://localhost:5000 ..."
	@python3 gui/server.py
