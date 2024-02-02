## Interested in contributing?

Help that I am currently looking for:

#### Art:

I would like to find someone who could essentially redo all of the existing Victoria 2 UI assets in a new artistic style to give the game its own distinctive visual identity. Yes, I know that this is no trivial thing.

#### Testers:

I am looking for people who can build a C++ project and run it, but who don't want to (or aren't comfortable) contributing code. Testers are expected to build the project and poke at it on a semi-regular basis to find bugs, especially those that don't appear for the software/hardware combinations that I am not developing on. Speak to our project coordinator if you are interested in being a tester.

#### Free agents

Since this is an open-source project, you don't have to be an official member of the team to contribute. Anyone can make a PR through github and contribute in an *ad hoc* manner that way.

### Building the project

The build uses CMake and should "just work", with some annoying exceptions.

#### Windows

1. You will need a full copy of git installed (https://git-scm.com/downloads), as the one that ships with Visual Studio is not sufficient for CMake, for some reason.
2. Make sure that you have a relatively up-to-date version of the Windows SDK installed (you can update your local version of the Windows SDK through the Visual Studio installer).
3. Open the project in Visual Studio and let it configure (CMake should run in the output window and download dependencies; this may take some time).
4. Go look at the "Final Touches" section at the bottom of this page.

If you experience problems with audio playback, you may wish to install Windows Media Player and Windows Media Feature Pack (https://support.microsoft.com/en-us/windows/get-windows-media-player-81718e0d-cfce-25b1-aee3-94596b658287) to be able to properly playback MP3 files (the music jukebox).

#### Linux (Debian-based distro)

Make sure to install the required dependencies.

```bash
sudo apt update
sudo apt install git build-essential clang cmake libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

After the installation is done, download the `cmake-tools` extension for Visual Studio Code and open the project. CMake should automatically configure the project for you.

If you get the error, `/usr/bin/ld: cannot find -lstdc++`, it might be caused by a broken gcc-12 package in Ubuntu jammy.
```bash
sudo apt remove gcc-12
```
https://stackoverflow.com/questions/67712376/after-updating-gcc-clang-cant-find-libstdc-anymore

(Linux only) The version of Intel's TBB library we use seems to fail to compile if you have any spaces in the path, so you need to make sure that wherever you put the project has no spaces anywhere in its path (yes, this seems dumb to me too).

#### Linux (Generic)

This is for non-Debian based distributions, utilities needed:
Basic Compiling Tools, (C Compiler & C++ Compiler)
CMake
Git
onetbb
GLFW3
X11 (support *should* exist for wayland but there is not guarantee.)

From here compiling is straightforward
1. `cd Project-Alice`
2. `nano src/local_user_settings.hpp` or use the text editor of your choice
3. add the following lines:
    ```cpp
        #ifndef GAME_DIR
        #define GAME_DIR "[insert file path here]"
        #define OOS_DAILY_CHECK 1
        #endif

    ```
  you should set GAME_DIR to the folder which contains your Vic2 files, if you had downloaded the game on steam then you can right click Vic2 > Browse Local Files, and that'll give you the correct path on linux its noteworthy that you dont need to put \ before spaces, so if your Linux file path is /home/user/Victoria\ 2/, then you write /home/user/Victoria 2/ in the GAME_DIR (surronded by quotes of course)
    copy the file path and replace [insert file path here] with it, then save. Additionaly, `OOS_DAILY_CHECK` should be defined if you want daily OOS checks instead of the usual "monthly" ones.
    
4. `cmake -B build . -DCMAKE_BUILD_TYPE=Debug`
5. `cmake --build build -j$(nproc)`


#### Final touches

Because the project in its current state needs to use the existing game files (as a source of graphics, for starters), everyone needs to tell the compiler where their copy of the game is installed. You do this by creating a file named `local_user_settings.hpp` in the directory `src`.
That file should contain the following four lines (the last one is an empty line):
```cpp
#ifndef GAME_DIR
#define GAME_DIR "C:\\Your\\Victoria2\\Files"
#define OOS_DAILY_CHECK 1
#endif
```
except replacing that path with your own installation location.
Note that on Windows you need to write `\\` instead of just `\` for each path separator. (Linux does not have this issue, and you can write a single `/`)
Second note: on Windows, BrickPi has made a change such that, if you have Victoria 2 installed, you may be able to bypass creating `local_user_settings.hpp` completely. You may want to try that first.
Finally, just build the Alice launch target, and you should see the game pop up on your screen.

Remember, `OOS_DAILY_CHECK` will make OOS checks occur daily.

### Incremental Build

The `Alice` build target combines nearly all of the game's source files into one large translation unit which means that changing a single line of source code almost always requires recompiling the entire game. This can take up to 10 minutes depending on build configuration and system specifications even if you change a single line. Therefore it's recommended that any contributors use the `AliceIncremental` build target as this breaks the game into smaller translation units that can be compiled in parallel and also greatly reduces the amount of time required to recompile any changes. Github CI builds the `Alice` target so it will catch any changes that do not compile under the `Alice` target.
