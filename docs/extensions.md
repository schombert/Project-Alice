# Modding extensions

This document covers modding extensions that have been added to Project Alice in addition to what Victoria 2 provided. We are generally open to adding more modding extensions by request, under the condition that you actually plan to use them. Generally, we are not interested in speculatively adding extensions just because they might be useful.

## Scripting

### Trigger Condition Parsing

In Victoria 2, a trigger condition such as as `prestige = 5` will trigger when the nation's prestige is greater than or equal to 5. If you want to test whether the value is less than 5, you would have to bury it inside a `NOT` scope. And testing for exact equality would be even more complicated. To simplify things, we support replacing the `=` with one of the following tokens: `==`, `!=`, `<`, `>`, `<=`, `>=`. `==` tests for exact equality, `!=` for inequality, and the rest have their ordinary meanings. We also support replacing `=` with `!=` in most situations. For example, `tag != USA` is the same as `NOT = { tag = USA }`.

Additionally, any effect scope can have a limit, in the original, `THIS`, `TAG`, `FROM`, `overlord`, `capital_scope` and `any_greater_power` couldn't reliably have a `limit = { ... }` defined within them. However Alice supports defining limits for any of them.

Also, province scopes will not crash the game, take for example:

```
252 = {
	owner = { prestige = 5 }
	owner = { badboy = 0.2 }
	secede_province = THIS
}
```

This would certainly be unthinkable, as in the original, anything after the first owner gets ignored. But not anymore.

```
ENG_52 = {
	secede_province = QQQ
	change_region_name = "Test"
	secede_province = THIS
}
```

This often caused crashes if not handled carefully (especially the seceding to a null tag part). Not anymore.

Another new feature is allowing usage of uniform `yes/no` inside triggers, in the original engine, triggers such as `unit_has_leader = yes` was the same as `unit_has_leader = no`, so it'd need to be wrapped around a `NOT = { unit_has_leader = yes }`. Not anymore.

Additionally, triggers such as technology triggers no longer suffer from having to be specified like: `nationalism_n_imperialism = 1`, they can alternatively be specified as `nationalism_n_imperialism = yes`. For ease of reading and uniformity.

### New effects

