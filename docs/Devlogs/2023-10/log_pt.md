# Progress as of October 2023

Welcome back to the October update. Only a few more months until I can be done writing these. That will probably make my translator happy too.

## Public alpha

The [0.8.1 alpha](https://github.com/schombert/Project-Alice/releases/download/v0.8.1%CE%B1/0.8.1-ALPHA.zip) version of Project Alice is now available. It is already a playable game as things stand, if you are willing to overlook some bugs here and there. I have already played a complete campaign as Austria (in HPM) without any major issues.

Bug reports from the community are still very much appreciated and will help us reach 1.0 faster. Even reporting little things like missing tooltips and other glitches with the ui is useful.

## Mod compatibility status

Here is the list of what works and to what extent. If you want us to investigate / work on mod compatibility for any mod not on this list, please let us know. See the next section for the details.

- HPM: requires a compatibility patch (but there are a lot of versions of HPM out there; if you are using a different version of HPM, you may need us to make a patch for your version).
- HFM: requires a compatibility patch and will eventually need a custom shader
- MHM (Modern History Mod): Working with the creator to find and fix bugs (basically done at this point). Will need a compatibility patch for a custom shader and may need some patches to access the various start dates for the time being
- ToL (Throne of Loraine): Bug reports submitted (needs a shader)
- AoE (Age of Enlightenment): Bug reports submitted (needs a shader)
- DoDR (Divergences of Darkness Rework): Bug reports submitted. However, due to the rapid pace of development of this mod it seems unlikely that I will be able to keep up with either a compatibility patch or the bug reports (also, needs a shader)
- DoD:HPM (Divergences of Darkness: HPM): Bug reports submitted
- HtA (Heirs to Aquitania): made a compatibility patch before realizing that the developer was still active. Bugs should be fixed in the mod now.
- ToK (Trail of Khans): Bug reports submitted
- GFM (Greater Flavor Mod): on hold. One of their developers asked us to wait with the bug reports until their next major release was done.
- TGC (The Grand Combination): currently incompatible (although, sent a handful of bug reports anyways). TGC makes some fundamental changes to poptypes and province buildings that Alice isn't currently designed to accommodate. We may eventually end up with a patch that takes the extra stuff out, or leaf may end up adding support, but neither has been done yet.

## What mod compatibility means

Recently, I have been loading different mods to see what works and what doesn't. This has proven to be a somewhat frustrating experience because Victoria 2 is extremely permissive in what it accepts. This doesn't mean that it is extremely permissive with what works, but in many cases mods can limp along with all sorts of bugs. This causes two issues for us. First, Alice was partly designed to make writing bug-free mods easier. Thus, it will complain about every problem it finds when you try to launch a mod, often resulting in a somewhat intimidating list of errors. As far as I am concerned, a mod that generates an error report isn't compatible with Alice (yet).

The second issue is how we handle those problems. The approach in Alice is generally to cut them out so that whatever remains can work bug-free. This means that if an event trigger refers to an undefined tag, for example, that part of the condition would be omitted, which in turn may lead to an event triggering much more often than expected. Victoria 2 ... does other things, ranging from crashing to failing to trigger at all. Thus, while you can still play a mod that generates an error report, it may not run the way it did in Victoria 2.

For mods that are still under development, I have been submitting (often hundreds of) bug reports. My hope is that by fixing these bugs, we can end up with mods that both work better for Victoria 2 and which can be loaded without issue by Alice. I have started writing compatibility patches (small mods for the mods that replace the files with bugs) for some mods that are no longer being developed.

Another barrier to perfect mod compatibility is the graphical changes some mods make to the map. Alice uses OpenGL to render its graphics (to make running on linux easier), while Victoria 2 used DirecX, and thus we cannot load the modified shaders that some mods rely on. The most common result is the map looking either extremely dark or extremely light when zoomed out. This too is a fixable problem. Our shaders can also be modded, and so we can either add an OpenGL shader to these mods to get them to render properly in Alice, or (if the mod developers aren't interested in supporting Alice) we can add the modified shader to a compatibility patch.

![too dark MHM](mhm.png)
![too light ToL](tol.png)

## Multiplayer

I don't know if we will have multiplayer as part of the 1.0 release, but leaf has been working on it this month. Her report is as follows:

Multiplayer has been developed through October, mostly ironing out inconveniences, for example, you can now kick and ban people in a lobby, even if already in-game (something not usually possible in vanilla, except with the assistance of mods). Chatting has also been improved, showing messages in real-time of who got banned, who joined, and who left, in a single window, which can be opened with TAB. We also improved networking queues for it, in other words, we ensured that commands sent to a client are actually received by the client in full form, not just randomly cut-off like it happened (which was a bug that got fixed).

We have also been testing multiplayer in the search of bugs and primarily asserting that gameplay is more or equally enjoyable as vanilla, you may also wish to join our "Multiplayer" team in our discord server. As for the multiplayer itself, it isn't considered "stable" enough to be pushed upstream, so it is released separately, it is usually more "bleeding-edge" than normal releases, incorporating new changes as soon as they are pushed upstream, due to more bugs being fixed = less OOS.

## The End

See you again next month! (or, if you can't wait that long, join us on [discord](https://discord.gg/QUJExr4mRn))