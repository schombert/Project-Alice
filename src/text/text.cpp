#include <string_view>

#include "text.hpp"
#include "system_state.hpp"
#include "parsers.hpp"
#include "simple_fs.hpp"

namespace text {
	text_color char_to_color(char in) {
		switch(in) {
			case 'W': return text_color::white;
			case 'G': return text_color::green;
			case 'R': return text_color::red;
			case 'Y': return text_color::yellow;
			case 'b': return text_color::black;
			default: return text_color::unspecified;
		}
	}

	std::string lowercase_str(std::string_view sv) {
		std::string result;
		result.reserve(sv.length());
		for(auto ch : sv) {
			result += char(tolower(ch));
		}
		return result;
	}

	void consume_csv_file(sys::state& state, uint32_t language, char const* file_content, uint32_t file_size) {
		auto start = (file_size != 0 && file_content[0] == '#') ? parsers::csv_advance_to_next_line(file_content, file_content + file_size) : file_content;
		while(start < file_content + file_size) {
			start = parsers::parse_first_and_nth_csv_values(language, start, file_content + file_size, ';', [&state](std::string_view key, std::string_view content) {
				
				char const* section_start = content.data();
				char const* seq_start = content.data();
				char const* seq_end = content.data() + content.size();

				const auto component_start_index = state.text_components.size();

				for(const char* pos = seq_start; pos < seq_end; ++pos) {
					if(uint8_t(*pos) == 0xA7) {
						if(section_start != pos) {
							auto added_key = state.add_to_pool(std::string_view(section_start, pos - section_start));
							state.text_components.emplace_back( added_key );
						}
						section_start = pos + 2;
						if(pos + 1 < seq_end)
							state.text_components.emplace_back( char_to_color(*(pos + 1)) );
						pos += 1;
					} else if(*pos == '$') {
						if(section_start != pos) {
							auto added_key = state.add_to_pool(std::string_view(section_start, pos - section_start));
							state.text_components.emplace_back( added_key );
						}

						const char* vend = pos + 1;
						while(vend != seq_end) {
							if(*vend == '$')
								break;
							++vend;
						}

						if(vend > pos + 1) {
							state.text_components.emplace_back( variable_type_from_name(std::string_view(pos + 1, pos + 1 - vend)) );
						}
						pos = vend;
						section_start = vend + 1;
					} else if(*pos == '\\' && pos + 1 != seq_end && *(pos + 1) == 'n') {
						if(section_start != pos) {
							auto added_key = state.add_to_pool(std::string_view(section_start, pos - section_start));
							state.text_components.emplace_back( added_key );
						}

						section_start = pos + 2;
						state.text_components.emplace_back(line_break{});
						++pos;
					}
				}

				if(section_start < seq_end) {
					auto added_key = state.add_to_pool(std::string_view(section_start, seq_end - section_start));
					state.text_components.emplace_back( added_key );
				}

				

				auto to_lower_temp = lowercase_str(key);
				if(auto it = state.key_to_text_sequence.find(to_lower_temp); it != state.key_to_text_sequence.end()) {
					// maybe report an error here -- repeated definition
					state.text_sequences[it->second] = text_sequence{
							static_cast<uint16_t>(component_start_index),
							static_cast<uint16_t>(state.text_components.size() - component_start_index) };
				} else {
					const auto nh = state.text_sequences.size();
					state.text_sequences.emplace_back(
						text_sequence{
							static_cast<uint16_t>(component_start_index),
							static_cast<uint16_t>(state.text_components.size() - component_start_index) });

					auto main_key = state.add_to_pool_lowercase(key);
					state.key_to_text_sequence.insert_or_assign(main_key, dcon::text_sequence_id(uint16_t(nh)));
				}

			});
		}
	}

	void load_text_data(sys::state& state, uint32_t language) {
		auto rt = get_root(state.common_fs);

		// first, load in special mod gui
		// TODO put this in a better location
		auto alice_csv = open_file(rt, NATIVE("assets/alice.csv"));
		if(alice_csv) {
			auto content = view_contents(*alice_csv);
			consume_csv_file(state, language, content.data, content.file_size);
		}

		auto text_dir = open_directory(rt, NATIVE("localisation"));
		auto all_files = list_files(text_dir, NATIVE(".csv"));

		for(auto& file : all_files) {
			auto ofile = open_file(file);
			if(ofile) {
				auto content = view_contents(*ofile);
				consume_csv_file(state, language, content.data, content.file_size);
			}
		}
	}

