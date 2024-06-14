#include <string_view>

#include "nations.hpp"
#include "text.hpp"
#include "system_state.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"
#include <type_traits>

namespace text {
text_color char_to_color(char in) {
	switch(in) {
	case 'W':
		return text_color::white;
	case 'G':
		return text_color::green;
	case 'R':
		return text_color::red;
	case 'Y':
		return text_color::yellow;
	case 'b':
		return text_color::black;
	case 'B':
		return text_color::light_blue;
	case 'g':
		return text_color::dark_blue;
	case 'L':
		return text_color::lilac;
	case 'O':
		return text_color::orange;
	case 'l':
		return text_color::light_grey;
	case 'C':
		return text_color::dark_red;
	case 'J':
		return text_color::dark_green;
	case 'I':
		return text_color::brown;
	case '!':
		return text_color::reset;
	default:
		return text_color::unspecified;
	}
}

inline bool is_qmark_color(char in) {
	return char_to_color(in) != text_color::unspecified;
}

std::string lowercase_str(std::string_view sv) {
	std::string result;
	result.reserve(sv.length());
	for(auto ch : sv) {
		result += char(tolower(ch));
	}
	return result;
}

uint32_t codepoint_from_utf8(char const* start, char const* end) {
	uint8_t byte1 = uint8_t(start + 0 < end ? start[0] : 0);
	uint8_t byte2 = uint8_t(start + 1 < end ? start[1] : 0);
	uint8_t byte3 = uint8_t(start + 2 < end ? start[2] : 0);
	uint8_t byte4 = uint8_t(start + 3 < end ? start[3] : 0);
	if((byte1 & 0x80) == 0) {
		return uint32_t(byte1);
	} else if((byte1 & 0xE0) == 0xC0) {
		return uint32_t(byte2 & 0x3F) | (uint32_t(byte1 & 0x1F) << 6);
	} else  if((byte1 & 0xF0) == 0xE0) {
		return uint32_t(byte3 & 0x3F) | (uint32_t(byte2 & 0x3F) << 6) | (uint32_t(byte1 & 0x0F) << 12);
	} else if((byte1 & 0xF8) == 0xF0) {
		return uint32_t(byte4 & 0x3F) | (uint32_t(byte3 & 0x3F) << 6) | (uint32_t(byte2 & 0x3F) << 12) | (uint32_t(byte1 & 0x07) << 18);
	}
	return 0;
}
size_t size_from_utf8(char const* start, char const* end) {
	uint8_t b = uint8_t(start + 0 < end ? start[0] : 0);
	return ((b & 0x80) == 0) ? 1 : ((b & 0xE0) == 0xC0) ? 2
		: ((b & 0xF0) == 0xE0) ? 3 : ((b & 0xF8) == 0xF0) ? 4
		: 1;
}
bool codepoint_is_space(uint32_t c) noexcept {
	return (c == 0x3000 || c == 0x205F || c == 0x202F || c == 0x2029 || c == 0x2028 || c == 0x00A0
		|| c == 0x0085 || c <= 0x0020 || (0x2000 <= c && c <= 0x200A));
}
bool codepoint_is_line_break(uint32_t c) noexcept {
	return  c == 0x2029 || c == 0x2028 || c == uint32_t('\n') || c == uint32_t('\r');
}

void consume_csv_file(sys::state& state, char const* file_content, uint32_t file_size, int32_t target_column, bool as_unicode) {
	auto cpos = file_content;
	while(cpos < file_content + file_size) {
		if(as_unicode) {
			cpos = parsers::parse_fixed_amount_csv_values<14>(cpos, file_content + file_size, ';', [&](std::string_view const* values) {
				auto key = state.add_key_utf8(values[0]);
				auto entry = state.add_locale_data_utf8(values[target_column]);
				state.locale_key_to_text_sequence.insert_or_assign(key, entry);
			});
		} else {
			cpos = parsers::parse_fixed_amount_csv_values<14>(cpos, file_content + file_size, ';', [&](std::string_view const* values) {
				auto key = state.add_key_win1252(values[0]);
				auto entry = state.add_locale_data_win1252(values[target_column]);
				state.locale_key_to_text_sequence.insert_or_assign(key, entry);
			});
		}
	}
}

template<size_t N>
bool is_fixed_token_ci(std::string_view v, char const (&t)[N]) {
	if(v.length() != (N - 1))
		return false;
	for(unsigned int i = 0; i < N - 1; ++i) {
		if(tolower(v[i]) != t[i])
			return false;
	}
	return true;
}

#define CT_STRING_ENUM(X) else if(is_fixed_token_ci(v, #X)) return variable_type::X;

variable_type variable_type_from_name(std::string_view v) {
	if(v.length() == 1) {
		if(false) { }
		CT_STRING_ENUM(d)
		CT_STRING_ENUM(m)
		CT_STRING_ENUM(n)
		CT_STRING_ENUM(x)
		CT_STRING_ENUM(y)
	} else if(v.length() == 2) {
		if(false) { }
		CT_STRING_ENUM(gp)
		CT_STRING_ENUM(nf)
		CT_STRING_ENUM(to)
		CT_STRING_ENUM(we)
	} else if(v.length() == 3) {
		if(false) { }
		CT_STRING_ENUM(adj)
		CT_STRING_ENUM(avg)
		CT_STRING_ENUM(bac)
		CT_STRING_ENUM(bat)
		CT_STRING_ENUM(bld)
		CT_STRING_ENUM(bon)
		CT_STRING_ENUM(bud)
		CT_STRING_ENUM(buy)
		CT_STRING_ENUM(cap)
		CT_STRING_ENUM(cat)
		CT_STRING_ENUM(con)
		CT_STRING_ENUM(cul)
		CT_STRING_ENUM(day)
		CT_STRING_ENUM(exc)
		CT_STRING_ENUM(exp)
		else if(is_fixed_token_ci(v, "for")) return variable_type::vtype_for;
		CT_STRING_ENUM(gov)
		CT_STRING_ENUM(hit)
		CT_STRING_ENUM(ide)
		CT_STRING_ENUM(inf)
		CT_STRING_ENUM(inv)
		CT_STRING_ENUM(law)
		CT_STRING_ENUM(lev)
		CT_STRING_ENUM(lim)
		CT_STRING_ENUM(loc)
		CT_STRING_ENUM(lvl)
		CT_STRING_ENUM(max)
		CT_STRING_ENUM(men)
		CT_STRING_ENUM(mil)
		CT_STRING_ENUM(min)
		else if(is_fixed_token_ci(v, "new")) return variable_type::vtype_new;
		CT_STRING_ENUM(now)
		CT_STRING_ENUM(num)
		CT_STRING_ENUM(old)
		CT_STRING_ENUM(opt)
		CT_STRING_ENUM(org)
		CT_STRING_ENUM(pay)
		CT_STRING_ENUM(pen)
		CT_STRING_ENUM(per)
		CT_STRING_ENUM(pop)
		CT_STRING_ENUM(rec)
		CT_STRING_ENUM(rel)
		CT_STRING_ENUM(req)
		CT_STRING_ENUM(scr)
		CT_STRING_ENUM(sea)
		CT_STRING_ENUM(str)
		CT_STRING_ENUM(sub)
		CT_STRING_ENUM(tag)
		CT_STRING_ENUM(tot)
		CT_STRING_ENUM(tra)
		CT_STRING_ENUM(val)
		CT_STRING_ENUM(war)
		CT_STRING_ENUM(who)
	} else if(v.length() == 4) {
		if(false) { }
		CT_STRING_ENUM(army)
		CT_STRING_ENUM(base)
		CT_STRING_ENUM(brig)
		CT_STRING_ENUM(cash)
		CT_STRING_ENUM(chan)
		CT_STRING_ENUM(cost)
		CT_STRING_ENUM(curr)
		CT_STRING_ENUM(date)
		CT_STRING_ENUM(days)
		CT_STRING_ENUM(desc)
		CT_STRING_ENUM(dest)
		CT_STRING_ENUM(dist)
		CT_STRING_ENUM(from)
		CT_STRING_ENUM(goal)
		CT_STRING_ENUM(good)
		CT_STRING_ENUM(govt)
		CT_STRING_ENUM(home)
		CT_STRING_ENUM(hull)
		CT_STRING_ENUM(list)
		CT_STRING_ENUM(lord)
		CT_STRING_ENUM(lose)
		CT_STRING_ENUM(much)
		CT_STRING_ENUM(name)
		CT_STRING_ENUM(navy)
		CT_STRING_ENUM(need)
		CT_STRING_ENUM(neut)
		CT_STRING_ENUM(odds)
		CT_STRING_ENUM(perc)
		CT_STRING_ENUM(pres)
		CT_STRING_ENUM(prog)
		CT_STRING_ENUM(proj)
		CT_STRING_ENUM(prov)
		CT_STRING_ENUM(rank)
		CT_STRING_ENUM(rate)
		CT_STRING_ENUM(rule)
		CT_STRING_ENUM(runs)
		CT_STRING_ENUM(self)
		CT_STRING_ENUM(sell)
		CT_STRING_ENUM(size)
		CT_STRING_ENUM(tech)
		CT_STRING_ENUM(text)
		CT_STRING_ENUM(them)
		CT_STRING_ENUM(time)
		CT_STRING_ENUM(type)
		CT_STRING_ENUM(unit)
		CT_STRING_ENUM(verb)
		CT_STRING_ENUM(what)
		CT_STRING_ENUM(year)
	} else if(v.length() == 5) {
		if(false) { }
		CT_STRING_ENUM(actor)
		CT_STRING_ENUM(bonus)
		CT_STRING_ENUM(casus)
		CT_STRING_ENUM(count)
		CT_STRING_ENUM(enemy)
		CT_STRING_ENUM(event)
		CT_STRING_ENUM(first)
		CT_STRING_ENUM(focus)
		CT_STRING_ENUM(funds)
		CT_STRING_ENUM(goods)
		CT_STRING_ENUM(group)
		CT_STRING_ENUM(iname)
		CT_STRING_ENUM(indep)
		CT_STRING_ENUM(input)
		CT_STRING_ENUM(issue)
		CT_STRING_ENUM(level)
		CT_STRING_ENUM(limit)
		CT_STRING_ENUM(local)
		CT_STRING_ENUM(money)
		CT_STRING_ENUM(month)
		CT_STRING_ENUM(order)
		CT_STRING_ENUM(other)
		CT_STRING_ENUM(owner)
		CT_STRING_ENUM(param)
		CT_STRING_ENUM(party)
		CT_STRING_ENUM(perc2)
		CT_STRING_ENUM(power)
		CT_STRING_ENUM(provs)
		CT_STRING_ENUM(ships)
		CT_STRING_ENUM(skill)
		CT_STRING_ENUM(speed)
		CT_STRING_ENUM(state)
		CT_STRING_ENUM(table)
		CT_STRING_ENUM(terms)
		CT_STRING_ENUM(third)
		CT_STRING_ENUM(title)
		CT_STRING_ENUM(total)
		CT_STRING_ENUM(truth)
		else if(is_fixed_token_ci(v, "union")) return variable_type::vtype_union;
		CT_STRING_ENUM(units)
		CT_STRING_ENUM(until)
		CT_STRING_ENUM(usnum)
		CT_STRING_ENUM(value)
		CT_STRING_ENUM(where)
		CT_STRING_ENUM(which)
		CT_STRING_ENUM(years)
	} else if(v.length() == 6) {
		if(false) { }
		CT_STRING_ENUM(action)
		CT_STRING_ENUM(active)
		CT_STRING_ENUM(amount)
		CT_STRING_ENUM(astate)
		CT_STRING_ENUM(badboy)
		CT_STRING_ENUM(chance)
		CT_STRING_ENUM(change)
		CT_STRING_ENUM(effect)
		CT_STRING_ENUM(folder)
		else if(is_fixed_token_ci(v, "friend")) return variable_type::vtype_friend;
		CT_STRING_ENUM(gp_adj)
		CT_STRING_ENUM(impact)
		CT_STRING_ENUM(income)
		CT_STRING_ENUM(infamy)
		CT_STRING_ENUM(leader)
		CT_STRING_ENUM(levels)
		CT_STRING_ENUM(months)
		CT_STRING_ENUM(nation)
		CT_STRING_ENUM(needed)
		CT_STRING_ENUM(number)
		CT_STRING_ENUM(option)
		CT_STRING_ENUM(output)
		CT_STRING_ENUM(player)
		CT_STRING_ENUM(policy)
		CT_STRING_ENUM(reform)
		CT_STRING_ENUM(region)
		CT_STRING_ENUM(result)
		CT_STRING_ENUM(rstate)
		CT_STRING_ENUM(second)
		CT_STRING_ENUM(source)
		CT_STRING_ENUM(strata)
		CT_STRING_ENUM(tag0_0)
		CT_STRING_ENUM(target)
		CT_STRING_ENUM(threat)
		CT_STRING_ENUM(usloss)
		CT_STRING_ENUM(versus)
		CT_STRING_ENUM(winner)
	} else if(v.length() == 7) {
		if(false) { }
		CT_STRING_ENUM(against)
		CT_STRING_ENUM(allowed)
		CT_STRING_ENUM(attunit)
		CT_STRING_ENUM(badword)
		CT_STRING_ENUM(capital)
		CT_STRING_ENUM(control)
		CT_STRING_ENUM(country)
		CT_STRING_ENUM(creator)
		CT_STRING_ENUM(credits)
		CT_STRING_ENUM(culture)
		CT_STRING_ENUM(current)
		CT_STRING_ENUM(defunit)
		CT_STRING_ENUM(details)
		CT_STRING_ENUM(effects)
		CT_STRING_ENUM(emilist)
		CT_STRING_ENUM(escorts)
		CT_STRING_ENUM(faction)
		CT_STRING_ENUM(factory)
		CT_STRING_ENUM(iamount)
		CT_STRING_ENUM(immlist)
		CT_STRING_ENUM(low_tax)
		CT_STRING_ENUM(maxloan)
		CT_STRING_ENUM(natives)
		CT_STRING_ENUM(opinion)
		CT_STRING_ENUM(optimal)
		CT_STRING_ENUM(our_num)
		CT_STRING_ENUM(our_res)
		CT_STRING_ENUM(passive)
		CT_STRING_ENUM(penalty)
		CT_STRING_ENUM(percent)
		CT_STRING_ENUM(poptype)
		CT_STRING_ENUM(setting)
		CT_STRING_ENUM(tag_0_0)
		CT_STRING_ENUM(tag_0_1)
		CT_STRING_ENUM(tag_0_2)
		CT_STRING_ENUM(tag_0_3)
		CT_STRING_ENUM(tag_1_0)
		CT_STRING_ENUM(tag_2_0)
		CT_STRING_ENUM(tag_3_0)
		CT_STRING_ENUM(terrain)
		CT_STRING_ENUM(wargoal)
		CT_STRING_ENUM(cavalry)
		CT_STRING_ENUM(special)
	} else if(v.length() == 8) {
		if(false) { }
		CT_STRING_ENUM(agressor)
		CT_STRING_ENUM(attacker)
		CT_STRING_ENUM(building)
		CT_STRING_ENUM(category)
		CT_STRING_ENUM(country1)
		CT_STRING_ENUM(country2)
		CT_STRING_ENUM(defender)
		CT_STRING_ENUM(fraction)
		CT_STRING_ENUM(high_tax)
		CT_STRING_ENUM(icountry)
		CT_STRING_ENUM(ideology)
		CT_STRING_ENUM(invested)
		CT_STRING_ENUM(literacy)
		CT_STRING_ENUM(location)
		CT_STRING_ENUM(movement)
		CT_STRING_ENUM(negative)
		else if(is_fixed_token_ci(v, "operator")) return variable_type::vtype_operator;
		CT_STRING_ENUM(opressor)
		CT_STRING_ENUM(our_lead)
		CT_STRING_ENUM(overlord)
		CT_STRING_ENUM(position)
		CT_STRING_ENUM(positive)
		CT_STRING_ENUM(prestige)
		CT_STRING_ENUM(produced)
		CT_STRING_ENUM(producer)
		CT_STRING_ENUM(progress)
		CT_STRING_ENUM(province)
		CT_STRING_ENUM(relation)
		CT_STRING_ENUM(religion)
		CT_STRING_ENUM(reqlevel)
		CT_STRING_ENUM(required)
		CT_STRING_ENUM(resource)
		CT_STRING_ENUM(strength)
		CT_STRING_ENUM(tag_0_0_)
		CT_STRING_ENUM(theirnum)
		CT_STRING_ENUM(totalemi)
		CT_STRING_ENUM(totalimm)
		CT_STRING_ENUM(infantry)
	} else if(v.length() == 9) {
		if(false) { }
		CT_STRING_ENUM(army_name)
		CT_STRING_ENUM(commander)
		CT_STRING_ENUM(countries)
		CT_STRING_ENUM(direction)
		CT_STRING_ENUM(employees)
		CT_STRING_ENUM(eventdesc)
		CT_STRING_ENUM(fromruler)
		CT_STRING_ENUM(ilocation)
		CT_STRING_ENUM(invention)
		CT_STRING_ENUM(messenger)
		CT_STRING_ENUM(militancy)
		CT_STRING_ENUM(navy_name)
		CT_STRING_ENUM(prescence)
		CT_STRING_ENUM(provinces)
		CT_STRING_ENUM(recipient)
		CT_STRING_ENUM(statename)
		CT_STRING_ENUM(their_num)
		CT_STRING_ENUM(their_res)
		CT_STRING_ENUM(theirlost)
		CT_STRING_ENUM(theirship)
		CT_STRING_ENUM(union_adj)
		CT_STRING_ENUM(yesterday)
	} else if(v.length() == 10) {
		if(false) { }
		CT_STRING_ENUM(countryadj)
		CT_STRING_ENUM(crisisarea)
		CT_STRING_ENUM(government)
		CT_STRING_ENUM(newcountry)
		CT_STRING_ENUM(oldcountry)
		CT_STRING_ENUM(ourcapital)
		CT_STRING_ENUM(ourcountry)
		CT_STRING_ENUM(percentage)
		CT_STRING_ENUM(playername)
		CT_STRING_ENUM(popularity)
		CT_STRING_ENUM(reconunits)
		CT_STRING_ENUM(string_0_0)
		CT_STRING_ENUM(string_0_1)
		CT_STRING_ENUM(string_0_2)
		CT_STRING_ENUM(string_0_3)
		CT_STRING_ENUM(string_0_4)
		CT_STRING_ENUM(string_9_0)
		CT_STRING_ENUM(targetlist)
		CT_STRING_ENUM(terrainmod)
		CT_STRING_ENUM(their_lead)
		CT_STRING_ENUM(unemployed)
		CT_STRING_ENUM(value_int1)
	} else if(v.length() == 11) {
		if(false) { }
		CT_STRING_ENUM(anyprovince)
		CT_STRING_ENUM(country_adj)
		CT_STRING_ENUM(countryname)
		CT_STRING_ENUM(crisistaker)
		CT_STRING_ENUM(date_long_0)
		CT_STRING_ENUM(date_long_1)
		CT_STRING_ENUM(fromcountry)
		CT_STRING_ENUM(otherresult)
		CT_STRING_ENUM(tag_0_0_adj)
		CT_STRING_ENUM(tag_0_1_adj)
		CT_STRING_ENUM(tag_0_2_adj)
		CT_STRING_ENUM(tag_0_3_adj)
		CT_STRING_ENUM(temperature)
		CT_STRING_ENUM(fromcapital)
		CT_STRING_ENUM(thiscountry)
	} else if(v.length() == 12) {
		if(false) { }
		CT_STRING_ENUM(construction)
		CT_STRING_ENUM(crisistarget)
		CT_STRING_ENUM(date_short_0)
		CT_STRING_ENUM(employee_max)
		CT_STRING_ENUM(fromprovince)
		CT_STRING_ENUM(monarchtitle)
		CT_STRING_ENUM(numfactories)
		CT_STRING_ENUM(organisation)
		CT_STRING_ENUM(provincename)
		CT_STRING_ENUM(spheremaster)
	} else if(v.length() == 13) {
		if(false) { }
		CT_STRING_ENUM(chief_of_navy)
		CT_STRING_ENUM(continentname)
		CT_STRING_ENUM(engineerunits)
		CT_STRING_ENUM(nationalvalue)
		CT_STRING_ENUM(opposing_army)
		CT_STRING_ENUM(opposing_navy)
		CT_STRING_ENUM(reconmaxunits)
		CT_STRING_ENUM(tag_0_0_upper)
		CT_STRING_ENUM(tag_0_1_upper)
		CT_STRING_ENUM(tag_2_0_upper)
		CT_STRING_ENUM(tag_3_0_upper)
		CT_STRING_ENUM(value_int_0_0)
		CT_STRING_ENUM(value_int_0_1)
		CT_STRING_ENUM(value_int_0_2)
		CT_STRING_ENUM(value_int_0_3)
		CT_STRING_ENUM(value_int_0_4)
		CT_STRING_ENUM(fromcontinent)
		CT_STRING_ENUM(fromstatename)
	} else if(v.length() == 14) {
		if(false) { }
		CT_STRING_ENUM(cb_target_name)
		CT_STRING_ENUM(chief_of_staff)
		CT_STRING_ENUM(countryculture)
		CT_STRING_ENUM(crisisattacker)
		CT_STRING_ENUM(crisisdefender)
		CT_STRING_ENUM(ourcountry_adj)
		CT_STRING_ENUM(second_country)
		CT_STRING_ENUM(strings_list_4)
		CT_STRING_ENUM(target_country)
	} else if(v.length() == 15) {
		if(false) { }
		CT_STRING_ENUM(base_percentage)
		CT_STRING_ENUM(crisistaker_adj)
		CT_STRING_ENUM(fromcountry_adj)
		CT_STRING_ENUM(provinceculture)
	} else if(v.length() == 16) {
		if(false) { }
		CT_STRING_ENUM(crisistarget_adj)
		CT_STRING_ENUM(engineermaxunits)
		CT_STRING_ENUM(provincereligion)
		CT_STRING_ENUM(spheremaster_adj)
	} else if(v.length() == 17) {
		if(false) { }
		CT_STRING_ENUM(culture_last_name)
	} else if(v.length() == 18) {
		if(false) { }
		CT_STRING_ENUM(cb_target_name_adj)
		CT_STRING_ENUM(head_of_government)
		CT_STRING_ENUM(crisisattacker_adj)
		CT_STRING_ENUM(crisisdefender_adj)
		CT_STRING_ENUM(culture_first_name)
	} else if(v.length() == 19) {
		if(false) { }
		CT_STRING_ENUM(culture_group_union)
		CT_STRING_ENUM(numspecialfactories)
		CT_STRING_ENUM(crisistaker_capital)
	} else if(v.length() == 20) {
		if(false) { }
	} else if(v.length() == 21) {
		if(false) { }
		CT_STRING_ENUM(crisistaker_continent)
	} else if(v.length() == 22) {
		if(false) { }
		CT_STRING_ENUM(crisisattacker_capital)
		CT_STRING_ENUM(crisisdefender_capital)
		CT_STRING_ENUM(spheremaster_union_adj)
	} else if(v.length() == 23) {
		if(false) { }
	} else if(v.length() == 24) {
		if(false) { }
		CT_STRING_ENUM(crisisattacker_continent)
		CT_STRING_ENUM(crisisdefender_continent)
	} else if(is_fixed_token_ci(v, "invested_in_us_message")) {
		return variable_type::invested_in_us_message;
	}

	return variable_type::error_no_matching_value;
}
#undef CT_STRING_ENUM

char16_t win1250toUTF16(char in) {
	constexpr static char16_t converted[256] =
			//		0		1		2		3		4		5		6		7		8		9		A		B		C		D		E		F
			/*0*/ {	u' ',		u'\u0001', u'\u0002', u'\u0003', u'\u0004', u' ', u' ', u' ', u' ', u'\t', u'\n', u' ', u' ', u' ', u' ', u' ',
			/*1*/	u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',		u' ',
			/*2*/	u' ',		u'!',		u'\"',		u'#',		u'$',		u'%',	u'&',		u'\'',		u'(',		u')',		u'*',		u'+',		u',',		u'-',		u'.',		u'/',
			/*3*/ u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u':', u';', u'<', u'=', u'>', u'?',
			/*4*/ u'@', u'A', u'B', u'C', u'D', u'E', u'F', u'G', u'H', u'I', u'J', u'K', u'L', u'M', u'N', u'O',
			/*5*/ u'P', u'Q', u'R', u'S', u'T', u'U', u'V', u'W', u'X', u'Y', u'Z', u'[', u'\\', u']', u'^', u'_',
			/*6*/ u'`', u'a', u'b', u'c', u'd', u'e', u'f', u'g', u'h', u'i', u'j', u'k', u'l', u'm', u'n', u'o',
			/*7*/ u'p', u'q', u'r', u's', u't', u'u', u'v', u'w', u'x', u'y', u'z', u'{', u'|', u'}', u'~', u' ',
			/*8*/ u'\u20AC', u' ', u'\u201A', u' ', u'\u201E', u'\u2026', u'\u2020', u'\u2021', u' ', u'\u2030', u'\u0160', u'\u2039', u'\u015A', u'\u0164', u'\u017D', u'\u0179',
			/*9*/ u' ', u'\u2018', u'\u2019', u'\u201C', u'\u201D', u'\u2022', u'\u2013', u'\u2014', u' ', u'\u2122', u'\u0161',
					u'\u203A', u'\u015B', u'\u0165', u'\u017E', u'\u017A',
			/*A*/ u'\u00A0', u'\u02C7', u'\u02D8', u'\u00A2', u'\u00A3', u'\u0104', u'\u00A6', u'\u00A7', u'\u00A8', u'\u00A9',
					u'\u015E', u'\u00AB', u'\u00AC', u'-', u'\u00AE', u'\u017B',
			/*B*/ u'\u00B0', u'\u00B1', u'\u02DB', u'\u0142', u'\u00B4', u'\u00B5', u'\u00B6', u'\u00B7', u'\u00B8', u'\u0105',
					u'\u015F', u'\u00BB', u'\u013D', u'\u02DD', u'\u013E', u'\u017C',
			/*C*/ u'\u0154', u'\u00C1', u'\u00C2', u'\u0102', u'\u00C4', u'\u0139', u'\u0106', u'\u00C7', u'\u010C', u'\u00C9',
					u'\u0118', u'\u00CB', u'\u011A', u'\u00CD', u'\u00CE', u'\u010E',
			/*D*/ u'\u0110', u'\u0143', u'\u0147', u'\u00D3', u'\u00D4', u'\u0150', u'\u00D6', u'\u00D7', u'\u0158', u'\u016E',
					u'\u00DA', u'\u0170', u'\u00DC', u'\u00DD', u'\u0162', u'\u00DF',
			/*E*/ u'\u0115', u'\u00E1', u'\u00E2', u'\u0103', u'\u00E4', u'\u013A', u'\u0107', u'\u00E7', u'\u010D', u'\u00E9',
					u'\u0119', u'\u00EB', u'\u011B', u'\u00ED', u'\u00EE', u'\u010F',
			/*F*/ u'\u0111', u'\u0144', u'\u0148', u'\u00F3', u'\u00F4', u'\u0151', u'\u00F6', u'\u00F7', u'\u0159', u'\u016F',
					u'\u00FA', u'\u0171', u'\u00FC', u'\u00FD', u'\u0163', u'\u02D9'};

	return converted[(uint8_t)in];
}

std::string produce_simple_string(sys::state const& state, dcon::text_key id) {
	std::string result;

	if(!id)
		return result;

	std::string_view sv;
	if(auto it = state.locale_key_to_text_sequence.find(id); it != state.locale_key_to_text_sequence.end()) {
		sv = state.locale_string_view(it->second);
	} else {
		sv = state.to_string_view(id);
	}

	char const* section_start = sv.data();
	for(char const* pos = sv.data(); pos < sv.data() + sv.length();) {
		bool colour_esc = false;
		if(pos + 1 < sv.data() + sv.length() && uint8_t(*pos) == 0xC2 && uint8_t(*(pos + 1)) == 0xA7) {
			if(section_start < pos)
				result += std::string_view(section_start, pos - section_start);

			pos += 2;
			section_start = pos;
			if(pos < sv.data() + sv.length()) {
				pos += 1;
				section_start = pos;
			}
		} else if(pos + 2 < sv.data() + sv.length() && uint8_t(*pos) == 0xEF && uint8_t(*(pos + 1)) == 0xBF && uint8_t(*(pos + 2)) == 0xBD && is_qmark_color(*(pos + 3))) {
			if(section_start < pos)
				result += std::string_view(section_start, pos - section_start);

			section_start = pos += 3;

			if(pos < sv.data() + sv.length()) {
				pos += 1;
				section_start = pos;
			}
		} else if(pos + 1 < sv.data() + sv.length() && *pos == '?' && is_qmark_color(*(pos + 1))) {
			if(section_start < pos)
				result += std::string_view(section_start, pos - section_start);

			pos += 1;
			section_start = pos;

			if(pos < sv.data() + sv.length()) {
				pos += 1;
				section_start = pos;
			}
		} else if(*pos == '$') {
			if(section_start < pos)
				result += std::string_view(section_start, pos - section_start);

			const char* vend = pos + 1;
			for(; vend != sv.data() + sv.length() && *vend != '$'; ++vend)
				;

			pos = vend + 1;
			section_start = pos;
		} else if(pos + 1 < sv.data() + sv.length() && *pos == '\\' && *(pos + 1) == 'n') {
			result += std::string_view(section_start, pos - section_start);
			section_start = pos += 2;
		} else {
			++pos;
		}
	}

	if(section_start < sv.data() + sv.length())
		result += std::string_view(section_start, (sv.data() + sv.length()) - section_start);

	return result;
}
std::string produce_simple_string(sys::state const& state, std::string_view txt) {
	auto v = state.lookup_key(txt);
	if(v)
		return produce_simple_string(state, v);
	else
		return std::string(txt);
}

dcon::text_key find_or_add_key(sys::state& state, std::string_view key, bool as_unicode) {
	if(as_unicode)
		return state.add_key_utf8(key);
	else
		return state.add_key_win1252(key);
}

std::string prettify_currency(float num) {
	constexpr static double mag[] = {
		1.0,
		1'000.0,
		1'000'000.0,
		1'000'000'000.0,
		1'000'000'000'000.0,
		1'000'000'000'000'000.0,
		1'000'000'000'000'000'000.0
	};
	constexpr static char const* sufx_two[] = {
		"%.2f £",
		"%.2fK £",
		"%.2fM £",
		"%.2fB £",
		"%.2fT £",
		"%.2fP £",
		"%.2fZ £"
	};
	constexpr static char const* sufx_one[] = {
		"%.1f £",
		"%.1fK £",
		"%.1fM £",
		"%.1fB £",
		"%.1fT £",
		"%.1fP £",
		"%.1fZ £"
	};
	constexpr static char const* sufx_zero[] = {
		"%.0f £",
		"%.0fK £",
		"%.0fM £",
		"%.0fB £",
		"%.0fT £",
		"%.0fP £",
		"%.0fZ £"
	};
	char buffer[200] = { 0 };
	double dval = double(num);
	if(std::abs(dval) <= 1.f) {
		snprintf(buffer, sizeof(buffer), sufx_two[0], float(dval));
		return std::string(buffer);
	}
	for(size_t i = std::extent_v<decltype(mag)>; i-- > 0;) {
		if(std::abs(dval) >= mag[i]) {
			auto reduced = dval / mag[i];
			if(std::abs(reduced) < 10.0) {
				snprintf(buffer, sizeof(buffer), sufx_two[i], float(reduced));
			} else if(std::abs(reduced) < 100.0) {
				snprintf(buffer, sizeof(buffer), sufx_one[i], float(reduced));
			} else {
				snprintf(buffer, sizeof(buffer), sufx_zero[i], float(reduced));
			}
			return std::string(buffer);
		}
	}
	return std::string("#inf");
}


std::string prettify(int64_t num) {
	if(num == 0)
		return std::string("0");

	constexpr static double mag[] = {
		1.0,
		1'000.0,
		1'000'000.0,
		1'000'000'000.0,
		1'000'000'000'000.0,
		1'000'000'000'000'000.0,
		1'000'000'000'000'000'000.0
	};
	constexpr static char const* sufx_two[] = {
		"%.0f",
		"%.2fK",
		"%.2fM",
		"%.2fB",
		"%.2fT",
		"%.2fP",
		"%.2fZ"
	};
	constexpr static char const* sufx_one[] = {
		"%.0f",
		"%.1fK",
		"%.1fM",
		"%.1fB",
		"%.1fT",
		"%.1fP",
		"%.1fZ"
	};
	constexpr static char const* sufx_zero[] = {
		"%.0f",
		"%.0fK",
		"%.0fM",
		"%.0fB",
		"%.0fT",
		"%.0fP",
		"%.0fZ"
	};

	char buffer[200] = { 0 };
	double dval = double(num);
	for(size_t i = std::extent_v<decltype(mag)>; i-- > 0;) {
		if(std::abs(dval) >= mag[i]) {
			auto reduced = dval / mag[i];
			if(std::abs(reduced) < 10.0) {
				snprintf(buffer, sizeof(buffer), sufx_two[i], float(reduced));
			} else if(std::abs(reduced) < 100.0) {
				snprintf(buffer, sizeof(buffer), sufx_one[i], float(reduced));
			} else {
				snprintf(buffer, sizeof(buffer), sufx_zero[i], float(reduced));
			}
			return std::string(buffer);
		}
	}
	return std::string("#inf");
}

std::string get_short_state_name(sys::state& state, dcon::state_instance_id state_id) {
	auto fat_id = dcon::fatten(state.world, state_id);
	for(auto st : fat_id.get_definition().get_abstract_state_membership()) {
		if(auto osm = st.get_province().get_state_membership().id; osm && fat_id.id != osm) {
			if(!state.key_is_localized(fat_id.get_definition().get_name())) {
				return get_name_as_string(state, fat_id.get_capital());
			} else {
				return get_name_as_string(state, fat_id.get_definition());
			}
		}
	}
	if(!state.key_is_localized(fat_id.get_definition().get_name()))
		return get_name_as_string(state, fat_id.get_capital());
	return get_name_as_string(state, fat_id.get_definition());
}

std::string get_dynamic_state_name(sys::state& state, dcon::state_instance_id state_id) {
	auto fat_id = dcon::fatten(state.world, state_id);
	for(auto st : fat_id.get_definition().get_abstract_state_membership()) {
		if(auto osm = st.get_province().get_state_membership().id; osm && fat_id.id != osm) {
			auto adj_id = text::get_adjective(state, fat_id.get_nation_from_state_ownership());
			auto adj = text::produce_simple_string(state, adj_id);
			if(!state.key_is_localized(fat_id.get_definition().get_name())) {
				if(!state.key_is_localized(adj_id)) {
					return get_name_as_string(state, fat_id.get_capital());
				}
				substitution_map sub{};
				add_to_substitution_map(sub, text::variable_type::state, fat_id.get_capital());
				add_to_substitution_map(sub, text::variable_type::adj, adj_id);
				return resolve_string_substitution(state, "compose_dynamic_adj_state", sub);
			} else if(!state.key_is_localized(adj_id)) {
				return get_name_as_string(state, fat_id.get_definition());
			}
			substitution_map sub{};
			add_to_substitution_map(sub, text::variable_type::state, fat_id.get_definition());
			add_to_substitution_map(sub, text::variable_type::adj, adj_id);
			return resolve_string_substitution(state, "compose_dynamic_adj_state", sub);
		}
	}
	if(!state.key_is_localized(fat_id.get_definition().get_name()))
		return get_name_as_string(state, fat_id.get_capital());
	return get_name_as_string(state, fat_id.get_definition());
}

std::string get_province_state_name(sys::state& state, dcon::province_id prov_id) {
	auto fat_id = dcon::fatten(state.world, prov_id);
	auto state_instance_id = fat_id.get_state_membership().id;
	if(state_instance_id) {
		return get_dynamic_state_name(state, state_instance_id);
	} else {
		auto sdef = fat_id.get_abstract_state_membership_as_province().get_state();
		if(!state.key_is_localized(sdef.get_name())) {
			auto lprovs = sdef.get_abstract_state_membership();
			if(lprovs.begin() != lprovs.end())
				return get_name_as_string(state, (*lprovs.begin()).get_province());
			return "NO_TEXT_KEY";
		}
		return get_name_as_string(state, sdef);
	}
}

dcon::text_key get_name(sys::state& state, dcon::nation_id id) {
	auto ident = state.world.nation_get_identity_from_identity_holder(id);
	auto gov_id = state.world.nation_get_government_type(id);
	if(gov_id) {
		auto gname =  state.world.national_identity_get_government_name(ident, gov_id);
		if(state.key_is_localized(gname))
			return gname;
	}
	return state.world.national_identity_get_name(ident);
}
dcon::text_key get_adjective(sys::state& state, dcon::nation_id id) {
	auto ident = state.world.nation_get_identity_from_identity_holder(id);
	//government specific adjective
	if(auto k = state.world.national_identity_get_government_adjective(ident, state.world.nation_get_government_type(id)); k) {
		return k;
	}
	return state.world.national_identity_get_adjective(ident);
}

std::string get_focus_category_name(sys::state const& state, nations::focus_type category) {
	switch(category) {
	case nations::focus_type::rail_focus:
		return text::produce_simple_string(state, "rail_focus");
	case nations::focus_type::immigration_focus:
		return text::produce_simple_string(state, "immigration_focus");
	case nations::focus_type::diplomatic_focus:
		return text::produce_simple_string(state, "diplomatic_focus");
	case nations::focus_type::promotion_focus:
		return text::produce_simple_string(state, "promotion_focus");
	case nations::focus_type::production_focus:
		return text::produce_simple_string(state, "production_focus");
	case nations::focus_type::party_loyalty_focus:
		return text::produce_simple_string(state, "party_loyalty_focus");
	case nations::focus_type::policy_focus:
		return text::produce_simple_string(state, "policy_focus");
	case nations::focus_type::tier_1_focus:
		return text::produce_simple_string(state, "tier_1_focus");
	case nations::focus_type::tier_2_focus:
		return text::produce_simple_string(state, "tier_2_focus");
	case nations::focus_type::tier_3_focus:
		return text::produce_simple_string(state, "tier_3_focus");
	case nations::focus_type::tier_4_focus:
		return text::produce_simple_string(state, "tier_4_focus");
	case nations::focus_type::tier_5_focus:
		return text::produce_simple_string(state, "tier_5_focus");
	case nations::focus_type::tier_6_focus:
		return text::produce_simple_string(state, "tier_6_focus");
	case nations::focus_type::tier_7_focus:
		return text::produce_simple_string(state, "tier_7_focus");
	case nations::focus_type::tier_8_focus:
		return text::produce_simple_string(state, "tier_8_focus");
	case nations::focus_type::building_focus:
		return text::produce_simple_string(state, "building_focus");
	case nations::focus_type::population_focus:
		return text::produce_simple_string(state, "population_focus");
	default:
		return text::produce_simple_string(state, "category");
	}
}

std::string get_influence_level_name(sys::state const& state, uint8_t v) {
	switch(v & nations::influence::level_mask) {
	case nations::influence::level_neutral:
		return text::produce_simple_string(state, "rel_neutral");
	case nations::influence::level_opposed:
		return text::produce_simple_string(state, "rel_opposed");
	case nations::influence::level_hostile:
		return text::produce_simple_string(state, "rel_hostile");
	case nations::influence::level_cordial:
		return text::produce_simple_string(state, "rel_cordial");
	case nations::influence::level_friendly:
		return text::produce_simple_string(state, "rel_friendly");
	case nations::influence::level_in_sphere:
		return text::produce_simple_string(state, "rel_sphere_of_influence");
	default:
		return text::produce_simple_string(state, "rel_neutral");
	}
}

std::string format_percentage(float num, size_t digits) {
	return format_float(num * 100.f, digits) + '%';
}

std::string format_float(float num, size_t digits) {
	char buffer[200] = {0};
	switch(digits) {
	default:
		// fallthrough
	case 3:
		if(num > 0.f && num < 0.001f) {
			return std::string(">0.000");
		} else if(num > -0.001f && num < 0.f) {
			return std::string("<0.000");
		}
		snprintf(buffer, sizeof(buffer), "%.3f", num);
		break;
	case 2:
		if(num > 0.f && num < 0.01f) {
			return std::string(">0.00");
		} else if(num > -0.01f && num < 0.f) {
			return std::string("<0.00");
		}
		snprintf(buffer, sizeof(buffer), "%.2f", num);
		break;
	case 1:
		if(num > 0.f && num < 0.1f) {
			return std::string(">0.0");
		} else if(num > -0.1f && num < 0.f) {
			return std::string("<0.0");
		}
		snprintf(buffer, sizeof(buffer), "%.1f", num);
		break;
	case 0:
		if(num > 0.f && num < 1.f) {
			return std::string(">0");
		} else if(num > -1.f && num < 0.f) {
			return std::string("<0");
		}
		return std::to_string(int64_t(num));
	}
	return std::string(buffer);
}

std::string format_money(float num) {
	return prettify_currency(num); // Currency is postfixed, NOT prefixed
}

std::string format_wholenum(int32_t num) {
	std::string result;

	if(num == 0) {
		result = "0";
		return result;
	}

	auto abs_value = num >= 0 ? num : -num;

	int32_t pcount = 0;
	while(abs_value > 0) {
		if(pcount == 3) {
			result.push_back(',');
		}
		auto last = abs_value % 10;
		abs_value /= 10;

		result.push_back(char('0' + last));

		++pcount;
	}

	if(num < 0)
		result.push_back('-');

	std::reverse(result.begin(), result.end());

	return result;
}

std::string format_ratio(int32_t left, int32_t right) {
	return std::to_string(left) + '/' + std::to_string(right);
}

void add_to_substitution_map(substitution_map& mp, variable_type key, substitution value) {
	mp.insert_or_assign(uint32_t(key), value);
}

dcon::text_key localize_month(sys::state const& state, uint16_t month) {
	static const std::string_view month_names[12] = {"january", "february", "march", "april", "may_month_name", "june", "july", "august",
		"september", "october", "november", "december"};
	
	if(month == 0 || month > 12) {
		return state.lookup_key("january");
	}
	return state.lookup_key(month_names[month - 1]);
}

std::string date_to_string(sys::state& state, sys::date date) {
	sys::year_month_day ymd = date.to_ymd(state.start_date);
	substitution_map sub{};
	add_to_substitution_map(sub, variable_type::year, int32_t(ymd.year));
	add_to_substitution_map(sub, variable_type::month, localize_month(state, ymd.month));
	add_to_substitution_map(sub, variable_type::day, int32_t(ymd.day));
	return resolve_string_substitution(state, "date_string_ymd", sub);
}

text_chunk const* layout::get_chunk_from_position(int32_t x, int32_t y) const {
	for(auto& chunk : contents) {
		if(int32_t(chunk.x) <= x && x <= int32_t(chunk.x) + chunk.width && chunk.y <= y && y <= chunk.y + chunk.height) {
			return &chunk;
		}
	}
	return nullptr;
}

endless_layout create_endless_layout(layout& dest, layout_parameters const& params) {
	dest.contents.clear();
	dest.number_of_lines = 0;
	return endless_layout(dest, params);
}

namespace impl {

void lb_finish_line(layout_base& dest, layout_box& box, int32_t line_height) {
	if(dest.fixed_parameters.align == alignment::center) {
		auto gap = (float(dest.fixed_parameters.right) - box.x_position) / 2.0f;
		for(size_t i = box.line_start; i < dest.base_layout.contents.size(); ++i) {
			dest.base_layout.contents.at(i).x += gap;
		}
	} else if(dest.fixed_parameters.align == alignment::right) {
		auto gap = float(dest.fixed_parameters.right) - box.x_position;
		for(size_t i = box.line_start; i < dest.base_layout.contents.size(); ++i) {
			dest.base_layout.contents.at(i).x += gap;
		}
	}

	box.x_position = float(box.x_offset + dest.fixed_parameters.left);
	box.y_position += line_height;
	dest.base_layout.number_of_lines += 1;
	box.line_start = dest.base_layout.contents.size();
}

} // namespace impl

void add_line_break_to_layout_box(sys::state& state, layout_base& dest, layout_box& box) {
	auto font_index = text::font_index_from_font_id(state, dest.fixed_parameters.font_id);
	auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);

