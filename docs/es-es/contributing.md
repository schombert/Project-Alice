# ¿Interesado en contribuir?

Ayuda que estoy buscando actualmente:

#### Arte:

Me gustaría encontrar a alguien que pudiera básicamente rediseñar todos los activos de la interfaz de usuario existentes de Victoria 2 en un nuevo estilo artístico para darle al juego su propia identidad visual distintiva. Sí, sé que esto no es algo trivial.

#### Testers:

Estoy buscando personas que puedan construir un proyecto en C++ y ejecutarlo, pero que no quieran (o no se sientan cómodas) contribuyendo con código. Se espera que los testers construyan el proyecto y lo investiguen de manera regular para encontrar errores, especialmente aquellos que no aparecen en las combinaciones de software/hardware en las que no estoy desarrollando. Hablen con nuestro coordinador de proyecto si están interesados en ser testers.

#### Agentes libres:

Dado que este es un proyecto de código abierto, no es necesario ser un miembro oficial del equipo para contribuir. Cualquiera puede hacer un Pull Request a través de GitHub y contribuir de manera *ad hoc* de esa manera.

### Construyendo el proyecto:

La compilación utiliza CMake y debería "funcionar" simplemente, con algunas excepciones molestas.

#### Windows:

1. Necesitarás una copia completa de git instalada (https://git-scm.com/downloads), ya que la que viene con Visual Studio no es suficiente para CMake, por alguna razón.
2. Asegúrate de tener una versión relativamente actualizada del Windows SDK instalada (puedes actualizar tu versión local de Windows SDK a través del instalador de Visual Studio).
3. Abre el proyecto en Visual Studio y deja que se configure (CMake debería ejecutarse en la ventana de salida y descargar dependencias; esto puede llevar algún tiempo).
4. Ve a la sección "Toques Finales" en la parte inferior de esta página.

Si experimentas problemas con la reproducción de audio, es posible que desees instalar Windows Media Player y el Paquete de funciones multimedia de Windows (https://support.microsoft.com/en-us/windows/get-windows-media-player-81718e0d-cfce-25b1-aee3-94596b658287) para poder reproducir adecuadamente archivos MP3 (la máquina de música).

#### Linux (distribución basada en Debian)

Asegúrate de instalar las dependencias necesarias.

```bash
sudo apt update
sudo apt install git build-essential clang cmake libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

Después de que se complete la instalación, descarga la extensión `cmake-tools` para Visual Studio Code y abre el proyecto. CMake debería configurar automáticamente el proyecto por ti.

Si obtienes el error `/usr/bin/ld: cannot find -lstdc++`, podría ser causado por un paquete gcc-12 roto en Ubuntu jammy.
```bash
sudo apt remove gcc-12
```
https://stackoverflow.com/questions/67712376/after-updating-gcc-clang-cant-find-libstdc-anymore

(Linux solamente) Parece que la versión de la biblioteca TBB de Intel que estamos utilizando no se compila correctamente si hay espacios en la ruta, así que asegúrate de que el lugar donde coloques el proyecto no tenga espacios en ninguna parte de su ruta (sí, esto también me parece extraño).

#### Linux (Genérico)

Esto es para distribuciones no basadas en Debian, utilidades necesarias:
Herramientas básicas de compilación (Compilador C y Compilador C++)
CMake
Git
onetbb
GLFW3
X11 (el soporte *debería* existir para Wayland, pero no hay garantía de ello).

A partir de aquí, la compilación es sencilla:
1. `cd Project-Alice`
2. `cmake -E make_directory build`
3. `cmake -E chdir build cmake ..`
4. `touch src/local_user_settings.hpp`
5. `nano src/local_user_settings.hpp` o utiliza el editor de texto a tu elección.
6. Agrega las siguientes líneas:
    ```cpp
        #ifndef GAME_DIR
        #define GAME_DIR "[insert file path here]"
        #define OOS_DAILY_CHECK 1
        #endif

    ```
  Deberías establecer GAME_DIR en la carpeta que contiene tus archivos de Vic2. Si descargaste el juego en Steam, puedes hacer clic derecho en Vic2 > Explorar archivos locales, y eso te dará la ruta correcta. En Linux, es importante señalar que no necesitas poner \ antes de los espacios. Entonces, si la ruta de tu archivo en Linux es /home/user/Victoria\ 2/, escribirías /home/user/Victoria 2/ en GAME_DIR (rodeado por comillas, por supuesto).
    Copia la ruta del archivo y reemplaza [inserta la ruta del archivo aquí] con ella, luego guarda. Además, `OOS_DAILY_CHECK` debería estar definido si deseas controles OOS diarios en lugar de los habituales "mensuales".

7. `cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..`
8. `cmake --build . -j$(nproc)`

### Últimos detalles

Debido a que el proyecto en su estado actual necesita utilizar los archivos de juego existentes (como fuente de gráficos, para empezar), todos deben indicar al compilador dónde tienen instalada su copia del juego. Esto se hace creando un archivo llamado `local_user_settings.hpp` en el directorio `src`.
Ese archivo debería contener las siguientes cuatro líneas (la última es una línea vacía):
```cpp
#ifndef GAME_DIR
#define GAME_DIR "C:\\Your\\Victoria2\\Files"
#define OOS_DAILY_CHECK 1
#endif
```
Excepto reemplazar esa ruta con la ubicación de tu propia instalación. 
Ten en cuenta que en Windows debes escribir `\\` en lugar de solo `\` para cada separador de ruta. (Linux no tiene este problema y puedes escribir un solo `/`)
Segunda nota: en Windows, BrickPi ha realizado un cambio de manera que, si tienes instalado Victoria 2, es posible que puedas evitar la creación de `local_user_settings.hpp` por completo. Puede ser una buena idea intentar eso primero.
Finalmente, simplemente compila el objetivo de lanzamiento de Alice y deberías ver el juego aparecer en tu pantalla.

Recuerda que `OOS_DAILY_CHECK` hará que las verificaciones de OOS ocurran diariamente.