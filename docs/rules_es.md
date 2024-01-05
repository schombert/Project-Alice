# Reglas del juego

## Modificadores

Tanto las naciones como las provincias tienen un conjunto de propiedades (una colección de valores flotantes) que pueden ser afectadas por modificadores. Algunos modificadores vinculados a una provincia también pueden afectar a la colección de valores asociada a la nación propietaria. En general, a medida que se añaden o eliminan propiedades, añadimos o eliminamos su efecto en la colección de valores (intentamos no recalcular todo el conjunto tanto como sea posible). Esto también significa que cuando una provincia cambia de propietario, debemos recordar actualizar su efecto en el conjunto nacional de modificadores.

### Modificadores nacionales escalados

Algunos modificadores están escalados por cosas como la agotamiento de guerra, la alfabetización, etc. Dado que estos deben actualizarse diariamente, hay una segunda colección de valores vinculada a las naciones solo para estos.

- Ocupación total: escalada por la fracción de provincias hostiles, pero no rebeladas, en el continente de origen cuando está en guerra (sí, no es la verificación normal de ultramar; solo en el continente de origen).
- Infamia: escalada por infamia.
- Bloqueo total: escalada por la fracción de puertos bloqueados que no son de ultramar.
- Agotamiento de guerra: escalado por agotamiento de guerra (como un valor de 0 a 100).
- Pluralidad: escalada por pluralidad (como un valor de 0 a 100, así que 100% de pluralidad = 100 veces los valores del modificador).
- Alfabetización: escalada por la alfabetización promedio (como un valor de 0 a 1).

### Otros modificadores nacionales condicionales

- Valor nacional de la nación.
- Escuela tecnológica de la nación.
- Uno de: el modificador estático de gran potencia, el modificador estático de potencia secundaria, el modificador estático de nación civilizada o el modificador estático de nación incivilizada.
- Modificadores de cualquier problema activo de partido, problemas sociales y políticos (si es civilizado), reformas (si es incivilizado), tecnologías (si es civilizado) o inventos.
- Uno de: guerra o paz.
- Desarme, si está actualmente desarmado.
- Deuda: El modificador de incumplimiento de deuda se activa cuando la nación tiene el incumplimiento de deuda generalizado y/o la bancarrota como un modificador temporal/desencadenado y acreedores impagos... creo.
- Modificadores temporales o permanentes aplicados por evento.
- Modificadores desencadenados donde se cumple la condición desencadenante.

### Modificadores provinciales escalados

- El modificador para cada edificio de provincia, escalado por su nivel.
- El modificador de infraestructura, escalado por el valor de infraestructura de la provincia (cada nivel de ferrocarril otorga una cantidad fija de infraestructura).
- El modificador de nacionalismo, escalado por el valor de nacionalismo de la provincia, asumiendo que el propietario no tiene un núcleo allí (comienza en define:YEARS_OF_NATIONALISM y disminuye en 1/12 por mes).

### Modificadores condicionales de provincia

- Modificador de terreno.
- Modificador de clima.
- Modificador de continente.
- Un modificador de enfoque nacional.
- Modificador de crimen.
- El modificador de tierra (land_province) de la provincia si es tierra y el modificador de zona marítima (sea_zone) en caso contrario (que probablemente no usemos).
- El modificador costero si la provincia es costera y el modificador no costero (non_coastal) en caso contrario.
- También hay un modificador de mar costero (coastal_sea), que voy a ignorar.
- El modificador de ultramar si la provincia está en ultramar.
- El modificador de asedio (has_siege) si hay un asedio activo.
- El modificador de bloqueo si la provincia está bloqueada.
- El modificador de núcleo si el propietario de la provincia tiene un núcleo allí.
- El modificador sin provincias adyacentes controladas (no_adjacent_controlled) si no hay provincias adyacentes controladas (el juego base no utiliza esto, y lo voy a ignorar).
- Modificadores temporales o permanentes aplicados por eventos.

### Frecuencia de actualización

Los modificadores activados para las naciones parecen actualizarse el primer día de cada mes. El juego parece actualizar los valores que resultan de la aplicación o no de modificadores mensualmente (el primero del mes) para provincias y naciones.

Cuando se actualizan los modificadores, cualquier gasto por encima de los límites máximos/mínimos se limita adecuadamente.

## Economía

### Automatización del almacenamiento

El jugador puede automatizar la gestión de su almacén, y la IA siempre lo hace (uh, por definición). Para cada mercancía, primero calculamos el costo requerido para comprar unidades de tierra-suministro x gasto en unidades de tierra + unidades navales-suministro x gasto en unidades navales + proyectos de construcción x gasto en construcción de esa mercancía. Si el límite para el almacén se establece en menos que la cantidad requerida para cumplir con esas compras, el almacén se configura para comprar y el límite se eleva al valor necesario. Y eso es todo. Entonces, con la automatización, eventualmente, los límites se elevan al mayor valor que se haya visto alguna vez para el gasto nacional.

### Factor de participación

Si la nación es una civilización y es una potencia secundaria, comienza con define:SECOND_RANK_BASE_SHARE_FACTOR; de lo contrario, comienza con define:CIV_BASE_SHARE_FACTOR. También calcula la inversión extranjera del propietario de la esfera en la nación como una fracción de la inversión extranjera total en la nación (creo que se trata como cero si no hay inversión extranjera en absoluto). El factor de participación es (1 - factor de participación base) x fracción de inversión del propietario de la esfera + factor de participación base. Para las naciones no civilizadas, el factor de participación es simplemente igual a define:UNCIV_BASE_SHARE_FACTOR (por defecto, 1). Si una nación no está en una esfera, permitimos que el factor de participación sea 0 si es necesario usarlo en cualquier otro cálculo.

### Selección de producción artesanal

Durante la actualización mensual de su población (ver abajo), si un artesano no ha estado satisfaciendo sus necesidades vitales, puede cambiar a producir otra mercancía desbloqueada (esto es más probable cuanto más tiempo el grupo de población de artesanos no ha estado satisfaciendo sus necesidades vitales). No cambiará a un bien donde se cumplan las siguientes condiciones: no es rentable en su base (es decir, precios de entrada x cantidades de entrada - precio de salida x cantidades de salida es positivo, sin considerar modificadores), la demanda mundial total real es menor que la oferta mundial total real (Schombert señala: tal vez sea más simple verificar la dirección del movimiento de precios recientemente), o donde no haya oferta disponible de uno o más insumos. La elección de qué producir entre las mercancías no rechazadas por lo anterior parece ponderarse por la ganancia unitaria, la disponibilidad de insumos, cuánto excede la demanda mundial a la oferta y por la presencia de un enfoque nacional que fomente la producción.

### Construcción

#### Proyectos nacionales

(Nota: esto se aplica tanto a unidades militares como a proyectos de construcción, según creo)
- Un proyecto en construcción exige el 1/100 de su costo total en mercancías por día hasta que se haya consumido suficiente de cada mercancía.
- Un proyecto en construcción toma sus mercancías diarias demandadas del almacén nacional (hasta donde estén disponibles). Luego, pierde dinero basado en el precio de esas mercancías (aunque esta cantidad no puede dejarlo con dinero negativo, por lo que existe la posibilidad de mercancías gratuitas), y la nación gana dinero igual a la cantidad que el proyecto pierde.
- Ten en cuenta que gran parte de esto es una computación completamente desperdiciada si la propia nación ha financiado el proyecto: coloca dinero en el proyecto solo para devolvérselo a sí misma. En el caso de la inversión privada, sería más simple pagar directamente a la nación al principio. Al final, todo llegará al mismo resultado, y elimina una cantidad no trivial de contabilidad.
- La cantidad demandada por los proyectos se suma a la demanda diaria de bienes de la nación.

#### Proyectos populares (Pop projects)

Los proyectos populares son cosas como la construcción / mejora / reapertura de fábricas o la construcción de ferrocarriles.
Los proyectos populares son posibles solo según las reglas políticas y deben estar en construcción el tiempo suficiente para completarse.

Las fábricas tienen prioridad para ser construidas en estados con alta población, mientras que los ferrocarriles tienen prioridad en estados con muchas fábricas (que se determina por la suma de todos los niveles de fábrica). Esto hace que los proyectos se construyan en las áreas más importantes y luego se extiendan a las menos importantes a medida que pasa el tiempo.

Un proyecto popular compra mercancías como lo haría una fábrica con sus insumos (es decir, no se extrae directamente del almacén nacional).
Construir un nuevo edificio tiene un costo basado en mercancías de (tecnología-costo-del-dueño-de-la-fábrica + 1) x (modificador-de-costo-del-dueño-de-la-fábrica-nacional) x (el-precio-necesario-para-comprar-bienes-de-construcción) + 25 x costo-de-compra-de-un-día-de-insumos-en-el-nivel-1
Reabrir un edificio tiene un costo de: 25 x costo-de-compra-de-un-día-de-insumos-en-el-nivel-1
Mejorar un edificio tiene un costo de: (tecnología-costo-del-dueño-de-la-fábrica + 1) x (modificador-de-costo-del-dueño-de-la-fábrica-nacional) x (el-precio-necesario-para-comprar-bienes-de-construcción) + costo-del-edificio x 2-a-la-potencia-del-nivel-menos-2.

#### Ferrocarriles

Los ferrocarriles aumentan el potencial militar y económico de una provincia. El nivel máximo al comienzo del juego es 0, y se puede aumentar a un máximo de 6 mediante investigaciones en la subcategoría de infraestructura de las tecnologías industriales. Puede ser construido por capitalistas y mediante el mecanismo de inversión extranjera.

Puedes construir un ferrocarril en una provincia si (nivel_máximo_de_ferrocarril_de_la_nación - modificador_de_niveles_de_ferrocarril_del_terreno_de_la_provincia) > 0 y se cumplen los requisitos de costo.

No puedes construir ferrocarriles en territorios ocupados.

Un ferrocarril aumenta:

- El modificador de infraestructura.
- La velocidad de movimiento de las tropas (tanto enemigas como aliadas).
- La producción de RGO.
- La capacidad de producción de las fábricas.
- El límite de suministros.

### Penalización en el extranjero

Por cada mercancía descubierta por *alguien* y marcada como parte de la penalización en el extranjero (en `goods.txt`), una nación debe pagar define:PROVINCE_OVERSEAS_PENALTY x número-de-provincias-en-el-extranjero por día. La penalización en el extranjero para una nación es entonces 0.25 x (1 - el promedio de todas las mercancías descubiertas de (la-cantidad-en-el-almacén / cantidad-requerida)). Una nación sin provincias en el extranjero tiene una penalización de 0. (Una mercancía se descubre si está marcada como activa desde el principio *o* la nación ha desbloqueado la fábrica que la produce).

### "Banca" invisible

Sí, hay flujos de moneda que son en gran medida invisibles en el juego. Los llamo colectivamente banca o finanzas, pero en realidad eso es solo un nombre bonito.

Asociamos un "banco" con cada nación, y lo consideramos como teniendo una sucursal en cada estado dentro de la nación. Este banco tiene una reserva central de dinero y puede acumular intereses de préstamos (siempre positivos; nunca pierde dinero en préstamos). Si el banco tiene casi nada de dinero, estos intereses de préstamos se depositan directamente en sus reservas centrales. Si las reservas centrales del banco no están vacías, entonces el dinero se "distribuye" a las sucursales. ("Distribuido" porque, como verás, eso es solo una analogía aproximada de lo que sucede). Para distribuirlo en los estados, mantenemos un total acumulado de los intereses de préstamos a restar a medida que visitamos cada estado. Primero calculamos para un estado la fracción de la reserva de la sucursal con respecto a la reserva central del banco y multiplicamos el total de intereses de préstamos por esa fracción. Si esa cantidad es menor que lo que queda en el total acumulado, damos esa cantidad a la sucursal como sus intereses de préstamos y la restamos del total acumulado. Si es mayor que lo que queda en el total acumulado, sumamos lo que queda en el total acumulado a los intereses de préstamos de la sucursal. Luego no alteramos el total acumulado y pasamos al siguiente estado. Como puedes ver, este método va a depender mucho del orden en términos de lo que sucede, y aún así no tengo idea si hay alguna manera canónica en que se supone que los estados deben ordenarse. En cualquier caso, después de la distribución, anulamos los intereses de préstamos acreditados al banco central.

Cuando una población retira dinero para hacer una compra, la cantidad que se puede retirar está limitada al menor de los ahorros de la población (obviamente), la cantidad de ahorros almacenada en su sucursal estatal y el dinero en las reservas centrales del banco, menos la cantidad prestada. La cantidad se resta tanto de la sucursal local como del banco central, así como de la cuenta de la población. NOTA: la cuenta bancaria de una población es además de cualquier dinero que tenga en mano. Las poblaciones intentan mantener aproximadamente el doble de sus necesidades de gasto fuera del banco y como efectivo en mano. Sin embargo, si una población satisface todas sus necesidades de lujo (o casi todas), pondrá define:POP_SAVINGS x su efectivo en mano en el banco. (Añadiendo una cantidad idéntica a los registros de sucursal estatal y banco central).

Los bancos de las grandes potencias comienzan con (8 - rango) x 20 + 100 en ellos.

Cada día, una nación debe pagar a sus acreedores. Debe pagar national-modifier-to-loan-interest x monto-de-deuda x tasa-de-interés-al-titular-de-la-deuda / 30 al banco nacional que hizo el préstamo (posiblemente tomando más préstamos para hacerlo). Si una nación no puede pagar y la cantidad que debe es menor que define:SMALL_DEBT_LIMIT, la nación a la que debe dinero recibe un evento `on_debtor_default_small` (con la nación que incumple en el lugar correspondiente). De lo contrario, el evento se extrae de `on_debtor_default`. La nación entonces se declara en bancarrota. Recibe el modificador `bad_debter` durante define:BANKRUPCY_EXTERNAL_LOAN_YEARS años (si va a la bancarrota *nuevamente* dentro de este período, los acreedores reciben un evento `on_debtor_default_second`). Recibe el modificador `in_bankrupcy` durante define:BANKRUPCY_DURATION días. Su prestigio se reduce por un factor de define:BANKRUPCY_FACTOR, y la militancia de cada una de sus poblaciones aumenta en 2. Se cancelan todas las construcciones de unidades o edificios emprendidas por el estado, así como las fábricas en construcción (es decir, si se permite al estado destruir fábricas, qué limitación tan extraña). Se cancelan todas las subvenciones de guerra. También se ajustan todos los controles deslizantes de gasto y se desactivan las subvenciones a las fábricas, pero esto probablemente sea en beneficio de la IA.

Cuando una nación toma un préstamo, la tasa de interés al titular de la deuda se establece en nation-taking-the-loan-technology-loan-interest-modifier + define:LOAN_BASE_INTEREST, con un mínimo de 0.01.

### Monto máximo del préstamo

Un país no puede endeudarse si han pasado menos de los años definidos por: BANKRUPTCY_EXTERNAL_LOAN_YEARS desde su última bancarrota. Existe un límite de ingresos para determinar cuánto se puede pedir prestado, a saber: MAX_LOAN_CAP_FROM_BANKS x (modificador-nacional-para-el-monto-máximo-del-préstamo + 1) x base-imponible-nacional. También hay un límite basado en lo disponible en los bancos centrales de otras naciones, donde para cada nación que no esté actualmente en guerra con la nación prestataria y que no sea un acreedor impago, sumamos: (modificador-nacional-para-el-monto-máximo-del-préstamo + 1) x (dinero-en-reservas-de-ese-banco-central - dinero-que-ese-banco-central-ha-prestado) - cantidad-ya-prestada (y esto debe calcularse incluso para la propia nación, es decir, al pedir prestado a su propio banco central), y luego definimos: SHADOWY_FINANCIERS_MAX_LOAN_AMOUNT. El valor más bajo entre estos dos es la cantidad máxima que un país puede pedir prestado.

### Ajuste de la fuerza laboral

Este es un concepto común entre fábricas y RGOs, que tienen empleados. Al ajustar el número de trabajadores empleados, tenemos tanto un número objetivo de empleados hacia el cual se realiza el ajuste como un factor de velocidad que determina qué tan rápido se realiza el ajuste. Llamemos delta al cambio en el empleo necesario para alcanzar nuestro objetivo. Al despedir personas, despedimos hasta el mayor de 50 o delta x EMPLOYMENT_FIRE_LOWEST, y luego limitamos eso a un máximo de delta x factor-de-velocidad x 0.25. Al contratar personas, la lógica parece ser completamente diferente. Las personas son contratadas hacia el objetivo a una velocidad de delta x factor-de-velocidad, o EMPLOYMENT_HIRE_LOWEST x número-actualmente-empleados, o 50, lo que sea mayor.

### Internos de la fábrica

Todos los días, a nivel estatal, debemos ajustar el estado de cada fábrica, yendo de la prioridad más alta a la más baja (el orden exacto dentro de las fábricas con la misma prioridad parece ser constante pero de lo contrario es opaco, ¿en la interfaz gráfica?). Realizamos su actualización diaria de la siguiente manera:
- Definimos la condición de eliminar-la-fábrica como verdadera si la regla de eliminar-fábricas-sin-entrada está activa y la nación está controlada por el jugador O si la nación está controlada por la IA y está en paz, y falso en caso contrario.
- Si la regla de eliminar-fábricas-sin-entrada está en efecto, cualquier fábrica que emplee a más de 1000 personas tiene su prioridad establecida en 0 (y en 1 si hay menos de 1000 personas empleadas).
- Debemos determinar si ha obtenido beneficios, lo cual depende de si su último ingreso + su última inversión + su última subvención por déficit es mayor que la cantidad que pagó en salarios + su gasto en insumos. Si es rentable, se marca como que ha tenido un día adicional rentable (creo que este registro tiene una duración de 8 días), y se registra la ganancia o pérdida neta.
- Las fábricas que reciben una inyección de efectivo durante N días reciben 1/N de la cantidad cada día en sus reservas locales de efectivo. Esto no afecta si se consideran rentables o no.
- Las fábricas que han pasado más de 10 días sin insumos o que son cerradas por el jugador se consideran cerradas.
- Las fábricas abiertas tienen su empleo ajustado. Una fábrica que está siendo subsidiada, o tiene gastos inferiores a sus ingresos, o tiene suficientes reservas de efectivo (¿100?) se considera financieramente estable. El empleo máximo efectivo en una fábrica es su nivel x su fuerza laboral base x la fracción del estado que no está ocupada (es decir, fracción-de-estado-controlada x nivel nos da su nivel efectivo). Si la fábrica es financieramente estable, ajustamos su fuerza laboral hacia el empleo máximo efectivo con una velocidad de 0.15, de lo contrario la ajustamos hacia cero con una velocidad de 0.15.
- Tenga en cuenta que las fábricas con mayor prioridad, porque van primero, tienen la primera oportunidad de contratar/despedir trabajadores de la fábrica, lo que significa que terminan con más trabajadores.
- Tenga en cuenta también que las fábricas que están actualmente en proceso de actualización ignoran los días sin insumos/otras reglas de cierre.
- Las fábricas recién construidas (actualizándose desde 0) parecen tener alguna lógica especial de empleo para llegar a 1000. ... Miren, solo puedo entender tantos detalles.
- Eliminación automática de fábricas: si se cumple la condición-de-eliminar-la-fábrica Y la fábrica ha pasado más de 10 días sin insumos Y no fue cerrada por el jugador Y hay al menos MIN_NUM_FACTORIES_PER_STATE_BEFORE_DELETING_LASSIEZ_FAIRE fábricas en el estado Y ninguna provincia en el estado está ocupada Y (hay al menos NUM_CLOSED_FACTORIES_PER_STATE_LASSIEZ_FAIRE fábricas cerradas O hay 8 fábricas en el estado) ENTONCES se eliminará la fábrica.
- Cada fábrica tiene multiplicadores de entrada, salida y rendimiento.
- Estos se calculan a partir de los empleados presentes. La entrada y salida son 1 + efectos de empleados, el rendimiento comienza en 0.
- El multiplicador de entrada también se multiplica por (1 + suma-de-cualquier-modificador-activado-para-la-fábrica) x 0v(impacto-de-la-movilización-nacional - (penalización-en-el-extranjero si está en el extranjero, o 0 en caso contrario)).
- La fracción de propietarios se calcula a partir de la fracción de propietarios en el estado con respecto a la población total del estado (con algún límite, ¿5%?).
- Para cada tipo de población empleada, calculamos la proporción de número-de-población-empleada-de-un-tipo / (fuer

za-laboral-base x nivel) con respecto a la fracción óptima definida para el tipo de producción (limitándola a 1). Esa proporción x la-cantidad-de-efecto-de-empleado se agrega al multiplicador de entrada/salida/rendimiento para la fábrica.
- Luego, para entrada/salida/rendimiento, sumamos los modificadores nacionales y provinciales a la entrada/salida/rendimiento general de la fábrica, se suman los modificadores tecnológicos a su producto de salida específico, se agrega uno a la suma y luego se multiplica el multiplicador de entrada/salida/rendimiento del personal por ello.
- Las fábricas también contienen un factor de compra que limita cuánto de sus insumos compran, que también se actualiza diariamente.
- Si la cantidad no vendida de la fábrica / producción actual de la fábrica es menor que FACTORY_PURCHASE_DRAWDOWN_FACTOR, entonces el factor de compra de la fábrica se incrementa en FACTORY_PURCHASE_DRAWDOWN_FACTOR.
- De lo contrario, se reduce su factor de compra en FACTORY_PURCHASE_DRAWDOWN_FACTOR, con un mínimo de FACTORY_PURCHASE_MIN_FACTOR.

Modificador de costo de la fábrica: (modificador-de-costo-de-fábrica-tecnología + 1) x modificador-de-costo-de-fábrica-nacional
(para RR / fuertes / bases navales construidas por la nación: (modificador-de-costo-de-fábrica-tecnología + 1) x (2 - eficiencia-administrativa-nacional))
(para RR construidas por las poblaciones: modificador-de-costo-de-fábrica-nacional)

### Internos de los artesanos

- Cada población de artesanos tiene su propia velocidad de producción que puede aumentar o disminuir según el estado de la población de artesanos.
- Si la población de artesanos está obteniendo sus necesidades diarias en más del 0.85 o todas sus necesidades diarias se consideran asequibles: la tasa de producción se multiplica por 1.15, hasta un máximo de 1.
- Si la población de artesanos está obteniendo sus necesidades diarias en menos del 0.85 y todas sus necesidades vitales son asequibles o sus necesidades vitales están satisfechas en al menos un 0.9: la tasa de producción no se ajusta.
- Si la población de artesanos está obteniendo sus necesidades diarias en menos del 0.85, sus necesidades vitales no son todas asequibles y su satisfacción de necesidades vitales es inferior al 0.9: la tasa de producción se multiplica por 0.85, hasta un mínimo de 0.15.
- Al igual que las fábricas, el gasto de los artesanos también está regulado en parte por un límite de compra. Este límite puede aumentar en 1.05 veces por día, limitado entre la mitad de su último ingreso y su último ingreso total (creo que este ingreso no tiene en cuenta sus gastos, sin embargo...).

### Internos de RGO

Cada RGO tiene un tamaño efectivo = tamaño base x (bonificación tecnológica al tamaño específico del buen tamaño RGO + bonificación tecnológica general al tamaño de granja o mina + modificador de tamaño de mina o granja provincial + 1).
Al igual que una fábrica, el RGO emplea a poblaciones. Si la provincia está actualmente ocupada, sus trabajadores se ajustan hacia 0, al igual que una fábrica realiza dicho ajuste. Si la provincia acaba de ser restaurada al control del propietario, parece haber alguna lógica especial que impulsa el empleo hacia arriba (algo así como: la velocidad de aumento de la fuerza laboral es 25 veces más rápida).
Si la proporción de suministro a la demanda real del bien producido por el RGO es mayor que 1.05: la fuerza laboral se ajusta hacia cero a una velocidad de (proporción de suministro a demanda - 1) x define: RGO_SUPPLY_DEMAND_FACTOR_FIRE.
Si la proporción de suministro a la demanda real es inferior a 1.1 pero mayor que 1.0: la fuerza laboral permanece igual.
Si la proporción de suministro a la demanda real es inferior a 1.0 pero mayor que 0.9: la fuerza laboral se ajusta hacia el pleno empleo a una velocidad de (1 - proporción de suministro a demanda) x define: RGO_SUPPLY_DEMAND_FACTOR_HIRE_LO.
Si la proporción de suministro a la demanda real es inferior a 0.9: la fuerza laboral se ajusta hacia el pleno empleo a una velocidad de (1 - proporción de suministro a demanda) x define: RGO_SUPPLY_DEMAND_FACTOR_HIRE_HI.
El RGO de dinero siempre tiene como objetivo el pleno empleo.

