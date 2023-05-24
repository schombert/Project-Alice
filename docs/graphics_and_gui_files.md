## Graphics and gui definitions

The information required to render an approximation of the Victoria 2 UI is contained in the `.gfx` and `.gui` files contained in the `interface` directory. The `.gfx` files describe, roughly, the assets that the UI rendering can call upon (i.e. the graphics), while the `.gui` files contain descriptions of the placement of those assets and the general type of behavior to associate with them (i.e. the gui). Since the `.gui` files may refer to graphics assets, it is natural to parse the `.gfx` files first.

Note that, since our parser converts everything to lowercase when doing a comparison anyways, I record all keywords in lowercase, whatever their capitalization was in the game files.

### `.gfx` files

The top level of a graphics file must be a group possessing one of the following keys `bitmapfonts` (font information), `fonts` (more font information), `lighttypes` (3D lighting definitions), `objecttypes` (3D object definitions), and `spritetypes` (2D graphic asset definitions). Since we will be using proper fonts, and we won't be supporting 3D models, the only one of these groups that is of interest to us is `spritetypes`, and we will discard the rest.

A `spritetypes` group is itself composed of groups, with the the following keys: `spritetype`, `corneredtilespritetype`,  `maskedshieldtype`, `textspritetype`, `tilespritetype`, `progressbartype`, `barcharttype`, `piecharttype`, `linecharttype`, and `scrollingsprite`. Of these, `scrollingsprite` isn't useful to us, and we will simply discard it. The rest will all be parsed in the same way, and into identical destination objects, and we will simply mark which of the types that the group was declared with. Although it may not matter at the moment, `progressbartype`, `barcharttype`, `piecharttype`, `linecharttype` all encode something about the behavior of an object that references them, unfortunately blurring the distinction between graphics and gui definitions. `corneredtilespritetype` are used to render expandable backgrounds; you render them by stretching a center rectangle in all directions while keeping the corners fixed, and expanding the edges in only one direction. `tilespritetype` represents a number of sprites arranged in a grid within a single image files. And `maskedshieldtype` represents a mask that is used to "cut out" parts of other textures. Everything else is just an image.

This brings us to the bulk of the parsing, which is to record the data in each of these objects. Most of the things found here are simple values:
- `name` : the name the asset is referred to by in the `.gui` files
- `horizontal` : a boolean distinguishing a horizontal from a vertical progress bar
- `allwaystransparent` : a boolean, but I don't know what it does. Still, it was easy to save, "just in case"
- `transparencecheck` : a boolean determining whether transparency matters for hit detection
- `flipv` : a boolean determining whether the asset should be displayed flipped vertically
- `noofframes` : an integer telling us how many sub-sprites are packed into the image
- `size` : this is a tricky one; either a single integer or a group with `{ x = ... y = ... }`, which determines the size to display the asset, in pixels. A single value means that the asset is square
- `clicksound` : appears to be something interesting, but actually always holds the text `click`; it might as well be a bool
- `texturefile` or `texturefile1` : the primary texture for the asset
- `texturefile2` : Present for either a `maskedshieldtype` or a `progressbar`. In the first case, this is the mask file while the other texture is an overlay to be displayed over the result. In the case of a progress bar, one texture serves as the bar and the other as the background.
- `bordersize` : a group containing `{ x = ... y = ... }`, which is used for `corneredtilespritetype` to determine how big the edges that are not stretched are. `x` and `y` always seem to be the same, so we only store one of them.
- `color`, `color2`, `colortwo` : no idea what these are supposed to be for, so I ignore them.
- `effectfile` : appears to control the directX shader for the control. We just ignore this.
- `loadtype`, `noreftype`, `norefcount` : all look like internal loading management controls; we ignore them.
- `linewidth` : is always 2, so we ignore it.

And, as strange as it seems, that is all the useful information there is to be had in the .gfx files.

### `.gui` files

The `.gui` files are significantly more complicated, in part because we aren't just throwing away a good portion of what they contain. Like the `.gfx` files, the top level of a `.gui` file is composed of top level groups that gather a number of definitions of a common type. Or at least I suppose that was the intent, because the top level contains only `guitypes` groups, as far as I can tell. So, the only thing interesting to us is the contents of those groups.

