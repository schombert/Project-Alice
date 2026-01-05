#pragma once
#include "dcon_generated_ids.hpp"
#include "economy_common_api_containers.hpp"

namespace sys {
struct state;
}

namespace economy {
inline constexpr float local_administration_efficiency = 0.5f;

// see details about internal workings of the system in cpp file

// ratio of taxes you can collect in a given province
float tax_collection_rate(sys::state& state, dcon::nation_id n, dcon::province_id);
float count_active_administrations(sys::state& state, dcon::nation_id n);
// these functions estimate spendings for given budget priority

// estimate "imperial administration" spendings which are based on total population of the realm
float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget);
// estimate total spendings for your administration
float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget);
// total current spendings
float full_spendings_administration(sys::state& state, dcon::nation_id n, float budget);
// register demand on labor or produce control
void update_consumption_administration(sys::state& state, dcon::nation_id n, float total_budget);

float capital_administration_control_production(sys::state& state, dcon::nation_id n, dcon::province_id capital);
float local_administration_control_production(sys::state& state, dcon::nation_id n, dcon::province_id p);
void update_production_administration(sys::state& state, dcon::nation_id n);
void refund_demand_administration(sys::state& state, dcon::nation_id n);

// perform a tax collection update
void collect_taxes(sys::state& state, ve::vectorizable_buffer<float, dcon::pop_id>& pop_income);

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

bool war_embargo_status(sys::state& state, dcon::nation_id n_a, dcon::nation_id n_b, dcon::nation_id market_leader_a, dcon::nation_id market_leader_b);
bool non_war_embargo_status(sys::state& state, dcon::nation_id n_a, dcon::nation_id n_b, dcon::nation_id market_leader_a, dcon::nation_id market_leader_b);
bool has_active_embargo(sys::state& state, dcon::nation_id from, dcon::nation_id to);

std::vector<employment_record> explain_local_administration_employment(sys::state& state, dcon::province_id p);
std::vector<employment_record> explain_capital_administration_employment(sys::state& state, dcon::nation_id n);
}