	auto& font = state.font_collection.get_font(state, font_index );
	auto text_height = int32_t(std::ceil(font.line_height(font_size)));
	auto line_height = text_height + dest.fixed_parameters.leading;

	impl::lb_finish_line(dest, box, line_height);
}
void add_line_break_to_layout(sys::state& state, columnar_layout& dest) {
	auto font_index = text::font_index_from_font_id(state, dest.fixed_parameters.font_id);
	auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);
	
	auto& font = state.font_collection.get_font(state, font_index);
	auto text_height = int32_t(std::ceil(font.line_height(font_size)));
	auto line_height = text_height + dest.fixed_parameters.leading;
	dest.base_layout.number_of_lines += 1;
	dest.y_cursor += line_height;
}
void add_line_break_to_layout(sys::state& state, endless_layout& dest) {
	auto font_index = text::font_index_from_font_id(state, dest.fixed_parameters.font_id);
	auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);

	auto& font = state.font_collection.get_font(state, font_index);
	auto text_height = int32_t(std::ceil(font.line_height(font_size)));
	auto line_height = text_height + dest.fixed_parameters.leading;
	dest.base_layout.number_of_lines += 1;
	dest.y_cursor += line_height;
}

text::alignment localized_alignment(sys::state& state, text::alignment in) {
	if(in == alignment::left && state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
		return alignment::right;
	} else if(in == alignment::right && state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
		return alignment::left;
	}
	return in;
}
ui::alignment localized_alignment(sys::state& state, ui::alignment in) {
	if(in == ui::alignment::left && state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
		return ui::alignment::right;
	} else if(in == ui::alignment::right && state.world.locale_get_native_rtl(state.font_collection.get_current_locale())) {
		return ui::alignment::left;
	}
	return in;
}

