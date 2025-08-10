#pragma once

#include <stdint.h>
#include <variant>
#include <vector>
#include <string>
#include <string_view>
#include "dcon_generated.hpp"
#include "nations.hpp"
#include "unordered_dense.h"
#include "fonts.hpp"

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
	gold,
	reset,
	brown
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
	religion,
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
	special,
	//vanilla but forgot
	thiscountry,
	//non-vanilla
	fromcontinent,
	fromcapital,
	crisistaker_continent,
	crisistaker_capital,
	crisisattacker_continent,
	crisisattacker_adj,
	crisisattacker_capital,
	crisisdefender_continent,
	crisisdefender_adj,
	crisisdefender_capital,
	culture_first_name,
	culture_last_name,
	fromstatename,
	spheremaster_adj,
	spheremaster_union_adj
};

struct line_break { };


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

namespace detail {
inline void mum(uint64_t* a, uint64_t* b) {
#    if defined(__SIZEOF_INT128__)
	__uint128_t r = *a;
	r *= *b;
	*a = static_cast<uint64_t>(r);
	*b = static_cast<uint64_t>(r >> 64U);
#    elif defined(_MSC_VER) && defined(_M_X64)
	*a = _umul128(*a, *b, b);
#    else
	uint64_t ha = *a >> 32U;
	uint64_t hb = *b >> 32U;
	uint64_t la = static_cast<uint32_t>(*a);
	uint64_t lb = static_cast<uint32_t>(*b);
	uint64_t hi{};
	uint64_t lo{};
	uint64_t rh = ha * hb;
	uint64_t rm0 = ha * lb;
	uint64_t rm1 = hb * la;
	uint64_t rl = la * lb;
	uint64_t t = rl + (rm0 << 32U);
	auto c = static_cast<uint64_t>(t < rl);
	lo = t + (rm1 << 32U);
	c += static_cast<uint64_t>(lo < t);
	hi = rh + (rm0 >> 32U) + (rm1 >> 32U) + c;
	*a = lo;
	*b = hi;
#    endif
}

// multiply and xor mix function, aka MUM
[[nodiscard]] inline auto mix(uint64_t a, uint64_t b) -> uint64_t {
	mum(&a, &b);
	return a ^ b;
}

// read functions. WARNING: we don't care about endianness, so results are different on big endian!
[[nodiscard]] inline auto r8(const uint8_t* p) -> uint64_t {
	uint64_t v{};
	std::memcpy(&v, p, 8U);
	return v | 0x2020202020202020;
}

[[nodiscard]] inline auto r4(const uint8_t* p) -> uint64_t {
	uint32_t v{};
	std::memcpy(&v, p, 4);
	return v | 0x20202020;
}

// reads 1, 2, or 3 bytes
[[nodiscard]] inline auto r3(const uint8_t* p, size_t k) -> uint64_t {
	return (static_cast<uint64_t>(p[0] | 0x20) << 16U) | (static_cast<uint64_t>(p[k >> 1U] | 0x20) << 8U) | p[k - 1] | 0x20;
}

[[nodiscard]] inline auto ci_wyhash(void const* key, size_t len) -> uint64_t {
	static constexpr auto secret = std::array{ UINT64_C(0xa0761d6478bd642f),
						  UINT64_C(0xe7037ed1a0b428db),
						  UINT64_C(0x8ebc6af09c88c6e3),
						  UINT64_C(0x589965cc75374cc3) };

	auto const* p = static_cast<uint8_t const*>(key);
	uint64_t seed = secret[0];
	uint64_t a{};
	uint64_t b{};
	if(ANKERL_UNORDERED_DENSE_LIKELY(len <= 16)) {
		if(ANKERL_UNORDERED_DENSE_LIKELY(len >= 4)) {
			a = (r4(p) << 32U) | r4(p + ((len >> 3U) << 2U));
			b = (r4(p + len - 4) << 32U) | r4(p + len - 4 - ((len >> 3U) << 2U));
		} else if(ANKERL_UNORDERED_DENSE_LIKELY(len > 0)) {
			a = r3(p, len);
			b = 0;
		} else {
			a = 0;
			b = 0;
		}
	} else {
		size_t i = len;
		if(ANKERL_UNORDERED_DENSE_UNLIKELY(i > 48)) {
			uint64_t see1 = seed;
			uint64_t see2 = seed;
			do {
				seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
				see1 = mix(r8(p + 16) ^ secret[2], r8(p + 24) ^ see1);
				see2 = mix(r8(p + 32) ^ secret[3], r8(p + 40) ^ see2);
				p += 48;
				i -= 48;
			} while(ANKERL_UNORDERED_DENSE_LIKELY(i > 48));
			seed ^= see1 ^ see2;
		}
		while(ANKERL_UNORDERED_DENSE_UNLIKELY(i > 16)) {
			seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
			i -= 16;
			p += 16;
		}
		a = r8(p + i - 16);
		b = r8(p + i - 8);
	}

	return mix(secret[1] ^ len, mix(a ^ secret[1], b ^ seed));
}

inline bool lazy_ci_eq(std::string_view a, std::string_view b) {
	if(a.length() != b.length())
		return false;
	for(uint32_t i = 0; i < a.length(); ++i) {
		if((a[i] | 0x20) != (b[i] | 0x20))
			return false;
	}
	return true;
}

}

