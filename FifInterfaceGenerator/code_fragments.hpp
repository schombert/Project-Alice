#pragma once

#include <optional>
#include <cstring>
#include "source_builder.hpp"
#include "parsing.hpp"

std::string type_to_fif_type(std::string const& tin);
bool known_as_fif_type(std::string const& tin);
std::string offset_of_member_container(std::string const& object_name, std::string const& member_name);
std::string array_member_leading_padding(std::string const& member_type, size_t raw_size, bool is_bitfield);
std::string array_member_trailing_padding(std::string const& member_type, size_t raw_size, bool is_bitfield);
std::string array_member_row_size(std::string const& member_type, size_t raw_size, bool is_bitfield);
std::string offset_of_array_member_container(std::string const& object_name, std::string const& member_name);

std::string make_id_definition(std::string const& type_name, std::string const& underlying_type);