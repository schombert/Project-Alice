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
					// maybe report an error here -- repreated definition
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
}