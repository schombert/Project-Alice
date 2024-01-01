# Towards a better UI system

## The ultimate goal

Make a WYSWYG tool to design the ui, up to and including naming the functions that describe the ui behaviors. Running the tool on the project files for the ui should produce two things: (a) generated c++ files that include prototypes, etc, for the ui (in other words, as much of the code as possible that doesn't have to be written by hand, so everything except the bodies of functions that define the desired ui behavior). (b) a single file, possibly in a binary format, that encodes all ui information that could be loaded at runtime. (c) a localization file, or possibly a stub of a localization file

The point of not packing everything into the C++ files is to leave as much as possible moddable. However, parsing the project file(s) from scratch is almost certainly too inefficient (opening small files is inefficient on windows in the first place, and anti-virus typically adds an additional cost to file operations). Thus, we will try to pack the ui into a single file. This may make combining mods that both intend to alter the ui harder, but I wil worry about that later.

## Bootstrapping

Working with UI code sucks, and the WYSWYG tool itself is going to need a bunch of complicated ui. Thus, the plan is to write the tool itself with the new ui system. Doing that directly would mean that we would have to use the tool to make the tool, which is impossible. So instead we will need a bootstrapping process. This will mean writing an initial implementation of at least some of the system inside a command line utility that can take a text-encoded project file and produce the various generated files. We will then use those generated files to make the first versions of the proper tool. And once we have enough simple controls and options to make the tool minimally usable, we can use the tool in its simple form to edit more complicated features into its own project files, thereby using the tool to complete itself. It also makes the tool an easy testbed for the system itself as we work on it.

## Text Directionality

I intend to support native RTL text by horizontally flipping ui layouts and most icons. This is a fairly easy transformation if you start out expecting to do it. Now, in a perfect world, I would also like to support vertical text, even though traditional Mongolian script is the only living language that requires it (as far as I am aware). Unfortunately, rotating a layout is much more complicated than mirroring it, because typically we cannot also rotate the dimensions of the top-level container of the ui. This means that rotating the layout requires more complicated logic for redoing it to handle the new dimensions, and past experience tells me that this is a huge headache. So, at least for the immediate future, I don't plan on supporting vertical text (with my apologies to traditional Mongolian).

## General rendering

Rendering will support generic stretching and texturing in various ways with image files (although, maybe not rotation). It will also support "brush fills", which is a technique that does not seem quite as common these days. A brush fill works by consulting a palette. Each entry in the palette will be either a solid color or a repeating texture (for us, although you can imagine adding gradients and other sorts of procedural brushes as well). You then "paint" a monochromatic mask with this brush to create the final image. (And, much like early printing techniques, you might perform two or more passes with different masks and brushes to create a more complicated image.) Personally, I like the idea of using this technique in combination with vector graphics to do various effects. Practically, in this project, this will be essential for text rendering. We will be rendering text monochromatically to a texture and then using that texture as a mask to draw the actual text on screen. This allows us to switch brushes to easily render parts of the text differently (for example, to show text selection).

We may also need to support clip masking in some way to simplify the rendering process. In my earlier project, I implemented ui elements that popped out of their container (such as expanding menus) by setting up clipping so that latter elements could not be drawn over them. I don't know how easy that will be to do in opengl, so some other solution may be needed there.

We need to be able to render part of the ui to a texture in order to be able to render ui animations.

## UI behavior

A ui that can't do things is pretty worthless. However, to make things easier we want to say what a button does in the ui creation tool, rather than having to connect it to behaviors in c++ (painful).

### Exposing functions

To make that possible, we have to do two things. First, when we load the "compiled" ui files we will need to fill in function pointers to make the behaviors work. This is partly where the generated c++ code comes in. When making the project, we will map each behavior function to an id, and in the generated code we will populate an array with the appropriate function pointers with those same ids. Thus, loading will be able to fill in the function pointers appropriately using just the ids from the generated file. (For mods to work, there will obviously have to be a toggle in the tool that fixes the existing ids and prevents new ones from being added so modders don't accidentally break things that would require a re-compile.)

### Getting properties

A property will be retrievable up the ui tree using a combination of compile-time type indices and code-generated identifier values. You will be able to access a value both by value (in which case you get a default constructed value if it is not found) or by pointer (with nullptr if it is not found). Since *which* properties an element can have are going to be partly controlled by the file we load at runtime, this will probably also involve some generated code tricks. In particular, we will probably allocate memory for both the ui object and some variable trailing space for its properties + information about which properties those are (for sanity, all 0 initialized). A fast version of this will also be available for an element to get one of its own properties without going up the tree.

(Note: how do you get a particular property? Each type of element has an id that gets registered in a table. You then index into the table with the property you want and it tells you either that the property is not available or its offset inside the trailing section.)

### Calling functions

Similarly, there is a need sometimes to be able to call functions on other ui elements. This is done via the table mentioned above, which allows us to go from an id to an implementation of that function class. We will provide methods both for calling the function on the nearest parent that supports it, for calling it on all of the nearest children that support it, and self calls.

### TODO

- Some way of exposing specific, unique elements that exist somewhere in the hierarchy globally (the need to open / close specific windows, for example -- can this be handled with specific functions? -- no search would be too long potentially -- needs some sort of automatic registration method)

## Localization and Translation

We will be using a system that is also generally based off work in the PrintUI project, but somewhat complicated because mods want to be able to combine additional text together. (Unlike with ui modding, which we can probably assume that only one mod will want to touch at a time.) Obviously, looking things up with text keys at runtime is totally impractical. At the same time, we need to load the text at runtime to enable locale switching, and we probably don't want to stash all text from all languages inside the scenario file. Thus, the scenario file will store the keys it saw on creation and slots for their text content. On launch, we will load the content of the localization files and point each key at its unparsed contents. Finally, we will parse the associated text as it is needed in the ui and cache the result. Since this text is only used in the ui, we can also safely reload the files (to switch language) in the ui thread.

We will do localization via storing for each language and script combination a directory in the localization directory (with a z, which will keep it distinct from the v2 directory). For example, we might have localization\en-US. Inside that directory will be a fonts directory and a text directory. The text directory will include 1 or more .txt files containing the text that matches particular keys. The fonts directory will include a number of font files (in .otf and .ttf formats) and number index files. Each index file will describe one of the font slots that is referred to in the ui. The index file will form a hierarchy of fonts, starting from a base font where glyphs are drawn from first, and then naming fallback fonts to use (in order) to try when glyphs are not found. The index file will also store various font parameters (including size, but not including color) that define what a given font slot is supposed to look like (e.g. weight, whether it should be italic, etc).

## Summary: Generated code

- includes a map of type ids to sizes
- includes a map of type ids to default constructor and destructor functions (from void*)
- provides prototypes for all the behavior functions that need implementations
- includes a map of behavior function ids to function pointers
- map of special names that will point to globally findable ui elements when they are created (should be in the root somehow in case we want to support creating multiple instances of our ui collection)

## Summary: Loading compiled ui

- loads definitions of all ui element types (possibly remapping internal ids if we support loading from multiple files
- stores any required function pointers
- eventually ... instantiates root element

## Summary: Loading localization file(s)

- lookup key in keymap -> point key slot contents to unparsed text
- will need backup handling for v2 localization files (needed for events, etc)

## Summary: Library files

- implements ui logic for event handling
- implements rendering logic
- implements layout
- implements accessibility & text services
- implements ui element instantiation functions