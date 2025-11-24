namespace sys {
std::string to_debug_string(const event_option& obj) {
	return "(name;ai_chance;effect) => " + std::to_string(obj.name.value) + ";" + std::to_string(obj.ai_chance.value) + ";" + std::to_string(obj.effect.value);
}

std::string to_debug_string(const gamerule_option& obj)  {
	return "(name;on_select;on_deselect) => " + std::to_string(obj.name.value) + ";" + std::to_string(obj.on_select.value) + ";" + std::to_string(obj.on_deselect.value);
}


std::string to_debug_string(const economy::commodity_set& obj) {
	std::string result{ };
	result += "(commodity_id;amount) => ";
	for(uint8_t i = 0; i < economy::commodity_set::set_size; i++) {
		result += std::to_string(obj.commodity_type[i].value) + ";" + std::to_string(obj.commodity_amounts[i]) + " ";
	}
	return result;
}


std::string to_debug_string(const economy::small_commodity_set& obj) {
	std::string result{ };
	result += "(commodity_id;amount) => ";
	for(uint8_t i = 0; i < economy::small_commodity_set::set_size; i++) {
		result += std::to_string(obj.commodity_type[i].value) + ";" + std::to_string(obj.commodity_amounts[i]) + " ";
	}
	return result;
}

std::string to_debug_string(const provincial_modifier_definition& obj)  {
	std::string result{ };
	result += "(offset;value) => ";
	for(uint8_t i = 0; i < provincial_modifier_definition::modifier_definition_size; i++) {
		result += std::to_string(obj.offsets[i].value) + ";" + std::to_string(obj.values[i]) + " ";
	}
	return result;
}


std::string to_debug_string(const national_modifier_definition& obj) {
	std::string result{ };
	result += "(offset;value) => ";
	for(uint8_t i = 0; i < national_modifier_definition::modifier_definition_size; i++) {
		result += std::to_string(obj.offsets[i].value) + ";" + std::to_string(obj.values[i]) + " ";
	}
	return result;
}


std::string to_debug_string(const commodity_modifier& obj) {
	return "(amount:type) => " + std::to_string(obj.amount) + ";" + std::to_string(obj.type.value);
}

std::string to_debug_string(const rebel_org_modifier& obj) {
	return "(amount;type) => " + std::to_string(obj.amount) + ";" + std::to_string(obj.type.value);
}

std::string to_debug_string(const dated_modifier& obj) {
	return "(expiration;mod_id) => " + std::to_string(obj.expiration.value) + ";" + std::to_string(obj.mod_id.value);
}

std::string to_debug_string(const military::ship_in_battle& obj) {
	return "(ship;taget_slot;flags;ships_targeting_this) => " + std::to_string(obj.ship.value) + ";" + std::to_string(obj.target_slot) + ";" + std::to_string(obj.flags) + ";" + std::to_string(obj.ships_targeting_this);
}

std::string to_debug_string(const military::mobilization_order& obj) {
	return "(when;where) => " + std::to_string(obj.when.value) + ";" + std::to_string(obj.where.value);
}

std::string to_debug_string(const military::reserve_regiment& obj) {
	return "(regiment;flags) => " + std::to_string(obj.regiment.value) + ";" + std::to_string(obj.flags);
}

std::string to_debug_string(const military::available_cb& obj) {
	return "(expiration;target;cb_type;target_state) => " + std::to_string(obj.expiration.value) + ";" + std::to_string(obj.target.value) + ";" + std::to_string(obj.cb_type.value) + ";" + std::to_string(obj.target_state.value);
}

}

