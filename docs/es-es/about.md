## Acerca de Project Alice

### Para empezar
Por Favor consulte docs/contributing.md para más información! Contiene información de como compilar el proyecto en Windows y Linux

### Reanudando donde OpenV2 quedó

El objetivo del Proyecto Alice (nombrado a partir de [Alice Roosevelt Longworth](https://en.wikipedia.org/wiki/Alice_Roosevelt_Longworth)) es esencialmente para crear una nueva version de [open v2](https://github.com/schombert/Open-V2) -- my proyecto posterior a este para crear un clon de Victoria 2 -- y luego terminar con una version funcional y con completa. Esto significa que al menos, inicialmente, va a ver unas cuantas partituras de hacer cosas como originalmente se hacían, simplemente para mantener el proyecto enfocado. Cuando la 1.0 sea lanzada, entonces podremos tirar bola a la experimentación.

Ahora mismo hemos hecho un progreso significativo para reconstruir open v2, la mayor parte hecha desde cero, pero no nos hemos acercado al nivel de donde open v2 estaba. En cambio, el trabajo ha ido más rápido que lo hizo la primera vez, parcialmente por que la gente que ahora contribuye lo hace con sus propios talentos (en comparación de open v2 que era de una sola persona), y parcialmente por que no tendré que hacer los errores que cometí la primera vez.

Los programadores trabajando en el proyecto se dividen en UI, el Mapa, el Núcleo y "Freelancer" (posiblemente añadiendo al Launcher). UI y Mapas se enfocan, como su nombre implica, en la interfaz de usuario y el mapa, respectivamente. El núcleo solamente por ahora soy yo, y colecta programadores enfocados en implementar la lógica interna del juego. Finalmente, Freelancer es para todos los demás que han hecho contribuciones pero no se han decidido en un grupo en particular. ¿Cómo te puedes unir a uno de esos grupos? Solamente haz una PR que sea aceptada en el proyecto ([Contributing](contributing.md) para más detalles).

En adición a esto, Espero formar un grupo de Arte para recrear el arte utilizado en Victoria 2 para darle a este proyecto su identidad visual propia. Reconozco que esto es algo muy complicado de pedir, pero por una parte, no habrá modelado 3D. Tenemos acceso a un set de banderas nacionales que podemos usar, cortesía del proyecto SOE.

### Ética de trabajo

Personalmente, estoy en disposición a trabajar en este proyecto al menos 1 o 2 horas cada día de trabajo. Esto no se refleja necesariamente en el tiempo programado. Puede ser tiempo invertido en documentar o investigar, o solamente de idear. Pero para mí, pienso que mi disposición a hacer algo con persistencia es importante.

### En comparación a otros proyectos

En comparación a SOE (Symphony of Empires / Sinfonía de Imperios) este proyecto es un clon directo de Victoria 2, en cambio, SOE es su propio juego. Sin embargo, quiero pensar en ese proyecto como un proyecto hermano. Tenemos algunas personas aquí que se involucran con ambos (yo mismo haciendo sugerencias aquí), si usted ve algo que podamos hacer que puedan usar, estoy seguro que aprecian la contribución. Igualmente, estaremos utilizando una parte de su trabajo.

Con respecto al proyecto OpenVic2, aunque se presentó como un clon de Victoria 2, esto no es lo que están intentando crear. En vez están con la intención de proveer nuevos recursos audiovisuales, incluyendo eventos, decisiones, etc..., que harán su proyecto más parecido al "inspirado por Victoria 2". Además se corre con un estilo de manejo diferente. Si estas interesado/a en el proyecto Alice y OpenVic2, le sugiero que le ofrezca tiempo a ambos o incluso unirse a ellos.

### Licencia

Todo el código producido por este proyecto es lanzado utilizando la licencia GPL3 (como lo era con open v2). La licencia para los recursos visuales estará a disposición del grupo de arte, pero preferiría una forma de Creative Commons.

### Herramientas de programación

El proyecto en sí mismo utiliza CMake (reconozco que da tabarra). Idealmente debería de ser posible compilar con cualquiera de los tres famosos compiladores: MSVC, Clang/LLVM o GCC. Personalmente tiendo a usar MSVC, pero también uso Clang en Windows. Verificar que las cosas funcionen en GCC es problema de alguien más, y si ningún contribuidor utiliza GCC regularmente entonces este desaparecerá de nuestra lista de compatibilidad.

Personalmente utilizo Visual Studio 2022 (es gratis, la edición para la comunidad) para trabajar en el proyecto pero si usted está contribuyendo entonces es posible que puedas utilizar cualquier herramienta que quieras, VS Code, CLion, EMACS, VI, etc

### ¿En dónde diverge el proyecto Alice de Victoria 2?

Inicialmente el proyecto Alice imita a Victoria 2 en mecánicas muy cercanamente, donde sea razonable, con algunas excepciones. Reconocemos que hay muchas cosas que podemos mejorar. Realice open v2 con la mentalidad de que se podría mejorar las cosas como las hiciera, y esa fue la mayor razón que me desvie y termine abandonando. No quiero cometer el mismo error dos veces, así que este tiempo intentaré resistir hacer mejoras hasta que tengamos un juego completo y estable. Con eso dicho, hay algunas cosas que tenemos que realizar sin importar lo predicho. El mapa con forma de esfera es demasiado interesante como para no utilizarlo. Y hay algunas mejoras de calidad de vida que se pueden realizar (como los links que te envían a el evento del que se está hablando, o una paráfrasis de por qué se lanzó el evento) son demasiado buenas para dejarlas atrás. Pero sin ajustes mayores a las mecánicas esta vez! ¡Lo prometo!
