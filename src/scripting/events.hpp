#pragma once

#include "dcon_generated.hpp"
#include "script_constants.hpp"
#include "system_state.hpp"

namespace event {

struct pending_human_n_event {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t primary_slot;
	int32_t from_slot;
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

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo, int32_t from_slot = 0);
void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo, int32_t primary_slot, int32_t from_slot);
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo);
void trigger_provincial_event(sys::state& state, dcon::provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo, int32_t from_slot = 0);
void trigger_provincial_event(sys::state& state, dcon::free_provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo);

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_event> const& v, int32_t primary_slot, dcon::nation_id this_slot, int32_t from_slot);
void fire_fixed_event(sys::state& state, std::vector<nations::fixed_province_event> const& v, dcon::province_id prov, int32_t from_slot);

void take_option(sys::state& state, pending_human_n_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_f_n_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_p_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_f_p_event const& e, uint8_t opt);


void update_events(sys::state& state);

}
