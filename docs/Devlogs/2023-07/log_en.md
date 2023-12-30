# Progress as of July 2023

Welcome to the slightly delayed update. I have been focused on getting the [public demo](https://github.com/schombert/Project-Alice/releases/download/v0.0.1-demo/2023-7-8-DEMO.zip) ready, to the exclusion of all else. But, better late than never.

## The Big News

The big news this month is that the project is finally ready to be released on the public, albeit in a very rough state. The demo includes no AI whatsoever, so it is the perfect time to play as Texas or some other hard start. Or to go for a world conquest, for that matter. Currently the "AI" will agree to anything you ask of it, regardless of whether it makes sense or not.

## UI progress

Since we don't have 3D models, we have had to introduce some new 2D icons to display military activity on the map. First, we have a compact view for when you are relatively zoomed out.

![compact](compact.png)

For ports, our solution is the following:

![port](port.png)

When you zoom in, you can see more information about individual units:

![unit](unit.png)

We also have some basic ui to display an ongoing siege or battle.

![siege](siege.png)
![battle](battle.png)

The UI is not finished by any means. However, most of its essential functionality is in, and for now my focus will be moving away from the UI and back towards the internals of the game. You shouldn't expect to see major changes in the UI between now and next month. Mostly I will be making small touch-ups as I go.

Work on the globe also continues. This month Erik added the rivers (as well as the internal logic to detect river crossings). This completes the basic functionality for the map itself, in the sense that it displays all the information that you need to play the game. There is certainly room for aesthetic improvement, but since we don't have an art team, I wouldn't hold your breath.

![rivers](rivers.png)

## AI

Now that the player side of the game is in a minimally functional state, the time has come to work on the AI. My current plan for the AI is to tackle it in basically two passes. First I will attempt to get the AI in a roughly working state, meaning that it will have the ability to perform all of the actions that a player can, without worrying too much about how well it plays the game. Then, I will do a second pass to try to get it up to some minimally acceptable level. I do *not* intend to try to make the AI "role play." The AI will mostly pursue what I view as a generally applicable strategy of trying to jump start industrialization while focusing on maximizing research points. The AI will generally play very selfishly. It will probably only join your offensive wars if it thinks that it can get something out of them; it will generally not do favors for you just because you have an alliance or high relations.

Ideally, I would like to have the AI play as competently as possible within the rules of the game. In particular, if there is some tedious micro that would give the AI an advantage, such as cycling units, I believe that the AI should exploit it if it is possible for the player to do so as well. This isn't just because I want the AI to be a challenge. I also want to motivate long-term improvements to the rules themselves by making players subject to the same abuses that they can pull on the AI.

## The End

See you again next month! (or, if you can't wait that long, join us on [discord](https://discord.gg/QUJExr4mRn))