- `increment_variable = ...`: Shorthand to increment by 1
- `decrement_variable = ...`: Shorthand to decrement by 1
- `set_variable_to_zero = ...`: Shorthand to set a variable to 0
- `ruling_party_ideology = THIS/FROM`: Appoints the ruling party with an ideology of `THIS` or `FROM`
- `add_accepted_culture = THIS/FROM`: Now with `THIS/FROM` adds the PRIMARY culture of `THIS/FROM` to the nation in scope
- `add_accepted_culture = this_union/from_union`: Adds the culture union of the primary culture of `THIS/FROM` as accepted to the nation in scope
- `kill_leader = "name of leader"`: kills a leader (admiral or general) belonging to the nation in scope with the given name. Note that this will only reliably function if you have explicitly created a leader with that name via effect or via definition in the history files.
- `annex_to = null`: this turns all the provinces owned by the nation in scope into un-owned provinces (which defeats the nation, liberates its puppets, etc).
- `secede_province = null`: turns the province in scope into an un-owned province. This is to replace some of the tricks mods did to turn provinces un-owned, such as seceding them to nonexistent tags, etc
- `random_greater_power = { ... }`: Like `any_greater_power`, but only one random great power is scoped.
- `random_empty_neighbor_province` - selects random empty neighbor province in nation/province scope.
- `any_empty_neighbor_province = { ... }`: Like `random_empty_neighbor_province`, but all of the empty adjacent provinces are scoped. Can be used in province or nation scope.
- `random_neighbor_province` can be used in nation and province scope.
- `any_neighbor_province` can be used in nation and province scope.
- `change_terrain = terrain`: Changes the terrain of the province on scope, can be used on pop scopes too (will default to the location of the pop)
- `religion = religion_name`: Effect can be used in a province and pops scope to change the religion of the province/pop.
- `assimilate = yes`: Effect can be used in a pop/province/state scope to change the culture of pops in the scope to primary culture.
- `reduce_pop_abs = amount`: Reduce pop in the pop scope by absolute amount (e.g. 500 soldiers). Use with negative values to increase pop size. Use with very big number to remove pop completely.
- `set_culture = culture`: Set culture of a pop in scope to a named culture.
- `any_existing_country_except_scoped`: Same behavior of `any_country` on decisions, any existing nation except the one scoped
- `any_defined_country`: Same behavior of `any_country` on events, scope all countries even those that don't exist and includes the current country
- `random_neighbor_country`: A random neighboring country.
- `all_core = { ... } (for provinces)`: The ability to use it within provinces to scope all the cores of the province in question, for example if England and Russia have cores on Houston, then this will scope both England and Russia
- `any_core = { ... }`: Effects dont have `any_core`, but triggers do, so for consistency its supported too.
- `from_bounce = { ... }`: Forward whatever is in the current scope to be "bounced" to `FROM`, until the end of this scope
- `this_bounce = { ... }`: Same as above but with `THIS`.
- `random_by_modifier = { ... }`: See below for syntax usage
- `build_bank_in_capital = { ... }`: Invalid in vanilla, added for mods to use.
- `build_university_in_capital = { ... }`: Invalid in vanilla, added for mods to use.
- `any_owned_province = { ... }`: Providing uniformity with respect to triggers.
- `any_prov = { ... }`: Shorthand for `any_owned`.
- `any_owned_province = { ... }` : Shorthand for `any_owned`.
- `any_substate = { ... }`: For parity with triggers, scopes any substates of the country.
- `remove_country_flag = flag`: Alias of `clr_country_flag`.
- `clear_global_flag = flag`: Alias of `clr_global_flag`.
- `province_immigrator = n`: `n` can only be `1` or `-1`. This is the "province selector" used in various mods.
- `immigrator = n`: Alias of `province_immigrator`.
- `immigrator_selector = n`: Alias of `province_immigrator`.
- `if = { ... }`: See below for if-else usage.
- `else_if = { ... }`: See below.
- `else = { ... }`: See below.
- `add_country_modifier = modifier`: Shorthand for `add_country_modifier = { name = modifier duration = -1 }`
- `add_province_modifier = modifier`: Shorthand for `add_province_modifier = { name = modifier duration = -1 }`
- `masquerade_as_nation = THIS/FROM`: Use the visual flag of `THIS/FROM` as the flag of the nation, most useful for dynamic rebellions for example.
- `change_party_name = { ... }`: This effect is defined using a block with two members. The first member is `ideology = ...` where `...` is either the name of an ideology or `ruling_party`. This member determines which party the effect will apply to. If an ideology is provided, it will apply to the first active political party with that ideology (generally the "first" party is the political party defined earlier in the files). If `ruling_party` is provided, the current ruling party of the nation in scope will be affected. The second member is `name = ...`. This value is the new name that the party will be given. This effect can be used only within a national scope. Example:
```
	change_party_name = {
		ideology = conservative
		name = NEW_PARTY_NAME
	}
```
- `change_party_position = { ... }` : This effect is defined using a block with two members. The first member is `ideology = ...` where `...` is either the name of an ideology or `ruling_party`, which determines which party the effect applies to as described above. The second member is `position = ...` where `...` is one of the options for a party issue or a political or social reform. The effect will cause the political party determined by the `ideology` member to change their party position to the given `position`, if it is a party issue or it will give them a voting preference in favor of that particular reform and remove any voting preference in favor of another reform in that group (see below for more on the extension of party preferences). This effect can be used only within a national scope. Example:
```
	change_party_position = {
		ideology = ruling_party
		position = free_trade
	}
```
- `diplo_points = ...` : Will add the number to the currently stored diplomatic points of the nation in scope. (Note: so you can use a negative number to subtract points. Diplomatic points can not be reduced to less than zero.)
- `suppression_points = ...` : Will add the number to the currently stored suppression points of the nation in scope. (Note: so you can use a negative number to subtract points. Suppression points can not be reduced to less than zero.)
 
As for `build_xxx_in_capital`, the game doesn't allow custom defined buildings to be used in this mode as an effect.

The syntax for `build_bank_in_capital` and it's university counterpart is the same:

```
build_bank_in_capital = {
	in_whole_capital_state = yes/no
	limit_to_world_greatest_level = yes/no
}
```

### New trigger conditions

