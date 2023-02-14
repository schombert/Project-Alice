#pragma once

#include "dcon_generated.hpp"
#include <vector>

namespace culture {

enum class flag_type : uint8_t {
	republic, communist, fascist, monarchy
};

struct government_type {
	uint32_t ideologies_allowed = 0;
	dcon::text_sequence_id name;
	bool has_elections = false;
	bool can_appoint_ruling_party = false;
	int8_t duration = 0;
	flag_type flag = flag_type::republic;
};

constexpr inline uint32_t to_bits(dcon::ideology_id id) {
	if(id)
		return uint32_t(1 << id.index());
	else
		return 0;
}

struct crime_info {
	dcon::text_sequence_id name;
	dcon::modifier_id modifier;
	dcon::trigger_key trigger;
	bool available_by_default = false;
};

enum class tech_category : uint8_t {
	army, navy, commerce, culture, industry
};

struct folder_info {
	dcon::text_sequence_id name;
	tech_category category = tech_category::army;
};

enum class pop_strata : uint8_t {
	poor = 0, middle = 1, rich = 2
};
enum class income_type : uint8_t {
	none = 0, administration = 1, military = 2, education = 3, reforms = 4,
};

struct global_cultural_state {
	std::vector<dcon::issue_id> party_issues;
	std::vector<dcon::issue_id> political_issues;
	std::vector<dcon::issue_id> social_issues;
	std::vector<dcon::issue_id> military_issues;
	std::vector<dcon::issue_id> economic_issues;

	std::vector<folder_info> tech_folders; // contains *all* the folder names; techs index into this by an integer index

	tagged_vector<government_type, dcon::government_type_id> governments;
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

	int32_t officer_leadership_points = 0;
	float bureaucrat_tax_efficiency = 0.0f;

	//special ideology
	dcon::ideology_id conservative;
};

enum class issue_category {
	party, political, social, military, economic
};

}
