#pragma once
#include "dcon_generated.hpp"
#include "system_state.hpp"

namespace pop_demographics {

constexpr inline uint32_t count_special_keys = 0;

dcon::pop_demographics_key to_key(sys::state const& state, dcon::ideology_id v);
dcon::pop_demographics_key to_key(sys::state const& state, dcon::issue_option_id v);
uint32_t size(sys::state const& state);

constexpr inline float pop_mc_scaling = 10.0f / float(std::numeric_limits<uint16_t>::max());
constexpr inline float pop_u16_scaling = 1.0f / float(std::numeric_limits<uint16_t>::max());
constexpr inline float pop_u8_scaling = 1.0f / float(std::numeric_limits<uint8_t>::max());
constexpr inline float inv_pop_mc_scaling = float(std::numeric_limits<uint16_t>::max()) / 10.0f;
constexpr inline float inv_pop_u16_scaling = float(std::numeric_limits<uint16_t>::max());
constexpr inline float inv_pop_u8_scaling = float(std::numeric_limits<uint8_t>::max());

inline uint8_t to_pu8(float v) {
	return uint8_t(v * inv_pop_u8_scaling + 0.49f);
}
inline uint16_t to_pu16(float v) {
	return uint16_t(v * inv_pop_u16_scaling + 0.49f);
}
inline uint16_t to_pmc(float v) {
	return uint16_t(v * inv_pop_mc_scaling + 0.49f);
}
inline ve::int_vector to_pu8(ve::fp_vector v) {
	return ve::to_int(v * inv_pop_u8_scaling + 0.49f);
}
inline ve::int_vector to_pu16(ve::fp_vector v) {
	return ve::to_int(v * inv_pop_u16_scaling + 0.49f);
}
inline ve::int_vector to_pmc(ve::fp_vector v) {
	return ve::to_int(v * inv_pop_mc_scaling + 0.49f);
}

inline float from_pu8(uint8_t v) {
	return float(v) * pop_u8_scaling;
}
inline float from_pu16(uint16_t v) {
	return float(v) * pop_u16_scaling;
}
inline float from_pmc(uint16_t v) {
	return float(v) * pop_mc_scaling;
}

inline ve::fp_vector from_pu8(ve::int_vector v) {
	return ve::to_float(v) * pop_u8_scaling;
}
inline ve::fp_vector from_pu16(ve::int_vector v) {
	return ve::to_float(v) * pop_u16_scaling;
}
inline ve::fp_vector from_pmc(ve::int_vector v) {
	return ve::to_float(v) * pop_mc_scaling;
}

void regenerate_is_primary_or_accepted(sys::state& state);
float get_demo(sys::state const& state, dcon::pop_id p, dcon::pop_demographics_key k);
void set_demo(sys::state& state, dcon::pop_id p, dcon::pop_demographics_key k, float v);
float get_militancy(sys::state const& state, dcon::pop_id p);
void set_militancy(sys::state& state, dcon::pop_id p, float v);
float get_consciousness(sys::state const& state, dcon::pop_id p);
void set_consciousness(sys::state& state, dcon::pop_id p, float v);
float get_literacy(sys::state const& state, dcon::pop_id p);
void set_literacy(sys::state& state, dcon::pop_id p, float v);
float get_employment(sys::state const& state, dcon::pop_id p);
float get_raw_employment(sys::state const& state, dcon::pop_id p);
void set_employment(sys::state& state, dcon::pop_id p, float v);
void set_raw_employment(sys::state& state, dcon::pop_id p, float v);
float get_life_needs(sys::state const& state, dcon::pop_id p);
void set_life_needs(sys::state& state, dcon::pop_id p, float v);
float get_everyday_needs(sys::state const& state, dcon::pop_id p);
void set_everyday_needs(sys::state& state, dcon::pop_id p, float v);
float get_luxury_needs(sys::state const& state, dcon::pop_id p);
void set_luxury_needs(sys::state& state, dcon::pop_id p, float v);
float get_social_reform_desire(sys::state const& state, dcon::pop_id p);
void set_social_reform_desire(sys::state& state, dcon::pop_id p, float v);
float get_political_reform_desire(sys::state const& state, dcon::pop_id p);
void set_political_reform_desire(sys::state& state, dcon::pop_id p, float v);

template<typename T>
auto get_employment(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uemployment(p);
	return from_pu8(ival) * state.world.pop_get_size(p);
}
template<typename T>
auto get_raw_employment(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uemployment(p);
	return from_pu8(ival);
}
template<typename T>
auto get_social_reform_desire(sys::state const& state, T p) {
	auto ival = state.world.pop_get_usocial_reform_desire(p);
	return from_pu8(ival);
}
template<typename T>
auto get_political_reform_desire(sys::state const& state, T p) {
	auto ival = state.world.pop_get_upolitical_reform_desire(p);
	return from_pu8(ival);
}
template<typename T>
auto get_militancy(sys::state const& state, T p) {
	auto ival = state.world.pop_get_umilitancy(p);
	return from_pmc(ival);
}
template<typename T>
auto get_consciousness(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uconsciousness(p);
	return from_pmc(ival);
}
template<typename T>
auto get_literacy(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uliteracy(p);
	return from_pu16(ival);
}
template<typename T>
auto get_life_needs(sys::state const& state, T p) {
	auto ival = state.world.pop_get_ulife_needs_satisfaction(p);
	return from_pu8(ival);
}
template<typename T>
auto get_everyday_needs(sys::state const& state, T p) {
	auto ival = state.world.pop_get_ueveryday_needs_satisfaction(p);
	return from_pu8(ival);
}
template<typename T>
auto get_luxury_needs(sys::state const& state, T p) {
	auto ival = state.world.pop_get_uluxury_needs_satisfaction(p);
	return from_pu8(ival);
}
template<typename T>
auto get_demo(sys::state const& state, T p, dcon::pop_demographics_key k) {
	auto ival = state.world.pop_get_udemographics(p, k);
	return from_pu8(ival);
}

} // namespace pop_demographics
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

