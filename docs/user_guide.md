# Download

To download a precompiled version of Project Alice, locate the "tags" button in the [Project Alice repository](https://github.com/schombert/Project-Alice).

![image](https://github.com/schombert/Project-Alice/assets/39974089/0d393492-0471-44b2-b222-4efe6ed57c92)

Choose the particular version you wish to download, we recommend obtaining the latest one (the topmost one) so you can enjoy a more pleseant experience:

![image](https://github.com/schombert/Project-Alice/assets/39974089/4f179bf2-773b-4b2c-84f1-142cde855918)

Upon clicking on the "Download" text, you'll observe there may be 3 files, two of which are source code ZIP files and TARs, for our purpouses, those are probably not useful to you - and if they're, you should see the building guide instead. So, dwnload the one which is NOT the source code.

![image](https://github.com/schombert/Project-Alice/assets/39974089/a32e10a5-8caf-4c0c-8aab-b141736a41e3)

# Installation

Once it finishes downloading, extract it, you'll get the following files (the README is sometimes not included).

![image](https://github.com/schombert/Project-Alice/assets/39974089/d669abdd-647a-4417-ac8a-2a5c9c6e6fec)

Drag and drop all of those files into your Victoria 2 directory, it should then look like this

![image](https://github.com/schombert/Project-Alice/assets/39974089/3dd79391-999b-410e-8bd1-020437d34b89)

Then simply click on the `launch_alice` executable.

- `Alice`: The main game executable, do not open it directly.
- `launch_alice`: The game launcher, you need to create/recreate scenarios for each change you do to the mods.
- `dbg_alice`: A small debug utility to help track down errors, optional and it's fine if you don't have it.
- `readme OR ELSE.txt`: Contains relevant information, again, optional and it's fine if you don't have it.
- `assets`: Folder NEEDED for Alice to work properly.

## Ubuntu/PopOS

To install the game on Ubuntu/PopOS, follow the same steps for installation and download as for Windows until the point of launching `launch_alice`. At that point, download [Lutris](https://lutris.net).

Start Lutris and click on the Wine icon in the sidebar. An icon with an arrow pointing downwards will show up, click that. You should have an installed version already but if you do not, pick one. ge versions are said to be a great base for many games. 

Now click on the + “Manually add a game” icon in the main menu's top left corner. Afterwards, select the last option to "Add a locally installed game". In the first window “Game Info” give the game a name (can be anything). Choose “Wine” as a runner from the pull down. In the second tab “Game options” locate your `launch_alice.exe` file and select it as your “Executable”. Ignore arguments and working directory. “Wine Prefix” I choose ~/Games/Project-Alice for example, and select 64bit for prefix architecture. Under “Runner options” you can choose different wine versions, if you have them. Ignore “System options” you can come in here later if you need to. “Save” and it will exit. if you don’t see you game in the list in Lutris, restart the app. There was a minor bug that won’t show launch banners until you have at least “one game” installed. It is unclear if this bug still exists.

Now, the game should be ready to play from the Lutris main menu.

## Installation troubleshooting guide

Please refer to this before opening a tech support ticket.

**Missing audio/music**: You may lack the proper MP3 decoders to playback the MP3 files, consider using [The Codec Guide](https://codecguide.com/download_kl.htm) or, if that fails, [LAV Filters](https://github.com/Nevcairiel/LAVFilters/releases/download/0.79.2/LAVFilters-0.79.2-Installer.exe)

## Controls

- Drag mouse: This will show a drag box with a white outline, selecting units, prioritizing armies over navies
- Hold Ctrl while dragging mouse: Prioritize navies over armies
- HOME: Go to your capital
- Ctrl-0, Ctrl-1, ..., Ctrl-9: Add units to the control group #1, #2, ..., #9. Will show in outliner between parenthesis for the unit, one unit may be on multiple groups
- 0, 1, ..., 9: Select the units in control group #1, #2, ..., #9
- Shift-0, Shift-1, ..., Shift-9: Append select the units in control group #1, #2, ..., #9
- Tilde (~): Open console
- Arrow keys: Move on the map
- WASD: Move the map (only if WASD movement is turned on in settings)
- Holding shift + Arrow keys: Move faster on the map
- Holding shift + WASD: Same thing but only if WASD is enabled
- PgDown/PgUp: Zoom in and zoom out
- Puase/Spacebar: Pause or unpause the game
- Escape: Open up the main menu
- F1: Open production tab
- F2: Open budget tab
- F3: Open technology tab
- F4: Open politics tab
- F5: Open population tab
- F6: Open trade tab
- F7: Open diplomacy tab
- F8: Open military tab
- F9: Open ledger
- F10: Open macro builder
- Numpad +: Increase speed
- Numpad -: Decrease speed
- s: Split unit
- r: Reorganize unit (create new unit from existing)

Other controls are described directly on the tooltip (for example, right clicking to add a technology to the queue).

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