- `test = name_of_scripted_trigger`: evaluates a scripted trigger (see below)
- `any_country = { ... }`: tests whether any existent country satisfies the conditions given in `...`. This is essentially just mirroring how the existing `any_country` effect scope works but for trigger conditions.
- `any_known_country = { ... }`: same as above, alias
- `every_country = { ... }`: Like `any_country`, but applies to EVERY country.
- `has_global_flag = project_alice`: true if playing on Project Alice, false if not
- `all_war_countries = { ... }`: All countries we're at war with MUST fulfill the condition, as opposed to `war_countries` were only one country has to fulfill the condition
- `any_war_countries = { ... }`: Equivalent to `war_countries`
- `all_state = { ... }`: All states must fulfill condition, similar to `any_state`
- `all_substate = { ... }`: All substates must fulfill condition, similar to `any_substate`
- `all_sphere_member = { ... }`: All sphere members must fulfill condition, similar to `any_sphere_member`
- `all_pop = { ... }`: All POPs must fulfill condition, similar to `any_pop`
- `all_greater_power = { ... }`: All greater powers must fulfill condition.
- `any_owned = { ... }`: Shorthand for `any_owned_province`.
- `is_ai = yes/no`: Alias of `ai = yes/no`.
- `recently_lost_war`: Alias of `has_recently_lost_war = yes`
- `has_recent_immigration`: Alias of `has_recent_imigration` (the original is misspelled).
- `industry_score`: Alias of `industrial_score`.
- `has_accepted_culture`: Alias of `accepted_culture`.
- `number_of_cities`: Alias of `num_of_cities`.
- `disarmed`: Alias of `is_disarmed`.
- `is_vassal_of`: Alias of `vassal_of`.
- `has_national_focus = focus`: Checks if a state (or province) in scope has a focus `focus`.
- `party_name = { ... }`: This trigger condition is defined using a block with two members. The first member is `ideology = ...` where `...` is either the name of an ideology or `ruling_party`. This member determines which political party the trigger condition will test. If an ideology is provided, it will test to the first active political party with that ideology (generally the "first" party is the political party defined earlier in the files). If `ruling_party` is provided, the current ruling party of the nation in scope will be tested. The second member is `name = ...`. This value is what the name of the selected party is tested against. An exact match evaluates as `true`, while a party with any other name will yield false. This trigger condition can be used only within a national scope. Example:
```
	party_name = {
		ideology = conservative
		name = SOME_PARTY_NAME
	}
```
- `party_position = { ... }` : This trigger condition is defined using a block with two members. The first member is `ideology = ...` where `...` is either the name of an ideology or `ruling_party`, which determines which political party the trigger condition will test, as described above. The second member is `position = ...` where `...` is one of the options for a party issue or a political or social reform. The trigger condition will test whether the political party determined by the `ideology` member holds the specified party position (if a party issue) or whether it has a specific voting preference in favor of that particular reform (see below for more on the extension of party preferences). This trigger condition can be used only within a national scope. Example:
```
	party_position = {
		ideology = ruling_party
		position = free_trade
	}
```
- `diplo_points = ...` : This trigger condition is true if the nation in scope has saved diplomatic points greater than or equal to the given number.
- `suppression_points = ...` : This trigger condition is true if the nation in scope has saved suppression points greater than or equal to the given number.

### New modifiers

[Modifiers](modifiers)

### Technologies

Technologies can have their cost in `leadership_cost`.

### FROM bounce
FROM bouncing is a technique where before, modders would do:
```
country_event (THIS = USA, FROM = ---) -> fire for X
country_event (THIS = X, FROM = USA) -> Fire for Y
country_event (THIS = Y, FROM = X) -> <Dynamic effects>
```
However this can be tedious to perform, hence we added a `from_bounce` effect scope, that can be used as follows:
```
any_country = {
	#Assume FROM = ENG
	from_bounce = {
		FROM = { add_accepted_culture = THIS }
		THIS = { add_accepted_culture = FROM }
	}
	#FROM is now again = ENG
}
```

You can even nest them! Why would you want this? Up to you really.

