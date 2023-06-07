# Game rules

## Modifiers

Both nations and provinces have a set of properties (a bag of float values) that may be affected by modifiers. Some modifiers attached to a province may also affect the bag of values associated with the owning nation. In general, as properties are added or removed, we add or remove their affect on the bag of values (we try not to recalculate the whole set as much as possible). This also means when a province changes ownership that we need to remember to update its affect on the national set of modifiers.

### Scaled national modifiers

Some modifiers are scaled by things such as war exhaustion, literacy, etc. Since these need to be updated daily, there is a second bag of values attached to nations just for these

- total occupation: scaled by the fraction of hostile, but not rebel occupied home continent provinces when at war (yeah, not the normal overseas check; home continent only)
- infamy: scaled by infamy
- total blockaded: scaled by the fraction of non-overseas blockaded ports
- war exhaustion: scaled by war exhaustion (as a value from 0 to 100)
- plurality: scaled by plurality (as a value from 0 to 100, so 100% plurality = 100x the modifier values)
- literacy: scaled by average literacy (as a value from 0 to 1)

### Other conditional national modifiers

- national value of the nation
- tech school of the nation
- one of: the great-power static modifier, the secondary-power static modifier, the civilized-nation static modifier, or the uncivilized-nation static modifier
- modifiers from any active party issues, social and political issues (if civilized), reforms (if unciv), technologies (if civ), or inventions
- one of: war or peace
- disarming, if currently disarmed
- debt: The debt default to modifier is triggered by the nation having the generalized debt default and/or bankruptcy as a timed/triggered modifier and unpaid creditors ... I think.
- timed or permanent modifiers applied by event
- triggered modifiers where the trigger condition is met

### Scaled provincial modifiers

- the modifier for each province building, scaled by its level
- the infrastructure modifier, scaled by the infrastructure value for the province (each level of railroad gives a fixed amount of infrastructure)
- the nationalism modifier, scaled by the province's nationalism value, assuming that the owner does not have a core there (starts at define:YEARS_OF_NATIONALISM and ticks down by 1/12 per month)

### Conditional province modifiers

