# Sesión 2 — Semana 2: Análisis léxico y definición del lenguaje

## Teoría del curso (S2)

- **Tokens, patrones y lexemas**: un token es una categoría (palabra clave,
  identificador, literal, operador, delimitador); el **lexema** es el texto concreto
  reconocido; el **patrón** es la regla (expresión regular) que lo describe.
- **Palabras reservadas** del lenguaje y su distinción de los identificadores.
- Representación de un programa en lexemas, en códigos numéricos de token y en tokens.

## Aporte del Grupo 5

- [`Informe Lenguaje de Programación S2 Grupo 5 - Lenguajes y Compiladores.pdf`](Informe%20Lenguaje%20de%20Programaci%C3%B3n%20S2%20Grupo%205%20-%20Lenguajes%20y%20Compiladores.pdf) — **definición del lenguaje Dummy**.
- [`Ejercicios S2 Grupo 5 - Lenguajes y Compiladores.pdf`](Ejercicios%20S2%20Grupo%205%20-%20Lenguajes%20y%20Compiladores.pdf) — ejercicios de análisis léxico.

### Resumen del lenguaje Dummy (extensión `.g5z80`)

| Elemento | Detalle |
|---|---|
| Programa | `inicio { ... }` (ámbito global) |
| Tipos declarables | `num` (entero), `dec` (decimal), `text` (cadena) |
| Booleano | `verdadero` / `falso` (en condiciones; no declarable) |
| Estructuras | `cuando (cond) { } sino { }`, `loop (cond) { }`, `imprimir(expr)`, `retornar expr` |
| Comentarios | `%% ... %%` (ignorados) |
| Operadores | aritméticos `+ - * / %`, relacionales `> < >= <= == !=`, lógicos `&& \|\| !`, asignación `=` |

## Relación con el compilador

Esta definición fija los **terminales** que reconoce el analizador léxico
(`src/lexer.l`) y cuyo catálogo de códigos vive en `src/tokens.h`. Es la base de la
gramática usada por el parser (Sesión 5–6) y de las reglas de tipos del semántico
(Sesión 7).
