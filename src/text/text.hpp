#pragma once

#include <stdint.h>
#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "unordered_dense.h"

namespace sys {
struct state;
}

namespace text {
enum class text_color : uint8_t {
	black,
	white,
	red,
	green,
	yellow,
	unspecified,
	light_blue,
	dark_blue,
	orange,
	lilac,
	light_grey,
	dark_red,
	dark_green,
	reset
};
enum class alignment : uint8_t { left, right, center };
enum class variable_type : uint16_t {
	error_no_matching_value,
	action,
	active,
	actor,
	adj,
	against,
	agressor,
	allowed,
	amount,
	anyprovince,
	army,
	army_name,
	astate,
	attacker,
	attunit,
	bac,
	badboy,
	badword,
	base,
	base_percentage,
	bat,
	bld,
	bon,
	bonus,
	brig,
	bud,
	building,
	buy,
	cap,
	capital,
	cash,
	casus,
	cat,
	category,
	cb_target_name,
	cb_target_name_adj,
	chan,
	chance,
	change,
	commander,
	con,
	construction,
	continentname,
	cost,
	count,
	countries,
	country,
	country1,
	country2,
	countryadj,
	countryculture,
	countryname,
	country_adj,
	creator,
	credits,
	crisisarea,
	crisisattacker,
	crisisdefender,
	crisistaker,
	crisistaker_adj,
	crisistarget,
	crisistarget_adj,
	cul,
	culture,
	culture_group_union,
	curr,
	current,
	d,
	date,
	date_long_0,
	date_long_1,
	date_short_0,
	day,
	days,
	defender,
	defunit,
	desc,
	dest,
	details,
	direction,
	dist,
	effect,
	effects,
	emilist,
	employees,
	employee_max,
	enemy,
	engineermaxunits,
	engineerunits,
	escorts,
	event,
	eventdesc,
	exc,
	exp,
	faction,
	factory,
	first,
	focus,
	folder,
	vtype_for,
	fraction,
	vtype_friend,
	from,
	fromcountry,
	fromcountry_adj,
	fromprovince,
	fromruler,
	funds,
	goal,
	good,
	goods,
	gov,
	government,
	govt,
	gp,
	gp_adj,
	group,
	high_tax,
	hit,
	home,
	hull,
	iamount,
	icountry,
	ide,
	ideology,
	ilocation,
	immlist,
	impact,
	iname,
	income,
	indep,
	inf,
	infamy,
	input,
	inv,
	invention,
	invested,
	invested_in_us_message,
	issue,
	law,
	leader,
	lev,
	level,
	levels,
	lim,
	limit,
	list,
	literacy,
	loc,
	local,
	location,
	lord,
	lose,
	low_tax,
	lvl,
	m,
	max,
	maxloan,
	men,
	messenger,
	mil,
	militancy,
	min,
	monarchtitle,
	money,
	month,
	months,
	movement,
	much,
	name,
	nation,
	nationalvalue,
	natives,
	navy,
	navy_name,
	need,
	needed,
	negative,
	neut,
	vtype_new,
	newcountry,
	nf,
	now,
	num,
	number,
	numfactories,
	numspecialfactories,
	odds,
	old,
	oldcountry,
	vtype_operator,
	opinion,
	opposing_army,
	opposing_navy,
	opressor,
	opt,
	optimal,
	option,
	order,
	org,
	organisation,
	other,
	otherresult,
	ourcapital,
	ourcountry,
	ourcountry_adj,
	our_lead,
	our_num,
	our_res,
	output,
	overlord,
	param,
	party,
	passive,
	pay,
	pen,
	penalty,
	per,
	perc,
	perc2,
	percent,
	percentage,
	player,
	policy,
	pop,
	poptype,
	popularity,
	position,
	positive,
	power,
	pres,
	prescence,
	prestige,
	produced,
	producer,
	prog,
	progress,
	proj,
	prov,
	provinceculture,
	provincename,
	provincereligion,
	provinces,
	provs,
	province,
	rank,
	rate,
	rec,
	recipient,
	reconmaxunits,
	reconunits,
	reform,
	region,
	rel,
	relation,
	req,
	reqlevel,
	required,
	resource,
	result,
	rstate,
	rule,
	runs,
	scr,
	sea,
	second,
	second_country,
	self,
	sell,
	setting,
	ships,
	size,
	skill,
	source,
	speed,
	spheremaster,
	state,
	statename,
	str,
	strata,
	strings_list_4,
	string_0_0,
	string_0_1,
	string_0_2,
	string_0_3,
	string_0_4,
	string_9_0,
	sub,
	table,
	tag,
	tag0_0,
	tag_0_0,
	tag_0_0_,
	tag_0_0_adj,
	tag_0_0_upper,
	tag_0_1,
	tag_0_1_adj,
	tag_0_1_upper,
	tag_0_2,
	tag_0_2_adj,
	tag_0_3,
	tag_0_3_adj,
	tag_1_0,
	tag_2_0,
	tag_2_0_upper,
	tag_3_0,
	tag_3_0_upper,
	target,
	targetlist,
	target_country,
	tech,
	temperature,
	terms,
	terrain,
	terrainmod,
	text,
	theirlost,
	theirnum,
	theirship,
	their_lead,
	their_num,
	their_res,
	them,
	third,
	threat,
	time,
	title,
	to,
	tot,
	totalemi,
	totalimm,
	tra,
	truth,
	type,
	unemployed,
	vtype_union,
	union_adj,
	unit,
	units,
	until,
	usloss,
	usnum,
	val,
	value,
	value_int1,
	value_int_0_0,
	value_int_0_1,
	value_int_0_2,
	value_int_0_3,
	value_int_0_4,
	verb,
	versus,
	war,
	wargoal,
	we,
	what,
	where,
	which,
	who,
	winner,
	x,
	y,
	year,
	years,
	chief_of_navy,
	chief_of_staff,
	control,
	head_of_government,
	owner,
	playername,
	yesterday,
	avg,
	n,
	strength,
	total,
	infantry,
	cavalry,
	special
};

struct line_break { };

struct text_sequence {
	uint32_t starting_component = 0;
	uint16_t component_count = 0;
};

using text_component = std::variant<line_break, text_color, variable_type, dcon::text_key>;

struct vector_backed_hash {
	using is_avalanching = void;
	using is_transparent = void;