```
any_country = {
	limit = { has_country_flag = paid_the_sultan }
	from_bounce = {
		prestige = 5
		SUL = { random_owned = { annex_to = FROM } } #Partition the sultan
		any_country = {
			#All countries at war with us will get annexed
			limit = { war_with = FROM }
			from_bounce = { inherit = FROM }
		}
	}
}
```

If the `from_bounce` has a limit, it will evaluate with FROM-relativity:
```
ENG = {
	from_bounce = {
		limit = { FROM = { tag != ENG } }
		prestige = 5
	}
}
```
The above code will never execute because it will never be true. Same applies for `this_bounce`.

Before, this would've required something akin to:
```
country_event = { #Setup THIS
	id = 50000
	option = {
		name = "Ok"
		any_country = { country_event = 50001 }
	}
}
country_event = { #Setup THIS-FROM
	id = 50001
	is_triggered_only = yes
	option = {
		name = "Ok"
		any_country = {
			limit = { NOT = { tag = FROM } }
			country_event = 50002
		}
	}
}
country_event = {
	id = 50002
	is_triggered_only = yes
	option = {
		name = "OK"
		FROM = { add_accepted_culture = THIS }
		THIS = { add_accepted_culture = FROM }
	}
}
```

### Random by modifier
The question of: How can we have probabilities which change depending on various factors?

Well the answer is `random_by_modifier`:

```
random_by_modifier = {
	chance = {
		base = 50 #50% chance
		modifier = {
			factor = 1.5 # x1.5 (50% x 1.5 = 75%)
			is_colonial = yes
		}
	}
	#50% base chance
	#75% chance if colonial
	secede_province = THIS
}
```

### Custom tooltip
```
custom_tooltip = {
	x = <variable> #$x$
	y = <variable> #$x$
	text = "localisation_key"
}
```
Alternatively:
`custom_tooltip = "localisation_key"`

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

This allows essentially to "inline" anonymous events, for example when doing a FAQ section event, or doing a long event chain for setup or "LARP choices" purposes.

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

If and Else statements are now provided to avoid code duplication and make your life easier. A `else` with a limit is equivalent to an `else_if` with a limit, and a `else_if` without a limit is equal to a `else` without a limit. In other words, `else` and `else_if` are essentially synonyms, provided for code clarity.

For example:
```
if = { #run if limit is true
  limit = { ... }
  ...
} else_if = { #run only if the limit above is false, and this limit is true
  limit = { ... }
  ...
} else = { #only if both cases are not fulfilled AND the limit is true
  limit = { ... }
  ...
}
```

No limit specified equals to an `always = yes`.

Additionally, the order of execution for `if` and `else`/`else_if` statements allows for nested code:

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

Additionally, negation of statements are implicit, in other words:
```
if = {
	limit = { test == yes }
} else {
	limit = { test != yes }
}
```

Is implicitly assumed for every `else` after a `if`, this means that an explicit negation (rewriting everything inside a big NOT statement) is not required for `else` statements, as they now logically are tied to all statements beforehand being false, and the statement of it's own limit being true.

An issue which might exist due to the volatility of the syntax could be:
```
else_if = {
  limit = { ... }
} if = {
  limit = { ... }
}
```

The behavior of this statement is that, since there is no preceding `if` before the `else_if`, the `else_if` will be able to run as if it was chained with an `if` that evaluated to false, in the case of it's limit evaluating to true, then it will run its own effect. However, the other `if` statement will run regardless of the previous expression.

As the lexicographical order of the statements are sequential, this is, every `else_if` and `else` must be preceded by an `if` statement, otherwise they will be chained to the nearest *preceding* `if` statement before them for their lexicographical evaluation, otherwise they will act as an `if` in itself if none is present.

```
else_if = {
  limit = { ... }
} else_if = {
  limit = { ... }
}
```
These `else_if` statements are chained together, if the first runs, the second will not, and vice versa. If no preceding `if` exists before them, the first `else_if` takes the role of the `if` statement.

## UI Modding

### Extension controls

