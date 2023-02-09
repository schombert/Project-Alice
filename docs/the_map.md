## The map

The `map::display_data` is largely self contained. It only handles the map movement and map rendering. The map is using [glm](https://github.com/g-truc/glm) for camera movement and it will probably be the only place in the project where glm is used.

The map uses two meshes, one for the water and one for the land. The water uses the `assets/shader/map_water_f.glsl` shader
The map uses two fragment shaders, `map_water_f.glsl` for the water and `map_f.glsl` for the terrain. They are located in `assets/shader`. Each shader is rendered on their own mesh, `water_vbo` and `land_vbo` respectively. There will later be separate shaders and meshes for the borders and shorelines.

### The map interface

The following functions are used for interfacing with the map:

- `map_display::load_map(sys::state&)` -- loads the map from the disk. It's only suppose to be called once at start up. It loads all the texture and shaders. It also creates the meshes.
- `map_display::render(uint32_t screen_x, uint32_t screen_y)` -- both renders the map and updates the map movement.
- `map_display::set_province_color(std::vector<uint32_t> const& prov_color)` -- changes the province colors. The `prov_color` vector will contain each new provinces color and needs to be the same size as the provinces.
- `map_display::set_pos(glm::vec2 pos)` -- sets the position of the camera to `pos`. The vector coordinates will be relative and go from 0 to 1.

The following are input event functions:

- `map_display::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod)` -- is called when a key is pressed and uses the arrow keys to move the camera.
- `map_display::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod)` -- is called when a key is released. The `map_display` stores the key presses and wont stop moving the camera until the key is released.
- `map_display::on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount)` -- is called when the mouse wheel scrolls to zoom the map.
- `map_display::on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod)` -- is called when mouse moves. It is used to drag the map when the middle mouse button is pressed.
- `map_display::on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod)` -- is called when the middle mouse button is pressed. It initiates the dragging of the map.
- `map_display::on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod)` -- is called when the middle mouse is released and stops the dragging of the map.

### Further goals

For the future goal most work will be recreating the graphics of Victoria 2. The only new thing we will add is the globe and some extra optimizations that Victoria 2 didn't include. These are the goals:

- Map modes : This is mostly done. We just need to send in the colors of the map mode in set_province_colors(). More map modes will be available as we parse more data from Victoria 2. One extra thing we will need to add is the diagonal borders, that will probably be done by having separate vertices for each province.
- Borders : This is another thing that will make a big impact on the visuals. The plan is to read the province texture and create new polygon borders from at the borders. We will see if we use the border textures or not.
- Shores : Will work very similar to borders, but will use the shore textures
- Rivers : Also very similar to borders and shore, it will just be a bit harder to parse since we will need to take the direction of the water into account.
- Globe : The globe is new thing that Victoria 2 didn't have. OpenV2 had it though so it has to be here too, it's also very cool! We will probably use the implementation from SoE when creating the globe.
- Smoother map movement : This makes a big impact when zooming and moving around, but it has to be done right otherwise it will feel even worse than no smooth movement.
- Save the loaded data to the scenario blob : Once the map is parsed it should be able to be serialized and saved as a binary blob. The next time the user loads up the game with the same mods it should be able to deserialize the blob instead for faster loading times.
- Optimize map parsing with multithreading : This isn't that important currently, but is going to speed up the parsing a lot.
- Add error handling for missing or broken files : When there is a problem loading the files it should be able to report the error and exit the application. This can be done by parsing an error object to the `load_map` function.

### Province adjacency in the data container (state.world)

The data container stores a relationship that you can use as a map of unordered pairs of province ids to data about the border and distance between them. The map creation process should be responsible for populating this information. I suggest, upon encountering two adjacent map tiles with different province ids, calling `state.world.try_create_province_adjacency(first_province_id, second_province_id);`. This will add a new adjacency relationship if one does not already exist. Then you will need to read the `map/adjacencies.csv` file. Only the first 3 columns matter to us. The first two columns are the province ids to be connected, and the third is the type of connection. For `sea` type connections, calling try create will return a valid id only if the connection did not already exist. If it doesn't exist, you can know that the provinces were not already adjacent and set the `non_adjacent_bit` (see below) for the connection. For those connections marked `impassable`, I suggest using `force_create_province_adjacency`, which always will return a handle to the adjacency relationship and setting the `impassible_bit`. Finally, `canal` can be handled much like `sea` type connections, except that you should also set the `impassible_bit` to indicate that the canal is not active at the start of the game.

For each adjacency relationship, a `uint8_t` stores the type of that relationship as a sequence of independent bits. The constants for these bits can be found in `constants.hpp` and they are
```
province::border::state_bit
province::border::national_bit
province::border::coastal_bit
province::border::impassible_bit
province::border::non_adjacent_bit
```
In addition to the bits that have been described above, the `state_bit` should be set when the adjacent provinces belong to different states, the `national_bit` should be set when they belong to different nations, and the `coastal_bit` should be set when one of them is a sea province and the other is land. Set the bits other than `state_bit` and `national_bit` when loading the map; those two bits depend on data that is being loaded in parallel and will be set in a later phase. These bytes are stored contiguously in the same order as the `province_adjacency_id` indices. To get a pointer to the array that stores them, you can do something like `uint8_t* buffer = &(state.world.province_adjacency_get_type(dcon::province_adjacency_id(0)));`. (Yes, I know it is ugly, but you aren't intended to need direct access in this way in the typical case.)

If you have a `province_adjacency_id` and you want to get the provinces it connects, you must call `id.get_connected_provinces(0)` and `id.get_connected_provinces(1)` (and you shouldn't rely on the order you might get the province ids out of the relationship by doing this.

If you want to go the other way, and get the adjacency relationship between two province ids, you can call `state.world.get_province_adjacency_by_province_pair(p1, p2)`, which will return an invalid handle if the provinces are not adjacent.

Finally, distances between provinces are also stored here, but **do not** attempt to set them at this point (although eventually that will probably be part of map loading). Making the correct distance calculation requires a call to `acos` and we will need to implement a custom version of that function that we can ensure will give the same results everywhere for everyone.
