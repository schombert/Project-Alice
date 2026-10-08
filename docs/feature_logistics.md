# Logistics and government stockpiles

This document seeks to explain the new logistics & government stockpiles feature

# Government stockpiles

Government stockpiles are now local to each individual market, as opposed to having a single stockpile per nation.
Government-related projects like government constructions and supplying military units will draw from each individual stockpile. The commodity transportation from the stockpile to the project is handled via `supply routes` (more on that later).

Likewise, the government will purchase the commodities needed for said projects from the local markets and deposit them into their respective government stockpile. The logic will purchase said commodities in markets with favorable deals (high supply, low prices, high expected satsifaction).
It will however not take the position of the market into account when purchasing, simply the market's own merit into account

If a market gets under enemy control in a war (decided by who controls the state capital, simiar to the regular market logic) then the enemy will gain control of the commodities within the stockpile, and can then be used by government projects.
The government is also able to purchase commodities from occupied markets and deposit them into the local govt stockpile.

# Transportation & supply routes

Once a government stockpile has a stockpile of commodities required by government projects, it can be transported via supply routes to said project.
Supply routes are estabilished automatically once there is demand for commodities in a project, and a stockpile can deliver them.
The route can deliver commodities to a project if there is a valid path from the stockpile to the project location (more on that later).

However, even if there is a valid path, the amount which can actually be delivered depends on the `supply throughput` versus the `volume` of the route.
`Volume` is defined as the raw amount of commodities attempting to be transported, modified by the commodity volume_weight stat.
`Supply throughput` is a province modifier which decides how much `volume` may be transported on each province connection. If it is a land-to-land connection, then `supply throughput` is used. If it is a port-to-sea connection, then `port supply capacity` is used instead, but generally refered to as `supply throughput`.
Therefore, the percentage of its commodities it can sucessfully transport per day is province-connection`supply throughput` divided by province-connection `volume`. The transport percentage (reffered to as `supply efficiency`) is then the smallest percentage calculated after walking the whole path.
Eg. if a route has a supply efficiency of 90%, then the project will only receive 90% of the goods transported, while the remaining 10% will still remain in the stockpile for later use.

Routes may also autonomously draw less from a specific stockpile if the path so said stockpile has an existing bottleneck.

`Supply loss` is another factor. A route may have a loss rate depending on the loss modifier on each province the path goes through aswell as the length. The loss % affects how much of the transported goods are lost en-route.
Eg. a loss rate of 10% will mean that the project will only receive 90% of the goods transported, and the remaining 10% is lost.

# Pathing rules & invalid paths

A path is considered invalid if it passes over any province which has a `supply throughput` of zero (or its a port-to-sea connection, and `port supply capacity` is zero) with a few exceptions (covered later).
In addition to custom modifiers, a few hardcoded things may set the `supply throughput` to 0, and thus invalidate any path through it:
- `Military access`: If you do not control, have military access through the province, or have the nation in your sphere, then `supply throughput` & `port supply capacity` will always be 0.
- `Hostile armies`: Hostile armies will apply a multiplicative `supply throughput` & `port supply capacity` modifier according to the `alice_army_supply_throughput_blockade_threshold` define. If the value is 0.9, then 0.9 * `POP_SIZE_PER_REGIMENT` unit strength is enough to set both modifiers to 0. It scales downwards linearly. Armies in battle also counts towards this
- `Blockading ports`: Hostile navies will apply a multiplicative `port supply capacity` modifier according to the `alice_navy_port_supply_capacity_blockade_threshold` define. If the value is 0.9, then a total of 90% ship unit strength is enough to set it to 0 and fully blockading the port. It scales downwards linearly.
- `Movement cost modifiers`: Movement cost modifiers will a `supply throughput` percentage modifier according to the defines `alice_supply_throughput_from_movement_cost_mult` and `alice_supply_throughput_from_movement_cost_max_penalty`

There a special exception to these rules. A path into a 0-`supply throughput`-province may be valid aslong as there is a controlled army in the province, and the adjacent province connection has greater than 0 `supply throughput` (this can not chain!).
This allows for eg. a battle to happen just on the enemy-side of a border, while still being able to supply it by having a safe supply like behind it. Or being able to seige an enemy province next to a supply line
