# Sesión 3 — Semana 3: Automatización del analizador léxico y cronograma

## Teoría del curso (S3)

- **Automatización de la construcción de un scanner**: de las expresiones
  regulares a un **autómata finito** que reconoce los tokens.
- El scanner como reconocedor: recorre el texto y, por máximo emparejamiento
  (*maximal munch*), agrupa caracteres en lexemas y emite el token correspondiente.
- Preparación del uso de un **generador de analizadores léxicos** (Flex/JFlex), que
  se desarrolla en las semanas 4–5.

## Aporte del Grupo 5

- [`Cronograma Lenguaje de Programación S3 Grupo 5.pdf`](Cronograma%20Lenguaje%20de%20Programaci%C3%B3n%20S3%20Grupo%205.pdf) — **cronograma del proyecto**: planificación de las fases (léxico, sintáctico, semántico, generación de código) por semana.

## Relación con el compilador

El cronograma organiza el desarrollo del `Proyecto-LyC` en fases. Las decisiones de
diseño (parser **top-down**, semántico como **gramática de atributos**, generación
de **3 direcciones + Z80**) siguen este plan.
