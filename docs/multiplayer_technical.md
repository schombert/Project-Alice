## Multiplayer

The main idea is that everyone runs the simulation, the commands are rebroadcasted from the server to the clients and the clients are given the "go" to when to continue the ticks.

Clients are set unpaused, and to the max speed, however their progress is blocked by the server until they receive a command telling them to advance a tick.

The server will receive commands from the clients. The server will then send back the commands that were able to be received. It will then wait for all clients using a semaphore, so that no client should be left behind.

The commands should be sent in chronological order. Clients **will** advance one tick when told so, and then execute the commands afterwards as if the player was doing it. This means that all commands should be properly acknowledged by the client as they occurred chronologically.

Everytime a client connects, it is given the checksum of the server, this checksum is for failsafe purpouses, to not let the client enter into an invalid state. The "play" button on the nation picker will bre greyed out if the checksum of the client and the host mismatch. Everytime a savefile is selected on the host, it will tell all it's clients that the checksum has indeed, changed now. This prevents clients who are otherwise not given yet a savefile to connect a game, or clients with different mods to connect a server for example.

Fog of War is forcefully set ON, however we know people can evade this restriction pretty easily, so it's mainly an honor thing - be kind with others :D

### Deterministic reimplementations

The standard C++ and C library provide `sin`, `cos`, and `acos` functions for performing their respective mathematical functions. However the implementation of these vary per platform and library, and since we're trying to provide a cross platform experience we reimplemented the mathematical functions from scratch, into a house-built solution.

- `void internal_check(float v, float err, float lower, float upper)` : Some mathematical functions have precision errors on them, we will however, check for any unreasonable numbers well beyond the boundaries of the function, `err` is the maximum absolute error for the function, `lower` and `upper` represent the lower and upper boundaries of the output. `v` being the value. This function should equate to a no-op on release.

### Notification commands

`notify_player_joins` - Tells the clients that a player has joined, marks the `source` nation as player-controlled.
`notify_player_pick_nation` - Picks a nation, this is useful for example on the lobby where players are switching nations constantly, IF the `source` is invalid (i.e a `dcon::nation_id{}`) then it refers to the current local player nation of the client, this is useful to set the "temporal nation" on the lobby so that clients can be identified by their nation automatically assigned by the server. Otherwise the `source` is the client who requested to pick a nation `target` in `data.nation_pick.target`.
`notify_save_loaded` - Updates the session checksum, used to check discrepancies between clients and hosts that could hinder gameplay and throw it into an invalid state. Following it comes an `uint32_t` describing the size of the save stream, and the save stream itself!
`notify_player_kick` - When kicking a player, it is disconnected, but allowed to rejoin.
`notify_player_ban` - When banning a player, it is disconnected, and not allowed to rejoin.
`notify_start_game` - Host has started the game, all players connected will be sent into the game.
`notify_stop_game` - Host has stopped the game (not paused), all players connected will be sent into the lobby.
`notify_pause_game` - Host has paused the game, exists mainly to notify clients that the host has paused the game.

The server will send new clients a `notify_player_joins` for each connected player. It will send a `notify_player_pick_nation` to the client, with an invalid source, telling it what is their "assigned nation".

An assigned nation is a "random" nation that the server will hand out to the client so it can identifiably connect to the server as a nation, and perform commands as such nation.

### Save streams

We send a copy of the save to the client, ultra-compressed, to permit it to connect without having to use external toolage, this is done for example when the host is loading a savefile - the client is given the new data of the savefile to keep them in sync.

### Hot-join

A new functionality is hotjoining to running sessions - the client may connect to the host and the host will assign them a random nation, usually uncivilized ones, if they wish to change their nation then they'll have to ask the host to go back to the lobby. This is a small measure to prevent abuse or random people entering games to ruin them, given the assumption most people will be choosing great powers.

### Out-of-sync (OOS)

On debug builds, a checksum will be generated every tick to ensure synchronisation hasn't been broken. If a desync happens, it will be pointed out in the tick where it occurred and a corresponding OOS dump will be generated.

Otherwise, the goal is no more oos :D
