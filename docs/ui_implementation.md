## User interface details

This document details the internal working of the user interface code. While it does explain briefly what is going on behind the scenes, the primary purpose of this document is to explain how to write custom behavior for controls.

### `element_base`: the fundamental ui type

Every element, and node in the ui tree for that matter, is derived from the `element_base` class (which is declared in the `gui_element_base.hpp` file). This class manages passing messages and information both up and down the ui hierarchy. It may also represent a specific image or type of control and may implement the required behavior. Because of its flexibility, there is a significant amount of complexity within the class. Typically, when implementing behavior for a control, most of this won't matter. As described below, most type of controls provide a (very) small number of customization points for you to override, meaning that all of this behind-the-scenes complexity can be ignored.

#### Three types of functions

So let's jump right into the complexity that surrounds this class by talking about the three kinds of functions it contains. The first kind, and fewest in number, can be used anywhere (as long as it is in the ui and rendering thread). These include the following functions:

- `set_visible(sys::state& state, bool vis)` : calling this function can be used to show or hide the element (when you might normally be inclined to destroy an element in other frameworks, here we either hide it or move it out of its container to some other location). This function will also trigger the `on_visible` or `on_hide` messages (see below) as necessary, and will also update the element (and any of its children) when it becomes visible (hidden elements are not updated in the normal course of things).
- `bool is_visible() const` : tells you whether the element is currently visible. Note that elements are visible by default when they are created.

And while they aren't functions, most of the member variables are similarly simple to use.

- `element_base* parent` : is a pointer to the containing element in the ui hierarchy. This should never be `nullptr` unless the element is currently detached, is the root container, or is a tooltip.
- `element_data base_data` : this begins its life as a *copy* of the definition that the element was created from. However, it can be freely updated, and the current position and size of the element are typically found here (not the size and position given by the definition).

#### Functions you implement

The second group of functions are those that are designed as customization points that most subclasses will use to extend the behavior of the ui element. Generally, speaking you **should not** directly call these functions unless you know what you are doing: they are designed to be called by the inner workings of the system (hopefully, the fact that they are marked as `protected` will prevent most misuses of this sort). Most of these functions return the `message_result` type, which has three values. Returning `message_result::unseen` will result in the system operating as if your element didn't exist with respect to a particular event or message. Returning `message_result::seen` tells the system that the event happened over or to some ui element (and so that it probably shouldn't pass it on to the map), but it will continue looking for an element to handle it. Finally, returning `message_result::consumed` tells the system that the message or event has been taken care of, and nothing else needs to see it. Note that this is not a guarantee that no other elements will see it; it depends on the details of the event or message, but that is generally the result.

