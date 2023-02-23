#include "triggers.hpp"
#include "system_state.hpp"

namespace trigger {

/*

#ifdef WIN32
#define CALLTYPE __vectorcall
#else
#define CALLTYPE
#endif

template<typename A, typename B>
[[nodiscard]] auto compare_values(uint16_t trigger_code, A value_a, B value_b) {
	switch(trigger_code & trigger_codes::association_mask) {
		case trigger_codes::association_eq:
			return value_a == value_b;
		case trigger_codes::association_gt:
			return value_a > value_b;
		case trigger_codes::association_lt:
			return value_a < value_b;
		case trigger_codes::association_le:
			return value_a <= value_b;
		case trigger_codes::association_ne:
			return value_a != value_b;
		case trigger_codes::association_ge:
			return value_a >= value_b;
		default:
			return value_a >= value_b;
	}
}

template<typename A, typename B>
[[nodiscard]] auto compare_values_eq(uint16_t trigger_code, A value_a, B value_b) {
	switch(trigger_code & trigger_codes::association_mask) {
		case trigger_codes::association_eq:
			return value_a == value_b;
		case trigger_codes::association_gt:
			return value_a != value_b;
		case trigger_codes::association_lt:
			return value_a != value_b;
		case trigger_codes::association_le:
			return value_a == value_b;
		case trigger_codes::association_ne:
			return value_a != value_b;
		case trigger_codes::association_ge:
			return value_a == value_b;
		default:
			return value_a == value_b;
	}
}

template<typename A>
[[nodiscard]] auto compare_to_true(uint16_t trigger_code, A value_a) -> decltype(!value_a) {
	switch(trigger_code & trigger_codes::association_mask) {
		case trigger_codes::association_eq:
			return value_a;
		case trigger_codes::association_gt:
			return !value_a;
		case trigger_codes::association_lt:
			return !value_a;
		case trigger_codes::association_le:
			return value_a;
		case trigger_codes::association_ne:
			return !value_a;
		case trigger_codes::association_ge:
			return value_a;
		default:
			return value_a;
	}
}
template<typename A>
[[nodiscard]] auto compare_to_false(uint16_t trigger_code, A value_a) -> decltype(!value_a) {
	switch(trigger_code & trigger_codes::association_mask) {
		case trigger_codes::association_eq:
			return !value_a;
		case trigger_codes::association_gt:
			return value_a;
		case trigger_codes::association_lt:
			return value_a;
		case trigger_codes::association_le:
			return !value_a;
		case trigger_codes::association_ne:
			return value_a;
		case trigger_codes::association_ge:
			return !value_a;
		default:
			return !value_a;
	}
}
float read_float_from_payload(const uint16_t* data) {
	union {
		struct {
			uint16_t low;
			uint16_t high;
		} v;
		float f;
	} pack_float;

	pack_float.v.low = data[0];
	pack_float.v.high = data[1];

	return pack_float.f;
}

template<typename return_type, typename primary_type, typename this_type, typename from_type>
return_type CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws, primary_type primary_slot, this_type this_slot, from_type from_slot);

#define TRIGGER_FUNCTION(function_name) template<typename return_type, typename primary_type, typename this_type, typename from_type> \
	return_type CALLTYPE function_name(uint16_t const* tval, sys::state& ws, primary_type primary_slot, \
	this_type this_slot, from_type from_slot)

template<typename T>
struct full_mask;

template<>
struct full_mask<bool> {
	static constexpr bool value = true;
};
template<>
struct full_mask<ve::vbitfield_type> {
	static constexpr ve::vbitfield_type value = ve::vbitfield_type{ ve::vbitfield_type::storage(-1) };
};

template<typename T>
struct empty_mask;

template<>
struct empty_mask<bool> {
	static constexpr bool value = false;
};
template<>
struct empty_mask<ve::vbitfield_type> {
	static constexpr ve::vbitfield_type value = ve::vbitfield_type{ ve::vbitfield_type::storage(0) };
};

TRIGGER_FUNCTION(apply_disjuctively) {
	const auto source_size = 1 + get_trigger_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + trigger_scope_data_payload(tval[0]);

	return_type result = return_type(false);
	while(sub_units_start < tval + source_size) {
		result = result | test_trigger_generic<primary_type, this_type, from_type>(sub_units_start, ws, primary_slot, this_slot, from_slot);

		auto compressed_res = ve::compress_mask(result);
		if(compressed_res == full_mask<return_type>::value)
			return result;

		sub_units_start += 1 + get_trigger_payload_size(sub_units_start);
	}
	return result;
}

TRIGGER_FUNCTION(apply_conjuctively) {
	const auto source_size = 1 + get_trigger_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + trigger_scope_data_payload(tval[0]);

	return_type result = return_type(false);
	while(sub_units_start < tval + source_size) {
		result = result & test_trigger_generic<primary_type, this_type, from_type>(sub_units_start, ws, primary_slot, this_slot, from_slot);

		auto compressed_res = ve::compress_mask(result);
		if(compressed_res == empty_mask<return_type>::value)
			return result;

		sub_units_start += 1 + get_trigger_payload_size(sub_units_start);
	}
	return result;
}

TRIGGER_FUNCTION(apply_subtriggers) {
	if((*tval & trigger_codes::is_disjunctive_scope) != 0)
		return apply_disjuctively<return_type, primary_type, this_type, from_type>(tval, ws, primary_slot, this_slot, from_slot);
	else
		return apply_conjuctively<return_type, primary_type, this_type, from_type>(tval, ws, primary_slot, this_slot, from_slot);
}

TRIGGER_FUNCTION(tf_none) {
	return return_type(true);
}

TRIGGER_FUNCTION(tf_generic_scope) {
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, primary_slot, this_slot, from_slot);
}


int32_t to_generic(dcon::province_id v) {
	return v.index();
}
ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::province_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::province_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::province_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::province_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}

dcon::province_id to_prov(int32_t v) {
	return dcon::province_id(dcon::province_id::value_base_t(v));
}
ve::tagged_vector<dcon::province_id> to_prov(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::province_id>(v, std::true_type{});
}
ve::contiguous_tags<dcon::province_id> to_prov(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::province_id>(v.value);
}
ve::unaligned_contiguous_tags<dcon::province_id> to_prov(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::province_id>(v.value);
}
ve::partial_contiguous_tags<dcon::province_id> to_prov(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::province_id>(v.value, v.subcount);
}

int32_t to_generic(dcon::nation_id v) {
	return v.index();
}
ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::nation_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::nation_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::nation_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::nation_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}

dcon::nation_id to_nation(int32_t v) {
	return dcon::nation_id(dcon::nation_id::value_base_t(v));
}
ve::tagged_vector<dcon::nation_id> to_nation(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::nation_id>(v, std::true_type{});
}
ve::contiguous_tags<dcon::nation_id> to_nation(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::nation_id>(v.value);
}
ve::unaligned_contiguous_tags<dcon::nation_id> to_nation(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::nation_id>(v.value);
}
ve::partial_contiguous_tags<dcon::nation_id> to_nation(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::nation_id>(v.value, v.subcount);
}

int32_t to_generic(dcon::pop_id v) {
	return v.index();
}
ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::pop_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::pop_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::pop_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::pop_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}

dcon::pop_id to_pop(int32_t v) {
	return dcon::pop_id(dcon::pop_id::value_base_t(v));
}
ve::tagged_vector<dcon::pop_id> to_pop(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::pop_id>(v, std::true_type{});
}
ve::contiguous_tags<dcon::pop_id> to_pop(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::pop_id>(v.value);
}
ve::unaligned_contiguous_tags<dcon::pop_id> to_pop(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::pop_id>(v.value);
}
ve::partial_contiguous_tags<dcon::pop_id> to_pop(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::pop_id>(v.value, v.subcount);
}

int32_t to_generic(dcon::state_instance_id v) {
	return v.index();
}
ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::state_instance_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::state_instance_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::state_instance_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::state_instance_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}

dcon::state_instance_id to_state(int32_t v) {
	return dcon::state_instance_id(dcon::state_instance_id::value_base_t(v));
}
ve::tagged_vector<dcon::state_instance_id> to_state(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::state_instance_id>(v, std::true_type{});
}
ve::contiguous_tags<dcon::state_instance_id> to_state(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::state_instance_id>(v.value);
}
ve::unaligned_contiguous_tags<dcon::state_instance_id> to_state(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::state_instance_id>(v.value);
}
ve::partial_contiguous_tags<dcon::state_instance_id> to_state(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::state_instance_id>(v.value, v.subcount);
}

template<typename F>
class true_accumulator : public F {
private:
	ve::tagged_vector<int32_t> value;
	uint32_t index = 0;
	int32_t accumulated_mask = 0;
public:
	bool result = false;

	true_accumulator(F&& f) : F(std::move(f)) { }

	void add_value(int32_t v) {
		if(!result) {
			accumulated_mask |= (int32_t(v != -1) << index);
			value[index++] = v;

			if(index == ve::vector_size) {
				result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) != 0;
				value = _mm256_setzero_si256();
				index = 0;
				accumulated_mask = 0;
			}
		}
	}
	void flush() {
		if(index != 0 && !result) {
			result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) != 0;
			index = 0;
		}
	}
};

template<typename F>
class false_accumulator : public F {
private:
	ve::tagged_vector<int32_t> value;
	uint32_t index = 0;
	int32_t accumulated_mask = 0;
public:
	bool result = true;

	false_accumulator(F&& f) : F(std::move(f)) { }

	void add_value(int32_t v) {
		if(result) {
			accumulated_mask |= (int32_t(v != -1) << index);
			value[index++] = v;

			if(index == ve::vector_size) {
				result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) == accumulated_mask;
				value = _mm256_setzero_si256();
				index = 0;
				accumulated_mask = 0;
			}
		}
	}
	void flush() {
		if(index != 0 && result) {
			result = (ve::compress_mask(F::operator()(value)) & accumulated_mask) == accumulated_mask;
			index = 0;
		}
	}
};

template<typename TAG, typename F>
class value_accumulator : public F {
private:
	ve::fp_vector value;
	ve::tagged_vector<TAG> store;

	uint32_t index = 0;
	int32_t accumulated_mask = 0;
public:

	value_accumulator(F&& f) : F(std::move(f)) { }

	void add_value(TAG v) {
		accumulated_mask |= (int32_t(is_valid_index(v)) << index);
		store.set(index++, v);

		if(index == ve::vector_size) {
			value = value + ve::select(accumulated_mask, F::operator()(store), 0.0f);
			index = 0;
			accumulated_mask = 0;
		}

	}
	float flush() {
		if(index != 0) {
			value = value + ve::select(accumulated_mask, F::operator()(store), 0.0f);
			index = 0;
		}

		return value.reduce();
	}
};


template<typename F>
auto make_true_accumulator(F&& f) -> true_accumulator<F> {
	return true_accumulator<F>(std::forward<F>(f));
}

template<typename F>
auto make_false_accumulator(F&& f) -> false_accumulator<F> {
	return false_accumulator<F>(std::forward<F>(f));
}

template<typename TAG, typename F>
auto make_value_accumulator(F&& f) -> value_accumulator<TAG, F> {
	return value_accumulator<TAG, F>(std::forward<F>(f));
}

auto existance_accumulator(sys::state& ws, uint16_t const* tval, int32_t this_slot, int32_t from_slot) {
	return make_true_accumulator([&ws, tval, this_slot, from_slot](ve::tagged_vector<int32_t> v) {
		return apply_subtriggers<ve::mask_vector, ve::tagged_vector<int32_t>, int32_t, int32_t>(tval, ws, v, this_slot, from_slot);
	});
}

auto universal_accumulator(sys::state& ws, uint16_t const* tval, int32_t this_slot, int32_t from_slot) {
	return make_false_accumulator([&ws, tval, this_slot, from_slot](ve::tagged_vector<int32_t> v) {
		return apply_subtriggers<ve::mask_vector, ve::tagged_vector<int32_t>, int32_t, int32_t>(tval, ws, v, this_slot, from_slot);
	});
}

TRIGGER_FUNCTION(tf_x_neighbor_province_scope) {
	return ve::apply(primary_slot, this_slot, from_slot, [&ws, tval](int32_t prov_id, int32_t t_slot, int32_t f_slot) {
		auto prov_tag = to_prov(prov_id);

		auto p_same_range = ws.s.province_m.same_type_adjacency.get_range(prov_id);
		auto p_diff_range = ws.s.province_m.coastal_adjacency.get_range(prov_id);

		if(*tval & trigger::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto adj : ws.world.province_get_province_adjacency(prov_tag)) {
				if((adj.get_type() & province::border::impassible_bit) == 0) {
					auto other = (prov_tag == adj.get_connected_provinces(0)) ? adj.get_connected_provinces(1).id : adj.get_connected_provinces(0).id;
					accumulator.add_value(to_generic(id));
				}
			}
			accumulator.flush();

			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto adj : ws.world.province_get_province_adjacency(prov_tag)) {
				if((adj.get_type() & province::border::impassible_bit) == 0) {
					auto other = (prov_tag == adj.get_connected_provinces(0)) ? adj.get_connected_provinces(1).id : adj.get_connected_provinces(0).id;
					accumulator.add_value(to_generic(id));
				}
			}
			accumulator.flush();

			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_nation) {
	return ve::apply(primary_slot, this_slot, from_slot, [&ws, tval](const_parameter p_slot, const_parameter t_slot, const_parameter f_slot) {
		auto nid = to_nation(p_slot);

		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto n_range = get_range(ws.w.nation_s.nations_arrays, ws.w.nation_s.nations.get<nation::neighboring_nations>(nid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);
			for(auto n : n_range) {
				accumulator.add_value(n.value);
				if(accumulator.result)
					return true;
			}
			accumulator.flush();

			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);
			for(auto n : n_range) {
				accumulator.add_value(n.value);
				if(!accumulator.result)
					return false;
			}
			accumulator.flush();

			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_pop) {
	auto pop_province = to_value<pop::location>(ws.w.population_s.pops, to_pop(primary_slot));
	auto province_owner = to_value<province_state::owner>(ws.w.province_s.province_state_container, pop_province);

	return tf_x_neighbor_country_scope_nation<value_to_type<decltype(province_owner)>, this_type, from_type>(tval, ws, province_owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_nation) {
	return ve::apply(primary_slot, this_slot, from_slot, [&ws, tval](const_parameter p_slot, const_parameter t_slot, const_parameter f_slot) {
		auto nid = to_nation(p_slot);

		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto opposing_countries = get_range(ws.w.nation_s.nations_arrays, ws.w.nation_s.nations.get<nation::opponents_in_war>(nid));

		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto n : opposing_countries) {
				accumulator.add_value(n.value);
				if(accumulator.result)
					return true;
			}
			accumulator.flush();

			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto n : opposing_countries) {
				accumulator.add_value(n.value);
				if(!accumulator.result)
					return false;
			}
			accumulator.flush();

			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_pop) {
	auto pop_province = to_value<pop::location>(ws.w.population_s.pops, to_pop(primary_slot));
	auto province_owner = to_value<province_state::owner>(ws.w.province_s.province_state_container, pop_province);

	return tf_x_war_countries_scope_nation<value_to_type<decltype(province_owner)>, this_type, from_type>(tval, ws, province_owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_greater_power_scope) {
	return ve::apply(this_slot, from_slot, [&ws, tval](const_parameter t_slot, const_parameter f_slot) {
		auto ranked_range = get_range(ws.w.nation_s.nations_arrays, ws.w.nation_s.nations_by_rank);
		int32_t great_nations_count = int32_t(ws.s.modifiers_m.global_defines.great_nations_count);

		if(*tval & trigger_codes::is_existance_scope) {
			int32_t count = 0;
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto n = std::begin(ranked_range); (n != std::end(ranked_range)) & (count < great_nations_count); ++n) {
				if(is_valid_index(*n)) {
					if(nations::is_great_power(ws, *n)) {
						++count;

						accumulator.add_value(n->value);
						if(accumulator.result)
							return true;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			int32_t count = 0;
			for(auto n = std::begin(ranked_range); (n != std::end(ranked_range)) & (count < great_nations_count); ++n) {
				if(is_valid_index(*n)) {
					if(nations::is_great_power(ws, *n)) {
						++count;

						accumulator.add_value(n->value);
						if(!accumulator.result)
							return false;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_state) {
	return ve::apply(to_state(primary_slot), this_slot, from_slot, [&ws, tval](nations::state_tag sid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.states.is_valid_index(sid))
			return false;

		auto region_id = ws.w.nation_s.states.get<state::region_id>(sid);
		if(!is_valid_index(region_id))
			return false;

		auto region_provinces = ws.s.province_m.states_to_province_index.get_range(region_id);
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : region_provinces) {
				if(ws.w.province_s.province_state_container.get<province_state::state_instance>(p) == sid) {
					accumulator.add_value(p.value);
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : region_provinces) {
				if(ws.w.province_s.province_state_container.get<province_state::state_instance>(p) == sid) {
					accumulator.add_value(p.value);
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_nation) {
	return ve::apply(to_nation(primary_slot), this_slot, from_slot, [&ws, tval](nations::country_tag nid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto owned_range = get_range(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(nid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : owned_range) {
				accumulator.add_value(p.value);
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : owned_range) {
				accumulator.add_value(p.value);
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_core_scope_province) {
	return ve::apply(to_prov(primary_slot), this_slot, from_slot, [&ws, tval](provinces::province_tag pid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.province_s.province_state_container.is_valid_index(pid))
			return false;

		auto core_range = get_range(ws.w.province_s.core_arrays, ws.w.province_s.province_state_container.get<province_state::cores>(pid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : core_range) {
				if(is_valid_index(p)) {
					auto core_holder = ws.w.culture_s.tags_to_holders[p];
					accumulator.add_value(core_holder.value);
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : core_range) {
				if(is_valid_index(p)) {
					auto core_holder = ws.w.culture_s.tags_to_holders[p];
					accumulator.add_value(core_holder.value);
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_core_scope_nation) {
	return ve::apply(to_nation(primary_slot), this_slot, from_slot, [&ws, tval](nations::country_tag nid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto tag = ws.w.nation_s.nations.get<nation::tag>(nid);
		if(!is_valid_index(tag))
			return false;

		auto core_range = get_range(ws.w.province_s.province_arrays, ws.w.culture_s.national_tags_state[tag].core_provinces);
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : core_range) {
				accumulator.add_value(p.value);
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : core_range) {
				accumulator.add_value(p.value);
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}

TRIGGER_FUNCTION(tf_x_state_scope) {
	return ve::apply(to_nation(primary_slot), this_slot, from_slot, [&ws, tval](nations::country_tag nid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto states = get_range(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(nid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto s = states.first; s != states.second; ++s) {
				accumulator.add_value(s->state.value);
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto s = states.first; s != states.second; ++s) {
				accumulator.add_value(s->state.value);
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_substate_scope) {
	return ve::apply(to_nation(primary_slot), this_slot, from_slot, [&ws, tval](nations::country_tag nid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto nation_range = get_range(ws.w.nation_s.nations_arrays, ws.w.nation_s.nations.get<nation::vassals>(nid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto i = nation_range.first; i != nation_range.second; ++i) {
				if(ws.w.nation_s.nations.get<nation::is_substate>(*i)) {
					accumulator.add_value(i->value);
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto i = nation_range.first; i != nation_range.second; ++i) {
				if(ws.w.nation_s.nations.get<nation::is_substate>(*i)) {
					accumulator.add_value(i->value);
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_sphere_member_scope) {
	return ve::apply(to_nation(primary_slot), this_slot, from_slot, [&ws, tval](nations::country_tag nid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto nation_range = get_range(ws.w.nation_s.nations_arrays, ws.w.nation_s.nations.get<nation::sphere_members>(nid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto i = nation_range.first; i != nation_range.second; ++i) {
				accumulator.add_value(i->value);
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto i = nation_range.first; i != nation_range.second; ++i) {
				accumulator.add_value(i->value);
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_pop_scope_province) {
	return ve::apply(primary_slot, this_slot, from_slot, [&ws, tval](const_parameter p_slot, const_parameter t_slot, const_parameter f_slot) {
		auto pid = to_prov(p_slot);
		if(!ws.w.province_s.province_state_container.is_valid_index(pid))
			return false;

		auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(pid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto i : pop_range) {
				accumulator.add_value(i.value);
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto i : pop_range) {
				accumulator.add_value(i.value);
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_pop_scope_state) {
	return ve::apply(to_state(primary_slot), this_slot, from_slot, [&ws, tval](nations::state_tag sid, const_parameter t_slot, const_parameter f_slot) {
		if(!ws.w.nation_s.states.is_valid_index(sid))
			return false;

		auto region_id = ws.w.nation_s.states.get<state::region_id>(sid);
		if(!is_valid_index(region_id))
			return false;

		auto province_range = ws.s.province_m.states_to_province_index.get_range(region_id);
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto j = province_range.first; j != province_range.second; ++j) {
				if(ws.w.province_s.province_state_container.get<province_state::state_instance>(*j) == sid) {
					auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(*j));
					for(auto i : pop_range) {
						accumulator.add_value(i.value);
						if(accumulator.result)
							return true;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto j = province_range.first; j != province_range.second; ++j) {
				if(ws.w.province_s.province_state_container.get<province_state::state_instance>(*j) == sid) {
					auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(*j));
					for(auto i : pop_range) {
						accumulator.add_value(i.value);
						if(!accumulator.result)
							return false;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_pop_scope_nation) {
	return ve::apply(primary_slot, this_slot, from_slot, [&ws, tval](const_parameter p_slot, const_parameter t_slot, const_parameter f_slot) {
		auto nid = to_nation(p_slot);
		if(!ws.w.nation_s.nations.is_valid_index(nid))
			return false;

		auto province_range = get_range(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(nid));
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto j = province_range.first; j != province_range.second; ++j) {
				if(is_valid_index(*j)) {
					auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(*j));
					for(auto i : pop_range) {
						accumulator.add_value(i.value);
						if(accumulator.result)
							return true;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto j = province_range.first; j != province_range.second; ++j) {
				if(is_valid_index(*j)) {
					auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(*j));
					for(auto i : pop_range) {
						accumulator.add_value(i.value);
						if(!accumulator.result)
							return false;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_x_provinces_in_variable_region) {
	auto region = trigger_payload(*(tval + 2)).state;
	auto provinces = ws.s.province_m.states_to_province_index.get_range(region);

	return ve::apply(this_slot, from_slot, [&ws, tval, provinces](const_parameter t_slot, const_parameter f_slot) {
		if(*tval & trigger_codes::is_existance_scope) {
			auto accumulator = existance_accumulator(ws, tval, t_slot, f_slot);

			for(auto i = provinces.first; i != provinces.second; ++i) {
				accumulator.add_value(i->value);
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto i = provinces.first; i != provinces.second; ++i) {
				accumulator.add_value(i->value);
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	});
}
TRIGGER_FUNCTION(tf_owner_scope_state) {
	auto owner = to_value<state::owner>(ws.w.nation_s.states, to_state(primary_slot));
	return is_valid_index(owner) & apply_subtriggers<value_to_type<decltype(owner)>, this_type, from_type>(tval, ws, owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_owner_scope_province) {
	auto owner = to_value<province_state::owner>(ws.w.province_s.province_state_container, to_prov(primary_slot));
	return is_valid_index(owner) & apply_subtriggers<value_to_type<decltype(owner)>, this_type, from_type>(tval, ws, owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_controller_scope) {
	auto owner = to_value<province_state::controller>(ws.w.province_s.province_state_container, to_prov(primary_slot));
	return is_valid_index(owner) & apply_subtriggers<value_to_type<decltype(owner)>, this_type, from_type>(tval, ws, owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_location_scope) {
	auto location = to_value<pop::location>(ws.w.population_s.pops, to_pop(primary_slot));
	return is_valid_index(location) & apply_subtriggers<value_to_type<decltype(location)>, this_type, from_type>(tval, ws, location, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_state) {
	auto owner = to_value<state::owner>(ws.w.nation_s.states, to_state(primary_slot));
	return is_valid_index(owner) & apply_subtriggers<value_to_type<decltype(owner)>, this_type, from_type>(tval, ws, owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_pop) {
	auto location = to_value<pop::location>(ws.w.population_s.pops, to_pop(primary_slot));
	auto owner = to_value<province_state::owner>(ws.w.province_s.province_state_container, location);
	return is_valid_index(owner) & apply_subtriggers<value_to_type<decltype(owner)>, this_type, from_type>(tval, ws, owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_capital_scope) {
	auto capital = to_value<nation::current_capital>(ws.w.nation_s.nations, to_nation(primary_slot));
	return is_valid_index(capital) & apply_subtriggers<value_to_type<decltype(capital)>, this_type, from_type>(tval, ws, capital, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_this_scope) {
	return apply_subtriggers<this_type, this_type, from_type>(tval, ws, this_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_from_scope) {
	return apply_subtriggers<from_type, this_type, from_type>(tval, ws, from_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_sea_zone_scope) {
	auto sea_zones = ve::apply(to_prov(primary_slot), [&ws, tval](provinces::province_tag pid) {
		if(!ws.w.province_s.province_state_container.is_valid_index(pid))
			return provinces::province_tag();
		auto sea_zones = ws.s.province_m.coastal_adjacency.get_range(pid);
		if(sea_zones.first != sea_zones.second)
			return *(sea_zones.first);
		return provinces::province_tag();
	});
	return is_valid_index(sea_zones) & apply_subtriggers<value_to_type<decltype(sea_zones)>, this_type, from_type>(tval, ws, sea_zones, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_cultural_union_scope) {
	auto cultures = to_value<nation::primary_culture>(ws.w.nation_s.nations, to_nation(primary_slot));
	auto union_tags = ve::load(cultures, ws.s.culture_m.cultures_to_tags.view());
	auto group_holders = ve::load(union_tags, ws.w.culture_s.tags_to_holders.view());
	return is_valid_index(group_holders) & apply_subtriggers<value_to_type<decltype(group_holders)>, this_type, from_type>(tval, ws, group_holders, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_overlord_scope) {
	auto so = to_value<nation::overlord>(ws.w.nation_s.nations, to_nation(primary_slot));
	return is_valid_index(so) & apply_subtriggers<value_to_type<decltype(so)>, this_type, from_type>(tval, ws, so, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_sphere_owner_scope) {
	auto so = to_value<nation::sphere_leader>(ws.w.nation_s.nations, to_nation(primary_slot));
	return is_valid_index(so) & apply_subtriggers<value_to_type<decltype(so)>, this_type, from_type>(tval, ws, so, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_independence_scope) {
	auto rtags = ve::apply(to_rebel(from_slot), [](population::rebel_faction_tag r) { return population::rebel_faction_tag_to_national_tag(r); });
	auto inations = ve::load(rtags, ws.w.culture_s.tags_to_holders.view());
	return is_valid_index(inations) & apply_subtriggers<value_to_type<decltype(inations)>, this_type, from_type>(tval, ws, inations, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_flashpoint_tag_scope) {
	auto ctags = to_value<state::crisis_tag>(ws.w.nation_s.states, to_state(from_slot));
	auto fp_nations = ve::load(ctags, ws.w.culture_s.tags_to_holders.view());
	return is_valid_index(fp_nations) & apply_subtriggers<value_to_type<decltype(fp_nations)>, this_type, from_type>(tval, ws, fp_nations, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_crisis_state_scope) {
	auto cstate = ve::widen_to<decltype(primary_slot)>(ws.w.current_crisis.state);
	if(is_valid_index(ws.w.current_crisis.state))
		return apply_subtriggers<value_to_type<decltype(cstate)>, this_type, from_type>(tval, ws, cstate, this_slot, from_slot);
	return false;
}
TRIGGER_FUNCTION(tf_state_scope_province) {
	auto state_instance = to_value<province_state::state_instance>(ws.w.province_s.province_state_container, to_prov(primary_slot));
	return is_valid_index(state_instance) & apply_subtriggers<value_to_type<decltype(state_instance)>, this_type, from_type>(tval, ws, state_instance, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_state_scope_pop) {
	auto location = to_value<pop::location>(ws.w.population_s.pops, to_pop(primary_slot));
	auto prov_state = to_value<province_state::state_instance>(ws.w.province_s.province_state_container, location);
	return is_valid_index(prov_state) & apply_subtriggers<value_to_type<decltype(prov_state)>, this_type, from_type>(tval, ws, prov_state, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_tag_scope) {
	auto tag = trigger_payload(tval[2]).tag;
	auto tag_holder = ws.w.culture_s.tags_to_holders[tag];
	if(tag_holder) {
		auto wtag_holder = ve::widen_to<decltype(primary_slot)>(tag_holder);
		return apply_subtriggers<value_to_type<decltype(wtag_holder)>, this_type, from_type>(tval, ws, wtag_holder, this_slot, from_slot);
	} else {
		return false;
	}
}
TRIGGER_FUNCTION(tf_integer_scope) {
	auto wprov = ve::widen_to<decltype(primary_slot)>(provinces::province_tag(tval[2]));
	return apply_subtriggers<value_to_type<decltype(wprov)>, this_type, from_type>(tval, ws, wprov, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_nation) {
	return apply_subtriggers<primary_type, this_type, from_type>(tval, ws, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_province) {
	auto owner = to_value<province_state::owner>(ws.w.province_s.province_state_container, to_prov(primary_slot));
	return is_valid_index(owner) & apply_subtriggers<value_to_type<decltype(owner)>, this_type, from_type>(tval, ws, owner, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_cultural_union_scope_pop) {
	auto cultures = to_value<pop::culture>(ws.w.population_s.pops, to_pop(primary_slot));
	auto union_tags = ve::load(cultures, ws.s.culture_m.cultures_to_tags.view());
	auto group_holders = ve::load(union_tags, ws.w.culture_s.tags_to_holders.view());

	return is_valid_index(group_holders) & apply_subtriggers<value_to_type<decltype(group_holders)>, this_type, from_type>(tval, ws, group_holders, this_slot, from_slot);
}

template<typename return_type, typename primary_type, typename this_type, typename from_type>
struct scope_container {
	constexpr static typename return_type(CALLTYPE* scope_functions[])(uint16_t const*, sys::state&,
		primary_type, this_type, from_type) = {
		tf_generic_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t generic_scope = 0x0000; // or & and
		tf_x_neighbor_province_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_neighbor_province_scope = 0x0001;
		tf_x_neighbor_country_scope_nation<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_neighbor_country_scope_nation = 0x0002;
		tf_x_neighbor_country_scope_pop<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_neighbor_country_scope_pop = 0x0003;
		tf_x_war_countries_scope_nation<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_war_countries_scope_nation = 0x0004;
		tf_x_war_countries_scope_pop<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_war_countries_scope_pop = 0x0005;
		tf_x_greater_power_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_greater_power_scope = 0x0006;
		tf_x_owned_province_scope_state<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_owned_province_scope_state = 0x0007;
		tf_x_owned_province_scope_nation<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_owned_province_scope_nation = 0x0008;
		tf_x_core_scope_province<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_core_scope_province = 0x0009;
		tf_x_core_scope_nation<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_core_scope_nation = 0x000A;
		tf_x_state_scope<primary_type, this_type, from_type>, //constexpr uint16_t x_state_scope = 0x000B;
		tf_x_substate_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_substate_scope = 0x000C;
		tf_x_sphere_member_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_sphere_member_scope = 0x000D;
		tf_x_pop_scope_province<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_pop_scope_province = 0x000E;
		tf_x_pop_scope_state<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_pop_scope_state = 0x000F;
		tf_x_pop_scope_nation<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_pop_scope_nation = 0x0010;
		tf_x_provinces_in_variable_region<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_provinces_in_variable_region = 0x0011; // variable name
		tf_owner_scope_state<return_type, primary_type, this_type, from_type>, //constexpr uint16_t owner_scope_state = 0x0012;
		tf_owner_scope_province<return_type, primary_type, this_type, from_type>, //constexpr uint16_t owner_scope_province = 0x0013;
		tf_controller_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t controller_scope = 0x0014;
		tf_location_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t location_scope = 0x0015;
		tf_country_scope_state<return_type, primary_type, this_type, from_type>, //constexpr uint16_t country_scope_state = 0x0016;
		tf_country_scope_pop<return_type, primary_type, this_type, from_type>, //constexpr uint16_t country_scope_pop = 0x0017;
		tf_capital_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t capital_scope = 0x0018;
		tf_this_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t this_scope_pop = 0x0019;
		tf_this_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t this_scope_nation = 0x001A;
		tf_this_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t this_scope_state = 0x001B;
		tf_this_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t this_scope_province = 0x001C;
		tf_from_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t from_scope_pop = 0x001D;
		tf_from_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t from_scope_nation = 0x001E;
		tf_from_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t from_scope_state = 0x001F;
		tf_from_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t from_scope_province = 0x0020;
		tf_sea_zone_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t sea_zone_scope = 0x0021;
		tf_cultural_union_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t cultural_union_scope = 0x0022;
		tf_overlord_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t overlord_scope = 0x0023;
		tf_sphere_owner_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t sphere_owner_scope = 0x0024;
		tf_independence_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t independence_scope = 0x0025;
		tf_flashpoint_tag_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t flashpoint_tag_scope = 0x0026;
		tf_crisis_state_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t crisis_state_scope = 0x0027;
		tf_state_scope_pop<return_type, primary_type, this_type, from_type>, //constexpr uint16_t state_scope_pop = 0x0028;
		tf_state_scope_province<return_type, primary_type, this_type, from_type>, //constexpr uint16_t state_scope_province = 0x0029;
		tf_tag_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t tag_scope = 0x002A; // variable name
		tf_integer_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t integer_scope = 0x002B; // variable name
		tf_country_scope_nation<return_type, primary_type, this_type, from_type>, //constexpr uint16_t country_scope_nation = 0x002C;
		tf_country_scope_province<return_type, primary_type, this_type, from_type>, //constexpr uint16_t country_scope_province = 0x002D;
		tf_cultural_union_scope_pop<return_type, primary_type, this_type, from_type>, //constexpr uint16_t cultural_union_scope_pop = 0x002E;
	};
};

template<typename return_type, typename primary_type, typename this_type, typename from_type>
return_type CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws, primary_type primary_slot, this_type this_slot, from_type from_slot) {
	if((*tval & trigger::is_scope) != 0) {
		return scope_container<primary_type, this_type, from_type>::scope_functions[*tval & trigger::code_mask](tval, ws, primary_slot, this_slot, from_slot);
	} else {
		//return trigger_container<primary_type, this_type, from_type>::trigger_functions[*tval & trigger::code_mask](tval, ws, primary_slot, this_slot, from_slot);
	}
}

#undef CALLTYPE
#undef TRIGGER_FUNCTION

/**/

}
