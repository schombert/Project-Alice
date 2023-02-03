#include "parsers_declarations.hpp"
#include <algorithm>
#include <iterator>

namespace parsers {

void default_map_file::finish(scenario_building_context& context) {
	auto first_sea = std::stable_partition(context.prov_id_to_original_id_map.begin(), context.prov_id_to_original_id_map.end(),
		[](province_data const& a) { return !(a.is_sea); });

	for(size_t i = context.prov_id_to_original_id_map.size(); i-- > 0; ) {
		dcon::province_id id = dcon::province_id(dcon::province_id::value_base_t(i));
		auto old_id = context.prov_id_to_original_id_map[id].id;
		context.original_id_to_prov_id_map[old_id] = id;

		auto name = std::string("PROV") + std::to_string(old_id);
		auto name_id = text::find_or_add_key(context.state, name);

		context.state.world.province_set_name(id, name_id);
	}

	auto to_first_sea = std::distance(context.prov_id_to_original_id_map.begin(), first_sea);
	context.state.province_definitions.first_sea_province = dcon::province_id(dcon::province_id::value_base_t(to_first_sea));
}

}