	std::vector<char>& text_data;

	vector_backed_hash(std::vector<char>& text_data) : text_data(text_data) { }

	auto operator()(std::string_view sv) const noexcept -> uint64_t {
		return ankerl::unordered_dense::detail::wyhash::hash(sv.data(), sv.size());
	}
	auto operator()(dcon::text_key tag) const noexcept -> uint64_t {
		auto sv = [&]() {
			if(!tag)
				return std::string_view();
			auto start_position = text_data.data() + tag.index();
			auto data_size = text_data.size();
			auto end_position = start_position;
			for(; end_position < text_data.data() + data_size; ++end_position) {
				if(*end_position == 0)
					break;
			}
			return std::string_view(text_data.data() + tag.index(), size_t(end_position - start_position));
		}();
		return ankerl::unordered_dense::detail::wyhash::hash(sv.data(), sv.size());
	}
};
struct vector_backed_eq {
	using is_transparent = void;

	std::vector<char>& text_data;

	vector_backed_eq(std::vector<char>& text_data) : text_data(text_data) { }

	bool operator()(dcon::text_key l, dcon::text_key r) const noexcept {
		return l == r;
	}
	bool operator()(dcon::text_key l, std::string_view r) const noexcept {
		auto sv = [&]() {
			if(!l)
				return std::string_view();
			auto start_position = text_data.data() + l.index();
			auto data_size = text_data.size();
			auto end_position = start_position;
			for(; end_position < text_data.data() + data_size; ++end_position) {
				if(*end_position == 0)
					break;
			}
			return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
		}();
		return sv == r;
	}
	bool operator()(std::string_view r, dcon::text_key l) const noexcept {
		auto sv = [&]() {
			if(!l)
				return std::string_view();
			auto start_position = text_data.data() + l.index();
			auto data_size = text_data.size();
			auto end_position = start_position;
			for(; end_position < text_data.data() + data_size; ++end_position) {
				if(*end_position == 0)
					break;
			}
			return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
		}();
		return sv == r;
	}
	bool operator()(dcon::text_key l, std::string const& r) const noexcept {
		auto sv = [&]() {
			if(!l)
				return std::string_view();
			auto start_position = text_data.data() + l.index();
			auto data_size = text_data.size();
			auto end_position = start_position;
			for(; end_position < text_data.data() + data_size; ++end_position) {
				if(*end_position == 0)
					break;
			}
			return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
		}();
		return sv == r;
	}
	bool operator()(std::string const& r, dcon::text_key l) const noexcept {
		auto sv = [&]() {
			if(!l)
				return std::string_view();
			auto start_position = text_data.data() + l.index();
			auto data_size = text_data.size();
			auto end_position = start_position;
			for(; end_position < text_data.data() + data_size; ++end_position) {
				if(*end_position == 0)
					break;
			}
			return std::string_view(text_data.data() + l.index(), size_t(end_position - start_position));
		}();
		return sv == r;
	}
};

struct fp_one_place {
	float value = 0.0f;
};
struct fp_two_places {
	float value = 0.0f;
};
struct fp_three_places {
	float value = 0.0f;
};
struct fp_four_places {
	double value = 0.0f;
};
struct fp_currency {
	float value = 0.0f;
};
struct pretty_integer {
	int64_t value = 0;
};
struct fp_percentage {
	float value = 0.0f;
};
struct fp_percentage_one_place {
	float value = 0.0f;
};
struct dp_percentage {
	double value = 0.0f;
};
struct int_percentage {
	int32_t value = 0;
};
struct int_wholenum {
	int32_t value = 0;
};
using substitution = std::variant<std::string_view, dcon::text_key, dcon::province_id, dcon::state_instance_id, dcon::nation_id,
		dcon::national_identity_id, int64_t, fp_one_place, sys::date, std::monostate, fp_two_places, fp_three_places, fp_four_places,
		fp_currency, pretty_integer, dp_percentage, fp_percentage, fp_percentage_one_place, int_percentage, int_wholenum, dcon::text_sequence_id,
		dcon::state_definition_id>;
using substitution_map = ankerl::unordered_dense::map<uint32_t, substitution>;

struct text_chunk {
	std::string win1250chars;
	float x = 0; // yes, there is a reason the x offset is a floating point value while the y offset is an integer
	substitution source = std::monostate{};
	int16_t y = 0;
	int16_t width = 0;
	int16_t height = 0;
	text_color color = text_color::black;
};
struct layout_parameters {
	int16_t left = 0;
	int16_t top = 0;
	int16_t right = 0;
	int16_t bottom = 0;
	uint16_t font_id = 0;
	int16_t leading = 0;
	alignment align = alignment::left;
	text_color color = text_color::white;
	bool suppress_hyperlinks = false;
};
struct layout {
	std::vector<text_chunk> contents;
	int32_t number_of_lines = 0;
	text_chunk const* get_chunk_from_position(int32_t x, int32_t y) const;
};

struct layout_box {
	size_t first_chunk = 0;
	size_t line_start = 0;
	int32_t x_offset = 0;
	int32_t x_size = 0;
	int32_t y_size = 0;

