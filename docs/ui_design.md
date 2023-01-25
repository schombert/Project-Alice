## User interface design document

This document **does not** cover the map / map rendering. This document **does not** cover the code for the ui system. This document cover the general ideas behind the ui system / plans for writing the code for it. This document also only covers the issues at a relatively high level. Specific details may only appear in the documentation for its implementation.

### Working with OpenGL

The logic for rendering all ui elements is contained in a single fragment/vertex shader pair. To get specific rendering capabilities for specific types of ui elements (for example to render a piechart instead of a simple textured rectangle) various combination of functions in it can be switched on or off. Ideally these will just be copied over from open v2 and then I will never touch them ever again.

### Quirks

Most UI systems would work as follows: every ui node (I often refer to such things as elements) has a bounding rectangle, and then any input and rendering work within that rectangle is negotiated between that element and its children. This means that the tree structure allows you to effectively filter out most events so that you don't have to look at most elements to determine what happens with a mouse click.

This does not appear to be how Paradox has made their ui system work. It appears to me that, in some places, elements intentionally position their children outside themselves. That this means is that messages must be sent everywhere, because even though a message is not relevant to the parent node, it could be relevant for a child node or some child's children. This is unfortunate, but I think it is easier to emulate this nonsense. Open V2 went with the saner approach, but it meant that I was constantly applying dynamic "fix ups" to elements to make children fit within parents, which created a not-insignificant extra inconvenience (because I could never trust that a gui description would just work as-is).

### Events and messages

Events (such as a mouse click) are passed through the ui elements in some order (usually from "topmost" down). With some events, such as a mouse click, once an event handler signals that it has consumed the event, the process stops immediately. Other events, such as mouse movement, are simply noted as having been intercepted by someone. Once events have been passed through the UI system, any that weren't handled in some way (meaning, that they were not "over" or "for" any ui element) will then be passed on to the map.

### Custom behavior

The key feature to a ui system is making the various controls do things. For this we first need to be able to attach event handling routines to a ui element. Each ui element will contain a `unique_ptr` that may point to a custom handling-routine. To attach these pointers, when we create a ui element, we will use its name plus some hash-map to look up whether there is global behavior registered for that type of element. If there is, the map will provide us with a function to create a new `unique_ptr`.

### Custom data

Most ui elements need to store some data to determine what they display / how they behave. This is managed in two ways. First, on element creation, when an element becomes visible, and whenever the game state changes, update messages are propagated to visible ui elements allowing them to pull in data from the game state. Secondly, to avoid coping the same data everywhere (for example, if the nation window tracks which nation you are looking at, not every subcontrol needs to duplicate that information), Each control will be given the ability, in conceptual term, to fill out a form requesting some piece of information, which will be passed up the ui hierarchy to see if any element can fill it out. Thus, for example, controls in the nation window can simply ask if there is any nation stored and get back the answer from the window containing them. This will be accomplished, on the technical side of things, with `std::any`. An element will initialize a `std::any` with a default-constructed type for the kind of data it wants. This will then be passed upwards until someone can populate it with data. (If necessary, the same mechanism will also be used externally to set the data stored in controls outside of the usual update mechanism.)

### Memory management

We won't do any of it. Specifically, no ui element or the behavior behind the pointer it points to will ever be deleted or moved in memory. When we want something in the ui to go away, we will just hide it. In many cases this will work trivially because there is only a unique instance of that bit of ui, conceptually speaking. There is only one province window, for example. If we do encounter a place where we need a more-or-less unbounded number of ui elements (say we are making ui elements to place over each map province in view), what we will do is pull those elements from a pool, reusing elements that are currently not marked as visible until we exhaust the pool before creating new ones. These pools should naturally find an upper bound, at which point no further ui element creation or deletion will be done at all. The point of all this is that, primarily, it allows us to freely store pointers to ui elements without worrying about it, as the worst that can happen will be that you are talking to an invisible element or one that has been reused.