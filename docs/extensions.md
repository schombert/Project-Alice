## Modding extensions

This document covers modding extensions that have been added to Project Alice in addition to what Victoria 2 provided. We are generally open to adding more modding extensions by request, under the condition that you actually plan to use them. Generally, we are not interested in speculatively adding extensions just because they might be useful.

### Trigger Condition Parsing

In Victoria 2, a trigger condition such as as `prestige = 5` will trigger when the nation's prestige is greater than or equal to 5. If you want to test whether the value is less than 5, you would have to bury it inside a `NOT` scope. And testing for exact equality would be even more complicated. To simplify things, we support replacing the `=` with one of the following tokens: `==`, `!=`, `<`, `>`, `<=`, `>=`. `==` tests for exact equality, `!=` for inequality, and the rest have their ordinary meanings. We also support replacing `=` with `!=` in most situations. For example, `tag != USA` is the same as `NOT = { tag = USA }`.

### New effects

- `increment_variable = ...`: Shorthand to increment by 1
- `decrement_variable = ...`: Shorthand to decrement by 1
- `set_variable_to_zero = ...`: Shorthand to set a variable to 0
- `ruling_party_ideology = THIS/FROM`: Appoints the ruling party with an ideology of `THIS` or `FROM`
- `add_accepted_culture = culture/THIS/FROM`: Now with `THIS/FROM` adds the PRIMARY culture of `THIS/FROM` to the nation in scope
- `add_accepted_culture = this_union/from_union`: Adds the culture union of the primary culture of `THIS/FROM` as accepted to the nation in scope
- `kill_leader = "name of leader"` -- kills a leader (admiral or general) belonging to the nation in scope with the given name. Note that this will only reliably function if you have explicitly created a leader with that name via effect or via definition in the history files.
- `annex_to = null` -- this turns all the provinces owned by the nation in scope into unowned provinces (which defeats the nation, liberates its puppets, etc).
- `secede_province = null` -- turns the province in scope into an unowned province. This is to replace some of the tricks mods did to turn provinces unowned, such as seceding them to nonexistent tags, etc
- `random_greater_power = { ... }`: Like `any_greater_power`, but only one random great power is scoped.
- `any_empty_neighbor_province = { ... }`: Like `random_empty_neighbor_province`, but all of the empty adjacent provinces are scoped.

### New Trigger Conditions

- `test = name_of_scripted_trigger` -- evaluates a scripted trigger (see below)
- `any_country = { ... }` -- tests whether any existent country satisfies the conditions given in `...`. This is essentially just mirroring how the existing `any_country` effect scope works but for trigger conditions.
- `every_country = { ... }`: Like `any_country`, but applies to EVERY country.

### Lambda events

Define anonymous lambda events within events, no need to give them an ID.

```
country_event = {
	id = 1000
	# etc...
	option = {
		name = "Option A"
		lambda_country_event = {
			#No need to define ID
			option = {
				name = "Option A"
				#...
			}
			option = {
				name = "Option B"
				#...
			}
		}
	}
	option = {
		name = "Option B"
		lambda_country_event = {
			#No need to define ID
			option = {
				name = "Option A"
				#...
			}
			option = {
				name = "Option B"
				#...
			}
		}
	}
}
```

This allows essentially to "inline" anonymous events, for example when doing a FAQ section event, or doing a long event chain for setup or "LARP choices" purpouses.

These events can't be triggered with the `event` command, nor can they be referenced by other events, they never will trigger on their own and will always be treated as if they were `is_triggered_only = yes`.

- `lambda_country_event`: Main slot is a `country`, inherits `FROM` and `THIS` slot types.
- `lambda_province_event`: Main slot is a `province`, inherits `FROM` and `THIS` slot types.

### Scripted Triggers

We have added the ability to write complicated trigger conditions once and then to use that same condition multiple times inside other trigger conditions. To use these "scripted triggers," you must add a `scripted triggers` directory to your mod (at the top level of your mod, so the path will look like: `...\mod name\scripted triggers`). Place any number of `.txt` files inside this directory, each of which can contain any number of scripted triggers.