Within them are lists of groups defining ui elements, each of which is labeled by its type (in terms of general behavior it is supposed to display). The possible top level types are: `guibuttontype`, `checkboxtype`, `eu3dialogtype`, `icontype`, `instanttextboxtype`, `listboxtype`, `positiontype`, `scrollbartype`, `windowtype`, `shieldtype`, `overlappingelementsboxtype`, `editboxtype`, and `textboxtype`. For the sake of simplicity we combine the parsing of `eu3dialogtype` and `windowtype` elements, treating both as the same thing, and we parse `instanttextboxtype`, `editboxtype` and `textboxtype` as a common text element type, we parse `shieldtype` and `icontype` as a common image type, and we parse `guibuttontype` and `checkboxtype` as both buttons.

Except as just mentioned, each type has its own set of unique properties and is parsed uniquely (unlike the contents of the `.gfx` files, which are all put through the same parsing path).

#### Quirks and caveats

Some files that come with the vanilla game are malformed in some way or have missing GFX resources - thus they are ignored and are never parsed:

- `confirmbuild.gui`
- `convoys.gui`
- `brigadeview.gui`

If a mod depends on those files being parsed and/or uses them for something - then don't. Before, `eu3dialog.gui` was ignored too, however it's now parsed and used, but the graphics element `GFX_icon_merchant` is overriden, if a mod depends on this not being overriden (for whatever reason); let us know.

#### A quick note on fonts

Some of these elements will refer to various fonts. Since we won't be using any of the fonts from the game directly, trying to decode the font descriptions and interpret them seems like a waste of time. Instead we will just recognize fonts by hard-coding the names of the fonts to later assign to fonts of our choosing. Each font name starts with one of the following: `arial`, `fps`, `main`, `tooltip`, `frangoth`, `garamond`, `impact`, `old_english`, `timefont`, or `vic`, and is then followed by an optional underscore, and then an optional number (presumably size), and then an underscore and a final bit of decoration such as `bold`,  `black`, `black_bold`, or `bl`. We will deal with this all by mapping the start of the name to one of a smaller number of fonts we will load, parsing out the size (if present), and then determining whether to render the font in white (the default) or in black.

#### Buttons (`guibuttontype` and `checkboxtype`)

