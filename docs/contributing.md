## Interested in contributing?

Help that I am currently looking for:

#### Programming (all in C++):

Priority 1: We could use more programmers to help finish the UI. There are a lot of complicated tool tips and other detail work that more programmers, even relative novices, could make meaningful contributions to. The major work on the UI is currently wrapping up, so this may soon disappear as an on-ramp for new contributors.
Priority 2: Making a cooler map. I am basically satisfied with the map as it currently stands, but maybe someone with the right combination of OpenGL experience and artistic talent could do something really cool with it.
Priority 3: AI work. We aren't actually to the AI-writing stage quite yet, but we are getting close to it (currently scheduled to start in July), and anyone with experience with that sort of work would be welcome to help with preparation and planning for it.
Priority 4: General bug fixing and polish. We need people who are able to run the game to find bugs, both of the program-crashing variety and simply places where the rules of the game have been implemented incorrectly, and who are then able to find the causes of those issues and, ideally, fix them.

#### Art:

I would like to find someone who could essentially redo all of the existing Victoria 2 UI assets in a new artistic style to give the game its own distinctive visual identity. Yes, I know that this is no trivial thing.

#### Testers:

We are looking for people who can build a C++ project and run it, but who don't want to (or aren't comfortable) contributing code. Testers are expected to build the project and poke at it on a semi-regular basis to find bugs, especially those that don't appear for the software/hardware combinations that we are developing on. Speak to our project coordinator if you are interested in being a tester.

#### Other:

If you have some other skill or contribution that you think you can make, I am open to suggestions

### Free agents

Since this is an open-source project, you don't have to be an official member of the team to contribute. Anyone can make a PR through github and contribute in an *ad hoc* manner that way.

### Building the project

The build uses CMake and should "just work", with some annoying exceptions.

#### Windows

1. You will need a full copy of git installed (https://git-scm.com/downloads), as the one that ships with Visual Studio is not sufficient for CMake, for some reason.
2. Make sure that you have a relatively up-to-date version of the Windows SDK installed (you can update your local version of the Windows SDK through the Visual Studio installer).
3. Open the project in Visual Studio and let it configure (CMake should run in the output window and download dependencies; this may take some time).
4. Go look at the "Final Touches" section at the bottom of this page.

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
2. `cmake -E make_directory build`
3. `cmake -E chdir build cmake ..`
4. `touch src/local_user_settings.hpp`
5. `nano src/local_user_settings.hpp` or use the text editor of your choice
6. add the following lines:
    ```cpp
        #ifndef GAME_DIR
        #define GAME_DIR "[insert file path here]"
        #endif
    ```
    substitute the value otherwise Alice wont work, if you downloaded it on steam then you can just right click Victoria 2 and browse local files
    copy the file path and replace [insert file path here] with it, then save.
7. `cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..`
8. `cmake --build . -j$(nproc)`

#### Note on Non-x86 Platforms

PA Does not support non-x86 platforms, as it relies on some optimizations which are specific to x86

##### Do I have an x86 platform?

Chances are you do, examples of non-x86 platform that is somewhat more common nowadays
Apple M1 and onwards are not (This is present in their newer iMacs)
Raspberry Pi are not (All of them, these all don't use x86)

Other platforms (ppc, mips, sparc etc) are likewise unsupported.

#### Final touches

Because the project in its current state needs to use the existing game files (as a source of graphics, for starters), everyone needs to tell the compiler where their copy of the game is installed. You do this by creating a file named `local_user_settings.hpp` in the directory `src`.

That file should contain the following four lines (the last one is an empty line):
```cpp
#ifndef GAME_DIR
#define GAME_DIR "C:\\Your\\Victoria2\\Files"
#endif

```
except replacing that path with your own installation location.

Note that on Windows you need to write `\\` instead of just `\` for each path separator. (Linux does not have this issue, and you can write a single `/`)
Second note: on Windows, BrickPi has made a change such that, if you have Victoria 2 installed, you may be able to bypass creating `local_user_settings.hpp` completely. You may want to try that first.

Finally, just build the Alice launch target, and you should see the game pop up on your screen.
