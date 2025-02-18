#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace economy {
// see details about internal workings of the system in cpp file

float population_per_admin(sys::state& state, dcon::nation_id n);

// amount of taxes you can collect in a given region
float tax_collection_capacity(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid);
// amount of taxes you can collect in a given region summed up for each region
float total_tax_collection_capacity(sys::state& state, dcon::nation_id n);
// ratio of current employed imperial admin to imperial admin required by your nation
float global_admin_ratio(sys::state& state, dcon::nation_id n);
// ratio of current employed local admin to local admin required by given state instance
float local_admin_ratio(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid);

// these functions estimate spendings for given budget priority

// estimate "imperial administration" spendings which are based on total population of the realm
float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority);
// estimate "local administration" spendings
float estimate_spendings_administration_state(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, float budget_priority);
// estimate total spendings for your administration
float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget_priority);
// total current spendings
float full_spendings_administration(sys::state& state, dcon::nation_id n);
// register demand on labor
void update_consumption_administration(sys::state& state, dcon::nation_id n);
// perform a tax collection update
void collect_taxes(sys::state& state, dcon::nation_id n);

struct tax_information {
	float poor = 0.f;
	float mid = 0.f;
	float rich = 0.f;

	float capacity = 0.f;

	float poor_potential = 0.f;
	float mid_potential = 0.f;
	float rich_potential = 0.f;
};

// sums up all tax income all over the nation
tax_information explain_tax_income(sys::state& state, dcon::nation_id n);
tax_information explain_tax_income_local(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid);
}
