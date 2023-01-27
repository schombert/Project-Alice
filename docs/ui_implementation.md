## User interface details

This document details the internal working of the user interface code. While it does explain briefly what is going on behind the scenes, the primary purpose of this document is to explain how to write custom behavior for controls.

### `element_base`, the fundamental ui type

Every element, and node in the ui tree for that matter, is derived from the `element_base` class. This class manages passing messages and information both up and down the ui hierarchy. It may also represent a specific image or type of control and may implement the required behavior. Because of its flexibility, there is a significant amount of complexity within the class. Typically, when implementing behavior for a control, most of this won't matter. As described below, most type of controls provide a (very) small number of customization points for you to override, meaning that all of this behind-the-scenes complexity can be ignored.

#### Three types of functions

So let's jump right into the complexity that surrounds this class by talking about the three kinds of functions it contains. The first kind, and fewest in number, can be used anywhere (as long as it is in the ui and rendering thread). These include the following functions:

- `set_visible(sys::state& state, bool vis)` : calling this function can be used to show or hide the element (when you might normally be inclined to destroy an element in other frameworks, here we either hide it or move it out of its container to some other location). This function will also trigger the `on_visible` or `on_hide` messages (see below) as necessary, and will also update the element (and any of its children) when it becomes visible (hidden elements are not updated in the normal course of things).
- `bool is_visible() const` : tells you whether the element is currently visible. Note that elements are visible by default when they are created.

And while they aren't functions, most of the member variables are similarly simple to use.

- `element_base* parent` : is a pointer to the containing element in the ui hierarchy. This should never be `nullptr` unless the element is currently detached, is the root container, or is a tooltip.
- `element_data base_data` : this begins its life as a *copy* of the definition that the element was created from. However, it can be freely updated, and the current position and size of the element are typically found here (not the size and position given by the definition).

#### Functions you implement

The second group of functions are those that are designed as customization points that most subclasses will use to extend the behavior of the ui element. Generally, speaking you **should not** directly call these functions unless you know what you are doing: they are designed to be called by the inner workings of the system. Most of these functions return the `message_result` type, which has three values. Returning `message_result::unseen` will result in the system operating as if your element didn't exist with respect to a particular event or message. Returning `message_result::seen` tells the system that the event happened over or to some ui element (and so that it probably shouldn't pass it on to the map), but it will continue looking for an element to handle it. Finally, returning `message_result::consumed` tells the system that the message or event has been taken care of, and nothing else needs to see it. Note that this is not a guarantee that no other elements will see it; it depends on the details of the event or message, but that is generally the result.

- `virtual message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept` : This is used to see whether a mouse event that trigged at this location would find a ui element to respond to it. This is most commonly used as part of the routines that find out which interactable element is under the mouse cursor.
- `virtual message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : This is the event for a left mouse click
- `virtual message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : this is the event for a right mouse click
- `virtual void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept` : This message is unusual in that it is not massed down through the ui hierarchy. Instead it is sent directly to the element pointed to by the `element_base* drag_target` member of `ui_state` (a member of `sys::state`). Elements typically ask to receive drag events by setting this pointer as part of their response to a left mouse button event. The old x and y coordinates sent as parameters are the previous position of the mouse. Comparing these to the current values will tell you how much the mouse has been dragged.
- `virtual message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept` : this is the event for a keypress
- `virtual void on_text(sys::state& state, char ch) noexcept` : This message works like `on_drag` in that it is sent only to the `edit_target` if any (and when the `edit_target` is set, `on_key_down` messages will not be sent).
- `virtual message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept` : this is the event triggered by the mouse scroll wheel.
- `virtual void on_update(sys::state& state) noexcept` : This event is sent to every visible ui element in the ui hierarchy to allow them to update their state when the game state is updated. It is also triggered when the ui element becomes visible after being hidden and on its creation to populate its contents in those situations.
- `virtual message_result get(sys::state& state, Cyto::Any& payload) noexcept` : See below
- `virtual message_result set(sys::state& state, Cyto::Any& payload) noexcept` : `get` and `set` actually work in the same way, and while their names represent the typical use case, they can also be misleading. When you overload these functions you are expected to check the `Any` payload for a type that you are able to respond to. If you find such a type you may read out data from it / write date to it / react in other ways as necessary, and the should return `message_result::consumed`. If it is not a type that you want to do something with, return `message_result::unseen`. `get` sends requests for information / commands up the hierarchy: if an element doesn't respond to it, then that element's parent will get a shot at it, and so on. `set` sends messages down the hierarchy, but unlike get, all the children at the lower levels will see the message, regardless of how any of them handle it. The return value from set only tells you that at least one child somewhere in the hierarchy responded to it. The typical usage pattern, and hence the names, is as follows: a child element in a window showing data for a particular nation wants to update the value it displays. So it sets an empty nation id into an `Any` and calls `impl_get` on its parent (see below). This will work its way up the hierarchy until it find an element that is willing to fill that nation id. Ideally this would be the containing window, and it would fill it out with the nation that all the controls in the window are expected to draw data from.
- `virtual void render(sys::state& state, int32_t x, int32_t y) noexcept` : this is used to draw the ui elements
- `virtual focus_result on_get_focus(sys::state& state) noexcept` : currently a design placeholder -- don't worry about it
- `virtual void on_lose_focus(sys::state& state) noexcept` : as above
- `virtual void on_drag_finish(sys::state& state) noexcept` : Sent to the `drag_target` when the overall drag event has ended. This may turn out to be unnecessary and thus may be removed in the future.
- `virtual void on_visible(sys::state& state) noexcept` : called when the element is switched to being visible from being invisible. *Note:* only that particular element receives the message, and not its children which were implicitly invisible.
- `virtual void on_hide(sys::state& state) noexcept` : as above, but when the element becomes hidden.
- `virtual tooltip_behavior has_tooltip(sys::state& state, int32_t x, int32_t y) noexcept` : will eventually become part of the tooltip system, but is currently not implemented
- `virtual void create_tooltip(sys::state& state, int32_t x, int32_t y, element_base& /*tooltip_window*/) noexcept` : as above
- `virtual void on_create(sys::state& state) noexcept` : this is called on the element after it is first created. The difference between this and the constructor is that when this function runs `base_data` will have been populated, while the constructor runs before that has a chance to happen.

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
