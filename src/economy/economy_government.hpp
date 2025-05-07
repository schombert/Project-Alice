#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace economy {
// see details about internal workings of the system in cpp file

// ratio of taxes you can collect in a given province
float tax_collection_rate(sys::state& state, dcon::nation_id n, dcon::province_id);

// these functions estimate spendings for given budget priority

// estimate "imperial administration" spendings which are based on total population of the realm
float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority);
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

	float local_multiplier = 0.f;

	float poor_potential = 0.f;
	float mid_potential = 0.f;
	float rich_potential = 0.f;
};

// sums up all tax income all over the nation
tax_information explain_tax_income(sys::state& state, dcon::nation_id n);
tax_information explain_tax_income_local(sys::state& state, dcon::nation_id n, dcon::province_id sid);
}
