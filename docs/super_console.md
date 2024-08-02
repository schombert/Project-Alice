## Super Console

### Player's Guide

In versions 1.1.3 the in game console (the window that pops up when you press `~`) has been completely reworked. Instead of supporting a fixed list of commands, the console is now running a complete programming language that can let you inspect and modify nearly any part of the game. This is probably a bit much for the average cheater, so this part of the guide will walk you through making the new console do the same things as the old one. Note that the new console is sensitive to capitalization, spaces, and the exact format of value (10.0 gets you a floating point value while 10 gets you an integer value, and commands that expect one won't accept the other). If your console input worked you will see a green `ok.` appear. Otherwise, you will get an error. If you encounter an error, carefully recheck your spelling and spacing and try again.

#### Changing infamy, prestige, and other values

Here is how you can use the console to change some common values:

- `10.0 TAG plurality !` : set the plurality of the three letter tag to 10.0 (note, the decimal place is required)
- `10.0 TAG prestige !` : set the prestige of the three letter tag to 10.0 (note that this may not be reflected in the displayed prestige exactly because of permanent prestige modifiers)
- `10.0 TAG infamy !` : set the infamy of the three letter tag to 10.0
- `10.0 TAG 0 >commodity_id stockpiles !` : set the treasury of the three letter tag to 10.0
- `10.0 TAG research_points !` : set the research points of the three letter tag to 10.0
- `10.0 TAG diplomatic_points !` : set the diplo points (what you pay for diplomatic actions) of the three letter tag to 10.0
- `10.0 TAG leadership_points !` : set the leadership points (what you pay for generals and admirals) of the three letter tag to 10.0

#### Special console functions

And here is a list of some more specialized console functions that you can use

- `clear` : removes old text form the console window
- `spectate` : switches the game to spectator mode (use `change-tag` to resume playing)
- `true fps` : turns the visible FPS counter on. A value of `false` will instead turn it off
- `false set-auto-choice` : turns off all existing auto event choices
- `TAG change-tag` : changes who you are playing as to TAG
- `TAG true set-westernized` : changes the civilized/uncivilized status of TAG
- `TAG 2030 fire-event` : Fires event 2030 for the TAG. (Note that this number must correspond to a national event that can occur randomly.)
- `make-crisis` : forces some crisis to start if one currently isn't going on
- `end-game` : switches to the end-game screen
- `TAG 10.0 set-mil` : sets the militancy of all pops in TAG to 10.0
- `TAG 10.0 set-con` : sets the consciousness of all pops in TAG to 10.0
- `TAG TAG make-allied` : creates an alliance between the two tags
- `true cheat-wargoals` : allows you to always add wargoals while it is enabled
- `true cheat-reforms` : allows you to change to any political reform while it is enabled
- `true cheat-diplomacy` : while it is enabled the AI will say yes to anything that you propose
- `true cheat-decisions` : you can ignore the `allow` requirements of decisions while it is enabled
- `true cheat-decision-potential` : you can ignore the `potential` requirements of decisions while it is enabled
- `true cheat-army` : your land units build instantly while it is enabled
- `true cheat-navy` : your naval units build instantly while it is enabled
- `true cheat-factories` : your factories build instantly while it is enabled
- `TAG true instant-research` : research for the TAG will start completing instantly
- `TAG complete-construction` : instantly completes the construction of all province buildings in TAG
- `TAG TAG conquer` : the first TAG immediately conquers all provinces of the second TAG
- `PROV123 TAG make-core` : adds a core of TAG in the province
- `PROV123 TAG remove-core` : removes any cores of TAG in the province
- `PROV123 TAG set-owner` : makes TAG the owner of the province
- `PROV123 TAG set-controller` : makes TAG the controller of the province
- `365 add-days` : adds 365 days to the current date
- `dump-oos` : makes an oos dump
- `true daily-oos-check` : makes the OOS check daily instead of monthly
- `dump-econ` : puts some economic data in the console and starts econ dumping
- `vanilla save-map` : makes an image of the map. `vanilla` can also be replaced by one of the following to alter its appearance: `no-sea-line`, `no-blend`, `no-sea-line-2`,  and `blend-no-sea`
	
#### `player @` and province names

Wherever you see a `TAG` in the above commands you can also substitute `player @` (note the space) which will act as the tag of whatever nation you are currently playing. Also, a quick note on province names. Being able to refer to `PROV123` relies on that being the province's name at the time the console was first opened. Initially, all provinces are named following that pattern (PROV plus their id from the game files). However, this may change because of renaming effects. If you load a game where a province has been given a new name, you will no longer be able to refer to it by its original `PROV123` name.
 

### Super User's and Developer's Guide
