# Sesión 1 — Semana 1: Introducción a los compiladores

## Teoría del curso (S1)

- Qué es un **compilador**: programa que traduce un programa de un lenguaje fuente
  a un lenguaje destino equivalente. Diferencia con un **intérprete**.
- **Fases de un compilador**: análisis léxico, sintáctico, semántico, generación y
  optimización de código intermedio, generación de código final. La **tabla de
  símbolos** y el **manejo de errores** atraviesan todas las fases.
- Estructura en **front-end** (dependiente del lenguaje fuente) y **back-end**
  (dependiente de la máquina objetivo), unidos por una **representación intermedia (IR)**.
- **Análisis léxico**: el scanner lee el flujo de caracteres y produce **tokens**
  (unidades con significado); el texto reconocido de un token es su **lexema**.
- **Proyecto fin de curso**: construir un compilador de un lenguaje **Dummy** hacia
  ensamblador **Z80** (Amstrad CPC), cubriendo las cinco fases.

## Aporte del Grupo 5

- [`Practica_1_Grupo_5_Lenguajes_Y_Compiladores.pdf`](Practica_1_Grupo_5_Lenguajes_Y_Compiladores.pdf) — primera práctica: identificación de tokens/lexemas y marco general del proyecto.

## Relación con el compilador

Establece el **objetivo global** del `Proyecto-LyC`: un compilador del lenguaje Dummy
con las cinco fases y salida a Z80. Todas las decisiones posteriores (léxico,
sintáctico top-down, semántico, TAC, Z80) parten de este marco.
