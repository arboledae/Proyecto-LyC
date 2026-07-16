# Documentación del Proyecto — Compilador Dummy (Grupo 5)

Índice de la documentación del proyecto, organizada por **sesión de trabajo** del
grupo (semanas 1 a 7 del curso). Cada carpeta `Sesión N/` contiene los
**entregables originales** del grupo (PDF/ZIP/MD) y un `README.md` que resume la
**teoría del curso** de esa etapa y el **aporte del grupo** al compilador.

> Este índice mantiene el proyecto *conforme con la carpeta de teoría* (`markdown/`
> del curso) y reúne, dentro del propio proyecto, la documentación de las sesiones 1–7.

## Mapa: sesión -> teoría -> fase del compilador

| Sesión | Semana(s) | Teoría del curso | Fase / aporte al compilador | Archivos del proyecto |
|---|---|---|---|---|
| [Sesión 1](Sesión%201/) | 1 | Introducción a los compiladores; fases; análisis léxico (tokens, lexemas) | Marco general del proyecto (compilador Dummy -> Z80) | — |
| [Sesión 2](Sesión%202/) | 2 | Análisis léxico: tokens, palabras reservadas, lexemas | **Definición del lenguaje Dummy** (tipos, estructuras, operadores) | `src/lexer.l`, `src/tokens.h` |
| [Sesión 3](Sesión%203/) | 3 | Automatización de un analizador léxico / scanner (ER -> autómata) | Cronograma del proyecto | — |
| [Sesión 4](Sesión%204/) | 4–5 | **Flex** y **JFlex**: generación de analizadores léxicos | **Fase 1 – Analizador léxico** | `src/lexer.l`, `src/lex.yy.c` |
| [Sesión 5](Sesión%205/) | 9 | Análisis sintáctico **top-down** (LL(1), descenso recursivo) | **Fase 2 – Léxico + sintáctico** (avance Semana 9) | `src/parser_rd.c`, `src/ast.c` |
| [Sesión 6](Sesión%206/) | 10 | Análisis sintáctico top-down (continuación) | **Fase 2 – Parser top-down + AST** (avance Semana 10) | `src/parser_rd.c`, `src/ast.c` |
| [Sesión 7](Sesión%207/) | 11–13 | Análisis **semántico**: gramáticas de atributos | **Fase 3 – Semántico** + **GUI** + código intermedio/Z80 | `src/semantico.c`, `src/simbolos.c`, `gui/`, `documentos/Sesión 7/*` |

> Nota sobre la numeración: las *sesiones* son las reuniones de trabajo del Grupo 5
> y no coinciden 1:1 con la semana del curso (por ejemplo, la Sesión 5 corresponde a
> la Semana 9). La columna "Semana(s)" indica la equivalencia con la teoría.

## Teoría del curso (carpeta `markdown/` fuera del proyecto)

La teoría completa (diapositivas convertidas a Markdown) está en
`../../documentos/markdown/`: `S1`, `S2`, `S3`, `S4y5`, `S6y7`, `S9y10`, `S11y12`.
Cada `README.md` de sesión resume la parte relevante y enlaza el entregable del grupo.

## Documentación técnica

- [`Sesión 7/Documentacion_Tecnica_Compilador_S6-S12.md`](Sesión%207/Documentacion_Tecnica_Compilador_S6-S12.md) — **documento técnico final**: describe el compilador tal como está implementado (parser por descenso recursivo, semántico como gramática de atributos, TAC y Z80).
- [`Sesión 7/Plan_Analisis_Semantico_Semana11-13.md`](Sesión%207/Plan_Analisis_Semantico_Semana11-13.md) — plan original de las semanas 11–13 (documento histórico).
- [`Sesión 7/Documentacion_Interfaz_Grafica_Semana11-13.md`](Sesión%207/Documentacion_Interfaz_Grafica_Semana11-13.md) — documentación de la interfaz gráfica.
