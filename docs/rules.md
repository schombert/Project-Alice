# Game rules

## Things to do in a daily update

- bring wars into a valid state
- either bring blockades into a valid state (this has knock on effects on the percentage blockaded and the modifier derived from that) or recalculate what is blockaded from scratch
- bring combats into a valid state (if the units wouldn't start fighting given the current set of wars, kick them from combat)

- update the dynamic modifiers (things like literacy where the size of the modifier is multiplied by the value) -- maybe (schombert notes: not that anyone would be able to tell if we updated them once every X days and split the updates for all the nations over the X days in the cycle, just something to think about.)
- removed expired province and national modifiers

- update cached economics values: Since there is a single global price for all commodities, a number of things can probably be cached at the beginning of each tick, assuming that we want to update the prices late in the tick

- update wars -- increase ticking war score based on occupying war goals (po_annex, po_transfer_provinces, po_demand_state) and letting time elapse, winning battles (tws_from_battles > 0)

## Tracking changes

There are some triggers, game rules, and ui elements that need to know the change of certain things, such as pop growth, migration amount, changes in pop wealth perhaps, etc. We will need to decide if these things are tracked by either resetting some value to zero at the beginning of the day and then increasing it over the day or by ensuring that all changes happen in a single place, so that the new delta can be written once (versus writing zero and then adding updates as we go). The second option is better for ui stability, since otherwise reading such a value over the course of a tick runs the risk of not just giving you an out-of-date value, but an incorrect one. We will also need to decide if we are storing these delta values in the save file. If we don't there are two consequences: (a) there will be no information about them in the ui on the first day after a load, and (b) we will need to make sure that they are always generated prior to being needed by any rule; we can't fall back on using the change value from the previous day as there may be no previous day.

### Paths and connectivity

It may be expedient to create some structures tracking properties of pathing / connectivity. Some properties, such as being "overseas" require in part the ability to check whether one province is connected to another. One way to do this would be by "flood filling" connected provinces with increasing ids until there are no more land provinces. Then, same ID => connected.

### Other values to track (typically we need these stored to evaluate one or more trigger conditions)

- number of ports
- number of ports connected to the capital
- number of overseas provinces
- number of rebel occupied provinces
- fraction of provinces with a crime
- whether any two nations are adjacent (i.e. a province owned by one is adjacent to a province owned by the other)
- whether any two nations each have a colonial province adjacent to a colonial province of the other
- how many regiments total could possibly be recruited, and what number actually have been recruited
- total number of allies, vassals, substates, etc

## Modifiers

Both nations and provinces have a set of properties (a bag of float values) that may be affected by modifiers. Some modifiers attached to a province may also affect the bag of values associated with the owning nation. In general, as properties are added or removed, we add or remove their affect on the bag of values (we try not to recalculate the whole set as much as possible). This also means when a province changes ownership that we need to remember to update its affect on the national set of modifiers.

Some modifiers are scaled by things such as war exhaustion, literacy, etc. Since these need to be updated daily, there is a second bag of values attached to nations just for these

## Economy

### Share factor

If the nation is a civ and is a secondary power start with define:SECOND_RANK_BASE_SHARE_FACTOR, and otherwise start with define:CIV_BASE_SHARE_FACTOR. Also calculate the sphere owner's foreign investment in the nation as a fraction of the total foreign investment in the nation (I believe that this is treated as zero if there is no foreign investment at all). The share factor is (1 - base share factor) * sphere owner investment fraction + base share factor. For uncivs, the share factor is simply equal to define:UNCIV_BASE_SHARE_FACTOR (so 1, by default). If a nation isn't in a sphere, we let the share factor be 0 if it needs to be used in any other calculation.


## Military

### Wars

- Each war has two sides, and each side has a primary belligerent
- Primary belligerents negotiate in peace deals for their whole side
- Secondary belligerents may in non crisis wars negotiate independently with the primary belligerent on the opposite side
- Wars must be kept in a valid state (see below), generally this is done by removing the cause of the invalid state. Any further problems caused by such a removal may not be caught until the next check of war validity
- A war that ends up with no members on one (or both) sides will simply be ended as a whole
- A province is blockaded if their is a hostile sea unit in an adjacent sea province and no ongoing naval combat there

#### Ticking war score

- ticking war score based on occupying war goals (po_annex, po_transfer_provinces, po_demand_state) and letting time elapse, winning battles (tws_from_battles > 0)
- limited by define: TWS_CB_LIMIT_DEFAULT
- to calculate: first you need to figure out the percentage of the war goal complete. This is percentage of provinces occupied or, for war score from battles see Battle score below

##### Battle score

- zero if fewer than define:TWS_BATTLE_MIN_COUNT have been fought
- calculate relative losses for each side (something on the order of the difference in losses / 10,000 for land combat or the difference in losses / 10 for sea combat) with the points going to the winner, and then take the total of the relative loss scores for both sides and divide by the relative loss score for the defender.

#### Invalid  war states

- A primary belligerent may not be a vassal or substate of another nation
- War goals must remain valid (must pass their is-valid trigger check, must have existing target countries, target countries must pass any trigger checks, there must be a valid state in the target country, the country that added them must still be in the war, etc)
- Crisis war goals may not be removed -- this may override other validity checks
- There must be at least one wargoal (other than status quo) in the war
- idle for too long -- if the war goes too long without some event happening within it (battle or occupation) it may be terminated. If something is occupied, I believe the war is safe from termination in this way

## Rebels

### Calculating how many regiments are "ready to join" a rebel faction

Loop over all the pops associated with the faction. For each pop with militancy >= define:MIL_TO_JOIN_RISING, the faction is credited one regiment per define:POP_SIZE_PER_REGIMENT the pop has in size.

## Other concepts

- overseas province: not on same continent as the nation's capital AND not connected by land to the capital
- province value type A: 1 (if unowned or colonial), otherwise: (1 + fort level + naval base level + number of factories in state (if capital province of the state state)) x 2 (if non-overseas core) x 3 (if it is the national capital) -- these values are used to determine the relative worth of provinces (I believe) as a proportion of the total type A valuation of all provinces owned by the country.
- a port: I believe that any coastal province is considered to have a port, regardless of whether it has a naval base
- revanchism: you get one point per unowned core if your primary culture is the dominant culture (culture with the most population) in the province, 0.25 points if it is not the dominant culture, and then that total is divided by the total number of your cores to get your revanchism percentage
