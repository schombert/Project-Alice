# Progress as of August 2023

Welcome back to the August update. It is way too hot and I need a vacation.

## AI

Most of my time this month was spent getting a rough draft of the AI working. By "rough draft," I mean that the AI exists and sort of hobbles along. However, it probably wouldn't be a very challenging opponent at this point, and there are plenty of ways that you can lure it into making ill-advised attacks and strategically boneheaded army deployments. But it does work. The AI is able to run its economy, research technologies, and even win wars against other AI players, and this makes it possible to start really testing the game.

I don't expect to make fundamental revisions to the AI prior to the 1.0 release. (But if someone really enthusiastic about AI work wants to revise things, I won't stop them.) Instead, I plan to focus on supplementing the AI's decision making with routines that are designed to compensate for its worst blunders. I don't expect to release 1.0 with good AI opponents; I am aiming for merely not-embarrassingly-bad. Obviously good AI opponents are important, and make the game much more fun. But making a good AI opponent for V2 means tailoring the AI to the "quirks" of V2, in particular how the combat system works. Post 1.0, I would really like to make some changes (in my mind, improvements) to how warfare works on a fundamental level, and so any work designed to increase the AI's competence with the existing combat system is probably going to be wasted effort in the long run. And that is how we have ended up with what we have now, which is me trying to get the best results with the minimal amount of effort.

Another important consideration when it comes to the AI is how well it will handle mods. Research is probably the strong suit of the AI in this respect. The AI decides what to research by looking at the bonuses that the tech will give it (it does not take into account the unlocked inventions yet). This means that the AI won't be bothered by complete revisions of the tech tree. The weakest part of the AI vis-à-vis mods is how it handles army composition. The AI is built under the assumption that the right balance of infantry and artillery can get it through the game, as these unit types are kept competitive in the base game by the upgrades that technology unlocks. If a mod has changed unit balance enough that different types of units are required to stay competitive, the AI will not be able to understand this.

It is obviously hard to illustrate progress on the AI with static images. Still, here is a picture of the AI winning a war:

![Mexican victory](victory.png)

A recent video showing the AI shuffling its units around can be found [here](https://www.youtube.com/watch?v=ua1n0MNf9mo).

## Movement arrows

Erik's contribution this month was adding arrows showing the path your units will take.

![Arrows](arrow.png)

They are still a bit of a work in progress because they neither scale to the zoom level properly nor show how much progress a unit has made toward the next province. However, they are infinitely better than having to just remember where you clicked and hoping that your units will get there eventually, which is what we had to work with before.

## Minor UI improvements

Most of my time this month has gone into the AI, so there aren't many changes in the UI. But that isn't to say that nothing has been done. The blue hyperlinks in text work now. Clicking on one will move the map to the relevant province and open the province window for it (or, for nations, the diplomacy window).

![Event](event.png)

Leaf has also fixed the occupation icon for us so that you can finally see which rebels have taken control of a province.

![Rebel occupation](rebels.png)

And Masterchef has been continuing work on adding polish to the military window.

![Unit tooltip](tooltip.png)

## Updated demo

This month's update also comes with an [updated demo](https://github.com/schombert/Project-Alice/releases/download/v0.0.2-demo/2023-8-7-DEMO.zip) where you can see for yourself the changes described above. Once again, *please* read the included text file for instructions on how to "install" the demo. Like last month's demo, we aren't yet at the point where public bug reports are particularly useful, but we may be getting there soon.

## Next month

I wasn't entirely kidding about needing a vacation. As most of the devs have been busy over the summer, I have ended up doing the bulk of the programming work, and that has left me a bit burned out. On the other hand, it means that if I really did go on vacation this month, almost nothing would get done. So, as a compromise, I am going to focus this month on simpler and less time-consuming improvements to the UI. I may also start work on the launcher, which is the last missing piece before we can roll out a public beta. Leaf will probably be working on multiplayer this month. She has already made a proof-of-concept demo that allows two games to connect and see each other's moves. However, that is only the tip of the iceberg when it comes to multiplayer, and I don't think the work will be finished when next month rolls around.

## The End

See you again next month! (or, if you can't wait that long, join us on [discord](https://discord.gg/QUJExr4mRn))