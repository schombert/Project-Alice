# Progreso hasta junio 2023

este me a sido inusualmente productivo, posiblemente por que mucho de los miembros del equipo tienen vacaciones de verano. pero antes de entrar a eso:

## Nuevos miembros del equipo

- BrickPi:es otro desarrollador que es part del proyecto "Open Vic 2". BrickPi nos a contribuido una mejora que permite a los desarrolladores en windows usar su copia local de Victoria 2, en vez de ponerla manualmente para comenzar.
- Breizh: mientras que nuevo al equipo, Breizh a estado en el discord desde hace un tiempo. A pesar de ser nuevo, a contribuido una gran cantidad de trabajo para terminar la interfaze de usuario.
- Ivysaur: Ivysaur tambien a estado en el discord desde el principio. Ivysaur se unio al equipo al final del mes, y no a tenido el tiempo para hacer algo diferente a tooltips.

## P&R del equipo

Este mes le preguntamos a Erik, uno de los cuatros fundadores del projecto y un gran contruibidor de [SDI](https://github.com/symphony-of-empires/symphony-of-empires): "Como hasido trbajar en Alicia comparado con SDI"

> Trabajar en Alicia a sido más facil, gracias a la experiencia de SDI y los objetivos más claros. Haver trbajado en un mapa antes fue de gran ayuda. El progreso en Alicia hasido rapido gracias al exelenta trabajo hecho por los contribuidores. otro factor es que en Alice se intenta prevenir el Síndrome del lavadero.
> 
> Debido a la demanda por una version de codigo abierto de victoria 2 (solo miren la cantidad de forks de Open V2) el interes en Alicia a incrementado. Tengo altas espectativas para la capacidad de modificaciones que va tener Alice cuando slaga la version 1.0. Lo que podemos modificar en victoria 2 es muy limitado, y quiero ver como podemos mejorarlo.

## progreso en la interface de usuario

Como indicamos en los meses pasado, el progreso en el globo a continuado. para hacer que el mapa de victoria 2 funcione en un globo emos añadido parches en los polos.No es una solución perfecta, pero funciona. estamos trbajando en una version completa del globo con la misma projección que en victoria 2 y esperamos sacar la version 1.0 con esto como una opción para partidas sin mods. (El mapa tradicional aun es una opción pero no eh sentido necesidad de usarlo.)

![Globo](globe.png)

Este mes nos centramos en la interface de usuario, por ejemplo, los eventos.

![Un evento](event.png)

En este evento se ven varias cosas interesantes. El texto azul se va a convertir en hypertextop que te va a llevar a la provincia, estado o nación nombrada. Puedes ver las lineas --aun en desarrollo-- que van a ayudar a organizar los eventos cuando tengas multiples de estos. El tool tipmuestra que los efectos de los archivos fueron procesados correctamente, y (con la exepción de unos errores) funcionan correctamente.

![Construcción de fabricas](factories.png)

Construciones de todos los tipos, desde fabricas, unidades militares, hasta edificios provinciales como los fuertes an sido implementados. Arriba puedes ver la construción de una fabrica de telas.

![ventana de construcción de fabricas](buildwindow.png)

Arriba pueden ver --la parcialmente incompleta-- ventana de construcción de fabricas. Inclui esto para mostrar que tan inutil es la descripción del papel que ah sobrevivido desde victoria 1. Aun me da riza.

![Barra superior](tech.png)

Esta pequeña imagen muestra una tecnologia siendo investigada, y los graficos de linea (hecha por forn).

![Liberal naciones](release.png)
![ireland](ireland.png)

ya puedes liberar naciones y jugar como ellas. Arriba puedes ver el cuadro de dialogo al liberar naciones (aun estamos trabajando en el color del texto) y a Irelanda despues de ser liberada.

![Ventana de diplomacia](diplomacy.png)

Aquí podemos ver qué es posible ganar influenia y mejorar sus relaciones con alguien. La mayoría de las acciones de influencia y las acciones diplomáticas básicas se han implementado. Sin embargo, sin una IA real, nuestra funcional temporal tiende a aceptar todas las peticiones de invasión y rechazar todas las peticiones de alianzas.

No existe suficiente espacio para incluir capturas de pantalla de todas las otras acciones que han Sido añadidas, pero he aquí la lista: cambio en el foco nacional, empezar investigacioned, construir edificios de provincia, mejorar relaciones, construir fábricas, construir barcos, armar regimientos, cancelar la instrucción de unidades, cambiar la prioridad de las fábricas, eliminar fábricas,  liberar nación, jugar como nación, dar y retirar subsidios de guerra, cambiar parámetros de presupuestos, iniciar una elección,cambiar la prioridad de influencia, desacreditar a asesores, expulsar asesores, prohibir embajadas, mejorar la opinión pública, empeorar la opinión pública, añadir a la esfera de influencia, remover de esfera de influencia ñ, convertir colonia en estado, invertir en una taza colonial, intervenir en una guerra, suprimir un movimiento, occidentalizar, poner un partido dominante, adoptar o repeler una reforma, unirse a una crisis, cambiar los parámetros de inventario, tomar una decisión, tomar una decisión frente a un evento, fabricar una cadus belli, solicitar acceso militar, pedir una alianza. 

## Internos

Un primer bosquejo de casi todo lo requerido para la actualización interna continua del juego, sin contar lo militar, ha Sido añadido. Esto incluye aspectos que van desde la migración de Pops, a elecciones, movimientos rebeldes, a eventos, a construcción de unidades y edificios.  Por supuesto, esto estando en desarrollo temprano, es probable que haya bastantes Bugs, pero es satisfactorio ver a la mayoría del juego comportándose como debería en términos generales. 

## Relaciones internacionales:

El proyecto Alice ha atraído algún en la comunidad China de Victoria 2. Dado principalmente a la barrera del lenguaje y a otros problemas logísticos, la mayoría de la comunidad no se encuentra presente en nuestro Discord. Para ayudar a reducir la brecha, trabajaremos con algunos miembros de la comunidad para proveer traducciones al Chino en nuestras actualizaciones mensuales y otros documentos clave. Si estás en contacto con algún miembro de alguna comunidad que no es de habla inglesa, estás interesado en el proyecto y estás dispuesto a hacer las traducciones necesarias, estaríamos felices de incluir otros lenguajes.

## Para el próximo mes

El mes siguiente se enfocará primariamente en la implementación de cosas asociadas al ejército y guerra, ambos en la interfaz de usuario y en las mecanicas internas del juego. Esto significa que cosas tales como mover unidades, decidir batallas, declarar guerras, y enviar ofertas de paz. Mi esperanza es llegar a una versión de prueba para el próximo mes, la cuál es un juego donde todo básicamente funciona pero la IA es totalmente pasiva.

## Pensando sobre la IA

Aunque hay una cantidad de trabajo importante que debe dirigirse al lado militar de las cosas, ninguna de ellas presenta un desafío técnico particular.  Lo mismo no puede decirse de la implementación de la IA. Planeó actualmente en afrontar el proceso de toma de decisiones de la IA como un conjunto de procesos independientes conectados por un entendimiento poco profundo de la situación subyacente, en el mejor de los casos.  Una inteligencia artificial de clase mundial tomaría todo en cuenta a la hora de tomar decisiones, pero pienso que aún podemos obtener un resultado aceptable al dividir su proceso de toma de decisiones. Y, más importantemente para lograr que se propone a tiempo, dividir la IA de esta manera facilita la división del trabajo  entre diferentes miembros del equipo, dado que la persona trabajando en como la IA escoge construir fábricas no debe preocuparse sobre como la IA escoge elaborar alianzas. 

Si estás interesado en trabajar en la IA y sabes C++, este sería un buen momento para unirse al equipo (por favor únete), ya que podrías usar el tiempo trabajando en el ejercito y la guerra para familiarizarte con el proyecto antes de movernos a la IA.

## El fin:

Los veremos el próximo mes! (O, si no puedes esperar tanto tiempo, unete a nosotros en [discord](https://discord.gg/QUJExr4mRn))
