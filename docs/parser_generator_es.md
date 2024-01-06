## El Generador de Analizadores: Cómo funciona

Hay algo importante que tener en cuenta al hablar del generador de analizadores, y es la distinción entre el generador, que analiza un archivo describiendo la salida que queremos de él, y los analizadores que genera, que pueden analizar la mayoría de los archivos de datos que se envían con el juego y convertirlos en datos útiles. Así que cuando hablamos de "el analizador", asegúrate de tener en cuenta si estamos hablando del analizador dentro del generador o de los analizadores que genera.

### Una breve lección de historia

Hay varias cosas extrañas y confusas en el generador y su formato de archivo, y eso se debe a su historia relativamente complicada. Eventualmente, sería bueno convertirlo y el archivo que consume en algo limpio y ordenado, pero lo que tenemos ahora fue tomado del proyecto open v2 con solo ajustes mínimos. Originalmente, el análisis en open v2 se manejaba mediante magia negra de plantillas que convertía un tipo que en sí mismo estaba estructurado como un árbol de tipos en un analizador utilizando información en cada nodo de ese árbol en tiempo de compilación. Esto sonaba extremadamente ingenioso en ese momento, pero era una verdadera pesadilla y a veces hacía que el compilador se bloqueara. Como una mejora menor, los analizadores se hicieron con la ayuda de varios macros, que aún eran molestos de compilar, pero al menos eran un poco más fáciles de trabajar. Eventualmente, me di cuenta de que había algunas mejoras de rendimiento obvias que podría obtener al tiempo que reducía considerablemente los tiempos de compilación, así que empecé a moverme hacia algo como el generador que tenemos aquí. Sin embargo, esa transición no se completó totalmente en open v2, y la mayoría de las definiciones de macros todavía existen allí.

Durante el desarrollo de open v2, la forma en que se usaban los analizadores cambió a medida que abordaba más y más archivos, lo que me expuso a varios casos límite, y simplemente experimentaba con diferentes formas de abordar las cosas para tratar de encontrar mejores soluciones. El resultado de esto es que el sistema de macros creció para incorporar varias formas diferentes de hacer las cosas, y para facilitar la compatibilidad con ese sistema, el generador de analizadores se construyó para poder hacer básicamente todo. Espero que eventualmente podamos simplificar las cosas, porque como tengo la intención de recrear más o menos las definiciones de los analizadores desde cero, intentaré seguir una forma de hacer las cosas, permitiendo a alguien (espero que otra persona) volver, eliminar las partes no utilizadas y dejarnos con algo limpio y ordenado. (Y sé de una optimización adicional, no implementada, para los analizadores generados que también me gustaría agregar, algún día.)

Como aproximación muy áspera, podemos pretender que el generador está destinado a admitir dos enfoques diferentes para el análisis. En el primer enfoque, analizamos un "grupo" particular en el archivo (algo que se ve como `key = { ... }`) creando un objeto que almacena cualquier información contenida en el grupo y luego la devuelve. Esta es una forma muy natural de hacer las cosas porque crea una jerarquía de objetos C++ que refleja el contenido del archivo, que luego puedes procesar fácilmente después de haber terminado con el archivo. El segundo enfoque para el análisis es colocar datos en su destino final inmediatamente al leer el archivo. Para hacer esto, a medida que avanzas a través de "grupos" anidados en el archivo, construyes un "contexto" (una representación de dónde estás en el archivo, en términos generales, así como referencias a los objetos externos que necesitas para almacenar los datos) que se pasa al analizador y que a su vez se pasa a cualquier función que llame para manejar los datos. En el primer enfoque, el analizador almacena valores que encuentra principalmente asignándolos a variables miembro de la clase con la que está asociado (más sobre esto a continuación); en el segundo enfoque, llama principalmente a funciones en esos datos a medida que los encuentra.

### Contenidos de los archivos en formato paradox que se analizarán en última instancia

Hay un término un poco idiosincrático que he desarrollado accidentalmente mientras escribía estas diversas iteraciones de analizadores, y ahora supongo que debes aprenderlo para facilitar la comprensión del generador de analizadores. Lo siento.

En el nivel más general, un archivo en formato paradox es una lista de elementos, y cada elemento puede ser una de cuatro cosas.

En primer lugar, un elemento puede ser un "valor" (`value`) o "asociación" (`association`)  (lo siento, ambos términos se usan en lugares), que generalmente tiene la forma de `yyy = zzzz`, `yyy` y `zzzz` pueden ser cualquier secuencia de caracteres que no incluya espacios, tabuladores, saltos de línea o corchetes, o pueden ser una secuencia de caracteres que contenga cualquiera de esos excepto los saltos de línea, siempre y cuando esté rodeada por comillas. El signo igual en sí se llama su "tipo de asociación" (`association type`). Para cualquier archivo en formato paradox normal, siempre es `=`. Sin embargo, para facilitar la modificación, también admitimos `<`, `>`, `>=`, `<=`, `!=`, `==`, y `<>` como tipos de asociación. (Esta no es una característica nueva; ya estaba en open v2 y sería más difícil quitarla en este momento).

