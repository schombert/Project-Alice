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
		ui_state.root->impl_on_update(*this);
		// TODO map needs to refresh itself with data
		// TODO also need to update any tooltips (which probably exist outside the root container)
	}
```
The part that is currently updated pushes an `update` message through the ui hierarchy, and any properly implemented control will respond to it by updating any data it is storing locally from the game state, recreating its text content if necessary, and so on. Also necessary, but currently unimplemented, the map will have to update the data it is basing the current map mode off of, and tooltips, which are likely stored in a different ui root container, will have to be updated as well.