void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view txt, text_color color, substitution source) {

	auto& font = state.font_collection.get_font(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id));
	auto text_height = int32_t(std::ceil(font.line_height(text::size_from_font_id(dest.fixed_parameters.font_id))));
	auto line_height = text_height + dest.fixed_parameters.leading;

	auto tmp_color = color;

	if(std::holds_alternative<dcon::nation_id>(source)
		|| std::holds_alternative<dcon::province_id>(source)
		|| std::holds_alternative<dcon::state_instance_id>(source)
		|| std::holds_alternative<dcon::state_definition_id>(source)) {

		if(!dest.fixed_parameters.suppress_hyperlinks) {
			if(color != text_color::black) {
				tmp_color = text_color::light_blue;
			} else {
				tmp_color = text_color::dark_blue;
			}
		} else {
			if(color != text_color::black) {
				tmp_color = text_color::yellow;
			} else {
				tmp_color = text_color::orange;
			}
		}
	}

	uint32_t start_position = 0;
	uint32_t end_position = 0;
	bool first_in_line = true;

	
	auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);

	text::stored_glyphs all_glyphs(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id), std::string(txt));

	auto find_non_ws = [&](uint32_t start) {
		for(uint32_t i = start; i < all_glyphs.glyph_count; ++i) {
			uint32_t c = text::codepoint_from_utf8(txt.data() + all_glyphs.glyph_info[i].cluster, txt.data() + txt.size());
			if(!codepoint_is_space(c) && !codepoint_is_line_break(c))
				return i;
		}
		return all_glyphs.glyph_count;
	};

	while(end_position < all_glyphs.glyph_count) {
		uint32_t next_wb = all_glyphs.glyph_count;
		uint32_t next_word = all_glyphs.glyph_count;
		bool force_end_of_line = false;

		for(uint32_t i = end_position; i < all_glyphs.glyph_count; ++i) {
			uint32_t c = text::codepoint_from_utf8(txt.data() + all_glyphs.glyph_info[i].cluster, txt.data() + txt.size());

			if(c == 0) {
				next_wb = i;
				next_word = find_non_ws(i);
				break;
			}

			if(codepoint_is_line_break(c)) {
				next_wb = i;
				next_word = find_non_ws(i);
				force_end_of_line = true;
				break;
			}
			if(codepoint_is_space(c)) {
				next_wb = i;
				next_word = find_non_ws(i);
				break;
			}
		}

		float extent = font.text_extent(state, all_glyphs, start_position, next_wb - start_position, font_size);

		if(first_in_line && int32_t(box.x_offset + dest.fixed_parameters.left) == box.x_position && box.x_position + extent >= dest.fixed_parameters.right) {
			// the current word is too long for the text box, break it internally

			auto sub_end_pos = start_position + 1;
			auto sub_prev_end_pos = sub_end_pos;
			while(sub_end_pos < next_wb) {
				extent = font.text_extent(state, all_glyphs, start_position, sub_end_pos - start_position, font_size);
				if(box.x_position + extent >= dest.fixed_parameters.right)
					break;

				sub_prev_end_pos = sub_end_pos;
				++sub_end_pos;
			}
			
			extent = font.text_extent(state, all_glyphs, start_position, sub_prev_end_pos - start_position, font_size);

			dest.base_layout.contents.push_back(
				text_chunk{ text::stored_glyphs(all_glyphs, start_position, sub_prev_end_pos - start_position), box.x_position, (!dest.fixed_parameters.suppress_hyperlinks) ? source : std::monostate{}, int16_t(box.y_position), int16_t(extent), int16_t(text_height), tmp_color });

			box.y_size = std::max(box.y_size, box.y_position + line_height);
			box.x_size = std::max(box.x_size, int32_t(box.x_position + extent));
			box.x_position += extent;

			impl::lb_finish_line(dest, box, line_height);

			start_position = sub_prev_end_pos;
			end_position = sub_prev_end_pos;
		} else if(force_end_of_line || box.x_position + extent >= dest.fixed_parameters.right) {
			if(box.x_position + extent >= dest.fixed_parameters.right) {
				next_wb = end_position;
				next_word = find_non_ws(end_position);
				extent = font.text_extent(state, all_glyphs, start_position, next_wb - start_position, font_size);
			}

			if(next_wb != start_position) {
				dest.base_layout.contents.push_back(
						text_chunk{ text::stored_glyphs(all_glyphs, start_position, next_wb - start_position), box.x_position, (!dest.fixed_parameters.suppress_hyperlinks) ? source : std::monostate{},
								int16_t(box.y_position), int16_t(extent), int16_t(text_height), tmp_color});

				box.y_size = std::max(box.y_size, box.y_position + line_height);
				box.x_size = std::max(box.x_size, int32_t(box.x_position + extent));

				box.x_position += extent;
			}
			impl::lb_finish_line(dest, box, line_height);

			end_position = next_word;
			start_position = next_word;
			first_in_line = true;
		} else if(next_word >= all_glyphs.glyph_count) {
			// we've reached the end of the text
			extent = font.text_extent(state, all_glyphs, start_position, next_word - start_position, font_size);
			dest.base_layout.contents.push_back(
					text_chunk{ text::stored_glyphs(all_glyphs, start_position, next_word - start_position), box.x_position, (!dest.fixed_parameters.suppress_hyperlinks) ? source : std::monostate{},
							int16_t(box.y_position), int16_t(extent), int16_t(text_height), tmp_color});

			box.y_size = std::max(box.y_size, box.y_position + line_height);
			box.x_size = std::max(box.x_size, int32_t(box.x_position + extent));

			box.x_position += extent;
			break;
		} else {
			end_position = next_word;
			first_in_line = false;
		}
	}
}

