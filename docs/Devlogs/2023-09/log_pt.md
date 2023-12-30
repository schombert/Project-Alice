# Progress as of September 2023

Welcome back to the [September](https://www.youtube.com/watch?v=nfLEc09tTjI) update. As usual, this update comes with an [updated demo](https://github.com/schombert/Project-Alice/releases/download/v0.0.6-demo/2023-9-7-DEMO.zip).

## The little things

This is going to be a relatively wordy update, and I know everyone hates that. So before we get started, here are some pretty pictures:

Units gathering to a rally point:
![rally point](rally_point.png)

Box selection working:
![box selection](box.png)

## Public alpha

We are getting close to releasing a public alpha. Since we already have a demo available, this might not make much of a difference to some of you. To me, however, an alpha means two things. First, it means that, while there are still going to be bugs and rough spots, you could enjoy a complete game with some luck. Second, it means that I will be opening the door to bug reports from the general public. Depending on the number and quality of those bug reports, we may be able to move on to a beta next month (the difference being that in beta you probably won't see major bugs, while in alpha you probably will).

The alpha releases will probably have the same limitations as the demos. This means that they will only run on Windows 10 or newer and that they will require a CPU that supports AVX2 (this is most CPUs released in the last decade). Some people also report having problems running the demos on graphic cards with sufficiently ancient OpenGL support, although leaf has been providing a workaround for that where needed.

### Making a bug report

If you don't intend to make any bug reports, you can skip this (somewhat long) section.

As already indicated, moving from alpha to beta will be driven largely by quality bug reports. Mechanically, making a bug report is easy: you just go to the bug-reports section of our discord and leave a message there. However, a simple bug report of "X is wrong" is typically not very useful, and maybe even useless. To fix a bug, a developer needs three things: to be able to identify it, to know what the correct behavior should be, and to be able to reproduce the bug for themselves. The bug report that you will be immediately inclined to write will probably not provide all of those.

Suppose, for the sake of argument, that you encountered a bug where declaring a war with a particular CB didn't give you the expected infamy. You might be tempted to make a post along the lines of "Declaring war with CB X didn't give the right amount of infamy." This may seem like it says everything that it needs to say, but it is actually pretty terrible in terms of getting the bug fixed. First, it doesn't pin down the bug very well. Does *every* war declared with that CB give the wrong amount of infamy or just some of them? And does this problem affect other CBs as well? The more you can tell us about the conditions where you have encountered the problem, the more likely a developer is to identify what the bug actually is.

The next thing missing from this bug report is information about what should be happening. It is not enough to say that the CB gave you the wrong amount of infamy: you should explain the amount of infamy that you expected to get and why you expected that. Without that information, we may not know what sort of fix to make. In the current demos, for example, movement speed, combat, and sieges are all extra fast. In time, we will eventually slow them down, but I don't think that we will be able to slow them down to the exact speed that they were in V2. This is in part because I don't know the exact factor to slow them down by, and so a bug report telling me that, say, battles are too fast isn't going to help improve things because it doesn't really bring me any closer to figuring out how fast they should be.

Finally, to get a bug fixed a developer needs to know how to see it for themselves, because that is often the only way to see what exactly is going wrong and to know whether a particular change will really solve the problem. Even in our simple example, a good bug report for this problem needs a set of steps guiding the developer to reproduce it. If the CB is available from the start, that could be as simple as saying that declaring on nation A using nation B is enough to see the bug. However, if this is some sort of conditionally available CB, then you would need to include basically a short walk through explaining how to get to the point where we can see the bug. It may seem obvious to you, but finding a bug by trial and error is extremely wasteful of developer time. If the bug can't be seen in the first few years, you may also want to attach a save (and its corresponding scenario file, and the ordered list of mods, if any) where the bug can be easily found.

## Launcher

The most recent major addition to Project Alice is the new launcher.

![Launcher](launcher.png)

The launcher isn't just a place to select mods and start the game. Project Alice uses what we call "scenario files" to speed up loading the game. A scenario file is essentially an efficiently packaged version of the game data (as modified by a given set of mods). When running the game for the first time, or selecting a new combination of mods, you will first have to create a scenario file using the launcher (this will take more or less time depending on the speed of your computer, your hard drive, and the complexity of the mods). If you update a mod, you will need to recreate the scenario file for any changes in the mod to show up (and your old saves will not be available when using the new scenario file, so be warned). Scenario files are placed in `Your Documents\Project Alice\scenarios` while save files are located in `Your Documents\Project Alice\saves`.

## HPM Compatibility

As a first step towards more general mod compatibility, we focused on getting HPM to load properly last month. In doing so, we discovered three minor bugs in the HPM files. Thus, to prevent the launcher from nagging you about them, we have also released a small HPM compatibility patch for them (available in the mod-compatibility-patches channel of the discord).

![HPM globe](hpm_globe.png)

It still needs a few tweaks to make text fit properly in some places with the new fonts, but most of the changes that HPM made loaded painlessly.

![HPM ui](hpm_ref.png)

## The End

See you again next month! (or, if you can't wait that long, join us on [discord](https://discord.gg/QUJExr4mRn))