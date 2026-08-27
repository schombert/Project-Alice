--- This file contains all lua functions used by hardcoded gamerules


---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_no_on_select(gamerule_id)
end


---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_no_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_yes_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_sphereling_declare_war_on_spherelord_opt_yes_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_partial_retreat_opt_disabled_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_partial_retreat_opt_disabled_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_partial_retreat_opt_enabled_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_allow_partial_retreat_opt_enabled_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_fog_of_war_opt_disabled_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_fog_of_war_opt_disabled_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_fog_of_war_opt_enabled_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_fog_of_war_opt_enabled_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_fog_of_war_opt_disabled_for_observer_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_fog_of_war_opt_disabled_for_observer_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_auto_concession_peace_opt_cannot_reject_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_auto_concession_peace_opt_cannot_reject_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_auto_concession_peace_opt_can_reject_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_auto_concession_peace_opt_can_reject_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_command_units_opt_disabled_on_select(gamerule_id)
	-- get all commanded units back to their original owners when the gamerule is disabled
	for nation_id = 0, NATION.size() - 1 do
		if(NATION.get_overlord_commanding_units(nation_id)) then
			NATION.set_overlord_commanding_units(nation_id, false)
		end
	end
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_command_units_opt_disabled_on_deselect(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_command_units_opt_enabled_on_select(gamerule_id)
end

---@param gamerule_id gamerule id
function alice.alice_gamerule_command_units_opt_enabled_on_deselect(gamerule_id)
end