	template<size_t N>
	bool is_fixed_token_ci(std::string_view v, const char(&t)[N]) {
		if(v.length() != (N - 1))
			return false;
		for(unsigned int i = 0; i < N - 1; ++i) {
			if(tolower(v[i]) != t[i])
				return false;
		}
		return true;
	}

#define CT_STRING_ENUM(X) else if(is_fixed_token_ci(v, # X )) return variable_type:: X;

	variable_type variable_type_from_name(std::string_view v) {
		if(v.length() == 1) {
			if(false) { }
			CT_STRING_ENUM(d)
			CT_STRING_ENUM(m)
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
		} else if(v.length() == 3) {
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
			CT_STRING_ENUM(reqlevel)
			CT_STRING_ENUM(required)
			CT_STRING_ENUM(resource)
			CT_STRING_ENUM(tag_0_0_)
			CT_STRING_ENUM(theirnum)
			CT_STRING_ENUM(totalemi)
			CT_STRING_ENUM(totalimm)
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
		} else if(v.length() == 17) {
			if(false) { }
			CT_STRING_ENUM(cb_target_name_adj)
			CT_STRING_ENUM(head_of_government)
		} else if(v.length() == 18) {
			if(false) { }
			CT_STRING_ENUM(culture_group_union)
			CT_STRING_ENUM(numspecialfactories)
		} else if(is_fixed_token_ci(v, "invested_in_us_message")) {
			return variable_type::invested_in_us_message;
		}

		return variable_type::error_no_matching_value;
	}
#undef CT_STRING_ENUM

	char16_t win1250toUTF16(char in) {
		constexpr static char16_t converted[256] =
			//       0       1         2         3         4         5         6         7         8         9         A         B         C         D         E         F
			/*0*/{ u' ' ,  u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u'\t' ,   u'\n' ,   u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,
			/*1*/  u' ' ,  u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,    u' ' ,
			/*2*/  u' ' ,  u'!' ,    u'\"',    u'#' ,    u'$' ,    u'%' ,    u'&' ,    u'\'',    u'(' ,    u')' ,    u'*' ,    u'+' ,    u',' ,    u'-' ,    u'.' ,    u'/' ,
			/*3*/  u'0' ,  u'1' ,    u'2' ,    u'3' ,    u'4' ,    u'5' ,    u'6' ,    u'7' ,    u'8' ,    u'9' ,    u':' ,    u';' ,    u'<' ,    u'=' ,    u'>' ,    u'?' ,
			/*4*/  u'@' ,  u'A' ,    u'B' ,    u'C' ,    u'D' ,    u'E' ,    u'F' ,    u'G' ,    u'H' ,    u'I' ,    u'J' ,    u'K' ,    u'L' ,    u'M' ,    u'N' ,    u'O' ,
			/*5*/  u'P' ,  u'Q' ,    u'R' ,    u'S' ,    u'T' ,    u'U' ,    u'V' ,    u'W' ,    u'X' ,    u'Y' ,    u'Z' ,    u'[' ,    u'\\',    u']' ,    u'^' ,    u'_' ,
			/*6*/  u'`' ,  u'a' ,    u'b' ,    u'c' ,    u'd' ,    u'e' ,    u'f' ,    u'g' ,    u'h' ,    u'i' ,    u'j' ,    u'k' ,    u'l',     u'm' ,    u'n' ,    u'o' ,
			/*7*/  u'p' ,  u'q' ,    u'r' ,    u's' ,    u't' ,    u'u' ,    u'v' ,    u'w' ,    u'x' ,    u'y' ,    u'z' ,    u'{' ,    u'|',     u'}' ,    u'~' ,    u' ' ,
			/*8*/u'\u20AC',u' ',     u'\u201A',u' ',     u'\u201E',u'\u2026',u'\u2020',u'\u2021',u' ',     u'\u2030',u'\u0160',u'\u2039',u'\u015A',u'\u0164',u'\u017D',u'\u0179',
			/*9*/u' ',     u'\u2018',u'\u2019',u'\u201C',u'\u201D',u'\u2022',u'\u2013',u'\u2014',u' ',     u'\u2122',u'\u0161',u'\u203A',u'\u015B',u'\u0165',u'\u017E',u'\u017A',
			/*A*/u'\u00A0',     u'\u02C7',u'\u02D8',u'\u00A2',u'\u00A3',u'\u0104',u'\u00A6',u'\u00A7',u'\u00A8',u'\u00A9',u'\u015E',u'\u00AB',u'\u00AC',u'-',     u'\u00AE',u'\u017B',
			/*B*/u'\u00B0',u'\u00B1',u'\u02DB',u'\u0142',u'\u00B4',u'\u00B5',u'\u00B6',u'\u00B7',u'\u00B8',u'\u0105',u'\u015F',u'\u00BB',u'\u013D',u'\u02DD',u'\u013E',u'\u017C',
			/*C*/u'\u0154',u'\u00C1',u'\u00C2',u'\u0102',u'\u00C4',u'\u0139',u'\u0106',u'\u00C7',u'\u010C',u'\u00C9',u'\u0118',u'\u00CB',u'\u011A',u'\u00CD',u'\u00CE',u'\u010E',
			/*D*/u'\u0110',u'\u0143',u'\u0147',u'\u00D3',u'\u00D4',u'\u0150',u'\u00D6',u'\u00D7',u'\u0158',u'\u016E',u'\u00DA',u'\u0170',u'\u00DC',u'\u00DD',u'\u0162',u'\u00DF',
			/*E*/u'\u0115',u'\u00E1',u'\u00E2',u'\u0103',u'\u00E4',u'\u013A',u'\u0107',u'\u00E7',u'\u010D',u'\u00E9',u'\u0119',u'\u00EB',u'\u011B',u'\u00ED',u'\u00EE',u'\u010F',
			/*F*/u'\u0111',u'\u0144',u'\u0148',u'\u00F3',u'\u00F4',u'\u0151',u'\u00F6',u'\u00F7',u'\u0159',u'\u016F',u'\u00FA',u'\u0171',u'\u00FC',u'\u00FD',u'\u0163',u'\u02D9'
		};

		return converted[(uint8_t)in];
	}