	float x_position = 0;
	int32_t y_position = 0;
	text_color color = text_color::white;
};

struct layout_base {
	layout& base_layout;
	layout_parameters fixed_parameters;

	layout_base(layout& base_layout, layout_parameters const& fixed_parameters)
			: base_layout(base_layout), fixed_parameters(fixed_parameters) { }

	virtual void internal_close_box(layout_box& box) = 0;
};

struct columnar_layout : public layout_base {
	int32_t used_height = 0;
	int32_t used_width = 0;
	int32_t y_cursor = 0;
	int32_t current_column_x = 0;
	int32_t column_width = 0;

	columnar_layout(layout& base_layout, layout_parameters const& fixed_parameters, int32_t used_height = 0, int32_t used_width = 0,
			int32_t y_cursor = 0, int32_t column_width = 0)
			: layout_base(base_layout, fixed_parameters), used_height(used_height), used_width(used_width), y_cursor(y_cursor),
				current_column_x(fixed_parameters.left), column_width(column_width) {
		layout_base::fixed_parameters.left = 0;
	}

	void internal_close_box(layout_box& box) final;
};

struct endless_layout : public layout_base {
	int32_t y_cursor = 0;

	endless_layout(layout& base_layout, layout_parameters const& fixed_parameters, int32_t y_cursor = 0)
			: layout_base(base_layout, fixed_parameters), y_cursor(y_cursor) { }

	void internal_close_box(layout_box& box) final;
};

text_color char_to_color(char in);

endless_layout create_endless_layout(layout& dest, layout_parameters const& params);
void close_layout_box(endless_layout& dest, layout_box& box);

columnar_layout create_columnar_layout(layout& dest, layout_parameters const& params, int32_t column_width);

layout_box open_layout_box(layout_base& dest, int32_t indent = 0);
void close_layout_box(columnar_layout& dest, layout_box& box);
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, dcon::text_sequence_id source_text,
		substitution_map const& mp = substitution_map{});
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view,
		text_color color = text_color::white, substitution source = std::monostate{});
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, substitution val,
		text_color color = text_color::white);
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, std::string const& val,
		text_color color = text_color::white);
