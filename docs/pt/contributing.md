## Interessado em contribuir?

Ajuda a qual estou atualmente procurando:

#### Arte:

Eu gostaria de encontrar alguém que poderia essencialmene refazer todos os assets de UI do Victoria 2 em um novo estilo artístico para dar ao jogo uma identidade visual única. Sim, eu sei que isso não é nada trivial.

#### Mapas:

Atualmente, fazemos algo um tanto feio ao estender os mapas de jogo existentes por todo o globo. Eu adoraria poder entregar um mapa que é capaz de cobrir todo o globo. Trabalhar no mapa requer habilidades básicas de edição de imagem e uma montanha inteira de paciência.

#### Testadores:

Procuro por pessoas que podem construir um projeto C++ e rodá-lo, mas que não querem (ou não ficam confortáveis em) contribuir. Espera-se que testadores construam o projeto e visitem-no de maneira semi-regular para encontrar bugs, especialmente aqueles que aparecem nas combinações software/hardware em que eu não estou desenvolvendo. Fale com o nosso coordenador de projeto se tiveres interesse.

#### Trabaladores autônomos

Já que este é um projeto open-source, não precisas ser um membro oficial do time para contribuir. Qualquer um pode fazer um PR pelo GitHub e contribuir de maneira *ad hoc*.

### Construindo o projeto

A build usa o CMake e deve "simplesmente funcionar", com algumas exceções chatas.

#### Windows

1. Precisarás instalar o Git (https://git-scm.com/downloads), já que o que vem com o Visual Studio não é suficiente para o CMake, por algum motivo.
2. Tenhas certeza de ter uma versão relativamente atualizada do SDK do Windows instalada (você pode atualizar sua versão do SDK do Windows através do instalador do Visual Studio).
3. Abras o projeto no Visual Studio e o deixes configurar (O CMake deve rodar na janela de output e baixar as dependências; isto pode levar algum tempo).
4. Vás olhar a se(c)ção "Toques finais" na parte de baixo desta página.

Se tiveres problemas com a reprodução de áudio, podes querer instalar o Windows Media Player e o Windows Media Feature Pack (https://support.microsoft.com/en-us/windows/get-windows-media-player-81718e0d-cfce-25b1-aee3-94596b658287) para poderes reproduzir arquivos MP3 (a jukebox de música).

#### Linux (Distro Debian-based)

Tenhas certeza de instalar as dependências requeridas.

```bash
sudo apt update
sudo apt install git build-essential clang cmake libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

Após terminar a instalação, baixes a extensão `cmake-tools` para o Visual Studio Code e abras o projeto. O CMake deve automaticamente configurar o projeto para ti.

Se obtiveres o erro, `/usr/bin/ld: cannot find -lstdc++`, ele pode estar sendo causado por um pacote gcc-12 quebrado no Ubuntu Jammy.
```bash
sudo apt remove gcc-12
```
https://stackoverflow.com/questions/67712376/after-updating-gcc-clang-cant-find-libstdc-anymore

(Somente Linux) A versão da livraria Intel TBB que usamos parece falhar em compilar se tiveres qualquer espaço no path, então deves garantir que onde quer que seja que coloques o projeto, não haja espaços (é, eu também acho isso estúpido).

#### Linux (Genérico)

Isto é para distribuições não-Debian, utilitários necessários:
Ferramentas de compilação básicas (C Compiler & C++ Compiler)
CMake
Git
onetbb
GLFW3
X11 (deve haver suporte para Wayland mas não há garantias.)

Daqui, compilar é bem direto
1. `cd Project-Alice`
2. `cmake -E make_directory build`
3. `cmake -E chdir build cmake ..`
4. `touch src/local_user_settings.hpp`
5. `nano src/local_user_settings.hpp` ou utilizes o teu editor de texto pessoal
6. adiciones as seguintes linhas:
    ```cpp
        #ifndef GAME_DIR
        #define GAME_DIR "[insiras o diretório aqui]"
        #endif

    ```
  deves definir o GAME_DIR para a pasta que contém os arquivos do Victoria 2, se baixaras o jogo na Steam então podes dar clique-direito em Vic2 > Buscar Arquivos Locais, e isto vai te dar o diretório correto no Linux. É notável que não é necessário pôr \ antes de espaços, então se o teu diretório é /home/user/Victoria\ 2/, then you write /home/user/Victoria 2/ in the GAME_DIR (surronded by quotes of course)
    copie o diretório e substitua [insirás o diretório aqui] com ele e então salves.

7. `cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..`
8. `cmake --build . -j$(nproc)`


#### Toques finais

Devido ao estágio atual do projeto, ele precisa utilizar o diretório do jogo (por exemplo como fonte de gráficos inicial), todos precisam dizer ao compilador onde a sua cópia do jogo está instalada. Podes fazer isso criando um arquivo `local_user_settings.hpp` no diretório `src`.
Esse arquivo deve conter as seguintes quatro linhas (a última é linha vazia):
```cpp
#ifndef GAME_DIR
#define GAME_DIR "C:\\Your\\Victoria2\\Files"
#endif
```
exceto substituindo aquele diretório com o teu diretório de jogo.
Note que no Windows precisas digitar `\\` ao invés de `\` para cada separador. (O Linux não tem esse problema e podes escrever simplesmente `/`)
Segunda Nota: no Windows, o BrickPi fez uma mudança tal que, se tiveres o Victoria 2 instalado, poderás ignorar a criação do `local_user_settings.hpp` completamente. Podes querer tentar isso primeiro.
Enfim, apenas faças build no launch target do Alice, e deves ver o jogo aparecer na tua tela.
