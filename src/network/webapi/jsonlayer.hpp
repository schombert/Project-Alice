#include "json.hpp"

using json = nlohmann::json;

namespace webui {

json format_color(sys::state& state, uint32_t c);

json format_commodity(sys::state& state, dcon::commodity_id c);
json format_commodity_link(sys::state& state, dcon::commodity_id c);

json format_nation(sys::state& state, dcon::nation_id n);

json format_nation(sys::state& state, dcon::national_identity_id n);
json format_nation_link(sys::state& state, dcon::nation_id n);

json format_state(sys::state& state, dcon::state_instance_id stid);
json format_state_link(sys::state& state, dcon::state_instance_id stid);

json format_province(sys::state& state, dcon::province_id prov);
json format_province_link(sys::state& state, dcon::province_id prov);

json format_wargoal(sys::state& state, dcon::wargoal_id wid);

json format_wargoal(sys::state& state, sys::full_wg wid);

json format_factory(sys::state& state, dcon::factory_id fid);
json format_factory_link(sys::state& state, dcon::factory_id fid);

json format_commodity_set(sys::state& state, economy::commodity_set set);

}