void add_space_to_layout_box(sys::state& state, layout_base& dest, layout_box& box);
void add_line_break_to_layout_box(sys::state& state, layout_base& dest, layout_box& box);

void add_line_break_to_layout(sys::state& state, columnar_layout& dest);
void add_line_break_to_layout(sys::state& state, endless_layout& dest);

void close_layout_box(layout_base& dest, layout_box& box);

void add_to_substitution_map(substitution_map& mp, variable_type key, substitution value);
void add_to_substitution_map(substitution_map& mp, variable_type key, std::string const&); // DO NOT USE THIS FUNCTION

void consume_csv_file(sys::state& state, uint32_t language, char const* file_content, uint32_t file_size);
variable_type variable_type_from_name(std::string_view);
void load_text_data(sys::state& state, uint32_t language);
char16_t win1250toUTF16(char in);
std::string produce_simple_string(sys::state const& state, dcon::text_sequence_id id);
std::string produce_simple_string(sys::state const& state, std::string_view key);
dcon::text_sequence_id find_or_add_key(sys::state& state, std::string_view key);
std::string date_to_string(sys::state const& state, sys::date date);

std::string prettify(int64_t num);
std::string format_money(float num);
std::string format_wholenum(int32_t num);
std::string format_percentage(float num, size_t digits = 2);
std::string format_float(float num, size_t digits = 2);
std::string format_ratio(int32_t left, int32_t right);
template<class T>
std::string get_name_as_string(sys::state const& state, T t);
template<class T>
std::string get_adjective_as_string(sys::state const& state, T t);
std::string get_dynamic_state_name(sys::state const& state, dcon::state_instance_id state_id);
std::string get_province_state_name(sys::state const& state, dcon::province_id prov_id);
std::string get_focus_category_name(sys::state const& state, nations::focus_type category);
std::string get_influence_level_name(sys::state const& state, uint8_t v);

void localised_format_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view key,
		substitution_map const& sub = substitution_map{});
void localised_single_sub_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view key, variable_type subkey,
		substitution value);

void add_line(sys::state& state, layout_base& dest, std::string_view key, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, variable_type subkey_d,
		substitution value_d, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_sequence_id txt, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_sequence_id txt, variable_type subkey, substitution value,
		int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_sequence_id txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_sequence_id txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_sequence_id txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, variable_type subkey_d,
		substitution value_d, int32_t indent = 0);
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, int32_t indent = 0);
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, int32_t indent = 0);

void add_divider_to_layout_box(sys::state& state, layout_base& dest, layout_box& box);

std::string resolve_string_substitution(sys::state& state, std::string_view key, substitution_map const& mp);
std::string resolve_string_substitution(sys::state& state, dcon::text_sequence_id key, substitution_map const& mp);

#define TEXT_NOTIF_MSG_TITLE(str)                                                                                                \
	{                                                                                                                              \
		auto box = text::open_layout_box(layout);                                                                                    \
		text::localised_format_box(state, layout, box, std::string_view(#str "_log"), sub);                                          \
		text::close_layout_box(layout, box);                                                                                         \
	}

#define TEXT_NOTIF_MSG_BODY(str)                                                                                                 \
	{                                                                                                                              \
		auto box0 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box0, std::string_view(#str "_header"), sub);                                      \
		text::close_layout_box(layout, box0);                                                                                        \
		auto box1 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box1, std::string_view(#str "_1"), sub);                                           \
		text::close_layout_box(layout, box1);                                                                                        \
		auto box2 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box2, std::string_view(#str "_2"), sub);                                           \
		text::close_layout_box(layout, box2);                                                                                        \
		auto box3 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box3, std::string_view(#str "_3"), sub);                                           \
		text::close_layout_box(layout, box3);                                                                                        \
		auto box4 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box4, std::string_view(#str "_4"), sub);                                           \
		text::close_layout_box(layout, box4);                                                                                        \
		auto box5 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box5, std::string_view(#str "_5"), sub);                                           \
		text::close_layout_box(layout, box5);                                                                                        \
		auto box6 = text::open_layout_box(layout);                                                                                   \
		text::localised_format_box(state, layout, box6, std::string_view(#str "_6"), sub);                                           \
		text::close_layout_box(layout, box6);                                                                                        \
	}

} // namespace text
