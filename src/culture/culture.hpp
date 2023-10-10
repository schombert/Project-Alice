#pragma once

#include "dcon_generated.hpp"
#include <vector>

namespace issue_rule {

constexpr inline uint32_t build_factory = 0x00000001;
constexpr inline uint32_t expand_factory = 0x00000002;
constexpr inline uint32_t open_factory = 0x00000004;
constexpr inline uint32_t destroy_factory = 0x00000008;
constexpr inline uint32_t factory_priority = 0x00000010;
constexpr inline uint32_t can_subsidise = 0x00000020;
constexpr inline uint32_t pop_build_factory = 0x00000040;
constexpr inline uint32_t pop_expand_factory = 0x00000080;
constexpr inline uint32_t pop_open_factory = 0x00000100;
constexpr inline uint32_t delete_factory_if_no_input = 0x00000200;
constexpr inline uint32_t build_factory_invest = 0x00000400;
constexpr inline uint32_t expand_factory_invest = 0x00000800;
constexpr inline uint32_t open_factory_invest = 0x00001000;
constexpr inline uint32_t build_railway_invest = 0x00002000;
constexpr inline uint32_t can_invest_in_pop_projects = 0x00004000;
constexpr inline uint32_t pop_build_factory_invest = 0x00008000;
constexpr inline uint32_t pop_expand_factory_invest = 0x00010000;
constexpr inline uint32_t pop_open_factory_invest = 0x00020000;
constexpr inline uint32_t allow_foreign_investment = 0x00040000;
constexpr inline uint32_t slavery_allowed = 0x00080000;
constexpr inline uint32_t primary_culture_voting = 0x00100000;
constexpr inline uint32_t culture_voting = 0x00200000;
constexpr inline uint32_t all_voting = 0x00400000;
constexpr inline uint32_t largest_share = 0x00800000;
constexpr inline uint32_t dhont = 0x01000000;
constexpr inline uint32_t sainte_laque = 0x02000000;
constexpr inline uint32_t same_as_ruling_party = 0x04000000;
constexpr inline uint32_t rich_only = 0x08000000;
constexpr inline uint32_t state_vote = 0x10000000;
constexpr inline uint32_t population_vote = 0x20000000;
constexpr inline uint32_t build_railway = 0x40000000;
// TODO: Add more bits to issue rules so we can accomodate both banks and universities SEPARATEDLY
// "Well the idea we had in mind is that universities are more of a state thing that you can build regardless of party, while banks of course are more tied to party policies"
constexpr inline uint32_t build_bank = 0x40000000; // tied to building railroads
constexpr inline uint32_t build_university = 0x80000000; // independent

} // namespace issue_rule

namespace culture {

enum class flag_type : uint8_t {
	default_flag = 0,
	republic,
	communist,
	fascist,
	monarchy,
	// Non-vanilla flags
	theocracy,
	special,
	spare,
	populist,
	realm,
	other,
	monarchy2,
	monarchy3,
	republic2,
	republic3,
	communist2,
	communist3,
	fascist2,
	fascist3,
	theocracy2,
	theocracy3,
	cosmetic_1,
	cosmetic_2,
	colonial,
	nationalist,
	sectarian,
	socialist,
	dominion,
	agrarism,
	national_syndicalist,
	theocratic,
	count
};

constexpr inline uint64_t to_bits(dcon::ideology_id id) {
	if(id)
		return uint64_t(1 << id.index());
	else
		return 0;
}

struct crime_info {
	dcon::text_sequence_id name;
	dcon::modifier_id modifier;
	dcon::trigger_key trigger;
	bool available_by_default = false;
};

enum class tech_category : uint8_t { army, navy, commerce, culture, industry, unknown };

struct folder_info {
	dcon::text_sequence_id name;
	tech_category category = tech_category::army;
};

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
uint32_t get_remapped_flag_type(sys::state const& state, flag_type type);
flag_type get_current_flag_type(sys::state const& state, dcon::nation_id target_nation);
flag_type get_current_flag_type(sys::state const& state, dcon::national_identity_id identity);
void update_nation_issue_rules(sys::state& state, dcon::nation_id n_id); // note: does react to changes in slavery rule
void update_all_nations_issue_rules(sys::state& state);									 // note: doesn't react to changes in slavery rule

void create_initial_ideology_and_issues_distribution(sys::state& state);
void set_default_issue_and_reform_options(sys::state& state);
void restore_unsaved_values(sys::state& state);

float effective_technology_cost(sys::state& state, uint32_t current_year, dcon::nation_id target_nation, dcon::technology_id tech_id);
void update_research(sys::state& state, uint32_t current_year);
void discover_inventions(sys::state& state);
void fix_slaves_in_province(sys::state& state, dcon::nation_id owner, dcon::province_id p);

void replace_cores(sys::state& state, dcon::national_identity_id old_tag, dcon::national_identity_id new_tag);

} // namespace culture
