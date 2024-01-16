## Otras herramientas de análisis diversas

Todas estas funciones están declaradas en `parsers.hpp`

### Conversión de texto a valores

Se proporcionan las siguientes funciones para convertir texto en los tipos de valores apropiados. Son bastante autoexplicativas, excepto que señalaré que todas esperan recibir solo el texto que contiene el valor, sin ningún espacio en blanco circundante (consulte la siguiente sección para eso).

```
float parse_float(std::string_view content, int32_t line, error_handler& err);
double parse_double(std::string_view content, int32_t line, error_handler& err);
bool parse_bool(std::string_view content, int32_t line, error_handler& err);
int32_t parse_int(std::string_view content, int32_t line, error_handler& err);
uint32_t parse_uint(std::string_view content, int32_t line, error_handler& err);
association_type parse_association_type(std::string_view content, int32_t line, error_handler& err);
```

### Otras funciones de utilidad

```
std::string_view remove_surrounding_whitespace(std::string_view txt);
```
Esta función elimina las formas comunes de espacios en blanco ASCII (` `, `\t`, `\r`, `\n`) de una cadena (técnicamente solo mueve la vista alrededor, pero funciona de la misma manera).

### Lectura de texto de un "csv"

Paradox incluyó Victoria 2 con algunos datos en un formato seudo-csv. A diferencia de los archivos csv normales, puede contener comentarios (¿no crees que los archivos csv normales tienen comentarios, verdad?) y no sigue las reglas csv para las comillas. Cuando abres un archivo csv para comenzar a analizar datos, lo primero que debes hacer es escribir las siguientes líneas:
```
if(sz != 0 && cpos[0] == '#')
	cpos = parsers::csv_advance_to_next_line(cpos, file_data + sz);
```
donde `cpos` es tu posición actual en los datos del archivo,  `file_data` es el comienzo del archivo (probablemente igual a `cpos` cuando estás haciendo esto) y `sz` es el número de bytes en el archivo. Esto avanzará la posición más allá del primer comentario. Nunca tendrás que hacer esto manualmente nuevamente, y todos los comentarios subsiguientes se omitirán automáticamente.

Para leer datos del archivo csv, usarás una de las siguientes dos funciones:
```
template<size_t count_values, typename T>
char const* parse_fixed_amount_csv_values(char const* start, char const* end, char separator, T&& function)
```
o
```
template<typename T>
char const* parse_first_and_nth_csv_values(uint32_t nth, char const* start, char const* end, char separator, T&& function)
```

La primera función,  `parse_fixed_amount_csv_values`, debe suministrarse con un parámetro de plantilla entero explícito para `count_values`, luego, llamará a `function` (probablemente una lambda que escribas) una vez por cada línea no comentada del archivo con una matriz de `std::string_view` de tamaño `count_values` que contiene los primeros `count_values` elementos de esa línea del archivo csv.

La segunda función, `parse_first_and_nth_csv_values`, funciona básicamente de la misma manera. Llama a  `function` una vez por línea del archivo con dos parámetros: un `string_view` que contiene el primer elemento de la línea y un `string_view` que contiene el elemento `nth` en la línea. Esto es útil al leer texto de los archivos que contienen las traducciones, ya que necesitas la clave, el primer elemento y luego algún elemento nth correspondiente al idioma que deseas.

El parámetro `separator` para ambas funciones determina el carácter que se utiliza para marcar dónde termina un elemento del archivo csv y comienza el siguiente. Para nosotros, creo que siempre será `;`.

**Nota** Los elementos que obtienes de un archivo csv pueden incluir espacios en blanco al principio y/o al final. Usa  `remove_surrounding_whitespace` primero si deseas obtener un valor de ellos.

### Análisis del archivo de definiciones similar a Lua

Estaba pensando en definir algunas funciones adecuadas para esto, pero dado que es un archivo que analizaremos una vez y no hay otros lugares donde tales funciones sean relevantes, planeo hacer algo ad hoc. Si realmente quieres una explicación de cómo analizaré el archivo, házmelo saber.