struct vector_backed_ci_hash {
	using is_avalanching = void;
	using is_transparent = void;

	std::vector<char>& text_data;

	vector_backed_ci_hash(std::vector<char>& text_data) : text_data(text_data) { }

	auto operator()(std::string_view sv) const noexcept -> uint64_t {
		return detail::ci_wyhash(sv.data(), sv.size());
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
		return detail::ci_wyhash(sv.data(), sv.size());
	}
};
struct vector_backed_ci_eq {
	using is_transparent = void;

	std::vector<char>& text_data;

	vector_backed_ci_eq(std::vector<char>& text_data) : text_data(text_data) { }

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
			return detail::lazy_ci_eq(sv, r);
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
			return detail::lazy_ci_eq(sv, r);
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
			return detail::lazy_ci_eq(sv, r);
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
			return detail::lazy_ci_eq(sv, r);
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
	float value = 0.0f;
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
struct fp_percentage_two_places {
	float value = 0.0f;
};
struct int_percentage {
	int32_t value = 0;
};
struct int_wholenum {
	int32_t value = 0;
};
enum class embedded_icon : uint8_t {
	check, xmark, army, navy
};
struct embedded_unit_icon {
	dcon::unit_type_id unit_type;
};
struct embedded_commodity_icon {
	dcon::commodity_id commodity;
};
struct embedded_flag {
	dcon::national_identity_id tag;
};
using substitution = std::variant<std::string_view, dcon::text_key, dcon::province_id, dcon::state_instance_id, dcon::nation_id,
		dcon::national_identity_id, int64_t, fp_one_place, sys::date, std::monostate, fp_two_places, fp_three_places, fp_four_places,
		fp_currency, pretty_integer, fp_percentage, fp_percentage_one_place, fp_percentage_two_places, int_percentage, int_wholenum,
		dcon::state_definition_id, embedded_icon, embedded_flag, embedded_unit_icon, embedded_commodity_icon>;
using substitution_map = ankerl::unordered_dense::map<uint32_t, substitution>;

struct text_chunk {
	text::stored_glyphs unicodechars;
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
	bool single_line = false;
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
	enum class rtl_status : uint8_t { ltr, rtl };
	layout& base_layout;
	layout_parameters fixed_parameters;
	rtl_status native_rtl = rtl_status::ltr;

	layout_base(layout& base_layout, layout_parameters const& fixed_parameters, rtl_status native_rtl)
			: base_layout(base_layout), fixed_parameters(fixed_parameters), native_rtl(native_rtl) {
		if(native_rtl == rtl_status::rtl) {
			if(fixed_parameters.align == text::alignment::left) {
				layout_base::fixed_parameters.align = text::alignment::right;
			} else if(fixed_parameters.align == text::alignment::left) {
				layout_base::fixed_parameters.align = text::alignment::left;
			}
		}
	}

	virtual void internal_close_box(layout_box& box) = 0;
};

struct columnar_layout : public layout_base {
	int32_t used_height = 0;
	int32_t used_width = 0;
	int32_t y_cursor = 0;
	int32_t current_column_x = 0;
	int32_t column_width = 0;

	columnar_layout(layout& base_layout, layout_parameters const& fixed_parameters, layout_base::rtl_status native_rtl, int32_t used_height = 0, int32_t used_width = 0, int32_t y_cursor = 0, int32_t column_width = 0)
			: layout_base(base_layout, fixed_parameters, native_rtl), used_height(used_height), used_width(used_width), y_cursor(y_cursor),
				current_column_x(fixed_parameters.left), column_width(column_width) {
		layout_base::fixed_parameters.left = 0;
	}

	void internal_close_box(layout_box& box) final;
};

struct endless_layout : public layout_base {
	int32_t y_cursor = 0;

	endless_layout(layout& base_layout, layout_parameters const& fixed_parameters, layout_base::rtl_status native_rtl, int32_t y_cursor = 0)
			: layout_base(base_layout, fixed_parameters, native_rtl), y_cursor(y_cursor) { }

	void internal_close_box(layout_box& box) final;
};

layout_box open_layout_box(layout_base& dest, int32_t indent = 0);

struct single_line_layout : public layout_base {
	layout_box box;

	single_line_layout(layout& base_layout, layout_parameters const& fixed_parameters, layout_base::rtl_status native_rtl)
		: layout_base(base_layout, fixed_parameters, native_rtl), box(open_layout_box(*this, 0)) {

		base_layout.number_of_lines = 0;
		base_layout.contents.clear();
	}

