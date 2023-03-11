# Game rules

## Things to do in a daily update

- bring wars into a valid state
- either bring blockades into a valid state (this has knock on effects on the percentage blockaded and the modifier derived from that) or recalculate what is blockaded from scratch
- bring combats into a valid state (if the units wouldn't start fighting given the current set of wars, kick them from combat)

- update the dynamic modifiers (things like literacy where the size of the modifier is multiplied by the value) -- maybe (schombert notes: not that anyone would be able to tell if we updated them once every X days and split the updates for all the nations over the X days in the cycle, just something to think about.)
- removed expired province and national modifiers

- update cached economics values: Since there is a single global price for all commodities, a number of things can probably be cached at the beginning of each tick, assuming that we want to update the prices late in the tick

- update wars -- increase ticking war score based on occupying war goals (po_annex, po_transfer_provinces, po_demand_state) and letting time elapse, winning battles (tws_from_battles > 0)

- update demographics
- update admin efficiency
- accumulate research points

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

If the nation is a civ and is a secondary power start with define:SECOND_RANK_BASE_SHARE_FACTOR, and otherwise start with define:CIV_BASE_SHARE_FACTOR. Also calculate the sphere owner's foreign investment in the nation as a fraction of the total foreign investment in the nation (I believe that this is treated as zero if there is no foreign investment at all). The share factor is (1 - base share factor) x sphere owner investment fraction + base share factor. For uncivs, the share factor is simply equal to define:UNCIV_BASE_SHARE_FACTOR (so 1, by default). If a nation isn't in a sphere, we let the share factor be 0 if it needs to be used in any other calculation.

### Artisan production selection

During its monthly pop update tick (see below), if an artisan hasn't been satisfying its life needs, it may switch to producing another, unlocked, commodity (this is more likely the longer the artisan pop has not been satisfying its life needs). It will not switch to a good where the following are true: it is not base profitable (i.e. raw input-prices x input-quantities - output-price x output-quantities is positive, with no modifiers considered), total world real demand is under total world real supply (schombert notes: maybe simpler to just check the direction of the price movement recently), or where there is no available supply of one or more inputs. The choice of what to produce among the commodities not rejected by the above seems to be weighted by the unit profit, the availability of inputs, how much world demand exceeds the supply, and by the presence of a national focus encouraging the output.

### Construction

(Note: this applies to both military unit and building projects, I believe)
- A project under construction demands 1/100th of its total commodity cost per day until enough of each commodity has been consumed.
- A project under construction takes its daily demanded commodities out of the national stockpile (up to availability). It then loses money based on the price of those commodities (although this amount is not allowed to leave it with negative money, so the possibility of free commodities exists), and the nation gains money equal to the amount the project loses.
- Note that much of this is entirely wasted computation if the nation itself has funded the project: it puts money into the project only to pay itself back. In the case of private investment, it would be simpler just to pay the nation directly at the start. In the end, it will all come to the same thing, and it removes a non-trivial amount of bookkeeping.
- The amount demanded by projects is added to the nation's daily demand for good

### Overseas penalty

For each commodity that has been discovered by *someone*, a nation pays define:PROVINCE_OVERSEAS_PENALTY x number-of-overseas-provinces per day. Then take the fraction of each commodity that the nation is *not* able to supply, average them together and multiply by 1/4th to determine the nation's overseas penalty.

## Pops and demographics

### Monthly updates for individual pops

The updates described in this section are run once per pop per month, spread out evenly over the month. Rather than trying to figure out the number of day in the month, and divide pops among those days, we should probably just pick a fixed interval. We also **do not** want to divide the updates for the pops by taking their lower N bits to group them. For performance reasons, it is important to update the pops in groups of contiguous ids.

This monthly update contains:
- determining what political or social movement the pop is part of
- determining what rebel faction the pop is part of
- determining if an artisan should switch production, and to what
- growing or shrinking the pop
- if the amount of people to move for the pop are either greater than its type's minimum promotion size or the size of the pop as a whole, the pop may partially migrate, either internally or externally or colonially. (note: slaves do not move or migrate)
- as above, but for promoting and demotion by pop type
- religious conversion -- number converted = define:CONVERSION_SCALE x pop-size x conversion chance factor (factor is computed additively). There appears to be some weird logic surrounding pops with religion marked as "pagan" or converting to such a religion, where pops in those cases won't convert unless there is already a matching pop with the same culture & type and with the religion to be converted to in the province. Not clear if we should try to replicate this logic. Also, slaves do not convert.
- cultural assimilation -- Limitations: slaves do not assimilate, pops of an accepted culture do not assimilate, pops in an overseas and colonial province do not assimilate. For a pop to assimilate, there must be a pop of the same strata of either a primary culture (preferred) or accepted culture in the province to assimilate into. (schombert notes: not sure if it is worthwhile preserving this limitation) Amount: define:ASSIMILATION_SCALE x (provincial-assimilation-rate-modifier + 1) x (national-assimilation-rate-modifier + 1) x pop-size x assimilation chance factor (computed additively, and always at least 0.01). If the pop size is less than 100 or thereabouts, they seem to get all assimilated if there is any assimilation. In a colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a factor of 100. In a non-colonial province, assimilation numbers for pops with an *non* "overseas"-type culture group are reduced by a factor of 10. All pops have their assimilation numbers reduced by a factor of 100 per core in the province sharing their primary culture.

### Growth

Province pop-growth factor: Only owned provinces grow. To calculate the pop growth in a province: First, calculate the modified life rating of the province. This is done by taking the intrinsic life rating and then multiplying by (1 + the provincial modifier for life rating). The modified life rating is capped at 40. Take that value, if it is greater than define:MIN_LIFE_RATING_FOR_GROWTH, subtract define:MIN_LIFE_RATING_FOR_GROWTH from it, and then multiply by define:LIFE_RATING_GROWTH_BONUS. If it is less than define:MIN_LIFE_RATING_FOR_GROWTH, treat it as zero. Now, take that value and add it to define:BASE_POPGROWTH. This gives us the growth factor for the province.

The amount a pop grows is determine by first computing the growth modifier sum: (pop-life-needs - define:LIFE_NEED_STARVATION_LIMIT) x province-pop-growth-factor x 4 + province-growth-modifier + tech-pop-growth-modifier + national-growth-modifier x 0.1. Then divide that by define:SLAVE_GROWTH_DIVISOR if the pop is a slave, and multiply the pop's size to determine how much the pop grows by (growth is computed and applied during the pop's monthly tick).


