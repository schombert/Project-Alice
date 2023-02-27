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
				value = ve::tagged_vector<int32_t>();
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
				value = ve::tagged_vector<int32_t>();
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
	auto sea_zones = ve::apply([&ws](int32_t p_slot, int32_t, int32_t) {
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


TRIGGER_FUNCTION(tf_year) {
	return compare_values(tval[0], ws.current_date.to_ymd(ws.start_date).year, int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_month) {
	return compare_values(tval[0], ws.current_date.to_ymd(ws.start_date).month, int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_port) {
	return compare_to_true(tval[0],
		 ws.world.province_get_is_coast(to_prov(primary_slot))
	);
}
TRIGGER_FUNCTION(tf_rank) {
	// note: comparison reversed since rank 1 is "greater" than rank 1 + N
	return compare_values(tval[0], int32_t(tval[1]), ws.world.nation_get_rank(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_technology) {
	auto tid = trigger::payload(tval[1]).tech_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_technologies(to_nation(primary_slot), tid));
}
TRIGGER_FUNCTION(tf_strata_rich) {
	auto type = ws.world.pop_get_poptype(to_pop(primary_slot));
	auto strata = ws.world.pop_type_get_strata(type);
	return compare_to_true(tval[0], strata == decltype(strata)(int32_t(culture::pop_strata::rich)));
}
TRIGGER_FUNCTION(tf_strata_middle) {
	auto type = ws.world.pop_get_poptype(to_pop(primary_slot));
	auto strata = ws.world.pop_type_get_strata(type);
	return compare_to_true(tval[0], strata == decltype(strata)(int32_t(culture::pop_strata::middle)));
}
TRIGGER_FUNCTION(tf_strata_poor) {
	auto type = ws.world.pop_get_poptype(to_pop(primary_slot));
	auto strata = ws.world.pop_type_get_strata(type);
	return compare_to_true(tval[0], strata == decltype(strata)(int32_t(culture::pop_strata::poor)));
}
TRIGGER_FUNCTION(tf_life_rating_province) {
	return compare_values(tval[0], ws.world.province_get_life_rating(to_prov(primary_slot)), trigger::payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_life_rating_state) {
	auto state_caps = ws.world.state_instance_get_capital(to_state(primary_slot));
	return compare_values(tval[0], ws.world.province_get_life_rating(state_caps), trigger::payload(tval[1]).signed_value);
}

auto empty_province_accumulator(sys::state const& ws) {
	return make_true_accumulator([&ws, sea_index = ws.province_definitions.first_sea_province.index()](ve::tagged_vector<int32_t> v) {
		auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(v));
		return (owners != ve::tagged_vector<dcon::nation_id>()) & (ve::int_vector(v) < sea_index);
	});
}

TRIGGER_FUNCTION(tf_has_empty_adjacent_state_province) {
	auto results = ve::apply([&ws](int32_t p_slot, int32_t, int32_t) {
		auto pid = to_prov(p_slot);
		auto acc = empty_province_accumulator(ws);

		for(auto p : ws.world.province_get_province_adjacency(pid)) {
			auto other = p.get_connected_provinces(0) == pid ? p.get_connected_provinces(1) : p.get_connected_provinces(0);
			acc.add_value(to_generic(other));
			if(acc.result)
				return true;
		}

		acc.flush();
		return acc.result;
	}, primary_slot, this_slot, from_slot);

	return compare_to_true(tval[0], results);
}
auto empty_province_from_state_accumulator(sys::state const& ws, dcon::state_instance_id sid) {
	return make_true_accumulator([&ws,
		vector_sid = ve::tagged_vector<dcon::state_instance_id>(sid),
		sea_index = ws.province_definitions.first_sea_province.index()](ve::tagged_vector<int32_t> v) {

		auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(v));
		return (owners != ve::tagged_vector<dcon::nation_id>()) & (ve::int_vector(v) < sea_index) & (ws.world.province_get_state_membership(to_prov(v)) != vector_sid);
	});
}

TRIGGER_FUNCTION(tf_has_empty_adjacent_state_state) {
	auto results = ve::apply([&ws](int32_t p_slot, int32_t, int32_t) {
		auto state_id = to_state(p_slot);
		auto region_owner = ws.world.state_instance_get_nation_from_state_ownership(state_id);

		auto acc = empty_province_from_state_accumulator(ws, state_id);
		auto region_id = ws.world.state_instance_get_definition(state_id);
		
		for(auto sp : ws.world.state_definition_get_abstract_state_membership(region_id)) {
			if(sp.get_province().get_nation_from_province_ownership() == region_owner) {
				for(auto p : ws.world.province_get_province_adjacency(sp.get_province())) {
					auto other = p.get_connected_provinces(0) == sp.get_province() ? p.get_connected_provinces(1) : p.get_connected_provinces(0);
					acc.add_value(to_generic(other));
					if(acc.result)
						return true;
				}
			}
		}

		acc.flush();
		return acc.result;
	}, primary_slot, this_slot, from_slot);

	return compare_to_true(tval[0], results);
}
TRIGGER_FUNCTION(tf_state_id_province) {
	auto pid = trigger::payload(tval[1]).prov_id;
	return compare_values_eq(tval[0], 
		ws.world.abstract_state_membership_get_state(ws.world.province_get_abstract_state_membership(to_prov(primary_slot))),
		ws.world.abstract_state_membership_get_state(ws.world.province_get_abstract_state_membership(pid)));
}
TRIGGER_FUNCTION(tf_state_id_state) {
	auto pid = trigger::payload(tval[1]).prov_id;
	return compare_values_eq(tval[0],
		ws.world.state_instance_get_definition(to_state(primary_slot)),
		ws.world.abstract_state_membership_get_state(ws.world.province_get_abstract_state_membership(pid)));
}
TRIGGER_FUNCTION(tf_cash_reserves) {
	auto ratio = read_float_from_payload(tval + 1);
	auto target = economy::desired_needs_spending(ws, to_pop(primary_slot));
	auto savings_qnty = ws.world.pop_get_savings(to_pop(primary_slot));
	return compare_values(tval[0], ve::select(target != 0.0f, savings_qnty * 100.0f / target, 100.0f), ratio);
}
TRIGGER_FUNCTION(tf_unemployment_nation) {
	auto total_employable = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::employable);
	auto total_employed = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::employed);
	return compare_values(tval[0],
		ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_state) {
	auto total_employable = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::employable);
	auto total_employed = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::employed);
	return compare_values(tval[0],
		ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_province) {
	auto total_employable = ws.world.province_get_demographics(to_prov(primary_slot), demographics::employable);
	auto total_employed = ws.world.province_get_demographics(to_prov(primary_slot), demographics::employed);
	return compare_values(tval[0],
		ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_pop) {
	auto employed = ws.world.pop_get_employment(to_pop(primary_slot));
	auto total_pop = ws.world.pop_get_size(to_pop(primary_slot));
	auto ptype = ws.world.pop_get_poptype(to_pop(primary_slot));
	return compare_values(tval[0],
		ve::select(ws.world.pop_type_get_has_unemployment(ptype), 1.0f - (employed / total_pop), 0.0f),
		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_slave_nation) {
	return compare_to_true(tval[0],
		(ws.world.nation_get_combined_issue_rules(to_nation(primary_slot)) & issue_rule::slavery_allowed) == issue_rule::slavery_allowed
	);
}
TRIGGER_FUNCTION(tf_is_slave_state) {
	return compare_to_true(tval[0], ws.world.province_get_is_slave(ws.world.state_instance_get_capital(to_state(primary_slot))));
}
TRIGGER_FUNCTION(tf_is_slave_province) {
	return compare_to_true(tval[0], ws.world.province_get_is_slave(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_slave_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_poptype(to_pop(primary_slot)), ws.culture_definitions.slaves);
}
TRIGGER_FUNCTION(tf_is_independant) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))), dcon::nation_id());
}
TRIGGER_FUNCTION(tf_has_national_minority_province) {
	auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto pcultures = ws.world.nation_get_primary_culture(owners);
	auto prov_populations = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);

	auto majority_pop = ve::apply([&ws](int32_t p_slot, dcon::culture_id c) {
		return ws.world.province_get_demographics(to_prov(p_slot), demographics::to_key(ws, c));
	}, primary_slot, pcultures);

	return compare_to_false(tval[0], majority_pop == prov_populations);
}
TRIGGER_FUNCTION(tf_has_national_minority_state) {
	auto owners = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto pcultures = ws.world.nation_get_primary_culture(owners);
	auto populations = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);

	auto majority_pop = ve::apply([&ws](int32_t p_slot, dcon::culture_id c) {
		return ws.world.state_instance_get_demographics(to_state(p_slot), demographics::to_key(ws, c));
	}, primary_slot, pcultures);

	return compare_to_false(tval[0], majority_pop == populations);
}
TRIGGER_FUNCTION(tf_has_national_minority_nation) {
	auto pcultures = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto populations = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);

	auto majority_pop = ve::apply([&ws](int32_t p_slot, dcon::culture_id c) {
		return ws.world.nation_get_demographics(to_nation(p_slot), demographics::to_key(ws, c));
	}, primary_slot, pcultures);

	return compare_to_false(tval[0], majority_pop == populations);
}
TRIGGER_FUNCTION(tf_government_nation) {
	auto gov_type = ws.world.nation_get_government_type(to_nation(primary_slot));
	return compare_values_eq(tval[0],
		gov_type,
		trigger::payload(tval[1]).gov_id);
}
TRIGGER_FUNCTION(tf_government_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto owners = ws.world.province_get_nation_from_province_ownership(location);

	return tf_government_nation<return_type>(tval, ws, to_generic(owners), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_capital) {
	return compare_values_eq(tval[0],
		ws.world.nation_get_capital(to_nation(primary_slot)),
		payload(tval[1]).prov_id);
}
TRIGGER_FUNCTION(tf_tech_school) {
	return compare_values_eq(tval[0],
		ws.world.nation_get_tech_school(to_nation(primary_slot)),
		trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_primary_culture) {
	return compare_values_eq(tval[0],
		ws.world.nation_get_primary_culture(to_nation(primary_slot)),
		trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_accepted_culture) {
	auto is_accepted = ve::apply([&ws, c = trigger::payload(tval[1]).cul_id](dcon::nation_id n) {
		if(n)
			return ws.world.nation_get_accepted_cultures(n).contains(c);
		else
			return false;
	}, to_nation(primary_slot));
	return compare_to_true(tval[0], is_accepted);
}
TRIGGER_FUNCTION(tf_culture_pop) {
	return compare_values_eq(tval[0],
		ws.world.pop_get_culture(to_pop(primary_slot)),
		trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_culture_state) {
	return compare_values_eq(tval[0],
		ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
		trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_culture_province) {
	return compare_values_eq(tval[0],
		ws.world.province_get_dominant_culture(to_prov(primary_slot)),
		trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_culture_nation) {
	auto c = trigger::payload(tval[1]).cul_id;
	return compare_to_true(tval[0], nations::nation_accepts_culture(ws, to_nation(primary_slot), c));
}
TRIGGER_FUNCTION(tf_culture_pop_reb) {
	return compare_values_eq(tval[0],
		ws.world.pop_get_culture(to_pop(primary_slot)),
		ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_state_reb) {
	return compare_values_eq(tval[0],
		ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
		ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_province_reb) {
	return compare_values_eq(tval[0],
		ws.world.province_get_dominant_culture(to_prov(primary_slot)),
		ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_nation_reb) {
	return compare_to_true(tval[0], nations::nation_accepts_culture(ws, to_nation(primary_slot), ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot))));
}
TRIGGER_FUNCTION(tf_culture_this_nation) {
	auto pc = ws.world.pop_get_culture(to_pop(primary_slot));
	return compare_to_true(tval[0], nations::nation_accepts_culture(ws, to_nation(this_slot), pc));
}
TRIGGER_FUNCTION(tf_culture_from_nation) {
	auto pc = ws.world.pop_get_culture(to_pop(primary_slot));
	return compare_to_true(tval[0], nations::nation_accepts_culture(ws, to_nation(from_slot), pc));
}
TRIGGER_FUNCTION(tf_culture_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return tf_culture_this_nation<return_type>(tval, ws, primary_slot, to_generic(owner), int32_t());
}
TRIGGER_FUNCTION(tf_culture_this_pop) {
	auto loc = ws.world.pop_get_province_from_pop_location(to_pop(this_slot));
	auto owner = ws.world.province_get_nation_from_province_ownership(loc);
	return tf_culture_this_nation<return_type>(tval, ws, primary_slot, to_generic(owner), int32_t());
}
TRIGGER_FUNCTION(tf_culture_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return tf_culture_this_nation<return_type>(tval, ws, primary_slot, to_generic(owner), int32_t());
}
TRIGGER_FUNCTION(tf_culture_group_nation) {
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)), trigger::payload(tval[1]).culgrp_id);
}
TRIGGER_FUNCTION(tf_culture_group_pop) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, trigger::payload(tval[1]).culgrp_id);
}
TRIGGER_FUNCTION(tf_culture_group_reb_nation) {
	auto c = ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot));
	auto rcg = ws.world.culture_get_group_from_culture_group_membership(c);

	return compare_values_eq(tval[0], rcg, nations::primary_culture_group(ws, to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_reb_pop) {
	auto c = ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot));
	auto rcg = ws.world.culture_get_group_from_culture_group_membership(c);

	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);

	return compare_values_eq(tval[0], rcg, cgroups);
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_nation) {
	return compare_values_eq(tval[0],
		nations::primary_culture_group(ws, to_nation(primary_slot)),
		nations::primary_culture_group(ws, to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_nation) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_nation_from_nation) {
	return compare_values_eq(tval[0],
		nations::primary_culture_group(ws, to_nation(primary_slot)),
		nations::primary_culture_group(ws, to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_pop_from_nation) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_province) {
	auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0],
		nations::primary_culture_group(ws, to_nation(primary_slot)),
		nations::primary_culture_group(ws, owners));
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_province) {
	auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, owners));
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_state) {
	auto owners = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0],
		nations::primary_culture_group(ws, to_nation(primary_slot)),
		nations::primary_culture_group(ws, owners));
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_state) {
	auto owners = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, owners));
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0],
		nations::primary_culture_group(ws, to_nation(primary_slot)),
		nations::primary_culture_group(ws, owner));
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, owner));
}
TRIGGER_FUNCTION(tf_religion) {
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		trigger::payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_religion_reb) {
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		ws.world.rebel_faction_get_religion(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_religion_from_nation) {
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		ws.world.nation_get_religion(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_religion_this_nation) {
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		ws.world.nation_get_religion(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_religion_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_religion_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_religion_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0],
		ws.world.pop_get_religion(to_pop(primary_slot)),
		ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_terrain_province) {
	return compare_values_eq(tval[0],
		ws.world.province_get_terrain(to_prov(primary_slot)),
		trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_terrain_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	return compare_values_eq(tval[0],
		ws.world.province_get_terrain(location),
		trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_trade_goods) {
	return compare_values_eq(tval[0],
		ws.world.province_get_rgo(to_prov(primary_slot)),
		trigger::payload(tval[1]).com_id);
}

TRIGGER_FUNCTION(tf_is_secondary_power_nation) {
	return compare_to_true(tval[0],
		ws.world.nation_get_rank(to_nation(primary_slot)) <= uint32_t(ws.defines.colonial_rank));
}
TRIGGER_FUNCTION(tf_is_secondary_power_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0],
		ws.world.nation_get_rank(owner) <= uint32_t(ws.defines.colonial_rank));
}
TRIGGER_FUNCTION(tf_has_faction_nation) {
	auto rebel_type = trigger::payload(tval[1]).reb_id;
	auto result = ve::apply([&ws, rebel_type](dcon::nation_id n) {
		for(auto factions : ws.world.nation_get_rebellion_within(n)) {
			if(factions.get_rebels().get_type() == rebel_type)
				return true;
		}
		return false;
	}, to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_faction_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_rebel_group(to_pop(primary_slot)), trigger::payload(tval[1]).reb_id);
}
auto unowned_core_accumulator(sys::state const& ws, dcon::nation_id n) {
	return make_true_accumulator([&ws, n](ve::tagged_vector<int32_t> v) {
		auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(v));
		return owners != n;
	});
}

TRIGGER_FUNCTION(tf_has_unclaimed_cores) {
	auto nation_tag = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto result = ve::apply([&ws](dcon::nation_id n, dcon::national_identity_id t) {
		auto acc = unowned_core_accumulator(ws, n);

		for(auto p : ws.world.national_identity_get_core(t)) {
			acc.add_value(to_generic(p.get_province().id));
			if(acc.result)
				return true;
		}
		acc.flush();
		return acc.result;
	}, to_nation(primary_slot), nation_tag);

	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_cultural_union_bool) {
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	return !compare_values_eq(tval[0], union_group, dcon::culture_group_id());
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_self_pop) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto pgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);

	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);

	return compare_values_eq(tval[0], union_group, pgroup);
}
TRIGGER_FUNCTION(tf_is_cultural_union_pop_this_pop) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto pgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);

	auto ident = ws.world.nation_get_identity_from_identity_holder(nations::owner_of_pop(to_pop(this_slot)));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);

	return compare_values_eq(tval[0], union_group, pgroup);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_nation) {
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto c = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(c);

	auto tident = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto tunion_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tculture = ws.world.nation_get_primary_culture(to_nation(this_slot));
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(tculture);

	return compare_to_true(tval[0], (union_group == tcg) || (tunion_group == cg));
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_pop) {
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);

	auto pculture = ws.world.pop_get_culture(to_pop(this_slot));
	auto pgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);

	return compare_values_eq(tval[0], union_group, pgroup);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_state) {
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto c = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(c);

	auto tnation = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto tident = ws.world.nation_get_identity_from_identity_holder(tnation);
	auto tunion_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tculture = ws.world.nation_get_primary_culture(tnation);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(tculture);

	return compare_to_true(tval[0], (union_group == tcg) || (tunion_group == cg));
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_province) {
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto c = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(c);

	auto tnation = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto tident = ws.world.nation_get_identity_from_identity_holder(tnation);
	auto tunion_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tculture = ws.world.nation_get_primary_culture(tnation);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(tculture);

	return compare_to_true(tval[0], (union_group == tcg) || (tunion_group == cg));
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_rebel) { // Yeah, so the rebel faction is in the from slot ━━(￣ー￣*|━━
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);

	auto rculture = ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot));
	auto rgroup = ws.world.culture_get_group_from_culture_group_membership(rculture);

	return compare_values_eq(tval[0], union_group, rgroup);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_nation) {
	auto pculture = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cgroup);

	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tag_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(trigger::payload(tval[1]).tag_id);

	return compare_to_true(tval[0], (utag == trigger::payload(tval[1]).tag_id) || (tag_group == union_group));
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_pop) {
	auto pculture = ws.world.pop_get_culture(to_pop(this_slot));
	auto cgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cgroup);

	return compare_values_eq(tval[0], utag, trigger::payload(tval[1]).tag_id);
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_state) {
	auto state_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto pculture = ws.world.nation_get_primary_culture(state_owner);
	auto cgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cgroup);

	auto ident = ws.world.nation_get_identity_from_identity_holder(state_owner);
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tag_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(trigger::payload(tval[1]).tag_id);

	return compare_to_true(tval[0], (utag == trigger::payload(tval[1]).tag_id) || (tag_group == union_group));
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_province) {
	auto prov_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto pculture = ws.world.nation_get_primary_culture(prov_owner);
	auto cgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cgroup);

	auto ident = ws.world.nation_get_identity_from_identity_holder(prov_owner);
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tag_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(trigger::payload(tval[1]).tag_id);

	return compare_to_true(tval[0], (utag == trigger::payload(tval[1]).tag_id) || (tag_group == union_group));
}
TRIGGER_FUNCTION(tf_is_cultural_union_tag_this_nation) {
	auto pculture = ws.world.nation_get_primary_culture(to_nation(this_slot));
	auto cgroup = ws.world.culture_get_group_from_culture_group_membership(pculture);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cgroup);

	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto tag_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(trigger::payload(tval[1]).tag_id);

	return compare_to_true(tval[0], (utag == trigger::payload(tval[1]).tag_id) || (tag_group == union_group));
}
TRIGGER_FUNCTION(tf_can_build_factory_nation) {
	return compare_values_eq(tval[0],
		ws.world.nation_get_combined_issue_rules(to_nation(primary_slot)) & issue_rule::pop_build_factory,
		issue_rule::pop_build_factory);
}
TRIGGER_FUNCTION(tf_can_build_factory_province) {
	auto p_owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values_eq(tval[0],
		ws.world.nation_get_combined_issue_rules(p_owner) & issue_rule::pop_build_factory, issue_rule::pop_build_factory);
}
TRIGGER_FUNCTION(tf_can_build_factory_pop) {
	auto p_owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values_eq(tval[0],
		ws.world.nation_get_combined_issue_rules(p_owner) & issue_rule::pop_build_factory, issue_rule::pop_build_factory);
}
TRIGGER_FUNCTION(tf_war_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_is_at_war(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_war_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_is_at_war(owner));
}
TRIGGER_FUNCTION(tf_war_exhaustion_nation) {
	return compare_values(tval[0], ws.world.nation_get_war_exhaustion(to_nation(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_war_exhaustion_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ws.world.nation_get_war_exhaustion(owner), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_war_exhaustion_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values(tval[0], ws.world.nation_get_war_exhaustion(owner), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_blockade) {
	return compare_values(tval[0],
		nations::central_blockaded_fraction(ws, to_nation(primary_slot)),
		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_owns) {
	auto pid = payload(tval[1]).prov_id;
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(pid), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_controls) {
	auto pid = payload(tval[1]).prov_id;
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(pid), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_core_integer) {
	auto pid = payload(tval[1]).prov_id;
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto result = ve::apply([&ws, pid](dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_nation) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto result = ve::apply([&ws](dcon::province_id pid, dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto tag = ws.world.nation_get_identity_from_identity_holder(owner);
	auto result = ve::apply([&ws](dcon::province_id pid, dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto tag = ws.world.nation_get_identity_from_identity_holder(owner);
	auto result = ve::apply([&ws](dcon::province_id pid, dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto tag = ws.world.nation_get_identity_from_identity_holder(owner);
	auto result = ve::apply([&ws](dcon::province_id pid, dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_from_nation) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(from_slot));
	auto result = ve::apply([&ws](dcon::province_id pid, dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_reb) {
	auto rtags = ws.world.rebel_faction_get_defection_target(to_rebel(from_slot));
	auto result = ve::apply([&ws](dcon::province_id pid, dcon::national_identity_id t) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot), rtags);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_tag) {
	auto result = ve::apply([&ws, t = trigger::payload(tval[1]).tag_id](dcon::province_id pid) {
		for(auto c : ws.world.province_get_core(pid)) {
			if(c.get_identity() == t)
				return true;
		}
		return false;
	}, to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_num_of_revolts) {
	return compare_values(tval[0],
		ws.world.nation_get_rebel_controlled_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_revolt_percentage) {
	return compare_values(tval[0],
		nations::central_reb_controlled_fraction(ws, to_nation(primary_slot)),
		read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_num_of_cities_int) {
	return compare_values(tval[0],
		ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
		tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_cities_from_nation) {
	return compare_values(tval[0],
		ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
		ws.world.nation_get_owned_province_count(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_nation) {
	return compare_values(tval[0],
		ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
		ws.world.nation_get_owned_province_count(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values(tval[0],
		ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
		ws.world.nation_get_owned_province_count(owner));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values(tval[0],
		ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
		ws.world.nation_get_owned_province_count(owner));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values(tval[0],
		ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
		ws.world.nation_get_owned_province_count(owner));
}
TRIGGER_FUNCTION(tf_num_of_ports) {
	return compare_values(tval[0],
		ws.world.nation_get_central_ports(to_nation(primary_slot)),
		tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_allies) {
	return compare_values(tval[0],
		ws.world.nation_get_allies_count(to_nation(primary_slot)),
		tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_vassals) {
	return compare_values(tval[0],
		ws.world.nation_get_vassals_count(to_nation(primary_slot)),
		tval[1]);
}
TRIGGER_FUNCTION(tf_owned_by_tag) {
	auto holders = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], holders, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_nation) {
	return compare_values_eq(tval[0], to_nation(this_slot), ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_from_nation) {
	return compare_values_eq(tval[0], to_nation(from_slot), ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_province) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(to_prov(this_slot)), ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], owner, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], owner, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_exists_bool) {
	return compare_to_true(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)) != 0);
}
TRIGGER_FUNCTION(tf_exists_tag) {
	auto holders = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	return compare_to_true(tval[0], ws.world.nation_get_owned_province_count(holders) != 0);
}
template<typename return_type, typename primary_type, typename this_type, typename from_type>
struct trigger_container {
	constexpr static return_type(CALLTYPE* trigger_functions[])(uint16_t const*, sys::state&,
		primary_type, this_type, from_type) = {

		tf_none<return_type, primary_type, this_type, from_type>,
		tf_year<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t year = 0x0001;
		tf_month<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t month = 0x0002;
		tf_port<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t port = 0x0003;
		tf_rank<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rank = 0x0004;
		tf_technology<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t technology = 0x0005;
		tf_strata_rich<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t strata_rich = 0x0006;
		tf_strata_middle<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t life_rating_province = 0x0007;
		tf_strata_poor<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t life_rating_state = 0x0008;
		tf_has_empty_adjacent_state_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_empty_adjacent_state_province = 0x0009;
		tf_has_empty_adjacent_state_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_empty_adjacent_state_state = 0x000A;
		tf_state_id_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t state_id_province = 0x000B;
		tf_state_id_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t state_id_state = 0x000C;
		tf_cash_reserves<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t cash_reserves = 0x000D;
		tf_unemployment_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_nation = 0x000E;
		tf_unemployment_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_state = 0x000F;
		tf_unemployment_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_province = 0x0010;
		tf_unemployment_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_pop = 0x0011;
		tf_is_slave_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_slave_nation = 0x0012;
		tf_is_slave_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_slave_state = 0x0013;
		tf_is_slave_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_slave_province = 0x0014;
		tf_is_slave_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_slave_pop = 0x0015;
		tf_is_independant<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_independant = 0x0016;
		tf_has_national_minority_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_national_minority_province = 0x0017;
		tf_has_national_minority_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_national_minority_state = 0x0018;
		tf_has_national_minority_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_national_minority_nation = 0x0019;
		tf_government_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t government_nation = 0x001A;
		tf_government_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t government_pop = 0x001B;
		tf_capital<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t capital = 0x001C;
		tf_tech_school<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tech_school = 0x001D;
		tf_primary_culture<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t primary_culture = 0x001E;
		tf_accepted_culture<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t accepted_culture = 0x001F;
		tf_culture_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_pop = 0x0020;
		tf_culture_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_state = 0x0021;
		tf_culture_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_province = 0x0022;
		tf_culture_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_nation = 0x0023;
		tf_culture_pop_reb<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_pop_reb = 0x0024;
		tf_culture_state_reb<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_state_reb = 0x0025;
		tf_culture_province_reb<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_province_reb = 0x0026;
		tf_culture_nation_reb<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_nation_reb = 0x0027;
		tf_culture_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_from_nation = 0x0028;
		tf_culture_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_this_nation = 0x0029;
		tf_culture_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_this_state = 0x002A;
		tf_culture_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_this_pop = 0x002B;
		tf_culture_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_this_province = 0x002C;
		tf_culture_group_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_nation = 0x002D;
		tf_culture_group_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_pop = 0x002E;
		tf_culture_group_reb_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_reb_nation = 0x002F;
		tf_culture_group_reb_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_reb_pop = 0x0030;
		tf_culture_group_nation_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_nation_from_nation = 0x0031;
		tf_culture_group_pop_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_pop_from_nation = 0x0032;
		tf_culture_group_nation_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_nation_this_nation = 0x0033;
		tf_culture_group_pop_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_pop_this_nation = 0x0034;
		tf_culture_group_nation_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_nation_this_province = 0x0035;
		tf_culture_group_pop_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_pop_this_province = 0x0036;
		tf_culture_group_nation_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_nation_this_state = 0x0037;
		tf_culture_group_pop_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_pop_this_state = 0x0038;
		tf_culture_group_nation_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_nation_this_pop = 0x0039;
		tf_culture_group_pop_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_group_pop_this_pop = 0x003A;
		tf_religion<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion = 0x003B;
		tf_religion_reb<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_reb = 0x003C;
		tf_religion_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_from_nation = 0x003D;
		tf_religion_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_this_nation = 0x003E;
		tf_religion_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_this_state = 0x003F;
		tf_religion_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_this_province = 0x0040;
		tf_religion_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_this_pop = 0x0041;
		tf_terrain_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t terrain_province = 0x0042;
		tf_terrain_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t terrain_pop = 0x0043;
		tf_trade_goods<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t trade_goods = 0x0044;
		tf_is_secondary_power_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_secondary_power_pop = 0x0045;
		tf_is_secondary_power_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_secondary_power_nation = 0x0046;
		tf_has_faction_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_faction_nation = 0x0047;
		tf_has_faction_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_faction_pop = 0x0048;
		tf_has_unclaimed_cores<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_unclaimed_cores = 0x0049;
		tf_is_cultural_union_bool<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_bool = 0x004A;
		tf_is_cultural_union_this_self_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_this_self_pop = 0x004B;
		tf_is_cultural_union_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_this_pop = 0x004C;
		tf_is_cultural_union_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_this_state = 0x004D;
		tf_is_cultural_union_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_this_province = 0x004E;
		tf_is_cultural_union_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_this_nation = 0x004F;
		tf_is_cultural_union_this_rebel<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_this_rebel = 0x0050;
		tf_is_cultural_union_tag_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_tag_nation = 0x0051;
		tf_is_cultural_union_tag_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_tag_this_pop = 0x0052;
		tf_is_cultural_union_tag_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_tag_this_state = 0x0053;
		tf_is_cultural_union_tag_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_tag_this_province = 0x0054;
		tf_is_cultural_union_tag_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_tag_this_nation = 0x0055;
		tf_can_build_factory_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_factory_pop = 0x0056;
		tf_war_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_pop = 0x0057;
		tf_war_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_nation = 0x0058;
		tf_war_exhaustion_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_exhaustion_nation = 0x0059;
		tf_blockade<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t blockade = 0x005A;
		tf_owns<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owns = 0x005B;
		tf_controls<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controls = 0x005C;
		tf_is_core_integer<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_integer = 0x005D;
		tf_is_core_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_this_nation = 0x005E;
		tf_is_core_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_this_state = 0x005F;
		tf_is_core_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_this_province = 0x0060;
		tf_is_core_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_this_pop = 0x0061;
		tf_is_core_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_from_nation = 0x0062;
		tf_is_core_reb<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_reb = 0x0063;
		tf_is_core_tag<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_core_tag = 0x0064;
		tf_num_of_revolts<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_revolts = 0x0065;
		tf_revolt_percentage<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t revolt_percentage = 0x0066;
		tf_num_of_cities_int<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_cities_int = 0x0067;
		tf_num_of_cities_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_cities_from_nation = 0x0068;
		tf_num_of_cities_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_cities_this_nation = 0x0069;
		tf_num_of_cities_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_cities_this_state = 0x006A;
		tf_num_of_cities_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_cities_this_province = 0x006B;
		tf_num_of_cities_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_cities_this_pop = 0x006C;
		tf_num_of_ports<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_ports = 0x006D;
		tf_num_of_allies<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_allies = 0x006E;
		tf_num_of_vassals<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_vassals = 0x006F;
		tf_owned_by_tag<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owned_by_tag = 0x0070;
		tf_owned_by_from_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owned_by_from_nation = 0x0071;
		tf_owned_by_this_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owned_by_this_nation = 0x0072;
		tf_owned_by_this_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owned_by_this_province = 0x0073;
		tf_owned_by_this_state<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owned_by_this_state = 0x0074;
		tf_owned_by_this_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t owned_by_this_pop = 0x0075;
		tf_exists_bool<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t exists_bool = 0x0076;
		tf_exists_tag<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t exists_tag = 0x0077;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_country_flag = 0x0078;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_nation = 0x0079;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_state = 0x007A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_province = 0x007B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_pop = 0x007C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_nation_this = 0x007D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_state_this = 0x007E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_province_this = 0x007F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_pop_this = 0x0080;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_nation_from = 0x0081;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_state_from = 0x0082;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_province_from = 0x0083;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t continent_pop_from = 0x0084;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t casus_belli_tag = 0x0085;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t casus_belli_from = 0x0086;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t casus_belli_this_nation = 0x0087;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t casus_belli_this_state = 0x0088;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t casus_belli_this_province = 0x0089;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t casus_belli_this_pop = 0x008A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_access_tag = 0x008B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_access_from = 0x008C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_access_this_nation = 0x008D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_access_this_state = 0x008E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_access_this_province = 0x008F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_access_this_pop = 0x0090;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t prestige_value = 0x0091;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t prestige_from = 0x0092;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t prestige_this_nation = 0x0093;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t prestige_this_state = 0x0094;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t prestige_this_province = 0x0095;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t prestige_this_pop = 0x0096;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t badboy = 0x0097;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_state = 0x0098;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_fort = 0x0099;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_railroad = 0x009A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_naval_base = 0x009B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t empty = 0x009C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_blockaded = 0x009D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_country_modifier = 0x009E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_province_modifier = 0x009F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t region = 0x00A0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tag_tag = 0x00A1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tag_this_nation = 0x00A2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tag_this_province = 0x00A3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tag_from_nation = 0x00A4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tag_from_province = 0x00A5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t neighbour_tag = 0x00A6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t neighbour_this = 0x00A7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t neighbour_from = 0x00A8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t units_in_province_value = 0x00A9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t units_in_province_from = 0x00AA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t units_in_province_this_nation = 0x00AB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t units_in_province_this_province = 0x00AC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t units_in_province_this_state = 0x00AD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t units_in_province_this_pop = 0x00AE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_with_tag = 0x00AF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_with_from = 0x00B0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_with_this_nation = 0x00B1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_with_this_province = 0x00B2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_with_this_state = 0x00B3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_with_this_pop = 0x00B4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unit_in_battle = 0x00B5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_amount_of_divisions = 0x00B6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t money = 0x00B7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t lost_national = 0x00B8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_vassal = 0x00B9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t ruling_party_ideology_pop = 0x00BA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t ruling_party_ideology_nation = 0x00BB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t ruling_party = 0x00BC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_ideology_enabled = 0x00BD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t political_reform_want_nation = 0x00BE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t political_reform_want_pop = 0x00BF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_reform_want_nation = 0x00C0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_reform_want_pop = 0x00C1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_amount_of_ships = 0x00C2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t plurality = 0x00C3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t corruption = 0x00C4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_state_religion_pop = 0x00C5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_state_religion_province = 0x00C6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_state_religion_state = 0x00C7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_pop = 0x00C8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_province = 0x00C9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_state = 0x00CA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_nation_this_pop = 0x00CB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_nation_this_nation = 0x00CC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_nation_this_state = 0x00CD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_nation_this_province = 0x00CE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_state_this_pop = 0x00CF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_state_this_nation = 0x00D0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_state_this_state = 0x00D1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_state_this_province = 0x00D2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_province_this_pop = 0x00D3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_province_this_nation = 0x00D4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_province_this_state = 0x00D5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_province_this_province = 0x00D6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_pop_this_pop = 0x00D7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_pop_this_nation = 0x00D8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_pop_this_state = 0x00D9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_primary_culture_pop_this_province = 0x00DA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_accepted_culture_pop = 0x00DB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_accepted_culture_province = 0x00DC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_accepted_culture_state = 0x00DD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_coastal = 0x00DE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_sphere_tag = 0x00DF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_sphere_from = 0x00E0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_sphere_this_nation = 0x00E1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_sphere_this_province = 0x00E2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_sphere_this_state = 0x00E3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_sphere_this_pop = 0x00E4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t produces_nation = 0x00E5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t produces_state = 0x00E6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t produces_province = 0x00E7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t produces_pop = 0x00E8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t average_militancy_nation = 0x00E9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t average_militancy_state = 0x00EA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t average_militancy_province = 0x00EB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t average_consciousness_nation = 0x00EC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t average_consciousness_state = 0x00ED;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t average_consciousness_province = 0x00EE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_next_reform_nation = 0x00EF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_next_reform_pop = 0x00F0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rebel_power_fraction = 0x00F1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t recruited_percentage_nation = 0x00F2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t recruited_percentage_pop = 0x00F3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_culture_core = 0x00F4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t nationalism = 0x00F5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_overseas = 0x00F6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_rebels = 0x00F7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_tag = 0x00F8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_from = 0x00F9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_this_nation = 0x00FA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_this_province = 0x00FB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_this_state = 0x00FC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_this_pop = 0x00FD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_owner = 0x00FE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t controlled_by_reb = 0x00FF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_canal_enabled = 0x0100;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_state_capital = 0x0101;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t truce_with_tag = 0x0102;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t truce_with_from = 0x0103;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t truce_with_this_nation = 0x0104;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t truce_with_this_province = 0x0105;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t truce_with_this_state = 0x0106;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t truce_with_this_pop = 0x0107;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_pops_nation = 0x0108;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_pops_state = 0x0109;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_pops_province = 0x010A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_pops_pop = 0x010B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_type_nation = 0x010C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_type_state = 0x010D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_type_province = 0x010E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_type_pop = 0x010F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_empty_adjacent_province = 0x0110;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_leader = 0x0111;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t ai = 0x0112;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_create_vassals = 0x0113;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_possible_vassal = 0x0114;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t province_id = 0x0115;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t vassal_of_tag = 0x0116;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t vassal_of_from = 0x0117;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t vassal_of_this_nation = 0x0118;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t vassal_of_this_province = 0x0119;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t vassal_of_this_state = 0x011A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t vassal_of_this_pop = 0x011B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t alliance_with_tag = 0x011C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t alliance_with_from = 0x011D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t alliance_with_this_nation = 0x011E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t alliance_with_this_province = 0x011F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t alliance_with_this_state = 0x0120;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t alliance_with_this_pop = 0x0121;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_recently_lost_war = 0x0122;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_mobilised = 0x0123;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t mobilisation_size = 0x0124;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t crime_higher_than_education_nation = 0x0125;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t crime_higher_than_education_state = 0x0126;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t crime_higher_than_education_province = 0x0127;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t crime_higher_than_education_pop = 0x0128;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t agree_with_ruling_party = 0x0129;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_colonial_state = 0x012A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_colonial_province = 0x012B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_factories = 0x012C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_default_tag = 0x012D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_default_from = 0x012E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_default_this_nation = 0x012F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_default_this_province = 0x0130;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_default_this_state = 0x0131;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t in_default_this_pop = 0x0132;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t total_num_of_ports = 0x0133;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t always = 0x0134;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t election = 0x0135;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_global_flag = 0x0136;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_capital = 0x0137;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t nationalvalue_nation = 0x0138;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t industrial_score_value = 0x0139;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t industrial_score_from_nation = 0x013A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t industrial_score_this_nation = 0x013B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t industrial_score_this_pop = 0x013C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t industrial_score_this_state = 0x013D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t industrial_score_this_province = 0x013E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_score_value = 0x013F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_score_from_nation = 0x0140;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_score_this_nation = 0x0141;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_score_this_pop = 0x0142;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_score_this_state = 0x0143;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_score_this_province = 0x0144;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t civilized_nation = 0x0145;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t civilized_pop = 0x0146;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t civilized_province = 0x0147;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t national_provinces_occupied = 0x0148;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_greater_power_nation = 0x0149;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_greater_power_pop = 0x014A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_tax = 0x014B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_tax = 0x014C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_tax = 0x014D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_spending_nation = 0x014E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_spending_pop = 0x014F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_spending_province = 0x0150;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t colonial_nation = 0x0151;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_religion_nation = 0x0152;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_religion_state = 0x0153;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_religion_province = 0x0154;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_culture_nation = 0x0155;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_culture_state = 0x0156;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_culture_province = 0x0157;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_issue_nation = 0x0158;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_issue_state = 0x0159;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_issue_province = 0x015A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_issue_pop = 0x015B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_ideology_nation = 0x015C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_ideology_state = 0x015D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_ideology_province = 0x015E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_majority_ideology_pop = 0x015F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_militancy_nation = 0x0160;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_militancy_state = 0x0161;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_militancy_province = 0x0162;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_militancy_pop = 0x0163;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_militancy_nation = 0x0164;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_militancy_state = 0x0165;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_militancy_province = 0x0166;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_militancy_pop = 0x0167;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_militancy_nation = 0x0168;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_militancy_state = 0x0169;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_militancy_province = 0x016A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_militancy_pop = 0x016B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_tax_above_poor = 0x016C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_has_union_tag_pop = 0x016D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t culture_has_union_tag_nation = 0x016E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_tag = 0x016F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_from = 0x0170;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_nation = 0x0171;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_province = 0x0172;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_state = 0x0173;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_pop = 0x0174;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_union_nation = 0x0175;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_union_province = 0x0176;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_union_state = 0x0177;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t this_culture_union_this_union_pop = 0x0178;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t minorities_nation = 0x0179;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t minorities_state = 0x017A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t minorities_province = 0x017B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t revanchism_nation = 0x017C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t revanchism_pop = 0x017D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_crime = 0x017E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_substates = 0x017F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t num_of_vassals_no_substates = 0x0180;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t brigades_compare_this = 0x0181;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t brigades_compare_from = 0x0182;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_tag = 0x0183;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_from = 0x0184;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_this_nation = 0x0185;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_this_province = 0x0186;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_this_state = 0x0187;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_this_pop = 0x0188;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_discovered = 0x0189;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_progress = 0x018A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t civilization_progress = 0x018B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t constructing_cb_type = 0x018C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_our_vassal_tag = 0x018D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_our_vassal_from = 0x018E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_our_vassal_this_nation = 0x018F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_our_vassal_this_province = 0x0190;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_our_vassal_this_state = 0x0191;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_our_vassal_this_pop = 0x0192;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t substate_of_tag = 0x0193;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t substate_of_from = 0x0194;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t substate_of_this_nation = 0x0195;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t substate_of_this_province = 0x0196;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t substate_of_this_state = 0x0197;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t substate_of_this_pop = 0x0198;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_substate = 0x0199;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t great_wars_enabled = 0x019A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_nationalize = 0x019B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t part_of_sphere = 0x019C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_sphere_leader_of_tag = 0x019D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_sphere_leader_of_from = 0x019E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_sphere_leader_of_this_nation = 0x019F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_sphere_leader_of_this_province = 0x01A0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_sphere_leader_of_this_state = 0x01A1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_sphere_leader_of_this_pop = 0x01A2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t number_of_states = 0x01A3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_score = 0x01A4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_releasable_vassal_from = 0x01A5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_releasable_vassal_other = 0x01A6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_recent_imigration = 0x01A7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t province_control_days = 0x01A8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_disarmed = 0x01A9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t big_producer = 0x01AA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t someone_can_form_union_tag_from = 0x01AB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t someone_can_form_union_tag_other = 0x01AC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_movement_strength = 0x01AD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t political_movement_strength = 0x01AE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_factory_in_capital_state = 0x01AF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_movement = 0x01B0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t political_movement = 0x01B1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_cultural_sphere = 0x01B2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t world_wars_enabled = 0x01B3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_pop_this_pop = 0x01B4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_state_this_pop = 0x01B5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_province_this_pop = 0x01B6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_nation_this_pop = 0x01B7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_pop = 0x01B8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_state = 0x01B9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_province = 0x01BA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_culture_nation = 0x01BB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_pop_this_pop = 0x01BC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_state_this_pop = 0x01BD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_province_this_pop = 0x01BE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_nation_this_pop = 0x01BF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_pop = 0x01C0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_state = 0x01C1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_province = 0x01C2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_pop_religion_nation = 0x01C3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t life_needs = 0x01C4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t everyday_needs = 0x01C5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t luxury_needs = 0x01C6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t consciousness_pop = 0x01C7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t consciousness_province = 0x01C8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t consciousness_state = 0x01C9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t consciousness_nation = 0x01CA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t literacy_pop = 0x01CB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t literacy_province = 0x01CC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t literacy_state = 0x01CD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t literacy_nation = 0x01CE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t militancy_pop = 0x01CF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t militancy_province = 0x01D0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t militancy_state = 0x01D1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t militancy_nation = 0x01D2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_spending_pop = 0x01D3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_spending_province = 0x01D4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_spending_state = 0x01D5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t military_spending_nation = 0x01D6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t administration_spending_pop = 0x01D7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t administration_spending_province = 0x01D8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t administration_spending_state = 0x01D9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t administration_spending_nation = 0x01DA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t education_spending_pop = 0x01DB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t education_spending_province = 0x01DC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t education_spending_state = 0x01DD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t education_spending_nation = 0x01DE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t trade_goods_in_state_state = 0x01DF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t trade_goods_in_state_province = 0x01E0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_flashpoint = 0x01E1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t flashpoint_tension = 0x01E2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t crisis_exist = 0x01E3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_liberation_crisis = 0x01E4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_claim_crisis = 0x01E5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t crisis_temperature = 0x01E6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t involved_in_crisis_pop = 0x01E7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t involved_in_crisis_nation = 0x01E8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_life_needs_nation = 0x01E9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_life_needs_state = 0x01EA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_life_needs_province = 0x01EB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_life_needs_pop = 0x01EC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_everyday_needs_nation = 0x01ED;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_everyday_needs_state = 0x01EE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_everyday_needs_province = 0x01EF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_everyday_needs_pop = 0x01F0;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_luxury_needs_nation = 0x01F1;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_luxury_needs_state = 0x01F2;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_luxury_needs_province = 0x01F3;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t rich_strata_luxury_needs_pop = 0x01F4;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_life_needs_nation = 0x01F5;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_life_needs_state = 0x01F6;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_life_needs_province = 0x01F7;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_life_needs_pop = 0x01F8;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_everyday_needs_nation = 0x01F9;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_everyday_needs_state = 0x01FA;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_everyday_needs_province = 0x01FB;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_everyday_needs_pop = 0x01FC;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_luxury_needs_nation = 0x01FD;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_luxury_needs_state = 0x01FE;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_luxury_needs_province = 0x01FF;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t middle_strata_luxury_needs_pop = 0x0200;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_life_needs_nation = 0x0201;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_life_needs_state = 0x0202;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_life_needs_province = 0x0203;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_life_needs_pop = 0x0204;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_everyday_needs_nation = 0x0205;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_everyday_needs_state = 0x0206;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_everyday_needs_province = 0x0207;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_everyday_needs_pop = 0x0208;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_luxury_needs_nation = 0x0209;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_luxury_needs_state = 0x020A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_luxury_needs_province = 0x020B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t poor_strata_luxury_needs_pop = 0x020C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t diplomatic_influence_tag = 0x020D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t diplomatic_influence_this_nation = 0x020E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t diplomatic_influence_this_province = 0x020F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t diplomatic_influence_from_nation = 0x0210;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t diplomatic_influence_from_province = 0x0211;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_nation = 0x0212;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_state = 0x0213;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_province = 0x0214;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_pop = 0x0215;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_nation_this_pop = 0x0216;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_state_this_pop = 0x0217;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t pop_unemployment_province_this_pop = 0x0218;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t relation_tag = 0x0219;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t relation_this_nation = 0x021A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t relation_this_province = 0x021B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t relation_from_nation = 0x021C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t relation_from_province = 0x021D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t check_variable = 0x021E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t upper_house = 0x021F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_by_type_nation = 0x0220;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_by_type_state = 0x0221;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_by_type_province = 0x0222;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t unemployment_by_type_pop = 0x0223;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_nation_province_id = 0x0224;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_from_nation_province_id = 0x0225;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_province_province_id = 0x0226;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_from_province_province_id = 0x0227;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_nation_from_province = 0x0228;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_from_nation_scope_province = 0x0229;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_railroad_no_limit_from_nation = 0x022A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_railroad_yes_limit_from_nation = 0x022B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_railroad_no_limit_this_nation = 0x022C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_railroad_yes_limit_this_nation = 0x022D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_fort_no_limit_from_nation = 0x022E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_fort_yes_limit_from_nation = 0x022F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_fort_no_limit_this_nation = 0x0230;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_fort_yes_limit_this_nation = 0x0231;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_naval_base_no_limit_from_nation = 0x0232;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_naval_base_yes_limit_from_nation = 0x0233;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_naval_base_no_limit_this_nation = 0x0234;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_in_province_naval_base_yes_limit_this_nation = 0x0235;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_railway_in_capital_yes_whole_state_yes_limit = 0x0236;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_railway_in_capital_yes_whole_state_no_limit = 0x0237;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_railway_in_capital_no_whole_state_yes_limit = 0x0238;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_railway_in_capital_no_whole_state_no_limit = 0x0239;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_fort_in_capital_yes_whole_state_yes_limit = 0x023A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_fort_in_capital_yes_whole_state_no_limit = 0x023B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_fort_in_capital_no_whole_state_yes_limit = 0x023C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_fort_in_capital_no_whole_state_no_limit = 0x023D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t work_available_nation = 0x023E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t work_available_state = 0x023F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t work_available_province = 0x0240;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_ideology_name_nation = 0x0241;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_ideology_name_state = 0x0242;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_ideology_name_province = 0x0243;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_ideology_name_pop = 0x0244;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_name_nation = 0x0245;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_name_state = 0x0246;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_name_province = 0x0247;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_name_pop = 0x0248;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_group_name_nation = 0x0249;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_group_name_state = 0x024A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_group_name_province = 0x024B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_issue_group_name_pop = 0x024C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_pop_type_name_nation = 0x024D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_pop_type_name_state = 0x024E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_pop_type_name_province = 0x024F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_pop_type_name_pop = 0x0250;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t variable_good_name = 0x0251;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t strata_middle = 0x0252;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t strata_poor = 0x0253;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_from_province_scope_province = 0x0254;
		tf_can_build_factory_nation<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_factory_nation = 0x0255;
		tf_can_build_factory_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t can_build_factory_province = 0x0256;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t nationalvalue_pop = 0x0257;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t nationalvalue_province = 0x0258;
		tf_war_exhaustion_pop<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_exhaustion_pop = 0x0259;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_culture_core_province_this_pop = 0x025A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t tag_pop = 0x025B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_country_flag_pop = 0x025C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_country_flag_province = 0x025D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_country_modifier_province = 0x025E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation = 0x025F;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation_reb = 0x0260;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation_from_nation = 0x0261;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation_this_nation = 0x0262;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation_this_state = 0x0263;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation_this_province = 0x0264;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t religion_nation_this_pop = 0x0265;
		tf_war_exhaustion_province<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t war_exhaustion_province = 0x0266;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_greater_power_province = 0x0267;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t is_cultural_union_pop_this_pop = 0x0268;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_factory = 0x0269;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_state_from_province = 0x026A;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t has_building_factory_from_province = 0x026B;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t party_loyalty_generic = 0x026C;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t invention = 0x026D;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t political_movement_from_reb = 0x026E;
		tf_none<return_type, primary_type, this_type, from_type>, //constexpr inline uint16_t social_movement_from_reb = 0x026F;
	};
};

template<typename return_type, typename primary_type, typename this_type, typename from_type>
return_type CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws, primary_type primary_slot, this_type this_slot, from_type from_slot) {
	if((*tval & trigger::is_scope) != 0) {
		return scope_container<return_type, primary_type, this_type, from_type>::scope_functions[*tval & trigger::code_mask](tval, ws, primary_slot, this_slot, from_slot);
	} else {
		return trigger_container<return_type, primary_type, this_type, from_type>::trigger_functions[*tval & trigger::code_mask](tval, ws, primary_slot, this_slot, from_slot);
		return false;
	}
}

#undef CALLTYPE
#undef TRIGGER_FUNCTION

/**/

}
