## Dude, where's my data?

This document serves as a quick guide for getting information out of the data container (the `world` member of the `sys::state` object). This is not meant to be a complete documentation of the information that is stored in that object, but rather as more of a "quick" reference for common questions and tasks. In the long term it may be worth your while reading the [data container documentation](https://github.com/schombert/DataContainer), as so much of the game's state is stored within it.

### Letting intellisense (TM) (or other code completion tools) help you

`world`, the data container object, exposes a huge number of functions, and finding the one you want directly from it probably won't be easy unless you are familiar with how the various function names are generated from the data container. Thus, I recommend a shortcut. Any object or relationship type that is managed by the data container can be turned into a "fat" handle. A fat handle made from a particular type of identifier exposes only the functions that are relevant for that object or relationship, making it much easier to find the one that you need via code completion. As long as you aren't storing the fat handles you make (or passing them to other functions) there should be no performance penalty for this ease of use. Creating a fat handle looks like so: `auto fat_handle_result = fatten(state.world, raw_handle_id);` Once you are calling functions from a fat handle, most handle values that are returned by its member functions will themselves automatically be upgraded to fat handles, so generally you will only have to call `fatten` once or twice.

### Navigating provinces and their relationships

Let us assume that you have a `province_id` called `id`. To start with, you want to write `auto fat_id = fatten(state.world, id);` Then to get any of the properties that are defined for a province (as given in `dcon_generated.txt`) you can write a line such as: `fat_id.get_life_rating()`, and just like that you will have the property you are interested in. There is only one confusing property for provinces (... so far): `fort_naval_base_level`. This is a single `uint8_t` that stores both the level of the current fort and naval base there (because why let 4 good bits go to waste). The following 4 functions are provided (in `province.hpp`) for easy access to the packed values:

- `uint8_t set_fort_level(uint32_t new_level, uint8_t old_value)` : Will change just the stored fort level in the value to a new level
- `uint8_t set_naval_base_level(uint32_t new_level, uint8_t old_value)` : As above, for the naval base level
- `int32_t get_fort_level(uint8_t value)` : Yields the fort level stored in the value.
- `int32_t get_naval_base_level(uint8_t value)` : Yields the naval base level stored in the value.

In addition to its properties, a province can be related to other objects, and often this is where the more interesting data lives. At the moment, provinces can be related to nations by control and ownership (two different relationship), factories can be located in them, and a province can be related to a national identity (i.e. a tag) by the relationship of a core being there. You can get a handle to the relationship itself (each relationship can be worked with as a kind of pseudo-object) as if it were a property. For example, you could call `fat_id.get_province_ownership();`. This would give you a fat handle to the relationship itself, and from that you could call `ownership_handle.get_nation();` to retrieve the owning nation. However, for some of these common situations where you want to "reach across" a relationship to find the other things related to, data container provides a shortcut. In this case you can call `fat_id.get_nation_from_province_ownership();` to get a handle to the owning nation directly.

#### Is this province water?

A common question that curiously may not seem to have an obvious answer, because there is no `get_is_sea()` member. This is because the provinces are grouped by whether they are land or sea. Thus, to test whether a province is water, you simply test whether it is in the water group of provinces, which you do by comparing `province_id.index()` to the index of the `province_definitions.first_sea_province` member of the `sys::state` object. Provinces with an equal or greater index are part of the sea group of provinces.

#### Does this province have an owner?

As a general rule of thumb, you get invalid indices out of the data container when you ask for something that doesn't exist. Thus, if a province is unowned and you ask for the owner, you get an invalid `nation_id` handle back. And you can test for invalid handles by casting them to bool (invalid handles are false, valid handles are true). Thus the contents of an if such as `if(bool(fat_id.get_nation_from_province_ownership())) { ... }` will only be evaluatef for provinces that have an owner.

### National identity

The name, flags, political parties, and map color of a tag are considered to be part of a national identity object. These objects are related uniquely (as in every nation has a national identity, and every national identity belongs to at most one nation at a time) by the `identity_holder` relationship. If you have a fat handle to a nation, you can go directly to its national identity via `get_identity_from_identity_holder()`. And from a national identity you can get its color (packed into a unit32_t) via `get_color()`. And the components of this color can be extracted as `float`s or integers using the `int_red_from_int` and `red_from_int` family of functions defined in `container_types.hpp`. (**Important Note**: Currently you have to go from the province to the owning nation to its identity to get a color, which could be something like `prov.get_nation_from_province_ownership().get_identity_from_identity_holder().get_color()` -- in the future we may cache things like the current color and name of a nation in the nation itself, so be aware that you may have to go back to some places and remove the `get_identity_from_identity_holder` step later.)

Note that identities and cores exist in a many-to-many relationship: each national identity can have cores in many provinces, and each province can have many national identity cores. Thus, you can't get a singular national identity from the `core` relationship when you start from a province. Instead the function `fat_id.get_core()` returns a range of the `core` relationships involving that province, which you could iterate over with `for(auto relationship : fat_id.get_core())`, and then extract the national identities with `relationship.get_identity()`.

### Great power relationships

Although most diplomatic relationships can exists between any two countries, there are a few properties that only make sense in the context of a great power's relationships with other nations. These properties are stored in the `gp_relationship` relationship.

There are three ways of accessing these relationships.

First, you can use a loop such as `for(auto gp_rel : state.world.nation_get_gp_relationship_as_great_power(some_nation_id)) { .. }`. This will iterate over all the great power relationships where `some_nation_id` is the great power. If the nation is not a great power, then this loop won't run at all.

Secondly, you can use a loop such as `for(auto gp_rel : state.world.nation_get_gp_relationship_as_influence_target(some_nation_id)) { .. }`. This works essentially in the same way as before, except that this loop will iterate over all of the relationships in which a great power is influencing this nation. Such a loop will not iterate over more than 8 relationships, as there at most 8 great powers. However, it may find fewer than 8 relationships, as if a great power is not interacting with a particular country no relationship will be generated.

Finally, you can use `state.world.get_gp_relationship_by_gp_influence_pair(target_id, great_power_id)` to get a handle to the `gp_relationship` between any nation and any great power. Note that this function will return the invalid handle value if no relationship exists between the two nations in question. (Which you can test for by casting it to bool).

The `influence` property of the relationship stores the current influence value as a `float`, which should be pretty self-explanatory. The `status` property, however, is more complex, as it contains a number of values packed together. You can get the individual properties out of the `uint8_t` by masking it with one of the constants found in the `nations::influence` namespace. With `(status & nations::influence::level_mask)` (don't forget the parentheses or `&` will be evaluated after `==`) you can extract the level of the influence relationship, which ranges from `nations::influence::level_hostile` to `nations::influence::level_in_sphere`. With `(status & nations::influence::level_mask)` you can extract the priority that the nation has been given with respect to influence point distribution. Priority levels range from `nations::influence::priority_zero` to `nations::influence::priority_three`. Finally there are three bits you can test with `nations::influence::is_expelled`, `nations::influence::is_discredited`, and `nations::influence::is_banned` to see if any of those status effects are active.

Because finding out which sphere a nation is in by iterating over the great power relationships would be quite inefficient, each nation also stores a handle to the nation that it is in the sphere of in its `in_sphere_of` property. If the nation is not in any sphere, this property will contain the invalid handle value.

### Population

Most provinces have more than one pop (although some may have none at all). To iterate over the pops in a province you can write a loop such as the following:
```
for(auto pops_by_location : state.world.province_get_pop_location(province_id)) {
	auto pop_id = pops_by_location.get_pop();
	...
}
```
This loop iterates over all of the pop-to-province location relationships involving the specified province. In the body of the loop, `pops_by_location` will fat handle to one of the instances of that relationship. And then you can pull the pop out of the relationship with `get_pop()`, which will also produce a fat handle to the pop.

#### Storing pop ids

**Never** store a `pop_id` without taking some precautions. Unlike most objects, pops can come and go, and to improve performance the data container will move pops around as they are deleted to keep them in a compact group. This means that, if you hold a handle to a pop over a tick of the game day, it may end up pointing to a different pop entirely. And yet, for the sake of making the ui responsive, you may very well want to store the id of the pop that a window or element is displaying. To solve this problem, when you store a pop_id you should also store the province_id of the location of the pop, the culture_id of the pop, the religion_id of the pop, and the pop_type_id of the pop. This combination of properties is guaranteed to be unique. And so when you store a pop_id, not only should you store these other properties, you should also write an `update` function for the ui element (`update` is sent to visible ui elements after a game tick has happened), in which you search the province you have stored for a pop with the same matching culture, religion, and pop type. Then take the id you find this way and update the stored pop id with it. (If you do not find a matching pop in this way, it means that the pop has disappeared, and you should no longer attempt to display information about it.)

### Final reminder

Keep in mind that most of the ways in which you extract a handle may sometimes yield an "invalid" handle, indicating that there is no such object. For example, you would get such a handle if you try to get the owner of an unowned province. Casting a handle (fat or otherwise) to `bool` returns `true` for valid handles and `false` for invalid handles. While there are some situations in which it is safe to read data from an invalid handle (see the data container documentation), it may be safer to get into the habit of checking the validity of handles.
