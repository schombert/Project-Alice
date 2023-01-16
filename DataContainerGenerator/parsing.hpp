#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <limits>

struct char_range {
	char const* start;
	char const* end;

	std::string to_string() const {
		return std::string(start, end);
	}
};

struct parsed_item {
	char_range key;
	std::vector<char_range> values;
	char const* terminal;
};

struct row_col_pair {
	int32_t row;
	int32_t column;
};

struct error_record {
	std::string accumulated;
	std::string file_name;

	error_record(std::string const& fn) : file_name(fn) {}

	void add(row_col_pair const& rc, int32_t code, std::string const& s) {
		//<origin>[(position)]: [category] <kind> <code>: <description>

		accumulated += file_name;
		if(rc.row > 0) {
			if(rc.column > 0) {
				accumulated += "(" + std::to_string(rc.row) + "," + std::to_string(rc.column) + ")";
			} else {
				accumulated += "(" + std::to_string(rc.row) + ")";
			}
		} else {

		}
		accumulated += ": error ";
		accumulated += std::to_string(code);
		accumulated += ": ";
		accumulated += s;
		accumulated += "\n";
	}
};

char const* advance_to_closing_bracket(char const* pos, char const* end);
char const* advance_to_non_whitespace(char const* pos, char const* end);
char const* advance_to_whitespace(char const* pos, char const* end);
char const* advance_to_whitespace_or_brace(char const* pos, char const* end);
char const* reverse_to_non_whitespace(char const* start, char const* pos);

row_col_pair calculate_line_from_position(char const* start, char const* pos);
parsed_item extract_item(char const* input, char const* end, char const* global_start, error_record& err);


enum class storage_type { contiguous, erasable, compactable };
enum class property_type { vectorizable, other, object, special_vector, bitfield,
	array_vectorizable, array_bitfield, array_other
};
enum class protection_type { none, hidden, read_only };

struct member_function_spec{
	std::string signature;
	std::string name;
	std::vector<std::string> parameter_names;
	bool is_const = false;
};

struct property_def {
	std::string name;

	property_type type = property_type::other;
	protection_type protection = protection_type::none;
	bool is_derived = false;

	int special_pool_size = 1000;
	std::string data_type;

	std::string array_index_type;

	bool hook_get = false;
	bool hook_set = false;

	std::vector<std::string> property_tags;
};

enum class index_type { many, at_most_one, none };
enum class list_type { list, array, std_vector };

struct relationship_object_def;

struct related_object {
	std::string property_name;
	std::string type_name;
	int32_t multiplicity = 1;
	index_type index = index_type::at_most_one;
	list_type ltype = list_type::list;
	bool is_optional = false;
	bool is_distinct = false;
	bool is_covered_by_composite_key = false;
	bool is_primary_key = false;
	protection_type protection = protection_type::none;
	relationship_object_def* related_to = nullptr;
};

struct in_relation_information {
	std::string relation_name;
	related_object const* linked_as;
	relationship_object_def const* rel_ptr;
};

struct key_component {
	std::string property_name;
	std::string object_type;
	int32_t multiplicity = 1;
	int32_t number_of_bits = 0;
	int32_t bit_position = 0;
};

struct composite_index_def {
	std::string name;
	bool involves_primary_key = false;
	int32_t total_bytes;
	std::vector<key_component> component_indexes;
};

struct primary_key_type {
	std::string property_name;
	relationship_object_def const* points_to = nullptr;

	bool operator==(related_object const& other) const {
		return points_to == other.related_to && other.property_name == property_name;
	}
	bool operator!=(related_object const& other) const {
		return points_to != other.related_to || other.property_name != property_name;
	}
};



struct relationship_object_def {
	std::string name;
	std::string force_pk;
	bool is_relationship = false;
	std::vector<std::string> obj_tags;
	size_t size = 1000;
	bool is_expandable = false;
	storage_type store_type = storage_type::contiguous;

	bool hook_create = false;
	bool hook_delete = false;
	bool hook_move = false;