### Migration

Internal and external "normal" migration appears to be handled distinctly from "colonial" migration. In performing colonial migration, all pops of the same culture+religion+type seems to get combined before moving them around. Pops may only migrate between countries of the same civ/unciv status.

Country targets for external migration: must be a country with its capital on a different continent from the source country *or* an adjacent country (same continent, but non adjacent, countries are not targets). Each country target is then weighted: Firs, the product of the country migration target modifiers (including the base value) is computed, and any results less than 0.01 are increased to that value. That value is then multiplied by (1.0 + the nations immigrant attractiveness modifier). Assuming that there are valid targets for immigration, the nations with the top three values are selected as the possible targets. The pop (or, rather, the part of the pop that is migrating) then goes to one of those three targets, selected at random according to their relative attractiveness weight. The final provincial destination for the pop is then selected as if doing normal internal migration.

Destination for internal migration: colonial provinces are not valid targets, nor are non state capital provinces for pop types restricted to capitals. Valid provinces are weighted according to the product of the factors, times the value of the immigration focus + 1.0 if it is present, times the provinces immigration attractiveness modifier + 1.0. The pop is then distributed more or less evenly over those provinces with positive attractiveness in proportion to their attractiveness, or dumped somewhere at random if no provinces are attractive.

Colonial migration appears to work much the same way as internal migration, except that *only* colonial provinces are valid targets, and pops belonging to cultures with "overseas" = false set will not colonially migrate outside the same continent. The same trigger seems to be used as internal migration for weighting the colonial provinces.