- `position` : a group with contents `{ x = ... y = ...}`
- `size` : a group with contents `{ x = ... y = ...}`
- `delayedtooltipText` : text, at least nominally. After an initial inspection, it looks like it is always assigned `""` when it appears, so I am discarding it for now.
- `format` : text value containing `right`, `left`, `center`, or `centre` (with possibly varying capitalization), determining how text is aligned in the control.
- `spritetype` : text naming a graphics resource to draw the element with (appears only for `icontype`, I think, but why not put it here just in case? 
- `rotation` : floating point value plus or minus 1.5708 (i.e. a 90 degree clockwise or counter-clockwise rotation).
- `shortcut` : shortcut key to associate with the control
- `tooltip` and `tooltiptext` : apparently all set to `""`
- `name` : text label that can be used to refer to this element
- `quadtexturesprite` :  text naming a graphics resource to draw the element with
- `buttontext` : text key referring to text loaded from the language files
- `buttonfont` : the font to render the text with, as described above
- `orientation` : `center`, `lower_left`, `lower_right`, `upper_left` (or `upperl_left`, as it is misspelled once), `upper_right`, `center_up`, or `center_down`; determines how the position is to be interpreted with respect to the parent element.
- `clicksound` : one of `click`, `close_window`, or `start_game`
- `parent` : I don't know what this is supposed to do, so I ignore it.

#### Images (`icontype` and `shieldtype`)

- `position` : a group with contents `{ x = ... y = ...}`
- `orientation` : as above
- `buttonmesh` : no idea what this is supposed to do, so I ignore it
- `frame` : when the graphics asset is a strip of icons in one image, selects one of those icons
- `name` : as above
- `spritetype` : as above
- `rotation` : as above
- `scale` : a floating point value describing how much to stretch the icon by

#### Text boxes (`instanttextboxtype`, `editboxtype` and `textboxtype`)

- `orientation` : as above
- `allwaystransparent` : as with the graphics assets, I don't know what this means, but I store it anyways
- `fixedsize` : another minor mystery. I store this but don't make use of it
- `font` : as above
- `format` : as above
- `maxheight` and `maxwidth` : suggest that the text box may be smaller, but generally I either size text boxes to their contents (as with tooltips), or create them at this size.
- `name` : as above
- `size` : a group with contents `{ x = ... y = ...}`, which I store in the same place as the `maxheight` and `maxwidth` values, removing any distinction between them.
- `text` : text key referring to text loaded from the language files
- `texturefile` : if present may be set to `""` or one of `gfx\\interface\\tiles_dialog.tga`, `gfx\\interface\\transparency.tga`, or `gfx\\interface\\small_tiles_dialog.dds`, which I just store as no background or background 1, 2, or 3.
- `position` : as above
- `bordersize` : a group with contents `{ x = ... y = ...}`, adds an extra internal margin to the text box where the text will not be positioned.

#### `listboxtype`

- `orientation` : as above
- `allwaystransparent` : as above
- `background` : text, referencing a graphics object to render the background of the control with
- `horizontal` : a boolean value, but I don't support values equivalent to `true`, and I don't think the files contain any
- `name` : as above
- `priority` : unknown, always appears to be set to 100
- `scrollbartype` : appears to be always `standardlistbox_slider`, and that is the only value we accept
- `spacing` : a small integer, sometimes written as a floating point value. I don't know what to do with this
- `step` : unknown, always appears to be set to 0
- `position` : as above
- `size` : as above
- `bordersize` : as above, but I have no idea what it is supposed to do for list boxes
- `offset` : a group with contents `{ x = ... y = ...}` ... can't quite recall what this one is for, but I think it has to do with positioning the list items.

#### `positiontype`

- `name` : as above
- `position` : a group with contents `{ x = ... y = ...}`; elements of this type represent positions that layout routines could refer to.

#### `scrollbartype`

- `horizontal` : whether the scrollbar is horizontal or vertical, but written as `1` and `0` instead of the usuals
- `leftbutton` and `rightbutton` : refer by name to sub object *buttons* of this object to determine which buttons cap the scroll bar
- `lockable` : a boolean value, but I don't know what it does
- `minvalue` and `maxvalue` : the numeric limits, as integers, of the scrollbar. However, `minvalue` is always 0, so we ignore it.
- `name` : as above
- `priority` : always is 100; not sure what the purpose of this is
- `rangelimitmax`, `rangelimitmin` : we ignore these
- `rangelimitmaxicon` and `rangelimitminicon` : refer by name to the sub object *icons* of this object that may be used to indicate limits for the slider short of the full length of the range
- `slider` : refer by name to a sub object *button* of this object to determine which button to use as the slider
- `startvalue` : we ignore this
- `stepsize` : determines the intervals that the slider can stop at. May be `2`, `1`, `0.1`, `0.01`, or `0.001`
- `track` : refer by name to a sub object *button* of this object to use as the background for the scrollbar
- `userangelimit` : a boolean value that determines whether the slider can be restricted to only a portion of the scrollbar
- `position` : as above
- `bordersize` : as above, but I have no idea what it is supposed to do for scrollbars
- `size` : as above
- `guibuttontype` and `icontype` : these are groups that define buttons and icons, respectively, in the ordinary fashion. However, their names need only be visible from within the scrollbar itself.

#### `overlappingelementsboxtype`

These are the lists of flags or other lists of icons you see in a few places.

- `orientation` : as above
- `format` : as for text boxes and buttons, except that it determines the alignment of the list of icons
- `name` : as above
- `spacing` : a float defining the ideal space between items, when possible
- `position` : as above
- `size` : as above

#### Windows (`eu3dialogtype` and `windowtype`)

- `orientation` : as above
- `background` : may be intended to designate one of the sub objects as the background, but is frequenly left blank.
- `dontrender`, `downsound`, `upsound` : we ignore these
- `fullscreen` : a boolean denoting whether the window was originally intended to be "modal" (i.e. to hide the map when it was active)
- `horizontalborder` and `verticalborder` : we ignore these
- `moveable` : a boolean denoting whether it was intended for you to be able to drag the window around
- `name` : as above
- `position` : as above
- `size` : as above

Additionally, a window may have any of the gui object types as members. Any such objects are intended to be the window's child controls.