### Precios, producción, compra y venta

El material de esta sección es a menudo lo que la gente considera como la economía de Victoria 2, así que prepárate para una explicación algo complicada.

Al comienzo de la actualización, calculamos valores generados el día anterior: (cambio-en-dinero-en-circulación + valor-de-dinero-producido) / (dinero-en-circulación + valor-de-dinero-producido) + ~0.8 (aproximadamente un poco menos que esto). Y luego este valor se establece en 1.0 si está fuera del rango de 0.0 a 1.0. Llamemos a esto la proporción-de-circulación.

1. Actualizar precios basados en valores del día anterior (todo lo siguiente se refiere a la demanda y oferta global):
- El precio del dinero permanece según el precio definido en el archivo de productos básicos.
- Para cada otro producto, verifica su demanda real (ver más abajo para lo que es la demanda real) y, si su demanda real es positiva, verifica su oferta. Si su oferta también es positiva, tomamos la raíz cuadrada de la demanda-real / oferta y multiplicamos ese resultado por el precio base del producto. Llamemos a esto el precio-ajustado-por-proporción.
- Si el precio-ajustado-por-proporción es menor que el precio actual - 0.01, entonces el precio disminuye en 0.01.
- Si el precio-ajustado-por-proporción es mayor que el precio actual + 0.01, entonces el precio aumenta en 0.01.
- Los precios luego se limitan a estar dentro de 1/5 al 5 veces el precio base del producto.

2. Oferta y demanda no ajustadas:
Mientras que la mayoría de lo que sucede a continuación se expresa directamente en términos de agregar a la oferta o demanda doméstica, a menos que se indique lo contrario, se debe considerar que lo mismo se agrega a la oferta o demanda mundial. Además, la oferta y demanda mundial para todos los productos deben tratarse como al menos 1.

A.

- Para cada producto:
- Si la reserva nacional no está comprando (es decir, está vendiendo)
- Si la nación no está configurada para comprar de su propia reserva
- Agrega la cantidad en la reserva por encima del límite a la oferta doméstica, reduce la reserva nacional al valor límite
- Si la nación está configurada para comprar de su propia reserva, entonces no la ajusta en absoluto en este punto
- Si la reserva nacional está comprando, agrega la diferencia entre la reserva real y el límite a la demanda doméstica
- Agrega las reservas nacionales a la oferta doméstica si la nación está configurada para comprar de sus propias reservas.

B.

- Para cada fábrica:
- Solo las fábricas no cerradas (ver arriba) que tienen al menos nivel 1 (es decir, no están en su construcción inicial) producen/consumen.
- Cada fábrica produce consumiendo insumos de su reserva interna.
- La escala objetivo de consumo de insumos es: multiplicador-de-insumo x multiplicador-de-producción x nivel de fábrica.
- La escala real de consumo está limitada por los productos básicos de entrada en la reserva (es decir, la escala-de-consumo-de-insumos x cantidad-de-insumo debe ser menor que la cantidad en la reserva).
- Se realiza un proceso similar para los insumos de eficiencia, excepto que el consumo de insumos de eficiencia es (modificador-de-mantenimiento-de-fábrica-nacional + 1) x multiplicador-de-insumo x multiplicador-de-producción x nivel de fábrica.
- Finalmente, obtenemos la escala-de-consumo-ajustada-por-eficiencia multiplicando la escala-base-de-consumo por (0.75 + 0.25 x la escala-de-consumo-de-eficiencia).
- (Una fábrica sin productos de eficiencia funciona como si todos sus productos de eficiencia fueran completamente consumibles)
- Una fábrica *produce* escala-de-consumo-ajustada-por-eficiencia x multiplicador-de-producción x multiplicador-de-producción x nivel x cantidad-de-producción de su bien de salida, que se agrega a la oferta doméstica.
- Usando el mismo cálculo de consumo, la fábrica agrega la diferencia entre lo que le queda en su reserva a lo que requeriría para producir a su escala máxima de consumo a la demanda doméstica.

C.

- Para cada expansión o proyecto de construcción:
- Multiplica el modificador de costo (ver arriba) por el costo en productos básicos del proyecto. Luego resta la reserva del proyecto de lo que se requiere. Calcula la cantidad máxima que el proyecto puede comprar según su dinero. Luego, agrega esos productos a la demanda doméstica.

D.

- Para cada población artesana:
- Calculamos tamaño-de-población x tasa-de-producción-artesanal (ver arriba) / fuerza-laboral-de-producción-artesanal. Luego hacemos el paso de producción básicamente como lo hacemos para las fábricas, excepto que usamos el valor precedente como el "nivel" de la fábrica.

E.

- Para cada RGO:
- Calculamos su tamaño efectivo que es su tamaño base x (bono-de-tecnología-al-tamaño-específico-de-bien-RGO + bono-de-tamaño-de-granja-o-mina-de-tecnología-general + modificador-de-tamaño-de-mina-o-granja-provincial + 1)
- Agregamos su producción a la oferta doméstica, calculando esa cantidad básicamente de la misma manera que lo hacemos para las fábricas, mediante el cálculo de la producción-de-RGO x rendimiento-de-RGO x tamaño-de-RGO x cantidad-base-de-producción-de-producto, excepto que está afectado por diferentes modificadores.

F.

- Para cada población:
- Cada estrato y tipo de necesidad de población tiene su propio modificador de demanda, calculado de la siguiente manera:
- (modificador-nacional-a-la-demanda-de-bienes + definir:BASE_GOODS_DEMAND) x (modificador-nacional-a-un-tipo-específico-de-estrato-y-tipo-de-necesidad + 1) x (definir:INVENTION_IMPACT_ON_DEMAND x número-de-inventos-desbloqueados + 1, pero solo para necesidades-no-vitales).
- Cada demanda de necesidades también se multiplica por 2 - la eficiencia administrativa de la nación si la población tiene ingresos de educación/administración/militares para esa categoría de necesidad.
- Calculamos un tamaño-de-población ajustado como (0.5 + conciencia-de-la-población / definir:PDEF_BASE_CON) x (para poblaciones no coloniales: 1 + pluralidad-nacional (como fracción de 100)) x tamaño-de-población.
- Cada una de las necesidades de la población para vida/cotidiano/lujos se multiplica por el modificador apropiado y se agrega a la demanda doméstica.

3. Ajuste de la oferta/demanda de la esfera
- Cada miembro de la esfera tiene su oferta y demanda base doméstica multiplicada por su factor de participación (ver arriba) y se suma a la oferta y demanda doméstica del líder de la esfera (esto no afecta la oferta y demanda global).
- Cada nación en una esfera (después de que lo anterior se ha calculado para toda la esfera) tiene su oferta doméstica efectiva establecida en (1 - su factor de participación) x oferta doméstica original + oferta doméstica del líder de la esfera.

4. Compras
Conceptos comunes de compras:
- Siempre que se realicen compras, el comprador intenta primero adquirir tanto como sea posible de la oferta doméstica y luego intenta comprar cualquier resto de la oferta global (¿de dónde proviene la oferta global? sigue leyendo).
- Si tomas la cantidad disponible que podría comprarse asumiendo oferta ilimitada, dada la cantidad de dinero que el comprador está gastando, obtienes una cantidad de productos que luego se suma a la *demanda real* de esos productos.
- Lo que realmente se compra está limitado primero por la disponibilidad en la oferta doméstica y luego por la disponibilidad en la oferta global, excepto que al comprar de la oferta global, los precios se multiplican por (la tarifa actual de la nación x su eficiencia arancelaria x su fracción bloqueada + 1).
- El dinero gastado en compras de la oferta global eventualmente se convierte en la base para los ingresos arancelarios. (Así que los bloqueos hacen que las cosas cuesten más, pero también pueden aumentar tus ingresos...)
- Ten en cuenta que el dinero utilizado para realizar compras no "va" a ninguna parte, simplemente desaparece. Consulta la sección de Venta a continuación para saber cómo ganan dinero los productores.

Realizamos lo siguiente para cada nación en orden de su rango (la clasificación 1 va primero):

Para cada fábrica abierta:
- Calcula los insumos totales que se requerirían para una producción completa dados los trabajadores actuales, modificadores, etc., *además* de lo que queda en su reserva.
- Luego reduce estos insumos multiplicándolos por el factor de compra de la fábrica.
- Estos productos son los que intentará comprar la fábrica.
- Se permite a una fábrica gastar hasta (reservas de efectivo actuales ^ último gasto x 1.05) v 0.1 x reservas de efectivo actuales.
- Si su límite de gasto es menor que el costo base de sus insumos, las compras de la fábrica se limitan proporcionalmente aún más.
- Si la fábrica no puede comprar nada, una vez que se han calculado los precios y no está siendo subsidiada, se marcará como que ha pasado un día sin insumos, lo que eventualmente resultará en su cierre.
- Los productos comprados se añaden a la reserva de la fábrica.
Para cada proyecto de construcción:
- El proyecto compra tanto como pueda basado en sus reservas de efectivo actuales.
- Los productos van a su reserva interna.
Para la reserva nacional:
- Los productos para los cuales la reserva está configurada para comprar y no hay suficientes en la reserva funcionan como compras normales, excepto que el país automáticamente toma prestado hasta su límite máximo para realizar la compra. Sin embargo, hay una gran advertencia: las naciones no parecen pagar extra por los productos de la oferta global (ni siquiera por los efectos del bloqueo), y naturalmente, sus compras de la reserva no se suman a las sumas arancelarias.
Para las poblaciones (pops):
- Los pops compran sus necesidades en orden de su estrato (los ricos primero), y luego dentro de eso... no conozco los detalles.
- Los artesanos compran insumos (retiran dinero de su banco para hacer compras en lugar de usar su dinero en efectivo).
- Los pops compran sus necesidades primero con su dinero en efectivo y luego retirando del banco si es necesario.
- Los pops de un tipo dado parecen comprar sus necesidades más o menos "juntos" (es decir, si la satisfacción de las necesidades está limitada por la disponibilidad de productos y no por el precio, los pops parecen obtener una parte más o menos equitativa de ello).
- Los pops obtienen un cambio en la constante:LUXURY_CON_CHANGE cada vez que gastan lo suficiente en bienes de lujo.

Ten en cuenta que una nación que esté en la esfera de otra puede comprar cualquier producto no vendido en la reserva doméstica del líder de la esfera después de agotar su propia reserva doméstica (y no se aplican aranceles, etc., en este caso).

Una vez completadas todas las compras, los productos que se trasladaron a la reserva doméstica para vender como parte de la configuración de compra desde la reserva del país se devuelven a las reservas nacionales si de hecho no fueron comprados.

El caso más simple son las naciones que no están en una esfera ni forman parte de una esfera:
Si la nación tiene habilitada la compra desde la reserva, cualquier producto "no vendido" que se haya trasladado de la reserva a la oferta doméstica se devuelve a la reserva. En otras palabras, la compra desde la reserva actúa como otra fuente de suministro de la cual se compran productos después de agotar la oferta doméstica normal.

El problema, por supuesto, es que la oferta doméstica de los miembros de la esfera se mezcla. ¿Cómo puedes hacer un seguimiento de cuánto de lo que se ha dumping en el mercado para vender realmente se ha vendido? Aquí está lo que probablemente debería suceder: calcularíamos el porcentaje de productos de la reserva que se colocaron en la oferta doméstica que se vendieron y se devolvería a cada miembro de la esfera (y al líder de la esfera) una cantidad proporcional a lo que puso. Sin embargo, parece que algo sale mal en el juego real aquí, y parece que algunos o todos estos productos se están agotando de las reservas del líder de la esfera (??). Puede ser necesario realizar más investigaciones para determinar la naturaleza exacta de este error.

Cualquier oferta doméstica restante e inconsumida se agrega a la oferta global.

5. Ingresos

Conceptos comunes de ingresos:
Recordatorio, el suministro doméstico efectivo se define anteriormente en el paso 3.
Suministro doméstico consumido: incluso cuando en una esfera, una nación se trata como si hubiera comprado primero su producción doméstica.
Cuando se pagan los "pops": al pagar a un "pop", debemos calcular la eficiencia fiscal de la nación, que es: (eficiencia-fiscal-de-la-nación + eficiencia-fiscal-de-la-tecnología + definir: BASE_COUNTRY_TAX_EFFICIENCY). La cantidad de ingresos del "pop" multiplicada por el valor de la eficiencia fiscal se suma a la base fiscal no modificada de la nación. Multiplicar esto por la configuración fiscal para la estratificación determina la cantidad de impuestos que la nación recauda.
Los "pops" del tipo esclavo no se cuentan como trabajadores al pagar a los "pops".

Para las fábricas:
Para cada fábrica, calculamos la proporción del suministro doméstico efectivo que contribuyó su producción.
También calculamos la proporción del suministro global del día anterior que su producción-no-comprada-domésticamente del día anterior contribuyó (obviamente, ese valor debe calcularse luego para el próximo día, etc.). Luego, a la fábrica se le otorga ingresos iguales a la cantidad de su producción que se vendió nacionalmente, así como por cualquier producción que se vendió del suministro global del día anterior (como si todas las compras se distribuyeran proporcionalmente a la cantidad de producción). Este valor se convierte en el ingreso de la fábrica (ver los detalles de cómo funcionan las fábricas para entender por qué esto puede ser importante), y podemos compararlo con los gastos del día para ver si la fábrica fue rentable.

Cada fábrica tiene gastos base equivalentes al dinero necesario para pagar por sus insumos al día siguiente más 1/5 de los costos de las necesidades básicas de sus empleados. Si está subsidiada y tiene menos que esa cantidad de dinero, la nación propietaria paga la diferencia en subsidios.

Si hay suficiente dinero en las reservas de efectivo para cubrir al menos una semana de insumos, la fábrica paga un bono. El bono es (ganancia-de-la-fábrica = ingresos - costos-de-insumos) x definir: FACTORY_PAYCHECKS_LEFTOVER_FACTOR x reservas-de-efectivo-de-la-fábrica / (definir: MAX_FACTORY_MONEY_SAVE x nivel-de-la-fábrica). Si este bono es mayor que el valor del salario mínimo (costos-de-necesidades-básicas-de-sus-empleados x modificador-de-salario-mínimo-nacional x eficiencia-administrativa-nacional), entonces, los propietarios, si los hay, reciben la cantidad del bono que excede el salario mínimo, y los trabajadores obtienen el salario mínimo. De lo contrario, los propietarios obtienen la mitad del bono y los trabajadores obtienen la otra mitad.

Ventas de reservas:
Cada nación gana dinero equivalente a la cantidad de productos vendidos de las reservas multiplicada por el precio del producto (esto se aplica tanto a las ventas simples de "volcar en el mercado" como a permitir que las entidades dentro de la nación compren desde las reservas).

Ingresos directos de oro:
Una nación gana definir: GOLD_TO_CASH_RATE x la cantidad de oro producida de cada RGO de dinero.

Reparaciones:
Una nación que paga reparaciones pierde definir: REPARATIONS_TAX_HIT x su base imponible en dinero por día, mientras que el país que las recibe recibe esa cantidad de dinero.

Subsidios de guerra:
Sí, se puede obtener dinero de los subsidios de guerra.

Aranceles:
Cada nación gana (o pierde, en el caso de subsidios) dinero equivalente a su configuración de aranceles x eficiencia de aranceles x cantidad total de compras del suministro global (ver arriba). Dado cómo se calculan estos valores, esto suele significar que el dinero aparece de la nada o desaparece en la nada.

RGOs:
El RGO de dinero calcula sus ingresos como si toda su producción se vendiera al precio actual (que está fijo para "dinero/oro") y luego se multiplica por definir: GOLD_TO_WORKER_PAY_RATE. De lo contrario, los ingresos del RGO se calculan como los ingresos de una fábrica. Dado que los RGO no tienen que mantener una reserva, creo que pagan salarios primero hasta el valor del salario mínimo a los trabajadores normales, y luego pagan el resto a los aristócratas (si los hay) (alguien debería verificar esto).

Artistas:
La ganancia se calcula como en una fábrica. Cualquier ganancia (es decir, ingresos por encima de lo necesario para comprar insumos) va a parar a los bolsillos del artesano, lo que significa que se gravan como ingresos normales de "pop".

Otros ingresos de "pop":
Los "pops" pueden recibir ingresos directamente de la nación según su tipo de ingresos/políticas nacionales. Para cada categoría de necesidades, el pago se basa en cuánto costaría satisfacer completamente esas necesidades y el tipo de pago. Los pagos de este tipo parecen estar gravados, de manera confusa.

Los "pops" que no tienen cubiertas sus categorías de necesidades por el gasto directo del estado parecen recibir: gasto-social x costo-de-necesidades-de-vida x nivel-de-pensión x eficiencia-administrativa + gasto-social x costo-de-necesidades-de-vida x fracción-de-desempleo x eficiencia-administrativa por día.

Y para cada categoría de necesidades donde el "pop" es pagado por educación/administración/militar:
La nación paga al "pop" gasto-administrativo/nacional-de-educación/gasto-militar x (2 - eficiencia-administrativa) x costo-de-necesidades.

## Población y demografía

### Actualizaciones mensuales para poblaciones individuales

Las actualizaciones descritas en esta sección se ejecutan una vez al mes por cada población (o "pop"), distribuidas de manera uniforme a lo largo del mes. En lugar de tratar de determinar el número de días en el mes y dividir las poblaciones entre esos días, probablemente deberíamos elegir un intervalo fijo. También **no** queremos dividir las actualizaciones para las poblaciones tomando sus N bits más bajos para agruparlas. Por razones de rendimiento, es importante actualizar las poblaciones en grupos de identificadores contiguos.

Esta actualización mensual incluye:
- Determinar a qué movimiento político o social pertenece la población.
- Determinar a qué facción rebelde pertenece la población.
- Determinar si un artesano debe cambiar su producción y a qué.
- Hacer crecer o decrecer la población.
- Si la cantidad de personas que deben mudarse para la población es mayor que el tamaño mínimo de promoción de su tipo o el tamaño de la población en su totalidad, la población puede migrar parcialmente, ya sea interna, externa o colonialmente. (Nota: los esclavos no se mueven ni migran).
- Lo mismo que lo anterior, pero para la promoción y degradación por tipo de población.
- Conversión religiosa: el número convertido = definir: CONVERSION_SCALE x tamaño de la población x factor de probabilidad de conversión (el factor se calcula de manera aditiva). Parece haber alguna lógica extraña en torno a las poblaciones con religión marcada como "pagana" o que se convierten a dicha religión, donde estas poblaciones no se convertirán a menos que ya haya una población coincidente con la misma cultura y tipo, y con la religión a la que se va a convertir en la provincia. No está claro si debemos intentar replicar esta lógica. Además, los esclavos no se convierten.
- Asimilación cultural: Limitaciones: los esclavos no asimilan, las poblaciones de una cultura aceptada no asimilan, las poblaciones en una provincia de ultramar y colonial no asimilan. Para que una población asimile, debe haber una población del mismo estrato de cultura primaria (preferiblemente) o cultura aceptada en la provincia para asimilar. (Notas de Schombert: no estoy seguro de si vale la pena conservar esta limitación). Cantidad: definir: SSIMILATION_SCALE x (modificador de tasa de asimilación provincial + 1) x (modificador de tasa de asimilación nacional + 1) x tamaño de la población x factor de probabilidad de asimilación (calculado de manera aditiva, y siempre al menos 0.01). Si el tamaño de la población es inferior a 100 aproximadamente, parece que se asimilan por completo si hay alguna asimilación. En una provincia colonial, los números de asimilación para las poblaciones con un grupo cultural de tipo *no* "ultramarino" se reducen por un factor de 100. En una provincia no colonial, los números de asimilación para las poblaciones con un grupo cultural de tipo *no* "ultramarino" se reducen por un factor de 10. Todos los números de asimilación de las poblaciones se reducen por un factor de 100 por cada núcleo en la provincia que comparte su cultura primaria.

### Crecimiento

Factor de crecimiento de la población de la provincia: Solo las provincias en propiedad experimentan crecimiento. Para calcular el crecimiento de la población en una provincia: Primero, calcula la calificación de vida modificada de la provincia. Esto se hace tomando la calificación de vida intrínseca y multiplicándola por (1 + el modificador provincial para la calificación de vida). La calificación de vida modificada está limitada a 40. Toma ese valor, si es mayor que definir: MIN_LIFE_RATING_FOR_GROWTH, réstale definir: MIN_LIFE_RATING_FOR_GROWTH y luego multiplícalo por definir: LIFE_RATING_GROWTH_BONUS. Si es menor que definir: MIN_LIFE_RATING_FOR_GROWTH, trátalo como cero. Ahora, toma ese valor y agrégalo a definir: BASE_POPGROWTH. Esto nos da el factor de crecimiento para la provincia.

La cantidad en que crece una población se determina primero calculando la suma del modificador de crecimiento: (necesidades de vida de la población - definir: LIFE_NEED_STARVATION_LIMIT) x factor de crecimiento de la población de la provincia x 4 + modificador de crecimiento de la provincia + modificador de crecimiento tecnológico + modificador de crecimiento nacional x 0.1. Luego divide eso por definir: SLAVE_GROWTH_DIVISOR si la población es esclava, y multiplica el tamaño de la población para determinar cuánto crece la población (el crecimiento se calcula y aplica durante el intervalo mensual de la población).

### Migración

La migración interna y externa "normal" parece manejarse de manera distinta a la migración "colonial". Al realizar la migración colonial, todas las poblaciones de la misma cultura+religión+tipo parecen combinarse antes de moverse. Las poblaciones solo pueden migrar entre países con el mismo estado civilizado/no civilizado.

#### Destinos

Objetivos del país para la migración externa: debe ser un país con su capital en un continente diferente al del país de origen *o* un país adyacente (los países en el mismo continente pero no adyacentes no son objetivos). Luego, cada objetivo de país se pondera: primero, se calcula el producto de los modificadores de destino de migración del país (incluido el valor base), y cualquier resultado menor a 0.01 se aumenta a ese valor. Ese valor se multiplica luego por (1.0 + el modificador de atractivo de inmigrantes de la nación). Suponiendo que haya objetivos válidos para la inmigración, se seleccionan las tres naciones con los valores más altos como posibles objetivos. La población (o, más bien, la parte de la población que está migrando) luego se dirige a uno de esos tres objetivos, seleccionado al azar según su peso relativo de atractivo. Luego, se selecciona el destino provincial final para la población como si se tratara de una migración interna normal.

Destino para la migración interna: las provincias coloniales no son objetivos válidos, al igual que las provincias no capitales estatales para tipos de población restringidos a capitales. Las provincias válidas se ponderan según el producto de los factores, multiplicado por el valor del enfoque de inmigración + 1.0 si está presente, multiplicado por el modificador de atractivo de inmigración de las provincias + 1.0. La población se distribuye entonces de manera más o menos uniforme en esas provincias con atractivo positivo proporcional a su atractivo, o se deposita en algún lugar al azar si no hay provincias atractivas.

La migración colonial parece funcionar de manera similar a la migración interna, excepto que *solo* las provincias coloniales son objetivos válidos y las poblaciones pertenecientes a culturas con "ultramar" = false no migrarán colonialmente fuera del mismo continente. Parece utilizarse el mismo desencadenante que la migración interna para ponderar las provincias coloniales.

#### Cantidades

Para poblaciones no esclavas ni coloniales en provincias con una población total > 100, algunas poblaciones pueden migrar dentro de la nación. Esto se hace calculando el factor de probabilidad de migración de manera *aditiva*. Si no es negativo, las poblaciones pueden migrar, y lo multiplicamos por (modificador de impulso migratorio de la provincia + 1) x definir: IMMIGRATION_SCALE x tamaño de la población para averiguar cuántas migran.