Each scripted trigger in such a file should look like:
```
name_of_condition = {
	main_parameter = nation/state/province/pop/rebel/none
	this_parameter = ...
	from_parameter = ...
	
	... contents of the trigger condition go here...
}
```
You should add an entry for `name_of_condition` to your csv file, since it will appear in tool tips when you use the scripted trigger. The three parameter entries define the content that the trigger will be tested upon (you must pick *one* of the possibilities). A scripted trigger can only be used in a context where the various parameters either match or where the parameter of the scripted trigger is `none` (you can think of `none` as matching everything). For example, if the main parameter is `province` then you could only evaluate the scripted trigger when there was a province in scope. The `this_parameter` and `from_parameter` are optional and may be omitted (they default to `none`). You can define more than one scripted trigger with the same name, so long as they have different parameter types. When using a scripted trigger with multiple versions, the one that fits the current parameters will automatically be selected.

To use a scripted trigger simply add `test = name_of_condition` in a trigger wherever you want to evaluate your scripted trigger and it will work as if you had copied its content into that location. I advise you not to use scripted triggers from within other scripted triggers while defining them. You can safely refer to scripted triggers earlier in the same file, but doing so across files will put you at the mercy of the file loading order.

### If/Else

If and Else statments are now provided to avoid code duplication and make your life easier. A `else` with a limit is equivalent to an `else_if` with a limit, and a `else_if` without a limit is equal to a `else` without a limit. In other words, `else` and `else_if` are essentially synonyms, provided for code clarity.

For example:
```
if = { #run if limit is true
  limit = { ... }
  ...
} else_if = { #run only if the limit above is false, and this limit is true
  limit = { ... }
  ...
} else = { #only if both cases are not fullfilled AND the limit is true
  limit = { ... }
  ...
}
```

No limit specified equals to an `always = yes`.

Additionally, the order of execution for `if` and `else`/`else_if` statments allows for nested code:

```
if = {
	limit = { a = yes }
		money = 1
	if = {
		limit = { b = yes }
		money = 50
	} else {
		money = 100
	}
}
```
This is equivalent to:
```
if = {
	limit = { a = yes b = yes }
	money = 1
	money = 50
} else_if {
	limit = { a = yes }
	money = 1
	money = 100
}
```

Additionally, negation of statments are implicit, in other words:
```
if = {
	limit = { test == yes }
} else {
	limit = { test != yes }
}
```

Is implicitly assumed for every `else` after a `if`, this means that an explicit negation (rewriting everything inside a big NOT statment) is not required for `else` statments, as they now logically are tied to all stamtents beforehand being false, and the statment of it's own limit being true.

An issue which might exist due to the volatility of the syntax could be:
```
else_if = {
  limit = { ... }
} if = {
  limit = { ... }
}
```

The behaviour of this statment is that, since there is no preceding `if` before the `else_if`, the `else_if` will be able to run as if it was chained with an `if` that evaluated to false, in the case of it's limit evaluating to true, then it will run its own effect. However, the other `if` statment will run regardless of the previous expression.

As the lexicographical order of the statments are sequential, this is, every `else_if` and `else` must be preceded by an `if` statment, otherwise they will be chained to the nearest *preceding* `if` statment before them for their lexicographical evaluation, otherwise they will act as an `if` in itself if none is present.

```
else_if = {
  limit = { ... }
} else_if = {
  limit = { ... }
}
```
These `else_if` statments are chained together, if the first runs, the second will not, and viceversa. If no preceding `if` exists before them, the first `else_if` takes the role of the `if` statment.

### Abbreviated `.gui` syntax
 
`size = { x = 5 y = 10 }` can be written as `size = { 5 10 }`, as can most places expecting an x and y pair.
Additionally, `maxwidth = 5` and `maxheight = 10` can be written as `maxsize = { 5 10 }`

### .gui layout extensions

Laying out elements in the GUI can be a tedious process, while a WYSWYG editor would be ideal, it's not currently available at the moment.

