# Compiler MICRO
---
## Especificaciones del lenguaje a compilar:

- El único tipo de variables que existen son de tipo `unsigned int` (ya que no se permiten restas).
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
- Lee en la línea de comando el nombre del archivo con extensión `.mic`, para simular el nuevo lenguaje *Micro*.
- Utiliza la estructura de datos trie para almacenar las variables.
- Maneja errores como:
  - En las asignaciones no se pueden hacer variables nuevas, solo anteriormente declaradas
  - Verificar que todas las instrucciones son válidas.
  - Los relacionados con asignar memoria, por ejemplo, a la hora de crear el trie o de insertarle una nueva variable.
  - Los relacionados con abrir los archivos respectivos, por ejemplo, el archivo de input que tiene el lenguaje debe tener la extensión `.mic`.
- Permite declarar una variable y hacer su asignación respectiva, y luego leerla en el input y hacerle los cambios necesarios cuantas veces se requiera, por ejemplo:
   ```
   begin;
  variable = 9+8; - asignacion
  write variable; - escribirla en terminal
  read variable; - pedir su nuevo valor en el input y hacerle esa asignacion nueva
  write variable; - escribirla en terminal
  variable = variable + variable; - nueva asignacion
  end;
  ``` 
- El proyecto se ejecuta de la siguiente manera:
  - Para mayor comodidad, se abre el proyecto en CLion, se presiona el botón "Build" y luego se ejecuta el siguiente comando:
     ```
     ./cmake-build-debug/micro ./codigoFuente.mic
     ```
  - Para ejecutarlo desde la terminal se realiza lo siguiente:
    1. Se navega a la carpeta raíz (`/micro`) del proyecto
       ``` 
       cd ~/Documents/TEC/compiladores/micro
       ```
    2. Se navega hasta el directorio de compilación (`/cmake-build-debug`): 
       ``` 
       cd cmake-build-debug
       ```
    3. Se ejecuta el `cmake`:
       ``` 
       cmake ..
       ```
    4. Se compila el proyecto con:
       ``` 
       make
       ```
    5. Se ejecuta con esto (se sigue estando dentro del directorio de compilación):
       ``` 
       ./micro
       ```