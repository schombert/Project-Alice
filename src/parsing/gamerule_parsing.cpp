#include "parsers_declarations.hpp"
#include "system_state.hpp"
#include "gamerule_parsing.hpp"

namespace parsers {
void make_gamerule(token_generator& gen, error_handler& err, scenario_building_context& context) {
	/*auto new_id = context.state.world.create_gamerule();
	scripted_gamerule_context new_context{ new_id, context };*/
	auto gamerule = parse_scripted_gamerule(gen, err, context);

}

void make_scan_gamerule(token_generator& gen, error_handler& err, scenario_building_context& context) {
	auto new_id = context.state.world.create_gamerule();
	scripted_gamerule_context new_context{ new_id, context };
	auto gamerule = parse_scan_scripted_gamerule(gen, err, new_context);
	if(gamerule.defined_name.empty()) {
		err.accumulated_errors += "Gamerule defined without name (" + err.file_name + ")" +  "\n";
	}
}


}



