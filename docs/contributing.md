## Interested in contributing?

If you want to be officially part of the team you just need to let me know how you are interested and able in contributing. Right now I am looking for the following sort of people:

Help that I am looking for:

#### Programming (all in C++):
Priority 1: I would like so find someone(s) with OpenGL experience and maybe a slight artistic inclination to work on rendering the map (both as a globe and as a flat map)
Priority 2: I would like a few people to help me with the core programming surrounding the game state and its updates. These are the people I will be working with most directly, so be willing to tolerate a degree of micromanagement.

#### Art:
I would like to find someone who could essentially redo all of the existing Victoria 2 UI assets in a new artistic style to give the game its own distinctive visual identity. Yes, I know that this is no trivial thing.


#### Testers:
We are looking for people who can build a C++ project and run it, but who don't want to (or aren't comfortable) contributing code. Testers are expected to build the project and poke at it on a semi-regular basis to find bugs, especially those that don't appear for the software/hardware combinations that we are developing on. Speak to our project coordinator if you are interested in being a tester.

#### Not looking for at the moment:
Translators -- this is a very late-stage consideration and we are nowhere near there yet.

#### Other:
If you have some other skill or contribution that you think you can make, I am open to suggestions

### Free agents

Since this is an open-source project, you don't have to be an official member of the team to contribute. Anyone can make a PR through github and contribute in an *ad hoc* manner that way.

### Building the project

The build uses CMake and should "just work", with some annoying exceptions.

#### Windows

1. You will need a full copy of git installed (https://git-scm.com/downloads), as the one that ships with Visual Studio is not sufficient for CMake, for some reason.
2. Make sure that you have a relatively up-to-date version of the Windows SDK installed (you can update your local version of the Windows SDK through the Visual Studio installer).
3. Open the project in Visual Studio and let it configure.

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