- `virtual message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept` : This is used to see whether a mouse event that trigged at this location would find a ui element to respond to it. This is most commonly used as part of the routines that find out which interactable element is under the mouse cursor.
- `virtual message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : This is the event for a left mouse click
- `virtual message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : this is the event for a right mouse click
- `virtual void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : This message is unusual in that it is not massed down through the ui hierarchy. Instead it is sent directly to the element pointed to by the `element_base* drag_target` member of `ui_state` (a member of `sys::state`). Elements typically ask to receive drag events by setting this pointer as part of their response to a left mouse button event. The old x and y coordinates sent as parameters are the previous position of the mouse. Comparing these to the current values will tell you how much the mouse has been dragged.
- `virtual message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept` : this is the event for a keypress
- `virtual void on_text(sys::state& state, char ch) noexcept` : This message works like `on_drag` in that it is sent only to the `edit_target` if any (and when the `edit_target` is set, `on_key_down` messages will not be sent).
- `virtual message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept` : this is the event triggered by the mouse scroll wheel.
- `virtual void on_update(sys::state& state) noexcept` : This event is sent to every visible ui element in the ui hierarchy to allow them to update their state when the game state is updated. It is also triggered when the ui element becomes visible after being hidden to populate its contents in those situations.
- `virtual message_result get(sys::state& state, Cyto::Any& payload) noexcept` : See below
- `virtual message_result set(sys::state& state, Cyto::Any& payload) noexcept` : `get` and `set` actually work in the same way, and while their names represent the typical use case, they can also be misleading. When you overload these functions you are expected to check the `Any` payload for a type that you are able to respond to. If you find such a type you may read out data from it / write date to it / react in other ways as necessary, and the should return `message_result::consumed`. If it is not a type that you want to do something with, return `message_result::unseen`. `get` sends requests for information / commands up the hierarchy: if an element doesn't respond to it, then that element's parent will get a shot at it, and so on. `set` sends messages down the hierarchy, but unlike get, all the children at the lower levels will see the message, regardless of how any of them handle it. The return value from set only tells you that at least one child somewhere in the hierarchy responded to it. The typical usage pattern, and hence the names, is as follows: a child element in a window showing data for a particular nation wants to update the value it displays. So it sets an empty nation id into an `Any` and calls `impl_get` on its parent (see below). This will work its way up the hierarchy until it find an element that is willing to fill that nation id. Ideally this would be the containing window, and it would fill it out with the nation that all the controls in the window are expected to draw data from.
- `virtual void render(sys::state& state, int32_t x, int32_t y) noexcept` : this is used to draw the ui elements
- `virtual focus_result on_get_focus(sys::state& state) noexcept` : currently a design placeholder -- don't worry about it
- `virtual void on_lose_focus(sys::state& state) noexcept` : as above
- `virtual void on_drag_finish(sys::state& state) noexcept` : Sent to the `drag_target` when the overall drag event has ended. This may turn out to be unnecessary and thus may be removed in the future.
- `virtual void on_visible(sys::state& state) noexcept` : called when the element is switched to being visible from being invisible. *Note:* only that particular element receives the message, and not its children which were implicitly invisible.
- `virtual void on_hide(sys::state& state) noexcept` : as above, but when the element becomes hidden.
- `virtual void on_create(sys::state& state) noexcept` : this is called on the element after it is first created. The difference between this and the constructor is that when this function runs `base_data` will have been populated, while the constructor runs before that has a chance to happen.
- `virtual tooltip_behavior has_tooltip(sys::state& state) noexcept` : this function is called to determine what kind of tool tip, if any, the element wants to display. By default, this function returns `tooltip_behavior::no_tooltip`, which means that no tool tip will be displayed when the mouse is above the element. Returning `tooltip_behavior::tooltip` means that this element has a single, fixed, tool tip associated with it. In other words, returning this means that the content of the tool tip will never change. This is not a very common situation, and should *not* be your default. Returning `tooltip_behavior::variable_tooltip` indicates that the displayed tooltip may need to change when the state of the game updates. This is probably a good default choice, as buttons that may become disabled, for example, may need to be able to change their tool tip to explain why they are disabled as the game state advances. Finally, you can return `tooltip_behavior::position_sensitive_tooltip`. This is useful if the content of the tool tip may change depending on the exact location of the mouse in the element (for example, pie charts need this). The downside to this is that the tool tip will be recreated every frame, so ensure that you are minimizing as much as possible the work required to generate such a tool tip.
- `virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept` : If the element has reported that it wishes to display a tool tip, this function will be invoked as necessary when the contents of that tool tip must be populated. You must use the `columnar_layout` parameter, as described below in [Text rendering and layout](#text-rendering-and-layout), to generate the contents of the tool tip. If you do not add any text to this layout, the tooltip will not be displayed (this may be the desired behavior if the tool tip should not be displayed in all conditions). `x` and `y` are the current position of the mouse in the element's local coordinate space.

#### Functions you call

This brings us to the last group of functions. These are all functions that you **should not** generally override unless you know what you are doing. They are, however, the functions you should *call* when you want to send messages to a ui element (as you should not generally be calling any of the above functions yourself). These functions mostly start with `impl_` because I am bad at naming things. They are also fairly self explanatory if you have read the previous section (so do that first).

- `virtual element_base* impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept` : you can call this function to find the (topmost) element under the provided coordinates.
- `virtual message_result impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : dispatches the event down the hierarchy.
- `virtual message_result impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : as above
- `virtual message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept` : as above
- `virtual message_result impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept` : as above
- `virtual void impl_on_update(sys::state& state) noexcept` : as above
- `message_result impl_get(sys::state& state, Cyto::Any& payload) noexcept` : as above, except this is more likely to be actually usful to you
- `virtual message_result impl_set(sys::state& state, Cyto::Any& payload) noexcept` : as above
- `virtual void impl_render(sys::state& state, int32_t x, int32_t y) noexcept` : as above

Finally, we get to the functions that are used to manipulate the ui hierarchy. You should not implement these, but you can call them. Also keep in mind that the goal is not to delete existing ui elements, only move them around. This means that you have to keep two things in mind: First, when you remove a child element, you will get it as a return value, which you then must store somewhere. Secondly, if you try to add a child to something that does not accept children, that is an error and you will get a crash.

- `virtual std::unique_ptr<element_base> remove_child(element_base* child) noexcept`
- `virtual void move_child_to_front(element_base* child) noexcept` : changes the relative "z-order" positioning of the children by moving that child to the top.
- `virtual void move_child_to_back(element_base* child) noexcept` : as above, but moves a child to the back
- `virtual void add_child_to_front(std::unique_ptr<element_base> child) noexcept` : adds a new child to the front of the collection
- `virtual void add_child_to_back(std::unique_ptr<element_base> child) noexcept` : adds a new child to the back of the collection (this is slightly more efficient).
- `virtual element_base* get_child_by_name(sys::state const& state, std::string_view name) noexcept` : finds a child with the given name, or returns `nullptr` if there is no such child.
- `virtual element_base* get_child_by_index(sys::state const& state, int32_t index) noexcept` : retrieves the nth child, or `nullptr` if the index is out of range.

### Helper functions

#### The global element map

I have implemented support for something that I call the global element map, but which we may not ultimately use the full features of. There are two aspects of this map. First, there is the `defs_by_name` hash map in the `ui_state` object. This maps the name of elements to a struct containing both the id for its definition and, possibly, a function pointer to a special creation function (described below). The idea behind this map is to provide the possibility of having a single place to connect ui elements by name to custom behavior. However, as we will see below, this may not be really necessary. But in either case:

- `void populate_definitions_map(sys::state& state)` : this function populates the map of element names to definition ids. It needs to be called at once after loading or creating the scenario data, as it is not saved between runs. If you were going to add custom mappings for various element names, you would add those to the body of this function.
- `template<typename T> constexpr ui_hook_fn hook()` : you can use this function, along with a class that describes the behavior of the element, to add the necessary generation function to the map, with a call such as `ui_hook_fn<my_element_type_name>()`.

#### Creating ui elements

In general, you should *never* use something like `make_unique` to create ui element. Instead use one of the functions described in this section.

- `std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name)` : This is the most generic function for creating elements. It finds the definition using the name of the element and either creates it using the generation function that you provided to the map, or by relying on `make_element_immediate` (see below).
- `std::unique_ptr<element_base> make_element_immediate(sys::state& state, dcon::gui_def_id id);` : This function uses the ui definition to attach the default behavior to the generated element. Generally, this means that the element won't do anything, so it is more of a placeholder than anything else. However, for some things, such as static icons, this may be all that you need.
- `template<typename T> std::unique_ptr<T> make_element_by_type(sys::state& state, dcon::gui_def_id id)` : This creates a new ui element with the behavior defined by the class `T`. Note that this bypasses the global element completely.
- `template<typename T> std::unique_ptr<element_base> make_element_by_type(sys::state& state, std::string_view name)` : This function works as the one above, except that it find the id of the definition by looking up the name in the global element map. Note that it still ignores any generation function that you may have stored in the map.

#### General properties access

- `xy_pair get_absolute_location(element_base const& node)` : This function returns the position of the element in terms of the global ui coordinate space (which may not be the same as screen space coordinates if the ui is also being scaled).
- `xy_pair child_relative_location(element_base const& parent, element_base const& child)` : This function returns the location of the child element in its parent's coordinate space. Note that this is not the same as the child element's unmodified position, as its position may be stored relative to a point within the parents coordinate space other than (0,0).
- `int32_t ui_width(sys::state const& state)` : returns the horizontal dimension of the global ui coordinate space
- `int32_t ui_height(sys::state const& state)` : returns the vertical dimension of the global ui coordinate space

### Defining custom control behavior

You could implement the behavior for a control by deriving directly from `element_base`. However, as various types of controls typically share behavior amongst themselves, there are a number of simpler classes that you can derive from in order to minimize the work required.

#### `image_element_base`

This serves as the base class for other controls that display graphics of some sort, but you can derive from it directly if you wish. `image_element_base` contains boolean members `disabled`, which will cause it to render in B&W when set, and `interactable` which will enable it to highlight when the mouse is over it, assuming that some derived class makes it opaque to the mouse. By itself, this element is invisible to the mouse.

#### `opaque_element_base`

This class derives from `image_element_base` and functions as it does, except that it is opaque to the mouse (meaning that, for example, if you click on it, the click event will be consumed by the element and will not be seen by another element).

#### `button_element_base`

This class derives from `opaque_element_base` and implements the functionality required to support reacting to shortcut keys, making a click sound when activated, and not reacting to input when disabled. To make your own buttons, derive from this class and override its `virtual void button_action(sys::state& state) noexcept` member with what you want the button to do when it is activated.
This class also provides the following member function:
- `void set_button_text(sys::state& state, std::string const& new_text)` : Using this function changes the text that will be displayed on the face of the button.

#### `multiline_text_element_base`

Use this in place of `simple_text_element_base` when you require "complex" text composition (see in the next section). This element has an `internal_layout` that can be manipulated and used for creating an endless/columnar layout (usually the preffered type is an endless layout). Unlike it's simpler variant, it can be coloured and can host substitution maps because it is able to support layouts.

#### `multiline_button_element_base`

Variant of `multiline_text_element_base` tailored for buttons.

#### `scrollable_text`

This implements text that can be scrolled through, it uses a delegate `multiline_text_element_base` to accomplish this, it is important to remember that the `delegate` is a separate element (and the scrollbar too). In order to populate the layout inside said `delegate` element, add an extra indirection of `this->delegate` for each what-would-be calls performed on a normal `multiline_text_element_base`.

#### `draggable_target`

This is a bit of an odd one because of the way windows are defined in the `.gui` files. Windows do not intrinsically have a background, instead they have some image element member that fills their visible space. Thus, to redirect mouse commands from that background element to the window itself, we have `draggable_target`. If an element is a `draggable_target` any drag mouse actions will be prorogated up from it to the window at the lowest level that is defined to be movable.

#### `generic_close_button`

A pre-made button that closes (i.e. makes invisible) the parent element that it is attached to.

#### `window_element_base`

This should be the base class for most window elements you wish to create. This class automatically handles making the window movable if its definition defines it to be movable. When defining a window you need to override the `virtual std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept;` member. This function is called during window creation once for each child element defined as part of the window. If you want to attach specific behaviors to the child elements (and you probably should) then you will match against the `name` parameter and then invoke and return the results of an element creation function of your choice using `state` and `id`. This is also the place to set any initial properties of the child element. If you return `nullptr`, then `make_element_by_type` will be used with `id` to determine the behavior of the child. If you wish to add *additional* child elements, you will need to override the `on_create` function. If you do so, make sure that you call `window_element_base::on_create` at some point during it to add the ordinary children of the window.

#### `scrollbar`

This is the base class for any scrollbar you may need. There are a couple of things to keep in mind here. First is that internally scrollbar store their value as an integer. If the scrollbar definition specifies a step amount of less than 1, then this integer will be scaled up internally, so that each step is 1. To react to changes in the value stored by the scroll bar (i.e. the user moving the scroll position) you must override
`virtual void on_value_change(sys::state& state, int32_t v) noexcept`, which is called with the internal stored value as a parameter. If you need that value in its unscaled form, you may call the `float scaled_value()` function. (I know, perhaps not the best name). If you want to change the value stored in the scrollbar (for example, in response to an update). You may call either `update_raw_value(sys::state& state, int32_t v)` to update the stored integer directly, or you may call `void update_scaled_value(sys::state& state, float v)` to update the unscaled value. Note that calling these functions *will not* result in your `on_value_change` function being called -- only updates made by the user do that. Finally, if you wish to change the underlying definition of the scroll bar (i.e. to set its range) you can use. `void change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s)`

`mutable_scrollbar_settings` is defined as follows:
```
struct mutable_scrollbar_settings {
	int32_t lower_value = 0;
	int32_t upper_value = 100;
	int32_t lower_limit = 0;
	int32_t upper_limit = 0;
	bool using_limits = false;
};
```
`lower_value` and `upper_value` determine the limits of the scrollbar's range. `upper_limit` and `lower_limit` can be used to set visual stops that restrict the movement of the slider to a smaller portion of that range (not currently implemented), and `using_limits` controls whether these additional stops should be used and displayed. Note that changing the scrollbar settings in this may may cause the position of the slider to be changed, and if it is, you will receive an `on_value_change` message.

#### `progress_bar`

This element inherits from `opaque_element_base`. Represent a progress bar, it will render only a portion of the associated texture image for the progress bar. Use the property `progress` to control the progress displayed (bound between `0.0` and `1.0`, values higher may cause unpredictable behaviour).

#### `vertical_progress_bar`

The vanilla UI system supports rotation of elements, however for performance reasons this isn't supported, so this element is used to display vertical progress bars. Works in the same fashion as the progress bar, except it's rotated 90 degrees counterclockwise.

#### `line_graph`

Linegraphs are implemented as the rendering of consecutive lines using te same texture throughout. The amount of nodes within the linegraph are around 32. It supports fudging (as seen in the original implementation) where fake values would be displayed alongside real ones. This behaviour can be toggled on/off by the user on the main menu.

#### `edit_box_element_base`

Represents an element that can be typed into, by default it handles all of the usual controls for an edit box, `<TAB>`, `<Backspace>`, `<Delete>`, and other commands. It inherits from `simple_text_element_base` so the text is saved on the same place as the aforementioned element. And it can be retrieved by overriding the `edit_box_update` method and reading the associated `std::string_view` containing the whole contents of the edit box.

- `virtual void edit_box_tab(sys::state &state, std::string_view s) noexcept` : Called when a `<TAB>` is pressed (used for autocompletion mainly).
- `virtual void edit_box_enter(sys::state &state, std::string_view s) noexcept` : Called when `<Enter>` is pressed (used for flushing and parsing the contents).
- `virtual void edit_box_update(sys::state &state, std::string_view s) noexcept` : Called whenever a new character is typed, a character is deleted, a command is issued or any combination of such.
- `virtual void edit_box_up(sys::state &state) noexcept` : Called when the `<Up Arrow>` key is pressed.
- `virtual void edit_box_down(sys::state &state) noexcept` : Called when the `<Down Arrow>` key is pressed.
- `virtual void edit_box_esc(sys::state &state) noexcept` : Called when the `<Escape>` key is pressed.
- `virtual void edit_box_backtick(sys::state &state) noexcept` : Called when the `<Backtick>` key is pressed.
- `virtual void edit_index_position(sys::state &state, int32_t index) noexcept` : Called when the index position is modified; for example, `<Left Arrow>` or `<Right Arrow>` were pressed, and the handler made them go -1 or +1 in the current index, so it will call this function.

### Text rendering and layout

#### Adding Tooltips
For adding tooltips you need to define two functions:
```cpp
tooltip_behavior has_tooltip(sys::state& state) noexcept override {
    return tooltip_behavior::variable_tooltip;
}

