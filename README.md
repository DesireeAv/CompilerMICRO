# Compiler MICRO
---
## Especificaciones del lenguaje a compilar:

- El único tipo de variables que existen son números positivos y el cero.
- Las operaciones que se pueden realizar son:
  - Suma con `+`
  - Asignaciones con `=`
  - Operaciones de entrada y salida 
  - Comentarios con `-`
- Palabras reservadas:
  - `read`
  - `write`
  - `begin`
  - `end`
- Las asignaciones son implícitas, ya que solamente existe un tipo de dato.

## Especificaciones del compilador:
- Utiliza la estructura de datos trie para almacenar las variables (falta de implementar)
- Maneja errores 