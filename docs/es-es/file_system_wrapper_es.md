## El envoltorio del sistema de archivos

El envoltorio del sistema de archivos está contenido en el espacio de nombres `simple_fs`. Incluye `simple_fs.hpp` para utilizarlo.

### Algo de contexto

Aunque existen rutinas de sistema de archivos que vienen con C++, necesitamos imitar el comportamiento de Victoria 2 para cargar adecuadamente sus modificaciones. Victoria 2 utiliza [PhysicsFs](https://icculus.org/physfs/), lo sabemos porque [uno de los desarrolladores nos lo dijo](https://mropert.github.io/2020/07/26/threading_with_physfs/). Entonces, ¿por qué no usar PhysicsFS? Bueno, por dos razones. En primer lugar, es una dependencia adicional, y cada dependencia complica un poco más la vida. Más importante aún, el juego parece utilizar muy pocas de sus capacidades. Según nuestro entendimiento, el sistema de archivos funciona así: agregamos una secuencia de directorios "raíz" al cargar en una modificación (en un juego con solo una modificación, este sería el directorio raíz para los activos del juego y luego el directorio de la modificación). Luego, cada vez que intentamos abrir un archivo, primero lo buscamos en la raíz más recientemente agregada (en nuestro ejemplo, el directorio de la modificación). Si no está allí, entonces lo buscamos en la raíz anterior, y así sucesivamente. Esto permite que las modificaciones en un orden de carga posterior reemplacen archivos de un orden de carga anterior.

Seamos claros: no es la mejor manera de hacer modificaciones, porque significa que las modificaciones no pueden agregar o ajustar archivos, solo sobrescribirlos por completo. Funciona muy bien cuando hablamos de una modificación que reemplaza algunos archivos en el juego base. Pero también dificulta extremadamente la cooperación entre modificaciones, porque no pueden contribuir alteraciones al mismo archivo. Pero, es lo que tenemos.

También puede preguntarse por qué el envoltorio del sistema de archivos se implementa como un conjunto de funciones libres, aunque hay cuatro `clases` definidas para ello. La razón simple es que los datos almacenados en estas clases deben variar según el sistema operativo para el que se compile el programa (por ejemplo, la clase `file` contiene tanto un identificador de archivo de Windows como un identificador de mapeo de memoria). Así que la definición real de las clases termina oculta en archivos de encabezado dependientes del sistema. Y para asegurarse de que la interfaz para interactuar con el sistema de archivos sea la misma sin importar el sistema operativo para el que esté compilando, no se pudo colocar en esos archivos. Por lo tanto, son un conjunto de funciones libres. (Sí, también podría haber heredado de las implementaciones y definido las funciones miembro en el encabezado compartido, pero simplemente no parecía valer la pena el esfuerzo.)

### Tipos de cadenas nativas

Para que ni Windows ni Linux funcionen peor que el otro, todas las cadenas que deben enviarse al sistema operativo o recuperarse de él se almacenan en su formato nativo. Para evitar tener dos versiones de todo, se utilizan las siguientes definiciones para manipular cadenas nativas:

`native_string` -- esto es `std::wstring` en Windows y `std::string` en Linux
`native_string_view` -- esto es `std::wstring_view` en Windows y `std::string_view` en Linux
`native_char` -- esto es `wchar_t` en Windows y `char` en Linux
`NATIVE(...)` -- esto es una macro que puedes usar en literales de cadenas o caracteres para convertirlos a sus equivalentes nativos, por ejemplo, con `NATIVE("text")` o `NATIVE('x')`.

El objeto del sistema de archivos

La clase `file_system` representa el sistema de archivos en su totalidad. Debería haber solo un objeto de estos creado durante la ejecución del programa, y nunca debería ser movido o reubicado (otros objetos del sistema de archivos pueden mantener punteros a él). Hay seis funciones para manipularlo:

- `void reset(file_system& fs)` -- Esta función restablece el `file_system` a su estado recién construido. Esta función probablemente nunca necesitará ser utilizada.
- `void add_root(file_system& fs, native_string_view root_path)` -- Esta función agrega una nueva raíz al sistema de archivos. Actualmente, no hemos decidido si estas deben ser todas relativas al directorio de trabajo actual o si deben especificarse absolutamente. Los nombres absolutos tienden a ser más robustos, ya que durante el desarrollo es fácil especificar rutas arbitrarias para encontrar los archivos necesarios, independientemente de donde se haya creado o lanzado el ejecutable compilado. Sin embargo, a medida que nos acercamos al lanzamiento, probablemente será más amigable para el usuario derivar estas rutas desde la ubicación del ejecutable. Si hacemos las cosas a través de rutas relativas, quizás podamos facilitar las cosas lanzando los ejecutables de depuración con un directorio actual establecido como si estuvieran en la ubicación correcta. Si usáramos rutas absolutas, entonces tendríamos que agregar trucos de CMake para que las compilaciones de todos se crearan donde viven sus activos de juego.
- `void add_relative_root(file_system& fs, native_string_view root_path)` -- Esta función tiene un nombre muy confuso, pero no tenía una idea mejor. Agrega una nueva raíz compuesta por la ruta especificada añadida a la ubicación del archivo exe. **Precaución:** si decidimos usar rutas relativas, no podremos usar esta función.
- `directory get_root(file_system const& fs)` -- Devuelve un objeto de directorio que representa la raíz de nuestro sistema de archivos (pero recuerda, nuestro sistema de archivos es un conjunto de raíces superpuestas, no un solo lugar en la jerarquía de archivos). Dado que necesitas un objeto de directorio para hacer cualquier otra cosa, la mayoría de las interacciones con el sistema de archivos comenzarán con esta función.
- `native_string extract_state(file_system const& fs)` -- Esta función se utiliza para empaquetar la colección de raíces en una única cadena (consulta la siguiente función para obtener más información).
- `void restore_state(file_system& fs, native_string_view data)` -- Esta función establece la colección de raíces del sistema de archivos al estado en que estaba cuando se realizó la llamada anterior a la función. El propósito de estas dos funciones es poder guardar la forma en que estaban las raíces cuando se creó un escenario a partir de una colección de modificaciones. Aunque incluimos la mayoría de la información en el archivo de escenario, no intentamos incluir ninguno de los activos artísticos o de sonido allí. Por lo tanto, para reproducir correctamente el escenario, necesitamos recrear el estado deseado del sistema de archivos para encontrar los activos de arte y música deseados.
- `void add_replace_path_rule(file_system& fs, directory const& dir, native_string_view path)` -- Agrega una regla de "replace_path" para anular un directorio, redirigiendo todos los accesos al directorio a la ruta especificada.
- `std::vector<native_string> list_roots(file_system const& fs)` -- Obtiene una lista de las rutas de raíz registradas en este sistema de archivos.
- `bool is_ignored_path(file_system const& fs, native_string_view path)` -- Si esta instancia específica de una ruta debe ser ignorada, por ejemplo, si una mod anula la carpeta "map/", entonces, para realizar adecuadamente el comportamiento de ignorar los archivos vanilla, ignoramos todos los archivos vanilla y fingimos que no existen, pero no ignoramos la carpeta de la modificación.

El objeto del directorio

La clase `directory` representa una "carpeta" en el sistema de archivos (o, más precisamente, una superposición de carpetas en las diferentes raíces). Al leer archivos, obtendrás directorios a través del objeto `file_system`, ya sea directa o indirectamente. Sin embargo, como se describe en una sección posterior, también hay algunos directorios especiales que puedes obtener que representan ubicaciones especiales en el sistema de archivos del usuario, que no se ven afectadas por las raíces dadas al `file_system`. Solo estos directorios especiales se pueden usar para crear o escribir archivos. Hay seis funciones para manipular directorios:

- `std::vector<unopened_file> list_files(directory const& dir, native_char const* extension)` -- Esta función crea una lista de archivos que podrías abrir dentro del directorio `dir`. Si pasas una cadena no vacía a `extension`, por ejemplo, `NATIVE(".txt")`, obtendrás solo los archivos disponibles que terminan con esa extensión.
- `std::vector<directory> list_subdirectories(directory const& dir)` -- Esta función lista todos los subdirectorios dentro del directorio actual que *no* comienzan con `.`.
- `std::optional<file> open_file(directory const& dir, native_string_view file_name)` -- Esta función abre el archivo con el nombre deseado y coloca su contenido en la memoria, si es posible. Si el opcional no contiene un `file`, entonces el archivo no pudo abrirse. No agregues separadores de ruta dependientes del sistema al nombre del archivo; aunque pudieras hacer que funcione, no sería portátil.
- `std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name)` -- La función se puede usar para verificar si existe un archivo con el nombre dado. Si lo hace, el opcional contendrá un `unopened_file` cuando la función devuelva. Luego puedes usar este `unopened_file` para abrir el archivo con menos sobrecarga.
- `directory open_directory(directory const& dir, native_string_view directory_name)` -- Esta función abre un directorio ubicado dentro del directorio actual. Esta función siempre parecerá tener éxito, incluso si no existe dicho directorio. (Usa `list_subdirectories` si necesitas verificar qué subdirectorios están disponibles). Solo intenta abrir un nuevo "nivel" de directorios a la vez. **No** pases separadores de ruta dependientes del sistema.
- `void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size)` -- Esta función solo se puede llamar con uno de los directorios especiales mencionados anteriormente. Cuando la llamas, se creará un archivo con el nombre especificado, si no existe

, y luego se abrirá. Luego, se escribirán `file_size` bytes de `file_data` en él, borrando cualquier dato anterior en el archivo. Luego, se cerrará el archivo y la función devolverá (esta no es una función asíncrona).
- `native_string get_full_name(directory const& dir)` -- esto devuelve el nombre del directorio, junto con los nombres de cualquier directorio padre en el que exista en relación con la raíz (o raíces) del sistema de archivos, separados por el separador nativo de la plataforma (`\` en Windows, `/` en Linux).

### El objeto de archivo no abierto

La clase `unopened_file` representa un archivo que existe en el sistema de archivos y que podrías abrir, pero que aún no ha sido abierto. Solo hay dos funciones que puedes llamar en esta clase:

- `std::optional<file> open_file(unopened_file const& f)` - Esta función intenta abrir el archivo y cargar sus contenidos en la memoria. Si tiene éxito, el opcional contendrá el objeto de archivo; de lo contrario, estará vacío.
- `native_string get_full_name(unopened_file const& f)` - Esto obtiene el nombre del archivo junto con cualquier directorio que se haya utilizado para llegar a él, así como el raíz particular al que pertenece. Puedes usar esto para enviar el archivo a alguna otra función sin leer datos de él directamente.
- `native_string get_file_name(unopened_file const& f)` - Obtener solo el nombre del archivo.

### El objeto de archivo

Esto representa un archivo abierto que tiene sus contenidos cargados en la memoria (en Windows, está mapeado en memoria). El destructor del objeto de archivo cerrará el archivo y lo descargará de la memoria, así que asegúrate de que el objeto de archivo sobreviva cualquier uso de sus contenidos. Hay dos funciones que puedes llamar en esta clase:

- `file_contents view_contents(file const& f)` - Esta función devuelve el objeto `file_contents`, que contiene un `char const*` no nulo llamado `data` que apunta al primer byte de los contenidos del archivo, y un miembro `uint32_t` llamado `file_size` que contiene la cantidad de bytes en el archivo. No intentes modificar estos bytes.
- `native_string get_full_name(file const& f)` - Esto obtiene el nombre del archivo junto con cualquier directorio que se haya utilizado para llegar a él, así como el raíz particular al que pertenece.

**Linux**: El archivo puede estar mapeado en memoria (es decir, un mapeo de memoria virtual del archivo desde el disco, optimizado por el sistema operativo) si está disponible. Sin embargo, si falta esta funcionalidad (o la biblioteca estándar C es de algunas de esas "distros"), los archivos se asignarán y sus contenidos completos se leerán en la memoria. El comportamiento del primero disminuye el uso de memoria física, mientras que el último lo aumenta.

### Funciones especiales de directorio

Actualmente existen tres funciones especiales de directorio, destinadas principalmente a ubicar el lugar correcto para guardar varios tipos de información.

- `directory get_or_create_save_game_directory()` - En Windows, esto abre (creando si es necesario) una carpeta en la carpeta de documentos del usuario llamada `Project Alice\saved games`. Aquí es donde guardaremos los juegos.
- `directory get_or_create_scenario_directory()` - En Windows, esto abre (creando si es necesario) una carpeta en la carpeta de documentos del usuario llamada `Project Alice\scenarios`. Aquí es donde almacenaremos los escenarios que creamos al agrupar los datos de una colección específica de modificaciones y archivos base.
- `directory get_or_create_oos_directory()` - En Windows, esto abre (creando si es necesario) una carpeta en la carpeta de documentos del usuario llamada `Project Alice\oos`. Aquí es donde agregaremos los volcados OOS.
- `directory get_or_create_settings_directory()` - En Windows, esto abre (creando si es necesario) una carpeta en el directorio de configuración de aplicaciones locales del usuario llamada `Project Alice`. Aquí almacenaremos la configuración global del programa. **No** escribas archivos grandes en este directorio. En particular, **no** escribas registros aquí. Si necesitamos un lugar para los registros, crearé una nueva función especial de directorio para ellos.

### Funciones de conversión de codificación

A veces necesitamos leer un directorio o nombre de archivo desde otro archivo. Creo que los archivos base del juego están en la página de códigos win1250, pero tal vez alguien podría verificar eso por mí. En general, cualquier archivo accesible por el usuario que escribamos estará en utf8. Por lo tanto, se proporcionan las siguientes funciones para convertir entre estos formatos y el formato nativo del sistema de archivos:

- `native_string win1250_to_native(std::string_view data_in)`
- `native_string utf8_to_native(std::string_view data_in)`
- `std::string native_to_utf8(native_string_view data_in)`

Aunque algunas de estas funciones pueden no hacer nada de manera efectiva en Linux, aún necesitas envolver cualquier transición a o desde cadenas nativas en ellas para mantener la portabilidad. Intenta mantener las cadenas nativas en su formato nativo siempre que sea posible.

