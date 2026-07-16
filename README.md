# HM-compress

> Implementación en C++20 del modelo algebraico de
["Compression Is All You Need: Modeling Mathematics"](.) (Aksenov, Bodnia,
Freedman, Mulligan, 2026). La matemática humana (HM) es comprimible mediante definiciones anidadas
jerárquicamente. El paper modela esto con **monoides**: una deducción es
una cadena de símbolos primitivos; una definición es una *macro* (un
generador adicional) cuyo uso comprime la cadena.

- En el **monoide abeliano libre** $A_n$ (los generadores conmutan, como
  en la notación posicional), un conjunto de macros de densidad
  *logarítmica* logra expansión **exponencial** de expresividad
  (Teorema 1). 
  
- En el **monoide libre** $F_n$ (el orden importa, sin relaciones), incluso
  macros de densidad *polinómica* solo logran expansión **lineal**
  (Teorema 4); se necesita una densidad casi máxima (exponencial) para
  superarla (Teorema 5).

El paper argumenta que MathLib (y por extensión, la matemática humana) se
comporta como $A_n$, no como $F_n$ — a pesar de que las pruebas formales
son, en principio, cadenas donde el orden importa.