void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
    // Insert your tooltip code in here
}
```
NOTE: as a beginner it may seem tempting to add tooltips to elements that already exist that dont have them, this is a pitfall you MUST avoid as it creates issues,
instead when you find a element in the codebase that requires a tooltip, inherit from the element class and add the tooltip code to the inherited version of the class,
this keeps the UI Code clean, consistent, and avoids tooltip cross contamination, which is when two elements get the same tooltip, when either they should have different ones or one/both of them
shouldnt have tooltips to begin with.
For a example of how to add tooltips properly, look at gui/gui_topbar.hpp for a example.

#### Simple text

You can render simple text with the following function:
`ogl::render_text(sys::state const& state, char const* codepoints, uint32_t count, color_modification enabled, float x, float y, float size, const color3f& c, text::font& f)`
The `count` parameter contains the number of characters to render from `codepoints` (the string *does not* have to be null terminated). The `color_modification` enum parameter must be one of `none`, `disabled`, `interactable`, or `interactable_disabled` (`interactable` and `interactable_disabled` are for when a clickable element is under the mouse) and should be chosen to match the way the rest of the element is currently being displayed. `x` and `y` position the text on the screen (you should strive to pick integral values for `y` whenever possible).

#### Complex text

The functions and classes described in this section are declared in `text.hpp` and are part of the `text` namespace. For anything beyond the trivial, we have to be able to arrange text so that we can later draw it with multiple calls to `render_text` in order to display text that spans more than a single line and/or is in more than a single color. To do this, we create a `text::layout` object. This object stores text along with positioning and color information. Layout objects should not be populated manually. Instead you should use either an endless or columnar layout.

The contents of the layout can be imagined as boxes, with each box containing a single item (a single logical unit of text, even if it takes more than one line to display it all). These boxes are then positioned from top to bottom within the boundaries of the imaginary page described by the `layout_parameters` (a struct, described below). In an endless layout, these boxes are allowed to spill off the bottom of the available page, with the assumption that it will be possible for the user to scroll the hidden contents into view by some means. In a columnar layout, reaching the end of the imaginary page instead starts a new column to the right of the column just completed, moving rightwards by `column_width` pixels (see below). The inter-column space is thus the difference between `column_width` and the right margin of the imaginary page.

To create an endless layout you must call the function `text::create_endless_layout(layout& dest, layout_parameters const& params)`, and to create a columnar layout you must call `columnar_layout text::create_columnar_layout(layout& dest, layout_parameters const& params, int32_t column_width)` (note the additional `column_width` parameter that controls spacing between columns). Note that on creation, both of these functions will clear the contents of the `layout`. The expected usage is to create the type of layout you wish to fill with text, populate it with the appropriate layout box function (see below), and then discard it. Only the base `layout` object should be stored persistently. Note that, once an endless layout has been created, the `number_of_lines` member of the layout will tell you how many lines it took to fit all the text, which you may find useful for deciding how to scroll its contents. You can also find out how much space was used by a columnar layout by consulting the `used_height` and `used_width` members of the `columnar_layout` object. Note that these measurements will take into account any left or top marginal space, but not any right or bottom marginal space.

##### `layout_parameters`

 The `layout_parameters` structure has the following members:
- `int16_t left` : the left margin in the layout's internal coordinate space
- `int16_t top` : the top margin in the layout's internal coordinate space
- `int16_t right` : the right margin in the layout's internal coordinate space
- `int16_t bottom` : this is unused in an endless layout
- `uint16_t font_id` : the font that will be used to determine how much space text takes up
- `int16_t leading` : the amount of additional space, in "pixels," to put below each line (may be negative)
- `alignment align` : the alignment of text within the margins of the imaginary page. May be `left`, `right`, or `centered`
- `text_color color` : the text color to start the layout with

##### Layout box functions

As described above, text is added to a layout by populating a series of layout boxes. Each box must be created by a call to `layout_box text::open_layout_box(𝘭𝘢𝘺𝘰𝘶𝘵_𝘵𝘺𝘱𝘦& dest, int32_t indent = 0)`. `indent` is a measurement of how far the box should be positioned to the right of the left margin. When you aren't adding the contents of a list, an `indent` of 0 is probably what you want. When you have finished adding text to an individual layout box, you must close it with `text::close_layout_box(𝘭𝘢𝘺𝘰𝘶𝘵_𝘵𝘺𝘱𝘦& dest, layout_box& box)` before starting the next box or finishing the layout. Forgetting to close a box will result in a layout containing improperly positioned text.

While the layout box is open, you can add content to it with any combination of the following functions (calling each as many times as you wish):

- `add_to_layout_box(sys::state const& state, layout_base& dest layout_box& box, std::string_view, text_color color, substitution source = std::monostate{})` : this function adds plain text to the box. The `substitution` parameter is only there for making text behave *as if* it came from substituting a variable, which is probably something that you do not have a need for.
- `add_to_layout_box(sys::state const& state, layout_base& dest layout_box& box, dcon::text_sequence_id source_text, substitution_map const& mp)` : adds complicated text to the layout box. This text will change color according to any color change commands embedded in the source text. Additionally, any variables in the source can be replaced at this point by populating the substitution map parameter. The easiest way to populate a substitution map is with `text::add_to_substitution_map(substitution_map& mp, variable_type key, substitution value)`. `substituion` is a `std::variant<std::string_view, dcon::text_key, dcon::province_id, dcon::state_instance_id, dcon::nation_id, int64_t, float, sys::date, std::monostate>;`. Values of nations, provinces, etc will be converted to their current name upon substitution, dates will turn into a textual representation, etc. (See hit testing, below, for how to get the source of a substitution back out.)
- `add_line_break_to_layout_box(sys::state& state, layout_base& dest, layout_box& box)` : this function adds a line break within a layout box (useful in columnar layouts if you want to create two or more lines that must end up in the same column).
- `localised_format_box(sys::state& state, layout_type& dest, layout_box& box, std::string_view key, text::substitution_map sub)` : this function is equivalent to doing the following code snippet:
```
if(auto k = state.key_to_text_sequence.find(std::string_view("some value present in CSV files...")); k != state.key_to_text_sequence.end()) {
    text::add_to_layout_box(state, contents, box, k->second, sub);
}
```
- `add_divider_to_layout_box(sys::state& state, layout_base& dest, layout_box& box)` : this function is the same as doing the following code snippet:
```
text::add_line_break_to_layout_box(state, contents box);
text::add_to_layout_box(state, contents, box, std::string_view("--------------"));
text::add_line_break_to_layout_box(state, contents box);
```

##### Rendering a text layout

Rendering the contents of a layout is as simple as iterating over it with a loop such as the following:
```
for(auto& txt : internal_layout.contents) {
	ogl::render_text(state,
		txt.win1250chars.c_str(), uint32_t(t.win1250chars.length()),
		ogl::color_modification::none,
		float(x) + txt.x, float(y + txt.y),
		float(font_size),
		get_text_color(txt.color),
		state.font_collection.fonts[font_id - 1]
	);
}
```
where `x` and `y` are amounts that you want to adjust the position of the layout as a whole relative to its internal coordinate space.

##### Hit testing a text layout

For implementing things such as hyperlinks, it may be necessary to determine what chunk of text, if any, a particular coordinate position is inside. To do this, use the `text_chunk const* get_chunk_from_position(int32_t x, int32_t y)` member of the `layout` object, keeping in mind that `x` and `y` are in terms of the layout's internal coordinate space. This function will return `nullptr` if there is no text being rendered at the given position. In terms of making hyperlinks work, the most important member of the returned object is `source`, which holds the `substitution` variant that created the text, if any. Inspecting the contents of this variant will allow you to find the id of the province, nation, etc that was put into the original substitution map.
