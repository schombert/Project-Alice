#pragma once

#include "dcon_generated.hpp"
#include "script_constants.hpp"
#include "container_types.hpp"

namespace event {

inline constexpr int32_t expiration_in_days = 30;

enum class slot_type : uint8_t { none, nation, state, province, pop };

struct pending_human_n_event {
	uint32_t r_lo = 0; //0,4
	uint32_t r_hi = 0; //4,4
	int32_t primary_slot; //8,4
	int32_t from_slot; //12,4
	sys::date date; //20,2
	dcon::national_event_id e; //16,2
	dcon::nation_id n; //18,2
	slot_type pt; //20,1
	slot_type ft; //21,1
};
static_assert(sizeof(pending_human_n_event) ==
	sizeof(pending_human_n_event::r_lo)
	+ sizeof(pending_human_n_event::r_hi)
	+ sizeof(pending_human_n_event::primary_slot)
	+ sizeof(pending_human_n_event::from_slot)
	+ sizeof(pending_human_n_event::date)
	+ sizeof(pending_human_n_event::e)
	+ sizeof(pending_human_n_event::n)
	+ sizeof(pending_human_n_event::pt)
	+ sizeof(pending_human_n_event::ft));
struct pending_human_f_n_event {
	uint32_t r_lo = 0; //0,4
	uint32_t r_hi = 0; //4,4
	sys::date date; //8,2
	dcon::free_national_event_id e; //10,2
	dcon::nation_id n; //12,2
	uint16_t padding = 0;
};
static_assert(sizeof(pending_human_f_n_event) ==
	sizeof(pending_human_f_n_event::r_lo)
	+ sizeof(pending_human_f_n_event::r_hi)
	+ sizeof(pending_human_f_n_event::date)
	+ sizeof(pending_human_f_n_event::e)
	+ sizeof(pending_human_f_n_event::n)
	+ sizeof(pending_human_f_n_event::padding));
struct pending_human_p_event {
	uint32_t r_lo = 0; //4
	uint32_t r_hi = 0; //4
	int32_t from_slot; //4
	sys::date date; //2
	dcon::provincial_event_id e; //2
	dcon::province_id p; //2
	slot_type ft; //1
	uint8_t padding = 0;
};
static_assert(sizeof(pending_human_p_event) ==
	sizeof(pending_human_p_event::r_lo)
	+ sizeof(pending_human_p_event::r_hi)
	+ sizeof(pending_human_p_event::from_slot)
	+ sizeof(pending_human_p_event::date)
	+ sizeof(pending_human_p_event::e)
	+ sizeof(pending_human_p_event::p)
	+ sizeof(pending_human_p_event::ft)
	+ sizeof(pending_human_p_event::padding));
struct pending_human_f_p_event {
	uint32_t r_lo = 0; //0,4
	uint32_t r_hi = 0; //4,4
	sys::date date; //8,2
	dcon::free_provincial_event_id e; //10,2
	dcon::province_id p; //12,2
	uint16_t padding = 0;
};
static_assert(sizeof(pending_human_f_p_event) ==
	sizeof(pending_human_f_p_event::r_lo)
	+ sizeof(pending_human_f_p_event::r_hi)
	+ sizeof(pending_human_f_p_event::date)
	+ sizeof(pending_human_f_p_event::e)
	+ sizeof(pending_human_f_p_event::p)
	+ sizeof(pending_human_f_p_event::padding));

void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo,
		int32_t from_slot = 0, slot_type ft = slot_type::none);
void trigger_national_event(sys::state& state, dcon::national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo,
		int32_t primary_slot, slot_type pt, int32_t from_slot, slot_type ft);
void trigger_national_event(sys::state& state, dcon::free_national_event_id e, dcon::nation_id n, uint32_t r_hi, uint32_t r_lo);
void trigger_provincial_event(sys::state& state, dcon::provincial_event_id e, dcon::province_id p, uint32_t r_hi, uint32_t r_lo,
		int32_t from_slot = 0, slot_type ft = slot_type::none);
void trigger_provincial_event(sys::state& state, dcon::free_provincial_event_id e, dcon::province_id p, uint32_t r_hi,
		uint32_t r_lo);

void fire_fixed_event(sys::state& state, std::vector<nations::fixed_event> const& v, int32_t primary_slot, slot_type pt, dcon::nation_id this_slot, int32_t from_slot, slot_type ft);
void fire_fixed_event(sys::state& state, std::vector<nations::fixed_election_event> const& v, int32_t primary_slot, slot_type pt, dcon::nation_id this_slot, int32_t from_slot, slot_type ft);
void fire_fixed_event(sys::state& state, std::vector<nations::fixed_province_event> const& v, dcon::province_id prov, int32_t from_slot, slot_type ft);

void take_option(sys::state& state, pending_human_n_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_f_n_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_p_event const& e, uint8_t opt);
void take_option(sys::state& state, pending_human_f_p_event const& e, uint8_t opt);

bool would_be_duplicate_instance(sys::state& state, dcon::national_event_id e, dcon::nation_id n, sys::date date);
void update_future_events(sys::state& state);
void update_events(sys::state& state);

dcon::issue_id get_election_event_issue(sys::state& state, dcon::national_event_id e);

} // namespace event
