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
	// trigger id goes here
	bool available_by_default = false;
};

struct global_cultural_state {
	std::vector<dcon::issue_id> party_issues;
	std::vector<dcon::issue_id> political_issues;
	std::vector<dcon::issue_id> social_issues;
	std::vector<dcon::issue_id> military_issues;
	std::vector<dcon::issue_id> economic_issues;

	tagged_vector<government_type, dcon::government_type_id> governments;
	tagged_vector<crime_info, dcon::crime_id> crimes;
};

enum class issue_category {
	party, political, social, military, economic
};

}
