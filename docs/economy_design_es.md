# Diseño de la Economía

Este documento proporciona una descripción general del diseño de cómo se implementará la economía 1.0. La economía 1.0 no es una réplica exacta de la economía de Victoria 2. Está diseñada para ser una aproximación cercana, pero se han realizado algunas modificaciones para mejorar el rendimiento. Sustancialmente, esto significa que intentamos almacenar la menor cantidad de información posible y realizar la menor cantidad de pasadas sobre los datos. Desafortunadamente, incluso una aproximación cercana requiere una secuenciación sustancial, y hay poca posibilidad de paralelismo. Lo mejor que podamos lograr es hacer que la economía sea lo más autosuficiente posible para que la actualización del resto del día pueda ejecutarse junto a ella.

## Términos

Rentabilidad virtual: qué tan rentable sería una unidad estandarizada de producción *si* todos los insumos estuvieran disponibles, con los bonos de los trabajadores como si tuvieran empleo completo. (Teniendo en cuenta la fracción de producción vendida el día anterior en comparación con la producción real)

Escala de producción: la producción del productor se reducirá por dos factores: primero, por la disponibilidad de insumos. La escala efectiva de producción está limitada por la disponibilidad de insumos (y la escala real de producción se reducirá para cumplir con este valor). Cuando la oferta es mayor que la demanda real: la escala de producción disminuye a cierta tasa (por determinar, probablemente hasta cierta cantidad mínima, no hasta 0). Cuando la oferta es menor que la demanda real: la escala de producción aumenta a cierta tasa. Es posible que necesitemos un segundo factor de derivada aquí para reducir las fluctuaciones, ya que este es un segundo mecanismo de retroalimentación que empuja en la misma dirección que los movimientos de precios.

Cerrado: una fábrica con una escala de producción lo suficientemente baja está cerrada. Los artesanos con una escala de producción lo suficientemente baja cambiarán la producción, incluso si es nominalmente rentable (ya que esto significa que tienen problemas de disponibilidad de insumos).

## Actualización diaria

Iteramos sobre las naciones, en orden de rango.

### Estado inicial de la oferta nacional y global

El estado inicial de la oferta nacional se basa en lo que se produjo internamente, y la oferta global es lo que se produjo internamente el día anterior y no se consumió. Para el resto de este documento, tomamos la oferta nacional como "oferta nacional, existencias nacionales si la compra está habilitada, y lo que queda en la oferta nacional del líder de la esfera", que emula el comportamiento de Victoria 2.

Al comenzar el día, trasladamos la producción, de manera fraccional, al grupo de producción nacional del líder de la esfera, siguiendo la misma lógica que Victoria 2.

#### Datos requeridos:

Producción por nación (= pool de mercado nacional) para el día N - 1

### Calcular precios efectivos

El problema de los dos precios:

Para calcular la demanda y distribuir lo que está disponible de manera uniforme entre los consumidores de la nación, necesitamos saber cuáles son los precios para determinar cuánto es posible que un consumidor compre dada una renta particular (y cuál sería su renta suponiendo que podrían vender su producción). El problema es que los aranceles, etc., hacen que la compra desde el grupo global sea más cara. Esencialmente, hay dos precios, y la parte que puedes comprar localmente puede ser más barata (o más cara, en el caso de los subsidios) que el bien en el mercado global. Y esto significa que el costo efectivo depende de cuánto se compre en total, lo que a su vez depende del costo...

Victoria 2 resuelve esto ordenando a los consumidores de manera arbitraria. Hacen compras en orden, cada uno comprando de la oferta nacional (sí, incluso si es más barato comprar de la oferta global) hasta que se agota, y luego el resto debe lidiar con el precio de la oferta global. Esto no es ideal si esos precios divergen significativamente, porque significa que algunas cosas pueden ser o no rentables según donde caigan en este orden arbitrario.

Resolveremos este problema de la siguiente manera: usaremos la demanda real del *día anterior* para determinar cuánta de la compra se realizará desde las ofertas nacionales y globales (es decir, qué porcentaje se pudo hacer desde el grupo más barato). Usaremos eso para calcular un precio efectivo. Y luego, al final del día actual, veremos cuánto de esa compra provino realmente de cada grupo, etc. Dependiendo de la estabilidad de la simulación, en lugar de tomar el día anterior, podríamos construir este valor de manera iterativa como una combinación lineal del nuevo día y el día anterior.