### Promotion/demotion

Pops appear to "promote" into other pops of the same or greater strata. Likewise they "demote" into pops of the same or lesser strata. (Thus both promotion and demotion can move pops within the same strata?). The promotion and demotion factors seem to be computed additively (by taking the same of all true conditions). If there is a national focus set towards a pop type in the state, that also seems to be added into the chances to promote into that type. If the net weight for the boosted pop type is > 0, that pop type always seems to be selected as the promotion type. Otherwise, the type is chosen at random, proportionally to the weights. If promotion to farmer is selected for a mine province, or vice versa, it is treated as selecting laborer instead (or vice versa). This obviously has the effect of making those pop types even more likely than they otherwise would be.

There are known bugs in terms of promotion not respecting culture and/or religion. I don't see a reason to try to replicate them.

As for demotion, there appear to an extra wrinkle. Pops do not appear to demote into pop types if there is more unemployment in that demotion target than in their current pop type. Otherwise, the national focus appears to have the same effect with respect to demotion.

Pops that move / promote seem to take some money with them, but there is also magical money generated by starter share, at least for capitalists, probably so that it is possible for the first capitalists in a state with no factories to build a factory. I propose the following instead: capitalists building a factory in a province with no open factories (or reopening a closed factory in those conditions) is simply free.

## Politics

### Party loyalty

Each province has a party loyalty value (although it is by ideology, not party, so it would be more appropriately called ideology loyalty ... whatever). If the national focus is present in the state for party loyalty, the loyalty in each province in the state for that ideology will increase by the loyalty value of the focus.

## Military

### Spending

Nations consume (or try to consume) commodities for their land and naval units daily at a rate proportional to their land/naval spending (at full spending they try to consume 100% of the necessary commodities). These commodities are subtracted directly from a nation's stockpiles (and there are no negative stockpiles allowed).

### CB fabrication

