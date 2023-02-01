#pragma once

#include "dcon_generated.hpp"
#include <vector>

namespace culture {
struct global_cultural_state {
	std::vector<dcon::issue_id> party_issues;
	std::vector<dcon::issue_id> political_issues;
	std::vector<dcon::issue_id> social_issues;
	std::vector<dcon::issue_id> military_issues;
	std::vector<dcon::issue_id> economic_issues;
};

enum class issue_category {
	party, political, social, military, economic
};

}