It is now possible to add new controls (such as buttons, text labels, and so on) to an existing window control without modifying the original file. To add new controls to a window simply define additional top level controls like the example below:
```
	guiButtonType = {
		name = "alice_move_capital"
		extends = "province_view_header"
		position = { x= 180 y = 3 }
		quadTextureSprite = "GFX_move_capital"	
	}
```
This control will then automatically be inserted into the window named `province_view_header` when it is created. This allows you to extend a window defined in a `.gui` file without editing that file. The example above comes from `alice.gui` and is used to add a button to a window defined in `province_interface.gui` without editing that file. You mod can thus define a new `.gui` file, add new controls there, and have them show up in existing windows without interfering with another mod that also wants to add controls to that window (because now both mods don't have to make changes to the *same* `.gui` file).

### Scriptable buttons

Of course, adding new buttons wouldn't mean much if you couldn't make them do things. To allow you to add custom button effects to the game, we have introduced two new ui element types: `provinceScriptButtonType` and `nationScriptButtonType`. These buttons are defined in the same way as a `guiButtonType`, except that they can be given additional `allow` and `effect` parameters. For example:
```
	provinceScriptButtonType = {
		name = "wololo_button"
		extends = "province_view_header"
		position = { x= 146 y = 3 }
		quadTextureSprite = "GFX_wololo"
		allow = {
			owner = { tag = FROM }
		}
		effect = {
			assimilate = "yes please"
		}
	}
```

A province script button has its main and THIS slots filled with the province that the containing window is about, with FROM the player's nation. A nation script button has its main and THIS slots filled with the nation that the containing window is about, if there is one, or the player's nation if there is not, and has FROM populated with the player's nation.

The allow trigger condition is optional and is used to determine when the button is enabled. If the allow condition is omitted, the button will always be enabled.

The tooltip for these scriptable buttons will always display the relevant allow condition and the effect. You may also optionally add a custom description to the tooltip by adding a localization key that is the name of the button followed by `_tooltip`. In the case of the button above, for example, the tooltip is defined as `wololo_button_tooltip;Wololo $PROVINCE$`. The following three variables can be used in the tooltip: `$PROVINCE$`, which will resolve to the targeted province, `$NATION$`, which will resolve to the targeted nation or the owner of the targeted province, and `$PLAYER$`, which will always resolve to the player's own nation.

### Abbreviated `.gui` syntax
 
`size = { x = 5 y = 10 }` can be written as `size = { 5 10 }`, as can most places expecting an x and y pair.
Additionally, `maxwidth = 5` and `maxheight = 10` can be written as `maxsize = { 5 10 }`

### .gui layout extensions

Laying out elements in the GUI can be a tedious process, while a WYSWYG editor would be ideal, it's not currently available at the moment.

However, the following new extensions will make GUI editing way less painful:

- `add_size = { x y }`: Adds the specified amount to the current `size`
- `add_position = { x y }`: Adds the specified amount to the current `position`
- `table_layout = { x y }`: Where `x` is the column and `y` is the row, this basically translates to `position.x = column * size.x`, and `position.y = row * size.y`. Useful for laying out elements in a table-like way

## Game rules modding

### New defines

Alice adds a handful of new defines:

- `factories_per_state`: Factories allowed per state, default 8
- `alice_speed_1`: Speed 1 in milliseconds
- `alice_speed_2`: Same as above but with speed 2
- `alice_speed_3`: Same as above but with speed 3
- `alice_speed_4`: Same as above but with speed 4
- `alice_speed_5`: Same as above but with speed 5
- `alice_ai_gather_radius`: Radius AI will use to gather nearby armies to make deathstacks
- `alice_ai_threat_radius`: Radius AI will scan for threats
- `alice_ai_threat_overestimate`: Overestimate AI opponents (higher values leads to camping)
- `alice_ai_attack_target_radius`: Radius AI will perform attacks
- `alice_full_reinforce`: 1 = Normal vanilla behavior, 0 = Understaffed armies are allowed
- `alice_ai_offensive_strength_overestimate`: Overestimate strength of an offensive opponent (makes AI less aggressive)
- `alice_lf_needs_scale`: Scale multiplier for life needs
- `alice_ev_needs_scale`: Scale multiplier for everyday needs
- `alice_lx_needs_scale`: Scale multiplier for luxury needs
- `alice_max_event_iterations`: The maximum number of iterations that are possible within recursive events, by default this will be `8`, so you can only recursively fire events `8` levels deep. If modders wish to increase their "recursiveness" they may increase this value up to whatever they wish.
- `alice_needs_scaling_factor`: Scale factor multiplier for all needs
- `alice_base_rgo_employment_bonus`: Additional rgo size of the main rgo.
- `alice_base_rgo_efficiency_bonus`: Additional rgo efficiency of the main rgo.
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
- `alice_disable_divergent_any_country_effect`: On events, `any_country = { ... }` refers to any country, including nonexistent and the one scoped, in decisions, `any_country = { ... }` refers only to existing nations and not the one on scope. Set 0 to keep this behavior, set 1 to use a universal `any_country = { ... }` that scopes existing countries including the currently scoped nation.
- `alice_unciv_civ_forbid_war`: Forbids civilized nations from aiding uncivilized ones except via scripted events
- `alice_ideology_base_change_rate`: Base change rate for ideology for pops
- `alice_nurture_religion_assimilation`: Whether religion assimilation should also convert pops or not (0 = yes, 1 = no)
- `alice_surrender_on_cores_lost`: Force AI to surrender when they have lost their cores, or if they have no cores, all of their land
- `alice_artificial_gp_limitant`: Limit the number of GP allies the AI can have
- `alice_rename_dont_use_localisation`: Keys specified on `change_region_name` or `change_province_name` will be treated as CSV keys, otherwise they will define in-line.
- `alice_spherelings_only_ally_sphere`: Spherelings will only ally their spherelord.
- `alice_overseas_mil`: Militancy increase in overseas provinces when overseas maintenance is at zero.
- `alice_substate_subject_money_transfer`: Percentage [0;100] of subjects' budget revenues transferred to the overlord. Default: 30.0
- `alice_puppet_subject_money_transfer`. Percentage [0;100] of substates' budget revenues transferred to the overlord. Default: 40.0
- `alice_privateinvestment_subject_transfer`: Percentage [0;100] of subjects' and overlord's private investment pool transferred daily when no useful projects are done. Overlord distributes money to subjects and subjects contribute to the overlord. Default: 2.0
- `alice_allow_revoke_subject_states`: Allows overlord to take subjects' states raising their militancy and giving separatism. Default: 0.0
- `alice_leadership_generation_divisor`: To allow for battles to generate leadership, passive leadership generation is divided by this.
- `alice_auto_hire_generals`: set to 0.0 to disable the game hiring generals and admirals automatically.
- `alice_disallow_factories_in_colonies`: if set to 0, allows to build factories that have `can_be_built_in_colonies = yes` flag in colonies
- `alice_naval_base_to_colonial_distance_factor` (default 0.04): multiplier in the formula for colonial distance unlocked by 1 lvl of naval base.

**Crises and conferences:**
- `alice_crisis_necessary_base_win_ratio = 2.5f`: Strength Ratio at which AI submits to demands after 80 temperature
- `alice_crisis_necessary_base_fast_win_ratio = 3.5f`: Strength Ratio at which AI submits to demands before 80 temperature
- `alice_crisis_per_wg_ratio = 0.1f`: Added necessary ratio per every WG

**Military:**

These relate to the added ability for reserve regiments to reinforce while in battle.

- `alice_reg_move_to_reserve_org = 0.1f`: if a regiment's org goes below this decimal in battle, it wil be taken out from the front, and moved to reserves. Default: 1.0
- `alice_reg_move_to_reserve_str = 0.0f`: if a regiment's strength is equal to or less than this decimal in battle, it wil be taken out from the front, and moved to reserves. Default 0.0
- `alice_reg_deploy_from_reserve_org = 0.1f`: if a reserve regiments org is equal to or higher than this decimal, it can be deployed to the front, if alice_reg_deploy_from_reserve_str is also fufilled. Default 0.1
- `alice_reg_deploy_from_reserve_str = 0.0f`: if a reserve regiments strength is greater than this decimal, it can be deployed to the front, if alice_reg_deploy_from_reserve_org is also fufilled. Default 0.0


### Support for reforms based on party issues

In issues.txt you can add a `vote_modifiers = { ... }` section to any particular issue option within the party issues section. For example, one could go here:
```
party_issues = {
	trade_policy = {
		protectionism = {
			vote_modifiers = { ... }
		...
```
Inside the `vote_modifiers` section one or more modifiers can be defined for reform options belonging to social or political issues. (These modifiers share the same format with the `add_political_reform` modifiers in the ideology file. Like those modifiers, they are computed additively, and like those modifiers they contain the nation in their primary and THIS scopes.) For example, you could add something like the following (not that it makes much sense as an example, but whatever).
```
		trinket_health_care = {
			base = 1
			modifier = {
				factor = -1
				NOT = { ruling_party_ideology = fascist }
			}
		}
```
When these modifiers are present, instead of using the generic add/remove political/social reform modifiers for an ideology to determine reform support the support for a particular reform option, the game will first attempt to use the modifier(s) associated with the active issue options of the active party associated with that ideology (if there is more than one such party, one will be picked arbitrarily for this). This will completely override the generic add/remove reform modifiers, allowing a party-ideology combination to support (or oppose) particular reforms in a category even when it would otherwise generically oppose (or support) all such reforms.

### Party reform support overrides

Previously in a party definition it was possible to insert a line such as `economic_policy = laissez_faire` which would define the specific party issue that the party would have. This has been extended so that any political or social reform category and option may appear there. For example `pensions = good_pensions` in the following party description is now valid

```
party = {
	name = "ENG_liberal"
	start_date = 1830.1.1
	end_date = 1859.1.1

	ideology = liberal

	economic_policy = laissez_faire
	trade_policy = free_trade
	religious_policy = pluralism
	citizenship_policy = limited_citizenship
	war_policy = anti_military
	
	pensions = good_pensions
}
```

What this means is that the party will always support that particular reform in that category (or the closest reform if the category can only be changed step by step) and that it will not support any other reforms in that category. This will override any other preference either via ideology or the extended support weights described above. 

### Political party triggers

Now you can turn on/off political parties, aside from the usual `start_date` and `end_date`. Remember that parties can be shared between countries, and that like other template types, tags you want this to apply to need an 'template = "test.txt"'- entry in their countries file.

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

### Extra on-actions

- `on_election_started`: When an election starts
- `on_election_finished`: When an election ends

## New data formats

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

This allows for higher volume of data, while keeping it readable, editable and most importantly: able to be edited on your favorite office spreadsheet program.

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

This works as a general "copy and paste" macro to use, so you can put anything that is valid for a country file within the template, as long as it's things for the root (i.e you can't specify a template inside a last names list).

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

