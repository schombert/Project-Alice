#pragma once
#include "dcon_generated.hpp"
#include "container_types.hpp"

namespace pop_demographics {

constexpr inline uint32_t count_special_keys = 0;

dcon::pop_demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::pop_demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
uint32_t size(sys::state const& state);

void regenerate_is_primary_or_accepted(sys::state& state);

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
constexpr inline dcon::demographics_key poor_life_needs(11);
constexpr inline dcon::demographics_key middle_life_needs(12);
constexpr inline dcon::demographics_key rich_life_needs(13);
constexpr inline dcon::demographics_key poor_everyday_needs(14);
constexpr inline dcon::demographics_key middle_everyday_needs(15);
constexpr inline dcon::demographics_key rich_everyday_needs(16);
constexpr inline dcon::demographics_key poor_luxury_needs(17);
constexpr inline dcon::demographics_key middle_luxury_needs(18);
constexpr inline dcon::demographics_key rich_luxury_needs(19);
constexpr inline dcon::demographics_key poor_total(20);
constexpr inline dcon::demographics_key middle_total(21);
constexpr inline dcon::demographics_key rich_total(22);

constexpr inline uint32_t count_special_keys = 23;

dcon::demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::pop_type_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::culture_id v);
dcon::demographics_key to_key(sys::state const& state, dcon::religion_id v);
dcon::demographics_key to_employment_key(sys::state const& state, dcon::pop_type_id v);

uint32_t size(sys::state const& state);

void regenerate_from_pop_data(sys::state& state);

struct promotion_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	ve::vectorizable_buffer<dcon::pop_type_id, dcon::pop_id> types;
	uint32_t size = 0;

	promotion_buffer() : amounts(0), types(0), size(0) { }
	void update(uint32_t s) {
		if(size < s) {
			size = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
			types = ve::vectorizable_buffer<dcon::pop_type_id, dcon::pop_id>(s);
		}
	}
};

void update_literacy(sys::state& state, uint32_t offset, uint32_t divisions);
void update_consciousness(sys::state& state, uint32_t offset, uint32_t divisions);
void update_militancy(sys::state& state, uint32_t offset, uint32_t divisions);
void update_ideologies(sys::state& state, uint32_t offset, uint32_t divisions);
void update_issues(sys::state& state, uint32_t offset, uint32_t divisions);
void update_growth(sys::state& state, uint32_t offset, uint32_t divisions);
void update_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& pbuf);

void apply_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& pbuf);

void remove_size_zero_pops(sys::state& state);

}
