# Progress as of April 2024

We missed you all, and we're sorry for not making a March dev diary, but sometimes you want to "stack things" until you have enough to talk about.

Most of the things that were done on March were of technical aspects, such as bug fixes, nudges, and fixing some glitches. Not a lot to showcase visually.

## 1.0.13

As of writing, the current version is 1.0.13, this new version includes many new features, fixes for mods and extremely useful features for management and gameplay.

## Colorblind mode

One key aspect I've wanted to work on since I first joined this project was accesibility: Making the game more accessible for people with disabilities or impairments.

I think the original is pretty much garbage at this, contrast is low, and the only option that barely ressembles a "constrast" option is the gamma filter.

I wanted to change this, hence, I started to identify key areas of the game that required contrast to be more visible, recolour elements so that key information isn't lost and ensure that the general experience is as smooth as possible for visually impaired people.

Both images are using a simulator to simulate how a colourblind person would see them, first one is no adjustment (colour blind mode is off), second one is adjustments for deutan and protan:

![](./images/cb_pro1.png)

![](./images/cb_pro2.png)

For now it is still very basic and pales in comparison to making the game unviersally distinc (try playing in grayscale and using achromatic mode, you'll most likely encounter issues).

But it's a good foundation for building upon, and to provide further accesibility to our users.

## DDS fixes

A rare case of DDS files being accidentally discoloured or misinterpreted caused issues on some mods, most notably crimeamod and GFM, for example the colours of some files were not appropriate (like a blue tinted horse on crimeamod). Or the GFM pauli branch having a broken tariff slider due to the DDS file not being correctly parsed.

Generally, DDS files are either uncompressed or compressed, the many issues come from the uncompressed side, because the compressed one is handled by the GPU.

So we finally put an end to discoloured DDS files. Hooray!

![](./images/tariff.png)

*Previously this slider was broken and unusable in GFM (Pauli Branch), but now it is fixed!*

## Regional names

Previously, "British Oceania" was the name used for... Australia. This is no longer the case, now you can be sleeping knowing that "British Australia" is displayed proudly on the map.

![](./images/australia.png)

*Wonderful and ominous Australia*

## TNO Fixes

An issue present in the mod TNO (The New Order) was that:
- Atlantropa wasn't properly given to their respective nations (Italy, Iberia, etc)
- OOB files were ignored because they only had the tag name (instead of being "IBE_oob.txt" for example)

Fortunately now you can enjoy this lustful and gorgeous mod again.

![](./images/tno.png)

*Ah, I wonder if Italy can manage to survive for more than 5 years in TNO*

## Infinite recursion?

One constant issue we had with mods was that they would often crash, but it wasn't because we did something wrong (well, in the early days we did, but not most of those bugs are fixed).

No, the issue was that events were calling upon themselves, and then it made an infinite loop which caused a stack overflow!

Imagine every time you call an event within an event you add a "block" to a "stack", this "stack" however has a limit, and that limit is around 8 MB (on Linux at least), so go over that and your program will crash.

But fear not! If you wish to do crazy things with events, you still can! You just have to upper the limit they're bound at (how many recursions an event can perform on the same day) - with the `alice_max_event_iterations` define! And by default it is 8, so more than enough for most sane mods.

![](./images/event.png)

*What? You call events within events? - Sure enough!*

## Nation Picker improvements

Now you can see a tooltip describing, briefly, the nation you're hovering.

Additionally, the whole nation selected will be outlined and highlighted, for ease of view.

![](./images/picker.png)

## Graceful errors

Errors, for example Network Errors, will now be shown in-game, and no longer cause a crash.

Additionally, players who get these errors will automatically put their nation as "no-AI". The host can then "unset" it by kicking the "ghost" player.

![](./images/error.png)

## Music player and settings

Don't like the current melody? Or wanna just hear the same song over and over again? Well, you can now! With the music player.

You can also "mute on focus lost", so when you click out of the window or do other things, the music and background SFX won't play out.

![](./images/player.png)

## Macro builder

Improvements to the macro builder were made, small bugfixes and some new interface features!

![](./images/macro.png)

*A new icon for the macro builder*

![](./images/macro2.png)

*And some new UX improvements*

## Control groups

Select a unit (or various units), then press Ctrl-1, or Ctrl-2, up to Ctrl-9. This will add them to their respective control group, be it 1, 2, 3, and so on.

It will show in the outliner in a parenthesis, *you can also add units to more than one control group*.

Then press 1, 2, 3, etc, to select their respective control groups instantly. Making micromanagement easier.

![](./images/control.png)

## Automatic frontlines and AI control

If you want to avoid doing busywork or want to make better strategic decisions when it comes to hunting rebels, or simply want to invade a country without having to micromanage things (for example, invading Caucasus in TGC as Russia). Then you can give the AI control of some of your units, via a new checkbox for them.

The AI will:
- Automatically reinforce frontlines
- Automatically siege when it's safe to do so
- Hunt rebels
- Form a defensive position against a nation we are likely to attack (AI will mostly guess)

![](./images/AI.png)

## Effective prices

While the original economy didn't have local prices, we added them, this way we can model different prices through nations, allowing for divergent "domestic" and "international" trade prices.

![](./images/economy.png)

## Console improvements

Additionally, new console commands have been added, while some broken ones were obviously fixed.

We also added some new sound effects to the console, so it's less "silent", to say the least.

![](./images/con1.png)

*A checkmark and a cross to indicate the state of cheats*

![](./images/con2.png)

*New console commands, add pop, instant_industry, tcore, innovate*

![](./images/con3.png)

*Suggestions for innovate command*

## That's all for now!

We are pleased to have made it into 1.0.12, and we hope to see you again in May!

Katerina will keep improving mod compatibility and multiplayer, whereas Schombert will continue working on revamping the UI to be more accessible (and modifiable) for international localisation.

Peter has helped to revamp the economy significantly, adding a new, complex layer of gameplay opportunities to the otherwise "dull" economy.
