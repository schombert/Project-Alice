#include "system_state.hpp"
#include "nations.hpp"
#include "parsers_declarations.hpp"

namespace parsers {
void national_identity_file::any_value(std::string_view tag, association_type, std::string_view txt, error_handler& err, int32_t line, scenario_building_context& context) {
	if(tag.length() != 3) {
		err.accumulated_errors += err.file_name + " line " + std::to_string(line) + ": encountered a tag that was not three characters\n";
		return;
	}
	auto as_int = nations::tag_to_int(tag[0], tag[1], tag[2]);
	auto new_ident = context.state.world.create_national_identity();
	context.file_names_for_idents.resize(context.state.world.national_identity_size());
	context.file_names_for_idents[new_ident] = txt;
	context.map_of_ident_names.insert_or_assign(as_int, new_ident);

}
}
