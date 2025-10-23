-- require("jit.v").start("dump.txt")

-- example of lua modding

---comment
---@param navy navy_id
---@param province province_id
local function fleet_bombardment(navy, province)
	local navy_control = NAVY.get_navy_control(navy)
	local navy_owner = NAVY_CONTROL.get_controller(navy_control)

	for k = 0, PROVINCE.get_range_length_army_location(province) - 1 do
		local unit_location = PROVINCE.get_item_from_range_army_location(province, k)
		local unit = ARMY_LOCATION.get_army(unit_location)
		local control = ARMY.get_army_control(unit)
		local army_owner = ARMY_CONTROL.get_controller(control)

		if MILITARY.at_war(army_owner, navy_owner) then
			local attack = 0
			for ship_index = 0, NAVY.get_range_length_navy_membership(navy) - 1 do
				local membership = NAVY.get_item_from_range_navy_membership(navy, ship_index)
				local ship = NAVY_MEMBERSHIP.get_ship(membership)
				local ship_type = SHIP.get_type(ship)
				local base_attack = MILITARY.get_stats(ship_type)[0].attack_or_gun_power
				attack = attack + SHIP.get_strength(ship) * base_attack ---@type number
			end

			local total_str = 0
			for reg_index = 0, ARMY.get_range_length_army_membership(unit) - 1 do
				local membership = ARMY.get_item_from_range_army_membership(unit, reg_index)
				local reg = ARMY_MEMBERSHIP.get_regiment(membership)
				total_str = total_str + REGIMENT.get_strength(reg) ---@type number
			end

			if (total_str > 0) then
				for reg_index = 0, ARMY.get_range_length_army_membership(unit) - 1 do
					local membership = ARMY.get_item_from_range_army_membership(unit, reg_index)
					local reg = ARMY_MEMBERSHIP.get_regiment(membership)
					local unit_type = REGIMENT.get_type(reg)
					local base_def = MILITARY.get_stats(unit_type)[0].defence_or_hull
					local regiment_str = REGIMENT.get_strength(reg)
					local shared_damage = regiment_str / total_str
					local actual_damage = attack * shared_damage / (1 + base_def) / 100
					if (actual_damage > regiment_str) then
						REGIMENT.set_strength(reg, 0)
					else
						REGIMENT.set_strength(reg, regiment_str - actual_damage)
					end
				end
			end
		end
	end
end

function alice.global_bombardment()
	for i = 0, NAVY.size() - 1 do
		local location = NAVY.get_navy_location(i)
		local province = NAVY_LOCATION.get_location(location)
		local adjacency_range = PROVINCE.get_range_length_province_adjacency(province)
		for j = 0, adjacency_range - 1 do
			local adj = PROVINCE.get_item_from_range_province_adjacency(province, j)
			local p1 = PROVINCE_ADJACENCY.get_connected_provinces(adj, 0)
			local p2 = PROVINCE_ADJACENCY.get_connected_provinces(adj, 1)
			local other = p1
			if p1 == province then
				other = p2
			end

			fleet_bombardment(i, other)
		end
		fleet_bombardment(i, province)
	end
end

---@param battle_id land_battle_id
function alice.store_battle_tick_results(battle_id)
end

-- FILE = assert(io.open("battles_log.txt", "w"))

---@param battle_id land_battle_id
---@param winner_exists boolean
---@param attacker_won boolean
function alice.handle_battle_end(battle_id, winner_exists, attacker_won)
	local casualties_attacker =
		LAND_BATTLE.get_attacker_cav_lost(battle_id)
		+ LAND_BATTLE.get_attacker_infantry_lost(battle_id)
		+ LAND_BATTLE.get_attacker_support_lost(battle_id)
	local casualties_defender =
		LAND_BATTLE.get_defender_cav_lost(battle_id)
		+ LAND_BATTLE.get_defender_infantry_lost(battle_id)
		+ LAND_BATTLE.get_defender_support_lost(battle_id)
end

-- ON_ACTION.add_daily_call("global_bombardment")
-- ON_ACTION.add_battle_call("store_battle_tick_results")
-- ON_ACTION.add_battle_end_call("handle_battle_end")