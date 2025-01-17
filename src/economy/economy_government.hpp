#pragma once
#include "dcon_generated.hpp"

namespace sys {
struct state;
}

namespace economy {
float tax_collection_capacity(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid);
float global_admin_ratio(sys::state& state, dcon::nation_id n);
float local_admin_ratio(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid);
float estimate_spendings_administration_capital(sys::state& state, dcon::nation_id n, float budget_priority);
float estimate_spendings_administration_state(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid, float budget_priority);
float estimate_spendings_administration(sys::state& state, dcon::nation_id n, float budget_priority);
float full_spendings_administration(sys::state& state, dcon::nation_id n);
void update_consumption_administration(sys::state& state, dcon::nation_id n);
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
tax_information explain_tax_income(sys::state& state, dcon::nation_id n);
tax_information explain_tax_income_local(sys::state& state, dcon::nation_id n, dcon::state_instance_id sid);
}
