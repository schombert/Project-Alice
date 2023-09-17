# Project Alice Community modding Suggestions

**General Prompt give to each modder for refferance**
Been asking for suggestion that modders might have wanted for Vic2 moddability since there is another OpenV2 attempt, idk if it will work but I want to help them where I can
ctrl+c ctrl+v section What are some suggestion you or other modders you know, would want to be more moddable about Victoria 2?
With the stilted prompt out of the way, notes I would like to make, and overall game mechanics will remain stay close to orginal Victoria 2 as possible. They plan that Victoria 2 mods will be compatible with OpenV2 Final note, the project is pretty early, and this is for planning purposes
The document will cover parts of modding however you can throw non-modding QoL and MP stuff in as well
Scripting - This for everything like event and decision, but also on-actions and UI events, IF operator, <>, more scopes, conditions, modifies, effects, etc.
Economy - anything to do with goods, buildings, production, trade, money, and pop relations to the economy
Politics - anything with the politics, movement/rebel screen, decisions, release nations window and pop politics or additions in that vain
Pop - anything that pops have to deal with direction and the modding of them like migration, promotions, jobs, political alignment, there relation to other pops, being accepted or not and who owns what economically as examples
Tech - just tech, how you would like the mod research point and civilization mechanics
Military - military stuff like having a defines to change roll amount or better leader creation
Mapping - Terrain abilities, how provinces are made or defined, rivers, states and the like
Diplo - the actions you would moddable, peace deal, cb, infamy, crisis etc.
Graphics and GFX - anything you can think of about looks and GFX
UI - they are planning UI already but anything you can think of yourself
Misc - anything else modding or suggestion wise

## Rascalnag:

Graphics and GFX - more customizable shader pipeline. Honestly Victoria 3 did well in this regard since you can replace and add new shader files pretty easily. So similar flexibility would be nice. Also being able to add new textures (not just replace existing ones) more easily/at all

UI - UI modding generally seems sucky to me in Vicky 2 so a big revamp to the process of making UI elements should be in order

Military - This should be quite modular. The ability to add a, for instance, supply lines module that scales attrition/tactics for a unit in a province depending on the existence of an actual supply line capable of supplying a unit’s needs would be amazing and encourage a transition to sustainable front strategy as units got more hungry with tech. But it would a complex undertaking to have a system capable of both representing Victoria II base warfare and such a large departure from Victoria II approaches. Especially considering AI would need companion modularity to work with any such addons

## MajorMajor:

Scripting -
Generally more functionality for province modifiers, I'd like to set for instance administration efficiency on a per-province basis and I don't think that's possible right now. More way to scope accepted/non-accepted culture/religion pops with these modifiers too on a province level

Economy -
I'd be great IMO if provinces had a base level of "subsistence goods" in addition to the "specialised/export" RGO. Right now unciv pops can starve if they don't have provinces with grain/fish/cattle. Maybe even tie pop growth to mechanisation increasing surplus beyond subsistence levels

Diplo -
HOI3- style guarantees would be great, and they'd fit the time period

## Atlas:

(Mostly agreed with MajorMajor)
Mapping - changing terrain mid game, making river crossing penalties modable, more canal effects and influences (also more canals in general, besides the 3 standard ones) and navigable rivers

Politics - More options for modders to work with ideologies and governments, e.g. more fluid and less forced ideology drifts, democracies also working for uncivs; stuff like that

Pops - more influence of religion (Atheism should actually exist) on pops, better and more modable assimilation, fixing craftsmen exploit and bugged migrations (instant assimilation and instant migration of pops that are smaller than 100)
oh yea
pops also should be able to have multiple languages
so making "culture" more modable would be big

## Jupiter:

Converting Religions and better assimilate features would be nice ngl

## Gelre:

Having Vic2 mods be auto compatible with OpenV2 means its flawed by design, so many things in Vic2 coding could be done way better to make life way easier for modders

