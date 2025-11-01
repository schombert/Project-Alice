## Multiplayer

The main idea is that everyone runs the simulation, the commands are rebroadcasted from the server to the clients and the clients are given the "go" to when to continue the ticks.

Clients are set unpaused, and to the max speed, however their progress is blocked by the server until they receive a command telling them to advance a tick.

The server will receive commands from the clients. The server will then send back the commands that were able to be received. It will then wait for all clients using a semaphore, so that no client should be left behind.

The commands should be sent in chronological order. Clients **will** advance one tick when told so, and then execute the commands afterwards as if the player was doing it. This means that all commands should be properly acknowledged by the client as they occurred chronologically.

Everytime a client connects, it is given the checksum of the server, this checksum is for failsafe purpouses, to not let the client enter into an invalid state. If the checksums do not match on-join, then the client will immediately OOS. Everytime a savefile is selected on the host, it will tell all it's clients that the checksum has indeed, changed now. This prevents clients who are otherwise not given yet a savefile to connect a game, or clients with different mods to connect a server for example.


### Deterministic reimplementations

The standard C++ and C library provide `sin`, `cos`, and `acos` functions for performing their respective mathematical functions. However the implementation of these vary per platform and library, and since we're trying to provide a cross platform experience we reimplemented the mathematical functions from scratch, into a house-built solution.

- `void internal_check(float v, float err, float lower, float upper)` : Some mathematical functions have precision errors on them, we will however, check for any unreasonable numbers well beyond the boundaries of the function, `err` is the maximum absolute error for the function, `lower` and `upper` represent the lower and upper boundaries of the output. `v` being the value. This function should equate to a no-op on release.




### Commands ###

A command consists of a `header`, which contains the command type, and the player ID it is sent from (or in the case of some notification commands, the player it concerns).

Then there is the `payload`. The payload is a variable sized vector. Data is pushed to the payload with the `<<` operator, and a reference to the payload can be retrieved with the `get_payload` function.
In order to ensure memory and network safety, all commands have a minumum, and maximum payload size. Any command not conforming to these restrictions will be discarded.
The minimum size is to guarantee that no unitialized memory is read from. The minimum size is typically the same size as its data struct which is pushed to the payload.

For fixed-size commands, the max and min sizes would be the same (the size of their data struct). In some cases a variable amount of data is needed in the payload, and this is when the max size is larger.

For variable sized payload, the `check_variable_size_payload` member function will confirm if a payload has a certain amount of variable data after the data struct.



### Notification commands

`notify_player_joins` - Tells the clients that a player has joined, marks the `source` nation as player-controlled. When a player joins, all other players in the lobby (including the host) will fully reset, then reload their gamestate. Currently this is done so that there is no "lingering" data left behind after reloading their own state. This is to futureproof it against even minor mistakes/changes in contributions which can easily break sync if state is not fully reset first.
`notify_player_pick_nation` - Picks a nation, this is useful for example on the lobby where players are switching nations constantly, IF the `source` is invalid (i.e a `dcon::nation_id{}`) then it refers to the current local player nation of the client, this is useful to set the "temporal nation" on the lobby so that clients can be identified by their nation automatically assigned by the server. Otherwise the `source` is the client who requested to pick a nation `target` in `data.nation_pick.target`.
`notify_save_loaded` - Updates the session checksum, used to check discrepancies between clients and hosts that could hinder gameplay and throw it into an invalid state. Following it comes an `uint32_t` describing the size of the save stream, and the save stream itself! Keep in mind that the client will automatically reload their state first before loading the save
`notify_player_kick` - When kicking a player, it is disconnected, but allowed to rejoin.
`notify_player_ban` - When banning a player, it is disconnected, and not allowed to rejoin.
`notify_start_game` - Host has started the game, all players connected will be sent into the game.
`notify_stop_game` - Host has stopped the game (not paused), all players connected will be sent into the lobby.
`notify_pause_game` - Host has paused the game, exists mainly to notify clients that the host has paused the game.
`notify_reload` - Perform a game state reload as if it was a savefile.
`notify_player_is_loading` - Sent by the host to (usually) all of the clients to notify that `source` is currently loading. The host will NOT process most commands while more than 0 clients are loading. This command may be sent to the loading client itself. The command is sent out after a reload or save stream is requested, or when a new player joins a lobby with a save.
`notify_notify_player_fully_loaded` - Sent by the client to notify the host, and then re-broadcast to all clients that `source` has finished loading.

The server will send new clients a `notify_player_joins` for each connected player. It will send a `notify_player_pick_nation` to the client, with an invalid source, telling it what is their "assigned nation".

An assigned nation is a "random" nation that the server will hand out to the client so it can identifiably connect to the server as a nation, and perform commands as such nation.

When a state-reset&reload is happening, it will block rendering updates in the meantime to prevent crashes or other oddities happening while the state is completly empty. This is also the case when the `notify_stop_game` or `notify_start_game` are executed, it will block rendering updates until the scene change has been executed.

### Save streams

We send a copy of the save to the client, ultra-compressed, to permit it to connect without having to use external toolage, this is done for example when the host is loading a savefile - the client is given the new data of the savefile to keep them in sync.

### Re-sync

The lobby can be resyncronized if one or more players are OOS, and must be done manually by the host in the "tab" lobby screen.
When a resync starts, it will send the notify_save_loaded command to the oos'd clients together with a save for them to load. The non-oos clients will receive a notify_reload command, and will reload their own save.


### Hot-join

A new functionality is hotjoining to running sessions - the client may connect to the host and the host will assign them a random nation, if they wish to change their nation then they'll have to ask the host to go back to the lobby.

When people rejoin the game, they are placed on the same country they had. Countries are not transferred to AI control till rehosting.

### Out-of-sync (OOS)

On debug builds, a checksum will be generated every tick to ensure synchronisation hasn't been broken. If a desync happens, it will be pointed out in the tick where it occurred and a corresponding OOS dump will be generated.

Otherwise, the goal is no more oos :D
