#include "parsers_declarations.hpp"
#include "military.hpp"

namespace parsers {

void register_cb_type(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto existing_it = context.map_of_cb_types.find(std::string(name));

	auto id = [&]() {
		if(existing_it != context.map_of_cb_types.end()) {
			existing_it->second.generator_state = gen;
			return existing_it->second.id;
		}
		auto new_id = context.state.world.create_cb_type();
		context.map_of_cb_types.insert_or_assign(std::string(name), pending_cb_content{ gen, new_id });
		return new_id;
	}();

	auto name_id = text::find_or_add_key(context.state, name);
	auto name_desc = std::string(name) + "_desc";
	auto name_setup = std::string(name) + "_setup";
	context.state.world.cb_type_set_name(id, name_id);
	context.state.world.cb_type_set_short_desc(id, text::find_or_add_key(context.state, name_desc));
	context.state.world.cb_type_set_long_desc(id, text::find_or_add_key(context.state, name_setup));

	uint32_t special_flags = 0;
	if(is_fixed_token_ci(name.data(), name.data() + name.length(), "free_peoples"))
		special_flags |= military::cb_flag::po_liberate;
	else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "liberate_country"))
		special_flags |= military::cb_flag::po_liberate;
	else if(is_fixed_token_ci(name.data(), name.data() + name.length(), "take_from_sphere"))
		special_flags |= military::cb_flag::po_take_from_sphere;

	context.state.world.cb_type_set_type_bits(id, special_flags);

	gen.discard_group();
}

void make_trait(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {

	dcon::leader_trait_id new_id = context.state.world.create_leader_trait();
	auto name_id = text::find_or_add_key(context.state, name);

	context.state.world.leader_trait_set_name(new_id, name_id);
	context.map_of_leader_traits.insert_or_assign(std::string(name), new_id);

	trait_context new_context{ context , new_id };
	parse_trait(gen, err, new_context);
}
void personality_traits_set(token_generator& gen, error_handler& err, scenario_building_context& context) {
	parse_traits_set(gen, err, context);
}
void background_traits_set(token_generator& gen, error_handler& err, scenario_building_context& context) {
	context.state.military_definitions.first_background_trait = dcon::leader_trait_id(dcon::leader_trait_id::value_base_t(context.state.world.leader_trait_size()));
	parse_traits_set(gen, err, context);
}


void make_unit(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context) {
	dcon::unit_type_id new_id = dcon::unit_type_id(dcon::unit_type_id::value_base_t(context.state.military_definitions.unit_base_definitions.size()));
	context.state.military_definitions.unit_base_definitions.emplace_back();

	auto name_id = text::find_or_add_key(context.state, name);
	context.state.military_definitions.unit_base_definitions.back().name = name_id;

	context.state.military_definitions.unit_base_definitions.back() = parsers::parse_unit_definition(gen, err, context);

	context.map_of_unit_types.insert_or_assign(std::string(name), new_id);
}

}

