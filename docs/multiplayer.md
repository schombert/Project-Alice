## Multiplayer

The main idea is that everyone runs the simulation, the commands are rebroadcasted from the server to the clients and the clients are given the "go" to when to continue the ticks.

Clients are set unpaused, and to the max speed, however their progress is blocked by the server until they receive a command telling them to advance a tick.

The server will receive commands from the clients. The server will then send back the commands that were able to be received. It will then wait for all clients using a semaphore, so that no client should be left behind.

The commands should be sent in chronological order. Clients **will** advance one tick when told so, and then execute the commands afterwards as if the player was doing it. This means that all commands should be properly acknowledged by the client as they occurred chronologically.

### Out-of-sync (OOS)

No more oos :D