namespace impl {

std::string lb_resolve_substitution(sys::state& state, substitution sub, substitution_map const& mp) {
	if(std::holds_alternative<std::string_view>(sub)) {
		return std::string(std::get<std::string_view>(sub));
	} else if(std::holds_alternative<dcon::text_key>(sub)) {
		auto tkey = std::get<dcon::text_key>(sub);
		if(auto it = state.locale_key_to_text_sequence.find(tkey); it != state.locale_key_to_text_sequence.end()) {
			return std::string(state.locale_string_view(it->second));
		} else {
			return std::string(state.to_string_view(tkey));
		}
	} else if(std::holds_alternative<dcon::nation_id>(sub)) {
		dcon::nation_id nid = std::get<dcon::nation_id>(sub);
		return text::produce_simple_string(state, text::get_name(state, nid));
	} else if(std::holds_alternative<dcon::state_instance_id>(sub)) {
		dcon::state_instance_id sid = std::get<dcon::state_instance_id>(sub);
		return get_dynamic_state_name(state, sid);
	} else if(std::holds_alternative<dcon::province_id>(sub)) {
		auto pid = std::get<dcon::province_id>(sub);
		return text::produce_simple_string(state, state.world.province_get_name(pid));
	} else if(std::holds_alternative<dcon::national_identity_id>(sub)) {
		auto t = std::get<dcon::national_identity_id>(sub);
		auto h = state.world.national_identity_get_nation_from_identity_holder(t);
		return text::produce_simple_string(state, h ? text::get_name(state, h) : state.world.national_identity_get_name(t));
	} else if(std::holds_alternative<int64_t>(sub)) {
		return std::to_string(std::get<int64_t>(sub));
	} else if(std::holds_alternative<fp_one_place>(sub)) {
		return text::format_float(std::get<fp_one_place>(sub).value, 1);
	} else if(std::holds_alternative<fp_two_places>(sub)) {
		return text::format_float(std::get<fp_two_places>(sub).value, 2);
	} else if(std::holds_alternative<fp_three_places>(sub)) {
		return text::format_float(std::get<fp_three_places>(sub).value, 3);
	} else if(std::holds_alternative<fp_four_places>(sub)) {
		return text::format_float(std::get<fp_four_places>(sub).value, 4);
	} else if(std::holds_alternative<sys::date>(sub)) {
		return date_to_string(state, std::get<sys::date>(sub));
	} else if(std::holds_alternative<fp_currency>(sub)) {
		return text::format_money(std::get<fp_currency>(sub).value);
	} else if(std::holds_alternative<pretty_integer>(sub)) {
		return prettify(std::get<pretty_integer>(sub).value);
	} else if(std::holds_alternative<fp_percentage>(sub)) {
		return text::format_percentage(std::get<fp_percentage>(sub).value, 0);
	} else if(std::holds_alternative<fp_percentage_one_place>(sub)) {
		return text::format_percentage(std::get<fp_percentage_one_place>(sub).value, 1);
	} else if(std::holds_alternative<int_percentage>(sub)) {
		return std::to_string(std::get<int_percentage>(sub).value) + "%";
	} else if(std::holds_alternative<int_wholenum>(sub)) {
		return text::format_wholenum(std::get<int_wholenum>(sub).value);
	} else if(std::holds_alternative<dcon::state_definition_id>(sub)) {
		return produce_simple_string(state, state.world.state_definition_get_name(std::get<dcon::state_definition_id>(sub)));
	} else {
		return std::string("?");
	}
}

} // namespace impl

