## Demographics Data

Each pop stores a number of values about its preferred issues and ideologies. These are often referred to as the demographic data for the pop. These values, along with information about the prevalence of various cultures, religions, pop types, and employment are then aggregated on the province, state, and national level for easy reference.

Each source of demographics data is stored in the `demographics` property of the corresponding object. The demographics property is itself a floating point array, with each index into it corresponding to a different piece of demographics information. Each value corresponds to the number of pops that fall into the given category. This means that if you want to calculate the fraction of pops that hold a given ideology in a state, for example, that you need to divide the demographics value for that ideology for the demographics value containing the total population of the state.

### Pop Demographics

Pops have a more limited set of demographics data than provinces, states, and nations do. This is in part because each pop has only a single culture, religion, and type, and so tracking that information would be redundant. There are only two kinds of demographic information attached to an individual pop: the number of people who support a particular ideology and the number of people who support a particular position on an issue. To retrieve these values you first need to convert the desired ideology or issue position into a pop demographics index. This is done through the `to_key(sys::state const& state, dcon::ideology_id v)` and `to_key(sys::state const& state, dcon::issue_option_id v)` functions found in the `pop_demographics` namespace (declared in `demographics.hpp`). Using the key returned, you can then get the information you are interested in with a line such as: `state.world.pop_get_demographics(pop_id, demographics_key)`.

### Province, State, and Nation Demographics

The other sources of demographic data are more diverse. First, there are six fixed demographic indexes, that are stored as `contexpr` constants in the `demographics` namespace:

- `total` - Contains the total number of pops (i.e. the sum of the sizes of the pops) in the given entity
- `employable` - Contains the total number of pops that could work in *some* job. Not all pop types are employable, so this will generally be less than `total`
- `employed` -  Contains the total number of employable pops that currently have a job. The ratio of this value to `employable` is used to calculate things such as unemployment
- `consciousness` - Contains the sum of consciousness x pop size for the pops in the given entity. You must divide by `total` to find the average consciousness.
- `militancy` - As above, but for pop militancy
- `literacy` - As above, but for pop literacy

Then, in addition to the fixed demographic indexes, there are indexes for the number of pops that support a particular ideology or position on an issue, as there are in the pop demographics, but in addition to that there are also demographics indexes for the number of pops of a particular pop type, culture, or religion. As with the pop demographics, the indexes for these values can be retrieved by using one of the following function from the `demographics` namespace:

- `to_key(sys::state const& state, dcon::ideology_id v)`
- `to_key(sys::state const& state, dcon::issue_option_id v)`
- `to_key(sys::state const& state, dcon::pop_type_id v)`
- `to_key(sys::state const& state, dcon::culture_id v)`
- `to_key(sys::state const& state, dcon::religion_id v)`
