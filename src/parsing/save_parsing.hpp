#pragma once

#include <stdint.h>
#include <optional>
#include <string_view>
#include <string>

#include "constants.hpp"
#include "parsers.hpp"
#include "script_constants.hpp"
#include "nations.hpp"
#include "container_types.hpp"

namespace parsers {
struct save_parser_context {
	sys::state& state;
	dcon::nation_id current;
	save_parser_context(sys::state& state);
};
struct save_tax {
	float current = 0.f;
	void finish(save_parser_context& context) { }
};
struct save_relations {
	float value = 0.f;
	void finish(save_parser_context& context) { }
};
struct save_unit_path {
	void finish(save_parser_context& context) { }
};
struct save_regiment {
	void id(association_type, save_typed_id v, error_handler& err, int32_t line, save_parser_context& context);
	void name(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void organisation(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void strength(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void experience(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void type(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void finish(save_parser_context& context) { }
};
struct save_army {
	void id(association_type, save_typed_id v, error_handler& err, int32_t line, save_parser_context& context);
	void leader(association_type, save_typed_id v, error_handler& err, int32_t line, save_parser_context& context);
	void name(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void previous(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void movement_progress(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void life_rating(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void path(association_type, save_unit_path v, error_handler& err, int32_t line, save_parser_context& context);
	void location(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void dig_in_last_date(association_type, sys::year_month_day v, error_handler& err, int32_t line, save_parser_context& context);
	void supplies(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void regiment(association_type, save_regiment& v, error_handler& err, int32_t line, save_parser_context& context);
	void finish(save_parser_context& context) { }
};
struct save_ship {
	void id(association_type, save_typed_id v, error_handler& err, int32_t line, save_parser_context& context);
	void name(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void organisation(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void strength(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void experience(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void type(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void finish(save_parser_context& context) { }
};
struct save_navy {
	void id(association_type, save_typed_id v, error_handler& err, int32_t line, save_parser_context& context);
	void leader(association_type, save_typed_id v, error_handler& err, int32_t line, save_parser_context& context);
	void name(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void previous(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void movement_progress(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void life_rating(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void path(association_type, save_unit_path v, error_handler& err, int32_t line, save_parser_context& context);
	void location(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void supplies(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void ship(association_type, save_ship& v, error_handler& err, int32_t line, save_parser_context& context);
	void finish(save_parser_context& context) { }
};
struct save_province {
	void name(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void owner(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void controller(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void core(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void colonial(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void life_rating(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void army(association_type, save_army v, error_handler& err, int32_t line, save_parser_context& context);
	void navy(association_type, save_navy v, error_handler& err, int32_t line, save_parser_context& context);
	void rgo(association_type, save_rgo v, error_handler& err, int32_t line, save_parser_context& context);
	void army(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void any_group(std::string_view name, save_pop v, error_handler& err, int32_t line, save_parser_context& context);
	void finish(save_parser_context& context) { }
};
struct save_country {
	void rich_tax(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void middle_tax(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void poor_tax(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void education_spending(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void crime_fighting(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void social_spending(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void military_spending(association_type, save_tax& v, error_handler& err, int32_t line, save_parser_context& context);
	void leadership(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void revanchism(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void plurality(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void diplomatic_points(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void badboy(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void suppression(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void ruling_party(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void research_points(association_type, float v, error_handler& err, int32_t line, save_parser_context& context);
	void civilized(association_type, bool v, error_handler& err, int32_t line, save_parser_context& context);
	void money(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void primary_culture(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void religion(association_type, std::string_view v, error_handler& err, int32_t line, save_parser_context& context);
	void last_election(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, save_parser_context& context);
	void last_bankrupt(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, save_parser_context& context);
	void any_group(std::string_view name, save_relations, error_handler& err, int32_t line, save_parser_context& context);
};
struct save_file {
	void date(association_type, sys::year_month_day ymd, error_handler& err, int32_t line, save_parser_context& context);
	void player(association_type, uint32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void government(association_type, int32_t v, error_handler& err, int32_t line, save_parser_context& context);
	void finish(save_parser_context& context) { }
};
} //namespace parsers
