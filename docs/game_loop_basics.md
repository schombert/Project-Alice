## Fundamentals of interacting with the game loop

The game loop is always running in the background, even when the game is nominally paused. However, when the game is paused, and the game loop has nothing to do, it yields its time back to the scheduler (via `sleep`). Practically, what this means is that you don't communicate with the main game loop by calling functions. Typically you send it messages by writing data somewhere and then relying on it to pick up that data on the next iteration. And similarly, when the game loop wants to notify the rest of the program that an event has occurred, it writes a message somewhere and relies on the render function, which is called on a regular basis, picking it up.

### Communicating game speed

As far as the update logic is concerned, there are 6 game speeds, each stored in the `actual_game_speed` member variable, with 0 being paused and 5 the uncapped speed. The logic from the ui side of things may be more complicated. You may want to keep track of a desired game speed that persists across pauses. If you do, it is up for the ui code to keep track of it. Treat `actual_game_speed` more like a signal to the game loop. You write your desired game speed into it with `actual_game_speed.store(speed_value, std::memory_order::release)` and the game loop will pick up on that the next time it checks the speed.

When displaying whether the game is paused, you should show the speed as paused if either actual_game_speed is currently equal to 0 or if the `internally_paused` member of `state` is true (this is set when the logic of the game prevents the date from advancing; it should never be altered by the ui). When deciding this, both values can be read without any atomic safeguards, since reading them incorrectly is essentially of no consequence in that situation.

### Responding to a change in game state

The game state may change as the result of a new day being processed, which is probably the most common case. However, it may also change while the game is paused as the result of player commands (for example, the player adjusting their budget or selecting an event choice). Fortunately, this difference is largely irrelevant to the ui. Before starting to render a new frame we do the following:
```
auto game_state_was_updated = game_state_updated.exchange(false, std::memory_order::acq_rel);
	if(game_state_was_updated) {
		ui_state.units_root->impl_on_update(*this);
		ui_state.rgos_root->impl_on_update(*this);
		ui_state.root->impl_on_update(*this);
		// TODO map needs to refresh itself with data
		// TODO also need to update any tooltips (which probably exist outside the root container)
	}
```
The part that is currently updated pushes an `update` message through the ui hierarchy, and any properly implemented control will respond to it by updating any data it is storing locally from the game state, recreating its text content if necessary, and so on. Also necessary, but currently unimplemented, the map will have to update the data it is basing the current map mode off of, and tooltips, which are likely stored in a different ui root container, will have to be updated as well.

### Sending commands

The ui may transmit the player's actions to the game state by sending commands through the functions provided in `commands.hpp`. Each distinct command has two functions associated with it: a function with a name describing what the command does (e.g. `set_national_focus`) and then another function named `can_...`. The first function sends the command to the game state. The second function returns a boolean indicating whether that command is currently valid. Although you can send invalid commands, they will be silently rejected by the game. This second function is useful for determining when you should disable a button, for example. However, since tooltips will often needs to explain *why* the button is disabled, the function can be equally useful as a reference to look at all of the conditions, in code, that may block a particular command from being executed.

Documentation for which commands are possible and what they do in plain English can be found at the [end of the rules document](rules.md#Commands).

### Sending and receiving more complicated messages to the ui from the game state

In addition to just informing the ui that things have changed and that it needs to refresh itself, there are three kind of more complicated notifications that the game state may want to send to the UI. These are: notifications that a new event has happened that the local player can interact with, new diplomatic requests (such as a request for an alliance or military access) that the local player can either accept or decline, and new notification messages that may either end up in a popup or in the message log.

Each of these are passed to the ui via a queue that is a member of `sys::state`. The ui should empty these queues as soon as possible (ideally, whenever a new frame is rendered) and place the information into its own internal storage.

The queues that hold new events are, for events:
```
rigtorp::SPSCQueue<event::pending_human_n_event> new_n_event;
rigtorp::SPSCQueue<event::pending_human_f_n_event> new_f_n_event;
rigtorp::SPSCQueue<event::pending_human_p_event> new_p_event;
rigtorp::SPSCQueue<event::pending_human_f_p_event> new_f_p_event;
```

For diplomatic requests:
```
rigtorp::SPSCQueue<diplomatic_message::message> new_requests;
```

For general notification messages:
```
rigtorp::SPSCQueue<notification::message> new_messages;
```

The event queues hold the complete description of events that have occurred for the nation that is controlled by the local player / provinces within that nation. That information as a whole is required for the player to eventually choose an event option, as it identifies which event, out of all the possible events for any human player, the choice is for.

The diplomatic request messages hold a very minimal amount of information. To respond to such a request the ui must call `command::respond_to_diplomatic_message(sys::state& state, dcon::nation_id source, dcon::nation_id from, diplomatic_message::type type, bool accept)` with the appropriate parameters. If a response is not made within `diplomatic_message::expiration_in_days` days, it will be automatically declined, and the ui should automatically discard any visible notifications or message boxes after that point, as any attempt to respond to expired requests will be ignored.

The notification messages hold 5 important pieces of information. They contain a `type`, a `primary` nation, which is nation that the message is primarily about, and an optional `secondary` nation, which is another nation that caused the thing to happen (for example, in a notification about a new war, the nation that declared the war would be the secondary nation, while the primary nation would be the target of the war). Those three items should be used, along with the player's saved notification settings, to determine what happens when the message is received (i.e. do we pause the game automatically? do we display a pop-up message? do we record it in the log?).

If the message is to be displayed in a pop-up notification or written to the log, the `title` and `body` members contain functions that, when called, will populate a layout with appropriate text. **NOTE:** when posting a message from the game state (using the `notification::post` function), you should ensure two things. First, that the lambdas passed to these members capture by value any information that they need. Secondly, you should ensure that any string manipulation and/or formatting is done within the body of the lambda, and not in the process of creating it. As a rule of thumb, you should ensure that it does not capture any `std::string` or `std::string_view` objects at all. Since the messages are created within the game loop, we want to make sure that sending the notifications has a minimal cost, and that the majority of the cost of displaying the message is paid when the `title` and `body` functions are executed, since they will be executed in the ui thread instead of the main update thread, and thus won't delay the game itself.
