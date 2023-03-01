#pragma once
#include "dcon_generated.hpp"
#include "container_types.hpp"

namespace pop_demographics {

dcon::pop_demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::pop_demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
uint32_t size(sys::state const& state);

}
namespace demographics {

constexpr inline dcon::demographics_key total(0);
constexpr inline dcon::demographics_key employable(1);
constexpr inline dcon::demographics_key employed(2);
constexpr inline dcon::demographics_key consciousness(3);
constexpr inline dcon::demographics_key militancy(4);
constexpr inline dcon::demographics_key literacy(5);
constexpr inline dcon::demographics_key political_reform_desire(6);
constexpr inline dcon::demographics_key social_reform_desire(7);
constexpr inline dcon::demographics_key poor_militancy(8);
constexpr inline dcon::demographics_key middle_militancy(9);
constexpr inline dcon::demographics_key rich_militancy(10);

constexpr inline uint32_t count_special_keys = 11;

dcon::demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::pop_type_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::culture_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::religion_id v);

uint32_t size(sys::state const& state);

void regenerate_from_pop_data(sys::state& state);

}
