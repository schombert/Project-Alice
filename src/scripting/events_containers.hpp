#pragma once

#include "dcon_generated_ids.hpp"
#include "date_interface.hpp"
#include "events_constants.hpp"

namespace event {
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


	bool operator ==(const pending_human_n_event& other) const {
		return date == other.date && e == other.e &&
			from_slot == other.from_slot && n == other.n &&
			primary_slot == other.primary_slot && r_hi == other.r_hi &&
			r_lo == other.r_lo;
	}
	bool operator !=(const pending_human_n_event& other) const {
		return !((*this) == other);
	}
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

	bool operator ==(const pending_human_f_n_event& other) const {
		return r_lo == other.r_lo && r_hi == other.r_hi && date == other.date && e == other.e && n == other.n;
	}
	bool operator !=(const pending_human_f_n_event& other) const {
		return !((*this) == other);
	}



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


	bool operator ==(const pending_human_p_event& other) const {
		return r_lo == other.r_lo && r_hi == other.r_hi && from_slot == other.from_slot && date == other.date && e == other.e && p == other.p;
	}
	bool operator !=(const pending_human_p_event& other) const {
		return !((*this) == other);
	}


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

	bool operator ==(const pending_human_f_p_event& other) const {
		return date == other.date && e == other.e && p == other.p && r_hi == other.r_hi && r_lo == other.r_lo;
	}
	bool operator !=(const pending_human_f_p_event& other) const {
		return !((*this) == other);
	}


};
static_assert(sizeof(pending_human_f_p_event) ==
	sizeof(pending_human_f_p_event::r_lo)
	+ sizeof(pending_human_f_p_event::r_hi)
	+ sizeof(pending_human_f_p_event::date)
	+ sizeof(pending_human_f_p_event::e)
	+ sizeof(pending_human_f_p_event::p)
	+ sizeof(pending_human_f_p_event::padding));
}