	std::string produce_simple_string(sys::state const& state, dcon::text_sequence_id id) {
		std::string result;

		if(!id)
			return result;

		auto& seq = state.text_sequences[id];
		for(uint32_t i = 0; i < seq.component_count; ++i) {
			//std::variant<line_break, text_color, variable_type, dcon::text_key>
			if(std::holds_alternative<dcon::text_key>(state.text_components[i + seq.starting_component])) {
				result += state.to_string_view(std::get<dcon::text_key>(state.text_components[i + seq.starting_component]));
			} else if(std::holds_alternative<variable_type>(state.text_components[i + seq.starting_component])) {
				result += '?';
			}
		}

		return result;

	}

	std::string produce_simple_string(sys::state const& state, std::string_view txt) {
		auto it = state.key_to_text_sequence.find(lowercase_str(txt));
		if(it != state.key_to_text_sequence.end()) {
			return produce_simple_string(state, it->second);
		} else {
			return std::string(txt);
		}
	}

	dcon::text_sequence_id find_or_add_key(sys::state& state, std::string_view txt) {
		auto it = state.key_to_text_sequence.find(lowercase_str(txt));
		if(it != state.key_to_text_sequence.end()) {
			return it->second;
		} else {
			auto new_key = state.add_to_pool_lowercase(txt);
			auto component_sz = state.text_components.size();
			state.text_components.push_back(new_key);
			auto seq_size = state.text_sequences.size();
			state.text_sequences.push_back(text::text_sequence{ uint16_t(component_sz), uint16_t(1) });
			auto new_id = dcon::text_sequence_id(dcon::text_sequence_id::value_base_t(seq_size));
			state.key_to_text_sequence.insert_or_assign(new_key, new_id);
			return new_id;
		}
	}