void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, dcon::text_key source_text, substitution_map const& mp) {
	if(!source_text)
		return;

	auto current_color = dest.fixed_parameters.color;
	auto font_index = text::font_index_from_font_id(state, dest.fixed_parameters.font_id);
	auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);

	auto& font = state.font_collection.get_font(state, font_index);
	auto text_height = int32_t(std::ceil(font.line_height(font_size)));
	auto line_height = text_height + dest.fixed_parameters.leading;

	std::string_view sv;
	if(auto it = state.locale_key_to_text_sequence.find(source_text); it != state.locale_key_to_text_sequence.end()) {
		sv = state.locale_string_view(it->second);
	} else {
		sv = state.to_string_view(source_text);
	}

	char const* seq_start = sv.data();
	char const* seq_end = sv.data() + sv.size();
	char const* section_start = seq_start;

	auto add_text_range = [&](std::string_view sv) {
		if(sv.length() > 0) {
			add_to_layout_box(state, dest, box, sv, current_color, std::monostate{});
		}
	};

	for(char const* pos = seq_start; pos < seq_end;) {
		bool colour_esc = false;
		if(pos + 1 < seq_end && uint8_t(*pos) == 0xC2 && uint8_t(*(pos + 1)) == 0xA7) {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			pos += 2;
			section_start = pos;
			if(pos < seq_end) {
				auto newcolor = char_to_color(*pos);
				if(newcolor == text_color::reset)
					current_color = dest.fixed_parameters.color;
				else
					current_color = newcolor;

				pos += 1;
				section_start = pos;
			}
		} else if(pos + 2 < seq_end && uint8_t(*pos) == 0xEF && uint8_t(*(pos + 1)) == 0xBF && uint8_t(*(pos + 2)) == 0xBD && is_qmark_color(*(pos + 3))) {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			section_start = pos += 3;

			if(pos < seq_end) {
				auto newcolor = char_to_color(*pos);
				if(newcolor == text_color::reset)
					current_color = dest.fixed_parameters.color;
				else
					current_color = newcolor;

				pos += 1;
				section_start = pos;
			}
		} else if(pos + 1 < seq_end && *pos == '?' && is_qmark_color(*(pos + 1))) {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			pos += 1;
			section_start = pos;
			// This colour escape sequence must be followed by something, otherwise
			// we should probably discard the last colour command
			if(pos < seq_end) {

				auto newcolor = char_to_color(*pos);
				if(newcolor == text_color::reset)
					current_color = dest.fixed_parameters.color;
				else
					current_color = newcolor;

				pos += 1;
				section_start = pos;
			}
		} else if(*pos == '$') {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			const char* vend = pos + 1;
			for(; vend != seq_end && *vend != '$'; ++vend)
				;
			if(vend > pos + 1) {
				auto var_type = variable_type_from_name(std::string_view(pos + 1, vend - pos - 1));
				if(var_type != variable_type::error_no_matching_value) {
					if(auto it = mp.find(uint32_t(var_type)); it != mp.end()) {
						auto txt = impl::lb_resolve_substitution(state, it->second, mp);
						add_to_layout_box(state, dest, box, std::string_view(txt), current_color, it->second);
					} else {
						add_to_layout_box(state, dest, box, std::string_view("???"), current_color, std::monostate{});
					}
				} else {
					add_to_layout_box(state, dest, box, std::string_view(pos, (vend - pos) + 1), current_color, std::monostate{});
				}
			}
			pos = vend + 1;
			section_start = pos;
		} else if(pos + 1 < seq_end && *pos == '\\' && *(pos + 1) == 'n') {
			add_text_range(std::string_view(section_start, pos - section_start));

			add_line_break_to_layout_box(state, dest, box);
			section_start = pos += 2;
		} else {
			++pos;
		}
	}

	if(section_start < seq_end)
		add_text_range(std::string_view(section_start, seq_end - section_start));
}

