## Graphics and gui definitions

The information required to render an approximation of the Victoria 2 UI is contained in the `.gfx` and `.gui` files contained in the `interface` directory. The `.gfx` files describe, roughly, the assets that the UI rendering can call upon (i.e. the graphics), while the `.gui` files contain descriptions of the placement of those assets and the general type of behavior to associate with them (i.e. the gui). Since the `.gui` files may refer to graphics assets, it is natural to parse the `.gfx` files first.

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