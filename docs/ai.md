## AI logic

### Estimations

- `float estimate_strength(sys::state& state, dcon::nation_id n)` -- Exclusively estimate the power of this nation, plus subjects and vassals (but not allies).
- `float estimate_defensive_strength(sys::state& state, dcon::nation_id n)` -- Estimate the defensive strength we would encounter if we were to battle against them (combined forces of the nation plus the allies).
- `float estimate_additional_offensive_strength(sys::state& state, dcon::nation_id n, dcon::nation_id target)` -- Estimate the additional offensive strength in comparison to the target.

## AI unit movement and construction facilities

### General

`void remove_ai_data(sys::state& state, dcon::nation_id n)`:
This function, when run, removes all ai control information associated with units for the given nation. This must be run when a player takes control of a nation. Otherwise, AI behaviors set for particular units may continue to run and cause the units to act without the player giving them orders.

`void daily_cleanup(sys::state& state)`:
Must be run each day. Removes empty armies and navies that may be left by AI merge actions. (This allows merging to be potentially done in parallel by nation, as no armies from different nations may merge together. 

`void general_ai_unit_tick(sys::state& state)`:
Runs the various functions described below that issue new orders to idle units.

### Naval AI

`void update_ships(sys::state& state)`:
This function is responsible for deleting out-of-date ships belonging to an AI-controlled nation. This function iterates over all nations, but only deletes ships belonging to AI nations that are *not* currently at war.

`void build_ships(sys::state& state)`:
This function fills out a AI-controlled nation's build queue for ships. Nations that are currently building one or more ships will be skipped. An AI will build at least 10 transports or 1/3 of its fleet cap, whichever is greater. It will then spend half of its remaining fleet cap on small ships and half on large ships.

`dcon::province_id get_home_port(sys::state& state, dcon::nation_id n)`:
Internal function. Finds the port province with the greatest max level that is closest to the capital. AI navies will use this as a home base to return to when they have no active mission.

`void refresh_home_ports(sys::state& state)`:
Sets the home port for each alive AI nation. Needs to be run when the scenario is loaded and, ideally, on each tick.

`bool navy_needs_repair(sys::state& state, dcon::navy_id n)`:
Internal function. Used to check whether a navy should remain in port to recover strength.

`bool naval_advantage(sys::state& state, dcon::nation_id n)`:
Internal function. Used to guess whether it is safe for the navy to leave port.

`void send_fleet_home(sys::state& state, dcon::navy_id n)`:
Internal function. Used primarily as a way of "recovering" a fleet when something has interrupted its activity. Attempts to send it to the home port and offload all transported armies in order to ready it for a new action.

`bool set_fleet_target(sys::state& state, dcon::nation_id n, dcon::province_id start, dcon::navy_id for_navy)`:
Internal function. Used to direct a fleet, two provinces at a time, towards another fleet when attempting to engage in naval combat.

`void unload_units_from_transport(sys::state& state, dcon::navy_id n)`:
Internal function. Used when a transport arrives off the coast of its destination but cannot dock. Paths all transported units towards their destinations, if possible.

`bool merge_fleet(sys::state& state, dcon::navy_id n, dcon::province_id p, dcon::nation_id owner)`
Internal function. Attempts to transfer the contents of the fleet to another non empty fleet owned by the same nation located in the same province. Leaves the existing navy itself behind, empty.

`void pickup_idle_ships(sys::state& state)`:
This function is used to give new tasks to a fleet that has finished its previous task. It also finds newly constructed ships and attempts to merge them into the main fleet. The more often this is run, the faster fleets will appear to respond to the current situation.

### Army AI

`void distribute_guards(sys::state& state, dcon::nation_id n)`:
Internal function. This function attempts to distribute currently un-tasked armies (guards) among the provinces controlled by a nation, prioritizing hostile borders, over normal borders, etc. This function *does not* actually move any units or even give them orders. Rather, it gives each a province that it should be standing in. Other functions will move idle armies back to those positions. This function may be costly; it should not be run too often.

`void make_defense(sys::state& state)`:
Public function that does the above in parallel for all nations

`void move_idle_guards(sys::state& state)`:
This function takes units assigned to be guards that are not currently doing anything and tries to path them towards their assigned province. Units that cannot be pathed are marked to be transported

`void update_naval_transport(sys::state& state)`:
This function takes units that are trying to be transported and attempts to move them onto any waiting fleet, if possible. This handles both guard transports and attack transports. This does not move the ships. Making loaded ships start moving is part of the naval unit ai.

`bool army_ready_for_battle(sys::state& state, dcon::nation_id n, dcon::army_id a)`:
Internal function. Tests the organization of the first regiment to estimate whether the army can be used offensively.

`void gather_to_battle(sys::state& state, dcon::nation_id n, dcon::province_id p, float sorting_distance_limit)`:
Internal function. Gathers all of the non-moving armies within the distance limit in a round trip movement to reinforce a battle.

`void assign_targets(sys::state& state, dcon::nation_id n)`:
Internal function. Decides on targets to attack, orders gather commands to attacker groups so they start from a single province. The more often this is run, the more aggressive AIs will be in making attacks.

`void make_attacks(sys::state& state)`:
Public function that does the above in parallel for all nations

`void perform_battle_cycling(sys::state& state)`:
Retreats units that are below organisation and strength threshold.

`void move_gathered_attackers(sys::state& state)`:
Gives move orders to attacking units that are not currently moving (or moves them back to guard status if the attack has ended). Is also responsible for arranging for transport for units that cannot reach their target over land.

`void update_land_constructions(sys::state& state)`:
`build_ships` but for land units. Tries to balance frontline and backline units.

`void new_units_and_merging(sys::state& state)`:
Handles assigning base ai activity types to "new" units. Gives movement orders to new units to try to form 10 stacks. Merges such units into an appropriate army when they arrive.

TODO: reinforce battles
