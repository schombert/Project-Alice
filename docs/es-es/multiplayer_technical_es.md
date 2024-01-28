## Multijugador

La idea principal es que todos ejecuten la simulación; los comandos se retransmiten desde el servidor a los clientes y a los clientes se les da la señal de "continuar" para avanzar los pasos.

Los clientes están configurados como no pausados y a la velocidad máxima; sin embargo, su progreso está bloqueado por el servidor hasta que reciban un comando que les indique avanzar un paso.

El servidor recibirá comandos de los clientes. Luego, enviará de vuelta los comandos que pudo recibir. Después, esperará a todos los clientes usando un semáforo, para que ningún cliente se quede atrás.

Los comandos deben enviarse en orden cronológico. Los clientes **deben** avanzar un paso cuando se les indique y luego ejecutar los comandos posteriormente como si el jugador los estuviera realizando. Esto significa que todos los comandos deben ser correctamente reconocidos por el cliente en el orden en que ocurrieron.

Cada vez que un cliente se conecta, se le proporciona el checksum del servidor. Este checksum es para fines de seguridad, para evitar que el cliente entre en un estado no válido. El botón "jugar" en la selección de nación estará desactivado si el checksum del cliente y el del host no coinciden. Cada vez que se selecciona un archivo de guardado en el host, se informa a todos sus clientes que el checksum ha cambiado. Esto evita que los clientes que aún no tienen un archivo de guardado se conecten a un juego, o que los clientes con mods diferentes se conecten a un servidor, por ejemplo.

La *Fog of War* o *Niebla de Guerra* se establece de forma obligatoria, sin embargo, sabemos que las personas pueden evadir fácilmente esta restricción, así que principalmente es una cuestión de honor: sé amable con los demás :D

### Reimplementaciones deterministas

Las bibliotecas estándar de C++ y C proporcionan funciones `sin`, `cos`, y `acos` para realizar sus respectivas funciones matemáticas. Sin embargo, la implementación de estas funciones varía según la plataforma y la biblioteca, y dado que estamos intentando ofrecer una experiencia multiplataforma, hemos vuelto a implementar las funciones matemáticas desde cero, en una solución desarrollada internamente.

- `void internal_check(float v, float err, float lower, float upper)` : Algunas funciones matemáticas tienen errores de precisión, sin embargo, verificaremos si hay números inadmisibles mucho más allá de los límites de la función, `err` es el error absoluto máximo permitido para la función, `lower` y `upper` representan los límites inferiores y superiores de la salida. `v` es el valor. Esta función debería equivaler a no realizar ninguna operación en la versión final.

### Comandos de notificación

`notify_player_joins` - Informa a los clientes que un jugador se ha unido, marcando la nación de `source` como controlada por el jugador..
`notify_player_pick_nation` - Elige una nación; esto es útil, por ejemplo, en el vestíbulo donde los jugadores cambian constantemente de nación. SI la `source` es inválida (es decir, una `dcon::nation_id{}`) , entonces se refiere a la nación local actual del jugador en el cliente. Esto es útil para establecer la "nación temporal" en el vestíbulo para que los clientes puedan ser identificados por la nación asignada automáticamente por el servidor. De lo contrario, la `source` es el cliente que solicitó elegir una nación `target` en `data.nation_pick.target`.
`notify_save_loaded` - Actualiza la suma de comprobación de la sesión, utilizada para verificar discrepancias entre clientes y anfitriones que podrían obstaculizar el juego y llevarlo a un estado no válido. A continuación, se presenta un `uint32_t` dque describe el tamaño del flujo de guardado y el propio flujo de guardado.
`notify_player_kick` - Cuando se expulsa a un jugador, se desconecta pero se le permite volver a unirse.
`notify_player_ban` - Cuando se prohíbe a un jugador, se desconecta y no se le permite volver a unirse.
`notify_start_game` - El anfitrión ha iniciado el juego, todos los jugadores conectados serán enviados al juego.
`notify_stop_game` - El anfitrión ha detenido el juego (no pausado), todos los jugadores conectados serán enviados al vestíbulo.
`notify_pause_game` - El anfitrión ha pausado el juego, principalmente para notificar a los clientes que el anfitrión ha pausado el juego.

El servidor enviará a los nuevos clientes un `notify_player_joins` por cada jugador conectado. Enviará un `notify_player_pick_nation` al cliente, con una `source` inválida, informándole cuál es su "nación asignada".

Una nación asignada es una nación "aleatoria" que el servidor asignará al cliente para que pueda conectarse identificablemente al servidor como una nación y ejecutar comandos como tal nación.

### Flujos de guardado

Enviamos una copia del guardado al cliente, ultra comprimida, para permitirle conectarse sin tener que usar herramientas externas. Esto se hace, por ejemplo, cuando el anfitrión está cargando un archivo de guardado: al cliente se le da la nueva información del archivo de guardado para mantenerlos sincronizados.

### Unirse en caliente (Hot-join)

Una nueva funcionalidad es unirse en caliente a sesiones en ejecución: el cliente puede conectarse al anfitrión y este les asignará una nación aleatoria, generalmente naciones no civilizadas. Si desean cambiar su nación, tendrán que pedir al anfitrión que vuelva al vestíbulo. Esta es una pequeña medida para evitar abusos o que personas aleatorias entren en juegos para arruinarlos, dado que se asume que la mayoría de las personas elegirán grandes potencias.

### Fuera de sincronización (OOS)

En compilaciones de depuración, se generará una suma de comprobación en cada tick para asegurar que la sincronización no se haya roto. Si ocurre una desincronización, se señalará en el tick donde ocurrió y se generará un volcado correspondiente de OOS.

De lo contrario, el objetivo es no tener más OOS :D