- terrain modifier
- climate modifier
- continent modifier
- a national focus modifier
- crime modifier
- the land_province modifier if it is land, and the sea_zone modifier otherwise (which we probably won't use)
- the coastal modifier if the province is coastal, and the non_coastal modifier otherwise
- there is also a coastal_sea modifier, which I am going to ignore
- the overseas modifier if the province is overseas
- the has_siege modifier if there is an active siege
- the blockaded modifier if the province is blockaded
- the core modifier if the owner of the province has a core there
- the no_adjacent_controlled modifier if not adjacent provinces are controlled (the base game doesn't use this, and I am going to ignore it)
- timed or permanent modifiers applied by event

### Update frequency

Triggered modifiers for nations appear to be updated on the first of each month. The game appears to update the values that result from modifiers being applied or not on a monthly basis (1st of the month) for provinces and nations.

When modifiers are updated, any spending over the maximum/minimums is limited appropriately.

## Economy

### Stockpile automation

The player may automate their stockpile management, and the AI always does (uh, by definition). For each commodity we first figure out the cost required to buy land-units-supply x land-units-spending + naval-units-supply x naval-units-spending + construction-projects x construction-spending worth of the commodity. If the limit for the stockpile is set to less than the amount required to fulfill those purchases, the stockpile is set to buy and the limit is raised to the necessary value. And that is it. So on automation eventually the limits get raised to the greatest value that has ever been seen for national spending.

### Share factor

If the nation is a civ and is a secondary power start with define:SECOND_RANK_BASE_SHARE_FACTOR, and otherwise start with define:CIV_BASE_SHARE_FACTOR. Also calculate the sphere owner's foreign investment in the nation as a fraction of the total foreign investment in the nation (I believe that this is treated as zero if there is no foreign investment at all). The share factor is (1 - base share factor) x sphere owner investment fraction + base share factor. For uncivs, the share factor is simply equal to define:UNCIV_BASE_SHARE_FACTOR (so 1, by default). If a nation isn't in a sphere, we let the share factor be 0 if it needs to be used in any other calculation.

### Artisan production selection

During its monthly pop update tick (see below), if an artisan hasn't been satisfying its life needs, it may switch to producing another, unlocked, commodity (this is more likely the longer the artisan pop has not been satisfying its life needs). It will not switch to a good where the following are true: it is not base profitable (i.e. raw input-prices x input-quantities - output-price x output-quantities is positive, with no modifiers considered), total world real demand is under total world real supply (schombert notes: maybe simpler to just check the direction of the price movement recently), or where there is no available supply of one or more inputs. The choice of what to produce among the commodities not rejected by the above seems to be weighted by the unit profit, the availability of inputs, how much world demand exceeds the supply, and by the presence of a national focus encouraging the output.

### Construction

#### National projects

(Note: this applies to both military unit and building projects, I believe)
- A project under construction demands 1/100th of its total commodity cost per day until enough of each commodity has been consumed.
- A project under construction takes its daily demanded commodities out of the national stockpile (up to availability). It then loses money based on the price of those commodities (although this amount is not allowed to leave it with negative money, so the possibility of free commodities exists), and the nation gains money equal to the amount the project loses.
- Note that much of this is entirely wasted computation if the nation itself has funded the project: it puts money into the project only to pay itself back. In the case of private investment, it would be simpler just to pay the nation directly at the start. In the end, it will all come to the same thing, and it removes a non-trivial amount of bookkeeping.
- The amount demanded by projects is added to the nation's daily demand for good

#### Pop projects

Pop projects are things like factory construction / upgrading / reopening / railroad building
Pop projects are possible only subject to the political rules, and must be under construction long enough to complete.

Factories are given priority to be built on states with high population, whereas railroads are prioritized on states with a lot of factories (which is determined by the sum of all factory levels). This makes it so projects are built on the most important areas and then spread out to less important ones as time goes on.

A pop project buys commodities like a factory would buy its inputs (i.e. it doesn't draw directly from the national stockpile).
Constructing a new building has a cost based on commodities of (technology-factory-owner-cost + 1) x (national-factory-owner-cost-modifier) x (the-price-required-to-purchase-construction-goods) + 25 x cost-to-purchase-one-day-of-inputs-at-level-1
Reopening a building has a cost of : 25 x cost-to-purchase-one-day-of-inputs-at-level-1
Upgrading a building has a cost of: (technology-factory-owner-cost + 1) x (national-factory-owner-cost-modifier) x (the-price-required-to-purchase-construction-goods) + building-cost x 2-to-the-level-minus-2-power

#### Railroads

Railroads increase a province military and economic potential. The maximum level at the start of the game is 0, it can be increased to a maximum of 6 through researches in the infrastructure subcategory of the industrial technologies. It can be built by capitalists and through the foreign investment mechanic.

You can build a railroad in a province if (nation_max_railroad_level - province_terrain_railroad_levels_modifier) > 0 and the cost requirements are satisfied.

You cannot build railroads on occupied territories.

A railway increases:

- The infrastructure modifier
- Troops movement speed (for both enemies and allies)
- RGO output
- Factory throughput
- Supply limit

### Overseas penalty

For each commodity that has been discovered by *someone* and which is marked as being part of the overseas penalty (in `goods.txt`), a nation must pay define:PROVINCE_OVERSEAS_PENALTY x number-of-overseas-provinces per day. The overseas penalty for a nation is then 0.25 x (1 - the average for all discovered commodities of (the-amount-in-stockpile / amount-required)). A nation without overseas provinces has a penalty of 0. (A good is discovered if it is marked as active from the start *or* the nation has unlocked the factory that produces it.)

### Invisible "banking"

Yes, there are flows of currency that are largely invisible in the game. I think of them collectively as banking or finance, but really that is just a nice name.

We associate a "bank" with each nation, and we think of it as having a branch in each state within the nation. This bank has a central reserve of money and can accumulate loan interest (always positive; it never loses money on loans). If the bank has nearly no money, this loan interest is deposited directly into its central reserves. If the central bank's reserves are not empty, then the money is "distributed" to the branches. ("distributed" because, as you will see, that is only a rough analogy for what happens). To distribute it over the states, we keep a running total of the loan interest to subtract from as we visit each state. First we calculate for a state the fraction of the branch's reserve to the central bank's reserve and multiply the total loan interest by that fraction. If that amount is less that what remains in the running total, we give that amount to the branch as its loan interest and subtract it from the running total. If it is greater than what is left in the running total, we add what remains in the running total to the branch's loan interest. We then do not alter the running total and proceed to the next state. As you can tell, this method is going to be very order dependent in terms of what happens, and yet I have no idea if there is any canonical way the states are supposed to be ordered. In any case, after distribution we zero out the loan interest credited to the central bank.

When a pop withdraws money to make a purchase, the amount that can be withdrawn is limited to the least of the pop's savings (obviously), the amount of savings stored in their state branch, and the money in the central bank's reserves - the amount it has lent out. The amount is then subtracted from both the local branch and the central bank, as well as the pop's account. NOTE: a pop's bank account is in addition to any money it has on hand. Pops try to keep about twice their needs spending out of the bank and as cash on hand. However, if a pop is satisfying all of its luxury needs (or nearly so) it will put define:POP_SAVINGS x its cash on hand into the bank. (Adding an identical amount to its state branch and central bank ledgers.)

The banks of great powers start with (8 - rank) x 20 + 100 in them.

Every day, a nation must pay its creditors. It must pay national-modifier-to-loan-interest x debt-amount x interest-to-debt-holder-rate / 30 to the national bank that made the loan (possibly taking out further loans to do so). If a nation cannot pay and the amount it owes is less than define:SMALL_DEBT_LIMIT, the nation it owes money to gets an `on_debtor_default_small` event (with the nation defaulting in the from slot). Otherwise, the event is pulled from `on_debtor_default`. The nation then goes bankrupt. It receives the `bad_debter` modifier for define:BANKRUPCY_EXTERNAL_LOAN_YEARS years (if it goes bankrupt *again* within this period, creditors receive an `on_debtor_default_second` event). It receives the `in_bankrupcy` modifier for define:BANKRUPCY_DURATION days. Its prestige is reduced by a factor of define:BANKRUPCY_FACTOR, and each of its pops has their militancy increase by 2. Any unit or building constructions being undertaken by the state are canceled, as are factories under construction (that is, if the state is allowed to destroy factories, what an odd limitation). All war subsidies are canceled. All the spending sliders are also adjusted and factory subsidies are turned off, but this is probably for the benefit of the AI.

When a nation takes a loan, the interest-to-debt-holder-rate is set at nation-taking-the-loan-technology-loan-interest-modifier + define:LOAN_BASE_INTEREST, with a minimum of 0.01.

### Maximum loan amount

A country cannot borrow if it is less than define:BANKRUPTCY_EXTERNAL_LOAN_YEARS since their last bankruptcy. THere is an income cap to how much may be borrowed, namely: define:MAX_LOAN_CAP_FROM_BANKS x (national-modifier-to-max-loan-amount + 1) x national-tax-base. And there is also a cap from what is available in the central banks of other nations, where for each nation not currently at war with the borrowing nation and which is not an unpaid creditor we add: (national-modifier-to-max-loan-amount + 1) x (money-in-reserves-of-that-central-bank - money-that-central-bank-has-loaned-out) - amount-already-loaned (and this should be calculated even for the nation itself, i.e. borrowing from its own central bank), and then define:SHADOWY_FINANCIERS_MAX_LOAN_AMOUNT. The lower of these two values is the maximum amount a nation can borrow.

### Workforce adjustment

This is a common concept between both factories and RGOs, which both have employment. When we are adjusting the number of workers employed we have both a target number of employees that the adjustment is made towards and a speed factor that determines how fast the adjustment is made. Let us call the change in employment required to reach our target delta. When firing people, we fire up to the greater of 50 or delta x define:EMPLOYMENT_FIRE_LOWEST, and then limit that to at most delta x speed-factor x 0.25. When hiring people, the logic appears to be completely different. People are hired towards the target at the rate of delta x speed-factor, or define:EMPLOYMENT_HIRE_LOWEST x number-currently-employed, or 50, whichever is greater.

### Factory internals

Every day, on a state by state basis, we must adjust the status of each factory, going from highest priority to the lowest (the exact ordering within factories with the same priority appears to be constant but is otherwise opaque -- leftmost in the gui?) we perform its daily update:
- We define the delete-factory-condition as true if the delete-factories-with-no-input rule is set and the nation is player controlled OR if the nation is ai controlled and at peace, and false otherwise
- If the delete-factories-with-no-input rule is in effect, any factory that employs more than 1000 people has its priority set to 0 (and set to 1 if there are fewer than 1000 people employed).
- We must determine if it has made a profit, which depends on whether its last income + its last investment + its last shortfall subsidy is greater than the amount it payed out in wages + its spending on inputs. If it is profitable, it gets marked as having an additional profitable day (I think this record is 8 days long?), and the net profit or loss is recorded
- Factories receiving an injection of cash for N days receive 1/Nth of the amount each day to their local cash reserves. This does not affect whether they are deemed to be profitable.
- Factories that have gone for more than 10 days without input or are closed by the player are considered to be closed.
- Open factories have their employment adjusted. A factory that is being subsidized, or has spending less than its income, or has sufficient cash reserves (100?) is considered to be financially stable. The effective max employment in a factory is its level x its base workforce x the fraction of the state that is not occupied (i.e. fraction-of-state-controlled x level gives us its effective level). If the factory is financially stable we adjust its workforce towards the effective max employment with a speed of 0.15, otherwise we adjust it towards zero with a speed of 0.15.
- Note that factories with higher priority, because they go first, get a first shot at the factory workers to hire/fire, meaning that they end up with more.
- Note also that factories that are currently upgrading ignore days without input / other shutdown rules
- Newly built factories (upgrading from 0) seem to have some special employment logic to get them up to 1000. ... Look I can only figure out so many details.
- Automatic deletion of factories: if the delete-factory-condition is met AND the factory has gone more than 10 days without input AND it wasn't closed by the player AND there are are least define:MIN_NUM_FACTORIES_PER_STATE_BEFORE_DELETING_LASSIEZ_FAIRE factories in the state AND no provinces in the state are occupied AND (there are at least define:NUM_CLOSED_FACTORIES_PER_STATE_LASSIEZ_FAIRE closed factories OR there are 8 factories in state) THEN the factory will be deleted
- Each factory has an input, output, and throughput multipliers.
- These are computed from the employees present. Input and output are 1 + employee effects, throughput starts at 0
- The input multiplier is also multiplied by (1 + sum-of-any-triggered-modifiers-for-the-factory) x 0v(national-mobilization-impact - (overseas-penalty if overseas, or 0 otherwise))
- Owner fraction is calculated from the fraction of owners in the state to total state population in the state (with some cap -- 5%?)
- For each pop type employed, we calculate the ratio of number-of-pop-employed-of-a-type / (base-workforce x level) to the optimal fraction defined for the production type (capping it at 1). That ratio x the-employee-effect-amount is then added into the input/output/throughput modifier for the factory.
- Then, for input/output/throughput we sum up national and provincial modifiers to general factory input/output/throughput are added, plus technology modifiers to its specific output commodity, add one to the sum, and then multiply the input/output/throughput modifier from the workforce by it.
- Factories also contain a purchasing factor that limits how much of their inputs they buy, which is also updated daily
- If factory unsold quantity / factory current production is less than define:FACTORY_PURCHASE_DRAWDOWN_FACTOR, then the factory's purchasing factor is increased by define:FACTORY_PURCHASE_DRAWDOWN_FACTOR
- Otherwise, decrease its purchasing factor by define:FACTORY_PURCHASE_DRAWDOWN_FACTOR to a minimum of define:FACTORY_PURCHASE_MIN_FACTOR


Factory cost modifier: (technology-factory-cost-modifier + 1) x national-factory-cost-modifier
(for RR / forts / naval bases built by the nation: (technology-factory-cost-modifier + 1) x (2 - national-administrative-efficiency))
(for RR build by pops: national-factory-cost-modifier)

### Artisans internals

- Each artisan pop has its own rate of production speed that can increase or decrease based on the status of the artisan pop.
- If the artisan pop is getting its everyday needs at more than 0.85 or all of its everyday needs are deemed to be affordable: the production rate is multiplied by 1.15 up to a maximum of 1
- If the artisan pop is getting its everyday needs at less than 0.85 and either all of its life needs are affordable or its life needs are satisfied by at least 0.9: the production rate is not adjusted
- If the artisan pop is getting its everyday needs at less than 0.85, is life needs are not all affordable, and its life need satisfaction is less than 0.9: the production rate is multiplied by 0.85 down to a minimum of 0.15
- Like factories, artisan spending is also governed partly by a purchasing limit. This limit can increases by 1.05 times per day limited to between half their last income and total last income (I believe that this income does not take into account their expenses, however ...).

### RGO internals

Each RGO has an effective size = base size x (technology-bonus-to-specific-rgo-good-size + technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1).
Like a factory, the rgo employs pops. If the province is currently occupied, its workers are adjusted towards 0, just as a factory makes such an adjustment. If the province is newly restored to owner control, there seems to be some special logic pushing employment up (something like: the workforce increase speed is 25x faster).
If the supply to real demand ratio for the commodity produced by the RGO is greater than 1.05: the workforce is adjusted towards zero at speed (supply-to-demand-ratio - 1) x define:RGO_SUPPLY_DEMAND_FACTOR_FIRE
If the supply to real demand ratio is less than 1.1 but greater than 1.0: the workforce remains the same
If the supply to real demand ratio is less than 1.0 but greater than 0.9: the workforce is adjusted towards full employment at speed (1 - supply-to-demand-ratio) x define:RGO_SUPPLY_DEMAND_FACTOR_HIRE_LO
If the supply to real demand ratio is less than 0.9: the workforce is adjusted towards full employment at speed (1 - supply-to-demand-ratio) x define:RGO_SUPPLY_DEMAND_FACTOR_HIRE_HI
The money RGO always targets full employment

### Prices, production, buying, and selling

The material in this section is often what people think of as the Victoria 2 economy, so brace yourself for a somewhat complicated explanation.

At the beginning of the update we calculate from values generated by the previous day: (change-in-money-in-circulation + money-value-produced) / (money-in-circulation + money-value-produced) + ~0.8 (approx-a-little-less-than-this). And then this value is itself set to 1.0 if it is out of the 0.0 to 1.0 range. Let us call this the circulation-ratio.

1. Update prices based on values from the previous day (everything below refers to global demand and supply):
- price of money remains the price defined by the commodities file
- for each other commodity, check its real demand (see below for what real demand is), and if its real demand is positive, check its supply. If its supply is also positive, we then take the square root of real-demand / supply and multiply that result by the base price of the commodity. Let us call this the ratio-adjusted-price.
- If the ratio-adjusted-price is less than the current price - 0.01, then the price decreases by 0.01.
- If the ratio-adjusted-price is greater than the current price + 0.01, then the price increases by 0.01.
- Prices are then limited to being within 1/5th to 5x the base price of the commodity.

2. Unadjusted supply and demand:
While most of what goes on below is expressed directly in terms of adding to domestic supply or demand, unless noted otherwise, it should be taken as given that the same is added to world supply or demand. Also, world supply and demand for all commodities should be treated as at least 1.

A.

- For each commodity:
- If the national stockpile is *not* buying (i.e. it is selling)
- - If the nation is *not* set to buy from its own stockpile
- - Add the amount in the stockpile over the limit to the domestic supply, reduce the national stockpile to the limit value
- - If the nation *is* set to buy from its own stockpile, then do not adjust it at all at this point
- If the national stockpile *is* buying, add the difference between the actual stockpile and the limit to domestic demand
- Add the national stockpiles to domestic supply if the nation is set to buy from its own stockpiles.

B.

- For each factory:
- Only non-closed (see above) factories that are at least level 1 (i.e. not doing their initial construction) produce/consume
- Each factory produces by consuming inputs from its internal stockpile
- The target input consumption scale is: input-multiplier x throughput-multiplier x factory level
- The actual consumption scale is limited by the input commodities sitting in the stockpile (i.e. input-consumption-scale x input-quantity must be less than the amount in the stockpile)
- A similar process is done for efficiency inputs, except the consumption of efficiency inputs is (national-factory-maintenance-modifier + 1) x input-multiplier x throughput-multiplier x factory level
- Finally, we get the efficiency-adjusted consumption scale by multiplying the base consumption scale by (0.75 + 0.25 x the efficiency consumption scale)
- (A factory with no efficiency goods performs as if all its efficiency goods are fully consumable)
- A factory *produces* efficiency-adjusted-consumption-scale x output-multiplier x throughput-multiplier x level x production quantity amount of its output good, which gets added to domestic supply
- Using the same consumption calculation, the factory adds the difference between what it has left in its stockpile to what it would require to produce at its maximum consumption scale to domestic demand.

C.

- For each expansion or construction project:
- Multiply the cost modifier (see above) by the cost in commodities for the project. Then subtract the project's stockpile from what is required. Figure out the maximum amount the project can purchase based on its money. Then, add those commodities to domestic demand.

D.

- For each artisan pop:
- We calculate pop-size x artisan-production-rate (see above) / artisan-production-workforce. Then we do the production step essentially as we do for factories, except that we use the preceding value as the "level" of the factory.

E.

- For each RGO:
- We calculate its effective size which is its base size x (technology-bonus-to-specific-rgo-good-size + technology-general-farm-or-mine-size-bonus + provincial-mine-or-farm-size-modifier + 1)
- We add its production to domestic supply, calculating that amount basically in the same way we do for factories, by computing RGO-throughput x RGO-output x RGO-size x base-commodity-production-quantity, except that it is affected by different modifiers.

F.

- For each pop
- Each pop strata and needs type has its own demand modifier, calculated as follows:
- (national-modifier-to-goods-demand + define:BASE_GOODS_DEMAND) x (national-modifier-to-specific-strata-and-needs-type + 1) x (define:INVENTION_IMPACT_ON_DEMAND x number-of-unlocked-inventions + 1, but for non-life-needs only)
- Each needs demand is also multiplied by  2 - the nation's administrative efficiency if the pop has education / admin / military income for that need category
- We calculate an adjusted pop-size as (0.5 + pop-consciousness / define:PDEF_BASE_CON) x (for non-colonial pops: 1 + national-plurality (as a fraction of 100)) x pop-size
- Each of the pop's needs for life/everyday/luxury are multiplied by the appropriate modifier and added to domestic demand

3. Adjusting sphere supply / demand
- Each sphere member has its domestic x its-share-factor (see above) of its base supply and demand added to its sphere leader's domestic supply and demand (this does not affect global supply and demand)
- Every nation in a sphere (after the above has been calculated for the entire sphere) has their effective domestic supply set to (1 - its-share-factor) x original-domestic-supply + sphere-leader's-domestic supply

4. Purchasing
Common purchasing concepts:
- Whenever purchases are made, the purchaser tries first to purchase as much as possible from domestic supply, and then tries to buy any remainder from the global supply (where does global supply come from? keep reading)
- If you take the the available amount that could be purchased assuming unlimited supply, given the amount of money that the purchaser is spending, you have a quantity of commodities that we then add to the *real demand* for those commodities.
- What is actually purchased is limited to first availability in domestic supply, and then availability in global supply, except that when purchasing from global supply, prices are multiplied by (the nation's current tariff setting x its tariff efficiency x its blockaded fraction + 1)
- Money spent on purchasing from the global supply will eventually become the basis for tariff income. (So blockades make things cost more, but they also potentially increase your income ...)
- Note that money used to make purchases does not "go" anywhere, it just vanishes. See Selling, below, for how producers earn money

We do the below for each nation in order of their rank (rank 1 goes first)

For each open factory:
- Calculate the total inputs that would be required for full production given the current workforce, modifiers, etc *in addition* to what is left in its stockpile
- Then reduce these inputs by multiplying them by the factory's purchasing factor
- These commodities are what it will try to purchase.
- A factory is allowed to spend up to (current-cash-reserves ^ last spending x 1.05) v 0.1 x current-cash-reserves
- If its spending limit is less than the base cost if its inputs, the factory's purchases are thus further proportionally limited
- If the factory can't purchase anything, once the prices have been computed, and it isn't being subsidized, then it would be marked as having gone a day without inputs, which will eventually result in it being closed.
- Purchased commodities go into the factory stockpile
For each construction project:
- The project purchases as much as it can based on its current cash reserves
- Commodities go into its internal stockpile
For the national stockpile:
- Commodities where the stockpile is set to buy and there is not enough in the stockpile function like normal purchases, except that the country automatically borrows up to its max loan limit to do the purchasing. There is one major caveat, however: nations don't appear to pay any extra for commodities from the global supply (not even from blockade effects), and naturally their stockpile purchases aren't added to tariff sums.
For pops:
- Pops buy their needs in order of their strata (rich first), and then within that ... I don't know the details.
- Artisans buy inputs (they withdraw from their bank to make purchases in preference to their money on hand)
- Pops buy needs first from their money on hand and then by withdrawing from the bank if necessary.
- Pops of a given type seem to buy their needs more or less "together" (i.e. if needs satisfaction is limited by commodity availability and not price, the pops seem to get more or less an even share of it)
- Pops get define:LUXURY_CON_CHANGE whenever they spend enough on luxury goods.

Note that a nation that is in the sphere or another can purchase from any unsold commodities in its sphere leader's domestic supply after it exhausts its own domestic supply (and no tariffs etc are applied here).

Once all purchasing has been completed, commodities that were moved into domestic supply to be sold as part of the buy-from-the-nation's-stockpile setting are moved back into national stockpiles if they were not in fact purchased.

The simplest case are nations neither in a sphere nor part of a sphere:
If the nation has buying from the stockpile enabled, any "unsold" commodities that were moved from the stockpile to domestic supply are moved back into the stockpile. In other words, buying from the stockpile acts like another source of supply that commodities are purchased from after normal domestic supply is exhausted.

The problem, of course, is that the domestic supply from sphere members gets mixed together. How can you keep track of how much of what has been dumped on the market for selling has actually been sold? Here is what probably should happen: we would figure out the percentage of stockpile commodities that were placed into domestic supply that were sold, and put back into each sphere member (and the sphere leader) an amount proportional to what it put in. However, something seems to go wrong in the actual game here, and it looks like some or all of these commodities are being drained out of the sphere leader's stockpiles (??). More investigation may be required here to determine the exact nature of this bug.

Any remaining, unconsumed domestic supply is then added to global supply.

5. Income

Common income concepts:
Reminder, the effective domestic supply is defined above in step 3.
Consumed domestic supply: even when in a sphere, a nation is treated as having bought its domestic production first
When pops are paid: when a pop is paid we must calculate the nation's tax efficiency, which is: (nation's-modifier-to-tax-efficiency + tax-efficiency-from-technology + define:BASE_COUNTRY_TAX_EFFICIENCY). The amount of pop income x tax-efficiency value add up to the unmodified tax base of the nation. Multiplying this by the tax setting for the strata determines the amount of tax the nation collects.
Slave type pops are not counted as workers when paying pops.

For factories:
For each factory, we calculate the share of the effective domestic supply that its production contributed to.
We also calculate the share of the previous day's global supply pool that its production-not-purchased-domestically from the previous day contributed to (obviously that value must then be calculated for the upcoming day, etc). The factory is then given income equal to the quantity of its production that was sold domestically as well as for any production that was sold from the global supply pool on the previous day (as if all purchases were distributed proportionally to the amount of production). This value becomes the factory's income (see the details of how factories work for why this may matter), and we can compare it to the spending from the day to see whether the factory was profitable.

Each factory has base expenses equal to the money required to pay for its inputs the next day plus 1/5th of the life needs costs of its employees. If it is subsidized and has less than that amount of money, the owning nation pays it the difference in subsidies.

If there is enough money in the cash reserves to cover at least one week of input, the factory pays out a bonus. The bonus is (factory-profit = income - input-costs) x define:FACTORY_PAYCHECKS_LEFTOVER_FACTOR x factory-cash-reserves / (define:MAX_FACTORY_MONEY_SAVE x factory-level). If this bonus is greater than the minimum wage value (life-needs-costs-of-its-employees x national-minimum-wage-modifier x national-administrative-efficiency), then the owners, if any, are paid the amount of the bonus in excess of the minimum wage, and the workers get the minimum wage. Otherwise, the owners get half the bonus and the workers get the other half.

Stockpile sales:
Each nation earns money equal to the quantity of commodities sold from the stockpile times the price of the commodity (this applies to both the simple "dumping on the market" sales and allowing entities within the nation to purchase form the stockpile).

Direct gold income:
A nations earns define:GOLD_TO_CASH_RATE x the amount of gold produced from each money RGO

Reparations:
A nation paying reparations loses define:REPARATIONS_TAX_HIT x its-tax-base in money per day, while the country receiving them gets that much money.

War subsidies:
Yep, you can get money from war subsidies.

Tariffs:
Each nation earns (or loses, for subsidies) money equal to its-tariff-setting x tariff-efficiency x total-amount-of-global-supply-purchases (see above). Given how these values are calculated, this usually means money is appearing from nothing or vanishing into nothing.

RGOs:
The money RGO computes its income as if all of its production sold at the current price (which is fixed for "money/gold") and then is further multiplied by define:GOLD_TO_WORKER_PAY_RATE. Otherwise, RGO income is calculated like factory income. Since RGOs don't have to keep a reserve, I believe that they pay out wages by first paying out up to the minimum wage value to the normal workers, and then pay the remainder to the aristocrats (if any) (someone should check this).

Artisans:
Profit is calculated as per a factory. Any profit (i.e. income over that required to purchase inputs) goes into the pockets of the artisan, meaning that it is taxed as normal pop income.

Other pop income:
Pops may receive income directly from the the nation depending on their income type/national policies. For each needs category, the payout is based on how much it would cost to fully satisfy those needs and the payout type. Payouts of this sort do seem to be taxed, confusingly enough.

Pops that don't have their needs categories covered by direct state spending appear to get: social-spending x life-needs-cost x pension-level x administrative-efficiency + social-spending x life-needs-cost x unemployment-fraction x administrative-efficiency per day.

And for each need category where the pop is payed by education/administration/military:
The nation pays the pop nations-administrative/education/military-spending x (2 - administrative-efficiency) x needs-cost

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

#### Destinations

Country targets for external migration: must be a country with its capital on a different continent from the source country *or* an adjacent country (same continent, but non adjacent, countries are not targets). Each country target is then weighted: First, the product of the country migration target modifiers (including the base value) is computed, and any results less than 0.01 are increased to that value. That value is then multiplied by (1.0 + the nations immigrant attractiveness modifier). Assuming that there are valid targets for immigration, the nations with the top three values are selected as the possible targets. The pop (or, rather, the part of the pop that is migrating) then goes to one of those three targets, selected at random according to their relative attractiveness weight. The final provincial destination for the pop is then selected as if doing normal internal migration.

Destination for internal migration: colonial provinces are not valid targets, nor are non state capital provinces for pop types restricted to capitals. Valid provinces are weighted according to the product of the factors, times the value of the immigration focus + 1.0 if it is present, times the provinces immigration attractiveness modifier + 1.0. The pop is then distributed more or less evenly over those provinces with positive attractiveness in proportion to their attractiveness, or dumped somewhere at random if no provinces are attractive.

Colonial migration appears to work much the same way as internal migration, except that *only* colonial provinces are valid targets, and pops belonging to cultures with "overseas" = false set will not colonially migrate outside the same continent. The same trigger seems to be used as internal migration for weighting the colonial provinces.

#### Amounts

For non-slave, non-colonial pops in provinces with a total population > 100, some pops may migrate within the nation. This is done by calculating the migration chance factor *additively*. If it is non negative, pops may migrate, and we multiply it by (province-immigrant-push-modifier + 1) x define:IMMIGRATION_SCALE x pop-size to find out how many migrate.

If a nation has colonies, non-factory worker, non-rich pops in provinces with a total population > 100 may move to the colonies. This is done by calculating the colonial migration chance factor *additively*. If it is non negative, pops may migrate, and we multiply it by (province-immigrant-push-modifier + 1) x (colonial-migration-from-tech + 1) x define:IMMIGRATION_SCALE x pop-size to find out how many migrate.

Finally, pops in a civ nation that are not in a colony any which do not belong to an `overseas` culture group in provinces with a total population > 100 may emigrate. This is done by calculating the emigration migration chance factor *additively*. If it is non negative, pops may migrate, and we multiply it by (province-immigrant-push-modifier + 1) x 1v(province-immigrant-push-modifier + 1) x define:IMMIGRATION_SCALE x pop-size to find out how many migrate.

### Promotion/demotion

Pops appear to "promote" into other pops of the same or greater strata. Likewise they "demote" into pops of the same or lesser strata. (Thus both promotion and demotion can move pops within the same strata?). The promotion and demotion factors seem to be computed additively (by taking the sum of all true conditions). If there is a national focus set towards a pop type in the state, that is also added into the chances to promote into that type. If the net weight for the boosted pop type is > 0, that pop type always seems to be selected as the promotion type. Otherwise, the type is chosen at random, proportionally to the weights. If promotion to farmer is selected for a mine province, or vice versa, it is treated as selecting laborer instead (or vice versa). This obviously has the effect of making those pop types even more likely than they otherwise would be.

There are known bugs in terms of promotion not respecting culture and/or religion. I don't see a reason to try to replicate them.

As for demotion, there appear to an extra wrinkle. Pops do not appear to demote into pop types if there is more unemployment in that demotion target than in their current pop type. Otherwise, the national focus appears to have the same effect with respect to demotion.

Pops that move / promote seem to take some money with them, but there is also magical money generated by starter share, at least for capitalists, probably so that it is possible for the first capitalists in a state with no factories to build a factory. I propose the following instead: capitalists building a factory in a province with no open factories (or reopening a closed factory in those conditions) is simply free.

Promotion amount:
Compute the promotion modifier *additively*. If it it non-positive, there is no promotion for the day. Otherwise, if there is a national focus to to a pop type present in the state and the pop in question could possibly promote into that type, add the national focus effect to the promotion modifier. Conversely, pops of the focused type are not allowed to promote out. Then multiply this value by national-administrative-efficiency x define:PROMOTION_SCALE x pop-size to find out how many promote (although at least one person will promote per day if the result is positive).

Demotion amount:
Compute the demotion modifier *additively*. If it it non-positive, there is no demotion for the day. Otherwise, if there is a national focus to to a pop type present in the state and the pop in question could possibly demote into that type, add the national focus effect to the demotion modifier. Then multiply this value by define:PROMOTION_SCALE x pop-size to find out how many demote (although at least one person will demote per day if the result is positive).

### Militancy

Let us define the local pop militancy modifier as the province's militancy modifier + the nation's militancy modifier + the nation's core pop militancy modifier (for non-colonial states, not just core provinces).
Each pop has its militancy adjusted by the local-militancy-modifier + (technology-separatism-modifier + 1) x define:MIL_NON_ACCEPTED (if the pop is not of a primary or accepted culture) - (pop-life-needs-satisfaction - 0.5) x define:MIL_NO_LIFE_NEED - (pop-everyday-needs-satisfaction - 0.5)^0 x define:MIL_LACK_EVERYDAY_NEED + (pop-everyday-needs-satisfaction - 0.5)v0 x define:MIL_HAS_EVERYDAY_NEED + (pop-luxury-needs-satisfaction - 0.5)v0 x define:MIL_HAS_LUXURY_NEED + pops-support-for-conservatism x define:MIL_IDEOLOGY / 100 + pops-support-for-the-ruling-party-ideology x define:MIL_RULING_PARTY / 100 - (if the pop has an attached regiment, applied at most once) leader-reliability-trait / 1000 + define:MIL_WAR_EXHAUSTION x national-war-exhaustion x (sum of support-for-each-issue x issues-war-exhaustion-effect) / 100.0 + (for pops not in colonies) pops-social-issue-support x define:MIL_REQUIRE_REFORM + pops-political-issue-support x define:MIL_REQUIRE_REFORM

Militancy is updated monthly

NOTE FOR FUTURE SELF: the default war-exhaustion effect of any issue is 1, not 0.

### Consciousness

Pops in provinces controlled by rebels are not subject to consciousness changes.

Otherwise, the daily change in consciousness is:
(pop-luxury-needs-satisfaction x define:CON_LUXURY_GOODS
+ define:CON_POOR_CLERGY or define:CON_MIDRICH_CLERGY x clergy-fraction-in-province
+ national-plurality x 0v((national-literacy-consciousness-impact-modifier + 1) x define:CON_LITERACY x pop-literacy)
) x define:CON_COLONIAL_FACTOR if colonial
+ province-pop-consciousness-modifier + national-pop-consciousness-modifier + national-core-pop-consciousness-modifier (in non-colonial states) + national-non-accepted-pop-consciousness-modifier (if not a primary or accepted culture)

Consciousness is updated monthly

### Literacy

The literacy of each pop changes by: 0.01 x define:LITERACY_CHANGE_SPEED x education-spending x ((total-province-clergy-population / total-province-population - define:BASE_CLERGY_FOR_LITERACY) / (define:MAX_CLERGY_FOR_LITERACY - define:BASE_CLERGY_FOR_LITERACY))^1 x (national-modifier-to-education-efficiency + 1.0) x (tech-education-efficiency + 1.0). Literacy cannot drop below 0.01.

Literacy is updated monthly

### Ideology

For ideologies after their enable date (actual discovery / activation is irrelevant), and not restricted to civs only for pops in an unciv, the attraction modifier is computed *multiplicatively*. Then, these values are collectively normalized. If the normalized value is greater than twice the pop's current support for the ideology: add 0.25 to the pop's current support for the ideology
If the normalized value is greater than the pop's current support for the ideology: add 0.05 to the pop's current support for the ideology
If the normalized value is greater than half the pop's current support for the ideology: do nothing
Otherwise: subtract 0.25 from the pop's current support for the ideology (to a minimum of zero)
The ideological support of the pop is then normalized after the changes.

### Issues

As with ideologies, the attraction modifier for each issue is computed *multiplicatively* and then are collectively normalized. Then we zero the attraction for any issue that is not currently possible (i.e. its trigger condition is not met or it is not the next/previous step for a next-step type issue, and for uncivs only the party issues are valid here)

Then, like with ideologies, we check how much the normalized attraction is above and below the current support, with a couple of differences. First, for political or social issues, we multiply the magnitude of the adjustment by (national-political-reform-desire-modifier + 1) or (national-social-reform-desire-modifier + 1) as appropriate. Secondly, the base magnitude of the change is either (national-issue-change-speed-modifier + 1.0) x 0.25 or (national-issue-change-speed-modifier + 1.0) x 0.05 (instead of a fixed 0.05 or 0.25). Finally, there is an additional "bin" at 5x more or less where the adjustment is a flat 1.0.

## Military

### Spending

Nations consume (or try to consume) commodities for their land and naval units daily at a rate proportional to their land/naval spending (at full spending they try to consume 100% of the necessary commodities). These commodities are subtracted directly from a nation's stockpiles (and there are no negative stockpiles allowed).

### CB fabrication

CB fabrication by a nation is paused while that nation is in a crisis (nor do events related to CB fabrication happen). CBs that become invalid (the nations involved no longer satisfy the conditions or enter into a war with each other) are canceled (and the player should be notified in this event). Otherwise, CB fabrication is advanced by points equal to:
define:CB_GENERATION_BASE_SPEED x cb-type-construction-speed x (national-cb-construction-speed-modifiers + technology-cb-construction-speed-modifier + 1). This value is further multiplied by (define:CB_GENERATION_SPEED_BONUS_ON_COLONY_COMPETITION + 1) if the fabricating country and the target country are competing colonially for the same state, and then further by (define:CB_GENERATION_SPEED_BONUS_ON_COLONY_COMPETITION_TROOPS_PRESENCE + 1) if either country has units in that colonial state. (schombert: I'm really not convinced that these effects are worth the trouble it would take to implement). When the fabrication reaches 100 points, it is complete and the nation gets the CB.

Each day, a fabricating CB has a define:CB_DETECTION_CHANCE_BASE out of 1000 chance to be detected. If discovered, the fabricating country gains the infamy for that war goal (sum the po costs) x the fraction of fabrication remaining. If discovered relations between the two nations are changed by define:ON_CB_DETECTED_RELATION_CHANGE. If discovered, any states with a flashpoint in the target nation will have their tension increase by define:TENSION_ON_CB_DISCOVERED

When fabrication progress reaches 100, the CB will remain valid for define:CREATED_CB_VALID_TIME months (so x30 days for us). Note that pending CBs have their target nation fixed, but all other parameters are flexible.

### Other CBs

Beyond fabricating a CB, you can get one from any CB with "always = yes" if the "is_valid" trigger is satisfied (with the potential target country in scope and the country that would be granted the CB in "this"). I believe that this is always tested daily. At least for the player, some record of which CBs were available in the previous day must be maintained in order to give messages about CB gained/lost for these triggered CBs

### Leaders (Generals and Admirals)

#### Recruitment

Getting a new leader costs define:LEADER_RECRUIT_COST leadership points. If automatic leader recruitment is turned on (and why would you ever turn it off?), then if you have one or more armies/navies that could be given a leader (is not in a currently hostile province, is not at sea, and is not currently retreating) and which does not currently have one, and if you have enough leadership points, new leaders will automatically be recruited. The game seems to try to balance how many general and admirals are recruited by how many you need of each type ... I'm not sure that is worth respecting; I would probably try to fill up the armies first.

New leaders get a random name based on the primary culture of their nation. Leaders also get a random personality and background trait. This seems entirely random *except*--and it gets weird here--admirals are biased away from having personalities and backgrounds that modify reliability, either positively or negatively.

Nations appear to be able to store up to define:LEADER_RECRUIT_COST x 3 leadership points. Nations accumulate leadership points monthly. To calculate the amount, we first take for each pop that provides leadership points, and multiply the amount of points it gives by the ratio of the fraction of the national population composed of that pop type to its research optimum (capping this ratio at 1). We then sum up those values and add the nation's modifier for leadership. Finally, we multiply that sum by (national-leadership-modifier-modifier + 1), giving up the monthly leadership points increase.

A nation gets ((number-of-officers / total-population) / officer-optimum)^1 x officer-leadership-amount + national-modifier-to-leadership x (national-modifier-to-leadership-modifier + 1) leadership points per month.

#### Daily death chances

Leaders who are both less than 26 years old and not in combat have no chance of death. Otherwise, we take the age of the leader and divide by define:LEADER_AGE_DEATH_FACTOR. Then we multiply that result by 2 if the leader is currently in combat. That is then the leader's current chance of death out of ... my notes say 11,000 here. Note that the player only gets leader death messages if the leader is currently assigned to an army or navy (assuming the message setting for it is turned on).

## Wars

- Each war has two sides, and each side has a primary belligerent
- Primary belligerents negotiate in peace deals for their whole side
- Secondary belligerents may in non crisis wars negotiate independently with the primary belligerent on the opposite side
- Wars must be kept in a valid state (see below), generally this is done by removing the cause of the invalid state. Any further problems caused by such a removal may not be caught until the next check of war validity
- A war that ends up with no members on one (or both) sides will simply be ended as a whole
- A province is blockaded if there is a hostile sea unit in an adjacent sea province and no ongoing naval combat there. My notes say that only either not-overseas (or maybe only connected to capital) provinces count for calculating the blockade fraction

### War and peace

#### Breaking a truce

If you declare a war while you have a truce. You get infamy equal to the sum of the break-truce infamy associated with each of the po_tags for the war goal you are declaring with, times the break_truce_infamy_factor for that specific cb. In the same way, you lose the sum of the break-truce prestige associated with each of the po_tags times the cb's break_truce_prestige_factor. And the same thing is done but with break_truce_militancy_factor to determine how much to increase the militancy of each pop in your nation by.

#### Declaring a war

All defender allies that could possibly join get a call to arms. The attacker may optionally call to arms their allies.

#### Joining a war

When you join (or declare) a war your relations with nations on the opposite side decrease by -100. If you end up on the opposite side of a war as an ally, your alliance breaks, but without any extra relations penalty. Substates and vassals are automatically dragged into the war.

#### Calling allies

Allies may be called into a war. In a defensive war, the sphere leader of the target will be called in. Any ally called must be able to join the war:

Standard war-joining conditions: can't join if you are already at war against any attacker or defender. Can't join a civil war. Can't join a war against your sphere leader or overlord . Can't join a crisis war prior to great wars being invented (i.e. you have to be in the crisis). Can't join as an attacker against someone you have a truce with.

There are somewhat opaque rules about when an ally called into the war will take over war leadership. We may choose either to keep war leadership fixed or to let the highest ranked participant be the leader.

#### Naming the war

The name of the war is determined by the `war_name` property of the CB that the war was declared with prefixed with either NORMAL_ or AGRESSION_ (yes, with that spelling. yes, I know). (It gets the aggression prefix if the attacker has no valid cb as the war is declared ... which shouldn't be possible right, since the war name comes from the CB that the war is declared with, right ...). If there is no declaring CB, the war defaults to NORMAL_WAR_NAME or AGRESSION_WAR_NAME.

A great war is named GREAT_WAR_NAME

The game also apparently supports special premade names between specific nations (or rather, specific tags). I don't intend to support this.

In addition to first and second, country_adj and state are also available in these strings.

#### Special Crisis rules

A pending crisis acts much like an active war in that war goals can be added and peace offers made. When adding a war goal to the crisis as an offer to get someone to join, multiply the normal infamy cost by define:CRISIS_WARGOAL_INFAMY_MULT.

#### Peace action

Costs define:PEACE_DIPLOMATIC_COST (although you seem to be able to clear a war with no war goals for free). Peace requires a war leader on at least one side of the offer, except in crisis wars, in which no separate peaces are possible.

When a peace offer is accepted, relations between the nations increase by defines:PEACE_RELATION_ON_ACCEPT, If it is refused, relations increase by define:PEACE_RELATION_ON_DECLINE.

If a "good" peace offer is refused, the refusing nation gains define:GOOD_PEACE_REFUSAL_WAREXH war exhaustion and all of its pops gain define:GOOD_PEACE_REFUSAL_MILITANCY. What counts as a good offer, well if the peace offer is considered "better" than expected. This seems to be a complicated thing to calculate involving: the direction the war is going in (sign of the latest war score change), the overall quantity of forces on each side (with navies counting for less), time since the war began, war exhaustion, war score, the peace cost of the offer, and whether the recipient will be annexed as a result.

A peace offer must be accepted when war score reaches 100.

When a losing peace offer is accepted, the ruling party in the losing nation has its party loyalty reduced by define:PARTY_LOYALTY_HIT_ON_WAR_LOSS percent in all provinces (this includes accepting a crisis resolution offer in which you lose). When a nation exits a war, it takes all of its vassals / substates with it. The nations on the other side of the war get a truce with the nation for define:BASE_TRUCE_MONTHS + the greatest truce months for any CB in the peach deal.

When a war goal is failed (the nation it targets leaves the war without that war goal being enacted): the nation that added it loses WAR_FAILED_GOAL_PRESTIGE_BASE^(WAR_FAILED_GOAL_PRESTIGE x current-prestige) x CB-penalty-factor prestige. Every pop in that nation gains CB-penalty-factor x define:WAR_FAILED_GOAL_MILITANCY militancy.

War goal results:
po_annex: nation is annexed, vassals and substates are freed, diplomatic relations are dissolved.
po_demand_state: state is taken (this can turn into annex if it is the last state)
po_remove_cores: also cores are removed from any territory taken / target territory if it is already owned by sender
po_transfer_provinces: all the valid states are transferred to the nation specified in the war goal. Relations between that country and the nation that added the war goal increase by define:LIBERATE_STATE_RELATION_INCREASE. If the nation is newly created by this, the nation it was created from gets a truce of define:BASE_TRUCE_MONTHS months with it (and it is placed in the liberator's sphere if that nation is a GP).
po_add_to_sphere: leaves its current sphere and has its opinion of that nation set to hostile. Is added to the nation that added the war goal's sphere with max influence.
po_clear_union_sphere: every nation of the actors culture group in the target nation's sphere leaves (and has relation set to friendly) and is added to the actor nation's sphere with max influence. All vassals of the target nation affected by this are freed.
po_release_puppet: nation stops being a vassal
po_make_puppet: the target nation releases all of its vassals and then becomes a vassal of the acting nation.
po_destory_forts: reduces fort levels to zero in any targeted states
po_destory_naval_bases: as above
po_disarmament: a random define:DISARMAMENT_ARMY_HIT fraction of the nations units are destroyed. All current unit constructions are canceled. The nation is disarmed. Disarmament lasts until define:REPARATIONS_YEARS or the nation is at war again.
po_reparations: the nation is set to pay reparations for define:REPARATIONS_YEARS
po_remove_prestige: the target loses (current-prestige x define:PRESTIGE_REDUCTION) + define:PRESTIGE_REDUCTION_BASE prestige
po_install_communist_gov: The target switches its government type and ruling ideology (if possible) to that of the nation that added the war goal. Relations with the nation that added the war goal are set to 0. The nation leaves its current sphere and enters the actor's sphere if it is a GP. If the war continues, the war leader on the opposite side gains the appropriate `counter_wargoal_on_install_communist_gov` CB, if any and allowed by the conditions of that CB.
po_uninstall_communist_gov_type: The target switches its government type to that of the nation that added the war goal. The nation leaves its current sphere and enters the actor's sphere if it is a GP.
po_colony: colonization finishes, with the adder of the war goal getting the colony and all other colonizers being kicked out
other/in general: the `on_po_accepted` member of the CB is run. Primary slot: target of the war goal. This slot: nation that added the war goal.
The nation that added the war goal gains prestige. This is done, by calculating the sum ,over all the po tags, of base-prestige-for-that-tag v (nations-current-prestige x prestige-for-that-tag) and then multiplying the result by the CB's prestige factor. The nation that was targeted by the war goal also loses that much prestige.

When a nation is annexed by a war goal, the war leader on that side gains a `counter_wargoal_on_annex` CB if possible
(NOTE: these CBs are actually resolved through the tweaks.lua file, which contains:
```
counter_wargoal_on_annex = { cb = "free_peoples" },
counter_wargoal_on_install_communist_gov = { cb = "uninstall_communist_gov_cb" },
```
)
A war goal added in this way will result in a status quo war goal on that side being removed.

Any war ending probably requires checking that units aren't stuck in invalid combats, updating blockade status, etc.

#### Crisis offers

Crisis resolution offers function much in the same way as peace offers. Every refused crisis offer increases the temperature of the current crisis by define:CRISIS_TEMPERATURE_ON_OFFER_DECLINE.

When a war goal is failed (i.e. the crisis ends in peace without the goal being pressed): the nation that added it loses WAR_FAILED_GOAL_PRESTIGE_BASE^(WAR_FAILED_GOAL_PRESTIGE x current-prestige) x CB-penalty-factor prestige x define:CRISIS_WARGOAL_PRESTIGE_MULT. Every pop in that nation gains CB-penalty-factor x define:WAR_FAILED_GOAL_MILITANCY militancy. Every pop in that nation gains CB-penalty-factor x define:WAR_FAILED_GOAL_MILITANCY x define:CRISIS_WARGOAL_MILITANCY_MULT militancy.

For war goals that are enforced, the winners get the normal amount of prestige x define:CRISIS_WARGOAL_PRESTIGE_MULT

The crisis winner also gets year-after-start-date x define:CRISIS_WINNER_PRESTIGE_FACTOR_YEAR prestige. The crisis winner gains define:CRISIS_WINNER_RELATIONS_IMPACT relations with everyone on their side, except everyone with an unfulfilled war goal gains -define:CRISIS_WINNER_RELATIONS_IMPACT relations with the leader of their side.

### War goals and war score

#### Adding a war goal

When you add a war goal (including the one that the war is declared with) it is removed from your constructed CBs list. Once the war is going: no war goals may be added if there is a status quo war goal present on the side of the conflict of the nation that wants to add a war goal. The war goal must be valid (see below). The nation adding the war goal must have positive war score against the target of the war goal (see below). And the nation must be already able to use the CB in question (e.g. it as fabricated previously) or it must be a constructible CB and the nation adding the war goal must have overall jingoism support >= defines:WARGOAL_JINGOISM_REQUIREMENT (x defines:GW_JINGOISM_REQUIREMENT_MOD in a great war).

When a war goal is added, its on_add effect runs. The nation adding the war goal is placed in the primary slot, with the nation targeted by the war goal in the from slot. The nation adding the war goal gains infamy (if the war goal was not already available). This is done by adding up the infamy as determined by the po tags and multiplying by the CB's `badboy_factor`. If it is a great war, this value is further multiplied by defines:GW_JUSTIFY_CB_BADBOY_IMPACT.

#### Testing if a war goal is valid

- Allowed countries:
The allowed country condition, if present, must be satisfied (evaluated with the nation targeted by the war goal in the primary slot, the nation adding the war goal in the this slot, and any other nation associated with the war goal -- for example the liberation target -- in the from slot if any).

- Allowed states:
If the war goal has `all_allowed_states` it will affect all states satisfying the condition. In any case, there must be some state owned by the target nation that passes the `allowed_states` condition if present (evaluated with that state in the primary slot, the nation adding the war goal in the this slot, and any other nation associated with the war goal -- for example the liberation target -- in the from slot if any).

#### Peace cost

Each war goal has a value that determines how much it is worth in a peace offer (and peace offers are capped at 100 points of war goals). Great war obligatory war goals cost 0. Then we iterate over the po tags and sum up the peace cost for each. Some tags have a fixed cost in the defines, such as define:PEACE_COST_RELEASE_PUPPET. For anything that conquers provinces directly (ex: demand state), each province is worth its value relative to the total cost of provinces owned by the target (see below) x 2.8. For po_clear_union_sphere, the cost is defines:PEACE_COST_CLEAR_UNION_SPHERE x the number of nations that will be affected. If the war is a great war, this cost is multiplied by defines:GW_WARSCORE_COST_MOD. If it is a great war, world wars are enabled, and the war score is at least defines:GW_WARSCORE_2_THRESHOLD, the cost is multiplied by defines:GW_WARSCORE_COST_MOD_2 instead. The peace cost of a single war goal is capped at 100.0.

#### Ticking war score

- ticking war score based on occupying war goals (po_annex, po_transfer_provinces, po_demand_state) and letting time elapse, winning battles (tws_from_battles > 0)
- limited by define:TWS_CB_LIMIT_DEFAULT
- to calculate: first you need to figure out the percentage of the war goal complete. This is percentage of provinces occupied or, for war score from battles see Battle score below

#### Battle score

- zero if fewer than define:TWS_BATTLE_MIN_COUNT have been fought
- only if the war goal has tws_battle_factor > 0
- calculate relative losses for each side (something on the order of the difference in losses / 10,000 for land combat or the difference in losses / 10 for sea combat) with the points going to the winner, and then take the total of the relative loss scores for both sides and divide by the relative loss score for the defender.
- subtract from tws_battle_factor and then divide by define:TWS_BATTLE_MAX_ASPECT (limited to -1 to +1). This then works is the occupied percentage described below.

#### Occupation score

Increases by occupation-percentage x define:TWS_FULFILLED_SPEED (up to define:TWS_CB_LIMIT_DEFAULT) when the percentage occupied is >= define:TWS_FULFILLED_IDLE_SPACE or when the occupation percentage is > 0 and the current occupation score is negative.
If there is no occupation, the score decreases by define:TWS_NOT_FULFILLED_SPEED. This can only take the score into negative after define:TWS_GRACE_PERIOD_DAYS, at which point it can go to -define:TWS_CB_LIMIT_DEFAULT.

#### War score outside war goals

Outside of war score from specific war goals, winning or losing battles can contribute up to define:MAX_WARSCORE_FROM_BATTLES. Other occupation counts proportionally to province score of the occupied provinces compared to the target's total province score.

#### Directed war score

Directed war score is how much war score a particular nation has against another particular nation on the other side of the conflict. This is used when adding war goals, and when one nation is trying to make a separate peace with another. Directed war score is considered to be 100 if the attacker has all provinces that they possibly can occupied and either 50% of the provinces of the target occupied or the target's capital occupied. (Or, -100 if the reverse is true. This appears to be a kind of forced capitulation condition.) If at least one of the two nations involved is a leader of their side, war score from battles applied. This score is capped at define:MAX_WARSCORE_FROM_BATTLES. For each land battle the winner gets the difference in strength losses (or zero if they won with more strength losses) / 10 plus an additional 0.1 added to their "direction" of the war score.

War score from occupations requires us to know the value of provinces. All provinces have a base value of 1. For non colonial provinces: each level of naval base increases its value by 1. If it is a state capital, its value increases by 1 for every factory in the state (factory level does not matter). Provinces get 1 point per fort level. This value is the doubled for non-overseas provinces where the owner has a core. It is then tripled for the nation's capital province.

For each province owned by B that A occupies, it gets credit toward its side of the war score equal to 100 x that province's value as a fraction of all the provinces owned by B. However, that province counts for only 3/4 of that value if it is currently blockaded by an enemy (why?).

Finally, war score is affected by any war goals in play against either side that have war score associated with them. Take the peace cost of the war goal and calculate 1^(peace-cost / define:TWS_CB_LIMIT_DEFAULT) x current-war-goal-score. This value is also added into the effective war score.

Directed war score is always treated as being in the -100 to 100 range.

### Invalid  war states

- A primary belligerent may not be a vassal or substate of another nation
- War goals must remain valid (must pass their is-valid trigger check, must have existing target countries, target countries must pass any trigger checks, there must be a valid state in the target country, the country that added them must still be in the war, etc)
- Crisis war goals may not be removed -- this may override other validity checks
- There must be at least one war goal (other than status quo) in the war
- idle for too long -- if the war goes too long without some event happening within it (battle or occupation) it may be terminated. If something is occupied, I believe the war is safe from termination in this way

### Mobilization

Mobilization size = national-modifier-to-mobilization-size + technology-modifier-to-mobilization-size
Mobilization impact = 1 - mobilization-size x (national-mobilization-economy-impact-modifier + technology-mobilization-impact-modifier), to a minimum of zero.

Mobilized regiments come only from unoccupied, non-colonial provinces. In those provinces, mobilized regiments come from non-soldier, non-slave, poor-strata pops with a culture that is either the primary culture of the nation or an accepted culture. The number of regiments these pops can provide is determined by pop-size x mobilization-size / define:POP_SIZE_PER_REGIMENT. Pops will provide up to this number of regiments per pop, although regiments they are already providing to rebels or which are already mobilized count against this number. At most, national-mobilization-impact-modifier x define:MIN_MOBILIZE_LIMIT v nation's-number-of-regiments regiments may be created by mobilization.

Mobilization is not instant. Province by province, mobilization advances by define:MOBILIZATION_SPEED_BASE x (1 + define:MOBILIZATION_SPEED_RAILS_MULT x average-railroad-level-in-state / 5) until it reaches 1. Once mobilization has occurred in one province, mobilization in the next province can start. The order this occurs in appears to be determined by the speed of mobilization: the provinces that will mobilize faster go before those that will go slower.

### Province conquest

When a province changes ownership:
All pops lose any money they have put into the bank (and the central bank retains that money, so now no pop could possibly withdraw it). Pops leave any movements or rebel factions they are part of. If the province is not colonial, and the new owner does not have a core there, any pops with a culture that is not primary or accepted for the new owner gain define:MIL_FROM_CONQUEST militancy.

All timed modifiers in the province automatically expire and any ongoing constructions are canceled.
The conquered province gets nationalism equal to define:YEARS_OF_NATIONALISM.
If the nation doing the conquering has a non-zero research-points-on-conquer modifier:
The nation gets "1 year of research points" for each province conquered times its research-points-on-conquest multiplier, and as an unciv (usually) this allows it to exceed its normal research points cap. However, this one year of research points isn't calculated normally; it is calculated as if the province being conquered was an OPM with the modifiers and technology of the conquering nation. Specifically, we let pop-sum = for each pop type in the province (research-points-from-type x 1v(fraction of population / optimal fraction)). Then, the research points earned are 365 x ((national-modifier-research-points-modifier + tech-research-modifier + 1) x (national-modifier-to-research-points) + pop-sum)) x research-points-on-conquer-modifier

Provinces a civ conquers from an unciv are turned into colonies.

### Siege

Garrison recovers at 10% per day when not being sieged (to 100%)

Only stationary, non black flagged regiments with at least 0.001 strength contribute to a siege.

If there is a regiment with siege present:
We find the effective level of the fort by subtracting: (rounding this value down to to the nearest integer) greatest-siege-value-present x
((the ratio of the strength of regiments with siege present to the total strength of all regiments) ^ define:ENGINEER_UNIT_RATIO) / define:ENGINEER_UNIT_RATIO, reducing it to a minimum of 0.
We calculate the siege speed modifier as: 1 + define:RECON_SIEGE_EFFECT x greatest-reconnaissance-value-present x ((the ratio of the strength of regiments with reconnaissance present to the total strength of all regiments) ^ define:RECON_UNIT_RATIO) / define:RECON_UNIT_RATIO.
We calculate the modifier for number of brigades: first we get the "number of brigades" as total-strength-of-regiments x 1000 / define:POP_SIZE_PER_REGIMENT, and capping it to at most define:SIEGE_BRIGADES_MAX. Then we calculate the bonus as (number-of-brigades - define:SIEGE_BRIGADES_MIN) x define:SIEGE_BRIGADES_BONUS if number-of-brigades is greater the minimum, and as number-of-brigades / define:SIEGE_BRIGADES_MIN otherwise.
Finally, the amount subtracted from the garrison each day is: siege-speed-modifier x number-of-brigades-modifier x Progress-Table\[random-int-from-0-to-9\] x (1.25 if the owner is sieging it back) x (1.1 if the sieger is not the owner but does have a core) / Siege-Table\[effective-fort-level\]

The garrison returns to 100% immediately after the siege is complete and the controller changes. If your siege returns a province to its owner's control without the owner participating, you get +25 relations with the owner.

When a province controller changes as the result of a siege, and it does not go back to the owner a random, `on_siege_win` event is fired, subject to the conditions of the events being met.

Siege Table:
0: 1.0
1: 2.0
2: 3.8
3: 3.4
4: 3.8
5: 4.2
6: 4.5
7: 4.8
8: 5.0
9: 5.2

Progress Table:
0: 0
1: 0.2
2: 0.5
3: 0.75
4: 0.75
5: 1
6: 1.1
7: 1.1
8: 1.25
9: 1.25

### Land combat

(Units in this section means regiments)

- For the duration of this section we will define a regiment's ADMOS as its (attack + defense + maneuver) x organization x strength.
- Combat width for a battle is determined by the least define:BASE_COMBAT_WIDTH + technology-modifier-to-combat width of any participant, from a maximum of 30 to a minimum of 2, multiplied by the combat-width modifier from terrain + 1.
- Effective military tactics = define:BASE_MILITARY_TACTICS + tactics-from-tech
- Each side has a leader that is in charge of the combat, which is the leader with the greatest value as determined by the following formula: (organization x 5 + attack + defend + morale + speed + attrition + experience / 2 + reconnaissance / 5  + reliability / 5) x (prestige + 1)

Any units added to an ongoing combat after the initial ones are added first as additional reserves. Units with strength less than 0.1 or organization less than 1 are immediately treated as retreating. Then divide the units into three groups: those with positive support value, those with zero support and positive reconnaissance, and the rest.

The initial front line will be constructed with pairs of reconnaissance units flanking the center, although I believe giving precedence to non reconnaissance units (? someone should double check this). The number of units without reconnaissance placed in the front line is limited to at most the number of opposing regiments. In all cases the highest ADMOS units are chosen for the initial front line. Units with positive support are placed in the back line in order of ADMOS.
Then we try to fill the back line with reserves. If both the front slot and back slot are empty, this is first done by finding a non-support unit with organization at least 1 and strength at least 0.1. Then for back row slots where the front slot is not empty, units with positive support value are put in, and then finally non support units are put in (someone should double check this; do support units really get preference here?).
All remaining units that are not initially placed on the field are put in the reserves.

On the first day, and every five days thereafter, each side in the combat receives a "die roll" of 0-to-9, inclusive.
Whenever the dice are rolled after the first day, if the attacker rolls higher than the defender, the defender loses one point of dig-in.
One side may instantly wipe the other on the first day. This happens if one side has less than one tenth the total unit strength of the other. In a wipe, each regiment is treated as having been destroyed.

Combat modifiers:
In addition to the dice, both sides are affected by:
Attack or defend trait of the leader in charge (attack for attackers, defend for defenders)
define:GAS_ATTACK_MODIFIER if the side has gas attack and the other side doesn't have gas defense
terrain defense bonus (defender only)
-1 for attacking across a river or -2 for attacking from the sea
minus the least dig in value on the opposing side / 2 (if the side has at a fraction of units with reconnaissance at least define:RECON_UNIT_RATIO) or divided by (greatest-regiment-reconnaissance x (leader-reconnaissance + 1) x fraction-of-reconnaissance-unit-strength / total-strength)

On each turn the attacker and defender deal damage simultaneously

Units attack the opposing front row, and may look either left or right of their position up to `maneuver` positions.
Units on the attacking side use their `attack` stat to do damage, while units on the defending side use `defense`.
Strength damage dealt: unit-strength x (attack/defense x 0.1 + 1) x Modifier-Table\[modifiers + 2\] x 2 / ((effective-fort-level x 0.1 + 1) x opposing-effective-military-tactics x (experience-of-opposing-regiment x 0.1 + 1))
Organization damage dealt: unit-strength x (attack/defense x 0.1 + 1) x Modifier-Table\[modifiers + 2\] x 2 / ((effective-fort-level x 0.1 + 1) x opposing-discipline (if non zero) x (experience-of-opposing-regiment x 0.1 + 1))
Units attacking from the back row have these values multiplied by the unit's support value.

If the unit is in a province controlled by a hostile power, we find the effective level of the fort as with a siege (We find the effective level of the fort by subtracting: (rounding this value down to to the nearest integer) greatest-siege-value-present x
((the ratio of the strength of regiments with siege present to the total strength of all regiments) ^ define:ENGINEER_UNIT_RATIO) / define:ENGINEER_UNIT_RATIO, reducing it to a minimum of 0.)

When a regiment takes strength damage, the size of the pop backing it is reduced by: define:POP_SIZE_PER_REGIMENT x damage-amount x define:SOLDIER_TO_POP_DAMAGE / (max-strength x (solder-to-pop-loss-from-tech + 1)). Note that this applied to damage from attrition as well.

When a rebel regiment is destroyed, divide the militancy of the backing pop by define:REDUCTION_AFTER_DEFEAT.

On completion of a land battle, both sides get war exhaustion equal to: define:COMBATLOSS_WAR_EXHAUSTION x (technology-war-exhaustion-modifier + 1) x strength-lost / (define:POP_SIZE_PER_REGIMENT x total-recruitable-regiments)

Each winning combatant get a random `on_battle_won` event, and each losing combatant gets a random `on_battle_lost` event.

Both sides compute their scaled losses fraction, which is strength losses as a percentage of total possible regiment strength (including mobilization). The scaled losses fraction of the loser / the sum of the scaled losses forms the base of the prestige gain for the nation and the leader in charge on the winning side. The winning leader gets that value / 100 as added prestige. The winning nations gets (define:LEADER_PRESTIGE_LAND_GAIN + 1) x (prestige-from-tech-modifier + 1) x that value. Similarly, the losing nation and leader have their prestige reduced, calculated in the same way.

War score is gained based on the difference in losses (in absolute terms) divided by 5 plus 0.1 to a minimum of 0.1

When you beat someone else's rebels without them being present, you get +5 relations with them.

Then, any unit in the back row with an empty font row slot ahead of it is moved forwards.

Modifier Table
-1 or less: 0
0: 0.02
1: 0.04
2: 0.06
3: 0.08
4: 0.1
5: 0.12
6: 0.16
7: 0.20
8: 0.25
9: 0.30
10: 0.35
11: 0.40
12: 0.45
13: 0.50
14: 0.60
15: 0.70
16: 0.80
17+: 0.90

### Naval combat

(Units in this section means ships)

On the first day, and every five days thereafter, each side in the combat receives a "die roll" of 0-to-9, inclusive.
One side may instantly wipe the other on the first day. This happens if one side has less than one tenth the total hull strength of the other. In a wipe, each ship is treated as having been sunk.

The side with more non-retreating ships has a stacking penalty of 1 - other-ships / more-ships, which is capped at 0.9

On each turn the attacker and defender deal damage simultaneously

A ship without a target (i.e. is seeking) has a chance of define:NAVAL_COMBAT_SEEKING_CHANCE + leader-in-charge-reconnaissance x (1.0 - define:NAVAL_COMBAT_STACKING_TARGET_SEEK x stacking-penalty-factor), or define:NAVAL_COMBAT_SEEKING_CHANCE_MIN, whichever is greater.
Which target is selected ... is apparently quite complicated. Does anyone care if we copy this exactly or not?
When a target is selected, distance is increased by random-value-in-range-\[0.0, 1.0) x (1.0 - combat-duration^define:NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) / NAVAL_COMBAT_SHIFT_BACK_DURATION_SCALE) x NAVAL_COMBAT_SHIFT_BACK_ON_NEXT_TARGET to a maximum of 1000, and the ship switches to approaching.

An approaching ship:
Has its distance reduced by (random-value-in-range-\[0.0 - 0.5) + 0.5) x max-speed x define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR to a minimum of 0.
Switches to engaged when its distance + the target's distance is less than its fire range

An engaged ships:
An engaged ship has a daily chance of stacking-penalty-factor x define:NAVAL_COMBAT_STACKING_TARGET_CHANCE + define: NAVAL_COMBAT_CHANGE_TARGET_CHANCE of switching targets. This would move it back into seeking.
Torpedo attack: is treated as 0 except against big ships
Damage to organization is (gun-power + torpedo-attack) x Modifier-Table\[modifiers + 2\] (see above) x target-strength x define:NAVAL_COMBAT_DAMAGE_ORG_MULT / (target-max-hull x target-experience x 0.1 + 1)
Damage to strength is (gun-power + torpedo-attack) x Modifier-Table\[modifiers + 2\] (see above) x attacker-strength x define:NAVAL_COMBAT_DAMAGE_STR_MULT x define:NAVAL_COMBAT_DAMAGE_MULT_NO_ORG (if target has no org) / (target-max-hull x target-experience x 0.1 + 1)

Retreating ships:
A ship with a strength fraction or organization fraction less than define:NAVAL_COMBAT_RETREAT_STR_ORG_LEVEL will attempt to retreat. The probability of a ship retreating is define:NAVAL_COMBAT_RETREAT_CHANCE  + leader-in-charge-experience + leader-in-charge_speed
A retreating ship will increase its distance by define:NAVAL_COMBAT_RETREAT_SPEED_MOD x define:NAVAL_COMBAT_SPEED_TO_DISTANCE_FACTOR x (random value in the range \[0.0 - 0.5) + 0.5) x ship-max-speed.
When it reaches a distance of 1000 it has successfully retreated.

Note that ship losses are reported in terms of ships sunk (vs. land combat, where they are reported in terms of damage done).

Embarked units that are destroyed because of their ship being destroyed do not reduce the pop size of the backing pop (i.e. they are not treated as having been dealt the damage required to destroy them).

On finishing a naval battle:
Each winning combatant get a random `on_battle_won` event, and each losing combatant gets a random `on_battle_lost` event.

Both sides compute their scaled losses fraction, which is (1 + (sum over ship type: supply-consumption-score x strength-losses)) as a percentage of (total possible naval supply + 1). The scaled losses fraction of the loser / the sum of the scaled losses forms the base of the prestige gain for the nation and the leader in charge on the winning side. The winning leader gets that value / 100 as added prestige. The winning nations gets (defineLEADER_PRESTIGE_NAVAL_GAIN + 1) x (prestige-from-tech-modifier + 1) x that value. Similarly, the losing nation and leader have their prestige reduced, calculated in the same way.

War score is gained based on the difference in losses (in absolute terms) divided by 5 plus 0.1 to a minimum of 0.1

See also: (https://forum.paradoxplaza.com/forum/threads/understanding-naval-combat-sort-of.1123034/), although in the case of disagreements you should assume that this document is correct in the absence of empirical evidence from the game proving otherwise.

### Supply limit

(province-supply-limit-modifier + 1) x (2.5 if it is owned an controlled or 2 if it is just controlled, you are allied to the controller, have military access with the controller, a rebel controls it, it is one of your core provinces, or you are sieging it) x (technology-supply-limit-modifier + 1)

## Units

### Unit daily update

Units in combat gain experience. The exact formula is somewhat opaque to me, but here is what I know: units in combat gain experience proportional to define:EXP_GAIN_DIV, the experience gain bonus provided by their leader + 1, and then some other factor that is always at least 1 and goes up as the opposing side has more organization.

Units that are not in combat and not embarked recover organization daily at: (national-organization-regeneration-modifier + morale-from-tech + leader-morale-trait + 1) x the-unit's-supply-factor / 5 up to the maximum organization possible for the unit.

Units that are moving lose any dig-in bonus they have acquired. A unit that is not moving gets one point of dig-in per define:DIG_IN_INCREASE_EACH_DAYS days.

Units backed by pops with define:MIL_TO_AUTORISE militancy or greater that are in a rebel faction, and which have organization at least 0.75 will become rebel units.

Supplies: Rebel units are always treated as having fully supply. Units not in combat consume supply. Which commodities and how many are consumed in this way depends on the specific type of unit. The supply quantities defined by its type are then multiplied by (2 - national-administrative-efficiency) x (supply-consumption-by-type + national-modifier-to-supply-consumption)^0.01 x (naval-or-land-spending-as-appropriate). The average fraction of that consumption that could be fulfilled times naval or land-spending (as appropriate) for each regiment or ship is then averaged over the army/navy. For armies, that value become their new supply for the army. For navies, this value is further multiplied by (1 - the-fraction-the-nation-is-over-naval-supply) before becoming the new supply value.

Navies with supplies less than define:NAVAL_LOW_SUPPLY_DAMAGE_SUPPLY_STATUS may receive attrition damage. Once a navy has been under that threshold for define:NAVAL_LOW_SUPPLY_DAMAGE_DAYS_DELAY days, each ship in it will receive define:NAVAL_LOW_SUPPLY_DAMAGE_PER_DAY x (1 - navy-supplies / define:NAVAL_LOW_SUPPLY_DAMAGE_SUPPLY_STATUS) damage (to its strength value) until it reaches define:NAVAL_LOW_SUPPLY_DAMAGE_MIN_STR, at which point no more damage will be dealt to it. NOTE: AI controlled navies are exempt from this, and when you realize that this means that *most* ships are exempt, it becomes less clear why we are even bothering the player with it.

### Monthly reinforcement

A unit that is not retreating, not embarked, not in combat is reinforced (has its strength increased) by:
define:REINFORCE_SPEED x (technology-reinforcement-modifier + 1.0) x (2 if in owned province, 0.1 in an unowned port province, 1 in a controlled province, 0.5 if in a province adjacent to a province with military access, 0.25 in a hostile, unblockaded port, and 0.1 in any other hostile province) x (national-reinforce-speed-modifier + 1) x army-supplies x (number of actual regiments / max possible regiments (feels like a bug to me) or 0.5 if mobilized)
The units experience is also reduced s.t. its new experience = old experience / (amount-reinforced / 3 + 1)

### Monthly ship repair / update

A ship that is docked at a naval base is repaired (has its strength increase) by:
maximum-strength x (technology-repair-rate + provincial-modifier-to-repair-rate + 1) x ship-supplies x (national-reinforce-speed-modifier + 1) x navy-supplies

A navy that is at sea out of supply range has its "at sea" counter tick up once per month.

### Monthly attrition

A unit that is not in combat and not black flagged receives attrition damage on the 1st of the month.

For armies:
For armies in their owner's provinces that are not retreating, attrition is calculated based on the total strength of units from their nation in the province. Otherwise, attrition is calculated based on the total strength of all units in the province. First we calculate (total-strength + leader-attrition-trait) x (attrition-modifier-from-technology + 1) - effective-province-supply-limit (rounded down to the nearest integer) + province-attrition-modifier + the-level-of-the-highest-hostile-fort-in-an-adjacent-province. We then reduce that value to at most the max-attrition modifier of the province, and finally we add define:SEIGE_ATTRITION if the army is conducting a siege. Units taking attrition lose max-strength x attrition-value x 0.01 points of strength. This strength loss is treated just like damage taken in combat, meaning that it will reduce the size of the backing pop.

For navies:
Only navies controlled by the player take attrition, and only those that are not in port or are not off of a controlled coast province. Navies in coastal sea zones in general only take attrition if they are out of supply range. The attrition value for such navies is (time-at-sea x 2 + 1 + leader-attrition-trait) x (naval-attrition-from-technology + 1). Whether a ship takes damage from attrition is partly random. The attrition value / 10 is the probability that the ship will take damage. If the ships does take damage, it loses max-strength x attrition-value x 0.01 points of strength.

When player navies die from attrition, the admirals are lost too.

### Movement

Adjacent provinces have a base distance between them (this base also takes terrain into account in some way). When moving to a province, this cost is multiplied by (destination-province-movement-cost-modifier + 1.0)^0.05. The unit "pays" for this cost each day based on its speed, and when it is all paid for, the unit arrives in its destination province. An army's or navy's speed is based on the speed of its slowest ship or regiment x (1 + infrastructure-provided-by-railroads x railroad-level-of-destination) x (possibly-some-modifier-for-crossing-water) x (define:LAND_SPEED_MODIFIER or define:NAVAL_SPEED_MODIFIER) x (leader-speed-trait + 1)

When a unit arrives in a new province, it takes attrition (as if it had spent the monthly tick in the province).

### Unit stats

- Unit experience goes up to 100. Units after being built start with a base experience level equal to the bonus given by technologies + the nations naval/land starting experience modifier (as appropriate)
- Units start with max strength and org after being built

### Unit construction

- Only a single unit is built per province at a time
- See the economy section for details on how construction happens
- After being built, units move towards the nearest rally point, if any, to merge with an army there upon arrival

### Regiments per pop

- A soldier pop must be at least define:POP_MIN_SIZE_FOR_REGIMENT to support any regiments
- If it is at least that large, then it can support one regiment per define:POP_SIZE_PER_REGIMENT x define:POP_MIN_SIZE_FOR_REGIMENT_COLONY_MULTIPLIER (if it is located in a colonial province) x define:POP_MIN_SIZE_FOR_REGIMENT_NONCORE_MULTIPLIER (if it is non-colonial but uncored)

### Naval supply points

- naval supply score: you get define:NAVAL_BASE_SUPPLY_SCORE_BASE x (2 to the power of (its-level - 1)) for each naval base or define:NAVAL_BASE_SUPPLY_SCORE_EMPTY for each state without one, multiplied by define:NAVAL_BASE_NON_CORE_SUPPLY_SCORE if it is neither a core nor connected to the capital (min 1 per coastal state).
- ships consume naval base supply at their supply_consumption_score. Going over the naval supply score comes with various penalties (described elsewhere).

## Movements

- Movements come in three types: political, social, and independence movements. Functionally, we can better distinguish the movements between those associated with a position on some issue (either political or social) and those for the independence of some national identity.
- Movements can accumulate radicalism, but only in civ nations. Internally we may represent radicalism as two values, a radicalism value and transient radicalism. Every day the radicalism value is computed as: define:MOVEMENT_RADICALISM_BASE + the movements current transient radicalism + number-of-political-reforms-passed-in-the-country-over-base x define:MOVEMENT_RADICALISM_PASSED_REFORM_EFFECT + radicalism-of-the-nation's-primary-culture + maximum-nationalism-value-in-any-province x define:MOVEMENT_RADICALISM_NATIONALISM_FACTOR + define:POPULATION_MOVEMENT_RADICAL_FACTOR x movement-support / nation's-non-colonial-population.
- When the radicalism value for a movement reaches 100, pops get removed from the movement and added to a rebel faction. Those pops have their militancy increased to a minimum of define:MIL_ON_REB_MOVE. See below for determining which rebel faction the pop joins.
- Political and social movements are used to calculate the support for political / social reform in the nation by taking the proportion of the non-colonial population involved in political / social movements, respectively, and multiplying by define:MOVEMENT_SUPPORT_UH_FACTOR.

### Membership

- Slave pops cannot belong to a movement
- Pops with define:MIL_TO_JOIN_REBEL or greater militancy cannot join a movement
- Pops may support a movement (either issue based on independence based), and their support is updated daily. If the pop's support of the issue for an issue-based movement drops below define:ISSUE_MOVEMENT_LEAVE_LIMIT the pop will leave the movement. If the pop's militancy falls below define:NATIONALIST_MOVEMENT_MIL_CAP, the pop will leave an independence movement. If the pop is *not* currently part of a movement, not in a colonial province, is not part of a rebel faction, and has a consciousness of at least 1.5 or a literacy of at least 0.25 the pop may join a movement.
- If there are one or more issues that the pop supports by at least define:ISSUE_MOVEMENT_JOIN_LIMIT, then the pop has a chance to join an issue-based movement at probability: issue-support x 9 x define:MOVEMENT_LIT_FACTOR x pop-literacy + issue-support x 9 x define:MOVEMENT_CON_FACTOR x pop-consciousness
- If there are no valid issues, the pop has a militancy of at least define:NATIONALIST_MOVEMENT_MIL_CAP, does not have the primary culture of the nation it is in, and does have the primary culture of some core in its province, then it has a chance (20% ?) of joining an independence movement for such a core.
- Pops joining / leaving a movement appears to happen once per month


### Suppression

- When a movement is suppressed:
Reduce the suppression points for the nation by: if define:POPULATION_SUPPRESSION_FACTOR is zero, movement radicalism + 1, otherwise the greater of movement-radicalism + 1 and movement-radicalism x movement-support / define:POPULATION_SUPPRESSION_FACTOR, to a minimum of zero
Increase the transient radicalism of the movement by: define:SUPPRESSION_RADICALISM_HIT
Set the consciousness of all pops that were in the movement to 1 and remove them from it.

## Rebels

- Rebel fraction: There is a trigger that depends on the fraction of provinces that are controlled by rebels. My notes say that only either not-overseas (or maybe only connected to capital) provinces count for calculating this fraction
- Pops with militancy >= define:MIL_TO_JOIN_REBEL will join a rebel faction rather than a movement (and if the pop's militancy is less than this, it will drop out).
- Picking a faction for a pop:
- A pop in a province sieged or controlled by rebels will join that faction, if the pop is compatible with the faction.
- Otherwise take all the compatible and possible rebel types. Determine the spawn chance for each of them, by taking the *product* of the modifiers. The pop then joins the type with the greatest chance (that's right, it isn't really a *chance* at all). If that type has a defection type, it joins the faction with the national identity most compatible with it and that type (pan-nationalist go to the union tag, everyone else uses the logic I outline below)
- Faction compatibility: a pop will not join a faction that it is excluded from based on its culture, culture group, religion, or ideology (here it is the dominant ideology of the pop that matters). There is also some logic for determining if a pop is compatible with a national identity for independence. I don't think it is worth trying to imitate the logic of the base game here. Instead I will go with: pop is not an accepted culture and either its primary culture is associated with that identity *or* there is no core in the province associated with its primary identity.
- When a pop joins a faction, my notes say that the organization of the faction increases by either by the number of divisions that could spawn from that pop (calculated directly by pop size / define:POP_MIN_SIZE_FOR_REGIMENT) or maybe some multiple of that.
- A rebel faction with no pops in it will disband
- Rebel factions whose independence country exists will disband (different for defection rebels?)
- Pan nationalists inside their union country will disband
- Any pops belonging to a rebel faction that disbands have their militancy reduced to 0
- Sum for each pop belonging to the faction that has made money in the day: (pop-income + 0.02) x pop-literacy x (10 if militancy is > define:MILITANCY_TO_AUTO_RISE and 5 if militancy is less than that but > define:MILITANCY_TO_JOIN_RISING) / (1 + national-administration-spending-setting). Take this sum, multiply by 0.001 x (rebel organization from technology + 1) and divide by the number of regiments those pops could form. If positive, add this to the current organization of the rebel faction (to a maximum of 1). This appears to be done daily.
- Rebels have a chance to rise once per month. If there are pops belonging to the faction with militancy greater than define:MILITANCY_TO_JOIN_RISING that can form at least one regiment, then a random check is made. The probability the rising will happen is: faction-organization x 0.05 + 0.02 + faction-organization x number-of-regiments-the-rising-could-form / 1v(number-of-regiments-controlled-by-nation x 20)
- When a rising happens, pops with at least define:MILITANCY_TO_JOIN_RISING will spawn faction-organization x max-possible-supported-regiments, to a minimum of 1 (if any more regiments are possible).
- Any pop that contributes regiments has its militancy reduced by define:REDUCTION_AFTER_RISING
- Any flashpoint state where one or more rebel regiments is created has its flashpoint tension increased by define:TENSION_ON_REVOLT to a maximum of 100
- Faction organization is reduced to 0 after an initial rising (for later contributory risings, it may instead be reduced by a factor of (number-of-additional-regiments x 0.01 + 1))

### Province defection

If, on the first of the month, a non-capital province is controlled by rebels and it is not currently being sieged, and no battle is currently taking place in it, and it has been at least as many months as defined in the rebel type, the province will defect if the rebel type permits it. Where that defection goes ... is complicated (anyone know the logic for this?), and seems to be biased towards nations with cores in the province, and then adjacent nations failing that. Although capital provinces don't defect in this way, if a nation has only a single state, and a province defects in this way, the whole nation will be annexed into the defection target.

### Rebel victory

Every day, an active rebel faction has its `demands_enforced_trigger` checked. If it wins, it gets its `demands_enforced_effect` executed. Any units for the faction that exist are destroyed (or transferred if it is one of the special rebel types). The government type of the nation will change if the rebel type has an associated government (with the same logic for a government type change from a wargoal or other cause). The upper house will then be set in a special way (even if the type of government has not changed). How the upper house is reset is a bit complicated, so the outline I will give here assumes that you understand in general how the upper house works. First we check the relevant rules: in "same as ruling party" we set it 100% to the ruling party ideology. If it is "state vote" we compute the upper house result for each non-colonial state as usual (i.e. we apply the standard strata voting modifiers and weight the vote by culture as the voting rules require), then we normalize each state result and add them together. Otherwise, we are in direct voting, meaning that we just add up each pop vote (again, weighting those votes according to the usual rules and modifiers). If the rebel type has "break alliances on win" then the nation loses all of its alliances, all of its non-substate vassals, all of its sphere members, and loses all of its influence and has its influence level set to neutral. The nation loses prestige equal to define:PRESTIGE_HIT_ON_BREAK_COUNTRY x (nation's current prestige + permanent prestige), which is multiplied by the nation's prestige modifier from technology + 1 as usual (!).

### Calculating how many regiments are "ready to join" a rebel faction

Loop over all the pops associated with the faction. For each pop with militancy >= define:MIL_TO_JOIN_RISING, the faction is credited one regiment per define:POP_SIZE_PER_REGIMENT the pop has in size.

## Crime

Crime is apparently the single place where the following value matters:
- state administrative efficiency: = define:NONCORE_TAX_PENALTY x number-of-non-core-provinces + (bureaucrat-tax-efficiency x total-number-of-primary-or-accepted-culture-bureaucrats / population-of-state)v1 / x (sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE)), all clamped between 0 and 1.
The crime fighting percent of a province is then calculated as: (state-administration-efficiency x define:ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT + administration-spending-setting x (1 - ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT)) x (define:MAX_CRIMEFIGHTING_PERCENT - define:MIN_CRIMEFIGHTING_PERCENT) + define:MIN_CRIMEFIGHTING_PERCENT

Once per month (the 1st) province crimes are updated. If the province has a crime, the crime fighting percent is the probability of that crime being removed. If there is no crime, the crime fighting percent is the probability that it will remain crime free. If a crime is added to the province, it is selected randomly (with equal probability) from the crimes that are possible for the province (determined by the crime being activated and its trigger passing).

## Technology

### Daily research points

Let pop-sum = for each pop type (research-points-from-type x 1^(fraction of population / optimal fraction))
Then, the daily research points earned by a nation is: (national-modifier-research-points-modifier + tech-research-modifier + 1) x (national-modifier-to-research-points) + pop-sum)

If a nation is not currently researching a tech (or is an unciv), research points will be banked, up to a total of 365 x daily research points, for civs, or define:MAX_RESEARCH_POINTS for uncivs.

If a nation is researching a tech, a max of define:MAX_DAILY_RESEARCH points can be dumped into it per day.

There is also an instant research cheat, but I see no reason to put it in the game.

### Cost

The effective amount of research points a tech costs = base-cost x 0v(1 - (current-year - tech-availability-year) / define:TECH_YEAR_SPAN) x define:TECH_FACTOR_VASSAL(if your overlord has the tech) / (1 + tech-category-research-modifier)

### Inventions

Inventions have a chance to be discovered on the 1st of every month. The invention chance modifier is computed additively, and the result is the chance out of 100 that the invention will be discovered. When an invention with shared prestige is discovered, the discoverer gains that amount of shared prestige / the number of times it has been discovered (including the current time).

## Politics

### Party loyalty

Each province has a party loyalty value (although it is by ideology, not party, so it would be more appropriately called ideology loyalty ... whatever). If the national focus is present in the state for party loyalty, the loyalty in each province in the state for that ideology will increase by the loyalty value of the focus (daily).

### When a social/political reform is possible

These are only available for civ nations. If it is "next step only" either the previous or next issue option must be in effect. At least define:MIN_DELAY_BETWEEN_REFORMS months must have passed since the last issue option change (for any type of issue). And it's `allow` trigger must be satisfied. Then. for each ideology, we test its `add_social_reform` or `remove_social_reform` (depending if we are increasing or decreasing, and substituting `political_reform` here as necessary), computing its modifier additively, and then adding the result x the fraction of the upper house that the ideology has to a running total. If the running total is > 0.5, the issue option can be implemented.

### When an economic/military reform is possible

These are only available for unciv nations. Some of the rules are the same as for social/political reforms:  If it is "next step only" either the previous or next reform option must be in effect. At least define:MIN_DELAY_BETWEEN_REFORMS months must have passed since the last reform option change (for any type of reform). And it's `allow` trigger must be satisfied. Where things are different: Each reform also has a cost in research points. This cost, however, can vary. The actual cost you must pay is multiplied by what I call the "reform factor" + 1. The reform factor is (sum of ideology-in-upper-house x add-reform-triggered-modifier) x define:ECONOMIC_REFORM_UH_FACTOR + the nation's self_unciv_economic/military_modifier + the unciv_economic/military_modifier of the nation it is in the sphere of (if any).

### Pop voting weight

Since this is used in a couple of places, it is easiest to make it its own rule: When a pop's "votes" in any form, the weight of that vote is the product of the size of the pop and the national modifier for voting for their strata (this could easily result in a strata having no votes). If the nation has primary culture voting set then primary culture pops get a full vote, accepted culture pops get a half vote, and other culture pops get no vote. If it has culture voting, primary and accepted culture pops get a full vote and no one else gets a vote. If neither is set, all pops get an equal vote.

### Yearly upper house updates

Every year, the upper house of each nation is updated. If the "same as ruling party" rule is set, the upper house becomes 100% the ideology of the ruling party. If the rule is "state vote", then for each non-colonial state: for each pop in the state that is not prevented from voting by its type we distribute its weighted vote proportionally to its ideology support, giving us an ideology distribution for each of those states. The state ideology distributions are then normalized and summed to form the distribution for the upper house. For "population_vote" and "rich_only" the voting weight of each non colonial pop (or just the rich ones for "rich only") is distributed proportionally to its ideological support, with the sum for all eligible pops forming the distribution for the upper house.

### Elections

A country with elections starts one every 5 years.

Elections last define:CAMPAIGN_DURATION months.

To determine the outcome of election, we must do the following:
- Determine the vote in each province. Note that voting is *by active party* not by ideology.
- Provinces in colonial states don't get to vote
- For each party we do the following: figure out the pop's ideological support for the party and its issues based support for the party (by summing up its support for each issue that the party has set, except that pops of non-accepted cultures will never support more restrictive culture voting parties). The pop then votes for the party (i.e. contributes its voting weight in support) based on the sum of its issue and ideological support, except that the greater consciousness the pop has, the more its vote is based on ideological support (pops with 0 consciousness vote based on issues alone). I don't know the exact ratio (does anyone care if I don't use the exact ratio?). The support for the party is then multiplied by (provincial-modifier-ruling-party-support + national-modifier-ruling-party-support + 1), if it is the ruling party, and by (1 + province-party-loyalty) for its ideology.
- Pop votes are also multiplied by (provincial-modifier-number-of-voters + 1)
- After the vote has occurred in each province, the winning party there has the province's ideological loyalty for its ideology increased by define:LOYALTY_BOOST_ON_PARTY_WIN x (provincial-boost-strongest-party-modifier + 1) x fraction-of-vote-for-winning-party
- If voting rule "largest_share" is in effect: all votes are added to the sum towards the party that recieved the most votes in the province. If it is "dhont", then the votes in each province are normalized to the number of votes from the province, and for "sainte_laque" the votes from the provinces are simply summed up.

What happens with the election result depends partly on the average militancy of the nation. If it is less than 5: We find the ideology group with the greatest support (each active party gives their support to their group), then the party with the greatest support from the winning group is elected. If average militancy is greater than 5: the party with the greatest individual support wins. (Note: so at average militancy less than 5, having parties with duplicate ideologies makes an ideology group much more likely to win, because they get double counted.)

### Election events

While an election is ongoing, random events from the `on_election_tick` category will be fired. These events are picked in the usual way: based on their weights out of those that have their trigger conditions (if any) satisfied. Events occur once every define:CAMPAIGN_EVENT_BASE_TIME / 2 days to define:CAMPAIGN_EVENT_BASE_TIME days (uniformly distributed).

## Scores and ranking

### Industrial score

Is the sum of the following two components:
- For each state: (fraction of factory workers in each state (types marked with can work factory = yes) to the total-workforce x building level of factories in the state (capped at 1)) x total-factory-levels
- For each country that the nation is invested in: define:INVESTMENT_SCORE_FACTOR x the amount invested x 0.01

### Military score

The military score is itself a complicated sum:

The first part  is complicated enough that I am going to simplify things slightly, and ignore how mobilization can interact with this:
First, we need to know the total number of recruitable regiments
We also need to know the average land unit score, which we define here as (attack + defense + national land attack modifier + national land defense modifier) x discipline
Then we take the lesser of the number of regiments in the field x 4 or the number of recruitable regiments and multiply it by define:DISARMAMENT_ARMY_HIT (if disarmed) multiply that by the average land unit score, multiply again by (national-modifier-to-supply-consumption + 1), and then divide by 7.

To that we add for each capital ship: (hull points + national-naval-defense-modifier) x (gun power + national-naval-attack-modifier) / 250

And then we add one point either per leader or per regiment, whichever is greater.

## Spheres and Great Powers

### Great powers

An uncivilized nation or a nation with only a single state cannot be a great power.

When a nation becomes great, it ceases to be a vassal / substate. It also gets an `on_new_great_nation` event.

When a nation loses GP status, it gets an `on_lost_great_nation` event.

A nation that falls out of the top 8 does not stop being a GP until define:GREATNESS_DAYS have passed since it was last a GP *and* it is not currently involved in a crisis war, or a war that could potentially be a great war (if they were to be enabled), *and* it is not involved in a crisis as either a defender or attacker.

### Influence

A nation does not accumulate influence if: their embassy has been banned, if they are at war against the nation, if they have a truce with the nation, if the nation has priority 0, or if their influence is capped (at max value with no other GP influencing).

The nation gets a daily increase of define:BASE_GREATPOWER_DAILY_INFLUENCE x (national-modifier-to-influence-gain + 1) x (technology-modifier-to-influence + 1). This is then divided among the nations they are accumulating influence with in proportion to their priority (so a target with priority 2 receives 2 shares instead of 1, etc).

This influence value does not translate directly into influence with the target nation. Instead it is first multiplied by the following factor:
1 + define:DISCREDIT_INFLUENCE_GAIN_FACTOR (if discredited) + define:NEIGHBOUR_BONUS_INFLUENCE_PERCENT (if the nations are adjacent) + define:SPHERE_NEIGHBOUR_BONUS_INFLUENCE_PERCENT (if some member of the influencing nation's sphere is adjacent but not the influencing nation itself) + define:OTHER_CONTINENT_BONUS_INFLUENCE_PERCENT (if the influencing nation and the target have capitals on different continents) + define:PUPPET_BONUS_INFLUENCE_PERCENT (if the target is a vassal of the influencer) + relation-value / define:RELATION_INFLUENCE_MODIFIER + define:INVESTMENT_INFLUENCE_DEFENCE x fraction-of-influencer's-foreign-investment-out-of-total-foreign-investment + define:LARGE_POPULATION_INFLUENCE_PENALTY x target-population / define:LARGE_POPULATION_INFLUENCE_PENALTY_CHUNK (if the target nation has population greater than define:LARGE_POPULATION_LIMIT) + (1 - target-score / influencer-score)^0

Note that you cannot gain influence with a nation while your embassy is banned, but you still appear to "spend" influence anyways. Any influence that accumulates beyond the max (define:MAX_INFLUENCE) will be subtracted from the influence of the great power with the most influence (other than the influencing nation).

General rules for influence actions: you cannot take an influence action where the influenced nation is in a war opposing you or if your embassy has been banned there. And, obviously, you must have enough influence points to pay the cost.

Discrediting:

A nation is discredited for define:DISCREDIT_DAYS. Being discredited twice does not add these durations together; it just resets the timer from the current day. Discrediting a nation "increases" your relationship with them by define:DISCREDIT_RELATION_ON_ACCEPT. Discrediting costs define:DISCREDIT_INFLUENCE_COST influence points. To discredit a nation, you must have an opinion of at least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation than the nation you are discrediting does.

Expelling advisors:

Expelling a nation's advisors "increases" your relationship with them by define:EXPELADVISORS_RELATION_ON_ACCEPT. This action costs define:EXPELADVISORS_INFLUENCE_COST influence points. To expel the advisors of a nation, you must have a an equal or better opinion level with the influenced nation than the nation you are doing the action to does does. Being expelled cancels any ongoing discredit effect. Being expelled reduces your influence to zero. To expel advisors you must have at least neutral opinion with the influenced nation and an equal or better opinion level than that of the nation you are expelling.

Banning embassy:

Banning a nation's embassy "increases" your relationship with them by define:BANEMBASSY_RELATION_ON_ACCEPT. This action costs define:BANEMBASSY_INFLUENCE_COST influence points. The ban embassy effect lasts for define:BANEMBASSY_DAYS. If you are already banned, being banned again simply restarts the timer. Being banned cancels out any ongoing discredit effect. To ban a nation you must be at least friendly with the influenced nation and have an equal or better opinion level than that of the nation you are expelling.

Increase opinion:

Increasing the opinion of a nation costs define:INCREASEOPINION_INFLUENCE_COST influence points. Opinion can be increased to a maximum of friendly.
	
Decrease opinion:

Decreasing the opinion of a nation "increases" your relationship with them by define:DECREASEOPINION_RELATION_ON_ACCEPT. This actions costs define:DECREASEOPINION_INFLUENCE_COST influence points. Decreasing the opinion of another nation requires that you have an opinion of at least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation than the nation you are lowering their opinion of does.
	
Add to sphere:

Adding a nation to your sphere costs define:ADDTOSPHERE_INFLUENCE_COST influence points, and the nation must have a friendly opinion of you.

Remove from sphere:

Removing a nation from a sphere costs define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. If you remove a nation from your own sphere you lose define:REMOVEFROMSPHERE_PRESTIGE_COST prestige and gain define:REMOVEFROMSPHERE_INFAMY_COST infamy. Removing a nation from the sphere of another nation "increases" your relationship with the former sphere leader by define:REMOVEFROMSPHERE_RELATION_ON_ACCEPT points. To preform this action you must have an opinion level of at least friendly with the nation you are removing from a sphere. (The removed nation then becomes friendly with its former sphere leader.)

## Crisis mechanics

### Flashpoint tension

The flashpoint focus is only available for non great power, non secondary power, and must be placed in a state that already has a flashpoint (in some province). States that are part of a substate (i.e. the special vassal type) or states that contain a nation's capital cannot have flashpoints or accumulate tension.

Whether a state contains a flashpoint depends on: whether a province in the state contains a core other than that of its owner and of a tag that is marked as releasable (i.e. not a cultural union core), and which has a primary culture that is not the primary culture or an accepted culture of its owner. If any core qualifies, the state is considered to be a flashpoint, and its default flashpoint tag will be the qualifying core whose culture has the greatest population in the state. (However, if a nation puts a flashpoint focus in the state, the flashpoint tag will change to that of the nation that placed the focus.)

- If at least one nation has a CB on the owner of a flashpoint state, the tension increases by define:TENSION_FROM_CB per day.
- If there is an independence movement within the nation owning the state for the independence tag, the tension will increase by movement-radicalism x define:TENSION_FROM_MOVEMENT x fraction-of-population-in-state-with-same-culture-as-independence-tag x movement-support / 4000, up to a maximum of define:TENSION_FROM_MOVEMENT_MAX per day.
- Any flashpoint focus increases the tension by the amount listed in it per day.
- Tension increases by define:TENSION_DECAY per day (this is negative, so this is actually a daily decrease).
- Tension increased by define:TENSION_WHILE_CRISIS per day while a crisis is ongoing.
- If the state is owned by a great power, tension is increased by define:RANK_X_TENSION_DECAY per day
- For each great power at war or disarmed on the same continent as either the owner or the state, tension is increased by define:AT_WAR_TENSION_DECAY per day.
- Tension ranges between 0 and 100

### Starting a Crisis

A crisis may not start until define:CRISIS_COOLDOWN_MONTHS months after the last crisis or crisis war has ended. When a crisis becomes possible, we first check each of the three states with the highest tension > 50 where neither the owner of the state nor the nation associated with the flashpoint (if any) is at war. I believe the probability of a crisis happening in any of those states is 0.001 x define:CRISIS_BASE_CHANCE x state-tension / 100. If this turns into a crisis, the tension in the state is immediately zeroed. Failing that, any contested colonial region where neither colonizer is at war will become a crisis and the colonial "temperature" has reached 100.

### Interested GPs

A GP that is not disarmed and not at war with war exhaustion less than define:CRISIS_INTEREST_WAR_EXHAUSTION_LIMIT and is either on the same continent as the crisis target, crisis attacker, or state (or with a few other special cases: I think European GPs are interested in everything, and I think north and south America are considered one continent here) is considered to be interested. When a GP becomes interested it gets a `on_crisis_declare_interest` event.

### Crisis temperature

Every day where there is at least one defending GP and one attacking GP, the crisis temperature increases by  define:CRISIS_TEMPERATURE_INCREASE x define:CRISIS_TEMPERATURE_PARTICIPANT_FACTOR x the ratio of GPs currently involved in the crisis to the total number of interested GPs and participating GPs.

### Crisis resolution

The nation controlling the state that was the target of the crisis will lose all tension in any of its flashpoint states at the end of the crisis (however it resolves).
If the crisis becomes a war, any interested GP which did not take a side loses (years-after-start-date x define:CRISIS_DID_NOT_TAKE_SIDE_PRESTIGE_FACTOR_YEAR + define:CRISIS_DID_NOT_TAKE_SIDE_PRESTIGE_FACTOR_BASE) as a fraction of their prestige.
If the crisis fails with a GP backer but no attackers, the lone backer gets extra prestige equal to define:LONE_BACKER_PRESTIGE_FACTOR as a fraction of their prestige.

## Colonization

You can invest colonially in a region if there are fewer than 4 other colonists there (or you already have a colonist there). You must also have sufficient liferating tech. Specifically, you must have colonial life rating points from technology + define:COLONIAL_LIFERATING less than or equal to the *greatest* life rating of an unowned province in the state. Your country must be of define:COLONIAL_RANK or less. The state may not be the current target of a crisis, nor may your country be involved in an active crisis war.

If you haven't yet put a colonist into the region, you must be in range of the region. Any region adjacent to your country or to one of your vassals or substates is considered to be in range. Otherwise it must be in range of one of your naval bases, with the range depending on the colonial range value provided by the naval base building x the level of the naval base.

If you have put a colonist in the region, and colonization is in phase 1 or 2, you can invest if it has been at least define:COLONIZATION_DAYS_BETWEEN_INVESTMENT since your last investment, you have enough colonial points, and the state remains in range.

If you have put in a colonist in a region and it goes at least define:COLONIZATION_DAYS_FOR_INITIAL_INVESTMENT without any other colonizers, it then moves into phase 3 with define:COLONIZATION_INTEREST_LEAD points.

If you get define:COLONIZATION_INTEREST_LEAD points, and it is phase 1, it moves into phase 2, kicking out all but the second-most colonizer (in terms of points).

In phase 2 if you get define:COLONIZATION_INFLUENCE_LEAD points ahead of the other colonizer, the other colonizer is kicked out and the phase moves to 3.

In phase 2 if there are competing colonizers, the "temperature" in the colony will rise by define:COLONIAL_INFLUENCE_TEMP_PER_DAY + maximum-points-invested x define:COLONIAL_INFLUENCE_TEMP_PER_LEVEL + define:TENSION_WHILE_CRISIS (if there is some other crisis going on) + define:AT_WAR_TENSION_DECAY (if either of the two colonizers are at war or disarmed)

To finish colonization and make a protectorate: you must be in colonization phase 3, you must have define:COLONIZATION_CREATE_PROTECTORATE_COST free colonial points, and your colonist in the region must have non zero points.

If you leave a colony in phase 3 for define:COLONIZATION_MONTHS_TO_COLONIZE months, the colonization will reset to phase 0 (no colonization in progress).

To turn a protectorate into a colony, you must have define:COLONIZATION_CREATE_COLONY_COST x number-of-provinces-in-state colonial points free.

To turn a colony into a regular state, it must have enough bureaucrats with your primary culture to make up define:STATE_CREATION_ADMIN_LIMIT fraction of the population. If the colony has a direct land connection to your capital, this doesn't require any free colonial points. Otherwise, you must also have define:COLONIZATION_CREATE_STATE_COST x number-of-provinces-in-state x 1v(distance-from-capital/define:COLONIZATION_COLONY_STATE_DISTANCE) free points.

### Points

Only nations with rank at least define:COLONIAL_RANK get colonial points. Colonial points come from three sources:
- naval bases: (1) determined by level and the building definition, except you get only define:COLONIAL_POINTS_FOR_NON_CORE_BASE (a flat rate) for naval bases not in a core province and not connected by land to the capital. (2) multiply that result by define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR (NOTE to self: double check this)
- units: the colonial points they grant x (1.0 - the fraction the nation's naval supply consumption is over that provided by its naval bases) x define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR
- points from technologies/inventions

Generally "spent" colonial points get tied up in the colony, and are returned to you when it stops being a colony (the exception being the point "cost" to turn a colony into a state, which is really just a requirement to have a certain number of colonial points unused). For any state that is in the process of being turned into a colony, any points spent there are locked away until the process ends, one way or another. Each province in a protectorate state costs define:COLONIZATION_PROTECTORATE_PROVINCE_MAINTAINANCE points. Each province in a colony state costs define:COLONIZATION_COLONY_PROVINCE_MAINTAINANCE + infrastructure-value-provided-by-railroads x railroad-level-in-the-province x define:COLONIZATION_COLONY_RAILWAY_MAINTAINANCE. Additionally, a colony state costs define:COLONIZATION_COLONY_INDUSTRY_MAINTAINANCE x the-number-of-factories.

Investing in a colony costs define:COLONIZATION_INVEST_COST_INITIAL + define:COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (if a province adjacent to the region is owned) to place the initial colonist. Further steps cost define:COLONIZATION_INTEREST_COST while in phase 1. In phase two, each point of investment cost define:COLONIZATION_INFLUENCE_COST up to the fourth point. After reaching the fourth point, further points cost define:COLONIZATION_EXTRA_GUARD_COST x (points - 4) + define:COLONIZATION_INFLUENCE_COST.

New notes on colonial points:

You have to be at least colonial rank to get any; equal sum of:
+ colonial points from tech
+ colonial points from navy = 
(nominal colonial points from ships x (1.0 - percent over naval limits) + colonial points from naval bases) x define:COLONIAL_POINTS_FROM_SUPPLY_FACTOR
+ colonial points from history files (we ignore this)

Colonial points used:

+ total amount invested in colonization (the race stage, not colony states)
+ for each colonial province COLONIZATION_COLONY_PROVINCE_MAINTAINANCE
+ infrastructure value of the province x COLONIZATION_COLONY_RAILWAY_MAINTAINANCE
+ COLONIZATION_COLONY_INDUSTRY_MAINTAINANCE per factory in a colony (???)
rounded to the nearest integer

## Events

### Yearly and quarterly pulse

I don't know if any mods make use of them (I assume not, at the moment), but a country will see a quarterly pulse event every 1 to 90 days (uniformly randomly distributed in that range) and a yearly pulse events every 1 to 365 days (also uniformly distributed). Yes, this means that on average you would see more than 4 quarterly events per year and more than 1 yearly event per year. Look, I don't make the rules, I just report them.

### Random events

Some events are designated as being able to be triggered at most once. Obviously, this means that they can happen at most once. Also note that some events have an immediate effect, which occurs when the event is triggered, before any option can be chosen.

Which events are possible at all seems to be tested less frequently than testing for the occurrence of an event.

Province events and national events are checked on alternating days.

For national events: the base factor (scaled to days) is multiplied with all modifiers that hold. If the value is non positive, we take the probability of the event occurring as 0.000001. If the value is less than 0.001, the event is guaranteed to happen. Otherwise, the probability is the multiplicative inverse of the value.

The probabilities for province events are calculated in the same way, except that they are twice as likely to happen.

## Other concepts

- Prestige: generally positive prestige gains are increased by a factor of (technology-prestige-modifier + 1)
- Prestige: a nation with a prestige modifier gains that amount of prestige per month (on the 1st)
- Infamy: a nation with a badboy modifier gains that amount of infamy per month
- War exhaustion: a nation with a war exhaustion modifier gains that much at the start of the month, and every month its war exhaustion is capped to its maximum-war-exhaustion modifier at most.
- Monthly relations adjustment = +0.25 for subjects/overlords, -0.01 for being at war, +0.05 if adjacent and both are at peace, +0.025 for having military access, -0.15 for being able to use a CB against each other (-0.30 if it goes both ways)
- Once relations are at 100, monthly increases cannot take them higher
- Except: ai overlords and vassals increase their relation by +2 a month up to 200.
- Monthly plurality increase: plurality increases by average consciousness / 45 per month.
- Monthly diplo-points: (1 + national-modifier-to-diplo-points + diplo-points-from-technology) x define:BASE_MONTHLY_DIPLOPOINTS (to a maximum of 9)
- Monthly suppression point gain: define:SUPPRESS_BUREAUCRAT_FACTOR x fraction-of-population-that-are-bureaucrats x define:SUPPRESSION_POINTS_GAIN_BASE x (suppression-points-from-technology + national-suppression-points-modifier + 1) (to a maximum of define:MAX_SUPPRESSION)
- overseas province: not on same continent as the nation's capital AND not connected by land to the capital
- province value type A: 1 (if unowned or colonial), otherwise: (1 + fort level + naval base level + number of factories in state (if capital province of the state state)) x 2 (if non-overseas core) x 3 (if it is the national capital) -- these values are used to determine the relative worth of provinces (I believe) as a proportion of the total type A valuation of all provinces owned by the country.
- a port: I believe that any coastal province is considered to have a port, regardless of whether it has a naval base
- revanchism: you get one point per unowned core if your primary culture is the dominant culture (culture with the most population) in the province, 0.25 points if it is not the dominant culture, and then that total is divided by the total number of your cores to get your revanchism percentage
- national administrative efficiency: = (the-nation's-national-administrative-efficiency-modifier + efficiency-modifier-from-technologies + 1) x number-of-non-colonial-bureaucrat-population / (total-non-colonial-population x (sum-of-the-administrative_multiplier-for-social-issues-marked-as-being-administrative x define:BUREAUCRACY_PERCENTAGE_INCREMENT + define:MAX_BUREAUCRACY_PERCENTAGE) )
- tariff efficiency: define:BASE_TARIFF_EFFICIENCY + national-modifier-to-tariff-efficiency + administrative-efficiency, limited to at most 1.0
- number of national focuses: the lesser of total-accepted-and-primary-culture-population / define:NATIONAL_FOCUS_DIVIDER and 1 + the number of national focuses provided by technology.
- province nationalism decreases by 0.083 (1/12) per month.

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

## Commands

This section describes all the actions that a player can take that affect the state of the game (i.e. anything that isn't just user interface navigation).

### Set national focus

This includes both setting the national focus in a state you own as well as setting the flashpoint focus in a state owned by another nation.

#### Conditions

If you are setting a focus for one of your states, then it must not be the flashpoint focus, the ideology must be available if it is a party loyalty focus (enabled and not restricted to civs only if you are an unciv), and either there must be a focus already in the state or fewer than your maximum number of available focus must be set in other states.

If it is the flashpoint focus, the state must not be owned by you, you must be less than colonial rank, you must have a core in that state, the owner must not accept your primary culture, your tag must be releasable, another nation must not already have a flashpoint focus there, and either you have fewer than your maximum number of focuses already active *or* you have a flashpoint focus active somewhere else (setting a flashpoint focus in one state will cancel any flashpoint focuses you have set in other states).

#### Effect

Changes the national focus active in the state

### Start research

This starts researching an specific technology.

#### Conditions

Nations can only start researching technologies if, they are not uncivilized, the corresponding date that the technology is activated at is already past by, and all the previous techs (if any) of the same folder are already researched fully. And the technology isn't already being researched.

#### Effect

Sets the current research of the country to the specified technology.

### War subsidies

This helps funding the war efforts of the target country, requiring no agreement between each.

#### Conditions

Can only perform if, the nations are not at war, the nation isn't already being given war subsidies, and there is defines:WARSUBSIDY_DIPLOMATIC_COST diplomatic points available. And the target isn't equal to the sender.

#### Effect

The sender will give the target `defines:WARSUBSIDIES_PERCENT x total-expenses-of-target` every tick.

### Increase relations

This increases relations between the two countries and requires no agreement between each.

#### Conditions

Can only perform if, the nations are not at war, the relation value isn't maxxed out at 200, and has defines:INCREASERELATION_DIPLOMATIC_COST diplomatic points. And the target can't be the same as the sender.

#### Effect

Increase relations value by the value of defines:INCREASERELATION_RELATION_ON_ACCEPT (normally set at 100) and decreases diplomatic points by defines:INCREASERELATION_DIPLOMATIC_COST.

### Decrease relations

This decreases relations between the two countries and requires no agreement between each.

#### Conditions

Can only perform if, the nations are not at war, the relation value isn't maxxed out at -200, and has defines:DECREASERELATION_DIPLOMATIC_COST diplomatic points. And the target can't be the same as the sender.

#### Effect

Increase relations value by the value of defines:DECREASERELATION_RELATION_ON_ACCEPT (normally set at -20) and decreases diplomatic points by defines:DECREASERELATION_DIPLOMATIC_COST.

### Conquering a province

Strictly speaking, this is not a command that we would expect the ui to ever send directly (except maybe via the console). However, it can be thought of as a component of the more complex commands that will eventually execute a peace deal, for example.

#### Conditions

Depends on the context it is generated in

#### Effect

In addition to transferring province ownership: (TODO: prevent more than one naval base per state)
- All pops in the province lose all their savings
- If the province is not a core of the new owner and is not a colonial province (prior to conquest), any pops that are not of an accepted or primary culture get define:MIL_HIT_FROM_CONQUEST militancy
- Provinces conquered from an unciv by a civ become colonial
- The conqueror may gain research points:
First, figure out how many research points the pops in the province would generate as if they were a tiny nation (i.e. for each pop type that generates research points, multiply that number by the fraction of the population it is compared to its optimal fraction (capped at one) and sum them all together). Then multiply that value by (1.0 + national modifier to research points modifier + tech increase research modifier). That value is then multiplied by define:RESEARCH_POINTS_ON_CONQUER_MULT and added to the conquering nation's research points. Ok, so what about the nations research points on conquer modifier?? Yeah, that appears to be bugged. The nation gets research points only if that multiplier is positive, but otherwise it doesn't affect the result.
- The province gets nationalism equal to define:YEARS_OF_NATIONALISM
- Pops leave any movements / rebellions
- Timed modifiers are removed; constructions are canceled
- When new states are created by conquest, the nation gets an `on_state_conquest` event

### Create a military leader (either general or admiral)

#### Conditions

The nation must have define:LEADER_RECRUIT_COST leadership points available.

#### Effect

The same as for automatic leader creation, except that you get to choose the type. The nation loses define: LEADER_RECRUIT_COST leadership points. (There is also a MILITARY_CREATE_GENERAL message)

### Start building a province building (naval base, fort, railroad)

Irrelevant note: it is POP_BUILD_FACTORY that allows pops to build railroads after all

#### Conditions

The nation must have the tech level required to build up to the new level in the target province. There must be no existing construction project for that building. The province must be controlled by its owner. The province must not be under siege. If not owned by the nation doing the building, it must be a railroad, the target nation must allow foreign investment, the nation doing the investing must be a great power while the target is not a great power, and the nation doing the investing must not be at war with the target nation. The nation being invested in must be civilized.

For naval bases: only one per state (check ongoing constructions in state too), and only on the coast.

For railroads: nations must have RULE_BUILD_RAILROAD to start a project.

#### Effect

Start the building construction project. If the province is in a foreign nation, get foreign investment credit equal to the projected total cost.

### Start building or upgrading a factory

#### Conditions

The nation must have the rule set to allow building / upgrading if this is a domestic target.

For foreign investment: the target nation must allow foreign investment, the nation doing the investing must be a great power while the target is not a great power, and the nation doing the investing must not be at war with the target nation. The nation being invested in must be civilized.

The factory building must be unlocked by the nation.
Factories cannot be built in a colonial state.
Coastal factories can only be built in coastal states.
Can't have duplicate factories (2 factories of the same type).

For new factories: no more than 7 existing + under construction new factories must be present.
For upgrades: no upgrading past max level.

#### Effect

Start the building construction project. If the province is in a foreign nation, get foreign investment credit equal to the projected total cost.

### Destroy Factory

Complication: factory must be identified by its location and type, since other factory commands may be in flight

#### Conditions

The nation must have the appropriate rule to allow the destruction set.

#### Effect

Factory goes away

### Change factory settings

Change the hiring priority or subsidized status of a factory

#### Conditions

Relevant national rules

#### Effect

Status changes

### Start unit construction

#### Conditions

The province must be owned and controlled by the building nation, without an ongoing siege.
The unit type must be available from start / unlocked by the nation

Land units:

Each soldier pop can only support so many regiments (including under construction and rebel regiments)
If the unit is culturally restricted, there must be an available primary culture/accepted culture soldier pop with space

Naval units:

The province must be coastal
The province must have a naval base of sufficient level, depending on the unit type
The province may not be overseas for some unit types
Some units have a maximum number per port where they can built that must be respected

#### Effect

Starts condition

### Cancel unit construction

#### Conditions

Must be the owner of the province where the unit is being built

#### Effect

Cancels construction

### Release a nation as a vassal

#### Conditions

Must not be at war and the country being released must not already exist. The associated tag must be releasable, and you must own some cores belonging to that tag.

#### Effect

Release the nation from any cores owned by the sender of the command. The command sender loses all cores on those provinces. The command sender gains define:RELEASE_NATION_PRESTIGE. The command sender gains define:RELEASE_NATION_INFAMY. The released nation has the same government as the releaser (this may force changing the ruling party, and thus changing the active party issues).

### Switch nations

#### Conditions

Target tag mustn't be controlled by another player. The tag should exist.

#### Effect

Switches nation to tag - no effect on AI controlled nations.

### Change budget settings

#### Conditions

No setting can be brought outside the permissible range

#### Effect

Settings are changed (by being clamped to the permissible range).

### Change stockpile setting

Change whether the stockpile is filling, and what its target amount its

### Start election

#### Conditions

Elections must be possible in the form of government. No election may be currently ongoing.

#### Effect

Starts an election

### Set influence priority

#### Conditions

The source must be a great power, while the target must not be a great power.

#### Effect

Sets the influence priority from 0 to 3

### Discredit

#### Conditions

The source must be a great power. The source must have define:DISCREDIT_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To discredit a nation, you must have an opinion of at least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation than the nation you are discrediting does.

#### Effect

A nation is discredited for define:DISCREDIT_DAYS. Being discredited twice does not add these durations together; it just resets the timer from the current day. Discrediting a nation "increases" your relationship with them by define:DISCREDIT_RELATION_ON_ACCEPT. Discrediting costs define:DISCREDIT_INFLUENCE_COST influence points. 

### Expel Advisors

#### Conditions

The source must be a great power. The source must have define:EXPELADVISORS_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To expel advisors you must have at least neutral opinion with the influenced nation and an equal or better opinion level than that of the nation you are expelling.

#### Effect

Expelling a nation's advisors "increases" your relationship with them by define:EXPELADVISORS_RELATION_ON_ACCEPT. This action costs define:EXPELADVISORS_INFLUENCE_COST influence points. Being expelled cancels any ongoing discredit effect. Being expelled reduces your influence to zero. 

### Ban embassy

#### Conditions

The source must be a great power. The source must have define:BANEMBASSY_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To ban a nation you must be at least friendly with the influenced nation and have an equal or better opinion level than that of the nation you are expelling.

#### Effect

Banning a nation's embassy "increases" your relationship with them by define:BANEMBASSY_RELATION_ON_ACCEPT. This action costs define:BANEMBASSY_INFLUENCE_COST influence points. The ban embassy effect lasts for define:BANEMBASSY_DAYS. If you are already banned, being banned again simply restarts the timer. Being banned cancels out any ongoing discredit effect. 

### Increase opinion:

#### Conditions

The source must be a great power. The source must have define:INCREASEOPINION_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. Your current opinion must be less than friendly

#### Effect

Increasing the opinion of a nation costs define:INCREASEOPINION_INFLUENCE_COST influence points. Opinion can be increased to a maximum of friendly.
	
### Decrease opinion:

#### Conditions

The source must be a great power. The source must have define:DECREASEOPINION_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. Decreasing the opinion of another nation requires that you have an opinion of at least "opposed" with the influenced nation and you must have a an equal or better opinion level with the influenced nation than the nation you are lowering their opinion of does. The secondary target must neither have the influenced nation in sphere nor may it already be at hostile opinion with them.

#### Effect

Decreasing the opinion of a nation "increases" your relationship with them by define:DECREASEOPINION_RELATION_ON_ACCEPT. This actions costs define:DECREASEOPINION_INFLUENCE_COST influence points. Opinion of the influenced nation of the secondary target decreases by one step.

### Add to sphere

#### Conditions

The source must be a great power. The source must have define:ADDTOSPHERE_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. The nation must have a friendly opinion of you and my not be in the sphere of another nation.

#### Effect

Adding a nation to your sphere costs define:ADDTOSPHERE_INFLUENCE_COST influence points.

### Remove from sphere

#### Conditions

The source must be a great power. The source must have define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. The source may not be currently banned with the direct target or currently on the opposite side of a war involving them. Only a great power can be a secondary target for this action. To preform this action you must have an opinion level of friendly with the nation you are removing from a sphere.

#### Effect

Removing a nation from a sphere costs define:REMOVEFROMSPHERE_INFLUENCE_COST influence points. If you remove a nation from your own sphere you lose define:REMOVEFROMSPHERE_PRESTIGE_COST prestige and gain define:REMOVEFROMSPHERE_INFAMY_COST infamy. Removing a nation from the sphere of another nation "increases" your relationship with the former sphere leader by define:REMOVEFROMSPHERE_RELATION_ON_ACCEPT points. The removed nation then becomes friendly with its former sphere leader.

### Make colony into state

#### Conditions

Must not be at war. State must be colonial. Primary and accepted bureaucrat pops must make up at least define:STATE_CREATION_ADMIN_LIMIT percent of the population in the state. Must have enough free colonial points. The point requirement is: define:COLONIZATION_CREATE_STATE_COST x number of provinces x 1v(either distance to capital / COLONIZATION_COLONY_STATE_DISTANCE or 0 if it has a land connection to the capital).

#### Effect

Provinces in the state stop being colonial.
Gain define:COLONY_TO_STATE_PRESTIGE_GAIN x (1.0 + colony-prestige-from-tech) x (1.0 + prestige-from-tech)
All timed modifiers active for provinces in the state expire
An event from `on_colony_to_state` happens (with the state in scope)
An event from `on_colony_to_state_free_slaves` happens (with the state in scope)
Update is colonial nation

### Invest in colony

#### Conditions

Must have an uncolonized province
Must be in phase 0 or 1 or 2
There cannot be an ongoing colonial crisis for the state

Investing in a colony costs define:COLONIZATION_INVEST_COST_INITIAL + define:COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER (if a province adjacent to the region is owned) to place the initial colonist. Further steps cost define:COLONIZATION_INTEREST_COST while in phase 1. In phase two, each point of investment cost define:COLONIZATION_INFLUENCE_COST up to the fourth point. After reaching the fourth point, further points cost define:COLONIZATION_EXTRA_GUARD_COST x (points - 4) + define:COLONIZATION_INFLUENCE_COST.

You can invest colonially in a region if there are fewer than 4 other colonists there (or you already have a colonist there). You must also have sufficient liferating tech. Specifically, you must have colonial life rating points from technology + define:COLONIAL_LIFERATING less than or equal to the *greatest* life rating of an unowned province in the state. Your country must be of define:COLONIAL_RANK or less.

If you haven't yet put a colonist into the region, you must be in range of the region. Any region adjacent to your country or to one of your vassals or substates is considered to be in range. Otherwise it must be in range of one of your naval bases, with the range depending on the colonial range value provided by the naval base building x the level of the naval base.

If you have put a colonist in the region, and colonization is in phase 1 or 2, you can invest if it has been at least define:COLONIZATION_DAYS_BETWEEN_INVESTMENT since your last investment, and you have enough free colonial points.

#### Effect

Adds a point to your colonization progress, resets the time since last investment. When someone reaches define:COLONIZATION_INTEREST_LEAD points, and it is phase 1, it moves into phase 2, kicking out all but the second-most colonizer (in terms of points). In phase 2 if you get define:COLONIZATION_INFLUENCE_LEAD points ahead of the other colonizer, the other colonizer is kicked out and the phase moves to 3.

### Finish colonization

(i.e. turn a colonial investment into a colonial state)

#### Conditions

Must have at least defines:COLONIZATION_CREATE_PROTECTORATE_COST free colonial points. Must be in colonial stage 3 and have a colonist present.

#### Effects

Gain define:COLONY_FORMED_PRESTIGE x (tech prestige modifier + 1) prestige. Makes unowned provinces into a colonial state.

### Intervene in war

#### Conditions

Standard war-joining conditions: can't join if you are already at war against any attacker or defender. Can't join a civil war. Can't join a war against your sphere leader or overlord (doesn't apply here obviously). Can't join a crisis war prior to great wars being invented (i.e. you have to be in the crisis). Can't join as an attacker against someone you have a truce with.

Must be a great power. Must not be involved in or interested in a crisis. Must be at least define:MIN_MONTHS_TO_INTERVENE since the war started.

If it is not a great war:
Must be at least friendly with the primary defender. May only join on the defender's side. Must not be in a crisis war. Defenders must either have no wargoals or only status quo. Primary defender must be at defines:MIN_WARSCORE_TO_INTERVENE or less. Note, you *can* intervene in a civil war, despite not being able to be called into one normally.

If the war is a great war:
It is then possible to join the attacking side as well.
Must have define:GW_INTERVENE_MIN_RELATIONS with the primary defender/attacker to intervene, must have at most define:GW_INTERVENE_MAX_EXHAUSTION war exhaustion.
Can't join if any nation in your sphere is on the other side
Can't join if you are allied to any allied to any nation on the other side
Can't join if you have units within a nation on the other side

#### Effect

Join the war. Adds a status quo wargoal if it is not a great war and one isn't already present

### Suppress movement

#### Conditions

Suppression point cost:
Depends on whether define:POPULATION_SUPPRESSION_FACTOR is non zero. If it is zero, suppression costs their effective radicalism + 1. If it is non zero, then the cost is the greater of that value and the movements effective radicalism x the movement's support / define:POPULATION_SUPPRESSION_FACTOR

#### Effect

The movement's radicalism is permanently increased by define:SUPPRESSION_RADICALISM_HIT. The consciousness of each pop belonging to the movement is set to 1 and then the pops are removed from the movement.

### Become civilized

#### Conditions

Civilization progress must be at 100% or more

#### Effect

The nation gains technologies. Specifically take the fraction of military reforms (for land and naval) or econ reforms (otherwise) applied, clamped to the defines:UNCIV_TECH_SPREAD_MIN and defines:UNCIV_TECH_SPREAD_MAX values, and multiply how far the sphere leader (or first GP) is down each tech column, rounded up, to give unciv nations their techs when they westernize.
The nation gets an `on_civilize` event.
Political and social reforms: First setting in all categories?

### Become interested in a crisis

#### Conditions

Not already interested in the crisis. Is a great power. Not at war. The crisis must have already gotten its initial backers.

#### Effect

Exactly what you expect

### Join a side in a crisis

#### Conditions

Must not be involved in the crisis already. Must be interested in the crisis. Must be a great power. Must not be disarmed. The crisis must have already gotten its initial backers.

#### Effect

Add to crisis side

### Accept/reject a crisis backing offer

#### Conditions

Must have a pending offer

#### Effect

Join crisis as primary attacker or defender

### Change Stockpile settings

#### Conditions

None

#### Effect

The obvious

### Change ruling party

#### Conditions

The ideology of the ruling party must be permitted by the government form. There can't be an ongoing election. It can't be the current ruling party. The government must allow the player to set the ruling party. The ruling party can manually be changed at most once per year.

#### Effect

The ruling party is changed. The party issues and rules are updated (note that this may further require things such as turning subsidies off).
If the new party ideology is *not* the same as the old one: all pops gain ((total number of political and social reforms over baseline) x 0.01 + 1.0) x define:RULING_PARTY_ANGRY_CHANGE x pop support of the old ideology militancy
all pops gain define:RULING_PARTY_HAPPY_CHANGE x pop support of the new ideology militancy
The same is also done for all party issues that differ between the two.

### Select political / social / military / economic reform

#### Conditions

The conditions for when these reforms can be selected has been described elsewhere in this document.

#### Effects

For political and social based reforms:
- Every issue-based movement with greater popular support than the movement supporting the given issue (if there is such a movement; all movements if there is no such movement) has its radicalism increased by 3v(support-of-that-movement /  support-of-movement-behind-issue (or 1 if there is no such movement) - 1.0) x defines:WRONG_REFORM_RADICAL_IMPACT.
- Each pop in the nation gains defines:CON_REFORM_IMPACT x pop support of the issue consciousness
- For every ideology, the pop gains defines:MIL_REFORM_IMPACT x pop-support-for-that-ideology x ideology's support for doing the opposite of the reform (calculated in the same way as determining when the upper house will support the reform or repeal) militancy
- If the pop is part of an movement for some other issue (or for independence), it gains defines:WRONG_REFORM_MILITANCY_IMPACT militancy. All other pops lose defines:WRONG_REFORM_MILITANCY_IMPACT militancy.

For military/economic reforms:
- Run the `on_execute` member
- Subtract research points (see discussion of when the reform is possible for how many)

In general:
- Increase the share of conservatives in the upper house by defines:CONSERVATIVE_INCREASE_AFTER_REFORM (and then normalize again)
- If slavery is forbidden (rule slavery_allowed is false), remove all slave states and free all slaves.
- Movements may now be invalid and may need to be removed (can the normal update handle this?)

### Take decision

#### Conditions

The allow and possible trigger conditions must be satisfied

#### Effect

The decision is taken (its effect runs)

### Make event choice

For implementation reasons, there are going to be four versions of this command, corresponding to the four types of events. You must populate the command with the exact values from the event you want to make a choice for or it will be ignored.

#### Conditions

None

#### Effect

Executes the effect of the choice. Internally, removes the event from the pending queue.

### Ask for military access

(implicitly: there must also be commands to accept or decline an access request)

#### Conditions

Must have defines:ASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must not be at war against each other. Must not already have military access.

#### Effects

Relations increased by defines:ASKMILACCESS_RELATION_ON_ACCEPT if it is accepted, and by defines:ASKMILACCESS_RELATION_ON_DECLINE if it is declined.

### Cancel military access

#### Conditions

Must have defines:CANCELASKMILACCESS_DIPLOMATIC_COST diplomatic points. Must have military access.

#### Effects

Ends access.

### Ask for an alliance

(implicitly: there must also be commands to accept or decline an access request)

#### Conditions

Must not have an alliance. Must not be in a war against each other. Costs defines:ALLIANCE_DIPLOMATIC_COST diplomatic points. Great powers may not form an alliance while there is an active crisis. Vassals and substates may only form an alliance with their overlords.

#### Effects

Increases relations by defines:ALLIANCE_RELATION_ON_ACCEPT on acceptance or by defines:ALLIANCE_RELATION_ON_DECLINE if it is declined. Creates an alliance.

### End alliance

#### Conditions

Must have an alliance. Cannot be in a war on the same side. Costs defines:CANCELALLIANCE_DIPLOMATIC_COST diplomatic points.

#### Effects

Ends alliance. Relations increase by defines:CANCELALLIANCE_RELATION_ON_ACCEPT

### Call ally to war

(implicitly: there must also be commands to accept or decline an access request)

#### Conditions

Requires defines:CALLALLY_DIPLOMATIC_COST diplomatic points. The ally must be able to join the war. The calling nation must be in the war.

#### Effect

Ally joins war (or not). Relations increase by defines:CALLALLY_RELATION_ON_ACCEPT or defines:CALLALLY_RELATION_ON_DECLINE. For a defensive call, declining causes the alliance to break.

### Begin CB fabrication

#### Conditions

Can't fabricate on someone you are at war with. Can't fabricate on anyone except your overlord if you are a vassal. Requires defines:MAKE_CB_DIPLOMATIC_COST diplomatic points. Can't fabricate on your sphere members. Cb must be fabricate-able.

#### Effect

Fabrication starts


TODO
Crisis invite with wargoal (+ accept, reject)
Crisis propose solution (+ accept, reject)
Crisis back down
Add CB to war
Declare war
Make peace offer (+ accept, reject)
Assign leader to unit
Move unit
Embark/disembark
Split unit
Take command of vassal units / give up command
Set hunt rebels on/off