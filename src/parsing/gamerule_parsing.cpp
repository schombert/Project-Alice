#include "system_state.hpp"
#include "gamerule_parsing.hpp"

namespace parsers {
void make_gamerule(token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto new_id = context.state.world.create_gamerule();
	scripted_gamerule_context new_context{ new_id, context };
	auto gamerule = parse_scripted_gamerule(gen, err, new_context);

}


dcon::effect_key make_gamerule_effect(token_generator& gen, error_handler& err, scripted_gamerule_context& context) {
	effect_building_context e_context{ context.outer_context, trigger::slot_contents::empty, trigger::slot_contents::empty, trigger::slot_contents::empty };
	e_context.effect_is_for_event = false;

	e_context.compiled_effect.push_back(uint16_t(effect::generic_scope | effect::scope_has_limit));
	e_context.compiled_effect.push_back(uint16_t(0));
	auto payload_size_offset = e_context.compiled_effect.size() - 1;
	e_context.limit_position = e_context.compiled_effect.size();
	e_context.compiled_effect.push_back(trigger::payload(dcon::trigger_key()).value);
	auto old_err_size = err.accumulated_errors.size();
	parsers::parse_effect_body<effect_building_context&>(gen, err, e_context);

	e_context.compiled_effect[payload_size_offset] = uint16_t(e_context.compiled_effect.size() - payload_size_offset);

	if(e_context.compiled_effect.size() >= std::numeric_limits<uint16_t>::max()) {
		err.accumulated_errors += "effect in gamerule " + text::produce_simple_string(context.outer_context.state, context.outer_context.state.world.gamerule_get_name(context.id)) + " is " +
			std::to_string(e_context.compiled_effect.size()) +
			" cells big, which exceeds 64 KB bytecode limit (" + err.file_name + ")\n";
		return dcon::effect_key{ 0 };
	}

	if(err.accumulated_errors.size() == old_err_size) {
		auto const new_size = simplify_effect(e_context.compiled_effect.data());
		e_context.compiled_effect.resize(static_cast<size_t>(new_size));
	} else {
		e_context.compiled_effect.clear();
	}

	auto effect_id = context.outer_context.state.commit_effect_data(e_context.compiled_effect);
	return effect_id;
}

}