CB fabrication by a nation is paused while that nation is in a crisis (nor do events related to CB fabrication happen). CBs that become invalid (the nations involved no longer satisfy the conditions or enter into a war with each other) are canceled (and the player should be notified in this event). Otherwise, CB fabrication is advanced by points equal to:
define:CB_GENERATION_BASE_SPEED x cb-type-construction-speed x (national-cb-construction-speed-modifiers + technology-cb-construction-speed-modifier + 1). This value is further multiplied by (define:CB_GENERATION_SPEED_BONUS_ON_COLONY_COMPETITION + 1) if the fabricating country and the target country are competing colonially for the same state, and then further by (define:CB_GENERATION_SPEED_BONUS_ON_COLONY_COMPETITION_TROOPS_PRESENCE + 1) if either country has units in that colonial state. (schombert: I'm really not convinced that these effects are worth the trouble it would take to implement). When the fabrication reaches 100 points, it is complete and the nation gets the CB.

Each day, a fabricating CB has a define:CB_DETECTION_CHANCE_BASE out of 1000 chance to be detected. If discovered, the fabricating country gains the infamy for that war goal x the fraction of fabrication remaining. If discovered relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states with a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED

When fabrication progress reaches 100, the CB will remain valid for define:CREATED_CB_VALID_TIME months (so x30 days for us). Note that pending CBs have their target nation fixed, but all other parameters are flexible.

### Leaders (Generals and Admirals)

#### Recruitment

Getting a new leader costs define:LEADER_RECRUIT_COST leadership points. If automatic leader recruitment is turned on (and why would you ever turn it off?), then if you have one or more armies/navies that could be given a leader (is not in a currently hostile province, is not at sea, and is not currently retreating) and which does not currently have one, and if you have enough leadership points, new leaders will automatically be recruited. The game seems to try to balance how many general and admirals are recruited by how many you need of each type ... I'm not sure that is worth respecting; I would probably try to fill up the armies first.

New leaders get a random name based on the primary culture of their nation. Leaders also get a random personality and background trait. This seems entirely random *except*--and it gets weird here--admirals are biased away from having personalities and backgrounds that modify reliability, either positively or negatively.

Nations appear to be able to store up to define:LEADER_RECRUIT_COST x 3 leadership points. Nations accumulate leadership points monthly. To calculate the amount, we first take for each pop that provides leadership points, and multiply the amount of points it gives by the ratio of the fraction of the national population composed of that pop type to its research optimum (capping this ratio at 1). We then sum up those values and add the nation's modifier for leadership. Finally, we multiply that sum by (national-leadership-modifier-modifier + 1), giving up the monthly leadership points increase.

#### Daily death chances

Leaders who are both less than 26 years old and not in combat have no chance of death. Otherwise, we take the age of the leader and divide by define:LEADER_AGE_DEATH_FACTOR. Then we multiply that result by 2 if the leader is currently in combat. That is then the leader's current chance of death out of ... my notes say 11,000 here. Note that the player only gets leader death messages if the leader is currently assigned to an army or navy (assuming the message setting for it is turned on).

### Wars

- Each war has two sides, and each side has a primary belligerent
- Primary belligerents negotiate in peace deals for their whole side
- Secondary belligerents may in non crisis wars negotiate independently with the primary belligerent on the opposite side
- Wars must be kept in a valid state (see below), generally this is done by removing the cause of the invalid state. Any further problems caused by such a removal may not be caught until the next check of war validity
- A war that ends up with no members on one (or both) sides will simply be ended as a whole
- A province is blockaded if there is a hostile sea unit in an adjacent sea province and no ongoing naval combat there. My notes say that only either not-overseas (or maybe only connected to capital) provinces count for calculating the blockade fraction

#### Ticking war score

- ticking war score based on occupying war goals (po_annex, po_transfer_provinces, po_demand_state) and letting time elapse, winning battles (tws_from_battles > 0)
- limited by define: TWS_CB_LIMIT_DEFAULT
- to calculate: first you need to figure out the percentage of the war goal complete. This is percentage of provinces occupied or, for war score from battles see Battle score below

##### Battle score

- zero if fewer than define:TWS_BATTLE_MIN_COUNT have been fought
- calculate relative losses for each side (something on the order of the difference in losses / 10,000 for land combat or the difference in losses / 10 for sea combat) with the points going to the winner, and then take the total of the relative loss scores for both sides and divide by the relative loss score for the defender.

##### Invalid  war states

- A primary belligerent may not be a vassal or substate of another nation
- War goals must remain valid (must pass their is-valid trigger check, must have existing target countries, target countries must pass any trigger checks, there must be a valid state in the target country, the country that added them must still be in the war, etc)
- Crisis war goals may not be removed -- this may override other validity checks
- There must be at least one wargoal (other than status quo) in the war
- idle for too long -- if the war goes too long without some event happening within it (battle or occupation) it may be terminated. If something is occupied, I believe the war is safe from termination in this way

### Units

#### Unit daily update

Units in combat gain experience. The exact formula is somewhat opaque to me, but here is what I know: units in combat gain experience proportional to define:EXP_GAIN_DIV, the experience gain bonus provided by their leader + 1, and then some other factor that is always at least 1 and goes up as the opposing side has more organization.

Units that are not in combat and not embarked recover organization daily at: (national-organization-regeneration-modifier + morale-from-tech + leader-morale-trait + 1) x the-unit's-supply-factor / 5 up to the maximum organization possible for the unit.

Units that are moving lose any dig-in bonus they have acquired. A unit that is not moving gets one point of dig-in per define:DIG_IN_INCREASE_EACH_DAYS days.

Units backed by pops with define:MIL_TO_AUTORISE militancy or greater that are in a rebel faction, and which have organization at least 0.75 will become rebel units.

Navies with supplies less than define:NAVAL_LOW_SUPPLY_DAMAGE_SUPPLY_STATUS may receive attrition damage. Once a navy has been under that threshold for define:NAVAL_LOW_SUPPLY_DAMAGE_DAYS_DELAY days, each ship in it will receive define:NAVAL_LOW_SUPPLY_DAMAGE_PER_DAY x (1 - navy-supplies / define:NAVAL_LOW_SUPPLY_DAMAGE_SUPPLY_STATUS) damage (to its strength value) until it reaches define:NAVAL_LOW_SUPPLY_DAMAGE_MIN_STR, at which point no more damage will be dealt to it. NOTE: AI controlled navies are exempt from this, and when you realize that this means that *most* ships are exempt, it becomes less clear why we are even bothering the player with it.

#### Unit stats

- Unit experience goes up to 100. Units after being built start with a base experience level equal to the bonus given by technologies + the nations naval/land starting experience modifier (as appropriate)
- Units start with max strength and org after being built

#### Unit construction

- Only a single unit is built per province at a time
- See the economy section for details on how construction happens
- After being built, units move towards the nearest rally point, if any, to merge with an army there upon arrival

### Siege

Garrison recovers at 10% per day when not being sieged

## Movements

- Movements come in two types: political and social (Here I fold independence movements under the political). Functionally, we can better distinguish the movements between those associated with a position on some issue (either political or social) and those for the independence of some national identity.
- Slave pops cannot belong to a movement

## Rebels

- Rebel fraction: There is a trigger that depends on the fraction of provinces that are controlled by rebels. My notes say that only either not-overseas (or maybe only connected to capital) provinces count for calculating this fraction
- Pops with militancy >= define:MIL_TO_JOIN_REBEL will join a rebel faction rather than a movement (and if the pop's militancy is less than this, it will drop out).
- Picking a faction for a pop:
- A pop in a province sieged or controlled by rebels will join that faction, if the pop is compatible with the faction.
- Otherwise take all the compatible and possible rebel types. Determine the spawn chance for each of them, by taking the *product* of the modifiers. The pop then joins the type with the greatest chance (that's right, it isn't really a *chance* at all). If that type has a defection type, it joins the faction with the national identity most compatible with it and that type (pan-nationalist go to the union tag, everyone else uses the logic I outline below)
- Faction compatibility: a pop will not join a faction that it is excluded from based on its culture, culture group, religion, or ideology (here it is the dominant ideology of the pop that matters). There is also some logic for determining if a pop is compatible with a national identity for independence. I don't think it is worth trying to imitate the logic of the base game here. Instead I will go with: pop is not an accepted culture and either its primary culture is associated with that identity *or* there is no core in the province associated with its primary identity.
- When a pop joins a faction, my notes say that the organization of the faction increases by either by the number of divisions that could spawn from that pop (calculated directly by pop size / define:POP_MIN_SIZE_FOR_REGIMENT) or maybe some multiple of that.

### Rebel victory

Every day, an active rebel faction has its `demands_enforced_trigger` checked. If it wins, it gets its `demands_enforced_trigger` executed. Any units for the faction that exist are destroyed (or transferred if it is one of the special rebel types). The government type of the nation will change if the rebel type has an associated government (with the same logic for a government type change from a wargoal or other cause). The upper house will then be set in a special way (even if the type of government has not changed). How the upper house is reset is a bit complicated, so the outline I will give here assumes that you understand in general how the upper house works. First we check the relevant rules: in "same as ruling party" we set it 100% to the ruling party ideology. If it is "state vote" we compute the upper house result for each non-colonial state as usual (i.e. we apply the standard strata voting modifiers and weight the vote by culture as the voting rules require), then we normalize each state result and add them together. Otherwise, we are in direct voting, meaning that we just add up each pop vote (again, weighting those votes according to the usual rules and modifiers). If the rebel type has "break alliances on win" then the nation loses all of its alliances, all of its non-substate vassals, all of its sphere members, and loses all of its influence and has its influence level set to neutral. The nation loses prestige equal to define:PRESTIGE_HIT_ON_BREAK_COUNTRY x (nation's current prestige + permanent prestige), which is multiplied by the nation's prestige modifier from technology + 1 as usual (!).

### Calculating how many regiments are "ready to join" a rebel faction

Loop over all the pops associated with the faction. For each pop with militancy >= define:MIL_TO_JOIN_RISING, the faction is credited one regiment per define:POP_SIZE_PER_REGIMENT the pop has in size.

## Technology

Daily research points:
Let pop-sum = for each pop type (research-points-from-type x 1^(fraction of population / optimal fraction))
Then, the daily research points earned by a nation is: (national-modifier-research-points-modifier + tech-research-modifier + 1) x (national-modifier-to-research-points) + pop-sum)

If a nation is not currently researching a tech (or is an unciv), research points will be banked, up to a total of 365 x daily research points, for civs, or define:MAX_RESEARCH_POINTS for uncivs.

If a nation is researching a tech, a max of define:MAX_DAILY_RESEARCH points can be dumped into it per day.

There is also an instant research cheat, but I see no reason to put it in the game.

## Politics

### Elections

Elections last define:CAMPAIGN_DURATION months.

To determine the outcome of election, we must do the following:
- Determine the vote in each province. Note that voting is *by active party* not by ideology.
- Provinces in colonial states don't get to vote
- Each pop has their vote multiplied by the national modifier for voting for their strata (this could easily result in a strata having no votes). If the nation has primary culture voting set. Primary culture pops get a full vote, accepted culture pops get a half vote, and other culture pops get no vote. If it has culture voting, primary and accepted culture pops get a full vote and no one else gets a vote. If neither is set, all pops get an equal vote.
- For each party we do the following: figure out the pop's ideological support for the party and its issues based support for the party (by summing up its support for each issue that the party has set, except that pops of non-accepted cultures will never support more restrictive culture voting parties). The pop then votes for the party based on the sum of its issue and ideological support, except that the greater consciousness the pop has, the more its vote is based on ideological support (pops with 0 consciousness vote based on issues alone). I don't know the exact ratio (does anyone care if I don't use the exact ratio?). The support for the party is then multiplied by (provincial-modifier-ruling-party-support + national-modifier-ruling-party-support + 1), if it is the ruling party, and by (1 + province-party-loyalty) for its ideology.
- After the vote has occurred in each province, the winning party there has the province's ideological loyalty for its ideology increased by define:LOYALTY_BOOST_ON_PARTY_WIN x (provincial-boost-strongest-party-modifier + 1) x fraction-of-vote-for-winning-party
- That pop's votes according to what it supports, multiplied by (provincial-modifier-number-of-voters + 1)

## Other concepts

- overseas province: not on same continent as the nation's capital AND not connected by land to the capital
- province value type A: 1 (if unowned or colonial), otherwise: (1 + fort level + naval base level + number of factories in state (if capital province of the state state)) x 2 (if non-overseas core) x 3 (if it is the national capital) -- these values are used to determine the relative worth of provinces (I believe) as a proportion of the total type A valuation of all provinces owned by the country.
- a port: I believe that any coastal province is considered to have a port, regardless of whether it has a naval base
- revanchism: you get one point per unowned core if your primary culture is the dominant culture (culture with the most population) in the province, 0.25 points if it is not the dominant culture, and then that total is divided by the total number of your cores to get your revanchism percentage
- naval base supply score: you get one point per level of naval base that is either in a core province or connected by land to the capital. You get define:NAVAL_BASE_NON_CORE_SUPPLY_SCORE per level of other naval bases
- ships consume naval base supply at their supply_consumption_score
- colonial points: (for nations with rank at least define:COLONIAL_RANK)
- from naval bases: (1) determined by level and the building defintion, except you get only define:COLONIAL_POINTS_FOR_NON_CORE_BASE (a flat rate) for naval bases not in a core province and not connected by land to the capital. (2) multiply that result by define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR
- from units: the colonial points they grant x (1.0 - the fraction the nation's naval supply consumption is over that provided by its naval bases) x define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR
- plus points from technologies/inventions
- national administrative efficiency: = (the-nation's-national-administrative-efficiency-modifier + efficiency-modifier-from-technologies + 1) x number-of-non-colonial-bureaucrat-population / (total-non-colonial-population x (sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE) )
