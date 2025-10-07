#pragma once
#include "parsers.hpp"

namespace parsers {

void make_gamerule(token_generator& gen, error_handler& err, scenario_building_context& context);

dcon::effect_key make_gamerule_effect(token_generator& gen, error_handler& err, scenario_building_context& context);

void make_scan_gamerule(token_generator& gen, error_handler& err, scenario_building_context& context);


}