Si una nación tiene colonias, las poblaciones no trabajadoras de fábricas y no ricas en provincias con una población total > 100 pueden mudarse a las colonias. Esto se hace calculando el factor de probabilidad de migración colonial de manera *aditiva*. Si no es negativo, las poblaciones pueden migrar, y lo multiplicamos por (modificador de impulso migratorio de la provincia + 1) x (migración colonial desde la tecnología + 1) x definir: IMMIGRATION_SCALE x tamaño de la población para averiguar cuántas migran.

Finalmente, las poblaciones en una nación civil que no están en una colonia y que no pertenecen a un grupo cultural "ultramarino" en provincias con una población total > 100 pueden emigrar. Esto se hace calculando el factor de probabilidad de migración de emigración de manera *aditiva*. Si no es negativo, las poblaciones pueden migrar, y lo multiplicamos por (modificador de impulso migratorio de la provincia + 1) x (1 + modificador de impulso migratorio de la provincia) x definir: IMMIGRATION_SCALE x tamaño de la población para averiguar cuántas migran.

### Ascenso/Degradación

Las poblaciones parecen "ascender" a otras poblaciones de la misma estrata o superior. De manera similar, "degradan" a poblaciones de la misma estrata o inferior. (¿Así que tanto el ascenso como la degradación pueden mover poblaciones dentro de la misma estrata?). Los factores de ascenso y degradación parecen calcularse de manera aditiva (tomando la suma de todas las condiciones verdaderas). Si hay un enfoque nacional establecido hacia un tipo de población en el estado, eso también se suma a las posibilidades de ascender a ese tipo. Si el peso neto para el tipo de población mejorado es > 0, ese tipo de población siempre parece ser seleccionado como el tipo de ascenso. De lo contrario, el tipo se elige al azar, proporcionalmente a los pesos. Si se selecciona el ascenso a granjero para una provincia minera, o viceversa, se trata como la selección de jornalero en su lugar (o viceversa). Esto obviamente tiene el efecto de hacer que esos tipos de población sean aún más probables de lo que serían de otra manera.

Hay errores conocidos en términos de ascenso que no respetan la cultura y/o religión. No veo razón para intentar replicarlos.

En cuanto a la degradación, parece haber una complicación adicional. Las poblaciones no parecen degradarse a tipos de población si hay más desempleo en ese objetivo de degradación que en su tipo de población actual. De lo contrario, el enfoque nacional parece tener el mismo efecto con respecto a la degradación.

Las poblaciones que se mueven/ascienden parecen llevar consigo algo de dinero, pero también hay dinero mágico generado por la participación inicial, al menos para los capitalistas, probablemente para que sea posible que los primeros capitalistas en un estado sin fábricas construyan una fábrica. Propongo lo siguiente en su lugar: que los capitalistas construyan una fábrica en una provincia sin fábricas abiertas (o reabran una fábrica cerrada en esas condiciones) sea simplemente gratuito.

Cantidad de ascensos:
Calcula el modificador de ascenso *de manera aditiva*. Si es no positivo, no hay ascenso para ese día. De lo contrario, si hay un enfoque nacional hacia un tipo de población presente en el estado y la población en cuestión podría ascender a ese tipo, añade el efecto del enfoque nacional al modificador de ascenso. A la inversa, no se permite que las poblaciones del tipo enfocado asciendan. Luego multiplica este valor por eficiencia administrativa nacional x definir: PROMOTION_SCALE x tamaño de la población para averiguar cuántos ascienden (aunque al menos una persona ascenderá por día si el resultado es positivo).

Cantidad de degradaciones:
Calcula el modificador de degradación *de manera aditiva*. Si es no positivo, no hay degradación para ese día. De lo contrario, si hay un enfoque nacional hacia un tipo de población presente en el estado y la población en cuestión podría degradarse a ese tipo, añade el efecto del enfoque nacional al modificador de degradación. Luego multiplica este valor por definir: PROMOTION_SCALE x tamaño de la población para averiguar cuántos se degradan (aunque al menos una persona se degradará por día si el resultado es positivo).

### Militancia

Definamos el modificador local de militancia de la población como el modificador de militancia de la provincia + el modificador de militancia de la nación + el modificador de militancia de la población principal de la nación (para estados no coloniales, no solo provincias principales).
Cada población tiene su militancia ajustada por el modificador de militancia local + (modificador de separatismo tecnológico + 1) x definir: MIL_NON_ACCEPTED (si la población no es de una cultura primaria o aceptada) - (satisfacción de las necesidades básicas de la población - 0.5) x definir: MIL_NO_LIFE_NEED - (satisfacción de las necesidades diarias de la población - 0.5)^0 x definir: MIL_LACK_EVERYDAY_NEED + (satisfacción de las necesidades diarias de la población - 0.5)v0 x definir: MIL_HAS_EVERYDAY_NEED + (satisfacción de las necesidades de lujo de la población - 0.5)v0 x definir: MIL_HAS_LUXURY_NEED + apoyo de la población a conservadurismo x definir: MIL_IDEOLOGY / 100 + apoyo de la población a la ideología del partido gobernante x definir: MIL_RULING_PARTY / 100 - (si la población tiene un regimiento adjunto, aplicado a lo sumo una vez) confiabilidad del líder / 1000 + definir: MIL_WAR_EXHAUSTION x agotamiento nacional de guerra x (suma de apoyo-por-cada-asunto x efecto-agotamiento-guerra-de-cada-asunto) / 100.0 + (para poblaciones no en colonias) apoyo-asunto-social-por-población x definir: MIL_REQUIRE_REFORM+ apoyo-asunto-político-por-población x definir: MIL_REQUIRE_REFORM

La militancia se actualiza mensualmente

NOTA PARA MI YO FUTURO: el efecto predeterminado de agotamiento de guerra de cualquier asunto es 1, no 0.

### Conciencia

Las poblaciones en provincias controladas por rebeldes no están sujetas a cambios en la conciencia.

De lo contrario, el cambio diario en la conciencia es:
(necesidades de lujo satisfechas de la población x definir: CON_LUXURY_GOODS
+ definir: CON_POOR_CLERGY o definir: CON_MIDRICH_CLERGY x fracción de clérigos en la provincia
+ pluralidad nacional x 0v((modificador de impacto en la conciencia de la alfabetización nacional + 1) x definir: CON_LITERACY x alfabetización de la población)
) x definir: CON_COLONIAL_FACTOR si es colonial
+ modificador de conciencia de la población de la provincia + modificador de conciencia nacional de la población + modificador de conciencia nacional del núcleo de la población (en estados no coloniales) + modificador de conciencia de población no aceptada a nivel nacional (si no es de cultura principal o aceptada)

La conciencia se actualiza mensualmente.

### Alfabetización

La alfabetización de cada población cambia por: 0.01 x definir: LITERACY_CHANGE_SPEED x gasto en educación x ((población total de clérigos en la provincia / población total de la provincia - definir: BASE_CLERGY_FOR_LITERACY) / (definir: MAX_CLERGY_FOR_LITERACY - definir: BASE_CLERGY_FOR_LITERACY))^1 x (modificador nacional para la eficiencia de la educación + 1.0) x (eficiencia tecnológica de la educación + 1.0). La alfabetización no puede caer por debajo de 0.01.

La alfabetización se actualiza mensualmente.

### Ideología

Para las ideologías después de su fecha de habilitación (el descubrimiento/activación real es irrelevante) y no restringidas a civilizaciones solo para poblaciones en una civilización no desarrollada, el modificador de atracción se calcula *multiplicativamente*. Luego, estos valores se normalizan colectivamente. Si el valor normalizado es mayor que el doble del apoyo actual de la población a la ideología: añadir 0.25 al apoyo actual de la población a la ideología.
Si el valor normalizado es mayor que el apoyo actual de la población a la ideología: añadir 0.05 al apoyo actual de la población a la ideología.
Si el valor normalizado es mayor que la mitad del apoyo actual de la población a la ideología: no hacer nada.
De lo contrario: restar 0.25 al apoyo actual de la población a la ideología (hasta un mínimo de cero).
El apoyo ideológico de la población se normaliza luego de los cambios.

### Problemas

Al igual que con las ideologías, el modificador de atracción para cada problema se calcula *multiplicativamente* y luego se normalizan colectivamente. Luego, anulamos la atracción para cualquier problema que no sea actualmente posible (es decir, su condición de activación no se cumple o no es el siguiente/paso anterior para un problema del tipo de siguiente paso, y solo las cuestiones de partido son válidas aquí para los estados incivilizados).

Luego, al igual que con las ideologías, verificamos cuánto está por encima y por debajo el atractivo normalizado del apoyo actual, con algunas diferencias. Primero, para problemas políticos o sociales, multiplicamos la magnitud del ajuste por (modificador de deseo de reforma política nacional + 1) o (modificador de deseo de reforma social nacional + 1) según corresponda. En segundo lugar, la magnitud base del cambio es o bien (modificador de velocidad de cambio de problemas nacionales + 1.0) x 0.25 o (modificador de velocidad de cambio de problemas nacionales + 1.0) x 0.05 (en lugar de un fijo 0.05 o 0.25). Finalmente, hay un "rango" adicional a 5 veces más o menos donde el ajuste es un valor fijo de 1.0.

## Militar

### Gastos

Las naciones consumen (o intentan consumir) productos básicos para sus unidades terrestres y navieras diariamente a una tasa proporcional a sus gastos terrestres/navieros (a máxima inversión intentan consumir el 100% de los productos básicos necesarios). Estos productos básicos se restan directamente de las reservas de una nación (y no se permiten reservas negativas).

### Fabricación de CB (Causa de Guerra)

La fabricación de CB por parte de una nación se pausa cuando esa nación está en crisis (ni tampoco suceden eventos relacionados con la fabricación de CB). Los CB que se vuelven inválidos (las naciones involucradas ya no cumplen las condiciones o entran en guerra entre sí) se cancelan (y el jugador debería ser notificado en este evento). De lo contrario, la fabricación de CB avanza por puntos iguales a:
define: CB_GENERATION_BASE_SPEED x cb-type-construction-speed x (modificadores de velocidad de construcción de CB nacionales + modificador de velocidad de construcción de CB tecnológico + 1). Este valor se multiplica aún más por (define: CB_GENERATION_SPEED_BONUS_ON_COLONY_COMPETITION + 1) si el país que fabrica y el país objetivo compiten colonialmente por el mismo estado, y luego aún más por (define: CB_GENERATION_SPEED_BONUS_ON_COLONY_COMPETITION_TROOPS_PRESENCE + 1) si alguno de los dos países tiene unidades en ese estado colonial. (schombert: Realmente no estoy convencido de que estos efectos valgan la pena para implementarlos). Cuando la fabricación alcanza los 100 puntos, se completa y la nación obtiene el CB.

Cada día, un CB en fabricación tiene una probabilidad de define: CB_DETECTION_CHANCE_BASE entre 1000 de ser detectado. Si se descubre, el país que fabrica el CB obtiene la infamia por ese objetivo de guerra (suma de los costos de puntos) x la fracción de fabricación restante. Si se descubre, las relaciones entre las dos naciones cambiarán según define: ON_CB_DETECTED_RELATION_CHANGE. Si se descubre, cualquier estado con un punto conflictivo en la nación objetivo verá aumentar su tensión según define: TENSION_ON_CB_DISCOVERED.

Cuando el progreso de la fabricación alcanza los 100, el CB permanecerá válido durante define: CREATED_CB_VALID_TIME meses (por lo tanto, x30 días para nosotros). Ten en cuenta que los CB pendientes tienen su nación objetivo fija, pero todos los demás parámetros son flexibles.

### Otros Casus Belli (CB)

Además de fabricar un Casus Belli, puedes obtener uno de cualquier CB con "always = yes" si se satisface la condición "is_valid" (con el país objetivo potencial en alcance y el país al que se le otorgaría el CB en "this"). Creo que esto se prueba diariamente. Al menos para el jugador, se debe mantener algún registro de qué CB estaban disponibles en el día anterior para poder enviar mensajes sobre la obtención/pérdida de CB para estos CB desencadenados.

### Líderes (Generales y Almirantes)

#### Reclutamiento

Obtener un nuevo líder tiene un costo de define: LEADER_RECRUIT_COST puntos de liderazgo. Si el reclutamiento automático de líderes está activado (¿y por qué lo desactivarías alguna vez?), entonces, si tienes uno o más ejércitos/flotas a los que se les podría asignar un líder (que no estén en una provincia actualmente hostil, que no estén en el mar y que no estén actualmente en retirada) y que actualmente no tengan uno, y si tienes puntos de liderazgo suficientes, se reclutarán automáticamente nuevos líderes. El juego parece intentar equilibrar la cantidad de generales y almirantes reclutados con la cantidad que necesitas de cada tipo... No estoy seguro de que valga la pena respetar eso; probablemente intentaría llenar primero los ejércitos.

Los nuevos líderes reciben un nombre aleatorio basado en la cultura principal de su nación. Los líderes también reciben una personalidad aleatoria y un rasgo de antecedentes aleatorio. Esto parece ser completamente aleatorio *excepto*—y aquí se vuelve extraño—los almirantes tienden a evitar tener personalidades y antecedentes que modifiquen la fiabilidad, ya sea positiva o negativamente.

Las naciones parecen poder almacenar hasta define: LEADER_RECRUIT_COST x 3 puntos de liderazgo. Las naciones acumulan puntos de liderazgo mensualmente. Para calcular la cantidad, primero tomamos para cada población que proporciona puntos de liderazgo y multiplicamos la cantidad de puntos que da por la proporción de la población nacional compuesta por ese tipo de población hasta su óptimo de investigación (limitando esta proporción a 1). Luego sumamos esos valores y agregamos el modificador de la nación para el liderazgo. Finalmente, multiplicamos esa suma por (el modificador del liderazgo nacional + 1), dando como resultado el aumento mensual de puntos de liderazgo.

Una nación obtiene ((número-de-oficiales / población-total) / óptimo-de-oficiales)^1 x cantidad-de-liderazgo-por-oficial + modificador-nacional-al-liderazgo x (modificador-nacional-al-liderazgo-modificador + 1) puntos de liderazgo por mes.

#### Probabilidades diarias de muerte

Los líderes que tienen menos de 26 años y no están en combate no tienen ninguna posibilidad de morir. De lo contrario, tomamos la edad del líder y la dividimos por definir: LEADER_AGE_DEATH_FACTOR. Luego multiplicamos ese resultado por 2 si el líder está actualmente en combate. Esa es entonces la probabilidad actual de muerte del líder de... mis notas dicen 11,000 aquí. Ten en cuenta que el jugador solo recibe mensajes de muerte del líder si el líder está actualmente asignado a un ejército o una armada (si la configuración de mensajes está activada).

## Guerras

- Cada guerra tiene dos lados, y cada lado tiene un beligerante principal.
- Los beligerantes principales negocian en acuerdos de paz para todo su lado.
- Los beligerantes secundarios pueden, en guerras no críticas, negociar de manera independiente con el beligerante principal en el lado opuesto.
- Las guerras deben mantenerse en un estado válido (ver abajo), generalmente esto se logra eliminando la causa del estado inválido. Cualquier problema adicional causado por tal eliminación puede que no se detecte hasta la siguiente verificación de la validez de la guerra.
- Una guerra que termine sin miembros en uno (o ambos) lados simplemente se dará por terminada en su totalidad.
- Una provincia está bloqueada si hay una unidad naval hostil en una provincia marítima adyacente y no hay combate naval en curso allí. Mis notas dicen que solo las provincias que no son de ultramar (o tal vez solo las conectadas a la capital) cuentan para calcular la fracción bloqueada.

### Guerra y paz

#### Romper una tregua

Si declaras una guerra mientras tienes una tregua, obtienes infamia igual a la suma de la infamia por romper la tregua asociada con cada uno de los po_tags para el objetivo de guerra que estás declarando, multiplicado por el factor de infamia por romper la tregua específico para ese cb. De la misma manera, pierdes la suma de la prestigio por romper la tregua asociada con cada uno de los po_tags multiplicado por el factor de prestigio por romper la tregua del cb. Y lo mismo se hace pero con el factor de belicosidad por romper la tregua para determinar cuánto aumentar la belicosidad de cada población en tu nación.

#### Declarar una guerra

Todos los aliados defensores que posiblemente puedan unirse reciben una llamada a las armas. El atacante puede opcionalmente llamar a las armas a sus aliados.

#### Unirse a una guerra

Cuando te unes (o declaras) a una guerra, tus relaciones con las naciones en el lado opuesto disminuyen en -100. Si terminas en el lado opuesto de una guerra como aliado, tu alianza se rompe, pero sin ninguna penalización adicional de relaciones. Los estados subordinados y los vasallos son arrastrados automáticamente a la guerra.

#### Llamar a aliados

Los aliados pueden ser llamados a una guerra. En una guerra defensiva, se llamará al líder de la esfera del objetivo. Cualquier aliado llamado debe poder unirse a la guerra:

Condiciones estándar para unirse a la guerra: no se puede unir si ya estás en guerra contra cualquier atacante o defensor. No se puede unir a una guerra civil. No se puede unir a una guerra contra tu líder de esfera o señor supremo. No se puede unir a una guerra de crisis antes de que se inventen las grandes guerras (es decir, debes estar en la crisis). No se puede unir como atacante contra alguien con quien tienes una tregua.

Hay reglas algo opacas sobre cuándo un aliado llamado a la guerra tomará el liderazgo en la guerra. Podemos elegir mantener el liderazgo de la guerra fijo o permitir que el participante de mayor rango sea el líder.

#### Nombrar la guerra

El nombre de la guerra está determinado por la propiedad `war_name` del CB con el que se declaró la guerra, precedido por NORMAL_ o AGGRESSION_ (sí, con esa ortografía. Sí, lo sé). (Recibe el prefijo de agresión si el atacante no tiene un cb válido cuando se declara la guerra... lo que no debería ser posible, ¿verdad, ya que el nombre de la guerra proviene del CB con el que se declara la guerra, ¿verdad...?). Si no hay un CB declarante, la guerra se nombra por defecto NORMAL_WAR_NAME o AGGRESSION_WAR_NAME.

Una gran guerra se llama GREAT_WAR_NAME.

El juego aparentemente también admite nombres predefinidos especiales entre naciones específicas (o más bien, etiquetas específicas). No tengo la intención de admitir esto.

Además de primero y segundo, country_adj y state también están disponibles en estas cadenas de texto.

#### Reglas Especiales para Crisis

Una crisis pendiente actúa de manera similar a una guerra activa en el sentido de que se pueden agregar objetivos de guerra y hacer ofertas de paz. Al agregar un objetivo de guerra a la crisis como una oferta para que alguien se una, multiplica el costo de infamia normal por definir: CRISIS_WARGOAL_INFAMY_MULT.

#### Acción de Paz

Costa definir: PEACE_DIPLOMATIC_COST (aunque parece que puedes terminar una guerra sin objetivos de guerra de forma gratuita). La paz requiere un líder de guerra en al menos un lado de la oferta, excepto en las guerras de crisis, en las que no son posibles las paz separadas.

Cuando se acepta una oferta de paz, las relaciones entre las naciones aumentan en define: PEACE_RELATION_ON_ACCEPT. Si se rechaza, las relaciones aumentan en define: PEACE_RELATION_ON_DECLINE.

Si se rechaza una oferta de paz "buena", la nación que la rechaza gana define: GOOD_PEACE_REFUSAL_WAREXH de agotamiento de guerra y todas sus poblaciones ganan define: GOOD_PEACE_REFUSAL_MILITANCY. ¿Qué cuenta como una oferta buena? Bueno, si la oferta de paz se considera "mejor" de lo esperado. Esto parece ser complicado de calcular e involucra: la dirección en la que va la guerra (signo del último cambio en la puntuación de guerra), la cantidad general de fuerzas en cada lado (contando las armadas por menos), el tiempo desde el inicio de la guerra, el agotamiento de la guerra, la puntuación de guerra, el costo de paz de la oferta y si el receptor será anexado como resultado.

Una oferta de paz debe ser aceptada cuando la puntuación de guerra alcanza el 100.

Cuando se acepta una oferta de paz perdedora, el partido gobernante en la nación perdedora ve reducida su lealtad al partido en define: PARTY_LOYALTY_HIT_ON_WAR_LOSS por ciento en todas las provincias (esto incluye aceptar una oferta de resolución de crisis en la que pierdes). Cuando una nación sale de una guerra, se lleva consigo a todos sus vasallos/subestados. Las naciones del otro lado de la guerra obtienen una tregua con la nación durante define: BASE_TRUCE_MONTHS + los mayores meses de tregua para cualquier CB en el acuerdo de paz.

Cuando un objetivo de guerra falla (la nación a la que apunta abandona la guerra sin que ese objetivo de guerra se cumpla): la nación que lo agregó pierde prestigio de WAR_FAILED_GOAL_PRESTIGE_BASE^(WAR_FAILED_GOAL_PRESTIGE x prestigio actual) x factor de penalización de CB. Cada población en esa nación gana factor de penalización de CB x define: WAR_FAILED_GOAL_MILITANCY de beligerancia.

Resultados de objetivos de guerra:
- po_annex: la nación es anexada, los vasallos y subestados son liberados, se disuelven las relaciones diplomáticas.
- po_demand_state: se toma el estado (esto puede convertirse en anexión si es el último estado).
- po_remove_cores: también se eliminan los núcleos de cualquier territorio tomado/territorio objetivo si ya es propiedad del remitente.
- po_transfer_provinces: todos los estados válidos se transfieren a la nación especificada en el objetivo de guerra. Las relaciones entre ese país y la nación que agregó el objetivo de guerra aumentan en define: LIBERATE_STATE_RELATION_INCREASE. Si la nación se crea de nuevo por esto, la nación de la que se creó obtiene una tregua de define: BASE_TRUCE_MONTHS meses con ella (y se coloca en la esfera del liberador si esa nación es una GP).
- po_add_to_sphere: deja su esfera actual y tiene su opinión sobre esa nación establecida como hostil. Se agrega a la esfera de la nación que agregó el objetivo de guerra con la máxima influencia.
- po_clear_union_sphere: cada nación del grupo cultural de los actores en la esfera de la nación objetivo se va (y tiene su relación establecida como amistosa) y se agrega a la esfera de la nación actor con la máxima influencia. Todos los vasallos de la nación objetivo afectados por esto son liberados.
- po_release_puppet: la nación deja de ser un vasallo.
- po_make_puppet: la nación objetivo libera a todos sus vasallos y luego se convierte en vasallo de la nación actuante.
- po_destory_forts: reduce los niveles de fortaleza a cero en cualquier estado objetivo.
- po_destory_naval_bases: como se mencionó anteriormente.
- po_disarmament: una fracción aleatoria de las unidades de la nación se destruye. Se cancelan todas las construcciones de unidades actuales. La nación queda desarmada. El desarme dura hasta define: REPARATIONS_YEARS o hasta que la nación esté en guerra nuevamente.
- po_reparations: la nación está obligada a pagar reparaciones durante define: REPARATIONS_YEARS.
- po_remove_prestige: la nación objetivo pierde (prestigio actual x define: PRESTIGE_REDUCTION) + define: PRESTIGE_REDUCTION_BASE de prestigio.
- po_install_communist_gov: la nación objetivo cambia su tipo de gobierno e ideología gobernante (si es posible) al de la nación que agregó el objetivo de guerra. Las relaciones con la nación que agregó el objetivo de guerra se establecen en 0. La nación abandona su esfera actual e ingresa a la esfera del actor si es una GP. Si la guerra continúa, el líder de guerra en el lado opuesto gana el CB apropiado `counter_wargoal_on_install_communist_gov`, si lo hay y se permite por las condiciones de ese CB.
- po_uninstall_communist_gov_type: la nación objetivo cambia su tipo de gobierno al de la nación que agregó el objetivo de guerra. La nación abandona su esfera actual e ingresa a la esfera del actor si es una GP.
- po_colony: la colonización termina, con el que agregó el objetivo de guerra obteniendo la colonia y todos los demás colonizadores siendo expulsados.
- otro/en general: se ejecuta el miembro `on_po_accepted` del CB. Ranura primaria: objetivo del objetivo de guerra. Esta ranura: nación que agregó el objetivo de guerra.
La nación que agregó el objetivo de guerra gana prestigio. Esto se hace calculando la suma, sobre todas las etiquetas de po, de la base-prestigio-para-esa-etiqueta v (prestigio-actual-de-las-naciones x prestigio-para-esa-etiqueta) y luego multiplicando el resultado por el factor de prestigio del CB. La nación que fue objetivo del objetivo de guerra también pierde tanto prestigio.