El siguiente elemento más común es el "grupo" (`group`), que tiene la forma `yyy = { ... }` donde `...` es en sí una lista de cero o más elementos. Strictamente hablando, cualquiera de los analizadores generados analiza un solo grupo (tratamos todo el contenido del archivo como un grupo implícito). Cuando un analizador encuentra un grupo, pasa el contenido de ese grupo a otro analizador y luego hace algo con el valor de retorno que recibe.

Luego tenemos los elementos mucho más raros. Ocasionalmente nos encontramos con un "valor libre" (`free value`), es decir, algo que parece simplemente `yyy`, pero que no va seguido de un tipo de asociación compatible (los más comunes de estos son números). Todos estos se envían al mismo controlador de valores libres. Y luego está el más raro de todos, el "grupo libre" (`free group`), que se ve así: `{ ... }`, es decir, una lista de elementos pero sin la etiqueta que esperarías encontrar en un grupo. El contenido de cada lista se analiza mediante el controlador de grupos libres. (Ten en cuenta que en un momento a estas cosas se les llamaba conjuntos libres o simplemente conjuntos, y aún puedes ver algo de esa terminología en los archivos fuente).

Es importante señalar que los comentarios no necesitan tenerse en cuenta; se filtran antes de que el analizador los vea. (Así es, hay un pequeño preprocesador).

### Definición de un analizador para el contenido de un grupo

Ten en cuenta que cada archivo es un grupo implícito, por lo que el analizador para cada tipo de archivo comienza con uno de estos que luego llama a otros analizadores de grupos.

Para cada analizador que deseamos definir, también debemos definir una clase única. El contenido de la clase se utilizará para almacenar los datos analizados o proporcionar funciones para procesarlos. Hay dos restricciones en estas clases. Primero, deben ser constructibles por defecto. En segundo lugar, deben admitir una función `finalize` que tome como único parámetro el contexto proporcionado al analizador (esta función se llama cuando el analizador ha terminado de leer el grupo, lo que puede ser útil cuando se desea hacer algo con los datos de inmediato, pero no se puede hacer hasta que se tengan todos los contenidos del grupo). Si tu analizador no utiliza un contexto, simplemente colocaría `int32_t` como el tipo de este parámetro (quizás algún día obtendremos un void regular).

Comenzamos la definición para cada analizador colocando el nombre del tipo asociado en una línea individual sin espacios ni tabulaciones previas. Luego, cada línea que siga a esa, que comience con una sola tabulación, se utilizará para definir ese analizador. Todas las líneas deben estar en minúsculas y solo pueden contener los caracteres '_', '@' o '#' para las cadenas; cualquier otro carácter (excepto paréntesis y caracteres de espacio, incluidas las comas) se tratará como un error. Desafortunadamente, el meta-analizador no está diseñado para la robustez, así que asegúrate de usar una sola tabulación para comenzar cada línea, usa espacios y no tabulaciones en otros lugares de la línea para la justificación, y no insertes líneas en blanco adicionales dentro de la definición de un analizador en particular.

Por ejemplo, visualmente, la definición de un analizador sintáctico podría verse así:
```
type_name
	line 1
	line 2
	line 3
	...
```

#### Definiendo cómo se manejan los valores

Esta sección describe primero tus opciones para analizar un valor en la forma `yyy = zzz` cuando ya conoces de antemano cuál será la etiqueta de texto `yyy`. Ten en cuenta que la coma `,`  se tratará como espacio en blanco y, por lo tanto, se puede utilizar libremente con fines de formato. Para analizar dicho valor, agrega una línea como la siguiente a la definición del analizador:
```
	yyy, value, type, handler_type
```
o una línea como
```
	yyy, value, type, handler_type (destination_name)
```
La palabra clave `value` debe estar presente para indicar al generador que estamos analizando un valor llamado `yyy` y no un grupo. `type` puede ser uno de los siguientes: `float`, `int` (para `int32_t`), `uint` (para `uint32_t`), `bool`, `text` o `date` (`date` no está actualmente soportado en el momento en que se está escribiendo este documento porque aún no lo he necesitado). Esto es simplemente una forma conveniente de realizar transformaciones comunes en `zzz`. Si especificas `text`, obtendrás el texto de `zzz` excepto con las comillas circundantes eliminadas, si las hay. `handler_type` debe ser uno de `discard`, `member`, `member_fn` o `function` (los efectos de esto se describen a continuación). Finalmente, `(destination_name)` si está presente, siempre debe estar rodeado por paréntesis. Al manejar `zzz`, la clave `yyy` también se utiliza como el nombre del miembro, la función de miembro o la función libre, a menos que anules esta elección especificando un nombre de destino (puedes usar esto para enviar varios valores al mismo controlador, por ejemplo, o para evitar violar las reglas de los identificadores de C++ si `zzz` resulta ser una palabra clave reservada).