void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, substitution val, text_color color) {
	auto txt = impl::lb_resolve_substitution(state, val, substitution_map{});
	add_to_layout_box(state, dest, box, std::string_view(txt), color, val);
}
void add_to_layout_box(sys::state& state, layout_base& dest, layout_box& box, std::string const& val, text_color color) {
	add_to_layout_box(state, dest, box, std::string_view(val), color, std::monostate{});
}
void add_space_to_layout_box(sys::state& state, layout_base& dest, layout_box& box) {
	auto& font = state.font_collection.get_font(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id));
	auto glyphid = FT_Get_Char_Index(font.font_face, ' ');
	box.x_position += font.base_glyph_width(glyphid) * text::size_from_font_id(dest.fixed_parameters.font_id) / 64.f;
}

layout_box open_layout_box(layout_base& dest, int32_t indent) {
	return layout_box{dest.base_layout.contents.size(), dest.base_layout.contents.size(), indent, 0, 0,
			float(indent + dest.fixed_parameters.left), 0, dest.fixed_parameters.color};
}
void close_layout_box(columnar_layout& dest, layout_box& box) {
	impl::lb_finish_line(dest, box, 0);

	if(box.y_size + dest.y_cursor >= dest.fixed_parameters.bottom) { // make new column
		dest.current_column_x = dest.used_width + dest.column_width;
		for(auto i = box.first_chunk; i < dest.base_layout.contents.size(); ++i) {
			dest.base_layout.contents[i].y += dest.fixed_parameters.top;
			dest.base_layout.contents[i].x += float(dest.current_column_x);
			dest.used_width = std::max(dest.used_width, int32_t(dest.base_layout.contents[i].x + dest.base_layout.contents[i].width));
		}
		dest.y_cursor = box.y_size + dest.fixed_parameters.top;
	} else { // append to current column
		for(auto i = box.first_chunk; i < dest.base_layout.contents.size(); ++i) {
			dest.base_layout.contents[i].y += int16_t(dest.y_cursor);
			dest.base_layout.contents[i].x += float(dest.current_column_x);
			dest.used_width = std::max(dest.used_width, int32_t(dest.base_layout.contents[i].x + dest.base_layout.contents[i].width));
		}
		dest.y_cursor += box.y_size;
	}
	dest.used_height = std::max(dest.used_height, dest.y_cursor);
}
void close_layout_box(endless_layout& dest, layout_box& box) {
	impl::lb_finish_line(dest, box, 0);

	for(auto i = box.first_chunk; i < dest.base_layout.contents.size(); ++i) {
		dest.base_layout.contents[i].y += int16_t(dest.y_cursor);
	}

	dest.y_cursor += box.y_size;
}

