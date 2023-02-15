## Scenario loading order

This document provides a rough order for loading the game files. This is based on the work that went into open v2 (i.e. figuring out which files refer to things in other files). There are going to be a few files at least that are missing from the first draft (probably) because there were a few minor features open v2 wasn't going to support (the little images for generals, for example). So expect one or two new things being added here at some later date.

### Phase 1: gui loading

- Load the game text from the `.csv` files (required to map text keys to text) *Done*
- Load .gfx files *Done*
- Load .gui files (refer to items in .gfx files by name) *Done*

### Phase 2: preparing names

In this phase we are focused mainly on figuring out the names of things that the contents of other files will refer to them by. In some cases we can just read in all the relevant data when we open the file, and there would be a performance cost to not just reading in the data as we see it, so we read it in. But in the case of a number of other files, there are parts of the file (descriptions of trigger conditions most commonly) that we cannot read until we have gathered all the names. These files are merely "pre parsed." In most of these cases, we parse the file only to the extent needed to gather the names that we need, and then we cache the parsing state "in progress" so that we can resume directly when we get to the next phase.

- Read national tags from `countries.txt` *Done*
- Read religions from `religion.txt` *Done*
- Read cultures from `cultures.txt` *Done*
- Read goods from `goods.txt` *Done*
- Read buildings from `buildings.txt` *Done*
- Pre-parse `ideologies.txt` *Done*
- Pre-parse `issues.txt` *Done*
- Read governments from `governments.txt` *Done*
- Pre-parse `cb_types.txt` *Done*
- Read leader traits from `traits.txt` *Done*
- Read `defines.lua` *Done*
- Pre parse `crime.txt` *Done*
- Pre parse `triggered_modifiers.txt` *Done*
- Read national values from `nationalvalues.txt` *Done*
- Read static modifiers from `static_modifiers.txt` *Done*
- Read event modifiers from `event_modifiers.txt` *Done*
- Pre parse pop types from the `poptypes` directory *Done*
- Pre parse rebel types from `rebel_types.txt` *Done*
- Read `default.map` *Done*
- Read terrain modifiers from `map\terrain.txt` *Done*
- Read states from `map\region.txt` *Done*
- Read continents from `map\continent.txt` *Done*
- Read climates from `map\climate.txt` *Done*
- Pre parse technologies from `technology.txt` *Done*
- Pre parse inventions from the `inventions` directory *Done*
- Read unit definitions from `units` directory *Done*

### Phase 3: main data loading

With all the names figured out, we can get back to the business of dumping all the data out

- Read country files from `common\countries` *Done*
- Read province history files *Done*
- Read flag graphics from the `gfx\flags` to determine which flags are available
- Populate the country names
- Read production types from `production_types.txt`
- Read prepared ideologies *Done*
- Read prepared issue options *Done*
- Read prepared CB types *Done*
- Read prepared crimes (from modifiers) *Done*
- Read prepared triggered modifiers *Done*
- Read `national_focus.txt` *Done*
- Read main poptypes file (`pop_types.txt`) *Done*
- Read files in directory `poptypes` *Done*
- Read prepared rebel types
- Read `on_actions.txt`
- Read event files from directory `events`
- Read decision files from directory `decisions`
- Read prepared technologies *Done*
- Read prepared inventions *Done*

### Phase 4: fixups

Now that all of the data has been read, we can do any final touchups or additions required to put the scenario in a savable state

- Setup the rules for each party
- Determine the farmer and laborer poptypes
- Commit pending triggered events
- Optional: store any fixed ui text handles that we want to cache for better performance
