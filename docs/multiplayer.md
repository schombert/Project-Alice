## Multiplayer

The main idea is that everyone runs the simulation, the commands are rebroadcasted from the server to the clients and the clients are given the "go" to when to continue the ticks.

Clients are set unpaused, and to the max speed, however their progress is blocked by the server until they receive a command telling them to advance a tick.

The server will receive commands from the clients. The server will then send back the commands that were able to be received. It will then wait for all clients using a semaphore, so that no client should be left behind.

The commands should be sent in chronological order. Clients **will** advance one tick when told so, and then execute the commands afterwards as if the player was doing it. This means that all commands should be properly acknowledged by the client as they occurred chronologically.

Everytime a client connects, it is given the checksum of the server, this checksum is for failsafe purpouses, to not let the client enter into an invalid state. The "play" button on the nation picker will bre greyed out if the checksum of the client and the host mismatch. Everytime a savefile is selected on the host, it will tell all it's clients that the checksum has indeed, changed now. This prevents clients who are otherwise not given yet a savefile to connect a game, or clients with different mods to connect a server for example.

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

The server will send new clients a `notify_player_joins` for each connected player. It will send a `notify_player_pick_nation` to the client, with an invalid source, telling it what is their "assigned nation".

An assigned nation is a "random" nation that the server will hand out to the client so it can identifiably connect to the server as a nation, and perform commands as such nation.

### Save streams

We send a copy of the save to the client, ultra-compressed, to permit it to connect without having to use external toolage, this is done for example when the host is loading a savefile - the client is given the new data of the savefile to keep them in sync.

### Hot-join

A new functionality is hotjoining to running sessions - the client may connect to the host and the host will assign them a random nation, usually uncivilized ones, if they wish to change their nation then they'll have to ask the host to go back to the lobby. This is a small measure to prevent abuse or random people entering games to ruin them, given the assumption most people will be choosing great powers.

### Out-of-sync (OOS)

On debug builds, a checksum will be generated every tick to ensure synchronisation hasn't been broken. If a desync happens, it will be pointed out in the tick where it occurred and a corresponding OOS dump will be generated.

Otherwise, the goal is no more oos :D

### User guide

In the launcher you may be presented with the following:

If you're someone trying to host:
* The IP address field doesn't need to be filled. (Exception below).
* Port forward the port `1984`, port forwarding varies per router, some ISPs do not allow port forwarding, so be aware.
* Use an [open port checker tool](https://www.yougetsignal.com/tools/open-ports/) to check you opened the ports correctly, be aware this kind of tools may not be 100% reliable, as such, another person helping to test is very much preferred.
* Ensure your firewall isn't blocking connections.
* In case you can't port forward, an alternative is using a VPN tunneling program, which carries it's own myriad of security risks, so only use it with trusted people.
* If you want IPv6 networking, simply type a colon ":" in the IP field.

Or if you're trying to join a game:
* Fill out the IP address field (IPv4 clients can't join IPv6 servers, and viceversa).
* Ensure you have the same game version, to avoid issues.
* And have the same mods, again, to avoid issues, you'll be told when your scenario (i.e, mods) do not match the host's.

As a client you'll not be able to control the speed of the game, or to load savefiles on your own - the host has absolute power over the game session. This can be both a good or a bad thing, depending on which server you joined. Hosts can kick and ban people at any time, they may also return to lobby momentarily to allow players to select other nations/switch.

* Kick: Disconnects the player from the session, they may join back if they wish.
* Ban: Disconnects and blacklists the player from the session, they will not join back unless a new session is done. Based on IP (or MAC, if using IPv6) - again, anyone dedicated enough is able to circumvent this, but it works for most practical cases.

Fog of war is always enabled on multiplayer, it's more of an honour system than anything - as any devious client may just unforce-it back.

### User guide (Client)

Booting up the game, a launcher screen showing your nickname and the IP you want to connect to, ensure the IP is typed correctly, and your desired nickname too, once you're done press the "Join" button.

NOTE: If you type an IPv6, your host will need to accept IPv6 connections too, or else it will fail. Same with IPv4, usually IPv4 is recommended for user-experience reasons.
NOTE: Domains are not supported, please resolve them manually and type their corresponding IPv4 (This is a feature for the future).

Joining the game will yield you to an usual singleplayer screen, except you'll see other players and the host too, select any nation you want to play as, two players cannot select the same nation (yet). So co-op is not possible at the moment.

### User guide (Host)

If you use IPv4, type a dot "." in the IP field, if you are going to use IPv6, type a semicolon ":" in the IP field. Clients that use IPv4 cannot connect to a IPv6 host.

As a host, you'll be able to kick and ban people in-game and in the lobby. Ensure you've prepared everything beforehand.

Take note, loading savefiles will send the savefile to every client you have connected and that will connect, so for example, if you have 8 players, you will send 8 times the savefile, which is usually about 3 MB, so be wary of the fact you may end up sending 3*8=24 MB of information EACH time you load a savefile. It is already compressed using the maximum compression level available, so there is no point in compressing it further (by the mathematical laws of compression, has it's own Wikipedia article which I will not dwell on).
