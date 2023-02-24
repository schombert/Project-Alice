#include "triggers.hpp"
#include "system_state.hpp"
#include "ve_scalar_extensions.hpp"

namespace trigger {



#ifdef WIN32
#define CALLTYPE __vectorcall
#else
#define CALLTYPE
#endif

template<typename A, typename B>
[[nodiscard]] auto compare_values(uint16_t trigger_code, A value_a, B value_b) {
	switch(trigger_code & trigger::association_mask) {
		case trigger::association_eq:
			return value_a == value_b;
		case trigger::association_gt:
			return value_a > value_b;
		case trigger::association_lt:
			return value_a < value_b;
		case trigger::association_le:
			return value_a <= value_b;
		case trigger::association_ne:
			return value_a != value_b;
		case trigger::association_ge:
			return value_a >= value_b;
		default:
			return value_a >= value_b;
	}
}

template<typename A, typename B>
[[nodiscard]] auto compare_values_eq(uint16_t trigger_code, A value_a, B value_b) {
	switch(trigger_code & trigger::association_mask) {
		case trigger::association_eq:
			return value_a == value_b;
		case trigger::association_gt:
			return value_a != value_b;
		case trigger::association_lt:
			return value_a != value_b;
		case trigger::association_le:
			return value_a == value_b;
		case trigger::association_ne:
			return value_a != value_b;
		case trigger::association_ge:
			return value_a == value_b;
		default:
			return value_a == value_b;
	}
}

template<typename A>
[[nodiscard]] auto compare_to_true(uint16_t trigger_code, A value_a) -> decltype(!value_a) {
	switch(trigger_code & trigger::association_mask) {
		case trigger::association_eq:
			return value_a;
		case trigger::association_gt:
			return !value_a;
		case trigger::association_lt:
			return !value_a;
		case trigger::association_le:
			return value_a;
		case trigger::association_ne:
			return !value_a;
		case trigger::association_ge:
			return value_a;
		default:
			return value_a;
	}
}
template<typename A>
[[nodiscard]] auto compare_to_false(uint16_t trigger_code, A value_a) -> decltype(!value_a) {
	switch(trigger_code & trigger::association_mask) {
		case trigger::association_eq:
			return !value_a;
		case trigger::association_gt:
			return value_a;
		case trigger::association_lt:
			return value_a;
		case trigger::association_le:
			return !value_a;
		case trigger::association_ne:
			return value_a;
		case trigger::association_ge:
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
struct full_mask { };

template<>
struct full_mask<bool> {
	static constexpr bool value = true;
};
template<>
struct full_mask<ve::vbitfield_type> {
	static constexpr ve::vbitfield_type value = ve::vbitfield_type{ ve::vbitfield_type::storage(-1) };
};

template<typename T>
struct empty_mask { };

template<>
struct empty_mask<bool> {
	static constexpr bool value = false;
};
template<>
struct empty_mask<ve::vbitfield_type> {
	static constexpr ve::vbitfield_type value = ve::vbitfield_type{ ve::vbitfield_type::storage(0) };
};

bool compare(bool a, bool b) {
	return a == b;
}
bool compare(ve::vbitfield_type a, ve::vbitfield_type b) {
	return a.v == b.v;
}

TRIGGER_FUNCTION(apply_disjuctively) {
	const auto source_size = 1 + get_trigger_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + trigger_scope_data_payload(tval[0]);

	return_type result = return_type(false);
	while(sub_units_start < tval + source_size) {
		result = result | test_trigger_generic<return_type, primary_type, this_type, from_type>(sub_units_start, ws, primary_slot, this_slot, from_slot);

		auto compressed_res = ve::compress_mask(result);
		if(compare(compressed_res, full_mask<decltype(compressed_res)>::value))
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
		result = result & test_trigger_generic<return_type, primary_type, this_type, from_type>(sub_units_start, ws, primary_slot, this_slot, from_slot);

		auto compressed_res = ve::compress_mask(result);
		if(compare(compressed_res, empty_mask<decltype(compressed_res)>::value))
			return result;

		sub_units_start += 1 + get_trigger_payload_size(sub_units_start);
	}
	return result;
}

TRIGGER_FUNCTION(apply_subtriggers) {
	if((*tval & trigger::is_disjunctive_scope) != 0)
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

int32_t to_generic(dcon::rebel_faction_id v) {
	return v.index();
}
ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::rebel_faction_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::rebel_faction_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::rebel_faction_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::rebel_faction_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}

dcon::rebel_faction_id to_rebel(int32_t v) {
	return dcon::rebel_faction_id(dcon::rebel_faction_id::value_base_t(v));
}
ve::tagged_vector<dcon::rebel_faction_id> to_rebel(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::rebel_faction_id>(v, std::true_type{});
}
ve::contiguous_tags<dcon::rebel_faction_id> to_rebel(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::rebel_faction_id>(v.value);
}
ve::unaligned_contiguous_tags<dcon::rebel_faction_id> to_rebel(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::rebel_faction_id>(v.value);
}
ve::partial_contiguous_tags<dcon::rebel_faction_id> to_rebel(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::rebel_faction_id>(v.value, v.subcount);
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
			value.set(index++, v);

			if(index == ve::vector_size) {
				result = (ve::compress_mask(F::operator()(value)).v & accumulated_mask) != 0;
				value = _mm256_setzero_si256();
				index = 0;
				accumulated_mask = 0;
			}
		}
	}
	void flush() {
		if(index != 0 && !result) {
			result = (ve::compress_mask(F::operator()(value)).v & accumulated_mask) != 0;
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
			value.set(index++, v);

			if(index == ve::vector_size) {
				result = (ve::compress_mask(F::operator()(value)).v & accumulated_mask) ==  accumulated_mask;
				value = _mm256_setzero_si256();
				index = 0;
				accumulated_mask = 0;
			}
		}
	}
	void flush() {
		if(index != 0 && result) {
			result = (ve::compress_mask(F::operator()(value)).v & accumulated_mask) == accumulated_mask;
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

auto existence_accumulator(sys::state& ws, uint16_t const* tval, int32_t this_slot, int32_t from_slot) {
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
	return ve::apply([&ws, tval](int32_t prov_id, int32_t t_slot, int32_t f_slot) {
		auto prov_tag = to_prov(prov_id);

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto adj : ws.world.province_get_province_adjacency(prov_tag)) {
				if((adj.get_type() & province::border::impassible_bit) == 0) {
					auto other = (prov_tag == adj.get_connected_provinces(0)) ? adj.get_connected_provinces(1).id : adj.get_connected_provinces(0).id;
					accumulator.add_value(to_generic(other));
				}
			}
			accumulator.flush();

			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto adj : ws.world.province_get_province_adjacency(prov_tag)) {
				if((adj.get_type() & province::border::impassible_bit) == 0) {
					auto other = (prov_tag == adj.get_connected_provinces(0)) ? adj.get_connected_provinces(1).id : adj.get_connected_provinces(0).id;
					accumulator.add_value(to_generic(other));
				}
			}
			accumulator.flush();

			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_nation) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
		auto nid = to_nation(p_slot);

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto adj : ws.world.nation_get_nation_adjacency(nid)) {
				auto iid = (nid == adj.get_connected_nations(0)) ? adj.get_connected_nations(1).id : adj.get_connected_nations(0).id;

				accumulator.add_value(to_generic(iid));
				if(accumulator.result)
					return true;
			}
			accumulator.flush();

			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);
			for(auto adj : ws.world.nation_get_nation_adjacency(nid)) {
				auto iid = (nid == adj.get_connected_nations(0)) ? adj.get_connected_nations(1).id : adj.get_connected_nations(0).id;

				accumulator.add_value(to_generic(iid));
				if(!accumulator.result)
					return false;
			}
			accumulator.flush();

			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto province_owner = ws.world.province_get_nation_from_province_ownership(location);

	return tf_x_neighbor_country_scope_nation<return_type>(tval, ws, to_generic(province_owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_nation) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
		auto nid = to_nation(p_slot);

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto wars_in : ws.world.nation_get_war_defender(nid)) {
				for(auto attackers : wars_in.get_war().get_war_attacker()) {
					accumulator.add_value(to_generic(attackers.get_nation().id));
					if(accumulator.result)
						return true;
				}
			}
			for(auto wars_in : ws.world.nation_get_war_attacker(nid)) {
				for(auto defenders : wars_in.get_war().get_war_defender()) {
					accumulator.add_value(to_generic(defenders.get_nation().id));
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto wars_in : ws.world.nation_get_war_defender(nid)) {
				for(auto attackers : wars_in.get_war().get_war_attacker()) {
					accumulator.add_value(to_generic(attackers.get_nation().id));
					if(!accumulator.result)
						return false;
				}
			}
			for(auto wars_in : ws.world.nation_get_war_attacker(nid)) {
				for(auto defenders : wars_in.get_war().get_war_defender()) {
					accumulator.add_value(to_generic(defenders.get_nation().id));
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto province_owner = ws.world.province_get_nation_from_province_ownership(location);

	return tf_x_war_countries_scope_nation<return_type>(tval, ws, to_generic(province_owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_greater_power_scope) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		uint32_t great_nations_count = uint32_t(ws.defines.great_nations_count);

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(uint32_t i = 0; i < ws.nations_by_rank.size() && i < great_nations_count; ++i) {
				accumulator.add_value(to_generic(ws.nations_by_rank[i]));
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(uint32_t i = 0; i < ws.nations_by_rank.size() && i < great_nations_count; ++i) {
				accumulator.add_value(to_generic(ws.nations_by_rank[i]));
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_state) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
		auto sid = to_state(p_slot);
		dcon::state_definition_fat_id state_def = fatten(ws.world, ws.world.state_instance_get_definition(sid));
		auto owner = ws.world.state_instance_get_nation_from_state_ownership(sid);

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : state_def.get_abstract_state_membership()) {
				if(p.get_province().get_nation_from_province_ownership() == owner) {
					accumulator.add_value(to_generic(p.get_province().id));
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : state_def.get_abstract_state_membership()) {
				if(p.get_province().get_nation_from_province_ownership() == owner) {
					accumulator.add_value(to_generic(p.get_province().id));
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_nation) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto nid = fatten(ws.world, to_nation(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : nid.get_province_ownership()) {
				accumulator.add_value(to_generic(p.get_province().id));
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : nid.get_province_ownership()) {
				accumulator.add_value(to_generic(p.get_province().id));
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_core_scope_province) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		dcon::province_fat_id pid = fatten(ws.world, to_prov(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : pid.get_core()) {
				auto holder = p.get_identity().get_nation_from_identity_holder();
				if(holder) {
					accumulator.add_value(to_generic(holder.id));
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : pid.get_core()) {
				auto holder = p.get_identity().get_nation_from_identity_holder();
				if(holder) {
					accumulator.add_value(to_generic(holder.id));
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_core_scope_nation) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto nid = fatten(ws.world, to_nation(p_slot));
		auto ident = nid.get_identity_holder_as_nation().get_identity();

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : ident.get_core()) {
				accumulator.add_value(to_generic(p.get_province().id));
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : ident.get_core()) {
				accumulator.add_value(to_generic(p.get_province().id));
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}

TRIGGER_FUNCTION(tf_x_state_scope) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto nid = fatten(ws.world, to_nation(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto s : nid.get_state_ownership()) {
				accumulator.add_value(to_generic(s.get_state().id));
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto s : nid.get_state_ownership()) {
				accumulator.add_value(to_generic(s.get_state().id));
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_substate_scope) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto nid = fatten(ws.world, to_nation(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto v : nid.get_overlord_as_ruler()) {
				if(v.get_is_substate()) {
					accumulator.add_value(to_generic(v.get_subject().id));
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto v : nid.get_overlord_as_ruler()) {
				if(v.get_is_substate()) {
					accumulator.add_value(to_generic(v.get_subject().id));
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_sphere_member_scope) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto nid = fatten(ws.world, to_nation(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto gpr : nid.get_gp_relationship_as_great_power()) {
				if((nations::influence::level_mask & gpr.get_status()) == nations::influence::level_in_sphere) {
					accumulator.add_value(to_generic(gpr.get_influence_target().id));
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto gpr : nid.get_gp_relationship_as_great_power()) {
				if((nations::influence::level_mask & gpr.get_status()) == nations::influence::level_in_sphere) {
					accumulator.add_value(to_generic(gpr.get_influence_target().id));
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_pop_scope_province) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		dcon::province_fat_id pid = fatten(ws.world, to_prov(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto i : pid.get_pop_location()) {
				accumulator.add_value(to_generic(i.get_pop().id));
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto i : pid.get_pop_location()) {
				accumulator.add_value(to_generic(i.get_pop().id));
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_pop_scope_state) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto sid = to_state(p_slot);
		dcon::state_definition_fat_id state_def = fatten(ws.world, ws.world.state_instance_get_definition(sid));
		auto owner = ws.world.state_instance_get_nation_from_state_ownership(sid);

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : state_def.get_abstract_state_membership()) {
				if(p.get_province().get_nation_from_province_ownership() == owner) {
					for(auto i : p.get_province().get_pop_location()) {
						accumulator.add_value(to_generic(i.get_pop().id));
						if(accumulator.result)
							return true;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : state_def.get_abstract_state_membership()) {
				if(p.get_province().get_nation_from_province_ownership() == owner) {
					for(auto i : p.get_province().get_pop_location()) {
						accumulator.add_value(to_generic(i.get_pop().id));
						if(!accumulator.result)
							return false;
					}
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_pop_scope_nation) {
	return ve::apply([&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {

		auto nid = fatten(ws.world, to_nation(p_slot));

		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : nid.get_province_ownership()) {
				for(auto i : p.get_province().get_pop_location()) {
					accumulator.add_value(to_generic(i.get_pop().id));
					if(accumulator.result)
						return true;
				}
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto p : nid.get_province_ownership()) {
				for(auto i : p.get_province().get_pop_location()) {
					accumulator.add_value(to_generic(i.get_pop().id));
					if(!accumulator.result)
						return false;
				}
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_provinces_in_variable_region) {
	auto state_def = trigger::payload(*(tval + 2)).state_id;

	return ve::apply([&ws, tval, state_def](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
		if(*tval & trigger::is_existence_scope) {
			auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

			for(auto i : ws.world.state_definition_get_abstract_state_membership(state_def)) {
				accumulator.add_value(to_generic(i.get_province().id));
				if(accumulator.result)
					return true;
			}

			accumulator.flush();
			return accumulator.result;
		} else {
			auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

			for(auto i : ws.world.state_definition_get_abstract_state_membership(state_def)) {
				accumulator.add_value(to_generic(i.get_province().id));
				if(!accumulator.result)
					return false;
			}

			accumulator.flush();
			return accumulator.result;
		}
	}, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_owner_scope_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_owner_scope_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_controller_scope) {
	auto controller = ws.world.province_get_nation_from_province_control(to_prov(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(controller), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_location_scope) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(location), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto owner = ws.world.province_get_nation_from_province_ownership(location);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_capital_scope) {
	auto cap = ws.world.nation_get_capital(to_nation(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(cap), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_this_scope) {
	return apply_subtriggers<return_type, this_type, this_type, from_type>(tval, ws, this_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_from_scope) {
	return apply_subtriggers<return_type, from_type, this_type, from_type>(tval, ws, from_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_sea_zone_scope) {
	auto sea_zones = ve::apply([&ws, tval](int32_t p_slot, int32_t, int32_t) {
		auto pid = fatten(ws.world, to_prov(p_slot));
		dcon::province_id sea_zone;
		for(auto adj : pid.get_province_adjacency()) {
			if(adj.get_connected_provinces(0).id.index() >= ws.province_definitions.first_sea_province.index()) {
				return adj.get_connected_provinces(0).id;
			} else if(adj.get_connected_provinces(1).id.index() >= ws.province_definitions.first_sea_province.index()) {
				return adj.get_connected_provinces(1).id;
			}
		}
		return dcon::province_id();
	}, primary_slot, this_slot, from_slot);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(sea_zones), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_cultural_union_scope) {
	auto cultures = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(group_holders), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_overlord_scope) {
	auto so = ws.world.nation_get_overlord_as_subject(to_nation(primary_slot));
	auto nso = ws.world.overlord_get_ruler(so);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(nso), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_sphere_owner_scope) {
	auto nso = ws.world.nation_get_in_sphere_of(to_nation(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(nso), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_independence_scope) {
	auto rtags = ws.world.rebel_faction_get_defection_target(to_rebel(from_slot));
	auto r_holders = ws.world.national_identity_get_nation_from_identity_holder(rtags);

	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(r_holders), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_flashpoint_tag_scope) {
	auto ctags = ws.world.state_instance_get_flashpoint_tag(to_state(primary_slot));
	auto fp_nations = ws.world.national_identity_get_nation_from_identity_holder(ctags);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(fp_nations), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_crisis_state_scope) {
	return apply_subtriggers<return_type, int32_t, this_type, from_type>(tval, ws, to_generic(ws.crisis_state), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_state_scope_province) {
	auto state_instance = ws.world.province_get_state_membership(to_prov(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(state_instance), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_state_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto state_instance = ws.world.province_get_state_membership(location);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(state_instance), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_tag_scope) {
	auto tag = trigger::payload(tval[2]).tag_id;
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(tag_holder), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_integer_scope) {
	auto wprov = trigger::payload(tval[2]).prov_id;
	return apply_subtriggers<return_type, int32_t, this_type, from_type>(tval, ws, to_generic(wprov), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_nation) {
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_cultural_union_scope_pop) {
	auto cultures = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);

	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, to_generic(group_holders), this_slot, from_slot);
}

template<typename return_type, typename primary_type, typename this_type, typename from_type>
struct scope_container {
	constexpr static return_type(CALLTYPE* scope_functions[])(uint16_t const*, sys::state&,
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
		tf_x_state_scope<return_type, primary_type, this_type, from_type>, //constexpr uint16_t x_state_scope = 0x000B;
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
		return scope_container<return_type, primary_type, this_type, from_type>::scope_functions[*tval & trigger::code_mask](tval, ws, primary_slot, this_slot, from_slot);
	} else {
		//return trigger_container<return_type, primary_type, this_type, from_type>::trigger_functions[*tval & trigger::code_mask](tval, ws, primary_slot, this_slot, from_slot);
	}
}

#undef CALLTYPE
#undef TRIGGER_FUNCTION

/**/

}