void regenerate_jingoism_support(sys::state& state, dcon::nation_id n);
void regenerate_from_pop_data_full(sys::state& state);
void regenerate_from_pop_data_daily(sys::state& state);

struct ideology_buffer {
	tagged_vector<ve::vectorizable_buffer<uint8_t, dcon::pop_id>, dcon::ideology_id> temp_buffers;
	uint32_t size = 0;

	ideology_buffer(sys::state& state) : size(0) {
		for(uint32_t i = 0; i < state.world.ideology_size(); ++i) {
			temp_buffers.emplace_back(uint32_t(0));
		}
	}

	void update(sys::state& state, uint32_t s) {
		if(size < s) {
			size = s;
			state.world.for_each_ideology(
					[&](dcon::ideology_id i) { temp_buffers[i] = ve::vectorizable_buffer<uint8_t, dcon::pop_id>(s);  /*state.world.pop_make_vectorizable_float_buffer();*/ });
		}
	}
};

struct issues_buffer {
	tagged_vector<ve::vectorizable_buffer<uint8_t, dcon::pop_id>, dcon::issue_option_id> temp_buffers;
	uint32_t size = 0;

	issues_buffer(sys::state& state) : size(0) {
		for(uint32_t i = 0; i < state.world.issue_option_size(); ++i) {
			temp_buffers.emplace_back(uint32_t(0));
		}
	}

	void update(sys::state& state, uint32_t s) {
		if(size < s) {
			size = s;
			state.world.for_each_issue_option(
					[&](dcon::issue_option_id i) { temp_buffers[i] = ve::vectorizable_buffer<uint8_t, dcon::pop_id>(s); /*state.world.pop_make_vectorizable_float_buffer();*/ });
		}
	}
};

struct promotion_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	ve::vectorizable_buffer<dcon::pop_type_id, dcon::pop_id> types;
	uint32_t size = 0;
	uint32_t reserved = 0;

	promotion_buffer() : amounts(0), types(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
			types = ve::vectorizable_buffer<dcon::pop_type_id, dcon::pop_id>(s);
		}
	}
};

struct assimilation_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	uint32_t size = 0;
	uint32_t reserved = 0;

	assimilation_buffer() : amounts(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
		}
	}
};

struct conversion_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	uint32_t size = 0;
	uint32_t reserved = 0;

	conversion_buffer() : amounts(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
		}
	}
};

