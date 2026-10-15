#pragma once

#include "dcon_generated_ids.hpp"
#include "system_state_forward.hpp"
#include <vector>
#include "container_types.hpp"

namespace culture {

constexpr inline uint64_t to_bits(dcon::ideology_id id) {
	if(id)
		return uint64_t(uint64_t(1) << uint64_t(id.index()));
	else
		return 0;
}

struct crime_info {
	dcon::text_key name;
	dcon::modifier_id modifier;
	dcon::trigger_key trigger;
	bool available_by_default = false;
};

enum class tech_category : uint8_t { army, navy, commerce, culture, industry, military_theory, population, diplomacy, flavor, unknown, total };

std::string get_tech_category_name(tech_category t);
std::vector<culture::tech_category> get_active_tech_categories(sys::state& state);

struct folder_info {
	dcon::text_key name;
	tech_category category = tech_category::army;
	uint8_t padding[3] = { 0 };
	folder_info(dcon::text_key _name, tech_category _category) : name(_name), category(_category) { }
	folder_info() { }
	bool operator==(const folder_info& other) const = default;
};
// initialize padding to 0 since the scenario checksum will bitwise checksum it
static_assert(sizeof(folder_info) == sizeof(folder_info::name) + sizeof(folder_info::category) + sizeof(folder_info::padding));

enum class pop_strata : uint8_t { poor = 0, middle = 1, rich = 2 };
enum class income_type : uint8_t {
	none = 0,
	administration = 1,
	military = 2,
	education = 3,
	reforms = 4,
};
enum class issue_type : uint8_t { party = 0, political = 1, social = 2, military = 3, economic = 4 };
struct global_cultural_state {
	std::vector<dcon::issue_id> party_issues;
	std::vector<dcon::issue_id> political_issues;
	std::vector<dcon::issue_id> social_issues;
	std::vector<dcon::reform_id> military_issues;
	std::vector<dcon::reform_id> economic_issues;

	std::vector<folder_info> tech_folders; // contains *all* the folder names; techs index into this by an integer index
	std::vector<dcon::pop_type_id> rgo_workers;

	tagged_vector<crime_info, dcon::crime_id> crimes;

	// these are not intended to be a complete list of *all* poptypes, just those with special rules
	dcon::pop_type_id artisans;
	dcon::pop_type_id capitalists;
	dcon::pop_type_id farmers;
	dcon::pop_type_id laborers;
	dcon::pop_type_id clergy;
	dcon::pop_type_id soldiers;
	dcon::pop_type_id officers;
	dcon::pop_type_id slaves;
	dcon::pop_type_id bureaucrat;
	dcon::pop_type_id aristocrat;

	dcon::pop_type_id primary_factory_worker;
	dcon::pop_type_id secondary_factory_worker;

	int32_t officer_leadership_points = 0;
	float bureaucrat_tax_efficiency = 0.0f;

	// special ideology
	dcon::ideology_id conservative;
	// special issue
	dcon::issue_option_id jingoism;

	// from pop_types.txt
	dcon::value_modifier_key promotion_chance;
	dcon::value_modifier_key demotion_chance;
	dcon::value_modifier_key migration_chance;
	dcon::value_modifier_key colonialmigration_chance;
	dcon::value_modifier_key emigration_chance;
	dcon::value_modifier_key assimilation_chance;
	dcon::value_modifier_key conversion_chance;

	uint64_t promotion_chance_fn = 0;
	uint64_t demotion_chance_fn = 0;
	uint64_t migration_chance_fn = 0;
	uint64_t colonialmigration_chance_fn = 0;
	uint64_t emigration_chance_fn = 0;
	uint64_t assimilation_chance_fn = 0;
	uint64_t conversion_chance_fn = 0;
};

enum class issue_category : uint8_t { party, political, social, military, economic };

enum class rebel_area : uint8_t { none = 0, nation, culture, nation_culture, nation_religion, religion, culture_group, all };
enum class rebel_defection : uint8_t { none = 0, culture, culture_group, religion, ideology, any, pan_nationalist };
enum class rebel_independence : uint8_t { none = 0, culture, culture_group, religion, colonial, any, pan_nationalist };

// these functions are to be called only after loading a save
void clear_existing_tech_effects(sys::state& state);
void repopulate_technology_effects(sys::state& state);
void repopulate_invention_effects(sys::state& state);


void apply_technology(sys::state& state, dcon::nation_id target_nation, dcon::technology_id tech_id);
void apply_invention(sys::state& state, dcon::nation_id target_nation, dcon::invention_id inv_id);
void remove_technology(sys::state& state, dcon::nation_id target_nation, dcon::technology_id tech_id);
void remove_invention(sys::state& state, dcon::nation_id target_nation, dcon::invention_id inv_id);
dcon::government_flag_id  get_current_flag_type(sys::state const& state, dcon::nation_id target_nation);
dcon::government_flag_id  get_current_flag_type(sys::state const& state, dcon::national_identity_id identity);
void update_nation_issue_rules(sys::state& state, dcon::nation_id n_id); // note: does react to changes in slavery rule
void update_all_nations_issue_rules(sys::state& state);									 // note: doesn't react to changes in slavery rule

void create_initial_ideology_and_issues_distribution(sys::state& state);
void set_default_issue_and_reform_options(sys::state& state);
void restore_unsaved_values(sys::state& state);

float effective_technology_lp_cost(sys::state& state, uint32_t current_year, dcon::nation_id target_nation, dcon::technology_id tech_id);
float effective_technology_rp_cost(sys::state& state, uint32_t current_year, dcon::nation_id target_nation, dcon::technology_id tech_id);
void update_research(sys::state& state, uint32_t current_year);
void discover_inventions(sys::state& state);
void fix_slaves_in_province(sys::state& state, dcon::nation_id owner, dcon::province_id p);

void replace_cores(sys::state& state, dcon::national_identity_id old_tag, dcon::national_identity_id new_tag);

} // namespace culture
