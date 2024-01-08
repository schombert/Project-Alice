## Fundamentos de interactuar con el bucle del juego

El bucle del juego siempre está en ejecución en segundo plano, incluso cuando el juego está nominalmente pausado. Sin embargo, cuando el juego está pausado y el bucle del juego no tiene nada que hacer, cede su tiempo de nuevo al planificador (a través de `sleep`). Prácticamente, esto significa que no te comunicas con el bucle principal del juego llamando funciones. Normalmente, le envías mensajes escribiendo datos en algún lugar y confiando en que los recoja en la próxima iteración. Y de manera similar, cuando el bucle del juego quiere notificar al resto del programa que ha ocurrido un evento, escribe un mensaje en algún lugar y confía en que la función de renderizado, que se llama de forma regular, lo recoja.

### Comunicación de la velocidad del juego

En lo que respecta a la lógica de actualización, hay 6 velocidades de juego, cada una almacenada en la variable miembro `actual_game_speed`, siendo 0 pausado y 5 la velocidad sin restricciones. La lógica desde el lado de la interfaz puede ser más complicada. Es posible que desees realizar un seguimiento de una velocidad de juego deseada que persista durante las pausas. Si es así, depende del código de interfaz de usuario hacer un seguimiento de ello. Trata `actual_game_speed` más como una señal para el bucle del juego. Escribe tu velocidad de juego deseada en ella con `actual_game_speed.store(speed_value, std::memory_order::release)` y el bucle del juego lo recogerá la próxima vez que compruebe la velocidad.

Al mostrar si el juego está pausado, debes mostrar la velocidad como pausada si actual_game_speed es actualmente igual a 0 o si el miembro `internally_paused` de `state` es verdadero (esto se establece cuando la lógica del juego impide que la fecha avance; nunca debería ser alterado por la interfaz de usuario). Al decidir esto, ambas valores se pueden leer sin ningún tipo de salvaguardias atómicas, ya que leerlos incorrectamente es esencialmente sin consecuencias en esa situación.

### Respuesta a un cambio en el estado del juego

El estado del juego puede cambiar como resultado de procesar un nuevo día, que probablemente sea el caso más común. Sin embargo, también puede cambiar mientras el juego está pausado como resultado de comandos del jugador (por ejemplo, ajustar su presupuesto o seleccionar una opción de evento). Afortunadamente, esta diferencia es en gran medida irrelevante para la interfaz de usuario. Antes de comenzar a renderizar un nuevo fotograma, realizamos lo siguiente:
```
auto game_state_was_updated = game_state_updated.exchange(false, std::memory_order::acq_rel);
	if(game_state_was_updated) {
		ui_state.units_root->impl_on_update(*this);
		ui_state.rgos_root->impl_on_update(*this);
		ui_state.root->impl_on_update(*this);
		// TODO map needs to refresh itself with data
		// TODO also need to update any tooltips (which probably exist outside the root container)
	}
```
La parte que se actualiza actualmente envía un mensaje de `update` a través de la jerarquía de la interfaz de usuario, y cualquier control implementado correctamente responderá actualizando cualquier dato que almacene localmente a partir del estado del juego, recreando su contenido de texto si es necesario, y así sucesivamente. También es necesario, pero actualmente no implementado, que el mapa actualice los datos en los que se basa el modo de mapa actual, y las herramientas de información, que probablemente estén almacenadas en un contenedor raíz de interfaz de usuario diferente, también deberán actualizarse.

### Envío de comandos

La interfaz de usuario puede transmitir las acciones del jugador al estado del juego enviando comandos a través de las funciones proporcionadas en `commands.hpp`. Cada comando distintivo tiene dos funciones asociadas: una función con un nombre que describe lo que hace el comando (por ejemplo, `set_national_focus`) y luego otra función llamada `can_...`. La primera función envía el comando al estado del juego. La segunda función devuelve un booleano que indica si ese comando es actualmente válido. Aunque puedes enviar comandos inválidos, el juego los rechazará silenciosamente. Esta segunda función es útil para determinar cuándo debes deshabilitar un botón, por ejemplo. Sin embargo, como las herramientas de información a menudo necesitarán explicar *por qué* el botón está desactivado, la función también puede ser igualmente útil como referencia para ver todas las condiciones, en código, que pueden bloquear la ejecución de un comando en particular.

