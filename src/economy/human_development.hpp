#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"

namespace economy::human_development {

// The first Cities and Human Development slice deliberately derives its state
// from existing POP, service, and city data. It therefore needs no save-format
// migration and remains an exact no-op outside Age of Transformation.
struct config {
	bool enabled = false;
	float maximum_overcrowding_growth_penalty = 0.0012f;
	float maximum_transition_growth_reduction = 0.00035f;
	float maximum_overcrowding_militancy = 0.10f;
	float minimum_migration_multiplier = 0.70f;
	float maximum_migration_multiplier = 1.35f;
};

struct inputs {
	float housing_access = 1.f;
	float urbanization = 0.f;
	// Share of local labour supply that employers actually bought.  This is the
	// city-scale predecessor of a pathfinding commute model.
	float job_access = 1.f;
	float education_access = 0.f;
	float literacy = 0.f;
};

struct breakdown {
	bool enabled = false;
	inputs factors{};
	float housing_shortage = 0.f;
	float overcrowding = 0.f;
	float employment_exclusion = 0.f;
	float demographic_transition = 0.f;
	float human_development_index = 0.f;
	float monthly_overcrowding_growth_penalty = 0.f;
	float monthly_transition_growth_reduction = 0.f;
	float monthly_growth_adjustment = 0.f;
	float monthly_militancy_adjustment = 0.f;
	float migration_quality = 1.f;
};

struct demographic_account {
	float baseline_natural_growth = 0.f;
	float starvation_loss = 0.f;
	float housing_loss = 0.f;
	float transition_reduction = 0.f;
	float net_natural_change = 0.f;
};

config ruleset_config_for(sys::state const& state);
breakdown calculate(config const& rules, inputs raw_inputs);
breakdown evaluate_province(sys::state const& state, dcon::province_id province,
	float literacy);
breakdown evaluate_pop(sys::state const& state, dcon::pop_id pop);

float migration_multiplier(config const& rules, breakdown const& origin,
	breakdown const& target);
float migration_multiplier(sys::state const& state, dcon::province_id origin,
	dcon::province_id target);

demographic_account calculate_account(float population, float legacy_growth_modifier,
	float starvation_penalty, breakdown const& development);

} // namespace economy::human_development
