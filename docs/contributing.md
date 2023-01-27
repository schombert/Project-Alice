## Interested in contributing?

If you want to be officially part of the team you just need to let me know how you are interested and able in contributing. Right now I am looking for the following sort of people:

Help that I am looking for:

#### Programming (all in C++):
Priority 1: I would like so find someone(s) with OpenGL experience and maybe a slight artistic inclination to work on rendering the map (both as a globe and as a flat map)
Priority 2: I would like a few people to help me with the core programming surrounding the game state and its updates. These are the people I will be working with most directly, so be willing to tolerate a degree of micromanagement.

#### Art:
I would like to find someone who could essentially redo all of the existing Victoria 2 UI assets in a new artistic style to give the game its own distinctive visual identity. Yes, I know that this is no trivial thing.

#### Not looking for at the moment:
Translators -- this is a very late-stage consideration and we are nowhere near there yet.

#### Other:
If you have some other skill or contribution that you think you can make, I am open to suggestions 

### Free agents

Since this is an open-source project, you don't have to be an official member of the team to contribute. Anyone can make a PR through github and contribute in an *ad hoc* manner that way.

### Building the project

The build uses CMake and should "just work". To make the tests and main program work, you will also need to set the CMake `GAME_FILES_DIRECTORY` cache variable to where the existing game files are located. There are a number of ways to do that, but here is the very non standard way that I found to be easiest. First run CMake once. Then go to the `out\build\your-configuration-goes-here` directory and open the `CMakeCache.txt` files. Seach for the text `GAME_FILES_DIRECTORY` and change the line you find it on to something like `GAME_FILES_DIRECTORY:PATH=D:/Programs/V2` (note that CMake likes the forward slashes). Then run CMake again and everything should work fine.