La documentación sobre qué comandos son posibles y qué hacen en inglés sencillo se puede encontrar al [final del documento de reglas](rules_es.md#Comandos).

### Envío y recepción de mensajes más complicados a la interfaz de usuario desde el estado del juego

Además de simplemente informar a la interfaz de que las cosas han cambiado y que necesita actualizarse, hay tres tipos de notificaciones más complicadas que el estado del juego puede querer enviar a la interfaz de usuario. Estas son: notificaciones de que ha ocurrido un nuevo evento con el que el jugador local puede interactuar, nuevas solicitudes diplomáticas (como una solicitud de alianza o acceso militar) que el jugador local puede aceptar o rechazar, y nuevos mensajes de notificación que pueden terminar en un pop-up o en el registro de mensajes.

Cada uno de estos se pasa a la interfaz de usuario a través de una cola que es miembro de `sys::state`. La interfaz de usuario debe vaciar estas colas tan pronto como sea posible (idealmente, cada vez que se renderiza un nuevo fotograma) y colocar la información en su propio almacenamiento interno.

Las colas que contienen nuevos eventos son, para eventos:
```
rigtorp::SPSCQueue<event::pending_human_n_event> new_n_event;
rigtorp::SPSCQueue<event::pending_human_f_n_event> new_f_n_event;
rigtorp::SPSCQueue<event::pending_human_p_event> new_p_event;
rigtorp::SPSCQueue<event::pending_human_f_p_event> new_f_p_event;
```

Para solicitudes diplomáticas:
```
rigtorp::SPSCQueue<diplomatic_message::message> new_requests;
```

Para mensajes de notificaciones generales:
```
rigtorp::SPSCQueue<notification::message> new_messages;
```

Las colas de eventos contienen la descripción completa de los eventos que han ocurrido para la nación controlada por el jugador local / provincias dentro de esa nación. Toda esa información es necesaria para que el jugador elija eventualmente una opción de evento, ya que identifica qué evento, de todos los eventos posibles para cualquier jugador humano, es la elección.

Los mensajes de solicitud diplomática contienen una cantidad muy mínima de información. Para responder a dicha solicitud, la interfaz de usuario debe llamar a `command::respond_to_diplomatic_message(sys::state& state, dcon::nation_id source, dcon::nation_id from, diplomatic_message::type type, bool accept)` con los parámetros apropiados. Si no se realiza una respuesta dentro de los días de `diplomatic_message::expiration_in_days`, será automáticamente rechazada, y la interfaz de usuario debe descartar automáticamente cualquier notificación visible o cuadro de mensajes después de ese punto, ya que cualquier intento de responder a solicitudes vencidas será ignorado.

Los mensajes de notificación contienen 5 piezas importantes de información. Contienen un `type`, una nación `primary`, que es la nación sobre la que trata principalmente el mensaje, y una nación `secondary` opcional, que es otra nación que causó que ocurriera la cosa (por ejemplo, en una notificación sobre una nueva guerra, la nación que declaró la guerra sería la nación secundaria, mientras que la nación primaria sería el objetivo de la guerra). Esos tres elementos deben usarse, junto con la configuración de notificación guardada del jugador, para determinar qué sucede cuando se recibe el mensaje (por ejemplo, ¿pausamos automáticamente el juego? ¿mostramos un mensaje emergente? ¿lo registramos en el registro?).

Si se va a mostrar el mensaje en una notificación emergente o se va a escribir en el registro, los miembros `title` y `body` contienen funciones que, cuando se llaman, llenarán un diseño con texto apropiado. **NOTA:** al publicar un mensaje desde el estado del juego (usando la función `notification::post`), debes asegurarte de dos cosas. Primero, que las lambdas pasadas a estos miembros capturen por valor cualquier información que necesiten. En segundo lugar, debes asegurarte de que cualquier manipulación de cadenas y/o formato se realice dentro del cuerpo de la lambda, y no en el proceso de creación. Como regla general, debes asegurarte de que no capture ningún objeto `std::string` o `std::string_view` en absoluto. Dado que los mensajes se crean dentro del bucle del juego, queremos asegurarnos de que enviar las notificaciones tenga un costo mínimo y que la mayor parte del costo de mostrar el mensaje se pague cuando se ejecuten las funciones `title` y `body`, ya que se ejecutarán en el hilo de la interfaz de usuario en lugar del hilo principal de actualización y, por lo tanto, no retrasarán el juego en sí mismo.