(Si quisiéramos ser *más* realistas que Victoria 2, también podríamos calcular un segundo precio de venta... pero ¿por qué deberíamos hacerlo, si el juego base no se molesta en hacerlo?).

#### Datos requeridos:

Demanda real por nación para el día N - 1. Búfer temporal de precio efectivo por comodidad.

### Determinar empleo

Los posibles empleados se distribuyen a las fábricas haciendo lo siguiente:

Ordenamos las fábricas por prioridad y luego por rentabilidad *virtual* (bueno, como detalle de implementación, es más fácil hacer una ordenación estable por rentabilidad y luego una ordenación estable por prioridad, con estabilidad en ambos casos garantizando que el orden permanezca igual en todas partes). Ninguna fábrica contrata a más trabajadores de los que su escala de producción del día anterior permitiría. El empleo solo se permite cambiar tanto en un día (por determinar).

Esto también se aplica a los RGO, aunque solo hay un objetivo para la distribución.

#### Datos requeridos:

Escala de producción por fábrica, escala de producción de RGO por provincia y artesano. Valores de empleo por fábrica y por provincia de RGO.

### La nación determina el consumo objetivo y real

Se calcula el consumo deseado para todas las fuentes (poblaciones, fábricas, artesanos, existencias nacionales, proyectos) según cuánto pueden comprar dado su dinero. Se supone que los artesanos y las fábricas toman prestado contra su beneficio esperado si serán rentables dados los precios efectivos actuales (es decir, si son virtualmente rentables, con los efectos de empleo tenidos en cuenta, y las fábr

icas también deben tener en cuenta un costo de entrada de mano de obra mínima para el beneficio esperado). Si lo son, intentan comprar tanto como puedan dadas las limitaciones de su escala de producción y los números de empleo (y si no, su escala de producción disminuye). Las poblaciones intentan comprar tanto como puedan (hasta su último ingreso). Las naciones... aquí tenemos un problema. Es bastante natural decir: tanto como puedan dado sus reservas. Esto significaría que las naciones nunca entrarían en deuda. ¿Es eso lo que queremos? (Quizás para la IA y como opción del jugador). En cualquier caso, esto nos da números de "demanda real".

A partir de esto, derivamos límites máximos en la escala de producción para fábricas / artesanos (pero estos números no serán completamente precisos: los límites en la disponibilidad del bien A pueden hacer que más de B esté disponible porque el proceso de producción que utiliza tanto A como B consumirá menos de B, dejando más de B disponible para un proceso de producción no limitado por A. Hay un algoritmo que puede resolver esto, es una forma de problema de optimización lineal, que es solucionable, pero es algo complicado). También reducimos la escala de todas las demás compras de la misma manera.

Con la escala de consumo calculada, restamos las mercancías de los diversos grupos de oferta comenzando con el más barato de global o nacional. Dentro de lo nacional, primero restamos de lo verdaderamente nacional, luego del líder de la esfera nacional restante, luego de las existencias nacionales (si están habilitadas).

#### Datos requeridos:

Grupo de oferta global. Ingresos del día anterior por población. Búfer temporal de mercancías por comodidad para determinar disponibilidad. Existencias nacionales.

### Efectos del consumo

Las mercancías producidas (fábricas, artesanos, RGO) se colocan en el grupo de oferta nacional para el próximo día. Se calculan las ganancias de fábricas y artesanos, y las ganancias de los RGO, y a partir de estos valores se calculan los ingresos para diversas poblaciones de empleados para el próximo día. Las compras de la población se utilizan para calcular la satisfacción de necesidades, que no es un resultado directo de cuánto se consume, sino más bien una combinación lineal de la satisfacción anterior, que simula el ahorro de la población.

Cualquier oferta nacional restante se traslada al grupo global para el próximo día.

### Fin del día

La oferta real frente a la demanda real hace que los precios se ajusten. Cualquier oferta restante en el grupo global se elimina. Calculamos cuánto de la producción total mundial fue comprada, ya que esto afectará el precio de venta efectivo para el próximo día.

#### Datos requeridos:

Valores de producción global, valores de consumo real (o tal vez solo la fracción).