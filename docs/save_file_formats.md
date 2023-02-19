## Formats for save and scenario files

In addition to the settings files, we generate two types of files: save files and scenario files. A scenario file includes the information that defines the starting situation for a game loaded with a particular combination of mods. (Each distinct combination of mods, including running the game without any mods, will require its own scenario file.) A safe file, in contrast just contains the information about the game state that may have changed from that given in the scenario file. Thus, the definition of a particular event would be found in the scenario file, but not in a save file, because that information cannot change during gameplay. However, the current prestige of a nation would be found in both a scenario file and a save file. Starting a new game requires only loading the scenario file. Resuming a save game, however, requires first loading the appropriate scenario file and then loading the save file.

In terms of compatibility, the data produced by the Data Container library is fairly robust, as it is possible to load data that has been stored with more or fewer object or properties, or even if the properties have changed type. However, the data that lives outside the Data Container is stored much less robustly. When that sort of data changes we will have to increase a file version number and simply reject files with the wrong version. Hopefully this won't happen too much, and hopefully we can confine most of our revisions to things that are in the Data Container.

### Scenario file

A scenario file is composed of three main sections. First, there is a short header section. The exact contents of the header haven't been decided upon yet, but the goal is to store information here that will allow the launcher to display information about the scenario without having to fully load it. Any checksum we generate will probably go into the header as well. The second section of the file contains the scenario data as a binary blob. This includes the map and text data, but contains only the file names of various other assets, not their complete contents (so graphics are not also packed into the scenario file). Finally, the third section contains what is essentially a save game that represents the initial state of the world. While this information is required when starting a new game, it does not have to be loaded when the scenario file is being loaded along with an existing save file.

#### Binary layout

#### Header

```
4 bytes   |   (little-endian) integer containing the length of the header section in bytes (not counting these first four bytes)
4 bytes   |   version number -- increases monotonically; files with the wrong version number will not be loaded
N bytes   |   remaining contents of the header section (TBD)
```

#### Scenario

```
4 bytes   |   (little-endian) integer containing the length of this section in bytes (not counting these first 8 bytes)
4 bytes   |   (little-endian) integer containing the decompressed size of this section in bytes
N bytes   |   the compressed (using zlib) contents of this section
```

#### Initial game state

```
4 bytes   |   (little-endian) integer containing the length of this section in bytes (not counting these first 8 bytes)
4 bytes   |   (little-endian) integer containing the decompressed size of this section in bytes
N bytes   |   the compressed (using zlib) contents of this section
```

### Save file

Like the scenario file, a save file contains a shorter header followed by a longer section containing most of its data. In addition to containing details required to explain the contents of the save (such as the player's nation and the game date), the header also denotes which scenario file is required to load the save (including possibly a checksum for it).