	std::vector<related_object> indexed_objects;
	std::vector<property_def> properties;
	std::vector<in_relation_information> relationships_involved_in;
	std::vector<composite_index_def> composite_indexes;
	std::vector< member_function_spec> member_functions;

	primary_key_type primary_key;
};

enum class filter_type { default_exclude, exclude, include };

struct load_save_def {
	std::string name;
	filter_type properties_filter = filter_type::default_exclude;
	filter_type objects_filter = filter_type::default_exclude;
	std::vector<std::string> obj_tags;
	std::vector<std::string> property_tags;
};

struct conversion_def {
	std::string from;
	std::string to;
};
 
struct made_id {
	std::string name;
	std::string base_type;
};


related_object parse_link_def(char const* start, char const* end, char const* global_start, error_record& err_out);
property_def parse_property_def(char const* start, char const* end, char const* global_start, error_record& err_out);
relationship_object_def parse_relationship(char const* start, char const* end, char const* global_start, error_record& err_out);
relationship_object_def parse_object(char const* start, char const* end, char const* global_start, error_record& err_out);
std::vector<std::string> parse_legacy_types(char const* start, char const* end, char const* global_start, error_record& err_out);
conversion_def parse_conversion_def(char const* start, char const* end, char const* global_start, error_record& err_out);
load_save_def parse_load_save_def(char const* start, char const* end, char const* global_start, error_record& err_out);


inline std::vector<std::string> common_types{ std::string("int8_t"), std::string("uint8_t"), std::string("int16_t"), std::string("uint16_t")
	, std::string("int32_t"), std::string("uint32_t"), std::string("int64_t"), std::string("uint64_t"), std::string("float"), std::string("double") };

inline std::string normalize_type(std::string const& in) {
	if(in == "char" || in == "unsigned char" || in == "bool")
		return "uint8_t";
	if(in == "signed char")
		return "int8_t";
	if(in == "short")
		return "int16_t";
	if(in == "unsigned short")
		return "uint16_t";
	if(in == "int" || in == "long")
		return "int32_t";
	if(in == "unsigned int" || in == "unsigned long")
		return "uint32_t";
	if(in == "size_t" || in == "unsigned long long")
		return "uint64_t";
	if(in == "long long")
		return "int64_t";
	else
		return in;
}

inline bool is_common_type(std::string const& in) {
	return std::find(common_types.begin(), common_types.end(), normalize_type(in)) != common_types.end();
}

inline std::string size_to_tag_type(size_t sz) {
	if(sz == 0) {
		return "uint32_t";
	} else if(sz <= 126) {
		return "uint8_t";
	} else if(sz <= std::numeric_limits<int16_t>::max() - 1) {
		return "uint16_t";
	}
	return "uint32_t";
}

struct qualified_name {
	std::string type_name;
	std::string member_name;
	std::string as_name;
};

qualified_name parse_qual_name(char const* &start, char const * end, char const * global_start, error_record & err);

struct selection_item {
	qualified_name property;
	std::string aggregate_name;
	bool is_aggregate = false;
};

selection_item parse_selection_item(char const* &start, char const * end, char const * global_start, error_record & err);

std::vector<selection_item> parse_all_selection_items(char const* &start, char const * end, char const * global_start, error_record & err);

enum class from_type { none, join, join_plus, parameter };

struct from_item {
	qualified_name table_identifier;

	std::string left_of_join;
	std::string join_on;

	from_type type = from_type::none;
};

from_item parse_from_item(char const* &start, char const * end, char const * global_start, error_record & err);
std::vector<from_item> parse_all_from_items(char const* &start, char const * end, char const * global_start, error_record & err);

struct select_statement_definition {
	std::vector<selection_item> returned_values;
	std::vector<from_item> from;
	std::string where_clause;
	std::string group_by;
};

select_statement_definition parse_select_statement(char const* &start, char const * end, char const * global_start, error_record & err);

struct type_name_pair {
	std::string name;
	std::string type;
};

type_name_pair parse_type_and_name(char const* start, char const * end);

struct query_definition {
	select_statement_definition select;
	std::vector<type_name_pair> parameters;
	std::string name;
	row_col_pair line = row_col_pair{ 0,0 };
};

