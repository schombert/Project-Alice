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

The build uses CMake and should "just work", with one important exception. Because the project in its current state needs to use the existing game files (as a source of graphics, for starters), everyone needs to tell the compiler where their copy of the game is installed. You do this by creating a file named `local_user_settings.hpp` in the directory `src`. That file should contain the following four lines (the last one is an empty line):
```
#ifndef GAME_DIR
#define GAME_DIR "D:\\programs\\V2"
#endif

```
except replacing the path there with your installation location. Note that on Windows you need to write `\\` instead of just `\` for each path separator. (Linux does not have this issue, and you can write a single `/`)