Cuando una nación es anexada por un objetivo de guerra, el líder de guerra en ese lado obtiene un CB `counter_wargoal_on_annex` si es posible.
(NOTA: estos CB se resuelven en realidad a través del archivo tweaks.lua, que contiene:
```
counter_wargoal_on_annex = { cb = "free_peoples" },
counter_wargoal_on_install_communist_gov = { cb = "uninstall_communist_gov_cb" },
```
)
Un objetivo de guerra agregado de esta manera resultará en la eliminación de un objetivo de guerra de statu quo en ese bando.

Cualquier finalización de guerra probablemente requiera verificar que las unidades no estén atrapadas en combates inválidos, actualizar el estado de bloqueo, etc.

Una nación que ha sido completamente anexada (es decir, que ya no existe) se eliminará automáticamente de todas las guerras en las que estuvo involucrada; también dará fin a todas las guerras en las que fue el atacante o defensor principal.

#### Ofertas de Resolución de Crisis

Las ofertas de resolución de crisis funcionan de manera similar a las ofertas de paz. Cada oferta de resolución de crisis rechazada aumenta la temperatura de la crisis actual en define: CRISIS_TEMPERATURE_ON_OFFER_DECLINE.

Cuando un objetivo de guerra falla (es decir, la crisis termina en paz sin que se presione el objetivo): la nación que lo agregó pierde WAR_FAILED_GOAL_PRESTIGE_BASE^(WAR_FAILED_GOAL_PRESTIGE x prestigio actual) x factor de penalización CB x define: CRISIS_WARGOAL_PRESTIGE_MULT. Cada población en esa nación obtiene factor de penalización CB x define: WAR_FAILED_GOAL_MILITANCY militancia. Cada población en esa nación obtiene factor de penalización CB x define: WAR_FAILED_GOAL_MILITANCY x define: CRISIS_WARGOAL_MILITANCY_MULT militancia.

Para los objetivos de guerra que son impuestos, los ganadores obtienen la cantidad normal de prestigio x define: CRISIS_WARGOAL_PRESTIGE_MULT.

El ganador de la crisis también obtiene año después de la fecha de inicio x define: CRISIS_WINNER_PRESTIGE_FACTOR_YEAR de prestigio. El ganador de la crisis obtiene un impacto en las relaciones con todos los que están de su lado, excepto que todos con un objetivo de guerra no cumplido pierden -define: CRISIS_WINNER_RELATIONS_IMPACT en las relaciones con el líder de su lado.

### Objetivos de guerra y puntaje de guerra

#### Agregar un objetivo de guerra

Cuando agregas un objetivo de guerra (incluido el que se declara al inicio de la guerra), se elimina de tu lista de CBs construidos. Una vez que la guerra está en curso: no se pueden agregar objetivos de guerra si hay un objetivo de status quo presente en el bando del conflicto de la nación que quiere agregar un objetivo de guerra. El objetivo de guerra debe ser válido (ver más abajo). La nación que agrega el objetivo de guerra debe tener un puntaje de guerra positivo contra el objetivo del objetivo de guerra (ver más abajo). Y la nación ya debe poder usar el CB en cuestión (por ejemplo, lo ha fabricado previamente) o debe ser un CB constructible y la nación que agrega el objetivo de guerra debe tener un apoyo jingoísta general >= define: WARGOAL_JINGOISM_REQUIREMENT (x define: GW_JINGOISM_REQUIREMENT_MOD en una gran guerra).

Cuando se agrega un objetivo de guerra, se ejecuta su efecto on_add. La nación que agrega el objetivo de guerra se coloca en la ranura principal, con la nación objetivo del objetivo de guerra en la ranura desde. La nación que agrega el objetivo de guerra obtiene infamia (si el objetivo de guerra aún no estaba disponible). Esto se hace sumando la infamia determinada por las etiquetas po y multiplicando por el `badboy_factor` del CB. Si es una gran guerra, este valor se multiplica además por definir: GW_JUSTIFY_CB_BADBOY_IMPACT.

#### Comprobación de la validez de un objetivo de guerra

- Países permitidos:
La condición de país permitido, si está presente, debe cumplirse (evaluada con la nación objetivo del objetivo de guerra en la ranura principal, la nación que agrega el objetivo de guerra en esta ranura, y cualquier otra nación asociada al objetivo de guerra, por ejemplo, el objetivo de liberación, en la ranura "from" si la hay).

- Estados permitidos:
Si el objetivo de guerra tiene `all_allowed_states`, afectará a todos los estados que cumplan con la condición. En cualquier caso, debe haber algún estado propiedad de la nación objetivo que cumpla con la condición `allowed_states` si está presente (evaluado con ese estado en la ranura principal, la nación que agrega el objetivo de guerra en esta ranura, y cualquier otra nación asociada al objetivo de guerra, por ejemplo, el objetivo de liberación, en la ranura "from" si la hay).

#### Costo de paz

Cada objetivo de guerra tiene un valor que determina cuánto vale en una oferta de paz (y las ofertas de paz están limitadas a 100 puntos de objetivos de guerra). Los objetivos de guerra obligatorios de la gran guerra tienen un costo de 0. Luego iteramos sobre las etiquetas de los objetivos de paz y sumamos el costo de paz de cada uno. Algunas etiquetas tienen un costo fijo en las definiciones, como define: PEACE_COST_RELEASE_PUPPET. Para cualquier cosa que conquiste provincias directamente (por ejemplo: exigir estado), cada provincia vale su valor relativo al costo total de las provincias propiedad del objetivo (ver abajo) x 2.8. Para po_clear_union_sphere, el costo es define: PEACE_COST_CLEAR_UNION_SPHERE x el número de naciones que se verán afectadas. Si la guerra es una gran guerra, este costo se multiplica por define: GW_WARSCORE_COST_MOD. Si es una gran guerra, las guerras mundiales están habilitadas y el puntaje de guerra es al menos define: GW_WARSCORE_2_THRESHOLD, el costo se multiplica por define: GW_WARSCORE_COST_MOD_2 en su lugar. El costo de paz de un solo objetivo de guerra está limitado a 100.0.

#### Puntaje de Guerra Continua

- Puntaje de guerra continua basado en la ocupación de objetivos de guerra (anexar, transferir provincias, demandar estados) y dejando pasar el tiempo, ganando batallas (tws_from_battles > 0).
- Limitado por definir:TWS_CB_LIMIT_DEFAULT.
- Para calcularlo: primero debes determinar el porcentaje de cumplimiento del objetivo de guerra. Este es el porcentaje de provincias ocupadas o, para el puntaje de guerra por batallas, consulta el puntaje de batalla a continuación.

#### Puntaje de Batalla

- Cero si se han librado menos de definir:TWS_BATTLE_MIN_COUNT batallas.
- Solo si el objetivo de guerra tiene tws_battle_factor > 0.
- Calcula las pérdidas relativas para cada bando (algo del orden de la diferencia en pérdidas / 10,000 para combates terrestres o la diferencia en pérdidas / 10 para combates navales) con los puntos yendo al ganador, y luego toma el total de los puntajes de pérdida relativa para ambos lados y divide por el puntaje de pérdida relativa para el defensor.
- Resta a tws_battle_factor y luego divide por definir:TWS_BATTLE_MAX_ASPECT (limitado a -1 a +1). Esto luego funciona como el porcentaje de ocupación descrito a continuación.

#### Puntaje de Ocupación

Aumenta por el porcentaje de ocupación x definir:TWS_FULFILLED_SPEED (hasta definir:TWS_CB_LIMIT_DEFAULT) cuando el porcentaje ocupado es >= definir:TWS_FULFILLED_IDLE_SPACE o cuando el porcentaje de ocupación es > 0 y el puntaje de ocupación actual es negativo.
Si no hay ocupación, el puntaje disminuye por definir:TWS_NOT_FULFILLED_SPEED. Esto solo puede llevar el puntaje a negativo después de definir:TWS_GRACE_PERIOD_DAYS, momento en el cual puede llegar a -definir:TWS_CB_LIMIT_DEFAULT.

#### Puntaje de Guerra fuera de Objetivos de Guerra

Fuera del puntaje de guerra de objetivos de guerra específicos, ganar o perder batallas puede contribuir hasta definir:MAX_WARSCORE_FROM_BATTLES. Otras ocupaciones cuentan proporcionalmente al puntaje de provincia de las provincias ocupadas en comparación con el puntaje total de provincias del objetivo.

#### Puntuación de guerra dirigida

La puntuación de guerra dirigida es la cantidad de puntuación de guerra que una nación en particular tiene contra otra nación en particular en el otro lado del conflicto. Se utiliza al agregar objetivos de guerra y cuando una nación intenta hacer una paz separada con otra. La puntuación de guerra dirigida se considera como 100 si el atacante tiene ocupadas todas las provincias que posiblemente puede y, además, tiene ocupado el 50% de las provincias del objetivo o la capital del objetivo. (O, -100 si ocurre lo contrario. Esto parece ser una especie de condición de capitulación forzada). Si al menos una de las dos naciones involucradas es líder de su bando, se aplica la puntuación de guerra de las batallas. Esta puntuación está limitada a definir: MAX_WARSCORE_FROM_BATTLES. Por cada batalla terrestre, el ganador obtiene la diferencia en pérdidas de fuerza (o cero si ganaron con más pérdidas de fuerza) / 10 más un adicional de 0.1 agregado a su "dirección" de la puntuación de guerra.

La puntuación de guerra por ocupaciones requiere conocer el valor de las provincias. Todas las provincias tienen un valor base de 1. Para provincias no coloniales: cada nivel de base naval aumenta su valor en 1. Si es una capital estatal, su valor aumenta en 1 por cada fábrica en el estado (el nivel de la fábrica no importa). Las provincias obtienen 1 punto por nivel de fortaleza. Este valor se duplica para provincias no extranjeras donde el propietario tiene un núcleo. Luego se triplica para la provincia capital de la nación.

Por cada provincia propiedad de B que A ocupa, obtiene crédito hacia su lado de la puntuación de guerra igual a 100 x el valor de esa provincia como fracción de todas las provincias propiedad de B. Sin embargo, esa provincia cuenta solo 3/4 de ese valor si está actualmente bloqueada por un enemigo (¿por qué?).

Finalmente, la puntuación de guerra se ve afectada por cualquier objetivo de guerra en juego contra cualquiera de los lados que tenga asociada una puntuación de guerra. Toma el costo de paz del objetivo de guerra y calcula 1^(costo de paz / definir: TWS_CB_LIMIT_DEFAULT) x puntuación de objetivo de guerra actual. Este valor también se suma a la puntuación de guerra efectiva.

La puntuación de guerra dirigida siempre se trata como si estuviera en el rango de -100 a 100.

### Estados de guerra no válidos

- Un beligerante principal no puede ser un vasallo o subestado de otra nación.
- Los objetivos de guerra deben seguir siendo válidos (deben pasar su comprobación de activación válida, deben tener países objetivos existentes, los países objetivos deben pasar cualquier comprobación de activación, debe haber un estado válido en el país objetivo, el país que los agregó debe seguir estando en la guerra, etc.).
- Los objetivos de guerra de crisis no pueden ser eliminados; esto puede anular otras comprobaciones de validez.
- Debe haber al menos un objetivo de guerra (que no sea el statu quo) en la guerra.
- Inactividad durante demasiado tiempo: si la guerra pasa demasiado tiempo sin que ocurra algún evento dentro de ella (batalla u ocupación), puede ser terminada. Si algo está ocupado, creo que la guerra está a salvo de ser terminada de esta manera.

### Movilización

Tamaño de movilización = modificador nacional al tamaño de movilización + modificador tecnológico al tamaño de movilización
Impacto de movilización = 1 - tamaño de movilización x (modificador de impacto económico de movilización nacional + modificador de impacto de movilización tecnológica), con un mínimo de cero.

Las unidades movilizadas provienen solo de provincias no ocupadas y no coloniales. En esas provincias, las unidades movilizadas provienen de poblaciones de la estrata pobre que no son soldados ni esclavos, con una cultura que es o la cultura principal de la nación o una cultura aceptada. La cantidad de regimientos que estas poblaciones pueden proporcionar se determina por el tamaño de la población x tamaño de movilización / definir: POP_SIZE_PER_REGIMENT. Las poblaciones proporcionarán hasta este número de regimientos por población, aunque los regimientos que ya estén proporcionando a rebeldes o que ya estén movilizados cuentan en contra de este número. Como máximo, se pueden crear por movilización nacional-modificador-de-impacto x (definir: MIN_MOBILIZE_LIMIT v número-de-regimientos-de-la-nación regimientos mediante la movilización).

La movilización no es instantánea. Provincia por provincia, la movilización avanza por definir: MOBILIZATION_SPEED_BASE x (1 + definir: MOBILIZATION_SPEED_RAILS_MULT x nivel-promedio-de-ferrocarril-en-el-estado / 5) hasta que alcance 1. Una vez que la movilización ha ocurrido en una provincia, la movilización en la siguiente provincia puede comenzar. El orden en que esto ocurre parece estar determinado por la velocidad de movilización: las provincias que se movilizarán más rápido van antes que aquellas que se movilizarán más lentamente.

La movilización aumenta la tensión en la crisis por definir: CRISIS_TEMPERATURE_ON_MOBILIZE.

### Conquista de provincias

Cuando una provincia cambia de dueño:
Todas las poblaciones pierden cualquier dinero que hayan depositado en el banco (y el banco central retiene ese dinero, por lo que ninguna población podría retirarlo). Las poblaciones abandonan cualquier movimiento o facción rebelde de la que formen parte. Si la provincia no es colonial y el nuevo dueño no tiene un núcleo allí, cualquier población con una cultura que no sea primaria o aceptada para el nuevo dueño adquiere definir:MIL_FROM_CONQUEST de militancia.

Todos los modificadores temporales en la provincia expiran automáticamente y se cancelan todas las construcciones en curso.
La provincia conquistada obtiene nacionalismo igual a definir:YEARS_OF_NATIONALISM.
Si la nación que realiza la conquista tiene un modificador no nulo de puntos de investigación al conquistar:
La nación obtiene "1 año de puntos de investigación" por cada provincia conquistada multiplicado por su multiplicador de puntos de investigación al conquistar, y como uncivilizado (por lo general), esto le permite superar su límite normal de puntos de investigación. Sin embargo, estos puntos de investigación de un año no se calculan normalmente; se calculan como si la provincia conquistada fuera un OPM con los modificadores y la tecnología de la nación conquistadora. Específicamente, permitimos que la suma de población (pop-sum) sea para cada tipo de población en la provincia (puntos de investigación de tipo x 1v(fracción de población / fracción óptima)). Luego, los puntos de investigación obtenidos son 365 x ((modificador nacional para puntos de investigación + modificador tecnológico para puntos de investigación + 1) x (modificador nacional a puntos de investigación) + pop-sum)) x modificador de puntos de investigación al conquistar.

Las provincias que una civilización conquista de un país no civilizado se convierten en colonias.

### Asedio

La guarnición se recupera al 10% por día cuando no está siendo asediada (hasta el 100%).

Solo los regimientos estacionarios, no marcados en negro, con al menos 0.001 de fuerza contribuyen a un asedio.

Si hay un regimiento con capacidad de asedio presente:
Encontramos el nivel efectivo del fuerte restando: (redondeando este valor hacia abajo al entero más cercano) el mayor valor de asedio presente x
((la proporción de la fuerza de los regimientos con capacidad de asedio presente a la fuerza total de todos los regimientos) ^ definir: ENGINEER_UNIT_RATIO) / definir: ENGINEER_UNIT_RATIO, reduciéndolo a un mínimo de 0.
Calculamos el modificador de velocidad de asedio como: 1 + definir: RECON_SIEGE_EFFECT x mayor-valor-de-reconocimiento-presente x ((la proporción de la fuerza de los regimientos con reconocimiento presente a la fuerza total de todos los regimientos) ^ definir: RECON_UNIT_RATIO) / definir: RECON_UNIT_RATIO.
Calculamos el modificador para el número de brigadas: primero obtenemos el "número de brigadas" como la fuerza total de los regimientos x 1000 / definir: POP_SIZE_PER_REGIMENT, limitándolo como máximo a definir: SIEGE_BRIGADES_MAX. Luego calculamos el bono como (número-de-brigadas - definir: SIEGE_BRIGADES_MIN) x definir: SIEGE_BRIGADES_BONUS si el número-de-brigadas es mayor que el mínimo, y como número-de-brigadas / definir: SIEGE_BRIGADES_MIN en caso contrario.
Finalmente, la cantidad restada de la guarnición cada día es: modificador-de-velocidad-de-asedio x modificador-de-número-de-brigadas x Progress-Table\[número-aleatorio-de-0-a-9\] x (1.25 si el propietario está asediándolo de vuelta) x (1.1 si el asediante no es el propietario pero tiene un núcleo) / Siege-Table\[nivel-efectivo-del-fuerte\]

La guarnición vuelve al 100% inmediatamente después de completarse el asedio y cambiar el control. Si tu asedio devuelve una provincia al control de su dueño sin que este participe, obtienes +2.5 relaciones con el propietario.

Cuando el control de una provincia cambia como resultado de un asedio, y no vuelve al propietario, se activa un evento aleatorio `on_siege_win`, sujeto a que se cumplan las condiciones del evento.

Tabla de Asedio:
| Nivel ajustado | Valor |
|----|----|
| 0 | 1.0 |
| 1 | 2.0 |
| 2 | 2.8 |
| 3 | 3.4 |
| 4 | 3.8 |
| 5 | 4.2 |
| 6 | 4.5 |
| 7 | 4.8 |
| 8 | 5.0 |
| 9 | 5.22 |

Tabla de Progreso:
| Tirada modificada | Valor |
|----|----|
| 0 | 0    |
| 1 | 0.2  |
| 2 | 0.5  |
| 3 | 0.75 |
| 4 | 0.75 |
| 5 | 1    |
| 6 | 1.1  |
| 7 | 1.1  |
| 8 | 1.25 |
| 9 | 1.25 |

### Combate terrestre

(En esta sección, "unidades" se refiere a regimientos)

- Durante esta sección definiremos el ADMOS de un regimiento como (ataque + defensa + maniobra) x organización x fuerza.
- El ancho de combate para una batalla se determina por el menor entre definir: BASE_COMBAT_WIDTH y el modificador tecnológico al ancho de combate de cualquier participante, con un máximo de 30 y un mínimo de 2, multiplicado por el modificador de ancho de combate del terreno + 1.
- Tácticas militares efectivas = definir: BASE_MILITARY_TACTICS + tácticas provenientes de tecnología.
- Cada lado tiene un líder a cargo del combate, que es el líder con el mayor valor determinado por la siguiente fórmula: (organización x 5 + ataque + defensa + moral + velocidad + atrición + experiencia / 2 + reconocimiento / 5 + confiabilidad / 5) x (prestigio + 1).

Cualquier unidad añadida a un combate en curso después de las iniciales se agrega primero como reservas adicionales. Las unidades con una fuerza menor a 0.1 u organización menor a 1 se tratan inmediatamente como retiradas. Luego, se dividen las unidades en tres grupos: aquellas con un valor de soporte positivo, aquellas con cero soporte y reconocimiento positivo, y el resto.

La línea frontal inicial se construirá con pares de unidades de reconocimiento flanqueando el centro, aunque creo que se le da preferencia a las unidades sin reconocimiento (? alguien debería verificar esto). El número de unidades sin reconocimiento colocadas en la línea frontal se limita a lo sumo al número de regimientos opuestos. En todos los casos, se eligen las unidades con mayor ADMOS para la línea frontal inicial. Las unidades con soporte positivo se colocan en la línea trasera en orden de ADMOS.
Luego, intentamos llenar la línea trasera con reservas. Si tanto el hueco frontal como el trasero están vacíos, primero se busca una unidad sin soporte con organización al menos de 1 y fuerza al menos de 0.1. Luego, para los espacios traseros donde el espacio frontal no está vacío, se colocan unidades con valor de soporte positivo, y finalmente se colocan unidades sin soporte (alguien debería verificar esto; ¿realmente las unidades de soporte tienen preferencia aquí?).
Todas las unidades restantes que no se colocan inicialmente en el campo se colocan en las reservas.

El primer día, y cada cinco días después, cada lado en el combate recibe una "tirada de dados" de 0 a 9, inclusive.
Cada vez que se tiran los dados después del primer día, si el atacante saca más alto que el defensor, el defensor pierde un punto de fortificación.
Un lado puede aniquilar instantáneamente al otro en el primer día. Esto sucede si un lado tiene menos de una décima parte de la fuerza total de la unidad del otro. En una aniquilación, cada regimiento se trata como si hubiera sido destruido.

Modificadores de combate:
Además de los dados, ambos lados son afectados por:
- Ataque o defensa del líder a cargo (ataque para atacantes, defensa para defensores).
- define: GAS_ATTACK_MODIFIER si el lado tiene ataque químico y el otro lado no tiene defensa química.
- Bono de defensa del terreno (solo para el defensor).
- -1 por atacar a través de un río o -2 por atacar desde el mar.
- Menos el valor de fortificación más bajo en el lado opuesto / 2 (si el lado tiene una fracción de unidades con reconocimiento al menos definir: RECON_UNIT_RATIO) o dividido por (mayor reconocimiento del regimiento enemigo x (reconocimiento del líder + 1) x fracción de la fuerza de la unidad de reconocimiento / fuerza total).

En cada turno, el atacante y el defensor infligen daño simultáneamente.

Las unidades atacan a la fila frontal opuesta y pueden mirar a la izquierda o a la derecha de su posición hasta `maneuver` posiciones.
Las unidades en el lado atacante usan su estadística de `attack` para infligir daño, mientras que las unidades en el lado defensor usan `defense`.
Daño infligido a la fuerza: fuerza de la unidad x (ataque/defensa x 0.1 + 1) x Tabla de Modificadores\[modificadores + 2\] x 2 / ((nivel efectivo del fuerte x 0.1 + 1) x tácticas militares efectivas del lado opuesto x (experiencia del regimiento enemigo x 0.1 + 1))
Daño infligido a la organización: fuerza de la unidad x (ataque/defensa x 0.1 + 1) x Tabla de Modificadores\[modificadores + 2\] x 2 / ((nivel efectivo del fuerte x 0.1 + 1) x disciplina del lado opuesto (si no es cero) x (experiencia del regimiento enemigo x 0.1 + 1))
Las unidades que atacan desde la fila trasera tienen estos valores multiplicados por el valor de soporte de la unidad.

Si la unidad está en una provincia controlada por una potencia hostil, encontramos el nivel efectivo del fuerte como con un asedio (Encontramos el nivel efectivo del fuerte restando: (redondeando este valor hacia abajo al entero más cercano) mayor valor de asedio presente x
((la proporción de la fuerza de los regimientos con asedio presente a la fuerza total de todos los regimientos) ^ definir: ENGINEER_UNIT_RATIO) / definir: ENGINEER_UNIT_RATIO, reduciéndolo a un mínimo de 0).

Cuando un regimiento recibe daño en la fuerza, el tamaño de la población que lo respalda se reduce por: definir: POP_SIZE_PER_REGIMENT x cantidad de daño x definir: SOLDIER_TO_POP_DAMAGE / (máxima fuerza x (pérdida de soldados a población por tecnología + 1)). Ten en cuenta que esto se aplica también al daño por atrición.

Cuando se destruye un regimiento rebelde, dividimos la militancia de la población que lo respalda por definir: REDUCTION_AFTER_DEFEAT.

Al completarse una batalla terrestre, ambos lados obtienen agotamiento de guerra igual a: definir: COMBATLOSS_WAR_EXHAUSTION x (modificador tecnológico de agotamiento de guerra + 1) x fuerza perdida / (definir: POP_SIZE_PER_REGIMENT x total de regimientos reclutables)

