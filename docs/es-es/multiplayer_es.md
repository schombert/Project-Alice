## Instrucciones para Multijugador

### Unirse a una partida

Después de iniciar el lanzador, verás algo similar a lo siguiente:

Al igual que en un juego para un solo jugador, primero deberás crear un escenario para la combinación de modificaciones con las que deseas jugar (los botones de Iniciar juego, Hospedar y Unirse estarán desactivados si no lo has hecho). Esta combinación de modificaciones *debe* coincidir con el juego al que intentas unirte (lo que incluye coincidir con la versión de esas modificaciones; recuerda que cambiar los archivos de modificación *no* es suficiente -- también debes reconstruir el escenario).

Luego, cambia tu nombre de usuario (actualmente, las casillas de texto son un poco complicadas: debes colocar el mouse sobre ellas y luego escribir -- hay un cursor invisible que actúa como si siempre estuviera al final del texto) e ingresa la dirección IP del anfitrión al que deseas conectarte (si copias esa dirección IP de otro lugar, usa el atajo *ctrl+v* para pegarla en la casilla de dirección).

Unirte al juego te llevará a la pantalla de selección de nación, excepto que verás a otros jugadores y al anfitrión. Puedes seleccionar cualquier nación que desees jugar, siempre y cuando no esté controlada por otro jugador, ya que dos jugadores no pueden controlar la misma nación (todavía).

Como cliente, no podrás controlar la velocidad del juego ni cargar archivos guardados por tu cuenta; el anfitrión tiene poder absoluto sobre la sesión de juego. Los anfitriones pueden expulsar y prohibir a las personas en cualquier momento, y también pueden regresar al vestíbulo momentáneamente para permitir que los jugadores seleccionen otras naciones o cambien.

El *Fog of War* o *Niebla de Guerra* siempre está activado en multijugador

- NOTA: Si escribes una dirección IPv6, tu anfitrión también deberá aceptar conexiones IPv6, o de lo contrario, fallará. Lo mismo ocurre con IPv4; generalmente, se recomienda IPv4 por razones de experiencia del usuario.
- NOTA: Los dominios no son compatibles, resuélvalos manualmente e ingrese su correspondiente dirección IPv4 (esto es una característica para el futuro).

#### Alojando una partida

Si estás utilizando IPv4, escribe un punto "." en el campo de IP. Si vas a utilizar IPv6, escribe un punto y coma ";" en el campo de IP. Los clientes que utilizan IPv4 no pueden conectarse a un host con IPv6.

Para aceptar conexiones de clientes (sin usar un servicio de túneles como Hamachi), deberás poder aceptar conexiones a través del puerto 1984. Por lo general, esto requiere realizar un reenvío de puertos en tu enrutador hacia tu máquina local. Puedes utilizar una [herramienta de verificación de puertos abiertos](https://www.yougetsignal.com/tools/open-ports/) para asegurarte de que has abierto los puertos correctamente, ten en cuenta que este tipo de herramientas pueden no ser 100% confiables, por lo que se prefiere la ayuda de otra persona para realizar las pruebas.

Al proporcionar a los jugadores tu dirección IP para que puedan conectarse contigo, debes darles la dirección que tu máquina tiene desde la perspectiva de Internet (es decir, no la dirección de tu subred local).

Como anfitrión, podrás expulsar y prohibir a personas tanto en el juego como en el vestíbulo. Asegúrate de haber preparado todo de antemano.

Ten en cuenta que cargar archivos de guardado enviará el archivo a cada cliente que esté conectado y que se conectará. Por ejemplo, si tienes 8 jugadores, enviarás el archivo de guardado 8 veces, que suele ser de alrededor de 3 MB, así que ten en cuenta que puedes terminar enviando 3*8=24 MB de información CADA vez que cargues un archivo de guardado. Ya está comprimido utilizando el nivel máximo de compresión disponible, por lo que no tiene sentido comprimirlo aún más (según las leyes matemáticas de la compresión, tiene su propio artículo en Wikipedia del cual no me detendré a explicar).

Para lidiar con los alborotadores:
- Expulsar (Kick): Desconecta al jugador de la sesión, pueden volver a unirse si lo desean.
- Prohibir (Ban): Desconecta y pone en lista negra al jugador de la sesión, no podrán volver a unirse a menos que se realice una nueva sesión. Basado en la IP (o MAC, si se utiliza IPv6) - nuevamente, cualquiera lo suficientemente dedicado puede eludir esto, pero funciona en la mayoría de los casos prácticos.

#### Solución de Problemas

Para Clientes:
- Verifica que la IP que ingresaste sea correcta.
- Asegúrate de que tu nombre de jugador no esté a) vacío, o b) incorrecto.
- Verifica que no hayas sido prohibido del servidor.
- Asegúrate de estar conectando utilizando IPv4 o IPv6, ya que los anfitriones no aceptarán un cliente con una IP diferente.
- Si usas IPv6, verifica que tu enrutador incluso lo admita.
- Si usas IPv4, no ingreses puntos y comas (es decir, `127.0.0.1:1283`), ya que se interpretará como una dirección IPv6.
- Verifica que tu firewall no esté bloqueando a Alice.
- Verifica tu conexión a Internet (quizás incluso haz un ping al anfitrión).
- Si estás utilizando una aplicación de túneles VPN, asegúrate de estar conectado al par.
- Asegúrate de que todos tengan la misma versión del juego.
- Verifica que otra persona pueda unirse (es decir, que el anfitrión haya reenviado correctamente el puerto 1984).
- Asegúrate de tener los mismos mods que el anfitrión.
- Puedes intentar usar el mismo archivo de escenario si piensas que estás teniendo problemas causados por versiones ligeramente diferentes de mods.

Para Anfitriones:
- Verifica que estás utilizando la IP correcta que deseas (los clientes también deben ser informados sobre qué IP necesitan usar).
- Asegúrate de que tu nombre de jugador no esté a) vacío, o b) incorrecto.
- Si usas IPv6, verifica que tu enrutador incluso lo admita.
- Si usas IPv4, no ingreses puntos y comas (es decir, `:1283`), ya que se interpretará como una dirección IPv6.
- Verifica que tu firewall no esté bloqueando a Alice.
- Verifica tu conexión a Internet.
- Verifica que el puerto `1984` esté correctamente reenviado en tu máquina (puedes usar una herramienta de verificación de puertos abiertos para analizar esto).

Translate by: elmatero08