	void internal_close_box(layout_box& box) final;

	~single_line_layout() {
		internal_close_box(box);
	}

	void add_text(sys::state& state, std::string_view v);
	void add_text(sys::state& state, dcon::text_key source_text);
};

text_color char_to_color(char in);

endless_layout create_endless_layout(sys::state& state, layout& dest, layout_parameters const& params);
void close_layout_box(endless_layout& dest, layout_box& box);

columnar_layout create_columnar_layout(sys::state& state, layout& dest, layout_parameters const& params, int32_t column_width);

void close_layout_box(columnar_layout& dest, layout_box& box);
void close_layout_box(single_line_layout& dest, layout_box& box);
void add_unparsed_text_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view sv, substitution_map const& mp = substitution_map{});
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, dcon::text_key source_text,
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
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, embedded_flag ico);
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, embedded_icon ico);

void close_layout_box(layout_base& dest, layout_box& box);

void add_to_substitution_map(substitution_map& mp, variable_type key, substitution value);
void add_to_substitution_map(substitution_map& mp, variable_type key, std::string const&); // DO NOT USE THIS FUNCTION

void consume_csv_file(sys::state& state, char const* file_content, uint32_t file_size, int32_t target_column, bool as_unicode);
variable_type variable_type_from_name(std::string_view);
char16_t win1250toUTF16(char in);
std::string produce_simple_string(sys::state const& state, std::string_view key);
std::string produce_simple_string(sys::state const& state, dcon::text_key id);
text::alignment localized_alignment(sys::state& state, text::alignment in);
ui::alignment localized_alignment(sys::state& state, ui::alignment in);
text::alignment to_text_alignment(ui::alignment in);

dcon::text_key find_or_add_key(sys::state& state, std::string_view key, bool as_unicode);

std::string date_to_string(sys::state& state, sys::date date);

std::string prettify(int64_t num);
std::string prettify_currency(float num);
std::string format_money(float num);
std::string format_wholenum(int32_t num);
std::string format_percentage(float num, size_t digits = 2);
std::string format_float(float num, size_t digits = 2);
std::string format_ratio(int32_t left, int32_t right);
template<class T>
std::string get_name_as_string(sys::state& state, T t) {
	return text::produce_simple_string(state, t.get_name());
}
template<class T>
std::string get_adjective_as_string(sys::state& state, T t) {
	return text::produce_simple_string(state, t.get_adjective());
}
std::string get_short_state_name(sys::state& state, dcon::state_instance_id state_id);
std::string get_dynamic_state_name(sys::state& state, dcon::state_instance_id state_id);
std::string get_province_state_name(sys::state& state, dcon::province_id prov_id);
std::string get_focus_category_name(sys::state const& state, nations::focus_type category);
std::string get_influence_level_name(sys::state const& state, uint8_t v);
dcon::text_key get_name(sys::state& state, dcon::nation_id n);
dcon::text_key get_adjective(sys::state& state, dcon::nation_id n);
dcon::text_key get_ruler_title(sys::state& state, dcon::nation_id n);
inline std::string get_name_as_string(sys::state& state, dcon::nation_id n) {
	return text::produce_simple_string(state, get_name(state, n));
}
inline std::string get_adjective_as_string(sys::state& state, dcon::nation_id n) {
	return text::produce_simple_string(state, get_adjective(state, n));
}
inline std::string get_name_as_string(sys::state& state, dcon::nation_fat_id n) {
	return text::produce_simple_string(state, get_name(state, n));
}
inline std::string get_adjective_as_string(sys::state& state, dcon::nation_fat_id n) {
	return text::produce_simple_string(state, get_adjective(state, n));
}

void localised_format_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view key,
		substitution_map const& sub = substitution_map{});
void localised_single_sub_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view key, variable_type subkey,
		substitution value);

void nation_name_and_flag(sys::state& state, dcon::nation_id n, layout_base& dest, int32_t indent = 0);

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
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, int32_t indent = 0);
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, variable_type subkey_d,
		substitution value_d, int32_t indent = 0);
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, int32_t indent = 0);
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, int32_t indent = 0);
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, variable_type subkeyb, substitution valueb, int32_t indent = 0);
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, variable_type subkeyb, substitution valueb, variable_type subkeyc, substitution valuec, int32_t indent = 0);
size_t size_from_utf8(char const* start, char const* end);

void add_divider_to_layout_box(sys::state& state, layout_base& dest, layout_box& box);

std::string resolve_string_substitution(sys::state& state, std::string_view key, substitution_map const& mp);
std::string resolve_string_substitution(sys::state& state, dcon::text_key key, substitution_map const& mp);

uint32_t codepoint_from_utf8(char const* start, char const* end);
size_t size_from_utf8(char const* start, char const*);

} // namespace text