However, the following new extensions will make GUI editing way less painful:

- `add_size = { x y }`: Adds the specified amount to the current `size`
- `add_position = { x y }`: Adds the specified amount to the current `position`
- `table_layout = { x y }`: Where `x` is the column and `y` is the row, this basically translates to `position.x = column * size.x`, and `position.y = row * size.y`. Useful for laying out elements in a table-like way


### New defines

Alice adds a handful of new defines:

- `factories_per_state`: Factories allowed per state, default 8
- `alice_speed_1`: Speed 1 in miliseconds
- `alice_speed_2`: Same as above but with speed 2
- `alice_speed_3`: Same as above but with speed 3
- `alice_speed_4`: Same as above but with speed 4
- `alice_speed_5`: Same as above but with speed 5
- `alice_ai_gather_radius`: Radius AI will use to gather nearby armies to make deathstacks
- `alice_ai_threat_radius`: Radius AI will scan for threats
- `alice_ai_threat_overestimate`: Overestimate AI opponents (higher values leads to camping)
- `alice_ai_attack_target_radius`: Radius AI will perform attacks
- `alice_full_reinforce`: 1 = Normal vanilla behaviour, 0 = Understaffed armies are allowed
- `alice_ai_offensive_strength_overestimate`: Overestimate strength of an offensive oppontent (makes AI less aggressive)
- `alice_lf_needs_scale`: Scale multiplier for life needs
- `alice_ev_needs_scale`: Scale multiplier for everyday needs
- `alice_lx_needs_scale`: Scale multiplier for luxury needs
- `alice_max_event_iterations`: The maximun number of iterations that are possible within recursive events, by default this will be `8`, so you can only recursively fire events `8` levels deep. If modders wish to increase their "recursiveness" they may uppen this value up to whatever they wish.
- `alice_needs_scaling_factor`: Scale factor multiplier for all needs
- `alice_factory_per_level_employment`: Employment per factory level.
- `alice_domestic_investment_multiplier`: Multiplier of domestic investment.
- `alice_rgo_boost`: Boost given to RGOs (for example, 1.2 produces 120% more)
- `alice_inputs_base_factor_artisans`: Artisan input base factor + modifiers.
- `alice_output_base_factor_artisans`: See above.
- `alice_inputs_base_factor`: See above, for factories.
- `alice_rgo_overhire_multiplier`: Overhire multiplier for RGOs.
- `alice_rgo_production_scale_neg_delta`: Scale delta for RGO production.
- `alice_invest_capitalist`: % of total budget that capitalists will invest in the private investment pool
- `alice_invest_aristocrat`: See above.
- `alice_needs_lf_spend`: % of total budget dedicated to life needs
- `alice_needs_ev_spend`: See above, but everyday needs
- `alice_needs_lx_spend`: See above, but luxury needs
- `alice_sat_delay_factor`: Satisfaction delay factor
- `alice_need_drift_speed`: Drift speed of need weights for POPs

### Dense CSV pop listing

This is an extension that allows you to use CSV files instead of scripting pops manually, this allowing you to edit the pop files on a spreadsheet program, for example:

```
province-id;size;culture;religion;pop-type;rebel-faction(optional)
825;100;albanian;orthodox;clergymen;jacobin
825;150;albanian;orthodox;aristocrats;jacobin
825;744;albanian;orthodox;capitalists;jacobin
825;4019578;albanian;orthodox;farmers;jacobin
...
```

This allows for higher volume of data, while keeping it readable, editable and most importantly: able to be edited on your favourite office spreadsheet program.

Using this in your mod is simple, create a file ending with `.csv`, like, `Africa.csv`, Alice will load it *alongside* other files, even `.txt` files, if you want to mix them you absolutely can, just bear in mind that every file in the `history/pops/yyyy.mm.dd` is loaded, so be aware of that.

### Dense CSV province history

Sometimes having too many history text files for provinces can make handling files a pain, or mess up compression algorithms with having too many province files.

Redundant data is the enemy of file size, and, while most province history files don't take much space, it is still good to load them:

```
province-id;owner;controller;core;trade_goods;life_rating;colonial;slave;
142;RPL;RPL;RPL;coal;31;1;0;
```

Similar to POP CSV format. The CSV files will be loaded **first** in the history folder, this means you can mix both CSV and text files to make greater granularity. For example when multiple cores are involved.

### Country templates

This allows to remove a lot of copy-pasting for countries, and other common files.

```
template = "test.txt"
```

Where `test.txt` would be in `common/templates/test.txt`.

### New event substitutions

**Events-only:**

- `$POP$`: Total population of `THIS`.
- `$FROMCONTINENT$`: Continent of `FROM`.
- `$FROMCAPITAL$`: Capital of `FROM`.
- `$GOOD$`, `$RESOURCE$`: Good produced by province `THIS`.
- `$NUMFACTORIES$`: Factories in state `THIS`.
- `$FROMRULER$`: Equivalent to `$MONARCHTITLE$` but for `FROM`
- `$FOCUS$`, `$NF$`: National focus of state `THIS`.
- `$TEMPERATURE$`: Current temperature of crisis.
- `$TERRAIN$`: Name of the terrain of province `THIS`.
- `$FROMSTATENAME$`: State name of `FROM`.
- `$FACTORY$`: Name of the factory of `THIS` province.
- `$DATE$`: Date that the event fired on.
- `$CONTROL$`: Nation that is currently controlling `THIS` province.
- `$OWNER$`: Nation that has ownership over `THIS` province.

**Decision and events:**

- `$GOVERNMENT$`: Name of the government of `THIS`.
- `$IDEOLOGY$`: Name of the ideology of the ruling party of `THIS`.
- `$PARTY$`: Name of the party of `THIS`.
- `$INFAMY$`, `$BADBOY$`: Current infamy.
- `$SPHEREMASTER$`: Sphere master of `THIS`.
- `$OVERLORD$`: Overlord of `THIS`.
- `$NATIONALVALUE$`: Name of the national value of `THIS`.
- `$CULTURE_FIRST_NAME$`: Randomly generated first-name, of primary culture of `THIS`.
- `$CULTURE_LAST_NAME$`: Randomly generated last-name, of primary culture of `THIS`.
- `$TECH$`: Currently researched tech.
- `$NOW$`: Current date.
- `$ANYPROVINCE$`: Any (random) province owned by this nation.

Decisions now can use crisis substitutions: `$CRISISTAKER$`, `$CRISISTAKER_ADJ$`, `$CRISISATTACKER$`, `$CRISISDEFENDER$`, `$CRISISTARGET$`, `$CRISISTARGET_ADJ$` and `$CRISISAREA$` - additionally, they can use `$CULTURE$`, `$CULTURE_GROUP_UNION$`, `$UNION_ADJ$` and `$COUNTRYCULTURE$`, working in the same fashion as their event counterparts.

**New crisis substitutions:**

- `$CRISISTAKER_CAPITAL$`: Capital of liberation tag.
- `$CRISISTAKER_CONTINENT$`: Continent of liberation tag, based from capital.
- `$CRISISATTACKER_CAPITAL$`: Capital of attacker.
- `$CRISISATTACKER_CONTINENT$`: Continent of attacker, based from capital.
- `$CRISISDEFENDER_CAPITAL$`: Capital of defender.
- `$CRISISDEFENDER_CONTINENT$`: Continent of attacker, based from capital.

### Political party triggers

Now you can turn on/off political parties, aside from the usual `start_date` and `end_date`. Remember that parties can be shared between countries.

```
party = {
	name = "default_fascist_military_junta"
	start_date = 1836.1.1 #also part of trigger check
	end_date = 2000.1.1 #same here
	ideology = fascist
	#[...]
	#Example trigger!
	trigger = {
		war = yes
		nationalism_n_imperialism = 1
	}
}
```

## Government ruler-names

Now you can define ruler names for a specific nation with a specific government type, for example:

```
RUS_absolute_monarchy;The Russian Empire
RUS_absolute_monarchy_ruler;Tsar
```