If THIS/FROM is a nation, the state and province are deduced from the capital.
If THIS/FROM is a state, the nation is deduced from the owner, and the province from the state capital.
If THIS/FROM is a province, the nation is deduced from the owner and the state from the state it pertains to (but it will only choose the state instance of the owner).

For example, Nejd and Hedjaz share state ENG_1, the FROM is on a province owned by Nejd on ENG_1, then the state will refer to the state owned by Nejd not by Hedjaz.

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

**New modifier description substitutions**

- `$COUNTRY$`: The nation of the modifier.
- `$COUNTRY_ADJ$`: The adjective of the nation of the modifier.
- `$CAPITAL$`: The capital of the nation of the modifier.
- `$CONTINENTNAME$`: The continent of the modifier (capital used as reference if national modifier).
- `$PROVINCENAME$`: The province of the modifier (capital used as reference if national modifier).

### Government ruler-names

Now you can define ruler names for a specific nation with a specific government type, for example:

```
RUS_absolute_monarchy;The Russian Empire
RUS_absolute_monarchy_ruler;Tsar
```

### Definitions for multiple goods produced by local RGO

As RGO can produce a whole distribution of goods, you can define your own distribution for specific provinces:

Example (`history\provinces` files):
```
rgo_distribution = {
	entry = {
        trade_good = silk
        max_employment = 100000
	}
    entry = {
        trade_good = opium
        max_employment = 100000
	}
}
```

To add some RGO entries without overriding ones generated from climate, terrain and primary trade_goods use `rgo_distribution_add_`.

```
rgo_distribution_add = {
    entry = {
        trade_good = coal
        max_employment = 100000
	}
}
```

### Factory potentials

Factory potentials (e.g. resource potentials) is a feature that allows modders to limit number of factories per province (state).

To limit factory creation to provinces having potentials, factory definition must have `uses_potentials = yes`.

In province history files define limits as follows:
```
factory_limit = {
	entry = {
        trade_good = iron
        max_level = 16
	}
}
```