query_definition parse_query_definition(char const* &start, char const * end, char const * global_start, error_record & err);

struct query_table_slot {
	std::string reference_name;
	std::string internally_named_as;
	
	relationship_object_def const* actual_table = nullptr;
	query_table_slot const* joined_to = nullptr;
	related_object const* joind_by_link = nullptr;

	bool is_parameter_type = false;
	bool is_join_plus = false;
	bool is_group_slot = false;
	bool expose_has_name = false;
};

struct perpared_selection_item {
	std::string exposed_name;

	property_def const* from_property;
	related_object const* from_link;
	query_table_slot const* derived_from_slot;
};

struct perpared_aggregate_item {
	std::string aggregate_name;
	std::string exposed_name;

	property_def const* from_property;
	related_object const* from_link;
	query_table_slot const* derived_from_slot;
};


struct prepared_query_definition {
	std::vector<perpared_selection_item> exposed_values;
	std::vector<perpared_aggregate_item> exposed_aggregates;
	std::vector<perpared_selection_item> exposed_min_max_terms;

	std::vector<query_table_slot> table_slots;
	std::vector<type_name_pair> parameters;

	std::string where_conditional;
	std::string name;
	bool has_group = false;
};

struct file_def {
	std::string namspace = "dcon";
	std::vector<std::string> includes;
	std::vector<std::string> globals;
	std::vector<std::string> legacy_types;
	std::vector<made_id> extra_ids;

	std::vector<relationship_object_def> relationship_objects;
	std::vector<load_save_def> load_save_routines;
	std::vector<conversion_def> conversion_list;

	std::vector<std::string> object_types;

	std::vector< query_definition> unprepared_queries;
	std::vector< prepared_query_definition> prepared_queries;
};

prepared_query_definition make_prepared_definition(file_def const& parsed_file, query_definition const& def, error_record & err);

file_def parse_file(char const* start, char const* end, error_record& err_out);

inline relationship_object_def const* find_by_name(file_def const& def, std::string const& name) {
	if(auto r = std::find_if(
		def.relationship_objects.begin(), def.relationship_objects.end(),
		[&name](relationship_object_def const& o) { return o.name == name; }); r != def.relationship_objects.end()) {
		return &(*r);
	}
	return nullptr;
}

inline relationship_object_def* find_by_name(file_def& def, std::string const& name) {
	if(auto r = std::find_if(
		def.relationship_objects.begin(), def.relationship_objects.end(),
		[&name](relationship_object_def const& o) { return o.name == name; }); r != def.relationship_objects.end()) {
		return &(*r);
	}
	return nullptr;
}

inline std::string make_relationship_parameters(relationship_object_def const& o) {
	std::string result;
	for(auto& i : o.indexed_objects) {
		if(result.length() != 0)
			result += ", ";
		if(i.multiplicity == 1) {
			result += i.type_name + "_id " + i.property_name + "_p";
		} else {
			result += i.type_name + "_id " + i.property_name + "_p0";
			for(int32_t j = 1; j < i.multiplicity; ++j) {
				result += ", " + i.type_name + "_id " + i.property_name + "_p" + std::to_string(j);
			}
		}
	}
	return result;
}

inline bool is_vectorizable_type(file_def const& def, std::string const& name) {
	return name == "char" || name == "bool" || name == "int" || name == "short" || name == "unsigned short" ||
		name == "unsigned int" || name == "unsigned char" || name == "signed char" ||
		name == "float" || name == "int8_t" || name == "uint8_t" ||
		name == "int16_t" || name == "uint16_t" || name == "int32_t" || name == "uint32_t"
		|| std::find_if(def.extra_ids.begin(), def.extra_ids.end(), [&](made_id const& mi) {
		return mi.name == name; }) != def.extra_ids.end()
			|| [&]() {
			return name.length() >= 4 && name[name.length() - 1] == 'd' && name[name.length() - 2] == 'i' &&
				name[name.length() - 3] == '_' && find_by_name(def, name.substr(0, name.length() - 3)) != nullptr;
		}();
}