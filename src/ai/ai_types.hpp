#pragma once
#include "dcon_generated.hpp"

namespace ai {
enum ai_strategies {
	industrious, militant, technological
};
struct weighted_state_instance {
	dcon::state_instance_id target;
	float weight;
};
struct possible_cb {
	dcon::nation_id target;
	dcon::nation_id secondary_nation;
	dcon::national_identity_id associated_tag;
	dcon::state_definition_id state_def;
	dcon::cb_type_id cb;
	float utility_expectation;
};
}
