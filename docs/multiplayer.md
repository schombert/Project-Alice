## Multiplayer Instructions

### Joining a game

After you start the launcher, you will see something like the following:

As with a single player game, you will first need to create a scenario for the combination of mods you want to play with (the Start game, Host, and Join buttons will be disabled if you have not). This combination of mods *must* match the game that you are trying to join (which includes matching the version of those mods; remember that changing the mod files is *not* enough -- you also need to rebuild the scenario).

Then, change your user name (currently, the text boxes are a bit wonky: you need to hover your mouse over them and then type -- there is an invisible cursor that acts as if it is always at the end of the text), and enter the ip address of the host you want to connect to (if you copy that ip address from elsewhere, ctrl+v will paste it into the address box).

Joining the game will put you into the nation-picking screen, except you'll see other players and the host. You can select any nation you want to play as long as it is not controlled by another player, since two players cannot control the same nation (yet).

As a client you will not be able to control the speed of the game, or to load savefiles on your own -- the host has absolute power over the game session. Hosts can kick and ban people at any time, and they may also return to lobby momentarily to allow players to select other nations/switch.

Fog of war is always on in multiplayer.

NOTE: If you type an IPv6, your host will need to accept IPv6 connections too, or else it will fail. Same with IPv4, usually IPv4 is recommended for user-experience reasons.
NOTE: Domains are not supported, please resolve them manually and type their corresponding IPv4 (This is a feature for the future).

#### Hosting a game

If you use IPv4, type a dot "." in the IP field, if you are going to use IPv6, type a semicolon ":" in the IP field. Clients that use IPv4 cannot connect to a IPv6 host.

To accept connections from clients (without using a tunneling service like Hamachi), you will need to be able to accept connections over port 1984. Usually this requires port forwarding it in your router to your local machine. You can use an [open port checker tool](https://www.yougetsignal.com/tools/open-ports/) to check you opened the ports correctly, be aware this kind of tools may not be 100% reliable, as such, another person helping to test is very much preferred.

When giving players your IP address so that they can connect to you, you need to provide them with the address that your machine appears to be at from the perspective of the internet (i.e. not your local subnet address).

As a host, you'll be able to kick and ban people in-game and in the lobby. Ensure you've prepared everything beforehand.

Take note, loading savefiles will send the savefile to every client you have connected and that will connect, so for example, if you have 8 players, you will send 8 times the savefile, which is usually about 3 MB, so be wary of the fact you may end up sending 3*8=24 MB of information EACH time you load a savefile. It is already compressed using the maximum compression level available, so there is no point in compressing it further (by the mathematical laws of compression, has it's own Wikipedia article which I will not dwell on).

For dealing with troublemakers:
- Kick: Disconnects the player from the session, they may join back if they wish.
- Ban: Disconnects and blacklists the player from the session, they will not join back unless a new session is done. Based on IP (or MAC, if using IPv6) - again, anyone dedicated enough is able to circumvent this, but it works for most practical cases.

#### Troubleshooting

For clients:
- Check the IP you typed was correct
- Check your player name is not a) empty, or b) improper
- Check that you haven't been banned from the server
- Ensure you're connecting using IPv4 or IPv6 - accordingly, hosts will not accept a client with a different IPv
- If using IPv6, check that your router even supports it
- If using IPv4, do not input semicolons (i.e, `127.0.0.1:1283`), as this will be interpreted as a IPv6 address
- Check that your firewall is not blocking Alice
- Check your internet connection (maybe even ping the host)
- If using a VPN tunneling app, check that you're connected to the peer
- Ensure you all have the same game version
- Check that someone else is able to join (i.e that the host has port forwarded properly, the port `1984`)
- Ensure you have the same mods as the host
- You can try using the same scenario file if you think that you are having issues caused by slightly different version of mods

For hosts:
- Check you're using the correct IPv you want (clients also have to be told which IPv they need to use)
- Check your player name is not a) empty, or b) improper
- If using IPv6, check that your router even supports it
- If using IPv4, do not input semicolons (i.e, `:1283`), as this will be interpreted as a IPv6 address
- Check that your firewall is not blocking Alice
- Check your internet connection
- Check that the port `1984`, is properly forwarded on your machine (you may use an open port checker to analyze this)