void close_layout_box(layout_base& dest, layout_box& box) {
	dest.internal_close_box(box);
}

void columnar_layout::internal_close_box(layout_box& box) {
	close_layout_box(*this, box);
}
void endless_layout::internal_close_box(layout_box& box) {
	close_layout_box(*this, box);
}

columnar_layout create_columnar_layout(layout& dest, layout_parameters const& params, int32_t column_width) {
	dest.contents.clear();
	dest.number_of_lines = 0;
	return columnar_layout(dest, params, 0, 0, params.top, column_width);
}

// Reduces code repeat
void localised_format_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view key, text::substitution_map const& sub) {
	if(auto k = state.lookup_key(key); k) {
		add_to_layout_box(state, dest, box, k, sub);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
}

void localised_single_sub_box(sys::state& state, layout_base& dest, layout_box& box, std::string_view key, variable_type subkey,
		substitution value) {
	text::substitution_map sub;
	text::add_to_substitution_map(sub, subkey, value);
	if(auto k = state.lookup_key(key); k) {
		add_to_layout_box(state, dest, box, k, sub);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
}

void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	add_to_layout_box(state, dest, box, txt);
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	text::substitution_map sub;
	text::add_to_substitution_map(sub, subkey, value);
	add_to_layout_box(state, dest, box, txt, sub);
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, int32_t indent) {

	auto box = text::open_layout_box(dest, indent);
	text::substitution_map sub;
	text::add_to_substitution_map(sub, subkey, value);
	text::add_to_substitution_map(sub, subkey_b, value_b);
	add_to_layout_box(state, dest, box, txt, sub);
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, int32_t indent) {

	auto box = text::open_layout_box(dest, indent);
	text::substitution_map sub;
	text::add_to_substitution_map(sub, subkey, value);
	text::add_to_substitution_map(sub, subkey_b, value_b);
	text::add_to_substitution_map(sub, subkey_c, value_c);
	add_to_layout_box(state, dest, box, txt, sub);
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, dcon::text_key txt, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, variable_type subkey_d,
		substitution value_d, int32_t indent) {

	auto box = text::open_layout_box(dest, indent);
	text::substitution_map sub;
	text::add_to_substitution_map(sub, subkey, value);
	text::add_to_substitution_map(sub, subkey_b, value_b);
	text::add_to_substitution_map(sub, subkey_c, value_c);
	text::add_to_substitution_map(sub, subkey_d, value_d);
	add_to_layout_box(state, dest, box, txt, sub);
	text::close_layout_box(dest, box);
}

void add_line(sys::state& state, layout_base& dest, std::string_view key, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	if(auto k = state.lookup_key(key); k) {
		add_to_layout_box(state, dest, box, k);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);

	auto str = state.font_collection.get_font(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id)).get_conditional_indicator(condition_met);
	if(condition_met) {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::green);
	} else {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::red);
	}

	text::add_space_to_layout_box(state, dest, box);

	if(auto k = state.lookup_key(key); k) {
		add_to_layout_box(state, dest, box, k);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);

	auto str = state.font_collection.get_font(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id)).get_conditional_indicator(condition_met);
	if(condition_met) {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::green);
	} else {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::red);
	}

	text::add_space_to_layout_box(state, dest, box);

	if(auto k = state.lookup_key(key); k) {
		text::substitution_map m;
		add_to_substitution_map(m, subkey, value);
		add_to_layout_box(state, dest, box, k, m);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, variable_type subkeyb, substitution valueb, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);

	auto str = state.font_collection.get_font(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id)).get_conditional_indicator(condition_met);
	if(condition_met) {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::green);
	} else {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::red);
	}

	text::add_space_to_layout_box(state, dest, box);

	if(auto k = state.lookup_key(key); k) {
		text::substitution_map m;
		add_to_substitution_map(m, subkey, value);
		add_to_substitution_map(m, subkeyb, valueb);
		add_to_layout_box(state, dest, box, k, m);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line_with_condition(sys::state& state, layout_base& dest, std::string_view key, bool condition_met, variable_type subkey, substitution value, variable_type subkeyb, substitution valueb, variable_type subkeyc, substitution valuec, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);

	auto str = state.font_collection.get_font(state, text::font_index_from_font_id(state, dest.fixed_parameters.font_id)).get_conditional_indicator(condition_met);
	if(condition_met) {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::green);
	} else {
		text::add_to_layout_box(state, dest, box, std::string_view(str), text::text_color::red);
	}

	text::add_space_to_layout_box(state, dest, box);

	if(auto k = state.lookup_key(key); k) {
		text::substitution_map m;
		add_to_substitution_map(m, subkey, value);
		add_to_substitution_map(m, subkeyb, valueb);
		add_to_substitution_map(m, subkeyc, valuec);
		add_to_layout_box(state, dest, box, k, m);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		int32_t indent) {

	auto box = text::open_layout_box(dest, indent);
	if(auto k = state.lookup_key(key); k) {
		text::substitution_map sub;
		text::add_to_substitution_map(sub, subkey, value);

		add_to_layout_box(state, dest, box, k, sub);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	if(auto k = state.lookup_key(key); k) {
		text::substitution_map sub;
		text::add_to_substitution_map(sub, subkey, value);
		text::add_to_substitution_map(sub, subkey_b, value_b);

		add_to_layout_box(state, dest, box, k, sub);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	if(auto k = state.lookup_key(key); k) {
		text::substitution_map sub;
		text::add_to_substitution_map(sub, subkey, value);
		text::add_to_substitution_map(sub, subkey_b, value_b);
		text::add_to_substitution_map(sub, subkey_c, value_c);

		add_to_layout_box(state, dest, box, k, sub);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}
void add_line(sys::state& state, layout_base& dest, std::string_view key, variable_type subkey, substitution value,
		variable_type subkey_b, substitution value_b, variable_type subkey_c, substitution value_c, variable_type subkey_d,
		substitution value_d, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	if(auto k = state.lookup_key(key); k) {
		text::substitution_map sub;
		text::add_to_substitution_map(sub, subkey, value);
		text::add_to_substitution_map(sub, subkey_b, value_b);
		text::add_to_substitution_map(sub, subkey_c, value_c);
		text::add_to_substitution_map(sub, subkey_d, value_d);

		add_to_layout_box(state, dest, box, k, sub);
	} else {
		add_to_layout_box(state, dest, box, key);
	}
	text::close_layout_box(dest, box);
}


void add_divider_to_layout_box(sys::state& state, layout_base& dest, layout_box& box) {
	text::add_line_break_to_layout_box(state, dest, box);
	text::add_to_layout_box(state, dest, box, std::string_view(" "));
	text::add_line_break_to_layout_box(state, dest, box);
}

void nation_name_and_flag(sys::state& state, dcon::nation_id n, layout_base& dest, int32_t indent) {
	auto box = text::open_layout_box(dest, indent);
	auto ident = state.world.nation_get_identity_from_identity_holder(n);
	auto flag_str = std::string("@");
	if(ident)
		flag_str += nations::int_to_tag(state.world.national_identity_get_identifying_int(ident));
	else
		flag_str += "REB";
	add_to_layout_box(state, dest, box, std::string_view{flag_str});
	add_space_to_layout_box(state, dest, box);
	add_to_layout_box(state, dest, box, text::get_name(state, n));
	text::close_layout_box(dest, box);
}

std::string resolve_string_substitution(sys::state& state, dcon::text_key source_text, substitution_map const& mp) {
	std::string result;

	std::string_view sv;
	if(auto it = state.locale_key_to_text_sequence.find(source_text); it != state.locale_key_to_text_sequence.end()) {
		sv = state.locale_string_view(it->second);
	} else {
		sv = state.to_string_view(source_text);
	}

	char const* seq_start = sv.data();
	char const* seq_end = sv.data() + sv.size();
	char const* section_start = seq_start;

	auto add_text_range = [&](std::string_view sv) {
		result += sv;
	};

	for(char const* pos = seq_start; pos < seq_end;) {
		bool colour_esc = false;
		if(pos + 1 < seq_end && uint8_t(*pos) == 0xC2 && uint8_t(*(pos + 1)) == 0xA7) {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			pos += 2;
			section_start = pos;
			if(pos < seq_end) {
				pos += 1;
				section_start = pos;
			}
		} else if(pos + 2 < seq_end && uint8_t(*pos) == 0xEF && uint8_t(*(pos + 1)) == 0xBF && uint8_t(*(pos + 2)) == 0xBD && is_qmark_color(*(pos + 3))) {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			section_start = pos += 3;

			if(pos < seq_end) {
				pos += 1;
				section_start = pos;
			}
		} else if(pos + 1 < seq_end && *pos == '?' && is_qmark_color(*(pos + 1))) {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			pos += 1;
			section_start = pos;

			if(pos < seq_end) {
				pos += 1;
				section_start = pos;
			}
		} else if(*pos == '$') {
			if(section_start < pos)
				add_text_range(std::string_view(section_start, pos - section_start));

			const char* vend = pos + 1;
			for(; vend != seq_end && *vend != '$'; ++vend)
				;
			if(vend > pos + 1) {
				auto var_type = variable_type_from_name(std::string_view(pos + 1, vend - pos - 1));
				if(var_type != variable_type::error_no_matching_value) {
					if(auto it = mp.find(uint32_t(var_type)); it != mp.end()) {
						result += impl::lb_resolve_substitution(state, it->second, mp);
					} else {
						result += "???";
					}
				} else {
					result += std::string_view(pos, (vend - pos) + 1);
				}
			}
			pos = vend + 1;
			section_start = pos;
		} else if(pos + 1 < seq_end && *pos == '\\' && *(pos + 1) == 'n') {
			add_text_range(std::string_view(section_start, pos - section_start));
			section_start = pos += 2;
		} else {
			++pos;
		}
	}

	if(section_start < seq_end)
		add_text_range(std::string_view(section_start, seq_end - section_start));

	return result;
}

std::string resolve_string_substitution(sys::state& state, std::string_view key, substitution_map const& mp) {
	dcon::text_key source_text;
	if(auto k = state.lookup_key(key); k) {
		source_text = k;
	}

	if(!source_text)
		return std::string{key};

	return resolve_string_substitution(state, source_text, mp);
}

} // namespace text