	std::string prettify(int32_t num) {
		if(num < 1000) {
			return std::to_string(num);
		}
		size_t i = 0;
		while(num >= 1'000'000) {
			num /= 1000;
			i++;
		}

		std::string pretty_num = std::to_string(num / 10);
		pretty_num.insert(pretty_num.size() - 2, ".");

		const std::string unit_palette = "KMBTPZ";
		pretty_num += unit_palette[std::min(unit_palette.size() - 1, i)];
		
		return pretty_num;
	}

	template<class T>
	std::string get_name_as_string(sys::state const& state, T t) {
		return text::produce_simple_string(state, t.get_name());
	}

	void add_to_substitution_map(substitution_map& mp, variable_type key, substitution value) {
		mp.insert_or_assign(uint32_t(key), value);
	}

	std::string localize_month(sys::state const& state, uint16_t month) {
		switch(month) {
			case 1:
				return text::produce_simple_string(state, "January");
			case 2:
				return text::produce_simple_string(state, "February");
			case 3:
				return text::produce_simple_string(state, "March");
			case 4:
				return text::produce_simple_string(state, "April");
			case 5:
				return text::produce_simple_string(state, "May");
			case 6:
				return text::produce_simple_string(state, "June");
			case 7:
				return text::produce_simple_string(state, "July");
			case 8:
				return text::produce_simple_string(state, "August");
			case 9:
				return text::produce_simple_string(state, "September");
			case 10:
				return text::produce_simple_string(state, "October");
			case 11:
				return text::produce_simple_string(state, "November");
			case 12:
				return text::produce_simple_string(state, "December");
			default:
				return text::produce_simple_string(state, "January");
		}
	}

	std::string date_to_string(sys::state const& state, sys::date date) {
		sys::year_month_day ymd = date.to_ymd(state.start_date);
		return localize_month(state, ymd.month) + " " + std::to_string(ymd.day) + ", " + std::to_string(ymd.year);
	}

	text_chunk const* layout::get_chunk_from_position(int32_t x, int32_t y) const {
		for(auto& chunk : contents) {
			if(int32_t(chunk.x) <= x && x <= int32_t(chunk.x) + chunk.width && chunk.y <= y && y <= chunk.y + chunk.height) {
				return &chunk;
			}
		}
		return nullptr;
	}