Cada combatiente ganador recibe un evento aleatorio `on_battle_won`, y cada combatiente perdedor recibe un evento aleatorio `on_battle_lost`.

Ambos lados calculan su fracción de pérdidas escalada, que es las pérdidas de fuerza como porcentaje de la fuerza total posible del regimiento (incluyendo la movilización). La fracción de pérdidas escalada del perdedor / la suma de las pérdidas escaladas forma la base de la ganancia de prestigio para la nación y el líder a cargo en el lado ganador. El líder ganador recibe ese valor / 100 como prestigio adicional. La nación y líder ganadores obtienen (definir: LEADER_PRESTIGE_LAND_GAIN + 1) x (modificador de prestigio de tecnología + 1) x ese valor. De manera similar, la nación y líder perdedores tienen su prestigio reducido, calculado de la misma manera.

Se gana puntuación de guerra en función de la diferencia en pérdidas (en términos absolutos) dividida por 5 más 0.1, con un mínimo de 0.1.

Cuando vences a los rebeldes de alguien más sin que estén presentes, obtienes +5 de relaciones con ellos.

Luego, cualquier unidad en la fila trasera con un espacio frontal vacío delante de ella se mueve hacia adelante.

Tabla de Modificadores
| Tirada modificada | Valor |
|----|----|
| -1 o menos | 0 |
| 0  | 0.02 |
| 1  | 0.04 |
| 2  | 0.06 |
| 3  | 0.08 |
| 4  | 0.10 |
| 5  | 0.12 |
| 6  | 0.16 |
| 7  | 0.20 |
| 8  | 0.25 |
| 9  | 0.30 |
| 10 | 0.35 |
| 11 | 0.40 |
| 12 | 0.45 |
| 13 | 0.50 |
| 14 | 0.60 |
| 15 | 0.70 |
| 16 | 0.80 |
| 17 o más | 0.90 |

### Combate Naval

(En esta sección, "unidades" se refiere a barcos)

En el primer día, y cada cinco días después, cada bando en el combate recibe una "tirada de dado" de 0 a 9, inclusive. Uno de los bandos puede eliminar instantáneamente al otro en el primer día. Esto ocurre si un bando tiene menos de una décima parte de la resistencia total del casco del otro. En caso de eliminación, cada barco se trata como si hubiera sido hundido.

El lado con más barcos no retirados tiene una penalización por apilamiento de 1 - barcos-del-otro / barcos-más, que está limitada a 0.9.

En cada turno, el atacante y el defensor infligen daño simultáneamente.

Un barco sin un objetivo (es decir, está buscando) tiene una probabilidad de define: NAVAL_COMBAT_SEEKING_CHANCE + líder-a-cargo-reconocimiento x (1.0 - define: NAVAL_COMBAT_STACKING_TARGET_SEEK x factor-de-penalización-de-apilamiento), o define: NAVAL_COMBAT_SEEKING_CHANCE_MIN, la que sea mayor. La selección del objetivo parece ser bastante complicada. ¿A alguien le importa si lo copiamos exactamente o no? Cuando se selecciona un objetivo, la distancia aumenta por un valor aleatorio en el rango \[0.0, 1.0) x (1.0 - duración-del-combate^define: NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) / NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) x NAVAL_COMBAT_SHIFT_BACK_ON_NEXT_TARGET hasta un máximo de 1000, y el barco cambia a aproximarse.

Un barco que se aproxima:
Tiene su distancia reducida por (valor-aleatorio-en-rango-\[0.0 - 0.5) + 0.5) x velocidad-máxima x define: NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR hasta un mínimo de 0.
Cambia a comprometido cuando su distancia + la distancia del objetivo es menor que su alcance de fuego.

Un barco comprometido:
Un barco comprometido tiene una probabilidad diaria de factor-de-penalización-de-apilamiento x define: NAVAL_COMBAT_STACKING_TARGET_CHANCE + define: NAVAL_COMBAT_CHANGE_TARGET_CHANCE de cambiar de objetivo. Esto lo devolvería a la búsqueda.
Ataque de torpedo: se trata como 0 excepto contra barcos grandes.
El daño a la organización es (potencia de fuego + ataque de torpedo) x Tabla-de-Modificadores\[modificadores + 2\] (ver arriba) x resistencia-del-objetivo x define: NAVAL_COMBAT_DAMAGE_ORG_MULT / (resistencia-máxima-del-objetivo x experiencia-del-objetivo x 0.1 + 1)
El daño a la resistencia es (potencia de fuego + ataque de torpedo) x Tabla-de-Modificadores\[modificadores + 2\] (ver arriba) x resistencia-del-atacante x define: NAVAL_COMBAT_DAMAGE_STR_MULT x define: NAVAL_COMBAT_DAMAGE_MULT_NO_ORG (si el objetivo no tiene organización) / (resistencia-máxima-del-objetivo x experiencia-del-objetivo x 0.1 + 1)

Barcos en retirada:
Un barco con una fracción de resistencia u organización menor que define: NAVAL_COMBAT_RETREAT_STR_ORG_LEVEL intentará retirarse. La probabilidad de que un barco se retire es define: NAVAL_COMBAT_RETREAT_CHANCE + líder-a-cargo-experiencia + líder-a-cargo_velocidad.
Un barco en retirada aumentará su distancia por define: NAVAL_COMBAT_RETREAT_SPEED_MOD x define: NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR x (valor aleatorio en el rango \[0.0 - 0.5) + 0.5) x velocidad-máxima-del-barco. Cuando alcanza una distancia de 1000, se ha retirado con éxito.

Cabe destacar que las pérdidas de barcos se informan en términos de barcos hundidos (a diferencia del combate terrestre, donde se informan en términos de daño infligido).

Las unidades embarcadas que se destruyen debido a que su barco se destruye no reducen el tamaño de población de la población respaldante (es decir, no se considera que han sufrido el daño necesario para destruirlas).

Al finalizar una batalla naval:
Cada combatiente ganador recibe un evento aleatorio de `on_battle_won`, y cada combatiente perdedor recibe un evento aleatorio de `on_battle_lost`.

Ambos lados calculan su fracción de pérdidas escalada, que es (1 + (suma sobre el tipo de barco: puntuación-de-consumo-de-suministros x pérdidas-de-resistencia)) como un porcentaje de (suministro naval total posible + 1). La fracción de pérdidas escalada del perdedor / la suma de las pérdidas escaladas forma la base de la ganancia de prestigio para la nación y el líder a cargo en el lado ganador. El líder ganador obtiene ese valor / 100 como prestigio adicional. La nación ganadora obtiene (defineLEADER_PRESTIGE_NAVAL_GAIN + 1) x (modificador-de-prestigio-por-tecnología + 1) x ese valor. De manera similar, la nación y el líder perdedores tienen su prestigio reducido, calculado de la misma manera.

El puntaje de guerra se obtiene según la diferencia en pérdidas (en términos absolutos) dividido por 5 más 0.1, con un mínimo de 0.1.

Ver también: (https://forum.paradoxplaza.com/forum/threads/understanding-naval-combat-sort-of.1123034/), aunque en caso de desacuerdo, debes asumir que este documento es correcto en ausencia de evidencia empírica del juego que demuestre lo contrario.

### Límite de suministro

(modificador de límite de suministro de la provincia + 1) x (2.5 si es de tu propiedad y control o 2 si solo está controlada, tienes alianza con el controlador, tienes acceso militar con el controlador, lo controla un rebelde, es una de tus provincias principales o la estás asediando) x (modificador tecnológico de límite de suministro + 1)

## Unidades

### Actualización diaria de unidades

Las unidades en combate ganan experiencia. La fórmula exacta me resulta un tanto opaca, pero esto es lo que sé: las unidades en combate ganan experiencia de manera proporcional a definir: EXP_GAIN_DIV, el bono de ganancia de experiencia proporcionado por su líder + 1, y luego algún otro factor que siempre es al menos 1 y aumenta a medida que el bando contrario tiene más organización.

Las unidades que no están en combate y no están embarcadas recuperan organización diariamente de la siguiente manera: (modificador de regeneración de organización nacional + moral de la tecnología + rasgo de moral del líder + 1) x factor de suministro de la unidad / 5 hasta la organización máxima posible para la unidad multiplicada por (0.25 + 0.75 x gasto efectivo en tierra o marítimo).

Las unidades que se están moviendo pierden cualquier bonificación de fortificación que hayan adquirido. Una unidad que no se está moviendo obtiene un punto de fortificación cada definir: DIG_IN_INCREASE_EACH_DAYS días.

Las unidades respaldadas por poblaciones con definir: MIL_TO_AUTORISE militancia o más que están en una facción rebelde y que tienen al menos 0.75 de organización se convertirán en unidades rebeldes.

Suministros: Las unidades rebeldes siempre se tratan como si tuvieran suministro completo. Las unidades que no están en combate consumen suministros. Los productos y la cantidad consumida de esta manera dependen del tipo específico de unidad. Las cantidades de suministro definidas por su tipo se multiplican por (2 - eficiencia administrativa nacional) x (consumo de suministro por tipo + modificador nacional al consumo de suministro)^0.01 x (gasto naval o terrestre según corresponda). La fracción promedio de ese consumo que podría ser satisfecha por el gasto naval o terrestre (según corresponda) para cada regimiento o nave se promedia luego en el ejército o la armada. Para los ejércitos, ese valor se convierte en su nuevo suministro. Para las armadas, este valor se multiplica además por (1 - la fracción que la nación está sobre el suministro naval) antes de convertirse en el nuevo valor de suministro.

Las armadas con suministros inferiores a definir: NAVAL_LOW_SUPPLY_DAMAGE_SUPPLY_STATUS pueden recibir daño por desgaste. Una vez que una armada ha estado por debajo de ese umbral durante definir: NAVAL_LOW_SUPPLY_DAMAGE_DAYS_DELAY días, cada nave en ella recibirá daño de definir: NAVAL_LOW_SUPPLY_DAMAGE_PER_DAY x (1 - suministros navales / definir: NAVAL_LOW_SUPPLY_DAMAGE_SUPPLY_STATUS) (a su valor de fuerza) hasta que alcance definir: NAVAL_LOW_SUPPLY_DAMAGE_MIN_STR, momento en el cual no se le infligirá más daño. NOTA: Las armadas controladas por la IA están exentas de esto, y cuando te das cuenta de que esto significa que *la mayoría* de los barcos están exentos, resulta menos claro por qué molestamos al jugador con esto.

### Reforzamiento mensual

Una unidad que no está retrocediendo, no está embarcada ni está en combate se refuerza (aumenta su fuerza) por:
definir: REINFORCE_SPEED x (modificador tecnológico de refuerzo + 1.0) x (2 si está en una provincia propia, 0.1 en una provincia con puerto no poseída, 1 en una provincia controlada, 0.5 si está en una provincia adyacente a una provincia con acceso militar, 0.25 en un puerto hostil no bloqueado y 0.1 en cualquier otra provincia hostil) x (modificador nacional de velocidad de refuerzo + 1) x suministros del ejército x (número de regimientos reales / número máximo posible de regimientos (parece un error para mí) o 0.5 si está movilizado).
La experiencia de las unidades también se reduce de manera que su nueva experiencia sea igual a la antigua experiencia dividida por (cantidad reforzada / 3 + 1).

### Reparación/actualización mensual de barcos

Un barco que está atracado en una base naval se repara (aumenta su fuerza) por:
fuerza máxima x (tasa de reparación tecnológica + modificador provincial a la tasa de reparación + 1) x suministros del barco x (modificador nacional de velocidad de refuerzo + 1) x suministros de la armada.

Una armada que está en el mar fuera del alcance de suministros aumenta su contador "en el mar" una vez al mes.

### Desgaste Mensual

Una unidad que no está en combate y no está marcada como desactivada recibe daño por desgaste el primer día del mes.

Para ejércitos:
Para los ejércitos en provincias de su propietario que no estén en retirada, el desgaste se calcula según la fuerza total de las unidades de su nación en la provincia. De lo contrario, el desgaste se calcula según la fuerza total de todas las unidades en la provincia. Primero calculamos (fuerza-total + rasgo-de-desgaste-del-líder) x (modificador-de-desgaste-de-la-tecnología + 1) - límite-de-suministro-efectivo-de-la-provincia (redondeado hacia abajo al entero más cercano) + modificador-de-desgaste-de-la-provincia + el-nivel-del-fuerte-hostil-más-alto-en-una-provincia-adyacente. Luego reducimos ese valor como máximo al modificador de desgaste máximo de la provincia, y finalmente agregamos definir: SEIGE_ATTRITION si el ejército está llevando a cabo un asedio. Las unidades que sufren desgaste pierden fuerza máxima x valor-de-desgaste x 0.01 puntos de fuerza. Esta pérdida de fuerza se trata como el daño sufrido en combate, lo que significa que reducirá el tamaño de la población respaldante.

Para flotas:
Solo las flotas controladas por el jugador sufren desgaste, y solo aquellas que no están en puerto o no están frente a una provincia costera controlada. Las flotas en zonas marítimas costeras generalmente sufren desgaste solo si están fuera del rango de suministro. El valor de desgaste para tales flotas es (tiempo-en-el-mar x 2 + 1 + rasgo-de-desgaste-del-líder) x (desgaste-naval-por-tecnología + 1). Si una nave sufre daño por desgaste es en parte aleatorio. El valor de desgaste / 10 es la probabilidad de que la nave sufra daño. Si la nave sufre daño, pierde fuerza máxima x valor-de-desgaste x 0.01 puntos de fuerza.

Cuando las flotas del jugador mueren por desgaste, los almirantes también se pierden.

### Movimiento

Las provincias adyacentes tienen una distancia base entre ellas (esta base también tiene en cuenta de alguna manera el terreno). Al moverse a una provincia, este costo se multiplica por (modificador-de-costo-de-movimiento-de-la-provincia-de-origen + 1.0)^0.05. La unidad "paga" este costo cada día según su velocidad, y cuando se paga todo, la unidad llega a su provincia de destino. La velocidad de un ejército o flota se basa en la velocidad de su nave o regimiento más lento x (1 + infraestructura-proporcionada-por-ferrocarriles x nivel-de-ferrocarril-de-la-provincia-de-origen) x (posiblemente-algun-modificador-para-cruzar-agua) x (definir: LAND_SPEED_MODIFIER o definir: NAVAL_SPEED_MODIFIER) x (rasgo-de-velocidad-del-líder + 1).

Cuando una unidad llega a una nueva provincia, sufre desgaste (como si hubiera pasado el intervalo mensual en la provincia).

### Estadísticas de Unidades

- La experiencia de la unidad llega hasta 100. Las unidades, después de ser construidas, comienzan con un nivel de experiencia base igual al bono proporcionado por las tecnologías + el modificador inicial de experiencia naval/terrestre de la nación (según corresponda).
- Las unidades comienzan con fuerza y organización máximas después de ser construidas.

### Construcción de Unidades

- Solo se construye una unidad por provincia a la vez.
- Consulta la sección de economía para obtener detalles sobre cómo ocurre la construcción.
- Después de ser construidas, las unidades se dirigen hacia el punto de reunión más cercano, si lo hay, para fusionarse con un ejército allí a su llegada.

### Regimientos por Población

- Una población de soldados debe tener al menos definir: POP_MIN_SIZE_FOR_REGIMENT para poder mantener cualquier regimiento.
- Si es al menos de ese tamaño, entonces puede mantener un regimiento por definir: POP_SIZE_PER_REGIMENT x definir: POP_MIN_SIZE_FOR_REGIMENT_COLONY_MULTIPLIER (si se encuentra en una provincia colonial) x definir: POP_MIN_SIZE_FOR_REGIMENT_NONCORE_MULTIPLIER (si no es colonial pero no está centralizada).

### Puntos de Suministro Naval

- Puntuación de suministro naval: obtienes definir: NAVAL_BASE_SUPPLY_SCORE_BASE x (2 elevado a la potencia de (su nivel - 1)) por cada base naval o definir: NAVAL_BASE_SUPPLY_SCORE_EMPTY por cada estado sin una, multiplicado por definir: NAVAL_BASE_NON_CORE_SUPPLY_SCORE si no es un núcleo ni está conectado a la capital (mínimo 1 por estado costero).
- Las naves consumen suministro de base naval según su puntuación de consumo de suministro. Sobrepasar la puntuación de suministro naval conlleva diversas penalizaciones (descritas en otros lugares).

## Movimientos

- Los movimientos se dividen en tres tipos: políticos, sociales e independentistas. Funcionalmente, podemos distinguir mejor los movimientos entre aquellos asociados con una posición sobre algún tema (ya sea político o social) y aquellos para la independencia de alguna identidad nacional.
- Los movimientos pueden acumular radicalismo, pero solo en naciones civilizadas. Internamente, podemos representar el radicalismo con dos valores, un valor de radicalismo y radicalismo transitorio. Cada día, el valor de radicalismo se calcula como: definir: MOVEMENT_RADICALISM_BASE + el radicalismo transitorio actual del movimiento + número de reformas políticas aprobadas en el país sobre la base x definir: MOVEMENT_RADICALISM_PASSED_REFORM_EFFECT + radicalismo de la cultura primaria de la nación + valor máximo de nacionalismo en cualquier provincia x definir: MOVEMENT_RADICALISM_NATIONALISM_FACTOR + definir: POPULATION_MOVEMENT_RADICAL_FACTOR x apoyo al movimiento / población no colonial de la nación.
- Cuando el valor de radicalismo para un movimiento alcanza los 100, las poblaciones son eliminadas del movimiento y se agregan a una facción rebelde. Esas poblaciones tienen su beligerancia aumentada a un mínimo de definir: MIL_ON_REB_MOVE. Consulta a continuación para determinar a qué facción rebelde se une la población.
- Los movimientos políticos y sociales se utilizan para calcular el apoyo a la reforma política/social en la nación, tomando la proporción de la población no colonial involucrada en movimientos políticos/sociales, respectivamente, y multiplicando por definir: MOVEMENT_SUPPORT_UH_FACTOR.

### Membresía

- Las poblaciones esclavas no pueden pertenecer a un movimiento.
- Las poblaciones con una beligerancia igual o mayor a definir: MIL_TO_JOIN_REBEL no pueden unirse a un movimiento.
- Las poblaciones pueden apoyar un movimiento (ya sea basado en un problema o en la independencia), y su apoyo se actualiza diariamente. Si el apoyo de la población al problema de un movimiento basado en un problema cae por debajo de definir: ISSUE_MOVEMENT_LEAVE_LIMIT, la población abandonará el movimiento. Si la beligerancia de la población cae por debajo de definir: NATIONALIST_MOVEMENT_MIL_CAP, la población abandonará un movimiento independentista. Si la población *no* es actualmente parte de un movimiento, no está en una provincia colonial, no es parte de una facción rebelde y tiene una conciencia de al menos 1.5 o una alfabetización de al menos 0.25, la población puede unirse a un movimiento.
- Si hay uno o más problemas que la población apoya al menos con definir: ISSUE_MOVEMENT_JOIN_LIMIT, entonces la población tiene la posibilidad de unirse a un movimiento basado en un problema con la probabilidad: apoyo al problema x 9 x definir: MOVEMENT_LIT_FACTOR x alfabetización de la población + apoyo al problema x 9 x definir: MOVEMENT_CON_FACTOR x conciencia de la población.
- Si no hay problemas válidos, la población tiene una beligerancia de al menos definir: NATIONALIST_MOVEMENT_MIL_CAP, no tiene la cultura primaria de la nación en la que se encuentra y sí tiene la cultura primaria de algún núcleo en su provincia, entonces tiene una posibilidad (¿20%?) de unirse a un movimiento independentista para dicho núcleo.
- Parece que las poblaciones que se unen/dejan un movimiento ocurren una vez al mes.

### Represión

- Cuando se reprime un movimiento:
- Reducir los puntos de represión para la nación por: si definir: POPULATION_SUPPRESSION_FACTOR es cero, radicalismo del movimiento + 1; de lo contrario, el mayor entre radicalismo del movimiento + 1 y radicalismo del movimiento x apoyo del movimiento / definir: POPULATION_SUPPRESSION_FACTOR, con un mínimo de cero.
- Aumentar el radicalismo transitorio del movimiento por: definir: SUPPRESSION_RADICALISM_HIT.
- Establecer la conciencia de todas las poblaciones que estaban en el movimiento a 1 y eliminarlas del mismo.

## Rebeldes

- Fracción rebelde: Hay un desencadenante que depende de la fracción de provincias controladas por rebeldes. Mis notas dicen que solo cuentan las provincias que no son de ultramar (o tal vez solo las conectadas a la capital) para calcular esta fracción.
- Las poblaciones con beligerancia >= definir: MIL_TO_JOIN_REBEL se unirán a una facción rebelde en lugar de un movimiento (y si la beligerancia de la población es menor que esto, se retirará).
- Elección de una facción para una población:
- Una población en una provincia asediada o controlada por rebeldes se unirá a esa facción, si la población es compatible con la facción.
- De lo contrario, toma todos los tipos de rebeldes compatibles y posibles. Determina la probabilidad de aparición para cada uno de ellos, tomando el *producto* de los modificadores. La población luego se une al tipo con la mayor probabilidad (así es, realmente no es una *probabilidad* en absoluto). Si ese tipo tiene un tipo de deserción, se une a la facción con la identidad nacional más compatible con él y ese tipo (los pan-nacionalistas van a la etiqueta de la unión, todos los demás utilizan la lógica que detallo a continuación).
- Compatibilidad de facciones: una población no se unirá a una facción de la que esté excluida según su cultura, grupo cultural, religión o ideología (aquí es la ideología dominante de la población la que importa). También hay alguna lógica para determinar si una población es compatible con una identidad nacional para la independencia. No creo que valga la pena intentar imitar la lógica del juego base aquí. En su lugar, iré con: la población no es de una cultura aceptada y su cultura principal está asociada con esa identidad *o* no hay núcleo en la provincia asociado con su identidad principal.
- Cuando una población se une a una facción, mis notas dicen que la organización de la facción aumenta ya sea por el número de divisiones que podrían generarse a partir de esa población (calculado directamente por el tamaño de la población / definir: POP_MIN_SIZE_FOR_REGIMENT) o tal vez algún múltiplo de eso.
- Una facción rebelde sin poblaciones en ella se disolverá.
- Las facciones rebeldes cuyo país de independencia existe se disolverán (¿diferente para los rebeldes de deserción?).
- Los pan-nacionalistas dentro de su país de la unión se disolverán.
- Cualquier población perteneciente a una facción rebelde que se disuelva tiene su beligerancia reducida a 0.
- Suma para cada población perteneciente a la facción que ha ganado dinero en el día: (ingreso de la población + 0.02) x alfabetización de la población x (10 si la beligerancia es > definir: MILITANCY_TO_AUTO_RISE y 5 si la beligerancia es menor que eso pero > definir: MILITANCY_TO_JOIN_RISING) / (1 + configuración de gastos de administración nacional). Toma esta suma, multiplícala por 0.001 x (organización rebelde de la tecnología + 1) y divídela por el número de regimientos que esas poblaciones podrían formar. Si es positivo, añádelo a la organización actual de la facción rebelde (con un máximo de 1). Esto parece hacerse diariamente.
- Los rebeldes tienen una oportunidad de levantarse una vez al mes. Si hay poblaciones pertenecientes a la facción con beligerancia mayor que definir: MILITANCY_TO_JOIN_RISING que pueden formar al menos un regimiento, entonces se realiza una comprobación aleatoria. La probabilidad de que ocurra el levantamiento es: organización de la facción x 0.05 + 0.02 + organización de la facción x número de regimientos que el levantamiento podría formar / 1v(número de regimientos controlados por la nación x 20).
- Cuando ocurre un levantamiento, las poblaciones con al menos definir: MILITANCY_TO_JOIN_RISING generarán la organización de la facción x máximo posible de regimientos respaldados, con un mínimo de 1 (si es posible más regimientos).
- Cualquier población que contribuya con regimientos tendrá su beligerancia reducida por definir: REDUCTION_AFTER_RISING.
- Cualquier estado de punto álgido donde se creen uno o más regimientos rebeldes tiene su tensión de punto álgido aumentada por definir: TENSION_ON_REVOLT, con un máximo de 100.
- La organización de la facción se reduce a 0 después de un levantamiento inicial (para levantamientos contributivos posteriores, puede reducirse en cambio por un factor de (número de regimientos adicionales x 0.01 + 1))

### Deserción de Provincia

Si, al primero del mes, una provincia no capital está controlada por rebeldes y no está siendo asediada en ese momento, y no hay ninguna batalla en curso en ella, y han pasado al menos tantos meses como se define en el tipo de rebelión, la provincia desertará si el tipo de rebelión lo permite. A dónde va esa deserción... es complicado (¿alguien conoce la lógica para esto?), y parece estar sesgado hacia naciones con núcleos en la provincia, y luego hacia naciones adyacentes en caso de que falten. Aunque las provincias capitales no desertan de esta manera, si una nación tiene solo un estado y una provincia desertara de esta manera, toda la nación será anexada al objetivo de deserción.

### Victoria Rebelde

Cada día, una facción rebelde activa tiene verificado su `demands_enforced_trigger`. Si gana, se ejecuta su `demands_enforced_effect`. Cualquier unidad para la facción que exista será destruida (o transferida si es uno de los tipos especiales de rebeldes). El tipo de gobierno de la nación cambiará si el tipo de rebelión tiene un gobierno asociado (con la misma lógica para un cambio de tipo de gobierno proveniente de un objetivo de guerra u otra causa). Luego, la cámara alta se establecerá de una manera especial (incluso si el tipo de gobierno no ha cambiado). Cómo se restablece la cámara alta es un poco complicado, así que el esquema que daré aquí asume que entiendes en general cómo funciona la cámara alta. Primero, verificamos las reglas relevantes: en "igual que el partido gobernante" la establecemos al 100% de la ideología del partido gobernante. Si es "voto estatal", calculamos el resultado de la cámara alta para cada estado no colonial como de costumbre (es decir, aplicamos los modificadores de votación de estratos estándar y ponderamos el voto por la cultura según lo requieren las reglas de votación), luego normalizamos cada resultado estatal y los sumamos. De lo contrario, estamos en votación directa, lo que significa que simplemente sumamos cada voto de la población (nuevamente, ponderando esos votos según las reglas y modificadores habituales). Si el tipo de rebelión tiene "romper alianzas al ganar", entonces la nación pierde todas sus alianzas, todos sus vasallos no subestatales, todos sus miembros en la esfera y pierde toda su influencia y tiene su nivel de influencia establecido en neutral. La nación pierde prestigio igual a definir: PRESTIGE_HIT_ON_BREAK_COUNTRY x (prestigio actual de la nación + prestigio permanente), que se multiplica por el modificador de prestigio de la nación proveniente de la tecnología + 1 como de costumbre (!).

### Cálculo de cuántos regimientos están "listos para unirse" a una facción rebelde

Recorre todas las poblaciones asociadas con la facción. Por cada población con malestar >= definir: MIL_TO_JOIN_RISING, se le atribuye a la facción un regimiento por cada definir: POP_SIZE_PER_REGIMENT que la población tenga en tamaño.

## Crimen

El crimen aparentemente es el único lugar donde importa el siguiente valor:
- eficiencia administrativa del estado: = definir: NONCORE_TAX_PENALTY x número-de-provincias-no-núcleo + (eficiencia-impositiva-de-burócratas x número-total-de-burócratas-de-cultura-primaria-o-aceptada / población-del-estado) v1 / x (suma-del-multiplicador-administrativo-para-temas-sociales-marcados-como-administrativos x definir: BUREAUCRACY_PERCENTAGE_INCREMENT + definir: MAX_BUREAUCRACY_PERCENTAGE)), todo limitado entre 0 y 1.
La percentual de lucha contra el crimen de una provincia se calcula entonces como: (eficiencia-de-administración-del-estado x definir: ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT + configuración-de-gasto-en-administración x (1 - ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT)) x (definir: MAX_CRIMEFIGHTING_PERCENT - definir: MIN_CRIMEFIGHTING_PERCENT) + definir: MIN_CRIMEFIGHTING_PERCENT

