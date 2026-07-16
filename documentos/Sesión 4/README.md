# Sesión 4 — Semanas 4 y 5: Analizador léxico con Flex / JFlex

## Teoría del curso (S4y5)

- **Flex**: generador de analizadores léxicos en C. Un archivo `.l` tiene tres
  secciones (definiciones, reglas, código) separadas por `%%`. Cada regla asocia
  una **expresión regular** a una acción que devuelve un token.
- **JFlex**: equivalente para Java (áreas de código, opciones/declaraciones y reglas).
- Funcionamiento: Flex construye un autómata que reconoce los patrones y genera la
  función `yylex()`; `yytext` contiene el lexema y `yylineno` la línea actual.

## Aporte del Grupo 5

- [`AnalizadorLexicoFlex.zip`](AnalizadorLexicoFlex.zip) — **analizador léxico del lenguaje Dummy en Flex**.
- [`Lexer Jflex.zip`](Lexer%20Jflex.zip) — versión equivalente en JFlex.

## Relación con el compilador

Corresponde a la **Fase 1** del `Proyecto-LyC`: el archivo `src/lexer.l` (Flex)
reconoce palabras clave, identificadores, literales (`num`, `dec`, `text`, booleanos),
operadores, delimitadores y comentarios `%% ... %%`. Los códigos de token los define
`src/tokens.h`. Se puede probar de forma aislada con `make lex` (binario
`analizador_lexico`).
