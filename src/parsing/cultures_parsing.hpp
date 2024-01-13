#pragma once

#include "parsers.hpp"

namespace parsers {
	void make_ideology_group(std::string_view name, token_generator& gen, error_handler& err, scenario_building_context& context);
}