Una vez al mes (el día 1), se actualizan los crímenes de la provincia. Si la provincia tiene un crimen, el porcentaje de lucha contra el crimen es la probabilidad de que ese crimen sea eliminado. Si no hay crimen, el porcentaje de lucha contra el crimen es la probabilidad de que permanezca libre de crimen. Si se agrega un crimen a la provincia, se selecciona al azar (con igual probabilidad) de entre los crímenes posibles para la provincia (determinados por la activación del crimen y el cumplimiento de su condición).

## Tecnología

### Puntos de investigación diarios

Deja que la suma de poblaciones (pop-sum) sea para cada tipo de población (puntos-de-investigación-por-tipo x 1^(fracción de población / fracción óptima)). Luego, los puntos de investigación diarios ganados por una nación son: (modificador-nacional-modificador-de-puntos-de-investigación + modificador-de-investigación-tecnológica + 1) x (modificador-nacional-a-puntos-de-investigación) + pop-sum)

Si una nación no está investigando actualmente una tecnología (o es una nación no civilizada), los puntos de investigación se acumularán, hasta un total de 365 x puntos de investigación diarios, para naciones civilizadas, o definir: MAX_RESEARCH_POINTS para naciones no civilizadas.

Si una nación está investigando una tecnología, se puede depositar un máximo de definir: MAX_DAILY_RESEARCH puntos en ella por día.

También existe un truco de investigación instantánea, pero no veo razón para incluirlo en el juego.

### Costo

La cantidad efectiva de puntos de investigación que cuesta una tecnología = costo-base x 0^(1 - (año-actual - año-disponibilidad-de-la-tecnología) / definir: TECH_YEAR_SPAN) x definir: TECH_FACTOR_VASSAL (si tu señor tiene la tecnología) / (1 + modificador-de-investigación-de-categoría-de-tecnología)

### Inventos

Los inventos tienen la posibilidad de descubrirse el 1 de cada mes. El modificador de probabilidad de descubrimiento de inventos se calcula de manera aditiva, y el resultado es la probabilidad de 100 de que se descubra el invento. Cuando se descubre un invento con prestigio compartido, el descubridor gana esa cantidad de prestigio compartido / el número de veces que se ha descubierto (incluyendo la vez actual).

## Política

### Lealtad partidista

Cada provincia tiene un valor de lealtad partidista (aunque es por ideología, no por partido, así que sería más apropiado llamarlo lealtad ideológica... lo que sea). Si el enfoque nacional está presente en el estado para la lealtad partidista, la lealtad en cada provincia en ese estado para esa ideología aumentará según el valor de lealtad del enfoque (diariamente).

### Cuándo es posible una reforma social/política

Estas solo están disponibles para naciones civilizadas. Si es "solo el próximo paso", debe estar en efecto la opción de problema anterior o siguiente. Deben haber pasado al menos definir: MIN_DELAY_BETWEEN_REFORMS meses desde el último cambio de opción de problema (para cualquier tipo de problema). Y su desencadenador `allow` debe cumplirse. Luego, para cada ideología, probamos su `add_social_reform` o `remove_social_reform` (dependiendo de si estamos aumentando o disminuyendo, y sustituyendo `political_reform` aquí según sea necesario), calculando su modificador de manera aditiva, y luego sumando el resultado multiplicado por la fracción de la cámara alta que la ideología tiene a un total en ejecución. Si el total en ejecución es > 0.5, se puede implementar la opción de problema.

### Cuándo es posible una reforma económica/militar

Estas solo están disponibles para naciones incivilizadas. Algunas de las reglas son las mismas que para las reformas sociales/políticas: si es "solo el próximo paso", debe estar en efecto la opción de reforma anterior o siguiente. Deben haber pasado al menos definir: MIN_DELAY_BETWEEN_REFORMS meses desde el último cambio de opción de reforma (para cualquier tipo de reforma). Y su desencadenador `allow` debe cumplirse. Donde las cosas son diferentes: cada reforma también tiene un costo en puntos de investigación. Sin embargo, este costo puede variar. El costo real que debes pagar se multiplica por lo que llamo el "factor de reforma" + 1. El factor de reforma es (suma de ideología en la cámara alta x modificador desencadenado por agregar reforma) x define: ECONOMIC_REFORM_UH_FACTOR + el modificador económico/militar de autoincivilizado de la nación + el modificador económico/militar de autoincivilizado de la nación a la que está en la esfera (si la hay).

### Peso del voto de la población

Dado que esto se utiliza en un par de lugares, es más fácil convertirlo en su propia regla: Cuando los "votos" de una población aparecen en cualquier forma, el peso de ese voto es el producto del tamaño de la población y del modificador nacional para votar por su estrato (esto podría dar como resultado que una estrata no tenga votos). Si la nación tiene establecido el voto de la cultura primaria, las poblaciones de cultura primaria obtienen un voto completo, las poblaciones de cultura aceptada obtienen medio voto y las poblaciones de otras culturas no obtienen ningún voto. Si hay votación por cultura, las poblaciones de cultura primaria y aceptada obtienen un voto completo y nadie más recibe un voto. Si ninguno está establecido, todas las poblaciones obtienen un voto igual.

### Actualizaciones anuales de la cámara alta

Cada año, la cámara alta de cada nación se actualiza. Si la regla es "igual que el partido gobernante", la cámara alta se convierte en un 100% de la ideología del partido gobernante. Si la regla es "voto estatal", entonces para cada estado no colonial: para cada población en el estado que no está impedida de votar por su tipo, distribuimos su voto ponderado proporcionalmente a su apoyo ideológico, dándonos una distribución de ideologías para cada uno de esos estados. Las distribuciones de ideologías estatales se normalizan y suman para formar la distribución de la cámara alta. Para "voto poblacional" y "solo ricos", el peso del voto de cada población no colonial (o solo los ricos para "solo ricos") se distribuye proporcionalmente a su apoyo ideológico, y la suma de todos los votos elegibles forma la distribución de la cámara alta.

### Elecciones

Un país con elecciones realiza una cada 5 años.

Las elecciones duran define: CAMPAIGN_DURATION meses.

Para determinar el resultado de las elecciones, debemos hacer lo siguiente:
- Determinar el voto en cada provincia. Ten en cuenta que la votación es *por partido activo* y no por ideología.
- Las provincias en estados coloniales no tienen derecho a votar.
- Para cada partido hacemos lo siguiente: averiguamos el apoyo ideológico de la población para el partido y su apoyo a los problemas del partido (sumando su apoyo para cada problema que el partido haya establecido, excepto que las poblaciones de culturas no aceptadas nunca apoyarán a partidos con votaciones culturales más restrictivas). Luego, la población vota por el partido (es decir, contribuye con su peso de voto en apoyo) basándose en la suma de su apoyo a problemas y apoyo ideológico, excepto que cuanto mayor sea la conciencia de la población, más se basará su voto en el apoyo ideológico (las poblaciones con 0 conciencia votan solo basándose en problemas). Desconozco la proporción exacta (¿a alguien le importa si no se utiliza la proporción exacta?). El apoyo al partido se multiplica luego por (modificador-provincial-apoyo-partido-gobernante + modificador-nacional-apoyo-partido-gobernante + 1), si es el partido gobernante, y por (1 + lealtad-de-partido-de-la-provincia) para su ideología.
- Los votos de la población también se multiplican por (modificador-provincial-numero-de-votantes + 1)
- Después de que se haya realizado la votación en cada provincia, el partido ganador allí aumenta la lealtad ideológica de la provincia para su ideología en define: LOYALTY_BOOST_ON_PARTY_WIN x (modificador-provincial-fortalecer-partido-mas-fuerte + 1) x fracción-de-voto-para-el-partido-ganador.
- Si la regla de votación "mayor porción" está en efecto: todos los votos se suman a la suma hacia el partido que recibió la mayoría de votos en la provincia. Si es "dhont", entonces los votos en cada provincia se normalizan al número de votos de la provincia y para "sainte_laque" los votos de las provincias simplemente se suman.

Lo que sucede con el resultado de las elecciones depende en parte de la militancia promedio de la nación. Si es menor que 5: encontramos el grupo ideológico con el mayor apoyo (cada partido activo da su apoyo a su grupo), luego el partido con el mayor apoyo del grupo ganador es elegido. Si la militancia promedio es mayor que 5: el partido con el mayor apoyo individual gana. (Nota: así que en una militancia promedio menor que 5, tener partidos con ideologías duplicadas hace que un grupo ideológico tenga muchas más posibilidades de ganar, porque se cuentan doble).

### Eventos de Elecciones

Mientras una elección está en curso, se activarán eventos aleatorios de la categoría `on_election_tick`. Estos eventos se eligen de la manera habitual: según sus pesos de entre aquellos que tienen sus condiciones de activación (si las hay) satisfechas. Los eventos ocurren una vez cada define: CAMPAIGN_EVENT_BASE_TIME / 2 días a define: CAMPAIGN_EVENT_BASE_TIME días (distribuidos uniformemente).

## Puntuaciones y clasificación

### Puntuación Industrial

Es la suma de los siguientes dos componentes:
- Para cada estado: (fracción de trabajadores de fábricas en cada estado (tipos marcados con "puede trabajar en fábrica = sí") respecto a la fuerza laboral total x nivel de construcción de fábricas en el estado (limitado a 1)) x total de niveles de fábricas
- Para cada país en el que la nación tiene inversiones: define: INVESTMENT_SCORE_FACTOR x la cantidad invertida x 0.01

### Puntuación Militar

La puntuación militar es en sí misma una suma complicada:

La primera parte es lo suficientemente complicada como para que voy a simplificar las cosas ligeramente, e ignorar cómo la movilización puede interactuar con esto:
Primero, necesitamos conocer el número total de regimientos reclutables.
También necesitamos conocer la puntuación promedio de las unidades terrestres, que definimos aquí como (ataque + defensa + modificador nacional de ataque terrestre + modificador nacional de defensa terrestre) x disciplina
Luego tomamos el menor entre el número de regimientos en el campo x 4 o el número de regimientos reclutables y lo multiplicamos por define: DISARMAMENT_ARMY_HIT (si está desarmado), multiplicamos eso por la puntuación promedio de las unidades terrestres, multiplicamos nuevamente por (modificador nacional para el consumo de suministros + 1), y luego dividimos por 7.

A eso le sumamos, para cada nave capital: (puntos de casco + modificador nacional de defensa naval) x (potencia de fuego + modificador nacional de ataque naval) / 250

Y luego sumamos un punto ya sea por líder o por regimiento, lo que sea mayor.

## Esferas y Grandes Potencias

### Grandes potencias

Una nación incivilizada o una nación con un solo estado no puede ser una gran potencia.

Cuando una nación se convierte en una gran potencia, deja de ser un vasallo o un estado subordinado. También recibe un evento `on_new_great_nation`.

Cuando una nación pierde su estatus de gran potencia, recibe un evento `on_lost_great_nation`.

Una nación que cae fuera de las primeras 8 no deja de ser una gran potencia hasta que hayan pasado define: GREATNESS_DAYS desde la última vez que fue una gran potencia *y* no está actualmente involucrada en una guerra de crisis, o una guerra que podría potencialmente ser una gran guerra (si estuvieran habilitadas), *y* no está involucrada en una crisis ni como defensora ni como agresora.

### Influencia

Una nación no acumula influencia si: su embajada ha sido prohibida, si está en guerra contra la nación, si tiene un alto el fuego con la nación, si la nación tiene prioridad 0 o si su influencia está limitada (al valor máximo sin que ninguna otra Gran Potencia influya).

La nación obtiene un aumento diario de definir: BASE_GREATPOWER_DAILY_INFLUENCE x (modificador-nacional-para-la-ganancia-de-influencia + 1) x (modificador-tecnológico-para-la-influencia + 1). Luego se divide entre las naciones con las que están acumulando influencia en proporción a su prioridad (por lo que un objetivo con prioridad 2 recibe 2 partes en lugar de 1, etc.).

Este valor de influencia no se traduce directamente en influencia con la nación objetivo. En cambio, primero se multiplica por el siguiente factor:
1 + definir: DISCREDIT_INFLUENCE_GAIN_FACTOR (si está desacreditado) + definir: NEIGHBOUR_BONUS_INFLUENCE_PERCENT (si las naciones son adyacentes) + definir: SPHERE_NEIGHBOUR_BONUS_INFLUENCE_PERCENT (si algún miembro de la esfera de la nación influyente es adyacente pero no la nación influyente en sí) + definir: OTHER_CONTINENT_BONUS_INFLUENCE_PERCENT (si la nación influyente y el objetivo tienen capitales en continentes diferentes) + definir: PUPPET_BONUS_INFLUENCE_PERCENT (si el objetivo es un vasallo del influenciador) + valor-relación / definir: RELATION_INFLUENCE_MODIFIER + definir: INVESTMENT_INFLUENCE_DEFENCE x fracción-de-inversiones-extranjeras-del-influenciador-fuera-del-total-de-inversiones-extranjeras + definir: LARGE_POPULATION_INFLUENCE_PENALTY x población-del-objetivo / definir: LARGE_POPULATION_INFLUENCE_PENALTY_CHUNK (si la nación objetivo tiene una población mayor que definir: LARGE_POPULATION_LIMIT) + (1 - puntuación-del-objetivo / puntuación-del-influenciador)^0

Ten en cuenta que no puedes ganar influencia con una nación mientras tu embajada esté prohibida, pero aún así parece que "gastas" influencia de todos modos. Cualquier influencia que se acumule más allá del máximo (definir: MAX_INFLUENCE) se restará de la influencia de la Gran Potencia con más influencia (que no sea la nación influyente).

Reglas generales para acciones de influencia: no puedes realizar una acción de influencia cuando la nación influenciada está en guerra en tu contra o si tu embajada ha sido prohibida. Y, obviamente, debes tener suficientes puntos de influencia para pagar el costo.

Desacreditación:

Una nación es desacreditada durante definir: DISCREDIT_DAYS. Ser desacreditado dos veces no suma estas duraciones; simplemente reinicia el temporizador desde el día actual. Desacreditar a una nación "aumenta" tu relación con ellos en definir: DISCREDIT_RELATION_ON_ACCEPT. Desacreditar tiene un costo de definir: DISCREDIT_INFLUENCE_COST puntos de influencia. Para desacreditar a una nación, debes tener una opinión de al menos "opuesta" con la nación influenciada y debes tener un nivel de opinión igual o mejor con la nación influenciada que la nación que estás desacreditando.

Expulsar asesores:

Expulsar asesores de una nación "aumenta" tu relación con ellos en definir: EXPELADVISORS_RELATION_ON_ACCEPT. Esta acción tiene un costo de definir: EXPELADVISORS_INFLUENCE_COST puntos de influencia. Para expulsar a los asesores de una nación, debes tener un nivel de opinión igual o mejor con la nación influenciada que la nación a la que le estás haciendo la acción. Ser expulsado cancela cualquier efecto de desacreditación en curso. Ser expulsado reduce tu influencia a cero. Para expulsar asesores, debes tener al menos una opinión neutral con la nación influenciada y un nivel de opinión igual o mejor que el de la nación que estás expulsando.

Prohibir embajada:

Prohibir la embajada de una nación "aumenta" tu relación con ellos en definir: BANEMBASSY_RELATION_ON_ACCEPT. Esta acción tiene un costo de definir: BANEMBASSY_INFLUENCE_COST puntos de influencia. El efecto de prohibir la embajada dura definir: BANEMBASSY_DAYS. Si ya estás prohibido, ser prohibido nuevamente simplemente reinicia el temporizador. Ser prohibido cancela cualquier efecto de desacreditación en curso. Para prohibir a una nación, debes tener al menos una relación amistosa con la nación influenciada y un nivel de opinión igual o mejor que el de la nación que estás expulsando.

Aumentar opinión:

Aumentar la opinión de una nación tiene un costo de definir: INCREASEOPINION_INFLUENCE_COST puntos de influencia. La opinión puede aumentarse hasta un máximo de amistosa.

Disminuir opinión:

Disminuir la opinión de una nación "aumenta" tu relación con ellos en definir: DECREASEOPINION_RELATION_ON_ACCEPT. Esta acción tiene un costo de definir: DECREASEOPINION_INFLUENCE_COST puntos de influencia. Disminuir la opinión de otra nación requiere que tengas una opinión de al menos "opuesto" con la nación influenciada y debes tener un nivel de opinión igual o mejor con la nación influenciada que la nación a la que estás disminuyendo su opinión.

Agregar a la esfera:

Agregar una nación a tu esfera tiene un costo de definir: ADDTOSPHERE_INFLUENCE_COST puntos de influencia, y la nación debe tener una opinión amistosa de ti.

Quitar de la esfera:

Quitar una nación de una esfera tiene un costo de definir: REMOVEFROMSPHERE_INFLUENCE_COST puntos de influencia. Si quitas una nación de tu propia esfera, pierdes definir: REMOVEFROMSPHERE_PRESTIGE_COST prestigio y ganas definir: REMOVEFROMSPHERE_INFAMY_COST infamia. Quitar una nación de la esfera de otra nación "aumenta" tu relación con el líder anterior de la esfera en definir: REMOVEFROMSPHERE_RELATION_ON_ACCEPT puntos. Para realizar esta acción, debes tener un nivel de opinión de al menos amistoso con la nación a la que estás quitando de la esfera. (La nación eliminada luego se vuelve amistosa con su antiguo líder de la esfera).

## Mecánicas de Crisis

### Tensión de Punto Crítico

El enfoque de punto crítico solo está disponible para naciones que no son grandes potencias ni potencias secundarias, y debe colocarse en un estado que ya tenga un punto crítico (en alguna provincia). Los estados que forman parte de un subestado (es decir, el tipo especial de vasallo) o los estados que contienen la capital de una nación no pueden tener puntos críticos ni acumular tensión.

Si un estado contiene un punto crítico depende de: si una provincia en el estado tiene un núcleo que no sea el de su propietario y de una etiqueta marcada como liberable (es decir, no es un núcleo de una unión cultural) y que tenga una cultura principal que no sea la cultura principal o una cultura aceptada de su propietario. Si algún núcleo califica, se considera que el estado es un punto crítico, y su etiqueta de punto crítico predeterminada será el núcleo calificado cuya cultura tenga la mayor población en el estado. (Sin embargo, si una nación coloca un enfoque de punto crítico en el estado, la etiqueta del punto crítico cambiará a la de la nación que colocó el enfoque).

- Si al menos una nación tiene una CB en el propietario de un estado de punto crítico, la tensión aumenta en definir:TENSION_FROM_CB por día.
- Si hay un movimiento de independencia dentro de la nación dueña del estado para la etiqueta de independencia, la tensión aumentará por radicalismo del movimiento x definir:TENSION_FROM_MOVEMENT x fracción-de-población-en-estado-con-la-misma-cultura-que-la-etiqueta-de-independencia x apoyo-del-movimiento / 4000, hasta un máximo de definir:TENSION_FROM_MOVEMENT_MAX por día.
- Cualquier enfoque de punto crítico aumenta la tensión por la cantidad indicada en él por día.
- La tensión aumenta en definir:TENSION_DECAY por día (esto es negativo, por lo que en realidad es una disminución diaria).
- La tensión aumenta en definir:TENSION_WHILE_CRISIS por día mientras haya una crisis en curso.
- Si el estado es propiedad de una gran potencia, la tensión aumenta en definir:RANK_X_TENSION_DECAY por día.
- Por cada gran potencia en guerra o desarmada en el mismo continente que el propietario o el estado, la tensión aumenta en definir:AT_WAR_TENSION_DECAY por día.
- La tensión varía entre 0 y 100.

### Iniciando una Crisis

Una crisis puede no comenzar hasta que hayan pasado definir: CRISIS_COOLDOWN_MONTHS meses después de que haya terminado la última crisis o guerra de crisis. Cuando una crisis se vuelve posible, primero verificamos cada uno de los tres estados con la mayor tensión > 50 donde ni el propietario del estado ni la nación asociada al punto álgido (si la hay) estén en guerra. Creo que la probabilidad de que ocurra una crisis en cualquiera de esos estados es de 0.001 x definir: CRISIS_BASE_CHANCE x tensión del estado / 100. Si esto se convierte en una crisis, la tensión en el estado se reduce inmediatamente a cero. De lo contrario, cualquier región colonial en disputa donde ninguno de los colonizadores esté en guerra se convertirá en una crisis y la "temperatura" colonial haya alcanzado los 100.