	void create_endless_layout(layout& dest, sys::state const& state, layout_parameters const& params, dcon::text_sequence_id source_text, substitution_map const& mp) {
		dest.contents.clear();
		dest.number_of_lines = 0;

		uint32_t line_start = 0;

		auto seq = state.text_sequences[source_text];
		auto font_index = text::font_index_from_font_id(params.font_id);
		auto font_size = text::size_from_font_id(params.font_id);
		auto& font = state.font_collection.fonts[font_index - 1];
		auto text_height = int32_t(std::ceil(font.line_height(font_size)));
		auto line_height = text_height + params.leading;

		text::text_color current_color = params.color;
		float current_x = params.left;
		int32_t current_y = params.top;

		auto finish_line = [&](bool force = false) {
			if(!force && line_start == dest.contents.size())
				return;

			++dest.number_of_lines;

			if(params.align == alignment::left) {
				auto adjust = params.right - current_x;
				for(uint32_t i = line_start; i < dest.contents.size(); ++i) {
					dest.contents[i].x -= adjust;
				}
			} else if(params.align == alignment::center) {
				auto adjust = (params.right - current_x) / 2;
				for(uint32_t i = line_start; i < dest.contents.size(); ++i) {
					dest.contents[i].x -= adjust;
				}
			}
			current_x = params.left;
			current_y += line_height;
			line_start = uint32_t(dest.contents.size());
		};

		auto append_text_chunk = [&](std::string_view txt, substitution source) {
			size_t str_i = 0;
			size_t current_len = 0;
			float extent = 0.0f;
			float previous_extent = 0.0f;
			auto tmp_color = current_color;
			if(std::holds_alternative<dcon::nation_id>(source) || std::holds_alternative<dcon::province_id>(source) || std::holds_alternative<dcon::state_instance_id>(source)) {
				tmp_color = text_color::light_blue;
			}
			while(str_i < txt.length()) {
				auto next_wb = txt.find_first_of(" \r\n\t", str_i + current_len);
				if(next_wb == std::string_view::npos) {
					next_wb = txt.length();
				}
				next_wb = std::min(next_wb, txt.length()) - str_i;
				if(next_wb == current_len) {
					current_len++;
				} else {
					std::string_view segment = txt.substr(str_i, next_wb);
					previous_extent = extent;
					extent = font.text_extent(segment.data(), uint32_t(next_wb), font_size);
					if(current_len == 0 && current_x + extent >= params.right) {
						// the current word is too long for the text box, just let it overflow
						dest.contents.push_back(text_chunk{ std::string(segment), current_x, source, int16_t(current_y), int16_t(extent), int16_t(text_height), tmp_color });
						current_x += extent;
						str_i += next_wb;
						current_len = 0;
						finish_line();
					} else if(current_x + extent >= params.right) {
						std::string_view section{ segment.data(), current_len };
						dest.contents.push_back(text_chunk{ std::string(section), current_x, source, int16_t(current_y), int16_t(previous_extent), int16_t(text_height), tmp_color });
						current_x += previous_extent;
						current_y += line_height;
						str_i += current_len;
						current_len = 0;
						finish_line();
					} else if(next_wb == txt.length() - str_i) {
						// we've reached the end of the text
						dest.contents.push_back(text_chunk{ std::string(segment), current_x, source, int16_t(current_y), int16_t(extent), int16_t(text_height), tmp_color });
						current_x += extent;
						break;
					} else {
						current_len = next_wb;
					}
				}
			}
		};

		auto resolve_substitution = [&](substitution sub) {
			if(std::holds_alternative<std::string_view>(sub)) {
				return std::string(std::get<std::string_view>(sub));
			} else if(std::holds_alternative<dcon::text_key>(sub)) {
				auto tkey = std::get<dcon::text_key>(sub);
				return std::string(state.to_string_view(tkey));
			} else if(std::holds_alternative<dcon::nation_id>(sub)) {
				dcon::nation_id nid = std::get<dcon::nation_id>(sub);
				return text::produce_simple_string(state, state.world.nation_get_name(nid));
			} else if(std::holds_alternative<dcon::state_instance_id>(sub)) {
				dcon::state_instance_id sid = std::get<dcon::state_instance_id>(sub);
				auto name = state.world.state_definition_get_name(state.world.state_instance_get_definition(sid));
				return text::produce_simple_string(state, name);
			} else if(std::holds_alternative<dcon::province_id>(sub)) {
				auto pid = std::get<dcon::province_id>(sub);
				return text::produce_simple_string(state, state.world.province_get_name(pid));
			} else if(std::holds_alternative<int64_t>(sub)) {
				return std::to_string(std::get<int64_t>(sub));
			} else if(std::holds_alternative<float>(sub)) {
				return std::to_string(std::get<float>(sub));
			} else if(std::holds_alternative<sys::date>(sub)) {
				return date_to_string(state, std::get<sys::date>(sub));
			} else {
				return std::string("?");
			}
		};

		for(size_t i = seq.starting_component; i < size_t(seq.starting_component + seq.component_count); ++i) {
			if(std::holds_alternative<dcon::text_key>(state.text_components[i])) {
				auto tkey = std::get<dcon::text_key>(state.text_components[i]);
				std::string_view text = state.to_string_view(tkey);
				append_text_chunk(text, std::monostate{});
			} else if(std::holds_alternative<text::line_break>(state.text_components[i])) {
				finish_line(true);
			} else if(std::holds_alternative<text::text_color>(state.text_components[i])) {
				current_color = std::get<text::text_color>(state.text_components[i]);
			} else if(std::holds_alternative<text::variable_type>(state.text_components[i])) {
				auto var_type = std::get<text::variable_type>(state.text_components[i]);
				if(auto it = mp.find(uint32_t(var_type)); it != mp.end()) {
					append_text_chunk(resolve_substitution(it->second), it->second);
				} else {
					append_text_chunk("???", std::monostate{});
				}
			}
		}

		finish_line();
	}