Also most important change at least imo is to incorporate the feature newer PDX games also have to spread out the event and decision workload by not having them be checked every day but only once every X days

## Doktor Ebin:

wargoals
I want to be able to go beyond 100 (or techincally 101 warscore)
OH
better logging
please, have better logging so crashes become MUCH easier to track down
such as logging what decision gets executed or what event got fired before putting them in action so that if a crash happens, we can track down exactly what caused it to crash
reason I say this and I'm sure you already know is because game logs log events and decisions after they've been completed so if a crash happens, it interrupts the decision or event which causes them to not get logged. Whereas logging them when they're first executed or fired before anything else happens would massively improve QOL for modders since they'll be able to read the log and find the very last entry and say "yep, this is the cause"

## Savs:

Ability to scope to things more freely / variably, as in Vic3. Proper IF with the ability for custom loc

## SirRunner:

Total conversion mapping is a bitch 😆
100% would want to be able to create pops out of thin air
And have the ability to move them to provinces without hard coding the province
Have the ability to put anything that can be done via tech as a modifier - like I would love to have the ability to give another national focus via a modifier to make it temporary
A more moddable ui. It's so hardcoded that basically all that one can do is add non-interactive components or rearrange things
UI specific, I would like to have major event and country events use different buttons
And to have "interesting countries" be a thing for choosing a country
The ability to use imported models - I would love to have use at least racially correct models in Vic 2
Also being able to have buildings be able to have more than 1 effect. That's really annoying
Also a pet peeve. It seems like crises are hard coded to expect at least 8 GPs. It's really easy to have crises be broken in TTA due to only having 4 gps
Ooooh. Related to pop effects, being able to change the size additively in addition to multiplicatively  
Thought of a couple more:
Making the check for leadership work. Only the effect works, which isn't really helpful
Being able to assign a portrait and id when creating a general
And define when a general dies rather than having it be random  
Oooh. Also making triggered modifiers show up somewhere

## DeNuke:

IF
... but that's the obvious meme one.
@Jman Has anyone suggested custom tooltips?
The big dammed feature from HOI4 which enables you to actually have visible localization for specific events.
Rather then being forced to show a blank.
A simple feature, but adds so much localization context to scripted event chains that does not currently exist.
As for "hidden code" I would love to know exactly what.... uh... what was it.
Currently there are modifiers that only work in the tech/inventions area or the issues area.
You can only use specific modifiers as country modifiers.
All other modifiers have to be done though technology or creating a new issue.
Well yeah.
Currently with events that just set flags I'm basically forced to show... nothing…
I think you should also make all modifiers universally available.
Currently there are modifiers that only work in the tech/inventions area
You can only use specific modifiers as country modifiers (or issues)
All other modifiers have to be done though technology/inventions.
@Jman Then of course you have the most important item that is unknown: "The code behind how capitalists are weighted to select which buildings to fund"
Which is hardcoded invisible code.
There must be a massive weight towards clippers.
Which I mean. Yeah. Makes sense. You want the AI to be able to build boats.
However that should cut off at a point if they have researched steamers.
We really have no control over the AI weights beyond technologies.
Wait. No. Defines as well.
Its really just defines and technologies.
Everything else is hardcoded and invisible.
So yeah, cracking open the AI weights is something that should be a priority since the AI only really functions well with default Vic 2 currently. It cannot address the existence of modded buildings or units without inevitable malfunctioning.  
So yeah, cracking open the hidden AI weights is something that should be a priority since the AI only really functions well with default Vic 2 currently. It cannot address the existence of modded buildings or units without inevitable malfunctioning.
... or of course any of the hardcoded functions and variables behind the economy which we all know exist, but can't touch

## settintotrieste:

