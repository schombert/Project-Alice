# Modifiers

Alice adds a handful of new modifiers to the game:

National modifiers:

| aristocrat_reinvestment | Increases the % of incomes this pop type deposits into the investment pool |
| capitalist_reinvestment | Increases the % of incomes this pop type deposits into the investment pool |
| middle_class_reinvestment | Increases the % of incomes this pop type deposits into the investment pool |
| farmers_reinvestment | Increases the % of incomes this pop type deposits into the investment pool |
| aristocrat_savings | Increases the % of incomes this pop type deposits into the national bank |
| capitalist_savings | Increases the % of incomes this pop type deposits into the national bank |
| middle_class_savings | Increases the % of incomes this pop type deposits into the national bank |
| farmers_savings | Increases the % of incomes this pop type deposits into the national bank |
| disallow_naval_trade = 1 | If >0.0f, then the nation can't trade with other countries by sea. Passing zero or negative values will lead to unexpected results. |
| disallow_land_trade = 1 | If >0.0f, then the nation can't trade with other countries by land. Passing zero or negative values will lead to unexpected results. |
| trade_routes_attraction | Increases attractiveness of trade routes |
| min_land_upkeep | Minimum setting allowed on the army supply slider |
| land_supply_speed_add | Adds the specified amount of land supply speed to the nation |
| land_supply_speed_percentage | Adds the specified percentage effect to land supply speed on the nation|
| naval_supply_speed_add | Adds the specified amount of naval supply speed to the nation |
| naval_supply_speed_percent | Adds the specified percentage effect to naval supply speed on the nation|
| national_supply_throughput_add | Adds the specified amount of supply throughput to all provinces the nation has supply routes pass through (and only for this nation)|
| national_supply_throughput_percent | Adds the specified percentage effect of supply throughput to all provinces the nation has supply routes pass through(and only for this nation)|
| national_supply_loss_add | Adds the specified amount of supply loss to all provinces the nation has supply routes pass through (and only for this nation)|
| national_supply_loss_percent | Adds the specified percentage effect of supply loss to all provinces the nation has supply routes pass through(and only for this nation)|
| national_port_capacity_add | Adds the specified amount of port supply capacity to all ports the nation has supply routes pass through (and only for this nation)|
| national_port_capacity_percent | Adds the specified percentage effect of port supply capacity to all ports the nation has supply routes pass through(and only for this nation)|


Province modifiers:

| supply_throughput_add | Adds the specified amount of supply throughput to the given province|
| supply_throughput_percent | Adds the specified percentage effect of supply throughput to the given province|
| supply_loss_add | Adds the specified amount of supply loss to the given province|
| supply_loss_percent | Adds the specified percentage effect of supply loss to the given province|
| port_capacity_add | Adds the specified amount of port supply capacity to the given port|
| port_capacity_percent | Adds the specified percentage effect of port supply capacity to the given port|


New static modifiers:
| province_control |  Adds the given scaling modifiers to all land province per 100% control|
| province_militancy |  Adds the given scaling modifiers to all land province per 10.0 militancy|
| nation_base | Adds the given modifiers to every nation in the game|
| province_base | Adds the given modifiers to every province in the game|
| civilian_port | Adds the given scaling modifiers to all provinces per 1000 level of civilian port|
| fastest_land_unit_speed | Adds the given scaling modifiers to all nations per 1 km/h speed of their fastest land unit|
| fastest_transport_unit_speed | Adds the given scaling modifiers to all nations per 1 km/h speed of their fastest transport unit|