	/*struct layout_box {
		size_t first_chunk = 0;
		int32_t x_offset = 0;
		int32_t y_size = 0;

		float x_position = 0;
		int32_t y_position = 0;
		text_color color = text_color::white;
	};
	struct columnar_layout {
		layout& base_layout;
		layout_parameters fixed_parameters;
		int32_t used_height = 0;
		int32_t used_width = 0;
		int32_t y_cursor = 0;
		int32_t current_column = 0;
		int32_t column_width = 0;
	};

	
	*/

	void add_to_layout_box(columnar_layout& dest, sys::state const& state, layout_box& box, std::string_view txt, text_color color, substitution source) {

		auto font_index = text::font_index_from_font_id(dest.fixed_parameters.font_id);
		auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);
		auto& font = state.font_collection.fonts[font_index - 1];
		auto text_height = int32_t(std::ceil(font.line_height(font_size)));
		auto line_height = text_height + dest.fixed_parameters.leading;

		auto finish_line = [&](bool force = false) {
			box.x_position = float(box.x_offset);
			box.y_position += line_height;
		};

		
		size_t str_i = 0;
		size_t current_len = 0;
		float extent = 0.0f;
		float previous_extent = 0.0f;
		auto tmp_color = color;
	
		while(str_i < txt.length()) {
			auto next_wb = txt.find_first_of(" \r\n\t", str_i + current_len);
			if(next_wb == std::string_view::npos) {
				next_wb = txt.length();
			}
			next_wb = std::min(next_wb, txt.length()) - str_i;
			if(next_wb == current_len) {
				current_len++;
			} else {
				std::string_view segment = txt.substr(str_i, next_wb);
				previous_extent = extent;
				extent = font.text_extent(segment.data(), uint32_t(next_wb), font_size);
				if(int32_t(box.x_position) == box.x_offset && box.x_position + extent >= dest.fixed_parameters.right) {
					// the current word is too long for the text box, just let it overflow
					dest.base_layout.contents.push_back(text_chunk{ std::string(segment), box.x_position, source, int16_t(box.y_position), int16_t(extent), int16_t(text_height), tmp_color });

					str_i += next_wb;

					box.y_size = std::max(box.y_size, box.y_position + line_height);
					box.x_size = std::max(box.x_size, int32_t(box.x_position + extent));
					current_len = 0;
					finish_line();
				} else if(box.x_position + extent >= dest.fixed_parameters.right) {
					if(current_len > 0) {
						std::string_view section{ segment.data(), current_len };
						dest.base_layout.contents.push_back(text_chunk{ std::string(section), box.x_position, source, int16_t(box.y_position), int16_t(previous_extent), int16_t(text_height), tmp_color });

						box.y_size = std::max(box.y_size, box.y_position + line_height);
						box.x_size = std::max(box.x_size, int32_t(box.x_position + previous_extent));
					}
					str_i += current_len;
					current_len = 0;
					finish_line();
				} else if(next_wb == txt.length() - str_i) {
					// we've reached the end of the text
					dest.base_layout.contents.push_back(text_chunk{ std::string(segment), box.x_position, source, int16_t(box.y_position), int16_t(extent), int16_t(text_height), tmp_color });

					box.y_size = std::max(box.y_size, box.y_position + line_height);
					box.x_size = std::max(box.x_size, int32_t(box.x_position + extent));

					box.x_position += extent;
					break;
				} else {
					current_len = next_wb;
				}
			}
		}
	}

	void add_to_layout_box(columnar_layout& dest, sys::state const& state, layout_box& box, dcon::text_sequence_id source_text, substitution_map const& mp) {

		auto current_color = dest.fixed_parameters.color;
		auto font_index = text::font_index_from_font_id(dest.fixed_parameters.font_id);
		auto font_size = text::size_from_font_id(dest.fixed_parameters.font_id);
		auto& font = state.font_collection.fonts[font_index - 1];
		auto text_height = int32_t(std::ceil(font.line_height(font_size)));
		auto line_height = text_height + dest.fixed_parameters.leading;

		auto resolve_substitution = [&](substitution sub) {
			if(std::holds_alternative<std::string_view>(sub)) {
				return std::string(std::get<std::string_view>(sub));
			} else if(std::holds_alternative<dcon::text_key>(sub)) {
				auto tkey = std::get<dcon::text_key>(sub);
				return std::string(state.to_string_view(tkey));
			} else if(std::holds_alternative<dcon::nation_id>(sub)) {
				dcon::nation_id nid = std::get<dcon::nation_id>(sub);
				return text::produce_simple_string(state, state.world.nation_get_name(nid));
			} else if(std::holds_alternative<dcon::state_instance_id>(sub)) {
				dcon::state_instance_id sid = std::get<dcon::state_instance_id>(sub);
				auto name = state.world.state_definition_get_name(state.world.state_instance_get_definition(sid));
				return text::produce_simple_string(state, name);
			} else if(std::holds_alternative<dcon::province_id>(sub)) {
				auto pid = std::get<dcon::province_id>(sub);
				return text::produce_simple_string(state, state.world.province_get_name(pid));
			} else if(std::holds_alternative<int64_t>(sub)) {
				return std::to_string(std::get<int64_t>(sub));
			} else if(std::holds_alternative<float>(sub)) {
				return std::to_string(std::get<float>(sub));
			} else if(std::holds_alternative<sys::date>(sub)) {
				return date_to_string(state, std::get<sys::date>(sub));
			} else {
				return std::string("?");
			}
		};

		auto seq = state.text_sequences[source_text];
		for(size_t i = seq.starting_component; i < size_t(seq.starting_component + seq.component_count); ++i) {
			if(std::holds_alternative<dcon::text_key>(state.text_components[i])) {
				auto tkey = std::get<dcon::text_key>(state.text_components[i]);
				std::string_view text = state.to_string_view(tkey);
				add_to_layout_box(dest, state, box, std::string_view(text), current_color, std::monostate{});
			} else if(std::holds_alternative<text::line_break>(state.text_components[i])) {
				box.x_position = float(box.x_offset);
				box.y_position += line_height;
			} else if(std::holds_alternative<text::text_color>(state.text_components[i])) {
				current_color = std::get<text::text_color>(state.text_components[i]);
			} else if(std::holds_alternative<text::variable_type>(state.text_components[i])) {
				auto var_type = std::get<text::variable_type>(state.text_components[i]);
				if(auto it = mp.find(uint32_t(var_type)); it != mp.end()) {
					auto txt = resolve_substitution(it->second);
					add_to_layout_box(dest, state, box, std::string_view(txt), current_color, it->second);
				} else {
					add_to_layout_box(dest, state, box, std::string_view("???"), current_color, std::monostate{});
				}
			}
		}
	}

	layout_box open_layout_box(columnar_layout& dest, int32_t indent) {
		return layout_box{ dest.base_layout.contents.size(), indent, 0, 0, float(indent + dest.fixed_parameters.left), 0, dest.fixed_parameters.color };
	}
	void close_layout_box(columnar_layout& dest, layout_box const& box) {
		if(box.y_size + dest.y_cursor >= dest.fixed_parameters.bottom) { // make new column
			++dest.current_column;
			for(auto i = box.first_chunk; i < dest.base_layout.contents.size(); ++i) {
				dest.base_layout.contents[i].y += dest.fixed_parameters.top;
				dest.base_layout.contents[i].x += float(dest.column_width * dest.current_column);
				dest.used_width = std::max(dest.used_width, int32_t(dest.base_layout.contents[i].x + dest.base_layout.contents[i].width));
			}
			dest.y_cursor = box.y_size + dest.fixed_parameters.top;
		} else { // append to current column
			for(auto i = box.first_chunk; i < dest.base_layout.contents.size(); ++i) {
				dest.base_layout.contents[i].y += int16_t(dest.y_cursor);
				dest.base_layout.contents[i].x += float(dest.column_width * dest.current_column);
				dest.used_width = std::max(dest.used_width, int32_t(dest.base_layout.contents[i].x + dest.base_layout.contents[i].width));
			}
			dest.y_cursor += box.y_size;
		}
		dest.used_height = std::max(dest.used_height, dest.y_cursor);
		dest.used_width = std::max(dest.used_width, box.x_size + dest.column_width * dest.current_column);
	}
	columnar_layout create_columnar_layout(layout& dest, layout_parameters const& params, int32_t column_width) {
		dest.contents.clear();
		dest.number_of_lines = 0;
		return columnar_layout{ dest, params, 0, 0, params.top, 0, column_width };
	}

}