### Grandes Potencias Interesadas

Una Gran Potencia que no esté desarmada y no esté en guerra con un agotamiento de guerra menor a definir: CRISIS_INTEREST_WAR_EXHAUSTION_LIMIT y esté en el mismo continente que el objetivo de la crisis, el atacante de la crisis o el estado (o con algunos casos especiales: creo que las Grandes Potencias europeas están interesados en todo, y creo que América del Norte y del Sur se consideran un solo continente aquí) se considera interesado. Cuando una Gran Potencia se vuelve interesada, recibe un evento `on_crisis_declare_interest`.

### Temperatura de la Crisis

Cada día en el que haya al menos una Gran Potencia defensora y una Gran Potencia atacante, la temperatura de la crisis aumenta en definir: CRISIS_TEMPERATURE_INCREASE x definir: CRISIS_TEMPERATURE_PARTICIPANT_FACTOR x la proporción de Grandes Potencias actualmente involucradas en la crisis respecto al número total de Grandes Potencias interesadas y participantes.

### Resolución de la Crisis

La nación que controla el estado que fue el objetivo de la crisis perderá toda la tensión en cualquiera de sus estados de puntos álgidos al final de la crisis (independientemente de cómo se resuelva).
Si la crisis se convierte en una guerra, cualquier Gran Potencia interesada que no haya tomado partido pierde (años después de la fecha de inicio x definir: CRISIS_DID_NOT_TAKE_SIDE_PRESTIGE_FACTOR_YEAR + definir: CRISIS_DID_NOT_TAKE_SIDE_PRESTIGE_FACTOR_BASE) como fracción de su prestigio.
Si la crisis falla con un respaldo de una Gran Potencia pero sin atacantes, el único respaldador obtiene prestigio adicional igual a definir: LONE_BACKER_PRESTIGE_FACTOR como fracción de su prestigio.

## Colonización

Puedes invertir en colonización en una región si hay menos de 4 colonizadores allí (o ya tienes un colonizador allí). También debes tener suficiente tecnología de calificación de vida. Específicamente, debes tener puntos de calificación de vida colonial provenientes de la tecnología + definir: COLONIAL_LIFERATING menor o igual al valor de calificación de vida *más alto* de una provincia no poseída en el estado. Tu país debe ser de definir: COLONIAL_RANK o menos. El estado no puede ser el objetivo actual de una crisis, ni tu país puede estar involucrado en una guerra de crisis activa.

Si aún no has enviado un colonizador a la región, debes estar dentro del alcance de la región. Cualquier región adyacente a tu país o a uno de tus vasallos o subestados se considera dentro del alcance. De lo contrario, debe estar dentro del alcance de una de tus bases navales, con el alcance dependiendo del valor de alcance colonial proporcionado por la construcción de la base naval x el nivel de la base naval.

Si has enviado un colonizador a la región, y la colonización está en la fase 1 o 2, puedes invertir si ha pasado al menos definir: COLONIZATION_DAYS_BETWEEN_INVESTMENT desde tu última inversión, tienes suficientes puntos coloniales y el estado permanece dentro del alcance.

Si has enviado un colonizador a una región y pasan al menos definir: COLONIZATION_DAYS_FOR_INITIAL_INVESTMENT sin ningún otro colonizador, pasa a la fase 3 con definir: COLONIZATION_INTEREST_LEAD puntos de interés.

Si obtienes definir: COLONIZATION_INTEREST_LEAD puntos, y está en la fase 1, pasa a la fase 2, expulsando a todos menos al segundo colonizador con más puntos.

En la fase 2, si obtienes definir: COLONIZATION_INFLUENCE_LEAD puntos por delante del otro colonizador, el otro colonizador es expulsado y la fase avanza a 3.

En la fase 2, si hay colonizadores competidores, la "temperatura" en la colonia aumentará por definir: COLONIAL_INFLUENCE_TEMP_PER_DAY + puntos-máximos-invertidos x definir: COLONIAL_INFLUENCE_TEMP_PER_LEVEL + definir: TENSION_WHILE_CRISIS (si hay alguna otra crisis en curso) + definir: AT_WAR_TENSION_DECAY (si alguno de los dos colonizadores está en guerra o desarmado).

Para finalizar la colonización y crear un protectorado: debes estar en la fase 3 de colonización, debes tener puntos coloniales libres igual a definir: COLONIZATION_CREATE_PROTECTORATE_COST, y tu colonizador en la región debe tener puntos no nulos.

Si dejas una colonia en la fase 3 durante definir: COLONIZATION_MONTHS_TO_COLONIZE meses, la colonización se reiniciará a la fase 0 (sin colonización en progreso).

Para convertir un protectorado en una colonia, debes tener puntos coloniales libres iguales a definir: COLONIZATION_CREATE_COLONY_COST x número-de-provincias-en-el-estado.

Para convertir una colonia en un estado regular, debe tener suficientes burócratas con tu cultura principal para constituir una fracción de la población igual a definir: STATE_CREATION_ADMIN_LIMIT. Si la colonia tiene una conexión directa por tierra con tu capital, esto no requiere puntos coloniales libres. De lo contrario, también debes tener definir: COLONIZATION_CREATE_STATE_COST x número-de-provincias-en-el-estado x 1v(distancia-desde-la-capital/definir: COLONIZATION_COLONY_STATE_DISTANCE) puntos libres.

### Puntos

Solo las naciones con rango al menos definir: COLONIAL_RANK obtienen puntos coloniales. Los puntos coloniales provienen de tres fuentes:
- Bases navales: (1) determinadas por el nivel y la definición del edificio, excepto que solo obtienes definir: COLONIAL_POINTS_FOR_NON_CORE_BASE (una tarifa fija) por bases navales que no están en una provincia central y no están conectadas por tierra a la capital. (2) multiplica ese resultado por definir: COLONIAL_POINTS_FROM_SUPPLY_FACTOR (NOTA personal: verificar esto nuevamente).
- Unidades: los puntos coloniales que otorgan x (1.0 - la fracción en que el consumo de suministro naval de la nación supera el proporcionado por sus bases navales) x definir: COLONIAL_POINTS_FROM_SUPPLY_FACTOR
- Puntos de tecnologías/inventos

En general, los puntos coloniales "gastados" se vinculan a la colonia y se devuelven cuando deja de ser una colonia (la excepción es el "costo" de puntos para convertir una colonia en un estado, que es simplemente un requisito para tener un cierto número de puntos coloniales sin usar). Para cualquier estado que esté en proceso de convertirse en una colonia, cualquier punto gastado allí se mantiene bloqueado hasta que termine el proceso, de una forma u otra. Cada provincia en un estado protectorado cuesta definir: COLONIZATION_PROTECTORATE_PROVINCE_MAINTAINANCE puntos. Cada provincia en un estado de colonia cuesta definir: COLONIZATION_COLONY_PROVINCE_MAINTAINANCE + valor-de-infraestructura-proporcionado-por-ferrocarriles x nivel-de-ferrocarril-en-la-provincia x definir: COLONIZATION_COLONY_RAILWAY_MAINTAINANCE. Además, un estado de colonia cuesta definir: COLONIZATION_COLONY_INDUSTRY_MAINTAINANCE x el-número-de-fábricas.

Invertir en una colonia tiene un costo de definir: COLONIZATION_INVEST_COST_INITIAL + definir: COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (si se posee una provincia adyacente a la región) para colocar al colono inicial. Los pasos adicionales cuestan definir: COLONIZATION_INTEREST_COST mientras estás en la fase 1. En la fase dos, cada punto de inversión cuesta definir: COLONIZATION_INFLUENCE_COST hasta el cuarto punto. Después de alcanzar el cuarto punto, los puntos adicionales cuestan definir: COLONIZATION_EXTRA_GUARD_COST x (puntos - 4) + definir: COLONIZATION_INFLUENCE_COST.

Nuevas notas sobre puntos coloniales:

Debes tener al menos rango colonial para obtener alguno; suma igual de:
+ Puntos coloniales de tecnología
+ Puntos coloniales de la marina = 
(puntos coloniales nominales de barcos x (1.0 - porcentaje sobre los límites navales) + puntos coloniales de bases navales) x definir: COLONIAL_POINTS_FROM_SUPPLY_FACTOR
+ Puntos coloniales de archivos históricos (ignoramos esto)

Puntos coloniales utilizados:

+ Cantidad total invertida en colonización (etapa de carrera, no estados coloniales)
+ Por cada provincia colonial, COLONIZATION_COLONY_PROVINCE_MAINTAINANCE
+ Valor de infraestructura de la provincia x COLONIZATION_COLONY_RAILWAY_MAINTAINANCE
+ COLONIZATION_COLONY_INDUSTRY_MAINTAINANCE por fábrica en una colonia (???) 
redondeado al número entero más cercano.

## Eventos

### Pulso anual y trimestral

No sé si algún mod los utiliza (asumo que no, en este momento), pero un país experimentará un evento de pulso trimestral cada 1 a 90 días (distribuidos de manera uniforme en ese rango) y eventos de pulso anuales cada 1 a 365 días (también distribuidos uniformemente). Sí, esto significa que, en promedio, verías más de 4 eventos trimestrales por año y más de 1 evento anual por año. Mira, yo no hago las reglas, solo las informo.

### Eventos aleatorios

Algunos eventos están designados para poder activarse como máximo una vez. Obviamente, esto significa que pueden ocurrir como máximo una vez. También ten en cuenta que algunos eventos tienen un efecto inmediato, que ocurre cuando se desencadena el evento, antes de que se pueda elegir cualquier opción.

Qué eventos son posibles parece ser probado con menos frecuencia que la prueba de la ocurrencia de un evento.

Los eventos de provincia y los eventos nacionales se verifican en días alternos.

Para eventos nacionales: el factor base (ajustado a días) se multiplica con todos los modificadores que se aplican. Si el valor no es positivo, tomamos la probabilidad de que ocurra el evento como 0.000001. Si el valor es menor que 0.001, el evento está garantizado de ocurrir. De lo contrario, la probabilidad es el inverso multiplicativo del valor.

Las probabilidades para eventos de provincia se calculan de la misma manera, excepto que tienen el doble de probabilidad de ocurrir.

## Otros conceptos

- Prestigio: generalmente, las ganancias positivas de prestigio se aumentan por un factor de (modificador de tecnología de prestigio + 1).
- Prestigio: una nación con un modificador de prestigio obtiene esa cantidad de prestigio por mes (el día 1).
- Infamia: una nación con un modificador de chico malo obtiene esa cantidad de infamia por mes.
- Agotamiento de guerra: una nación con un modificador de agotamiento de guerra obtiene esa cantidad al comienzo del mes, y cada mes su agotamiento de guerra se limita al máximo modificador de agotamiento de guerra.
- Ajuste mensual de relaciones = +0.25 para súbditos/señores feudales, -0.01 por estar en guerra, +0.05 si son adyacentes y ambos están en paz, +0.025 por tener acceso militar, -0.15 por poder usar una CB en contra del otro (-0.30 si es mutuo).
- Una vez que las relaciones alcanzan los 100, los aumentos mensuales no pueden llevarlas más allá.
- Excepción: los señores feudales y los vasallos IA aumentan su relación en +2 al mes hasta llegar a 200.
- Aumento mensual de pluralidad: la pluralidad aumenta por conciencia promedio / 45 por mes.
- Puntos diplomáticos mensuales: (1 + modificador nacional de puntos diplomáticos + puntos diplomáticos de la tecnología) x define: BASE_MONTHLY_DIPLOPOINTS (con un máximo de 9).
- Ganancia mensual de puntos de represión: define: SUPPRESS_BUREAUCRAT_FACTOR x fracción de población que son burócratas x define: SUPPRESSION_POINTS_GAIN_BASE x (puntos de represión de la tecnología + modificador nacional de puntos de represión + 1) (con un máximo de define: MAX_SUPPRESSION).
- Provincia en ultramar: no está en el mismo continente que la capital de la nación Y no está conectada por tierra a la capital.
- Tipo de valor de provincia A: 1 (si no está en propiedad o es colonial), de lo contrario: (1 + nivel del fuerte + nivel de la base naval + número de fábricas en el estado (si es la provincia capital del estado)) x 2 (si es núcleo no ultramar) x 3 (si es la capital nacional) - estos valores se utilizan para determinar el valor relativo de las provincias (creo) en proporción al valor total de tipo A de todas las provincias propiedad del país.
- Un puerto: creo que cualquier provincia costera se considera que tiene un puerto, independientemente de si tiene una base naval.
- Revanchismo: obtienes un punto por cada núcleo no poseído si tu cultura principal es la cultura dominante (cultura con más población) en la provincia, 0.25 puntos si no es la cultura dominante, y luego ese total se divide por el número total de tus núcleos para obtener tu porcentaje de revanchismo.
- Eficiencia administrativa nacional: = (modificador de eficiencia administrativa nacional de la nación + modificador de eficiencia de tecnologías + 1) x número de población burocrática no colonial / (población no colonial total x (suma del multiplicador administrativo para problemas sociales marcados como administrativos x define: BUREAUCRACY_PERCENTAGE_INCREMENT + define: MAX_BUREAUCRACY_PERCENTAGE))
- Eficiencia arancelaria: define: BASE_TARIFF_EFFICIENCY + modificador nacional de eficiencia arancelaria + eficiencia administrativa, limitada a un máximo de 1.0.
- Número de enfoques nacionales: el menor entre la población total aceptada y de cultura principal / define: NATIONAL_FOCUS_DIVIDER y 1 + el número de enfoques nacionales proporcionados por la tecnología.
- El nacionalismo de la provincia disminuye en 0.083 (1/12) por mes.
- Nacionalización (puede ser desencadenada por un efecto): Todas las naciones pierden su inversión extranjera en una nación. Cada nación con inversión extranjera recibe un evento aleatorio de `on_my_factories_nationalized` con el país que realiza la nacionalización en la ranura "from".

## Seguimiento de cambios

Existen algunos desencadenantes, reglas de juego y elementos de interfaz de usuario que necesitan conocer el cambio de ciertos elementos, como el crecimiento de la población, la cantidad de migración, cambios en la riqueza de la población quizás, etc. Tendremos que decidir si estos elementos se registran ya sea restableciendo algún valor a cero al comienzo del día y luego aumentándolo durante el día, o asegurándonos de que todos los cambios ocurran en un solo lugar, de modo que el nuevo delta se pueda escribir una vez (en lugar de escribir cero y luego agregar actualizaciones a medida que avanzamos). La segunda opción es mejor para la estabilidad de la interfaz de usuario, ya que de lo contrario, leer dicho valor a lo largo de un intervalo de tiempo corre el riesgo de no solo proporcionar un valor desactualizado, sino uno incorrecto. También tendremos que decidir si almacenamos estos valores delta en el archivo de guardado. Si no lo hacemos, hay dos consecuencias: (a) no habrá información al respecto en la interfaz de usuario el primer día después de cargar, y (b) tendremos que asegurarnos de que siempre se generen antes de ser necesarios para cualquier regla; no podemos recurrir a usar el valor de cambio del día anterior, ya que puede que no haya un día anterior.

### Caminos y conectividad

Puede ser conveniente crear algunas estructuras que sigan las propiedades del trazado/conectividad. Algunas propiedades, como estar "en el extranjero", requieren en parte la capacidad de verificar si una provincia está conectada a otra. Una forma de hacer esto sería mediante el "llenado de inundación" de provincias conectadas con identificadores crecientes hasta que no haya más provincias terrestres. Luego, el mismo ID => conectado.

### Otros valores a seguir (normalmente necesitamos almacenar estos para evaluar una o más condiciones de desencadenantes)

- Número de puertos
- Número de puertos conectados a la capital
- Número de provincias en el extranjero
- Número de provincias ocupadas por rebeldes
- Fracción de provincias con crimen
- Si hay dos naciones adyacentes (es decir, una provincia propiedad de una está adyacente a una provincia propiedad de la otra)
- Si hay dos naciones y cada una tiene una provincia colonial adyacente a una provincia colonial de la otra
- Cuántos regimientos totales podrían reclutarse y cuántos realmente se han reclutado
- Número total de aliados, vasallos, subestados, etc.

## Comandos

Esta sección describe todas las acciones que un jugador puede tomar que afectan el estado del juego (es decir, cualquier cosa que no sea simplemente navegación en la interfaz de usuario).

### Establecer enfoque nacional

Esto incluye tanto establecer el enfoque nacional en un estado que posees como establecer el enfoque de punto crítico en un estado propiedad de otra nación.

#### Condiciones

Si estás estableciendo un enfoque para uno de tus estados, entonces no debe ser el enfoque de punto crítico, la ideología debe estar disponible si es un enfoque de lealtad partidista (habilitado y no restringido solo a civilizaciones si eres una nación no civilizada), y debe haber un enfoque ya establecido en el estado o menos que tu número máximo de enfoques disponibles debe estar establecido en otros estados.

Si es el enfoque de punto crítico, el estado no debe ser de tu propiedad, debes tener un rango colonial inferior, debes tener un núcleo en ese estado, el propietario no debe aceptar tu cultura principal, tu etiqueta debe ser liberable, otra nación no debe tener ya un enfoque de punto crítico allí, y o bien ya tienes menos que tu número máximo de enfoques activos *o* ya tienes un enfoque de punto crítico activo en otro lugar (establecer un enfoque de punto crítico en un estado cancelará cualquier enfoque de punto crítico que hayas establecido en otros estados).

#### Efecto

Cambia el enfoque nacional activo en el estado.

### Iniciar investigación

Esto comienza a investigar una tecnología específica.

#### Condiciones

Las naciones solo pueden comenzar a investigar tecnologías si no son incivilizadas, la fecha correspondiente a la activación de la tecnología ya ha pasado y todas las tecnologías anteriores (si las hay) de la misma carpeta ya están completamente investigadas. Y la tecnología no se está investigando actualmente.

#### Efecto

Establece la investigación actual del país en la tecnología especificada.

### Subsidios de guerra

Esto ayuda a financiar los esfuerzos de guerra del país objetivo, sin necesidad de un acuerdo entre ellos.

#### Condiciones

Solo se puede realizar si las naciones no están en guerra, la nación no está recibiendo ya subsidios de guerra y hay puntos diplomáticos defines: WARSUBSIDY_DIPLOMATIC_COST disponibles. Y el objetivo no es igual al remitente.

#### Efecto

El remitente dará al objetivo `defines:WARSUBSIDIES_PERCENT x total-expenses-of-target` en cada intervalo. 

### Aumentar relaciones

Esto aumenta las relaciones entre los dos países y no requiere acuerdo entre ellos.

#### Condiciones

Solo se puede realizar si las naciones no están en guerra, el valor de relación no está al máximo en 200 y tiene puntos diplomáticos definidos por INCREASERELATION_DIPLOMATIC_COST. Además, el objetivo no puede ser el mismo que el remitente.

#### Efecto

Aumenta el valor de las relaciones en la cantidad definida por INCREASERELATION_RELATION_ON_ACCEPT (normalmente establecida en 100) y disminuye los puntos diplomáticos en INCREASERELATION_DIPLOMATIC_COST.

### Disminuir relaciones

Esto disminuye las relaciones entre los dos países y no requiere acuerdo mutuo.

#### Condiciones

Solo se puede realizar si las naciones no están en guerra, el valor de relación no está al máximo en -200 y tiene puntos diplomáticos definidos por DECREASERELATION_DIPLOMATIC_COST. Además, el objetivo no puede ser el mismo que el remitente.

#### Efecto

Disminuye el valor de las relaciones en la cantidad definida por DECREASERELATION_RELATION_ON_ACCEPT (normalmente establecida en -20) y disminuye los puntos diplomáticos en DECREASERELATION_DIPLOMATIC_COST.

### Conquistar una provincia

Estrictamente hablando, no es un comando que esperaríamos que la interfaz de usuario envíe directamente (excepto quizás a través de la consola). Sin embargo, se puede considerar como un componente de comandos más complejos que eventualmente ejecutarán un acuerdo de paz, por ejemplo.

#### Condiciones

Depende del contexto en el que se genere.

#### Efecto

Además de transferir la propiedad de la provincia: (TODO: evitar más de una base naval por estado)
- Todas las poblaciones en la provincia pierden todos sus ahorros.
- Si la provincia no es un núcleo del nuevo propietario y no es una provincia colonial (antes de la conquista), cualquier población que no sea de una cultura aceptada o principal recibe define:MIL_HIT_FROM_CONQUEST de beligerancia.
- Las provincias conquistadas a un país incivilizado por un país civilizado se convierten en coloniales.
- El conquistador puede ganar puntos de investigación:
  Primero, calcula cuántos puntos de investigación generarían las poblaciones en la provincia como si fueran una pequeña nación (es decir, para cada tipo de población que genera puntos de investigación, multiplica ese número por la fracción de la población en comparación con su fracción óptima (limitada a uno) y súmalos todos juntos). Luego multiplica ese valor por (1.0 + el modificador nacional al modificador de puntos de investigación + el modificador de aumento de tecnología para la investigación). Ese valor se multiplica luego por define:RESEARCH_POINTS_ON_CONQUER_MULT y se agrega a los puntos de investigación de la nación conquistadora. Bueno, ¿y qué pasa con el modificador de puntos de investigación de la nación al conquistar? Sí, eso parece tener un error. La nación obtiene puntos de investigación solo si ese multiplicador es positivo, pero de lo contrario, no afecta el resultado.
- La provincia adquiere nacionalismo igual a define:YEARS_OF_NATIONALISM.
- Las poblaciones abandonan cualquier movimiento/rebelión.
- Se eliminan los modificadores temporales; se cancelan las construcciones.
- Cuando se crean nuevos estados por conquista, la nación recibe un evento `on_state_conquest`.

### Crear un líder militar (ya sea general o almirante)

#### Condiciones

La nación debe tener puntos de liderazgo disponibles definidos por define:LEADER_RECRUIT_COST.

#### Efecto

Lo mismo que la creación automática de líderes, excepto que puedes elegir el tipo. La nación pierde puntos de liderazgo definidos por define:LEADER_RECRUIT_COST. (También hay un mensaje MILITARY_CREATE_GENERAL).

### Comenzar a construir un edificio de provincia (base naval, fortaleza, ferrocarril)

Nota irrelevante: después de todo, es POP_BUILD_FACTORY lo que permite a las poblaciones construir ferrocarriles.

#### Condiciones

- La nación debe tener el nivel tecnológico necesario para construir hasta el nuevo nivel en la provincia objetivo.
- No debe haber ningún proyecto de construcción existente para ese edificio.
- La provincia debe estar bajo el control de su propietario.
- La provincia no debe estar sitiada.
- Si no es propiedad de la nación que realiza la construcción, debe ser un ferrocarril; la nación objetivo debe permitir la inversión extranjera; la nación que realiza la inversión debe ser una gran potencia mientras que la objetivo no lo es; y la nación que realiza la inversión no debe estar en guerra con la nación objetivo.
- La nación que recibe la inversión debe ser civilizada.

Para bases navieras: solo una por estado (verificar construcciones en curso en el estado también) y solo en la costa.

Para ferrocarriles: las naciones deben tener la RULE_BUILD_RAILROAD para iniciar un proyecto.

#### Efecto

Iniciar el proyecto de construcción del edificio. Si la provincia se encuentra en una nación extranjera, obtener crédito de inversión extranjera igual al costo total proyectado.

### Iniciar la construcción o mejora de una fábrica

#### Condiciones

- La nación debe tener la regla establecida para permitir la construcción/mejora si se trata de un objetivo nacional.
- Para inversión extranjera: la nación objetivo debe permitir la inversión extranjera; la nación que realiza la inversión debe ser una gran potencia mientras que la objetivo no lo es; y la nación que realiza la inversión no debe estar en guerra con la nación objetivo. La nación que recibe la inversión debe ser civilizada.
- El edificio de la fábrica debe estar desbloqueado por la nación.
- Las fábricas no se pueden construir en un estado colonial.
- Las fábricas costeras solo se pueden construir en estados costeros.
- No puede haber fábricas duplicadas (2 fábricas del mismo tipo).
- Para fábricas nuevas: no puede haber más de 7 fábricas existentes + en construcción.
- Para mejoras: no se puede mejorar más allá del nivel máximo.