struct migration_buffer {
	ve::vectorizable_buffer<float, dcon::pop_id> amounts;
	ve::vectorizable_buffer<dcon::province_id, dcon::pop_id> destinations;
	uint32_t size = 0;
	uint32_t reserved = 0;

	migration_buffer() : amounts(0), destinations(0), size(0) { }
	void update(uint32_t s) {
		size = s;
		if(reserved < s) {
			reserved = s;
			amounts = ve::vectorizable_buffer<float, dcon::pop_id>(s);
			destinations = ve::vectorizable_buffer<dcon::province_id, dcon::pop_id>(s);
		}
	}
};

void update_literacy(sys::state& state, uint32_t offset, uint32_t divisions);
void update_consciousness(sys::state& state, uint32_t offset, uint32_t divisions);
void update_militancy(sys::state& state, uint32_t offset, uint32_t divisions);
void update_ideologies(sys::state& state, uint32_t offset, uint32_t divisions, ideology_buffer& ibuf);
void update_issues(sys::state& state, uint32_t offset, uint32_t divisions, issues_buffer& ibuf);
void update_growth(sys::state& state, uint32_t offset, uint32_t divisions);
void update_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& pbuf);
void update_assimilation(sys::state& state, uint32_t offset, uint32_t divisions, assimilation_buffer& pbuf);
void update_internal_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void update_colonial_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void update_immigration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void update_conversion(sys::state& state, uint32_t offset, uint32_t divisions, conversion_buffer& pbuf);

float get_estimated_literacy_change(sys::state& state, dcon::nation_id n);
float get_estimated_mil_change(sys::state& state, dcon::nation_id n);
float get_estimated_con_change(sys::state& state, dcon::nation_id n);
float get_estimated_promotion(sys::state& state, dcon::nation_id n);

void apply_ideologies(sys::state& state, uint32_t offset, uint32_t divisions, ideology_buffer& pbuf);
void apply_issues(sys::state& state, uint32_t offset, uint32_t divisions, issues_buffer& pbuf);
void apply_type_changes(sys::state& state, uint32_t offset, uint32_t divisions, promotion_buffer& pbuf);
void apply_assimilation(sys::state& state, uint32_t offset, uint32_t divisions, assimilation_buffer& pbuf);
void apply_internal_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void apply_colonial_migration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void apply_immigration(sys::state& state, uint32_t offset, uint32_t divisions, migration_buffer& pbuf);
void apply_conversion(sys::state& state, uint32_t offset, uint32_t divisions, conversion_buffer& pbuf);

void remove_size_zero_pops(sys::state& state);
void remove_small_pops(sys::state& state);

float get_monthly_pop_increase(sys::state& state, dcon::pop_id);
int64_t get_monthly_pop_increase(sys::state& state, dcon::nation_id n);
int64_t get_monthly_pop_increase(sys::state& state, dcon::state_instance_id n);
int64_t get_monthly_pop_increase(sys::state& state, dcon::province_id n);

float get_estimated_literacy_change(sys::state& state, dcon::pop_id n);
float get_estimated_mil_change(sys::state& state, dcon::pop_id n);
float get_estimated_con_change(sys::state& state, dcon::pop_id n);
float get_estimated_type_change(sys::state& state, dcon::pop_id n);
float get_effective_estimation_type_change(sys::state& state, dcon::nation_id nation, dcon::pop_type_id target_type);
float get_estimated_promotion(sys::state& state, dcon::pop_id n);
float get_estimated_demotion(sys::state& state, dcon::pop_id n);
float get_estimated_assimilation(sys::state& state, dcon::pop_id n);
float get_estimated_internal_migration(sys::state& state, dcon::pop_id n);
float get_estimated_colonial_migration(sys::state& state, dcon::pop_id n);
float get_estimated_emigration(sys::state& state, dcon::pop_id n);
void estimate_directed_immigration(sys::state& state, dcon::nation_id n, std::vector<float>& national_amounts);
float get_estimated_conversion(sys::state& state, dcon::pop_id n);

float calculate_nation_sol(sys::state& state, dcon::nation_id nation_id);
void reduce_pop_size_safe(sys::state& state, dcon::pop_id pop_id, int32_t amount);

} // namespace demographics
