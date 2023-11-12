## The file system wrapper

The file system wrapper is contained in the namespace `simple_fs`. Include `simple_fs.hpp` to use it.

### Some background

While there are file system routines that ship with C++, we need to imitate the behavior of Victoria 2 in order to properly load its mods. Victoria 2 uses the [PhysicsFs](https://icculus.org/physfs/), which we know because [one of the developers told us as much](https://mropert.github.io/2020/07/26/threading_with_physfs/). So why not use PhysicsFS. Well, for two reasons. One it is an additional dependency, and every dependency makes life just a little bit harder. More importantly, the game actually seems to use quite few of its capabilities. As far as we are concerned, the file system works like this: We add a sequence of "root" directories upon loading into a mod (in a game with just one mod, this would be the root directory for the game's assets and then the mod directory). Then, whenever we try to open a file, we first look for it in the most recently added root (in our example, the mod directory). If it isn't there, we then check for it in the preceding root, and so on. This allows mods later in the loading order to replace files from earlier in the loading order.

Let's be clear: this isn't a great way to do modding, because it means that mods can't add to files or adjust them, only overwrite them completely. When we are talking about one mod replacing some files in the base game, this works great. But it also makes it extremely hard for mods to cooperate, because they can't both contribute alterations to the same file. But, it is what we are stuck with.

You may also wonder why the file system wrapper is implemented as a bunch of free functions, even though there are four `class`es defined for it. The simple reason is that the data stored in these classes needs to vary depending on the OS that the program is compiled for (for example, the `file` class contains both a windows file handle and a handle to a memory mapping). Thus the actual definition of the classes ends up squirreled away in system-dependent header files. And to ensure that the interface for interacting with the file system remains the same no matter what OS you are building for, it could not be placed in those files. Thus, it is a bunch of free functions. (Yes, I could also have inherited from the implementations and defined the member functions in the shared header, but it just didn't seem worth the effort.)

### Native string types

In order to not make either Windows or Linux perform worse than the other, any strings that need to be sent to the OS or retrieved from it are stored in their native format. In order to not have two versions of everything, the following defines are used to manipulate native strings:

`native_string` -- this is `std::wstring` on Windows and `std::string` on Linux
`native_string_view` -- this is `std::wstring_view` on Windows and `std::string_view` on Linux
`native_char` -- this is `wchar_t` on Windows and `char` on Linux
`NATIVE(...)` -- this is a macro that you can use on string or character literals to turn them into their native equivalents, for example such as with `NATIVE("text")` or `NATIVE('x')`.

### The file system object

The class `file_system` represents the file system as a whole. There should only be one of these objects created during the lifetime of the program, and it should never be moved or relocated (other file system objects may maintain pointers to it). There are six functions for manipulating it:

- `void reset(file_system& fs)` -- This function resets the `file_system` to its just-constructed state. This function will probably never need to be used.
- `void add_root(file_system& fs, native_string_view root_path)` -- This function adds a new root to the file system. Currently we have not decided whether these should be all relative to the current working directory, or whether they should be specified absolutely. Absolute names tends to be more robust, as during development it is easy to simply specify arbitrary paths to find the necessary files on, regardless of where the compiled executable is created or launched from. However, as we get closer to release, it is probably going to be more user friendly to derive these paths from the location of the executable. If we do things via relative paths, then maybe we can make things not too painful for us by launching the debug executables with a current directory set as if they were in the right location. If we were to use absolute paths, then we would instead have to add CMake trickery to make everyone's builds be created wherever their game assets live.
- `void add_relative_root(file_system& fs, native_string_view root_path)` -- This function has a very misleading name, but I didn't have a better idea. It adds a new root composed of the specified path appended to the location of the exe file. **Caution:** if we decide to use relative paths, we won't be able to use this function.
- `directory get_root(file_system const& fs)` -- Returns a directory object representing the root of our file system (but remember, our file system is a bunch of overlapped roots, not a single place in the file hierarchy). Since you need a directory object to get anything else done, most interaction with the file system will start with this function.
- `native_string extract_state(file_system const& fs)` -- This function is used to package the collection of roots into a single string (see the next function for more information).
- `void restore_state(file_system& fs, native_string_view data)` -- This function sets the collection of roots for the files system back to the state it was in when the previous function call was made. The point of these two functions is to be able to save the way that the roots were when a scenario is made from a collection of mods. While we do bundle most of the information into the scenario file, we don't try to stuff in any of the art or sound assets there. Thus to play the scenario correctly we need to recreate the desired state of the file system to find the desired art and music assets.
- `void add_replace_path_rule(file_system& fs, directory const& dir, native_string_view path)` -- Adds a "replace_path" rule to override a directory, this redirects all accesses of the directory to the given path
- `std::vector<native_string> list_roots(file_system const& fs)` -- Obtain a list of the root paths registered on this filesystem.
- `bool is_ignored_path(file_system const& fs, native_string_view path)` -- If this specific instance of a path should be ignored, for example if we have a mod override the folder "map/", then to properly perform the behaviour of ignoring the vanilla files we ignore all the vanilla files and pretend they don't exist, but we do not ignore the mod folder however.

### The directory object

The class `directory` represents a "folder" in the file system (or, more accurately, a superposition of folders in the different roots. When reading files, you will get directories via the `file_system` object, either directly or indirectly. However, as described in a later section, there are also a few special directories you can get that represent special locations in the user's file system, which are not affected by the roots given to the `file_system`. Only these special directories can be used to create or write to files. There are six functions for manipulating directories:

- `std::vector<unopened_file> list_files(directory const& dir, native_char const* extension)` -- This function creates a list of files that you could open within directory `dir`. If you pass a non-empty string to `extension`, for example `NATIVE(".txt")`, you will get only the available files ending with that extension.
- `std::vector<directory> list_subdirectories(directory const& dir)` -- This function lists all the subdirectories within the current directory that *do not* begin with `.`.
- `std::optional<file> open_file(directory const& dir, native_string_view file_name)` -- This function opens the file with the desired name and places its contents in memory, if possible. If the optional does not hold a `file`, the the file could not be opened. Don't add any system-dependent path separators to the file name; even if you could make it work, it wouldn't be portable.
- `std::optional<unopened_file> peek_file(directory const& dir, native_string_view file_name)` -- The function can be used to check whether a file with the given name exists. If it does, the optional will contain an `unopened_file` when the function returns. You can then use this `unopened_file` to open the file with less overhead.
- `directory open_directory(directory const& dir, native_string_view directory_name)` -- This function opens a directory located within the current directory. This function will always appear to succeed, even if there is no such directory. (Use `list_subdirectories` if you need to check which subdirectories are available). Only attempt to open one new "level" of directories at a time. **Do not** pass any system-dependent path separators.
- `void write_file(directory const& dir, native_string_view file_name, char const* file_data, uint32_t file_size)` -- This function can only be called with one of the special directories mentioned above. When you call it, a file with the name specified will be created, if it does not exist, and then opened. Then `file_size` bytes from `file_data` will be written to it, erasing any previous data in the file. Then the file will be closed and the function will return (this is not an asynchronous function).
- `native_string get_full_name(directory const& dir)` -- this returns the name of the directory, along with the names of any parent directories it exists under relative to the file system root(s), separated by the platforms native separator (`\` on Windows, `/` on Linux).

### The unopened file object

The class `unopened_file` represents an file that exists in the file system, and which you could open, but which has not yet been opened. There are only two functions you can call on this class:

- `std::optional<file> open_file(unopened_file const& f)` -- This function attempts to open the file and load its contents into memory. If successful, the optional will hold the file object, otherwise it will be empty.
- `native_string get_full_name(unopened_file const& f)` -- This gets the name of the file along with any of the directories that were used to get to it, as well as the particular root that it belongs to. You can use this to send to the file to some other function without reading data form it yourself.
- `native_string get_file_name(unopened_file const& f)` -- Obtain the filename only

### The file object

This represents an open file that has its contents loaded into memory (on Windows, it is memory mapped). The destructor of the file object will close the file and unload it from memory, so make sure that the file object outlives any use of its contents. There are two functions you can call on this class:

- `file_contents view_contents(file const& f)` -- This function returns the `file_contents` object, which contains a non-null `char const*` called data that points to the first byte of the file's contents, and a `uint32_t` member called `file_size` that contains the number of bytes in the file. Do not attempt to modify these bytes.
- `native_string get_full_name(file const& f)` -- This gets the name of the file along with any of the directories that were used to get to it, as well as the particular root that it belongs to.

**Linux**: The file may be memory-mapped (that is, a virtual memory mapping of the file from the disk, optimized by the OS) if available, however if this functionality is lacking (or the libc is from some of those "distros") the files instead will be allocated and their entire contents read onto the memory. The behaviour of the former decreases physical memory usage whereas the latter increases it.

### Special directory functions

There are currently three special directory functions, which are mainly intended to locate the right place to save various kinds of information.

- `directory get_or_create_save_game_directory()` -- On Windows this opens (creating if necessary) a folder in the user's documents folder named `Project Alice\saved games`. This is where we will be saving games.
- `directory get_or_create_scenario_directory()` -- On Windows this opens (creating if necessary) a folder in the user's documents folder named `Project Alice\scenarios`. This is where we will be storing the scenarios we create by bundling the data from a particular collection of mods and base files together.
- `directory get_or_create_oos_directory()` -- On Windows this opens (creating if necessary) a folder in the user's documents folder named `Project Alice\oos`. This is where we will be adding OOS dumps.
- `directory get_or_create_settings_directory()` -- On Windows this opens (creating if necessary) a folder in the user's local application settings directory named `Project Alice`. We will store global program settings here. **Do not** write large files to this directory. In particular, **do not** write logs here. If we need a place for logs, I will create a new special directory function for them.

### Encoding conversion functions

Sometimes we need to read a directory or file name from within another file. I believe that the game's base files are in the win1250 code page, but maybe someone could double check that for me. In general, any user accessible files we write will be in utf8. So to convert to and from these formats and the native format of the file system, the following functions are provided:

- `native_string win1250_to_native(std::string_view data_in)`
- `native_string utf8_to_native(std::string_view data_in)`
- `std::string native_to_utf8(native_string_view data_in)`

Even though some of these function may effectively do nothing on Linux, you still need to wrap any transition to or from native strings in them to remain portable. Try to keep native strings in their native format wherever possible.