#### Efecto

Iniciar el proyecto de construcción del edificio. Si la provincia se encuentra en una nación extranjera, obtener crédito de inversión extranjera igual al costo total proyectado.

### Destruir Fábrica

Complicación: la fábrica debe ser identificada por su ubicación y tipo, ya que puede haber otros comandos de fábrica en curso.

#### Condiciones

La nación debe tener la regla correspondiente para permitir la destrucción.

#### Efecto

La fábrica desaparece.

### Cambiar configuración de fábrica

Cambiar la prioridad de contratación o el estado subvencionado de una fábrica.

#### Condiciones

Reglas nacionales relevantes.

#### Efecto

Cambios en el estado.

### Iniciar construcción de unidad

#### Condiciones

La provincia debe ser propiedad y estar controlada por la nación constructora, sin un asedio en curso.
El tipo de unidad debe estar disponible desde el inicio/desbloqueado por la nación.

Unidades terrestres:

Cada población de soldados solo puede respaldar un número limitado de regimientos (incluyendo los en construcción y los regimientos rebeldes).
Si la unidad está culturalmente restringida, debe haber una población de soldados de cultura principal o cultura aceptada disponible con espacio.

Unidades navales:

La provincia debe ser costera.
La provincia debe tener una base naval de un nivel suficiente, dependiendo del tipo de unidad.
La provincia no puede estar en ultramar para algunos tipos de unidades.
Algunas unidades tienen un número máximo por puerto donde pueden ser construidas y eso debe respetarse.

#### Efecto

Condición de inicio

### Cancelar construcción de unidad

#### Condiciones

Debe ser el propietario de la provincia donde se está construyendo la unidad.

#### Efecto

Cancela la construcción.

### Liberar a una nación como vasallo

#### Condiciones

No debe estar en guerra y el país que se va a liberar no debe existir actualmente. La etiqueta asociada debe ser liberable, y debes poseer algunos núcleos pertenecientes a esa etiqueta.

#### Efecto

Libera a la nación de cualquier núcleo propiedad del remitente del comando. El remitente del comando pierde todos los núcleos en esas provincias. El remitente del comando gana define: RELEASE_NATION_PRESTIGE. El remitente del comando gana define: RELEASE_NATION_INFAMY. La nación liberada tiene el mismo gobierno que el liberador (esto puede forzar el cambio del partido gobernante y, por lo tanto, cambiar los problemas activos del partido).

### Cambiar de naciones

#### Condiciones

La etiqueta objetivo no debe estar controlada por otro jugador. La etiqueta debe existir.

#### Efecto

Cambia la nación a la etiqueta, sin efecto en naciones controladas por la IA.

### Cambiar configuración del presupuesto

#### Condiciones

Ninguna configuración puede salirse del rango permitido.

#### Efecto

Se cambian las configuraciones (ajustándolas al rango permitido).

### Cambiar configuración del almacenamiento

Cambiar si el almacenamiento se está llenando y cuál es su cantidad objetivo.

### Iniciar elección

#### Condiciones

Las elecciones deben ser posibles en la forma de gobierno. No puede haber ninguna elección en curso actualmente.

#### Efecto

Inicia una elección.

### Establecer prioridad de influencia

#### Condiciones

La fuente debe ser una gran potencia, mientras que el objetivo no debe ser una gran potencia.

#### Efecto

Establece la prioridad de influencia de 0 a 3.

### Desacreditar

#### Condiciones

La fuente debe ser una gran potencia. La fuente debe tener puntos de influencia define: DISCREDIT_INFLUENCE_COST. La fuente no puede estar actualmente prohibida con el objetivo directo ni estar actualmente en el lado opuesto de una guerra que los involucre. Solo una gran potencia puede ser un objetivo secundario para esta acción. Para desacreditar a una nación, debes tener una opinión de al menos "opuesto" con la nación influenciada y debes tener un nivel de opinión igual o mejor con la nación influenciada que la que estás desacreditando.

### Efecto

Una nación queda desacreditada durante definir: DISCREDIT_DAYS. Ser desacreditado dos veces no suma estas duraciones; simplemente reinicia el temporizador desde el día actual. Desacreditar a una nación "aumenta" tu relación con ellos en definir: DISCREDIT_RELATION_ON_ACCEPT. Desacreditar tiene un costo de definir: DISCREDIT_INFLUENCE_COST puntos de influencia.

### Expulsar Asesores

#### Condiciones

La fuente debe ser una gran potencia. La fuente debe tener definir: EXPELADVISORS_INFLUENCE_COST puntos de influencia. La fuente no puede estar actualmente prohibida con el objetivo directo o estar actualmente en el lado opuesto de una guerra que los involucre. Solo una gran potencia puede ser un objetivo secundario para esta acción. Para expulsar asesores, debes tener al menos una opinión neutral con la nación influenciada y un nivel de opinión igual o mejor que el de la nación que estás expulsando.

#### Efecto

Expulsar a los asesores de una nación "aumenta" tu relación con ellos en definir: EXPELADVISORS_RELATION_ON_ACCEPT. Esta acción tiene un costo de definir: EXPELADVISORS_INFLUENCE_COST puntos de influencia. Ser expulsado cancela cualquier efecto de desacreditación en curso. Ser expulsado reduce tu influencia a cero.

### Prohibir embajada

#### Condiciones

La fuente debe ser una gran potencia. La fuente debe tener definir: BANEMBASSY_INFLUENCE_COST puntos de influencia. La fuente no puede estar actualmente prohibida con el objetivo directo ni estar actualmente en el lado opuesto de una guerra que los involucre. Solo una gran potencia puede ser un objetivo secundario para esta acción. Para prohibir a una nación, debes ser al menos amistoso con la nación influenciada y tener un nivel de opinión igual o mejor que el de la nación que estás expulsando.

#### Efecto

Prohibir la embajada de una nación "aumenta" tu relación con ellos en definir: BANEMBASSY_RELATION_ON_ACCEPT. Esta acción tiene un costo de definir: BANEMBASSY_INFLUENCE_COST puntos de influencia. El efecto de prohibición de embajada dura definir: BANEMBASSY_DAYS. Si ya estás prohibido, ser prohibido nuevamente simplemente reinicia el temporizador. Ser prohibido cancela cualquier efecto de desacreditación en curso.

### Aumentar opinión:

#### Condiciones

La fuente debe ser una gran potencia. La fuente debe tener definir: INCREASEOPINION_INFLUENCE_COST puntos de influencia. La fuente no puede estar actualmente prohibida con el objetivo directo ni estar actualmente en el lado opuesto de una guerra que los involucre. Solo una gran potencia puede ser un objetivo secundario para esta acción. Tu opinión actual debe ser menor que amistosa.

#### Efecto

Aumentar la opinión de una nación tiene un costo de definir: INCREASEOPINION_INFLUENCE_COST puntos de influencia. La opinión puede aumentarse hasta un máximo de amistosa.

### Disminuir la opinión:

#### Condiciones

El origen debe ser una gran potencia. El origen debe tener puntos de influencia definidos: DECREASEOPINION_INFLUENCE_COST. El origen no puede estar actualmente vetado con el objetivo directo ni estar actualmente en el lado opuesto de una guerra en la que estén involucrados. Solo una gran potencia puede ser un objetivo secundario para esta acción. Disminuir la opinión de otra nación requiere que tengas una opinión de al menos "opuesto" con la nación influida y debes tener un nivel de opinión igual o mejor con la nación influida que la que estás disminuyendo. El objetivo secundario no debe tener a la nación influida en su esfera ni puede estar en una opinión hostil con ellos.

#### Efecto

Disminuir la opinión de una nación "aumenta" tu relación con ellos en definir: DECREASEOPINION_RELATION_ON_ACCEPT. Esta acción cuesta puntos de influencia definidos: DECREASEOPINION_INFLUENCE_COST. La opinión de la nación influida del objetivo secundario disminuye en un paso.

### Agregar a la esfera

#### Condiciones

El origen debe ser una gran potencia. El origen debe tener puntos de influencia definidos: ADDTOSPHERE_INFLUENCE_COST. El origen no puede estar actualmente vetado con el objetivo directo ni estar actualmente en el lado opuesto de una guerra en la que estén involucrados. Solo una gran potencia puede ser un objetivo secundario para esta acción. La nación debe tener una opinión amistosa de ti y no debe estar en la esfera de otra nación.

#### Efecto

Agregar una nación a tu esfera cuesta puntos de influencia definidos: ADDTOSPHERE_INFLUENCE_COST.

### Quitar de la esfera

#### Condiciones

El origen debe ser una gran potencia. El origen debe tener puntos de influencia definidos: REMOVEFROMSPHERE_INFLUENCE_COST. El origen no puede estar actualmente vetado con el objetivo directo ni estar actualmente en el lado opuesto de una guerra en la que estén involucrados. Solo una gran potencia puede ser un objetivo secundario para esta acción. Para realizar esta acción, debes tener un nivel de opinión amistosa con la nación de la que estás eliminando de una esfera.

#### Efecto

Quitar una nación de una esfera cuesta puntos de influencia definidos: REMOVEFROMSPHERE_INFLUENCE_COST. Si quitas una nación de tu propia esfera, pierdes puntos de prestigio definidos: REMOVEFROMSPHERE_PRESTIGE_COST y ganas puntos de infamia definidos: REMOVEFROMSPHERE_INFAMY_COST. Quitar una nación de la esfera de otra nación "aumenta" tu relación con el líder de la esfera anterior en definir: REMOVEFROMSPHERE_RELATION_ON_ACCEPT puntos. La nación eliminada luego se vuelve amistosa con su antiguo líder de la esfera.

### Convertir colonia en estado

#### Condiciones

No debe estar en guerra. El estado debe ser colonial. Las poblaciones de burócratas primarios y aceptados deben representar al menos el define: STATE_CREATION_ADMIN_LIMIT por ciento de la población en el estado. Debe haber suficientes puntos coloniales libres. El requisito de puntos es: define: COLONIZATION_CREATE_STATE_COST x número de provincias x 1v (ya sea distancia al capital / COLONIZATION_COLONY_STATE_DISTANCE o 0 si tiene una conexión terrestre con el capital).

#### Efecto

Las provincias en el estado dejan de ser coloniales.
Gana define: COLONY_TO_STATE_PRESTIGE_GAIN x (1.0 + prestigio-de-la-colonia-desde-la-tecnología) x (1.0 + prestigio-desde-la-tecnología).
Todos los modificadores temporales activos para las provincias en el estado expiran.
Ocurre un evento de `on_colony_to_state` (con el estado en foco).
Ocurre un evento de `on_colony_to_state_free_slaves` (con el estado en foco).
Se actualiza como nación colonial.

### Invierte en colonia

#### Condiciones

Debe haber una provincia no colonizada.
Debe estar en la fase 0, 1 o 2.
No puede haber una crisis colonial en curso para el estado.

Invertir en una colonia cuesta define:COLONIZATION_INVEST_COST_INITIAL + define:COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (si una provincia adyacente a la región es propiedad) para colocar al colonizador inicial. Pasos adicionales cuestan define:COLONIZATION_INTEREST_COST mientras esté en la fase 1. En la fase dos, cada punto de costo de inversión define:COLONIZATION_INFLUENCE_COST hasta el cuarto punto. Después de alcanzar el cuarto punto, los puntos adicionales cuestan define:COLONIZATION_EXTRA_GUARD_COST x (puntos - 4) + define:COLONIZATION_INFLUENCE_COST.

Puedes invertir colonialmente en una región si hay menos de 4 colonizadores allí (o ya tienes un colonizador allí). También debes tener suficiente tecnología de life rating. Específicamente, debes tener puntos de life rating colonial de tecnología + define:COLONIAL_LIFERATING menores o iguales al *mayor* life rating de una provincia no poseída en el estado. Tu país debe ser de define:COLONIAL_RANK o menos.

Si aún no has puesto un colonizador en la región, debes estar dentro del alcance de la región. Cualquier región adyacente a tu país o a uno de tus vasallos o subestados se considera dentro del alcance. De lo contrario, debe estar dentro del alcance de una de tus bases navales, con el alcance dependiendo del valor de rango colonial proporcionado por el edificio de la base naval multiplicado por el nivel de la base naval.

Si has puesto un colonizador en la región, y la colonización está en la fase 1 o 2, puedes invertir si han pasado al menos define:COLONIZATION_DAYS_BETWEEN_INVESTMENT desde tu última inversión, y tienes suficientes puntos coloniales libres.

#### Efecto

Añade un punto a tu progreso de colonización, reinicia el tiempo desde la última inversión. Cuando alguien alcanza define:COLONIZATION_INTEREST_LEAD puntos, y es fase 1, pasa a la fase 2, expulsando a todos menos al segundo colonizador con más puntos. En la fase 2, si obtienes define:COLONIZATION_INFLUENCE_LEAD puntos por delante del otro colonizador, el otro colonizador es expulsado y la fase pasa a 3.

### Finalizar colonización

(es decir, convertir una inversión colonial en un estado colonial)

#### Condiciones

Debe tener al menos define:COLONIZATION_CREATE_PROTECTORATE_COST puntos coloniales libres. Debe estar en la etapa colonial 3 y tener un colonizador presente.

#### Efectos

Gana define:COLONY_FORMED_PRESTIGE x (modificador de prestigio de tecnología + 1) de prestigio. Convierte las provincias no poseídas en un estado colonial.

### Intervenir en la guerra

#### Condiciones

Condiciones estándar para unirse a la guerra: no puedes unirte si ya estás en guerra contra cualquier atacante o defensor. No puedes unirte a una guerra civil. No puedes unirte a una guerra contra tu líder o soberano de esfera (obviamente no se aplica aquí). No puedes unirte a una guerra de crisis antes de que se inventen las guerras importantes (es decir, debes estar en la crisis). No puedes unirte como atacante contra alguien con quien tienes un alto el fuego.

Debes ser una gran potencia. No debes estar involucrado ni interesado en una crisis. Deben haber pasado al menos define:MIN_MONTHS_TO_INTERVENE desde que comenzó la guerra.

Si no es una guerra importante:
Debes tener al menos una relación amistosa con el defensor principal. Solo puedes unirte al lado del defensor. No debes estar en una guerra de crisis. Los defensores deben no tener objetivos bélicos o solo tener el statu quo. El defensor principal debe tener define:MIN_WARSCORE_TO_INTERVENE o menos. Nota: *Puedes* intervenir en una guerra civil, a pesar de no poder ser llamado normalmente.

Si la guerra es una guerra importante:
Entonces es posible unirse también al lado atacante.
Debe haber una relación de define:GW_INTERVENE_MIN_RELATIONS con el defensor/atacante principal para intervenir, y debe tener como máximo el agotamiento de guerra define:GW_INTERVENE_MAX_EXHAUSTION.
No puedes unirte si alguna nación en tu esfera está en el otro lado.
No puedes unirte si estás aliado con alguna nación aliada de alguna nación en el otro lado.
No puedes unirte si tienes unidades dentro de una nación en el otro lado.

#### Efecto

Unirse a la guerra. Agrega un objetivo bélico de statu quo si no es una guerra importante y aún no está presente.

### Suprimir movimiento

#### Condiciones

Costo de puntos de supresión:
Depende de si define:POPULATION_SUPPRESSION_FACTOR es diferente de cero. Si es cero, la supresión cuesta su radicalismo efectivo + 1. Si no es cero, entonces el costo es el mayor entre ese valor y el radicalismo efectivo del movimiento x el apoyo del movimiento / define:POPULATION_SUPPRESSION_FACTOR.

#### Efecto

El radicalismo del movimiento aumenta permanentemente en define:SUPPRESSION_RADICALISM_HIT. La conciencia de cada población perteneciente al movimiento se establece en 1 y luego las poblaciones son eliminadas del movimiento.

### Convertirse en civilizado

#### Condiciones

El progreso de la civilización debe ser del 100% o más.

#### Efecto

La nación obtiene tecnologías. Específicamente, toma la fracción de las reformas militares (para tierra y naval) o reformas económicas (en otro caso) aplicadas, limitadas a los valores define:UNCIV_TECH_SPREAD_MIN y define:UNCIV_TECH_SPREAD_MAX, y multiplica la posición del líder de la esfera (o el primer GP) en cada columna tecnológica, redondeada hacia arriba, para dar a las naciones incivilizadas sus tecnologías al occidentalizarse.
La nación recibe un evento `on_civilize`.
Reformas políticas y sociales: ¿Primer ajuste en todas las categorías?

### Desarrollar interés en una crisis

#### Condiciones

No estar ya interesado en la crisis. Ser una gran potencia. No estar en guerra. La crisis ya debe haber obtenido sus partidarios iniciales.

#### Efecto

Exactamente lo que esperas.

### Unirse a un bando en una crisis

#### Condiciones

No estar ya involucrado en la crisis. Debe estar interesado en la crisis. Debe ser una gran potencia. No debe estar desarmado. La crisis ya debe haber obtenido sus partidarios iniciales.

#### Efecto

Unirse al bando de la crisis.

### Aceptar/rechazar una oferta de respaldo de crisis

#### Condiciones

Debe haber una oferta pendiente.

#### Efecto

Unirse a la crisis como atacante o defensor principal.

### Cambiar configuraciones de reserva

#### Condiciones

Ninguna.

#### Efecto

Lo obvio.

### Cambiar partido gobernante

#### Condiciones

La ideología del partido gobernante debe estar permitida por la forma de gobierno. No puede haber una elección en curso. No puede ser el partido gobernante actual. El gobierno debe permitir al jugador establecer el partido gobernante. El partido gobernante solo puede cambiarse manualmente como máximo una vez al año.

#### Efecto

Se cambia el partido gobernante. Se actualizan los problemas y reglas del partido (ten en cuenta que esto puede requerir otras acciones, como desactivar los subsidios).
Si la ideología del nuevo partido *no* es la misma que la antigua: todas las poblaciones obtienen ((número total de reformas políticas y sociales sobre la base) x 0.01 + 1.0) x define:RULING_PARTY_ANGRY_CHANGE x militancia de apoyo de la antigua ideología
todas las poblaciones obtienen define:RULING_PARTY_HAPPY_CHANGE x militancia de apoyo de la nueva ideología
Lo mismo se hace también para todos los problemas del partido que difieren entre los dos.

### Seleccionar reforma política / social / militar / económica

#### Condiciones

Las condiciones para seleccionar estas reformas se han descrito en otro lugar de este documento.

#### Efectos

Para reformas basadas en política y sociedad:
- Cada movimiento basado en problemas con un mayor apoyo popular que el movimiento que respalda el problema dado (si hay tal movimiento; todos los movimientos si no hay tal movimiento) tiene su radicalismo aumentado en 3v(apoyo-de-ese-movimiento / apoyo-de-movimiento-detras-de-la-cuestion (o 1 si no hay tal movimiento) - 1.0) x define:WRONG_REFORM_RADICAL_IMPACT.
- Cada población en la nación gana define:CON_REFORM_IMPACT x apoyo-poblacional de la conciencia del problema.
- Para cada ideología, la población gana define:MIL_REFORM_IMPACT x apoyo-poblacional-para-esa-ideología x apoyo-de-ideología-para-hacer-lo-opuesto-a-la-reforma (calculado de la misma manera que determinar cuándo la cámara alta apoyará la reforma o la derogación) militancia.
- Si la población forma parte de un movimiento para algún otro problema (o para la independencia), obtiene define:WRONG_REFORM_MILITANCY_IMPACT de militancia. Todas las demás poblaciones pierden define:WRONG_REFORM_MILITANCY_IMPACT de militancia.

Para reformas militares/económicas:
- Ejecutar el miembro `on_execute`.
- Restar puntos de investigación (ver discusión sobre cuántos puntos de investigación se pueden realizar cuando la reforma es posible).

En general:
- Aumentar la proporción de conservadores en la cámara alta por define:CONSERVATIVE_INCREASE_AFTER_REFORM (y luego normalizar nuevamente).
- Si la esclavitud está prohibida (la regla slavery_allowed es falsa), eliminar todos los estados esclavistas y liberar a todos los esclavos.
- Los movimientos ahora pueden ser inválidos y pueden necesitar ser eliminados (¿puede la actualización normal manejar esto?).

### Tomar decisión

#### Condiciones

Deben cumplirse las condiciones de permitir y posible desencadenar.

#### Efecto

Se toma la decisión (se ejecuta su efecto).

### Tomar decisión del evento

Por razones de implementación, habrá cuatro versiones de este comando, correspondientes a los cuatro tipos de eventos. Debes llenar el comando con los valores exactos del evento por el cual deseas tomar una decisión, o de lo contrario, será ignorado.

#### Condiciones

Ninguna

#### Efecto

Ejecuta el efecto de la elección. Internamente, elimina el evento de la cola de eventos pendientes.

### Solicitar acceso militar

(Implicitamente: también deben existir comandos para aceptar o rechazar una solicitud de acceso)

#### Condiciones

Debe tener costos diplomáticos de defines:ASKMILACCESS_DIPLOMATIC_COST puntos diplomáticos. No deben estar en guerra entre sí. No debe tener ya acceso militar.

#### Efectos

Las relaciones aumentan en defines:ASKMILACCESS_RELATION_ON_ACCEPT si es aceptado, y en defines:ASKMILACCESS_RELATION_ON_DECLINE si es rechazado.

### Cancelar acceso militar

#### Condiciones

Debe tener puntos diplomáticos de defines:CANCELASKMILACCESS_DIPLOMATIC_COST. Debe tener acceso militar.

#### Efectos

Finaliza el acceso.

### Solicitar una alianza

(implícitamente: también debe haber comandos para aceptar o rechazar una solicitud de acceso)

#### Condiciones

No debe tener una alianza. No deben estar en guerra entre sí. Cuesta puntos diplomáticos de defines:ALLIANCE_DIPLOMATIC_COST. Las grandes potencias no pueden formar una alianza mientras haya una crisis activa. Los vasallos y estados dependientes solo pueden formar una alianza con sus soberanos.

#### Efectos

Aumenta las relaciones en defines:ALLIANCE_RELATION_ON_ACCEPT al aceptarse o en defines:ALLIANCE_RELATION_ON_DECLINE si se rechaza. Crea una alianza.

### Finalizar alianza

#### Condiciones

Debe tener una alianza. No puede estar en guerra en el mismo bando. Cuesta puntos diplomáticos de defines:CANCELALLIANCE_DIPLOMATIC_COST.

#### Efectos

Finaliza la alianza. Las relaciones aumentan en defines:CANCELALLIANCE_RELATION_ON_ACCEPT.

### Llamar al aliado a la guerra

(implícitamente: también debe haber comandos para aceptar o rechazar una solicitud de acceso)

#### Condiciones

Requiere define:CALLALLY_DIPLOMATIC_COST puntos diplomáticos. El aliado debe poder unirse a la guerra. La nación que llama debe estar en la guerra.

#### Efecto

El aliado se une a la guerra (o no). Las relaciones aumentan en define:CALLALLY_RELATION_ON_ACCEPT o define:CALLALLY_RELATION_ON_DECLINE. Para una llamada defensiva, rechazarla provoca la ruptura de la alianza.

### Iniciar fabricación de CB (Causa de guerra)

#### Condiciones

No se puede fabricar sobre alguien con quien estás en guerra. No se puede fabricar sobre nadie más que tu soberano si eres un vasallo. Requiere define:MAKE_CB_DIPLOMATIC_COST puntos diplomáticos. No se puede fabricar sobre los miembros de tu esfera. El CB debe ser fabricable.

#### Efecto

Comienza la fabricación.

### Alternar caza de rebeldes ACTIVADA/DESACTIVADA

#### Condiciones

El ejército debe ser tuyo o estar bajo tu control.

#### Efecto

Activa/Desactiva la caza de rebeldes; si está activada, el ejército derrotará automáticamente a los rebeldes.

TODO
Tomar el mando de las unidades vasallas / renunciar al mando

# Reglas extendidas del juego

Se han agregado algunas características nuevas para permitir a los jugadores una mayor libertad en la toma de decisiones:

### Vasallo libre

Libera a un vasallo de tu control.

#### Condiciones

Debe ser nuestro vasallo y no estar en guerra.

#### Efecto

Deja de ser nuestro vasallo, pero las alianzas se mantienen.