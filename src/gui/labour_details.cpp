#include "labour_details.hpp"
#include "economy_stats.hpp"

namespace ui {

// Return text key for tooltips on the provided labour type
std::string_view labour_type_to_employment_type_text_key(int32_t type) {
	// TODO: implement the rest of labour types
	if(type == economy::labor::high_education_and_accepted) {
		return "employment_type_high_education_and_accepted";
	} else if(type == economy::labor::high_education) {
		return "employment_type_high_education";
	} else if(type == economy::labor::guild_education) {
		return "employment_type_guild_education";
	} else if(type == economy::labor::basic_education) {
		return "employment_type_basic_education";
	} else if(type == economy::labor::no_education) {
		return "employment_type_no_education";
	}
	return "alice_invalid_value";
}

std::string_view labour_type_to_employment_name_text_key(int32_t labor_type) {
	switch(labor_type) {
	case economy::labor::no_education:
		return "alice_labor_no_education";
	case economy::labor::basic_education:
		return "alice_labor_basic_education";
	case economy::labor::high_education:
		return "alice_labor_high_education";
	case economy::labor::guild_education:
		return "alice_labor_guild";
	case economy::labor::high_education_and_accepted:
		return "alice_labor_high_education_and_accepted";
	default:
		return "alice_invalid_value";
	}
};

}
