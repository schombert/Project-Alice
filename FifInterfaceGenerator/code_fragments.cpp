#include "code_fragments.hpp"

std::string type_to_fif_type(std::string const& tin) {
	if(tin == "uint32_t")
		return "u32";
	else if(tin == "int32_t")
		return "i32";
	else if(tin == "int16_t")
		return "i16";
	else if(tin == "uint16_t")
		return "u16";
	else if(tin == "int8_t")
		return "i8";
	else if(tin == "uint8_t")
		return "u8";
	else if(tin == "int64_t")
		return "i64";
	else if(tin == "uint64_t")
		return "u64";
	else if(tin == "float")
		return "f32";
	else if(tin == "double")
		return "f64";
	else if(tin == "bool")
		return "bool";
	else
		return tin;
}
bool known_as_fif_type(std::string const& tin) {
	if(tin == "uint32_t")
		return true;
	else if(tin == "int32_t")
		return true;
	else if(tin == "int16_t")
		return true;
	else if(tin == "uint16_t")
		return true;
	else if(tin == "int8_t")
		return true;
	else if(tin == "uint8_t")
		return true;
	else if(tin == "int64_t")
		return true;
	else if(tin == "uint64_t")
		return true;
	else if(tin == "float")
		return true;
	else if(tin == "double")
		return true;
	else if(tin == "bool")
		return true;
	else
		return false;
}

std::string make_id_definition(std::string const& type_name, std::string const& underlying_type) {
	return
		std::string("\":struct ") + type_name + " " + type_to_fif_type(underlying_type) + " ival ; \"\n"
		+ std::string("\":s >index ") + type_name + " s: .ival >i32 -1 + ; \"\n"
		+ std::string("\":s >") + type_name + " i32 s: 1 + >" + type_to_fif_type(underlying_type) + " make " + type_name + " .ival! ; \"\n"
		+ std::string("\":s = ") + type_name + " " + type_name + " s: .ival swap .ival = ; \"\n"
		+ std::string("\":s @ ") + type_name + " s:  ; \"\n"
		+ std::string("\":s valid? ") + type_name + " s: .ival 0 <> ; \"\n"
	;
}

std::string offset_of_member_container(std::string const& object_name, std::string const& member_name) {
	std::string class_name = "dcon::internal::" + object_name + "_class";
	std::string offset = "offsetof(dcon::data_container, " + object_name + ") "
		"+ offsetof(" + class_name + ", m_" + member_name + ")"
		"+ offsetof(dcon::internal::" + object_name + "_class::dtype_" + member_name + ", values)";
	
	return "\" + std::to_string(" + offset + ") + \"";
}

std::string offset_of_array_member_container(std::string const& object_name, std::string const& member_name) {
	std::string class_name = "dcon::internal::" + object_name + "_class";
	std::string offset = "offsetof(dcon::data_container, " + object_name + ") "
		"+ offsetof(" + class_name + ", m_" + member_name + ")"
		"+ offsetof(dcon::internal::" + object_name + "_class::dtype_" + member_name + ", values)";

	return "\" + std::to_string(" + offset + ") + \"";
}
std::string array_member_leading_padding(std::string const& member_type, size_t , bool is_bitfield) {
	return is_bitfield ? std::string("64") : std::string("\" + std::to_string(") + std::string("sizeof(") + member_type + ") + 64 - (sizeof(" + member_type + ") & 63)" + std::string(") + \"");
}
std::string array_member_trailing_padding(std::string const& member_type, size_t raw_size, bool is_bitfield) {
	return std::string("\" + std::to_string(") + (is_bitfield ? std::string("64 - (( (") + std::to_string(raw_size) + " + 7) / 8) & 63)" : std::string("64 - ((") + std::to_string(raw_size) + " * sizeof(" + member_type + ")) & 63)") + std::string(") + \"");
}
std::string array_member_row_size(std::string const& member_type, size_t raw_size, bool is_bitfield) {
	std::string leading_padding = is_bitfield ? std::string("64") : std::string("sizeof(") + member_type + ") + 64 - (sizeof(" + member_type + ") & 63)";
	std::string trailing_padding = is_bitfield ? std::string("64 - (( (") + std::to_string(raw_size) + " + 7) / 8) & 63)" : std::string("64 - ((") + std::to_string(raw_size) + " * sizeof(" + member_type + ")) & 63)";
	std::string row_size = std::string("(") + (
		is_bitfield
		? leading_padding + " + " + std::string("(") + std::to_string(raw_size) + " + 7) / 8" + " + " + trailing_padding
		: leading_padding + " + " + std::string("sizeof(") + member_type + ") * " + std::to_string(raw_size) + " + " + trailing_padding)
		+ std::string(")");
	return std::string("\" + std::to_string(") + row_size + std::string(") + \"");
}

