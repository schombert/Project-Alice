## Extensiones de Modding

Este documento aborda las extensiones de modding que se han agregado a Project Alice, además de lo que proporcionó Victoria 2. En general, estamos abiertos a agregar más extensiones de modding a solicitud, bajo la condición de que realmente planees usarlas. En términos generales, no estamos interesados en agregar extensiones de manera especulativa solo porque podrían ser útiles.

### Análisis de Condiciones de Activación

En Victoria 2, una condición de activación como `prestige = 5` se activará cuando el prestigio de la nación sea mayor o igual a 5. Si deseas probar si el valor es menor que 5, tendrías que incluirlo dentro de un alcance `NOT`. Y probar la igualdad exacta sería aún más complicado. Para simplificar las cosas, admitimos reemplazar el `=` con uno de los siguientes tokens: `==`, `!=`, `<`, `>`, `<=`, `>=`. `==` prueba la igualdad exacta, `!=` la desigualdad, y el resto tienen sus significados habituales. También admitimos reemplazar `=` con `!=` en la mayoría de las situaciones. Por ejemplo, `tag != USA` es lo mismo que `NOT = { tag = USA }`.

### Nuevos Efectos

- `kill_leader = "nombre del líder"`: elimina a un líder (almirante o general) perteneciente a la nación en el alcance con el nombre proporcionado. Ten en cuenta que esto solo funcionará de manera confiable si has creado explícitamente un líder con ese nombre mediante un efecto o mediante una definición en los archivos de historia.
- `annex_to = null`: convierte todas las provincias propiedad de la nación en el alcance en provincias sin dueño (lo que derrota a la nación, libera a sus títeres, etc.).
- `secede_province = null`: convierte la provincia en el alcance en una provincia sin dueño. Esto reemplaza algunos trucos que los mods utilizaban para convertir provincias en sin dueño, como cediéndolas a etiquetas que no existen, etc.

### Nuevas Condiciones de Activación

- `test = name_of_scripted_trigger` - evalúa un desencadenador scriptado (ver más abajo).
- `any_country = { ... }` - prueba si algún país existente cumple con las condiciones dadas en `...`. Esto es esencialmente un reflejo de cómo funciona el alcance de efecto `any_country` existente, pero para condiciones de activación.

### Desencadenadores Scriptados

Hemos añadido la capacidad de escribir condiciones de activación complicadas una vez y luego usar esa misma condición varias veces dentro de otras condiciones de activación. Para usar estos "desencadenadores scriptados", debes agregar un directorio llamado `scripted triggers` a tu mod (en el nivel superior de tu mod, por lo que la ruta se verá así: `...\mod name\scripted triggers`). Coloca cualquier cantidad de archivos `.txt` dentro de este directorio, cada uno de los cuales puede contener cualquier cantidad de desencadenadores scriptados.

Cada desencadenador scriptado en dicho archivo debería lucir así:
```
name_of_condition = {
	main_parameter = nation/state/province/pop/rebel/none
	this_parameter = ...
	from_parameter = ...
	
	... contents of the trigger condition go here...
}
```
Deberías agregar una entrada para `name_of_condition` en tu archivo CSV, ya que aparecerá en los consejos (tooltips) cuando uses el disparador (trigger) scripteado. Las tres entradas de parámetros definen el contenido que se probará con el disparador (debes elegir *una* de las posibilidades). Un disparador scripteado solo se puede usar en un contexto donde los diversos parámetros coincidan o donde el parámetro del disparador scripteado sea `none` (puedes pensar en `none` como que coincide con todo). Por ejemplo, si el parámetro principal es `province`, solo podrías evaluar el disparador scripteado cuando haya una provincia en el alcance. Los parámetros `this_parameter` y `from_parameter` son opcionales y pueden omitirse (su valor predeterminado es `none`). Puedes definir más de un disparador scripteado con el mismo nombre, siempre y cuando tengan tipos de parámetros diferentes. Al usar un disparador scripteado con múltiples versiones, se seleccionará automáticamente aquella que se ajuste a los parámetros actuales.

Para usar un disparador scripteado, simplemente agrega `test = name_of_condition` en un disparador donde quieras evaluar tu disparador scripteado y funcionará como si hubieras copiado su contenido en esa ubicación. Te aconsejo que no uses disparadores scripteados desde otros disparadores scripteados mientras los defines. Puedes referirte de manera segura a disparadores scripteados antes en el mismo archivo, pero hacerlo entre archivos te dejará a merced del orden de carga de archivos.

### Si/Sino (If/Else)

Se proporcionan declaraciones de Si y Sino ahora para evitar la duplicación de código y facilitar tu vida. Un `else` con un límite es equivalente a un `else_if` con un límite, y un `else_if` sin un límite es igual a un `else` sin un límite. En otras palabras, `else` y `else_if` son esencialmente sinónimos, proporcionados para la claridad del código.

Por ejemplo:
```
if = { #run if limit is true
  limit = { ... }
  ...
} else_if = { #run only if the limit above is false, and this limit is true
  limit = { ... }
  ...
} else = { #only if both cases are not fullfilled AND the limit is true
  limit = { ... }
  ...
}
```

No se especifica límite, lo cual equivale a un `always = yes`.

Además, el orden de ejecución para las declaraciones `if` y `else`/`else_if` permite código anidado:

```
if = {
	limit = { a = yes }
		money = 1
	if = {
		limit = { b = yes }
		money = 50
	} else {
		money = 100
	}
}
```
Esto es equivalente a:
```
if = {
	limit = { a = yes b = yes }
	money = 1
	money = 50
} else_if {
	limit = { a = yes }
	money = 1
	money = 100
}
```

Adicionalmente, la negación de afirmaciones es implícita, en otras palabras:
```
if = {
	limit = { test == yes }
} else {
	limit = { test != yes }
}
```

Se asume implícitamente para cada `else` después de un `if`, esto significa que no es necesario realizar una negación explícita (reescribir todo dentro de una gran declaración NOT) para las declaraciones `else`, ya que ahora lógicamente están vinculadas a que todas las declaraciones anteriores sean falsas y la declaración de su propio límite sea verdadera.

Un problema que podría existir debido a la volatilidad de la sintaxis podría ser:
```
else_if = {
  limit = { ... }
} if = {
  limit = { ... }
}
```

El comportamiento de esta declaración es que, dado que no hay un `if` precedente antes del `else_if`, el `else_if` podrá ejecutarse como si estuviera encadenado con un `if` que se evaluó como falso; en el caso de que su límite se evalúe como verdadero, entonces ejecutará su propio efecto. Sin embargo, la otra declaración `if` se ejecutará independientemente de la expresión anterior.

Dado que el orden lexicográfico de las declaraciones es secuencial, es decir, cada `else_if` y `else` debe ir precedido por una declaración `if`, de lo contrario se encadenarán al *precedente* `if` más cercano antes de ellos para su evaluación lexicográfica; de lo contrario, actuarán como un `if` en sí mismos si no hay ninguno presente.

```
else_if = {
  limit = { ... }
} else_if = {
  limit = { ... }
}
```
Estas declaraciones `else_if` están encadenadas entre sí; si la primera se ejecuta, la segunda no lo hará y viceversa. Si no existe un `if` precedente antes de ellas, la primera `else_if` asume el papel de la declaración `if`.

### Sintaxis abreviada de `.gui`

`size = { x = 5 y = 10 }` se puede escribir como `size = { 5 10 }`, al igual que en la mayoría de los lugares que esperan un par de valores x e y.
Además, `maxwidth = 5` y `maxheight = 10` se pueden escribir como `maxsize = { 5 10 }`.