Para cada línea que agregues para manejar un valor, debes tener algo preparado para aceptarlo, a menos que desees descartarlo, que es lo que hará especificar `discard` para `handler_type`. Si especificas `member` para `handler_type`, entonces la clase asociada al analizador debe tener un miembro con el nombre de destino correcto y que sea de un tipo con un `operator=` que pueda aceptar el tipo de `zzz` analizado. Si `handler_type` es `member_fn`, entonces la clase debe tener una función de miembro con el nombre de destino y que pueda aceptar los siguientes parámetros: un enumerado de tipo `association_type`, el tipo de `zzz` analizado, un objeto de la clase `error_handler` por referencia, un `int32_t` que contenga el número de línea en el que se leyó el lado izquierdo del valor, y el contexto para el analizador, ya sea por valor o por referencia (cuando no se esté utilizando el contexto, la convención es hacerlo de tipo `int32_t`). El parámetro `association_type` te permite inspeccionar el tipo de asociación que estaba presente para el valor, y puedes usar el `error_handler` y el número de línea para registrar si el valor no puede ser aceptado por alguna razón. Finalmente, `function` funciona de la misma manera que `member_fn`, excepto que llama a una función libre con el nombre de destino especificado y lo pasa como un primer parámetro adicional el objeto utilizado para el análisis, por referencia. `function` realmente no proporciona mucha flexibilidad adicional, y trataré de evitarlo.

Además de manejar valores donde el lado izquierdo es una cantidad conocida que está fija de antemano, también puedes enviar tanto el lado izquierdo como el lado derecho de un valor a una rutina de tu elección (siempre que no coincida con un lado izquierdo específicamente explícito). Hacer esto requiere una línea como la siguiente:
```
	#any, value, type, handler_type (destination_name)
```
Y como de costumbre, el contenido entre paréntesis también puede omitirse por completo. Y al igual que en el caso en el que especificas explícitamente el lado izquierdo, el texto `value` es necesario para indicar que esta línea se aplica a los valores. `type` y `handler_type` también toman los mismos valores con prácticamente los mismos significados que antes. `discard` como tipo de manipulador funciona como lo hacía antes y puede ser útil si deseas descartar todos los demás valores que no hayas coincidido explícitamente sin generar errores. `member` es en su mayoría inútil en este contexto, ya que no comunica ninguna información sobre cuál fue el lado izquierdo de la expresión de valor. `member_fn` y `function` se modifican en el sentido de que recibirán un `string_view` adicional como su primer o segundo parámetro, respectivamente, que contiene el lado izquierdo de la expresión de valor.

#### Definiendo cómo se manejan los grupos

Especificar cómo debe analizarse un grupo se hace de manera muy similar a como se hace para los valores, pero con algunas opciones adicionales, ya que puedes escribir la línea necesaria de cualquiera de las siguientes maneras.
```
	yyy, group, type, handler_type (destination_name)
```
o
```
	yyy, extern, function_name, handler_type (destination_name)
```
Con `(destination_name)` siendo opcional como antes. Ambas líneas instruirán al analizador generado sobre qué hacer con un elemento de grupo que se ve como `yyy = { ... }`. La diferencia entre `group` y `extern` es que `group` manejará el contenido del grupo (es decir, los elementos que componen `...`) invocando otro analizador generado (llamando al analizador que estaba asociado con la clase `type` con el contenido de ese grupo y el contexto actual). `extern`, por otro lado, llamará a la función con el nombre dado con tres parámetros: un objeto de tipo `token_generator` por referencia, un objeto de clase `error_handler` por referencia y el contexto para el analizador, ya sea por valor o por referencia. Esto es útil cuando realmente estás utilizando el contexto. El patrón típico con `extern` es simplemente usarlo como un lugar para crear un contexto con más información antes de invocar otro analizador generado para manejarlo.

`handler_type` y `destination_name` funcionan esencialmente como lo hacen para los valores, excepto que en lugar de manejar el lado derecho analizado del valor, manejan el valor de retorno del analizador u otra función, con la única diferencia de que no hay un parámetro `association_type`, ya que eso no tiene sentido para los grupos.