Hi jman, I think what can make v2 better is to remove the 8 building per state cap, the fixed number of GPs and auto de puppet from becoming a GP. Aside from that an immigration system that does not have a hard cap on how many countries can receive immigration is also good. Rebels also need to be untied to how much money they have. A system to prevent the election of certain ideologies under some government types would also be good.

## General Townes:

(Note: His suggestion was through VC going to memory and isn't verbatim)
His suggestions were primarly economics base as most of his suggestions were already covered my someone else. More configurable economy, such as defines for prioritization of trade the ability, province/state buildings can produce goods, input costs, be owned, and employ pops simlar to a factory, being able to have more 8 factories and have inputs be more than 4 goods or be able to dynamically change based on profitablity, ex; Wool to Textiles can get invested in dynamically to turn to Cotton to Textiles. More Dynamic parties having the ability to change policy over time, parties from annexed countries of accepted culture appearing.


## MaTi5893
(Note: His suggestion was through VC going to memory and isn't verbatim)
Agree with most everything else stated before, with his main concern being event, decisions, and UI modding.

## Jman

Victoria 2 is inconsistent in a lot of it's commands
but probably an error log to quickly trace crashes and bugs, since it is a major issue I have to deal with every time I mod the game is dealing with an unexpected crash and searching through files and validators to find what caused it
other then that like Victoria 2 modding commands that don't break the game or work as intended (for example commands like unit in province removes RGOs from a province or negative variables don't insta-crash)
also some new commands, like rebel spawing, pop spawning, and probably scopes and other modding abilities from newer games
having timed on-actions where you can have an mod script launch effects every like day, week, month, year, etc.
HOI4/EU4 era Boolean Operators https://hoi4-modding.fandom.com/wiki/Operators
I am not the most in depth economy modder, but some sort of building system that can employ pops but isn't an RGO or Factory, this would have been pretty useful for original AoE's economy - also good that can go into the same input like wool and cotton for fabric
also RGO output beyond just more pop and tech, certain provinces could have major mines but but doesn't produce purely on pop (Elba had a massive mine but due to the low pop in Vic2 it could never be as effective)
Economy modders I know are more mechanical rather modding, I would presume stuff like being able to turn off/on sphere duping and more defines options
in politics there is not much to ask for that isn't a mechanics rework, but having the ability to choose what voting methods imply rather then PDX made presets
Pops, probably a pop modifier or trait ability, being able to scope underemployment and pay
Map modding has always iffy, having the ability to have un-used province ids would stop the issue of one person being able to map mod at a time  
Taking individual provinces in war and concessions in peace (US paying war-reps after taking land from Mexico to keep infamy in check for example)  
Province Flags should also made to work
Cosmetic tags and flags would also be nice.

### Modders like Tawani and Zombie_Freak did respond but said they mostly agreed with what said already.

## Kiwi/ColdSlav

  ### Modding functionality - Economics
  - The ability to check for good prices, either by a fixed amount or by a determined percentage of the good's defined default price.
  - This would also imply the ability to scope goods, either individually, or by group (or other factors that the modder may deem suitable)
  - The ability to artificially control good prices (definitely not harmful to the economic system), in the same way as above
  - The ability to scope factories, and check them by type goods produced, input goods consumed (by a modder-defined amount of time), output goods produced (the same way as input), profitability, employees, employee ratio, presence of X type of employyes, cash reserves, if upgradeable, under construction, construction time?, amount (in country), and more?
  - The ability to not only build factories in the capital but also any state
  - The ability to modify the amount of factories per state, or even better (if easily implementable), the ability to set factory amount caps on a state basis
  - the ability to check for most-produced goods on a state level, nationally and internationally (this already sort of exists in vanilla Vic2, but it has barely been used and the way it works is poorly understood)
  - moddable wages, the abiliy to have a defined base wage value, and the ability to modifiy them on a PoP type, country, factory, and perhaps region basis.
  - Fix the income modifiers, and also include more of the same modifiers but pop type-based rather than just strata-based
  - More economic modding
