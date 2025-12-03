#pragma once

#include <string>


namespace economy {
struct commodity_set;
struct small_commodity_set;
}

namespace military {
struct ship_in_battle;
struct mobilization_order;
struct reserve_regiment;
struct available_cb;
}

namespace sys {

struct event_option;
struct gamerule_option;
struct provincial_modifier_definition;
struct national_modifier_definition;
struct commodity_modifier;
struct rebel_org_modifier;
struct dated_modifier;

std::string to_debug_string(const event_option& obj);

std::string to_debug_string(const gamerule_option& obj);


std::string to_debug_string(const economy::commodity_set& obj);


std::string to_debug_string(const economy::small_commodity_set& obj);

std::string to_debug_string(const provincial_modifier_definition& obj);


std::string to_debug_string(const national_modifier_definition& obj);


std::string to_debug_string(const commodity_modifier& obj);

std::string to_debug_string(const rebel_org_modifier& obj);

std::string to_debug_string(const dated_modifier& obj);

std::string to_debug_string(const military::ship_in_battle& obj);

std::string to_debug_string(const military::mobilization_order& obj);

std::string to_debug_string(const military::reserve_regiment& obj);

std::string to_debug_string(const military::available_cb& obj);

}

