#pragma once

#include "dcon_generated.hpp"
#include "script_constants.hpp"
#include "container_types.hpp"

namespace event {

inline constexpr int32_t expiration_in_days = 30;

enum class slot_type : uint8_t { none, nation, state, province, pop };

struct pending_human_n_event {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t primary_slot;
	slot_type pt;
	int32_t from_slot;
	slot_type ft;
	dcon::national_event_id e;
	dcon::nation_id n;
	sys::date date;
};
struct pending_human_f_n_event {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	dcon::free_national_event_id e;
	dcon::nation_id n;
	sys::date date;
};
struct pending_human_p_event {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t from_slot;
	slot_type ft;
	dcon::provincial_event_id e;
	dcon::province_id p;
	sys::date date;
};
struct pending_human_f_p_event {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	dcon::free_provincial_event_id e;
	dcon::province_id p;
	sys::date date;
};

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo, int32_t from_slot = 0, slot_type ft = slot_type::none);
void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo, int32_t primary_slot, slot_type pt, int32_t from_slot, slot_type ft);
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo);
void trigger_provincial_event(sys::state& state, dcon::provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo, int32_t from_slot = 0, slot_type ft = slot_type::none);
void trigger_provincial_event(sys::state& state, dcon::free_provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo);

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_event> const& v, int32_t primary_slot, slot_type pt, dcon::nation_id this_slot, int32_t from_slot, slot_type ft);
void fire_fixed_event(sys::state& state, std::vector<nations::fixed_province_event> const& v, dcon::province_id prov, int32_t from_slot, slot_type ft);

void take_option(sys::state& state, pending_human_n_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_f_n_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_p_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_f_p_event const& e, uint8_t opt);


void update_events(sys::state& state);

}