**NOTA** Otra diferencia menor entre `group` y `extern` es que incluso si especificas manejar `extern` con `discard`, aún se ejecutará en el contenido del grupo, lo cual es útil si te inclinas hacia procesar los datos de inmediato y no tienes nada útil que devolver. Sí, esto hace que la lectura de la definición sea un poco confusa.

Al igual que con los valores, puedes reemplazar `yyy` con `#any` para analizar grupos donde el token a la izquierda del `=` no está fijo de antemano. Y al igual que con los valores, esto resultará en un parámetro adicional `string_view` que se enviará al controlador. Además, una función llamada debido a `extern` también recibirá un `string_view` que contiene el texto a la izquierda del `=` como un primer parámetro adicional (esto no está disponible para analizadores generados automáticamente llamados debido a `group`, ya que no pueden tomar parámetros adicionales).

#### Definir cómo se manejan los valores libres

Para definir cómo se debe manejar cualquier valor libre encontrado, agrega una línea como
```
	#free, value, type, handler_type (destination_name)
```
A la definición del analizador sintáctico. `value` debe estar presente para indicar que así es como deseas manejar valores libres y no conjuntos libres. Al igual que con los valores ordinarios, `type` determina el tipo de valor que se analizará inmediatamente, `handler_type` determina qué hacer con ese valor, y `destination_name`, si está presente, anula el nombre predeterminado del controlador (que predeterminadamente es `free_value` si no se especifica algo).

Si `handler_type` es `member_fn`, entonces la clase debe tener una función miembro que pueda aceptar los siguientes parámetros: el tipo de `zzz` analizado, un objeto de la clase `error_handler` por referencia, un `int32_t` que contiene el número de línea en el que se leyó el lado izquierdo del valor, y el contexto para el analizador, ya sea por valor o por referencia. (**NOTA** la diferencia con los valores estándar es la ausencia del enum `association_type`) `function` funciona de la misma manera que `member_fn`, excepto que llama a una función libre y le pasa como primer parámetro adicional el objeto utilizado para el análisis, por referencia.

Ten en cuenta que solo puedes tener un controlador para valores libres. Si necesitas poder aceptar diferentes tipos de valores libres (por ejemplo, datos numéricos y de texto), entonces deberás especificar `text` como el tipo y luego decidir qué tipo de valor es por ti mismo.

#### Definir cómo se manejan los grupos libres

Para definir cómo se debe manejar un grupo libre, agrega una línea como
```
	#free, group, type, handler_type (destination_name)
```
o
```
	#free, extern, function_name, handler_type (destination_name)
```
A la definición del analizador sintáctico. Funcionalmente, los grupos libres funcionan exactamente de la misma manera que los grupos normales, excepto que su nombre de destino predeterminado es `free_group`. Pero realmente no deberías preocuparte por eso; los grupos libres son tan raros que ni siquiera estoy seguro de que haya uno presente en los archivos de Victoria 2 (sí, la especificación de estos analizadores se derivó de una especificación aún más antigua basada en archivos de CK2, no preguntes).

#### Reutilización de elementos de una definición de analizador sintáctico existente

Finalmente, es posible reutilizar, de manera algo limitada, el contenido de una definición de analizador sintáctico dentro de otra. Puedes hacer esto agregando la línea
```
	#base, name
```
Donde `name` es el nombre que has utilizado para alguna otra definición de analizador sintáctico *previamente en el mismo archivo*. Esto funciona esencialmente "copiando" el contenido de la definición anterior, y no hay disposición para anular nada allí; solo puedes agregar nuevos elementos.

### Una breve nota sobre los analizadores sintácticos generados de esta manera

Un analizador sintáctico generado puede ser invocado como `parsers::parse_typename(token_gen, error_record, context)`, donde `token_gen` es de tipo `parsers::token_generator`, `error_record` es de tipo `parsers::error_handler`, y `context` puede ser cualquier cosa que desees, aunque la convención es pasar `0` si no se está utilizando el contexto.

Un `token_generator` se crea pasando dos punteros `char` al constructor, indicando el rango de texto del cual se deben extraer los tokens (en el estilo usual de C++, por lo que el segundo puntero es la ubicación de memoria uno más allá del final del rango). Generalmente, deberías crear solo un generador de tokens por archivo.

Un `error_handler` se crea pasando una cadena al constructor que contiene el nombre del archivo al cual se deben atribuir los errores. Para agregar un error personalizado a un objeto de este tipo, simplemente agrega el mensaje descriptivo (idealmente, incluyendo tanto el nombre del archivo almacenado en el miembro `file_name` como un `std::string` y el número de línea) a su miembro `accumulated_errors`, que es de tipo `std::string`. Asegúrate también de que tu mensaje de error termine con `\n`. La convención es asumir que si la longitud de `accumulated_errors` es cero, entonces no hubo errores al analizar el archivo. Actualmente, no hay forma de expresar advertencias.