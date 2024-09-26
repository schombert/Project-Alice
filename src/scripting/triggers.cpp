#include "triggers.hpp"
#include "system_state.hpp"
#include "demographics.hpp"
#include "military_templates.hpp"
#include "nations_templates.hpp"
#include "prng.hpp"
#include "province_templates.hpp"
#include "ve_scalar_extensions.hpp"
#include "script_constants.hpp"
#include "politics.hpp"

namespace trigger {

#ifdef WIN32
#define CALLTYPE __vectorcall
#else
#define CALLTYPE
#endif

template<typename A, typename B>
[[nodiscard]] auto compare_values(uint16_t trigger_code, A value_a, B value_b) -> decltype(value_a == value_b) {
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
[[nodiscard]] auto compare_values_eq(uint16_t trigger_code, A value_a, B value_b) -> decltype(value_a == value_b) {
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

template<typename B>
[[nodiscard]] auto compare_values_eq(uint16_t trigger_code, dcon::nation_fat_id value_a, B value_b)
		-> decltype(value_a.id == value_b) {
	switch(trigger_code & trigger::association_mask) {
	case trigger::association_eq:
		return value_a.id == value_b;
	case trigger::association_gt:
		return value_a.id != value_b;
	case trigger::association_lt:
		return value_a.id != value_b;
	case trigger::association_le:
		return value_a.id == value_b;
	case trigger::association_ne:
		return value_a.id != value_b;
	case trigger::association_ge:
		return value_a.id == value_b;
	default:
		return value_a.id == value_b;
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
float read_float_from_payload(uint16_t const* data) {
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
int32_t read_int32_t_from_payload(uint16_t const* data) {
	union {
		struct {
			uint16_t low;
			uint16_t high;
		} v;
		int32_t f;
	} pack_float;

	pack_float.v.low = data[0];
	pack_float.v.high = data[1];

	return pack_float.f;
}

template<typename T>
struct gathered_s {
	using type = T;
};

template<>
struct gathered_s<ve::contiguous_tags<int32_t>> {
	using type = ve::tagged_vector<int32_t>;
};

template<>
struct gathered_s<ve::unaligned_contiguous_tags<int32_t>> {
	using type = ve::tagged_vector<int32_t>;
};

template<>
struct gathered_s<ve::partial_contiguous_tags<int32_t>> {
	using type = ve::tagged_vector<int32_t>;
};

template<typename T>
using gathered_t = typename gathered_s<T>::type;

template<typename return_type, typename primary_type, typename this_type, typename from_type>
return_type CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws, primary_type primary_slot, this_type this_slot,
		from_type from_slot);

#define TRIGGER_FUNCTION(function_name)                                                                                          \
	template<typename return_type, typename primary_type, typename this_type, typename from_type>                                  \
	return_type CALLTYPE function_name(uint16_t const* tval, sys::state& ws, primary_type primary_slot, this_type this_slot,       \
			from_type from_slot)

template<typename T>
struct full_mask { };

template<>
struct full_mask<bool> {
	static constexpr bool value = true;
};
template<>
struct full_mask<ve::vbitfield_type> {
	static constexpr ve::vbitfield_type value = ve::vbitfield_type{ve::vbitfield_type::storage(-1)};
};

template<typename T>
struct empty_mask { };

template<>
struct empty_mask<bool> {
	static constexpr bool value = false;
};
template<>
struct empty_mask<ve::vbitfield_type> {
	static constexpr ve::vbitfield_type value = ve::vbitfield_type{ve::vbitfield_type::storage(0)};
};

bool compare(bool a, bool b) {
	return a == b;
}
bool compare(ve::vbitfield_type a, ve::vbitfield_type b) {
	return a.v == b.v;
}

TRIGGER_FUNCTION(apply_disjuctively) {
	auto const source_size = 1 + get_trigger_scope_payload_size(tval);
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
	auto const source_size = 1 + get_trigger_scope_payload_size(tval);
	auto sub_units_start = tval + 2 + trigger_scope_data_payload(tval[0]);

	return_type result = return_type(true);
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
TRIGGER_FUNCTION(tf_unused_1) {
	return return_type(true);
}

TRIGGER_FUNCTION(tf_generic_scope) {
	return apply_subtriggers<return_type, primary_type, this_type, from_type>(tval, ws, primary_slot, this_slot, from_slot);
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
				result = (ve::compress_mask(F::operator()(value)).v & accumulated_mask) == accumulated_mask;
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
	return ve::apply(
			[&ws, tval](int32_t prov_id, int32_t t_slot, int32_t f_slot) {
				auto prov_tag = to_prov(prov_id);

				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					for(auto adj : ws.world.province_get_province_adjacency(prov_tag)) {
						if((adj.get_type() & province::border::impassible_bit) == 0) {
							auto other = adj.get_connected_provinces(prov_tag == adj.get_connected_provinces(0) ? 1 : 0).id;
							accumulator.add_value(to_generic(other));
						}
					}
					accumulator.flush();

					return accumulator.result;
				} else {
					auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

					for(auto adj : ws.world.province_get_province_adjacency(prov_tag)) {
						if((adj.get_type() & province::border::impassible_bit) == 0) {
							auto other = adj.get_connected_provinces(prov_tag == adj.get_connected_provinces(0) ? 1 : 0).id;
							accumulator.add_value(to_generic(other));
						}
					}
					accumulator.flush();

					return accumulator.result;
				}
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_neighbor_province_scope_state) {
	return ve::apply(
		[&ws, tval](int32_t s_id, int32_t t_slot, int32_t f_slot) {
				auto state_tag = to_state(s_id);

				std::vector<dcon::province_id> vadj;
				vadj.reserve(32);

				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(state_tag))) {
					if(p.get_province().get_state_membership() == state_tag) {
						for(auto adj : p.get_province().get_province_adjacency()) {
							if((adj.get_type() & province::border::impassible_bit) == 0) {
								auto other = (p.get_province() == adj.get_connected_provinces(0)) ? adj.get_connected_provinces(1).id : adj.get_connected_provinces(0).id;
								if(std::find(vadj.begin(), vadj.end(), other) == vadj.end())
									vadj.push_back(other);
							}
						}
					}
				}

				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					for(auto p : vadj)
						accumulator.add_value(to_generic(p));

					accumulator.flush();
					return accumulator.result;
				} else {
					auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

					for(auto p : vadj)
						accumulator.add_value(to_generic(p));

					accumulator.flush();
					return accumulator.result;
				}
			},
	primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_nation) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_neighbor_country_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto province_owner = ws.world.province_get_nation_from_province_ownership(location);

	return tf_x_neighbor_country_scope_nation<return_type>(tval, ws, to_generic(province_owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_nation) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
				auto nid = to_nation(p_slot);

				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					for(auto wars_in : ws.world.nation_get_war_participant(nid)) {
						auto is_attacker = wars_in.get_is_attacker();
						for(auto o : wars_in.get_war().get_war_participant()) {
							if(o.get_is_attacker() != is_attacker && o.get_nation() != nid) {
								accumulator.add_value(to_generic(o.get_nation().id));
								if(accumulator.result)
									return true;
							}
						}
					}

					accumulator.flush();
					return accumulator.result;
				} else {
					auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

					for(auto wars_in : ws.world.nation_get_war_participant(nid)) {
						auto is_attacker = wars_in.get_is_attacker();
						for(auto o : wars_in.get_war().get_war_participant()) {
							if(o.get_is_attacker() != is_attacker && o.get_nation() != nid) {
								accumulator.add_value(to_generic(o.get_nation().id));
								if(!accumulator.result)
									return false;
							}
						}
					}

					accumulator.flush();
					return accumulator.result;
				}
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_war_countries_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto province_owner = ws.world.province_get_nation_from_province_ownership(location);

	return tf_x_war_countries_scope_nation<return_type>(tval, ws, to_generic(province_owner), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_country_scope) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					uint32_t added = 0;
					for(auto n : ws.world.in_nation) {
						if(n.get_owned_province_count() != 0) {
							accumulator.add_value(to_generic(n.id));
							if(accumulator.result)
								return true;
						}
					}

					accumulator.flush();
					return accumulator.result;
				} else {
					auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

					for(auto n : ws.world.in_nation) {
						if(n.get_owned_province_count() != 0) {
							accumulator.add_value(to_generic(n.id));
							if(!accumulator.result)
								return false;
						}
					}

					accumulator.flush();
					return accumulator.result;
				}
			},
		primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_greater_power_scope) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
				uint32_t great_nations_count = uint32_t(ws.defines.great_nations_count);

				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					uint32_t added = 0;
					for(uint32_t i = 0; i < ws.nations_by_rank.size() && added < great_nations_count; ++i) {
						if(nations::is_great_power(ws, ws.nations_by_rank[i])) {
							++added;
							accumulator.add_value(to_generic(ws.nations_by_rank[i]));
							if(accumulator.result)
								return true;
						}
					}

					accumulator.flush();
					return accumulator.result;
				} else {
					auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

					uint32_t added = 0;
					for(uint32_t i = 0; i < ws.nations_by_rank.size() && added < great_nations_count; ++i) {
						if(nations::is_great_power(ws, ws.nations_by_rank[i])) {
							++added;
							accumulator.add_value(to_generic(ws.nations_by_rank[i]));
							if(!accumulator.result)
								return false;
						}
					}

					accumulator.flush();
					return accumulator.result;
				}
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_state) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_owned_province_scope_nation) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_core_scope_province) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_core_scope_nation) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}

TRIGGER_FUNCTION(tf_x_state_scope) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_substate_scope) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
				auto nid = fatten(ws.world, to_nation(p_slot));

				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					for(auto v : nid.get_overlord_as_ruler()) {
						if(v.get_subject().get_is_substate()) {
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
						if(v.get_subject().get_is_substate()) {
							accumulator.add_value(to_generic(v.get_subject().id));
							if(!accumulator.result)
								return false;
						}
					}

					accumulator.flush();
					return accumulator.result;
				}
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_sphere_member_scope) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_pop_scope_province) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_pop_scope_state) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_pop_scope_nation) {
	return ve::apply(
			[&ws, tval](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
			primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_provinces_in_variable_region) {
	auto state_def = trigger::payload(*(tval + 2)).state_id;

	return ve::apply(
			[&ws, tval, state_def](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
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
			},
		primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_x_provinces_in_variable_region_proper) {
	auto state_def = trigger::payload(*(tval + 2)).reg_id;

	return ve::apply(
			[&ws, tval, state_def](int32_t p_slot, int32_t t_slot, int32_t f_slot) {
				if(*tval & trigger::is_existence_scope) {
					auto accumulator = existence_accumulator(ws, tval, t_slot, f_slot);

					for(auto i : ws.world.region_get_region_membership(state_def)) {
						accumulator.add_value(to_generic(i.get_province().id));
						if(accumulator.result)
							return true;
					}

					accumulator.flush();
					return accumulator.result;
				} else {
					auto accumulator = universal_accumulator(ws, tval, t_slot, f_slot);

					for(auto i : ws.world.region_get_region_membership(state_def)) {
						accumulator.add_value(to_generic(i.get_province().id));
						if(!accumulator.result)
							return false;
					}

					accumulator.flush();
					return accumulator.result;
				}
			},
		primary_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_owner_scope_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(owner), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_owner_scope_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(owner), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_controller_scope) {
	auto controller = ws.world.province_get_nation_from_province_control(to_prov(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(controller),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_location_scope) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(location), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(owner), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_country_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto owner = ws.world.province_get_nation_from_province_ownership(location);
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(owner), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_capital_scope) {
	auto cap = ws.world.nation_get_capital(to_nation(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(cap), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_capital_scope_province) {
	auto cap = ws.world.nation_get_capital(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(cap), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_capital_scope_pop) {
	auto cap = ws.world.nation_get_capital(nations::owner_of_pop(ws, to_pop(primary_slot)));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(cap), this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_this_scope) {
	return apply_subtriggers<return_type, this_type, this_type, from_type>(tval, ws, this_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_from_scope) {
	return apply_subtriggers<return_type, from_type, this_type, from_type>(tval, ws, from_slot, this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_sea_zone_scope) {
	/*
	auto sea_zones = ve::apply(
			[&ws](int32_t p_slot) {
				auto pid = fatten(ws.world, to_prov(p_slot));
				for(auto adj : pid.get_province_adjacency()) {
					if(adj.get_connected_provinces(0).id.index() >= ws.province_definitions.first_sea_province.index()) {
						return adj.get_connected_provinces(0).id;
					} else if(adj.get_connected_provinces(1).id.index() >= ws.province_definitions.first_sea_province.index()) {
						return adj.get_connected_provinces(1).id;
					}
				}
				return dcon::province_id();
			},
			primary_slot);
	*/
	auto ports = ws.world.province_get_port_to(to_prov(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(ports),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_cultural_union_scope) {
	auto cultures = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(group_holders),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_overlord_scope) {
	auto so = ws.world.nation_get_overlord_as_subject(to_nation(primary_slot));
	auto nso = ws.world.overlord_get_ruler(so);
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(nso), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_sphere_owner_scope) {
	auto nso = ws.world.nation_get_in_sphere_of(to_nation(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(nso), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_independence_scope) {
	auto rtags = ws.world.rebel_faction_get_defection_target(to_rebel(from_slot));
	auto r_holders = ws.world.national_identity_get_nation_from_identity_holder(rtags);

	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(r_holders),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_flashpoint_tag_scope) {
	auto ctags = ws.world.state_instance_get_flashpoint_tag(to_state(primary_slot));
	auto fp_nations = ws.world.national_identity_get_nation_from_identity_holder(ctags);
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(fp_nations),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_crisis_state_scope) {
	return apply_subtriggers<return_type, int32_t, this_type, from_type>(tval, ws, to_generic(ws.crisis_state), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_state_scope_province) {
	auto state_instance = ws.world.province_get_state_membership(to_prov(primary_slot));
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(state_instance),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_state_scope_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto state_instance = ws.world.province_get_state_membership(location);
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(state_instance),
			this_slot, from_slot);
}
TRIGGER_FUNCTION(tf_tag_scope) {
	auto tag = trigger::payload(tval[2]).tag_id;
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(tag);
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(tag_holder),
			this_slot, from_slot);
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
	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(owner), this_slot,
			from_slot);
}
TRIGGER_FUNCTION(tf_cultural_union_scope_pop) {
	auto cultures = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(cultures);
	auto union_tags = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	auto group_holders = ws.world.national_identity_get_nation_from_identity_holder(union_tags);

	return apply_subtriggers<return_type, gathered_t<primary_type>, this_type, from_type>(tval, ws, to_generic(group_holders),
			this_slot, from_slot);
}

//
// non scope trigger functions
//

TRIGGER_FUNCTION(tf_year) {
	return compare_values(tval[0], ws.current_date.to_ymd(ws.start_date).year, int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_month) {
	return compare_values(tval[0], ws.current_date.to_ymd(ws.start_date).month, int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_port) {
	return compare_to_true(tval[0], ws.world.province_get_is_coast(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_rank) {
	// note: comparison reversed since rank 1 is "greater" than rank 1 + N
	return compare_values(tval[0], int32_t(tval[1]), ws.world.nation_get_rank(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_technology) {
	auto tid = trigger::payload(tval[1]).tech_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_technologies(to_nation(primary_slot), tid));
}
TRIGGER_FUNCTION(tf_technology_province) {
	auto tid = trigger::payload(tval[1]).tech_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_technologies(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)), tid));
}
TRIGGER_FUNCTION(tf_technology_pop) {
	auto tid = trigger::payload(tval[1]).tech_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_technologies(nations::owner_of_pop(ws, to_pop(primary_slot)), tid));
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
	return compare_values(tval[0], ws.world.province_get_life_rating(to_prov(primary_slot)),
			trigger::payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_life_rating_state) {
	auto state_caps = ws.world.state_instance_get_capital(to_state(primary_slot));
	return compare_values(tval[0], ws.world.province_get_life_rating(state_caps), trigger::payload(tval[1]).signed_value);
}

auto empty_province_accumulator(sys::state const& ws) {
	return make_true_accumulator(
			[&ws, sea_index = ws.province_definitions.first_sea_province.index()](ve::tagged_vector<int32_t> v) {
				auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(v));
				return (owners == ve::tagged_vector<dcon::nation_id>()) & (ve::int_vector(v) < sea_index);
			});
}

TRIGGER_FUNCTION(tf_has_empty_adjacent_state_province) {
	auto results = ve::apply(
			[&ws](int32_t p_slot, int32_t, int32_t) {
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
			},
			primary_slot, this_slot, from_slot);

	return compare_to_true(tval[0], results);
}
auto empty_province_from_state_accumulator(sys::state const& ws, dcon::state_instance_id sid) {
	return make_true_accumulator([&ws, vector_sid = ve::tagged_vector<dcon::state_instance_id>(sid),
																	 sea_index = ws.province_definitions.first_sea_province.index()](ve::tagged_vector<int32_t> v) {
		auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(v));
		return (owners == ve::tagged_vector<dcon::nation_id>()) & (ve::int_vector(v) < sea_index) &
					 (ws.world.province_get_state_membership(to_prov(v)) != vector_sid);
	});
}

TRIGGER_FUNCTION(tf_has_empty_adjacent_state_state) {
	auto results = ve::apply(
			[&ws](int32_t p_slot, int32_t, int32_t) {
				auto state_id = to_state(p_slot);
				auto region_owner = ws.world.state_instance_get_nation_from_state_ownership(state_id);

				auto acc = empty_province_from_state_accumulator(ws, state_id);
				auto region_id = ws.world.state_instance_get_definition(state_id);

				for(auto sp : ws.world.state_definition_get_abstract_state_membership(region_id)) {
					if(sp.get_province().get_nation_from_province_ownership() == region_owner) {
						for(auto p : ws.world.province_get_province_adjacency(sp.get_province())) {
							auto other =
									p.get_connected_provinces(0) == sp.get_province() ? p.get_connected_provinces(1) : p.get_connected_provinces(0);
							acc.add_value(to_generic(other));
							if(acc.result)
								return true;
						}
					}
				}

				acc.flush();
				return acc.result;
			},
			primary_slot, this_slot, from_slot);

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
	return compare_values_eq(tval[0], ws.world.state_instance_get_definition(to_state(primary_slot)),
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
	return compare_values(tval[0], ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_state) {
	auto total_employable = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::employable);
	auto total_employed = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::employed);
	return compare_values(tval[0], ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_province) {
	auto total_employable = ws.world.province_get_demographics(to_prov(primary_slot), demographics::employable);
	auto total_employed = ws.world.province_get_demographics(to_prov(primary_slot), demographics::employed);
	return compare_values(tval[0], ve::select(total_employable > 0.0f, 1.0f - (total_employed / total_employable), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_pop) {
	auto employed = ws.world.pop_get_employment(to_pop(primary_slot));
	auto total_pop = ws.world.pop_get_size(to_pop(primary_slot));
	auto ptype = ws.world.pop_get_poptype(to_pop(primary_slot));
	return compare_values(tval[0], ve::select(ws.world.pop_type_get_has_unemployment(ptype), 1.0f - (employed / total_pop), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_slave_nation) {
	return compare_to_true(tval[0], (ws.world.nation_get_combined_issue_rules(to_nation(primary_slot)) &
																			issue_rule::slavery_allowed) == issue_rule::slavery_allowed);
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
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			dcon::nation_id());
}
TRIGGER_FUNCTION(tf_has_national_minority_province) {
	auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto pcultures = ws.world.nation_get_primary_culture(owners);
	auto prov_populations = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);

	auto majority_pop = ve::apply(
			[&ws](int32_t p_slot, dcon::culture_id c) {
				if(c)
					return ws.world.province_get_demographics(to_prov(p_slot), demographics::to_key(ws, c));
				else
					return 0.0f;
			},
			primary_slot, pcultures);

	return compare_to_false(tval[0], majority_pop == prov_populations);
}
TRIGGER_FUNCTION(tf_has_national_minority_state) {
	auto owners = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto pcultures = ws.world.nation_get_primary_culture(owners);
	auto populations = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);

	auto majority_pop = ve::apply(
			[&ws](int32_t p_slot, dcon::culture_id c) {
				if(c)
					return ws.world.state_instance_get_demographics(to_state(p_slot), demographics::to_key(ws, c));
				else
					return 0.0f;
			},
			primary_slot, pcultures);

	return compare_to_false(tval[0], majority_pop == populations);
}
TRIGGER_FUNCTION(tf_has_national_minority_nation) {
	auto pcultures = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto populations = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);

	auto majority_pop = ve::apply(
			[&ws](int32_t p_slot, dcon::culture_id c) {
				if(c)
					return ws.world.nation_get_demographics(to_nation(p_slot), demographics::to_key(ws, c));
				else
					return 0.0f;
			},
			primary_slot, pcultures);

	return compare_to_false(tval[0], majority_pop == populations);
}
TRIGGER_FUNCTION(tf_government_nation) {
	auto gov_type = ws.world.nation_get_government_type(to_nation(primary_slot));
	return compare_values_eq(tval[0], gov_type, trigger::payload(tval[1]).gov_id);
}
TRIGGER_FUNCTION(tf_government_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto owners = ws.world.province_get_nation_from_province_ownership(location);

	return tf_government_nation<return_type>(tval, ws, to_generic(owners), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_capital) {
	return compare_values_eq(tval[0], ws.world.nation_get_capital(to_nation(primary_slot)), payload(tval[1]).prov_id);
}
TRIGGER_FUNCTION(tf_tech_school) {
	return compare_values_eq(tval[0], ws.world.nation_get_tech_school(to_nation(primary_slot)), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_primary_culture) {
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_primary_culture_pop) {
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(nations::owner_of_pop(ws, to_pop(primary_slot))),
			trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_accepted_culture) {
	auto is_accepted = ws.world.nation_get_accepted_cultures(to_nation(primary_slot), trigger::payload(tval[1]).cul_id);
	return compare_to_true(tval[0], is_accepted);
}
TRIGGER_FUNCTION(tf_culture_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)), trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_culture_state) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_culture_province) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)),
			trigger::payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_culture_nation) {
	auto c = trigger::payload(tval[1]).cul_id;
	return compare_to_true(tval[0], nations::nation_accepts_culture(ws, to_nation(primary_slot), c));
}
TRIGGER_FUNCTION(tf_culture_pop_reb) {
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)),
			ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_state_reb) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_province_reb) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)),
			ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_nation_reb) {
	return compare_to_true(tval[0], nations::nation_accepts_culture(ws, to_nation(primary_slot),
																			ws.world.rebel_faction_get_primary_culture(to_rebel(from_slot))));
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
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)),
			trigger::payload(tval[1]).culgrp_id);
}
TRIGGER_FUNCTION(tf_culture_group_pop) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, trigger::payload(tval[1]).culgrp_id);
}
TRIGGER_FUNCTION(tf_culture_group_province) {
	auto pculture = ws.world.province_get_dominant_culture(to_prov(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, trigger::payload(tval[1]).culgrp_id);
}
TRIGGER_FUNCTION(tf_culture_group_state) {
	auto pculture = ws.world.state_instance_get_dominant_culture(to_state(primary_slot));
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
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)),
			nations::primary_culture_group(ws, to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_nation) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_nation_from_nation) {
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)),
			nations::primary_culture_group(ws, to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_pop_from_nation) {
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_culture_group_nation_this_province) {
	auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)),
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
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)),
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
	return compare_values_eq(tval[0], nations::primary_culture_group(ws, to_nation(primary_slot)),
			nations::primary_culture_group(ws, owner));
}
TRIGGER_FUNCTION(tf_culture_group_pop_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto pculture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cgroups = ws.world.culture_get_group_from_culture_group_membership(pculture);
	return compare_values_eq(tval[0], cgroups, nations::primary_culture_group(ws, owner));
}
TRIGGER_FUNCTION(tf_religion) {
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)), trigger::payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_religion_reb) {
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)),
			ws.world.rebel_faction_get_religion(to_rebel(from_slot)));
}
TRIGGER_FUNCTION(tf_religion_from_nation) {
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)),
			ws.world.nation_get_religion(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_religion_this_nation) {
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)),
			ws.world.nation_get_religion(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_religion_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)), ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_religion_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)), ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_religion_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)), ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_terrain_province) {
	return compare_values_eq(tval[0], ws.world.province_get_terrain(to_prov(primary_slot)), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_terrain_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.province_get_terrain(location), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_trade_goods) {
	return compare_values_eq(tval[0], ws.world.province_get_rgo(to_prov(primary_slot)), trigger::payload(tval[1]).com_id);
}

TRIGGER_FUNCTION(tf_is_secondary_power_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_rank(to_nation(primary_slot)) <= uint32_t(ws.defines.colonial_rank));
}
TRIGGER_FUNCTION(tf_is_secondary_power_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_rank(owner) <= uint32_t(ws.defines.colonial_rank));
}
TRIGGER_FUNCTION(tf_has_faction_nation) {
	auto rebel_type = trigger::payload(tval[1]).reb_id;
	auto result = ve::apply(
			[&ws, rebel_type](dcon::nation_id n) {
				for(auto factions : ws.world.nation_get_rebellion_within(n)) {
					if(factions.get_rebels().get_type() == rebel_type)
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_faction_pop) {
	auto rf = ws.world.pop_get_rebel_faction_from_pop_rebellion_membership(to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.rebel_faction_get_type(rf), trigger::payload(tval[1]).reb_id);
}
auto unowned_core_accumulator(sys::state const& ws, dcon::nation_id n) {
	return make_true_accumulator([&ws, n](ve::tagged_vector<int32_t> v) {
		auto owners = ws.world.province_get_nation_from_province_ownership(to_prov(v));
		return owners != n;
	});
}

TRIGGER_FUNCTION(tf_has_unclaimed_cores) {
	auto nation_tag = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::national_identity_id t) {
				auto acc = unowned_core_accumulator(ws, n);

				for(auto p : ws.world.national_identity_get_core(t)) {
					acc.add_value(to_generic(p.get_province().id));
					if(acc.result)
						return true;
				}
				acc.flush();
				return acc.result;
			},
			to_nation(primary_slot), nation_tag);

	return compare_to_true(tval[0], result);
}

TRIGGER_FUNCTION(tf_have_core_in_nation_tag) {
	auto h = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	return compare_to_true(tval[0], ve::apply([&](dcon::national_identity_id n) {
			for(auto p : ws.world.nation_get_province_ownership(h)) {
				if(ws.world.get_core_by_prov_tag_key(p.get_province(), n))
					return true;
			}
			return false;
		}, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot))));
}
TRIGGER_FUNCTION(tf_have_core_in_nation_this) {
	return compare_to_true(tval[0], ve::apply([&](dcon::national_identity_id n, dcon::nation_id h) {
		for(auto p : ws.world.nation_get_province_ownership(h)) {
			if(ws.world.get_core_by_prov_tag_key(p.get_province(), n))
				return true;
		}
		return false;
	}, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot)), trigger::to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_have_core_in_nation_from) {
	return compare_to_true(tval[0], ve::apply([&](dcon::national_identity_id n, dcon::nation_id h) {
		for(auto p : ws.world.nation_get_province_ownership(h)) {
			if(ws.world.get_core_by_prov_tag_key(p.get_province(), n))
				return true;
		}
		return false;
	}, ws.world.nation_get_identity_from_identity_holder(trigger::to_nation(primary_slot)), trigger::to_nation(from_slot)));
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

	auto ident = ws.world.nation_get_identity_from_identity_holder(nations::owner_of_pop(ws, to_pop(this_slot)));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);

	return compare_values_eq(tval[0], union_group, pgroup);
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_nation) {
	auto ident = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto union_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(ident);
	auto c = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(c);

	auto tident = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto tunion_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(tident);
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
	auto tunion_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(tident);
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
	auto tunion_group = ws.world.national_identity_get_culture_group_from_cultural_union_of(tident);
	auto tculture = ws.world.nation_get_primary_culture(tnation);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(tculture);

	return compare_to_true(tval[0], (union_group == tcg) || (tunion_group == cg));
}
TRIGGER_FUNCTION(tf_is_cultural_union_this_rebel) {
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
	return compare_values_eq(tval[0], ws.world.nation_get_combined_issue_rules(p_owner) & issue_rule::pop_build_factory,
			issue_rule::pop_build_factory);
}
TRIGGER_FUNCTION(tf_can_build_factory_pop) {
	auto p_owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_combined_issue_rules(p_owner) & issue_rule::pop_build_factory,
			issue_rule::pop_build_factory);
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
	return compare_values(tval[0], nations::central_blockaded_fraction(ws, to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_owns) {
	auto pid = payload(tval[1]).prov_id;
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(pid), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_owns_province) {
	auto pid = payload(tval[1]).prov_id;
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(pid), ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_controls) {
	auto pid = payload(tval[1]).prov_id;
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(pid), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_core_integer) {
	auto pid = payload(tval[1]).prov_id;
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot));
	auto result = ve::apply(
			[&ws, pid](dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_nation) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id pid, dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto tag = ws.world.nation_get_identity_from_identity_holder(owner);
	auto result = ve::apply(
			[&ws](dcon::province_id pid, dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto tag = ws.world.nation_get_identity_from_identity_holder(owner);
	auto result = ve::apply(
			[&ws](dcon::province_id pid, dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto tag = ws.world.nation_get_identity_from_identity_holder(owner);
	auto result = ve::apply(
			[&ws](dcon::province_id pid, dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_from_nation) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(from_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id pid, dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_reb) {
	auto rtags = ws.world.rebel_faction_get_defection_target(to_rebel(from_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id pid, dcon::national_identity_id t) {
				for(auto c : ws.world.province_get_core(pid)) {
					if(c.get_identity() == t)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), rtags);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_state_from_nation) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(from_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id sid, dcon::national_identity_id t) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(sid))) {
					if(!(ws.world.get_core_by_prov_tag_key(p.get_province(), t)))
						return false;
				}
				return true;
			},
			to_state(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_state_this_nation) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id sid, dcon::national_identity_id t) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(sid))) {
					if(!(ws.world.get_core_by_prov_tag_key(p.get_province(), t)))
						return false;
				}
				return true;
			},
			to_state(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_state_this_province) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(ws.world.province_get_nation_from_province_ownership(to_prov(this_slot)));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id sid, dcon::national_identity_id t) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(sid))) {
					if(!(ws.world.get_core_by_prov_tag_key(p.get_province(), t)))
						return false;
				}
				return true;
			},
			to_state(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_state_this_pop) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(nations::owner_of_pop(ws, to_pop(this_slot)));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id sid, dcon::national_identity_id t) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(sid))) {
					if(!(ws.world.get_core_by_prov_tag_key(p.get_province(), t)))
						return false;
				}
				return true;
			},
			to_state(primary_slot), tag);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_boolean) {
	return compare_to_true(tval[0], ws.world.province_get_is_owner_core(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_core_tag) {
	auto result = ve::apply(
			[&ws, t = trigger::payload(tval[1]).tag_id](dcon::province_id pid) {
				return bool(ws.world.get_core_by_prov_tag_key(pid, t));
			},
			to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_pop_tag) {
	auto result = ve::apply(
			[&ws, t = trigger::payload(tval[1]).tag_id](dcon::province_id pid) {
				return bool(ws.world.get_core_by_prov_tag_key(pid, t));
			},
			ws.world.pop_get_province_from_pop_location(to_pop(primary_slot)));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_core_state_tag) {
	auto result = ve::apply(
			[&ws, t = trigger::payload(tval[1]).tag_id](dcon::state_instance_id sid) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(sid))) {
					if(!(ws.world.get_core_by_prov_tag_key(p.get_province(), t)))
						return false;
				}
				return true;
			},
			to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_num_of_revolts) {
	return compare_values(tval[0], ws.world.nation_get_rebel_controlled_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_revolt_percentage) {
	return compare_values(tval[0], nations::central_reb_controlled_fraction(ws, to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_num_of_cities_int) {
	return compare_values(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_cities_from_nation) {
	return compare_values(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
			ws.world.nation_get_owned_province_count(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_nation) {
	return compare_values(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
			ws.world.nation_get_owned_province_count(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
			ws.world.nation_get_owned_province_count(owner));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
			ws.world.nation_get_owned_province_count(owner));
}
TRIGGER_FUNCTION(tf_num_of_cities_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)),
			ws.world.nation_get_owned_province_count(owner));
}
TRIGGER_FUNCTION(tf_num_of_ports) {
	return compare_values(tval[0], ws.world.nation_get_central_ports(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_allies) {
	return compare_values(tval[0], ws.world.nation_get_allies_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_vassals) {
	return compare_values(tval[0], ws.world.nation_get_vassals_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_owned_by_tag) {
	auto holders = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], holders, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_nation) {
	return compare_values_eq(tval[0], to_nation(this_slot),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_from_nation) {
	return compare_values_eq(tval[0], to_nation(from_slot),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_province) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(to_prov(this_slot)),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], owner, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], owner, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_state_tag) {
	auto holders = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], holders, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_state_this_nation) {
	return compare_values_eq(tval[0], to_nation(this_slot),
			ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_state_from_nation) {
	return compare_values_eq(tval[0], to_nation(from_slot),
			ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_state_this_province) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(to_prov(this_slot)),
			ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_state_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], owner, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_owned_by_state_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], owner, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_exists_bool) {
	return compare_to_true(tval[0], ws.world.nation_get_owned_province_count(to_nation(primary_slot)) != 0);
}
TRIGGER_FUNCTION(tf_exists_tag) {
	auto holders = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	return compare_to_true(tval[0], ws.world.nation_get_owned_province_count(holders) != 0);
}
TRIGGER_FUNCTION(tf_has_country_flag) {
	return compare_to_true(tval[0], ws.world.nation_get_flag_variables(to_nation(primary_slot), payload(tval[1]).natf_id));
}
TRIGGER_FUNCTION(tf_has_country_flag_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_flag_variables(owner, payload(tval[1]).natf_id));
}
TRIGGER_FUNCTION(tf_has_country_flag_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_flag_variables(owner, payload(tval[1]).natf_id));
}
TRIGGER_FUNCTION(tf_has_country_flag_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_flag_variables(owner, payload(tval[1]).natf_id));
}
TRIGGER_FUNCTION(tf_continent_province) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(to_prov(primary_slot)), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_continent_state) {
	auto state_caps = ws.world.state_instance_get_capital(to_state(primary_slot));
	return compare_values_eq(tval[0], ws.world.province_get_continent(state_caps), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_continent_nation) {
	auto nat_caps = ws.world.nation_get_capital(to_nation(primary_slot));
	return compare_values_eq(tval[0], ws.world.province_get_continent(nat_caps), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_continent_pop) {
	auto prov_id = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.province_get_continent(prov_id), trigger::payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_continent_nation_this) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(primary_slot))),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_continent_state_this) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(ws.world.state_instance_get_capital(to_state(primary_slot))),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_continent_province_this) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(to_prov(primary_slot)),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_continent_pop_this) {
	return compare_values_eq(tval[0],
			ws.world.province_get_continent(ws.world.pop_get_province_from_pop_location(to_pop(primary_slot))),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_continent_nation_from) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(primary_slot))),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(from_slot))));
}
TRIGGER_FUNCTION(tf_continent_state_from) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(ws.world.state_instance_get_capital(to_state(primary_slot))),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(from_slot))));
}
TRIGGER_FUNCTION(tf_continent_province_from) {
	return compare_values_eq(tval[0], ws.world.province_get_continent(to_prov(primary_slot)),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(from_slot))));
}
TRIGGER_FUNCTION(tf_continent_pop_from) {
	return compare_values_eq(tval[0],
			ws.world.province_get_continent(ws.world.pop_get_province_from_pop_location(to_pop(primary_slot))),
			ws.world.province_get_continent(ws.world.nation_get_capital(to_nation(from_slot))));
}
TRIGGER_FUNCTION(tf_casus_belli_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	auto result = ve::apply([&ws, tag_holder](dcon::nation_id n) { return military::can_use_cb_against(ws, n, tag_holder); },
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_casus_belli_from) {
	auto result =
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id target) { return military::can_use_cb_against(ws, n, target); },
					to_nation(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_casus_belli_this_nation) {
	auto result =
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id target) { return military::can_use_cb_against(ws, n, target); },
					to_nation(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_casus_belli_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result =
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id target) { return military::can_use_cb_against(ws, n, target); },
					to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_casus_belli_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result =
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id target) { return military::can_use_cb_against(ws, n, target); },
					to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_casus_belli_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result =
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id target) { return military::can_use_cb_against(ws, n, target); },
					to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_military_access_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	auto result = ve::apply(
			[&ws, holder](dcon::nation_id n) {
				return ws.world.unilateral_relationship_get_military_access(
						ws.world.get_unilateral_relationship_by_unilateral_pair(holder, n));
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_military_access_from) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id t) {
				return ws.world.unilateral_relationship_get_military_access(
						ws.world.get_unilateral_relationship_by_unilateral_pair(t, n));
			},
			to_nation(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_military_access_this_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id t) {
				return ws.world.unilateral_relationship_get_military_access(
						ws.world.get_unilateral_relationship_by_unilateral_pair(t, n));
			},
			to_nation(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_military_access_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id t) {
				return ws.world.unilateral_relationship_get_military_access(
						ws.world.get_unilateral_relationship_by_unilateral_pair(t, n));
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_military_access_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id t) {
				return ws.world.unilateral_relationship_get_military_access(
						ws.world.get_unilateral_relationship_by_unilateral_pair(t, n));
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_military_access_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id t) {
				return ws.world.unilateral_relationship_get_military_access(
						ws.world.get_unilateral_relationship_by_unilateral_pair(t, n));
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_prestige_value) {
	return compare_values(tval[0], ws.world.nation_get_prestige(to_nation(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_prestige_from) {
	return compare_values(tval[0], ws.world.nation_get_prestige(to_nation(primary_slot)),
			ws.world.nation_get_prestige(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_prestige_this_nation) {
	return compare_values(tval[0], ws.world.nation_get_prestige(to_nation(primary_slot)),
			ws.world.nation_get_prestige(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_prestige_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values(tval[0], ws.world.nation_get_prestige(to_nation(primary_slot)), ws.world.nation_get_prestige(owner));
}
TRIGGER_FUNCTION(tf_prestige_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values(tval[0], ws.world.nation_get_prestige(to_nation(primary_slot)), ws.world.nation_get_prestige(owner));
}
TRIGGER_FUNCTION(tf_prestige_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values(tval[0], ws.world.nation_get_prestige(to_nation(primary_slot)), ws.world.nation_get_prestige(owner));
}
TRIGGER_FUNCTION(tf_badboy) {
	return compare_values(tval[0], ws.world.nation_get_infamy(to_nation(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_has_building_fort) {
	return compare_to_true(tval[0], ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::fort)) != 0);
}
TRIGGER_FUNCTION(tf_has_building_railroad) {
	return compare_to_true(tval[0], ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::railroad)) != 0);
}
TRIGGER_FUNCTION(tf_has_building_naval_base) {
	return compare_to_true(tval[0], ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) != 0);
}

TRIGGER_FUNCTION(tf_has_building_factory) {
	auto result = ve::apply([&ws](dcon::state_instance_id s) { return economy::has_factory(ws, s); }, to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_building_state) {
	auto result =
			ve::apply([&ws, f = payload(tval[1]).fac_id](dcon::state_instance_id s) { return economy::has_building(ws, s, f); },
					to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_building_state_from_province) {
	auto state = ws.world.province_get_state_membership(to_prov(primary_slot));
	auto result =
			ve::apply([&ws, f = payload(tval[1]).fac_id](dcon::state_instance_id s) { return economy::has_building(ws, s, f); }, state);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_building_factory_from_province) {
	auto state = ws.world.province_get_state_membership(to_prov(primary_slot));
	auto result = ve::apply([&ws](dcon::state_instance_id s) { return economy::has_factory(ws, s); }, state);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_empty) {
	return compare_to_true(tval[0],
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)) == dcon::nation_id());
}
TRIGGER_FUNCTION(tf_empty_state) {
	return compare_to_true(tval[0], to_state(primary_slot) == dcon::state_instance_id());
}
TRIGGER_FUNCTION(tf_is_blockaded) {
	return compare_to_true(tval[0], military::province_is_blockaded(ws, to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_has_country_modifier) {
	auto const mod = trigger::payload(tval[1]).mod_id;
	auto result = ve::apply(
			[&ws, mod](dcon::nation_id n) {
				for(auto m : ws.world.nation_get_current_modifiers(n)) {
					if(m.mod_id == mod)
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_country_modifier_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto const mod = trigger::payload(tval[1]).mod_id;
	auto result = ve::apply(
			[&ws, mod](dcon::nation_id n) {
				for(auto m : ws.world.nation_get_current_modifiers(n)) {
					if(m.mod_id == mod)
						return true;
				}
				return false;
			},
			owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_province_modifier) {
	auto const mod = trigger::payload(tval[1]).mod_id;
	auto result = ve::apply(
			[&ws, mod](dcon::province_id n) {
				for(auto m : ws.world.province_get_current_modifiers(n)) {
					if(m.mod_id == mod)
						return true;
				}
				return false;
			},
			to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_region) {
	return compare_values_eq(tval[0], ws.world.province_get_state_from_abstract_state_membership(to_prov(primary_slot)),
			trigger::payload(tval[1]).state_id);
}
TRIGGER_FUNCTION(tf_region_state) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_definition(to_state(primary_slot)),
			trigger::payload(tval[1]).state_id);
}
TRIGGER_FUNCTION(tf_region_pop) {
	return compare_values_eq(tval[0], ws.world.province_get_state_from_abstract_state_membership(ws.world.pop_get_province_from_pop_location(to_pop(primary_slot))), trigger::payload(tval[1]).state_id);
}


TRIGGER_FUNCTION(tf_region_proper) {
	auto r = trigger::payload(tval[1]).reg_id;
	auto result = ve::apply([&ws, r](dcon::province_id p) {
		for(auto m : ws.world.province_get_region_membership(p)) {
			if(m.get_region() == r)
				return true;
		}
		return false;
	}, to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_region_proper_state) {
	auto r = trigger::payload(tval[1]).reg_id;
	auto result = ve::apply([&ws, r](dcon::province_id p) {
		for(auto m : ws.world.province_get_region_membership(p)) {
			if(m.get_region() == r)
				return true;
		}
		return false;
	}, ws.world.state_instance_get_capital(to_state(primary_slot)));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_region_proper_pop) {
	auto r = trigger::payload(tval[1]).reg_id;
	auto result = ve::apply([&ws, r](dcon::province_id p) {
		for(auto m : ws.world.province_get_region_membership(p)) {
			if(m.get_region() == r)
				return true;
		}
		return false;
	}, ws.world.pop_get_province_from_pop_location(to_pop(primary_slot)));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_owns_region_proper) {
	auto result = ve::apply([&](dcon::region_id sd, dcon::nation_id n) {
		for(auto p : ws.world.region_get_region_membership(sd)) {
			if(p.get_province().get_nation_from_province_ownership() != n)
				return false;
		}
		return true;
	}, trigger::payload(tval[1]).reg_id, trigger::to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_owns_region) {
	auto result = ve::apply([&](dcon::state_definition_id sd, dcon::nation_id n) {
		for(auto p : ws.world.state_definition_get_abstract_state_membership(sd)) {
			if(p.get_province().get_nation_from_province_ownership() != n)
				return false;
		}
		return true;
	}, trigger::payload(tval[1]).state_id, trigger::to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_tag_tag) {
	return compare_values_eq(tval[0], ws.world.nation_get_identity_from_identity_holder(to_nation(primary_slot)),
			trigger::payload(tval[1]).tag_id);
}
TRIGGER_FUNCTION(tf_tag_this_nation) {
	return compare_values_eq(tval[0], to_nation(primary_slot), to_nation(this_slot));
}
TRIGGER_FUNCTION(tf_tag_this_province) {
	return compare_values_eq(tval[0], to_nation(primary_slot),
			ws.world.province_get_nation_from_province_ownership(to_prov(this_slot)));
}
TRIGGER_FUNCTION(tf_tag_from_nation) {
	return compare_values_eq(tval[0], to_nation(primary_slot), to_nation(from_slot));
}
TRIGGER_FUNCTION(tf_tag_from_province) {
	return compare_values_eq(tval[0], to_nation(primary_slot),
			ws.world.province_get_nation_from_province_ownership(to_prov(from_slot)));
}
TRIGGER_FUNCTION(tf_tag_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_identity_from_identity_holder(owner), trigger::payload(tval[1]).tag_id);
}
TRIGGER_FUNCTION(tf_stronger_army_than_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);

	auto main_brigades = ws.world.nation_get_active_regiments(to_nation(primary_slot));
	auto this_brigades = ws.world.nation_get_active_regiments(tag_holder);
	return compare_values(tval[0], main_brigades, this_brigades);
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_nation) {
	auto main_brigades = ws.world.nation_get_active_regiments(to_nation(primary_slot));
	auto this_brigades = ws.world.nation_get_active_regiments(trigger::to_nation(this_slot));
	return compare_values(tval[0], main_brigades, this_brigades);
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(trigger::to_state(this_slot));
	return tf_stronger_army_than_this_nation<return_type>(tval, ws, primary_slot, to_generic(owner), int32_t());
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(trigger::to_prov(this_slot));
	return tf_stronger_army_than_this_nation<return_type>(tval, ws, primary_slot, to_generic(owner), int32_t());
}
TRIGGER_FUNCTION(tf_stronger_army_than_this_pop) {
	auto owner = nations::owner_of_pop(ws, trigger::to_pop(this_slot));
	return tf_stronger_army_than_this_nation<return_type>(tval, ws, primary_slot, to_generic(owner), int32_t());
}
TRIGGER_FUNCTION(tf_stronger_army_than_from_nation) {
	return tf_stronger_army_than_this_nation<return_type>(tval, ws, primary_slot, from_slot, int32_t());
}
TRIGGER_FUNCTION(tf_stronger_army_than_from_province) {
	return tf_stronger_army_than_this_province<return_type>(tval, ws, primary_slot, from_slot, int32_t());
}
TRIGGER_FUNCTION(tf_neighbour_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	auto result =
			ve::apply([&ws, tag_holder](
										dcon::nation_id n) { return bool(ws.world.get_nation_adjacency_by_nation_adjacency_pair(n, tag_holder)); },
					to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_neighbour_this) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id o) { return bool(ws.world.get_nation_adjacency_by_nation_adjacency_pair(n, o)); },
			to_nation(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_neighbour_from) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id o) { return bool(ws.world.get_nation_adjacency_by_nation_adjacency_pair(n, o)); },
			to_nation(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_neighbour_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id o) { return bool(ws.world.get_nation_adjacency_by_nation_adjacency_pair(n, o)); },
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_neighbour_from_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(from_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id o) { return bool(ws.world.get_nation_adjacency_by_nation_adjacency_pair(n, o)); },
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_country_units_in_state_from) {
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::nation_id tag) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(s))) {
					if(p.get_province().get_state_membership() == s) {
						for(auto a : ws.world.province_get_army_location(p.get_province())) {
							if(a.get_army().get_controller_from_army_control() == tag)
								return true;
						}
					}
				}
				return false;
	},
	to_state(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_nation) {
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::nation_id tag) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(s))) {
					if(p.get_province().get_state_membership() == s) {
						for(auto a : ws.world.province_get_army_location(p.get_province())) {
							if(a.get_army().get_controller_from_army_control() == tag)
								return true;
						}
					}
				}
				return false;
	},
	to_state(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::nation_id tag) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(s))) {
					if(p.get_province().get_state_membership() == s) {
						for(auto a : ws.world.province_get_army_location(p.get_province())) {
							if(a.get_army().get_controller_from_army_control() == tag)
								return true;
						}
					}
				}
				return false;
	},
	to_state(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::nation_id tag) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(s))) {
					if(p.get_province().get_state_membership() == s) {
						for(auto a : ws.world.province_get_army_location(p.get_province())) {
							if(a.get_army().get_controller_from_army_control() == tag)
								return true;
						}
					}
				}
				return false;
	},
	to_state(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_country_units_in_state_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::nation_id tag) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(s))) {
					if(p.get_province().get_state_membership() == s) {
						for(auto a : ws.world.province_get_army_location(p.get_province())) {
							if(a.get_army().get_controller_from_army_control() == tag)
								return true;
						}
					}
				}
				return false;
	},
	to_state(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_country_units_in_state_tag) {
	auto tag = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	auto result = ve::apply(
			[&ws, tag](dcon::state_instance_id s) {
				if(!tag)
					return false;
				for(auto p : ws.world.state_definition_get_abstract_state_membership(ws.world.state_instance_get_definition(s))) {
					if(p.get_province().get_state_membership() == s) {
						for(auto a : ws.world.province_get_army_location(p.get_province())) {
							if(a.get_army().get_controller_from_army_control() == tag)
								return true;
						}
					}
				}
				return false;
		},
	to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_units_in_province_value) {
	auto result = ve::apply(
			[&ws](dcon::province_id p) {
				int32_t total = 0;
				for(auto a : ws.world.province_get_army_location(p)) {
					for(auto u : a.get_army().get_army_membership()) {
						++total;
					}
				}
				return total;
			},
			to_prov(primary_slot));
	return compare_values(tval[0], result, int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_units_in_province_tag) {
	auto tag = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	auto result = ve::apply(
			[&ws, tag](dcon::province_id p) {
				if(!tag)
					return false;
				for(auto a : ws.world.province_get_army_location(p)) {
					if(a.get_army().get_controller_from_army_control() == tag)
						return true;
				}
				return false;
			},
			to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}

TRIGGER_FUNCTION(tf_units_in_province_from) {
	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::nation_id n) {
				for(auto a : ws.world.province_get_army_location(p)) {
					if(a.get_army().get_controller_from_army_control() == n)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_units_in_province_this_nation) {
	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::nation_id n) {
				for(auto a : ws.world.province_get_army_location(p)) {
					if(a.get_army().get_controller_from_army_control() == n)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_units_in_province_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::nation_id n) {
				for(auto a : ws.world.province_get_army_location(p)) {
					if(a.get_army().get_controller_from_army_control() == n)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_units_in_province_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::nation_id n) {
				for(auto a : ws.world.province_get_army_location(p)) {
					if(a.get_army().get_controller_from_army_control() == n)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_units_in_province_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::nation_id n) {
				for(auto a : ws.world.province_get_army_location(p)) {
					if(a.get_army().get_controller_from_army_control() == n)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_war_with_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(trigger::payload(tval[1]).tag_id);
	auto result =
			ve::apply([&ws, holder](dcon::nation_id a) { return military::are_at_war(ws, holder, a); }, to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_war_with_from) {
	auto result = ve::apply([&ws](dcon::nation_id a, dcon::nation_id b) { return military::are_at_war(ws, a, b); },
			to_nation(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_war_with_this_nation) {
	auto result = ve::apply([&ws](dcon::nation_id a, dcon::nation_id b) { return military::are_at_war(ws, a, b); },
			to_nation(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_war_with_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply([&ws](dcon::nation_id a, dcon::nation_id b) { return military::are_at_war(ws, a, b); },
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_war_with_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result = ve::apply([&ws](dcon::nation_id a, dcon::nation_id b) { return military::are_at_war(ws, a, b); },
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_war_with_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result = ve::apply([&ws](dcon::nation_id a, dcon::nation_id b) { return military::are_at_war(ws, a, b); },
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_unit_in_battle) {
	return compare_to_true(tval[0], military::battle_is_ongoing_in_province(ws, to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_unit_has_leader) {
	auto result = ve::apply([&ws](dcon::province_id p) {
		for(const auto ar : ws.world.province_get_army_location(p)) {
			if(ws.world.army_leadership_get_general(ws.world.army_get_army_leadership(ws.world.army_location_get_army(ar)))) {
				return true;
			}
		}
		return false;
	}, to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_national_focus_state) {
	auto result = ve::apply([&ws, tval](dcon::state_instance_id p) {
		return ws.world.state_instance_get_owner_focus(p) == trigger::payload(tval[1]).nf_id;
	}, to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_national_focus_province) {
	auto result = ve::apply([&ws, tval](dcon::province_id p) {
		return ws.world.state_instance_get_owner_focus(ws.world.province_get_state_membership(p)) == trigger::payload(tval[1]).nf_id;
	}, to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_total_amount_of_divisions) {
	return compare_values(tval[0], ws.world.nation_get_active_regiments(to_nation(primary_slot)), int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_money) {
	return compare_values(tval[0], ws.world.nation_get_stockpiles(to_nation(primary_slot), economy::money),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_money_province) {
	return compare_values(tval[0], ws.world.nation_get_stockpiles(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)), economy::money),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_lost_national) {
	return compare_values(tval[0], 1.0f - ws.world.nation_get_revanchism(to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_vassal) {
	return compare_to_true(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))) != dcon::nation_id());
}
TRIGGER_FUNCTION(tf_ruling_party_ideology_nation) {
	auto rp = ws.world.nation_get_ruling_party(to_nation(primary_slot));
	return compare_values_eq(tval[0], ws.world.political_party_get_ideology(rp), trigger::payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_ruling_party_ideology_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto rp = ws.world.nation_get_ruling_party(owner);
	return compare_values_eq(tval[0], ws.world.political_party_get_ideology(rp), trigger::payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_ruling_party_ideology_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto rp = ws.world.nation_get_ruling_party(owner);
	return compare_values_eq(tval[0], ws.world.political_party_get_ideology(rp), trigger::payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_ruling_party) {
	auto rp = ws.world.nation_get_ruling_party(to_nation(primary_slot));
	dcon::text_key name{dcon::text_key::value_base_t(read_int32_t_from_payload(tval + 1)) };
	return compare_values_eq(tval[0], ws.world.political_party_get_name(rp), name);
}
TRIGGER_FUNCTION(tf_is_ideology_enabled) {
	return compare_to_true(tval[0], ws.world.ideology_get_enabled(trigger::payload(tval[1]).ideo_id));
}
TRIGGER_FUNCTION(tf_political_reform_want_nation) {
	return compare_values(tval[0],
			ws.world.nation_get_demographics(to_nation(primary_slot), demographics::political_reform_desire) *
					ws.defines.movement_support_uh_factor / ws.world.nation_get_non_colonial_population(to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_political_reform_want_pop) {
	return compare_values(tval[0], ws.world.pop_get_political_reform_desire(to_pop(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_social_reform_want_nation) {
	return compare_values(tval[0],
			ws.world.nation_get_demographics(to_nation(primary_slot), demographics::social_reform_desire) *
					ws.defines.movement_support_uh_factor / ws.world.nation_get_non_colonial_population(to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_social_reform_want_pop) {
	return compare_values(tval[0], ws.world.pop_get_social_reform_desire(to_pop(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_total_amount_of_ships) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n) {
				int32_t total = 0;
				for(auto a : ws.world.nation_get_navy_control(n)) {
					auto memb = a.get_navy().get_navy_membership();
					total += int32_t(memb.end() - memb.begin());
				}
				return total;
			},
			to_nation(primary_slot));
	return compare_values(tval[0], result, int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_plurality) {
	return compare_values(tval[0], ws.world.nation_get_plurality(to_nation(primary_slot)), read_float_from_payload(tval + 1) / 100.0f);
}
TRIGGER_FUNCTION(tf_plurality_pop) {
	return compare_values(tval[0], ws.world.nation_get_plurality(nations::owner_of_pop(ws, to_pop(primary_slot))), read_float_from_payload(tval + 1) / 100.0f);
}
TRIGGER_FUNCTION(tf_corruption) {
	return compare_values(tval[0], nations::central_has_crime_fraction(ws, to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_state_religion_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(owner), ws.world.pop_get_religion(to_pop(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_state_religion_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(owner),
			ws.world.province_get_dominant_religion(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_state_religion_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(owner),
			ws.world.state_instance_get_dominant_religion(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(owner), ws.world.pop_get_culture(to_pop(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(owner),
			ws.world.province_get_dominant_culture(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(owner),
			ws.world.state_instance_get_dominant_culture(to_state(primary_slot)));
}
TRIGGER_FUNCTION(tf_primary_culture_from_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_primary_culture_from_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(from_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_pop) {
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			ws.world.pop_get_culture(to_pop(this_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_nation_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_primary_culture(to_nation(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_nation) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_pop) {
	auto this_owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_state_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_nation) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_pop) {
	auto this_owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_province_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_nation) {
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_pop) {
	auto this_owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}
TRIGGER_FUNCTION(tf_is_primary_culture_pop_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner));
}

template<typename N, typename C>
auto internal_tf_culture_accepted(sys::state& ws, N nids, C cids) {
	return ve::apply(
		[&ws](dcon::nation_id n, dcon::culture_id c) {
			return ws.world.nation_get_accepted_cultures(n, c);
		}, nids, cids);
}

TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_nation) {
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, to_nation(primary_slot), ws.world.nation_get_primary_culture(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_pop) {
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, to_nation(primary_slot), ws.world.pop_get_culture(to_pop(this_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, to_nation(primary_slot), ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_nation_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, to_nation(primary_slot), ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_nation) {
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_pop) {
	auto this_owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_nation) {
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(to_nation(this_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_pop) {
	auto this_owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)),
			ws.world.nation_get_primary_culture(this_owner)));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_nation) {
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, to_nation(this_slot), ws.world.pop_get_culture(to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_pop) {
	auto this_owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, this_owner, ws.world.pop_get_culture(to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_state) {
	auto this_owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, this_owner, ws.world.pop_get_culture(to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_is_accepted_culture_pop_this_province) {
	auto this_owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_to_true(tval[0], internal_tf_culture_accepted(ws, this_owner, ws.world.pop_get_culture(to_pop(primary_slot))));
}

TRIGGER_FUNCTION(tf_is_accepted_culture_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto is_accepted = ve::apply(
			[&ws](dcon::nation_id n, dcon::culture_id c) {
				if(n)
					return ws.world.nation_get_accepted_cultures(n, c);
				else
					return false;
			},
			owner, ws.world.pop_get_culture(to_pop(primary_slot)));
	return compare_to_true(tval[0], is_accepted);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto is_accepted = ve::apply(
			[&ws](dcon::nation_id n, dcon::culture_id c) {
				if(n)
					return ws.world.nation_get_accepted_cultures(n, c);
				else
					return false;
			},
			owner, ws.world.province_get_dominant_culture(to_prov(primary_slot)));
	return compare_to_true(tval[0], is_accepted);
}
TRIGGER_FUNCTION(tf_is_accepted_culture_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto is_accepted = ve::apply(
			[&ws](dcon::nation_id n, dcon::culture_id c) {
				if(n)
					return ws.world.nation_get_accepted_cultures(n, c);
				else
					return false;
			},
			owner, ws.world.state_instance_get_dominant_culture(to_state(primary_slot)));
	return compare_to_true(tval[0], is_accepted);
}
TRIGGER_FUNCTION(tf_is_coastal_province) {
	return compare_to_true(tval[0], ws.world.province_get_is_coast(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_coastal_state) {
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s) {
				return province::state_is_coastal(ws, s);
			},
			to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_in_sphere_tag) {
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)),
			ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id));
}
TRIGGER_FUNCTION(tf_in_sphere_from) {
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)), to_nation(from_slot));
}
TRIGGER_FUNCTION(tf_in_sphere_this_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)), to_nation(this_slot));
}
TRIGGER_FUNCTION(tf_in_sphere_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)), owner);
}
TRIGGER_FUNCTION(tf_in_sphere_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)), owner);
}
TRIGGER_FUNCTION(tf_in_sphere_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)), owner);
}
TRIGGER_FUNCTION(tf_produces_nation) {
	auto good = payload(tval[1]).com_id;
	return compare_to_true(tval[0], ws.world.nation_get_domestic_market_pool(to_nation(primary_slot), good) > 0.0f);
}
TRIGGER_FUNCTION(tf_produces_province) {
	/* return compare_to_true(tval[0],
			(ws.world.province_get_rgo(to_prov(primary_slot)) == payload(tval[1]).com_id) ||
					(ws.world.province_get_artisan_production(to_prov(primary_slot)) == payload(tval[1]).com_id)); */
	return compare_to_true(tval[0], ws.world.province_get_rgo(to_prov(primary_slot)) == payload(tval[1]).com_id);
}
TRIGGER_FUNCTION(tf_produces_state) {
	auto good = payload(tval[1]).com_id;
	return compare_to_true(tval[0],
			ve::apply(
					[&](dcon::state_instance_id si, dcon::nation_id o) {
						auto d = ws.world.state_instance_get_definition(si);
						for(auto p : ws.world.state_definition_get_abstract_state_membership(d)) {
							if(p.get_province().get_nation_from_province_ownership() == o) {
								if(p.get_province().get_rgo() == good)
									return true;
								//if(p.get_province().get_artisan_production() == good)
								//	return true;

								for(auto f : p.get_province().get_factory_location()) {
									if(f.get_factory().get_building_type().get_output() == good)
										return true;
								}
							}
						}
						return false;
					},
					to_state(primary_slot), ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot))));
}
TRIGGER_FUNCTION(tf_produces_pop) {
	auto pop_location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto good = payload(tval[1]).com_id;

	/*return compare_to_true(tval[0], (ws.world.pop_get_poptype(to_pop(primary_slot)) == ws.culture_definitions.artisans) &&
																			(ws.world.province_get_artisan_production(pop_location) == good)); */

	//return compare_to_true(tval[0], (ws.world.pop_get_poptype(to_pop(primary_slot)).get_is_paid_rgo_worker()) && (ws.world.province_get_rgo(pop_location) == good));
	return compare_to_true(tval[0], false);
}
TRIGGER_FUNCTION(tf_average_militancy_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	auto mil_amount = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::militancy);
	return compare_values(tval[0], ve::select(total_pop > 0, mil_amount / total_pop, 0.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_average_militancy_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	auto mil_amount = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::militancy);
	return compare_values(tval[0], ve::select(total_pop > 0, mil_amount / total_pop, 0.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_average_militancy_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	auto mil_amount = ws.world.province_get_demographics(to_prov(primary_slot), demographics::militancy);
	return compare_values(tval[0], ve::select(total_pop > 0, mil_amount / total_pop, 0.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_average_consciousness_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	auto mil_amount = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::consciousness);
	return compare_values(tval[0], ve::select(total_pop > 0, mil_amount / total_pop, 0.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_average_consciousness_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	auto mil_amount = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::consciousness);
	return compare_values(tval[0], ve::select(total_pop > 0, mil_amount / total_pop, 0.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_average_consciousness_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	auto mil_amount = ws.world.province_get_demographics(to_prov(primary_slot), demographics::consciousness);
	return compare_values(tval[0], ve::select(total_pop > 0, mil_amount / total_pop, 0.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_next_reform_nation) {
	auto ref_id = payload(tval[1]).opt_id;
	auto prior_opt = dcon::issue_option_id(dcon::issue_option_id::value_base_t(ref_id.index() - 1));
	auto reform_parent = ws.world.issue_option_get_parent_issue(ref_id);
	auto active_option = ws.world.nation_get_issues(to_nation(primary_slot), reform_parent);

	return compare_values_eq(tval[0], active_option, prior_opt);
}
TRIGGER_FUNCTION(tf_is_next_reform_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));

	auto ref_id = payload(tval[1]).opt_id;
	auto prior_opt = dcon::issue_option_id(dcon::issue_option_id::value_base_t(ref_id.index() - 1));
	auto reform_parent = ws.world.issue_option_get_parent_issue(ref_id);
	auto active_option = ws.world.nation_get_issues(owner, reform_parent);

	return compare_values_eq(tval[0], active_option, prior_opt);
}
TRIGGER_FUNCTION(tf_is_next_rreform_nation) {
	auto ref_id = payload(tval[1]).ropt_id;
	auto prior_opt = dcon::reform_option_id(dcon::reform_option_id::value_base_t(ref_id.index() - 1));
	auto reform_parent = ws.world.reform_option_get_parent_reform(ref_id);
	auto active_option = ws.world.nation_get_reforms(to_nation(primary_slot), reform_parent);

	return compare_values_eq(tval[0], active_option, prior_opt);
}
TRIGGER_FUNCTION(tf_is_next_rreform_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));

	auto ref_id = payload(tval[1]).ropt_id;
	auto prior_opt = dcon::reform_option_id(dcon::reform_option_id::value_base_t(ref_id.index() - 1));
	auto reform_parent = ws.world.reform_option_get_parent_reform(ref_id);
	auto active_option = ws.world.nation_get_reforms(owner, reform_parent);

	return compare_values_eq(tval[0], active_option, prior_opt);
}
TRIGGER_FUNCTION(tf_rebel_power_fraction) {
	// note: virtually unused
	return compare_to_true(tval[0], false);
}
TRIGGER_FUNCTION(tf_recruited_percentage_nation) {
	auto value = ve::apply([&ws](dcon::nation_id n) { return military::recruited_pop_fraction(ws, n); }, to_nation(primary_slot));
	return compare_values(tval[0], value, read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_recruited_percentage_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto value = ve::apply([&ws](dcon::nation_id n) { return military::recruited_pop_fraction(ws, n); }, owner);
	return compare_values(tval[0], value, read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_has_culture_core) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto culture = ws.world.pop_get_culture(to_pop(primary_slot));

	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::culture_id c) {
				for(auto co : ws.world.province_get_core(p)) {
					if(co.get_identity().get_primary_culture() == c)
						return true;
				}
				return false;
			},
			location, culture);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_culture_core_province_this_pop) {
	auto culture = ws.world.pop_get_culture(to_pop(this_slot));

	auto result = ve::apply(
			[&ws](dcon::province_id p, dcon::culture_id c) {
				for(auto co : ws.world.province_get_core(p)) {
					if(co.get_identity().get_primary_culture() == c)
						return true;
				}
				return false;
			},
			to_prov(primary_slot), culture);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_nationalism) {
	return compare_values(tval[0], ws.world.province_get_nationalism(to_prov(primary_slot)), float(tval[1]));
}
TRIGGER_FUNCTION(tf_is_overseas) {
	return compare_to_true(tval[0], province::is_overseas(ws, to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_overseas_pop) {
	return compare_to_true(tval[0], province::is_overseas(ws, ws.world.pop_get_province_from_pop_location(to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_is_overseas_state) {
	return compare_to_true(tval[0], province::is_overseas(ws, ws.world.state_instance_get_capital(to_state(primary_slot))));
}
TRIGGER_FUNCTION(tf_controlled_by_rebels) {
	return compare_to_true(tval[0],
			ws.world.province_get_rebel_faction_from_province_rebel_control(to_prov(primary_slot)) != dcon::rebel_faction_id());
}
TRIGGER_FUNCTION(tf_controlled_by_tag) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(to_prov(primary_slot)),
			ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id));
}
TRIGGER_FUNCTION(tf_controlled_by_from) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(to_prov(primary_slot)),
			to_nation(from_slot));
}
TRIGGER_FUNCTION(tf_controlled_by_this_nation) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(to_prov(primary_slot)),
			to_nation(this_slot));
}
TRIGGER_FUNCTION(tf_controlled_by_this_province) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(to_prov(primary_slot)),
			ws.world.province_get_nation_from_province_ownership(to_prov(this_slot)));
}
TRIGGER_FUNCTION(tf_controlled_by_this_state) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(to_prov(primary_slot)),
			ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot)));
}
TRIGGER_FUNCTION(tf_controlled_by_this_pop) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_control(to_prov(primary_slot)),
			nations::owner_of_pop(ws, to_pop(this_slot)));
}
TRIGGER_FUNCTION(tf_controlled_by_owner) {
	return compare_values_eq(tval[0], ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)),
			ws.world.province_get_nation_from_province_control(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_controlled_by_reb) {
	return compare_values_eq(tval[0], ws.world.province_get_rebel_faction_from_province_rebel_control(to_prov(primary_slot)),
			to_rebel(from_slot));
}
TRIGGER_FUNCTION(tf_is_canal_enabled) {
	return compare_to_true(tval[0], (ws.world.province_adjacency_get_type(ws.province_definitions.canals[tval[1] - 1]) &
																			province::border::impassible_bit) == 0);
}
TRIGGER_FUNCTION(tf_is_state_capital) {
	auto sid = ws.world.province_get_state_membership(to_prov(primary_slot));
	return compare_values_eq(tval[0], ws.world.state_instance_get_capital(sid), to_prov(primary_slot));
}
TRIGGER_FUNCTION(tf_is_state_capital_pop) {
	auto id = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto sid = ws.world.province_get_state_membership(id);
	return compare_values_eq(tval[0], ws.world.state_instance_get_capital(sid), id);
}
TRIGGER_FUNCTION(tf_truce_with_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	auto result = ve::apply(
			[&ws, holder](dcon::nation_id a) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, holder);
				auto date = ws.world.diplomatic_relation_get_truce_until(rel);
				return bool(date) && date > ws.current_date;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_truce_with_from) {
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				auto date = ws.world.diplomatic_relation_get_truce_until(rel);
				return bool(date) && date > ws.current_date;
			},
			to_nation(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_truce_with_this_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				auto date = ws.world.diplomatic_relation_get_truce_until(rel);
				return bool(date) && date > ws.current_date;
			},
			to_nation(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_truce_with_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				auto date = ws.world.diplomatic_relation_get_truce_until(rel);
				return bool(date) && date > ws.current_date;
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_truce_with_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				auto date = ws.world.diplomatic_relation_get_truce_until(rel);
				return bool(date) && date > ws.current_date;
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_truce_with_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				auto date = ws.world.diplomatic_relation_get_truce_until(rel);
				return bool(date) && date > ws.current_date;
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_total_pops_nation) {
	return compare_values(tval[0], ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_total_pops_state) {
	return compare_values(tval[0], ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_total_pops_province) {
	return compare_values(tval[0], ws.world.province_get_demographics(to_prov(primary_slot), demographics::total),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_total_pops_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	return compare_values(tval[0], ws.world.province_get_demographics(location, demographics::total),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_has_pop_type_nation) {
	auto type = payload(tval[1]).popt_id;
	return compare_to_true(tval[0],
			ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, type)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_type_state) {
	auto type = payload(tval[1]).popt_id;
	return compare_to_true(tval[0],
			ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, type)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_type_province) {
	auto type = payload(tval[1]).popt_id;
	return compare_to_true(tval[0],
			ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, type)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_type_pop) {
	auto type = payload(tval[1]).popt_id;
	return compare_values_eq(tval[0], ws.world.pop_get_poptype(to_pop(primary_slot)), type);
}
TRIGGER_FUNCTION(tf_has_empty_adjacent_province) {
	auto result = ve::apply(
			[&ws](dcon::province_id p) {
				auto acc = empty_province_accumulator(ws);
				for(auto a : ws.world.province_get_province_adjacency(p)) {
					auto other = a.get_connected_provinces(0) != p ? a.get_connected_provinces(0) : a.get_connected_provinces(1);
					acc.add_value(to_generic(other));
					if(acc.result)
						return true;
				}
				acc.flush();
				return acc.result;
			},
			to_prov(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_leader) {
	dcon::unit_name_id name{ dcon::unit_name_id::value_base_t(read_int32_t_from_payload(tval + 1)) };
	auto result = ve::apply(
			[&ws, name](dcon::nation_id n) {
				for(auto l : ws.world.nation_get_leader_loyalty(n)) {
					auto lname = l.get_leader().get_name();
					if(ws.to_string_view(lname) == ws.to_string_view(name))
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_ai) {
	return compare_to_false(tval[0], ws.world.nation_get_is_player_controlled(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_can_create_vassals) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n) {
				for(uint32_t i = 0; i < ws.world.national_identity_size(); ++i) {
					dcon::national_identity_id tag{dcon::national_identity_id::value_base_t(i)};
					if(nations::can_release_as_vassal(ws, n, tag))
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_is_possible_vassal) {
	auto tag = payload(tval[1]).tag_id;
	auto result =
			ve::apply([&ws, tag](dcon::nation_id n) { return nations::can_release_as_vassal(ws, n, tag); }, to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_province_id) {
	return compare_values_eq(tval[0], to_prov(primary_slot), payload(tval[1]).prov_id);
}
TRIGGER_FUNCTION(tf_vassal_of_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			tag_holder);
}
TRIGGER_FUNCTION(tf_vassal_of_from) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			to_nation(from_slot));
}
TRIGGER_FUNCTION(tf_vassal_of_this_nation) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			to_nation(this_slot));
}
TRIGGER_FUNCTION(tf_vassal_of_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			owner);
}
TRIGGER_FUNCTION(tf_vassal_of_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			owner);
}
TRIGGER_FUNCTION(tf_vassal_of_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))),
			owner);
}
TRIGGER_FUNCTION(tf_vassal_of_province_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)))),
			tag_holder);
}
TRIGGER_FUNCTION(tf_vassal_of_province_from) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)))),
			to_nation(from_slot));
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_nation) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)))),
			to_nation(this_slot));
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)))),
			owner);
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)))),
			owner);
}
TRIGGER_FUNCTION(tf_vassal_of_province_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)))),
			owner);
}


TRIGGER_FUNCTION(tf_substate_of_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_to_true(tval[0],
			ws.world.nation_get_is_substate(to_nation(primary_slot)) &&
					(ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))) == tag_holder));
}
TRIGGER_FUNCTION(tf_substate_of_from) {
	return compare_to_true(tval[0], ws.world.nation_get_is_substate(to_nation(primary_slot)) &&
																			(ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(
																					 to_nation(primary_slot))) == to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_substate_of_this_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_is_substate(to_nation(primary_slot)) &&
																			(ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(
																					 to_nation(primary_slot))) == to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_substate_of_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_to_true(tval[0],
			ws.world.nation_get_is_substate(to_nation(primary_slot)) &&
					(ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))) == owner));
}
TRIGGER_FUNCTION(tf_substate_of_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_to_true(tval[0],
			ws.world.nation_get_is_substate(to_nation(primary_slot)) &&
					(ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))) == owner));
}
TRIGGER_FUNCTION(tf_substate_of_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_to_true(tval[0],
			ws.world.nation_get_is_substate(to_nation(primary_slot)) &&
					(ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(primary_slot))) == owner));
}
TRIGGER_FUNCTION(tf_alliance_with_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	auto result = ve::apply(
			[&ws, tag_holder](dcon::nation_id n) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(n, tag_holder);
				return bool(rel) && ws.world.diplomatic_relation_get_are_allied(rel);
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_alliance_with_from) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id m) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(n, m);
				return bool(rel) && ws.world.diplomatic_relation_get_are_allied(rel);
			},
			to_nation(primary_slot), to_nation(from_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_alliance_with_this_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id m) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(n, m);
				return bool(rel) && ws.world.diplomatic_relation_get_are_allied(rel);
			},
			to_nation(primary_slot), to_nation(this_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_alliance_with_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id m) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(n, m);
				return bool(rel) && ws.world.diplomatic_relation_get_are_allied(rel);
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_alliance_with_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id m) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(n, m);
				return bool(rel) && ws.world.diplomatic_relation_get_are_allied(rel);
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_alliance_with_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::nation_id m) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(n, m);
				return bool(rel) && ws.world.diplomatic_relation_get_are_allied(rel);
			},
			to_nation(primary_slot), owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_recently_lost_war) {
	return compare_to_true(tval[0], ve::apply(
		[&](dcon::nation_id n) {
			auto d = ws.world.nation_get_last_war_loss(n);
			return bool(d) && ws.current_date <= (d + 365 * 5);
		}, to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_has_recently_lost_war_pop) {
	return compare_to_true(tval[0], ve::apply(
		[&](dcon::nation_id n) {
			auto d = ws.world.nation_get_last_war_loss(n);
			return bool(d) && ws.current_date <= (d + 365 * 5);
		}, nations::owner_of_pop(ws, to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_is_mobilised) {
	return compare_to_true(tval[0], ws.world.nation_get_is_mobilized(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_mobilisation_size) {
	return compare_values(tval[0],
			ws.world.nation_get_modifier_values(to_nation(primary_slot), sys::national_mod_offsets::mobilization_size),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_administrative_spending(to_nation(primary_slot)) >=
		ws.world.nation_get_education_spending(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_administrative_spending(owner) >= ws.world.nation_get_education_spending(owner));
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_administrative_spending(owner) >= ws.world.nation_get_education_spending(owner));
}
TRIGGER_FUNCTION(tf_crime_higher_than_education_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_administrative_spending(owner) >= ws.world.nation_get_education_spending(owner));
}
TRIGGER_FUNCTION(tf_agree_with_ruling_party) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto ruling_ideology = ws.world.political_party_get_ideology(ws.world.nation_get_ruling_party(owner));
	auto population_size = ws.world.pop_get_size(to_pop(primary_slot));
	auto ruling_support = ve::apply(
			[&](dcon::pop_id p, dcon::ideology_id i) {
				if(i)
					return ws.world.pop_get_demographics(p, pop_demographics::to_key(ws, i));
				else
					return 0.0f;
			},
			to_pop(primary_slot), ruling_ideology);
	return compare_values(tval[0], ve::select(population_size > 0.0f, ruling_support / population_size, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_colonial_state) {
	auto cap_p = ws.world.state_instance_get_capital(to_state(primary_slot));
	return compare_to_true(tval[0], ws.world.province_get_is_colonial(cap_p));
}
TRIGGER_FUNCTION(tf_is_colonial_province) {
	return compare_to_true(tval[0], ws.world.province_get_is_colonial(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_colonial_pop) {
	return compare_to_true(tval[0], ws.world.province_get_is_colonial(ws.world.pop_get_province_from_pop_location(to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_has_factories_state) {
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s) { return economy::has_factory(ws, s); },
			to_state(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_factories_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n) {
				for(auto p : ws.world.nation_get_province_ownership(n)) {
					auto rng = p.get_province().get_factory_location();
					if(rng.begin() != rng.end())
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_in_default_bool) {
	return compare_to_true(tval[0], ws.world.nation_get_is_bankrupt(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_in_default_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_to_true(tval[0],
			ve::apply([&ws, holder](dcon::nation_id n) { return economy::is_bankrupt_debtor_to(ws, n, holder); },
					to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_in_default_from) {
	return compare_to_true(tval[0],
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id h) { return economy::is_bankrupt_debtor_to(ws, n, h); },
					to_nation(primary_slot), to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_in_default_this_nation) {
	return compare_to_true(tval[0],
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id h) { return economy::is_bankrupt_debtor_to(ws, n, h); },
					to_nation(primary_slot), to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_in_default_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_to_true(tval[0],
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id h) { return economy::is_bankrupt_debtor_to(ws, n, h); },
					to_nation(primary_slot), owner));
}
TRIGGER_FUNCTION(tf_in_default_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_to_true(tval[0],
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id h) { return economy::is_bankrupt_debtor_to(ws, n, h); },
					to_nation(primary_slot), owner));
}
TRIGGER_FUNCTION(tf_in_default_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_to_true(tval[0],
			ve::apply([&ws](dcon::nation_id n, dcon::nation_id h) { return economy::is_bankrupt_debtor_to(ws, n, h); },
					to_nation(primary_slot), owner));
}
TRIGGER_FUNCTION(tf_total_num_of_ports) {
	return compare_values(tval[0], ws.world.nation_get_total_ports(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_always) {
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_election) {
	return compare_to_true(tval[0], ve::apply(
		[&ws](dcon::nation_id n) {
			auto d = ws.world.nation_get_election_ends(n);
			return bool(d) && d > ws.current_date;
		}, to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_has_global_flag) {
	return compare_to_true(tval[0], ws.national_definitions.is_global_flag_variable_set(payload(tval[1]).glob_id));
}
TRIGGER_FUNCTION(tf_is_capital) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_capital(owner), to_prov(primary_slot));
}
TRIGGER_FUNCTION(tf_nationalvalue_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_national_value(to_nation(primary_slot)), payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_nationalvalue_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_national_value(owner), payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_nationalvalue_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_national_value(owner), payload(tval[1]).mod_id);
}
TRIGGER_FUNCTION(tf_industrial_score_value) {
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_industrial_score_tag) {
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)),
		ws.world.nation_get_industrial_score(ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id)));
}
TRIGGER_FUNCTION(tf_industrial_score_from_nation) {
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)),
			ws.world.nation_get_industrial_score(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_industrial_score_this_nation) {
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)),
			ws.world.nation_get_industrial_score(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_industrial_score_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)),
			ws.world.nation_get_industrial_score(owner));
}
TRIGGER_FUNCTION(tf_industrial_score_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)),
			ws.world.nation_get_industrial_score(owner));
}
TRIGGER_FUNCTION(tf_industrial_score_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values(tval[0], ws.world.nation_get_industrial_score(to_nation(primary_slot)),
			ws.world.nation_get_industrial_score(owner));
}
TRIGGER_FUNCTION(tf_military_score_value) {
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_military_score_tag) {
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)),
		ws.world.nation_get_military_score(ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id)));
}
TRIGGER_FUNCTION(tf_military_score_from_nation) {
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)),
			ws.world.nation_get_military_score(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_military_score_this_nation) {
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)),
			ws.world.nation_get_military_score(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_military_score_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)),
			ws.world.nation_get_military_score(owner));
}
TRIGGER_FUNCTION(tf_military_score_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)),
			ws.world.nation_get_military_score(owner));
}
TRIGGER_FUNCTION(tf_military_score_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values(tval[0], ws.world.nation_get_military_score(to_nation(primary_slot)),
			ws.world.nation_get_military_score(owner));
}
TRIGGER_FUNCTION(tf_civilized_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_is_civilized(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_civilized_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_is_civilized(owner));
}
TRIGGER_FUNCTION(tf_civilized_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_is_civilized(owner));
}
TRIGGER_FUNCTION(tf_national_provinces_occupied) {
	return compare_values(tval[0], nations::occupied_provinces_fraction(ws, to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_is_greater_power_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_is_great_power(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_greater_power_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_is_great_power(owner));
}
TRIGGER_FUNCTION(tf_is_greater_power_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_to_true(tval[0], ws.world.nation_get_is_great_power(owner));
}
TRIGGER_FUNCTION(tf_rich_tax) {
	return compare_values(tval[0], ws.world.nation_get_rich_tax(to_nation(primary_slot)), payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_middle_tax) {
	return compare_values(tval[0], ws.world.nation_get_middle_tax(to_nation(primary_slot)), payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_poor_tax) {
	return compare_values(tval[0], ws.world.nation_get_poor_tax(to_nation(primary_slot)), payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_rich_tax_pop) {
	return compare_values(tval[0], ws.world.nation_get_rich_tax(nations::owner_of_pop(ws, to_pop(primary_slot))), payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_middle_tax_pop) {
	return compare_values(tval[0], ws.world.nation_get_middle_tax(nations::owner_of_pop(ws, to_pop(primary_slot))), payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_poor_tax_pop) {
	return compare_values(tval[0], ws.world.nation_get_poor_tax(nations::owner_of_pop(ws, to_pop(primary_slot))), payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_social_spending_nation) {
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_social_spending(to_nation(primary_slot))) * ws.world.nation_get_spending_level(to_nation(primary_slot)), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_social_spending_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_social_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_social_spending_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_social_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_military_spending_nation) {
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_military_spending(to_nation(primary_slot))) * ws.world.nation_get_spending_level(to_nation(primary_slot)), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_military_spending_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_military_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_military_spending_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_military_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_military_spending_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_military_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_administration_spending_nation) {
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_administrative_spending(to_nation(primary_slot))) * ws.world.nation_get_spending_level(to_nation(primary_slot)), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_administration_spending_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_administrative_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_administration_spending_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_administrative_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_administration_spending_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_administrative_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_education_spending_nation) {
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_education_spending(to_nation(primary_slot))) * ws.world.nation_get_spending_level(to_nation(primary_slot)), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_education_spending_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_education_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_education_spending_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_education_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_education_spending_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	return compare_values(tval[0], ve::to_float(ws.world.nation_get_education_spending(owner)) * ws.world.nation_get_spending_level(owner), float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_colonial_nation) {
	return compare_to_true(tval[0], ws.world.nation_get_is_colonial_nation(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_pop_majority_religion_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_dominant_religion(to_nation(primary_slot)), payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_pop_majority_religion_nation_this_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_dominant_religion(to_nation(primary_slot)), ws.world.nation_get_religion(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_pop_majority_religion_state) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_religion(to_state(primary_slot)),
			payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_pop_majority_religion_province) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_religion(to_prov(primary_slot)), payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_pop_majority_culture_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_dominant_culture(to_nation(primary_slot)), payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_pop_majority_culture_state) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_culture(to_state(primary_slot)),
			payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_pop_majority_culture_province) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_culture(to_prov(primary_slot)), payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_dominant_issue_option(to_nation(primary_slot)), payload(tval[1]).opt_id);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_state) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_issue_option(to_state(primary_slot)),
			payload(tval[1]).opt_id);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_province) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_issue_option(to_prov(primary_slot)), payload(tval[1]).opt_id);
}
TRIGGER_FUNCTION(tf_pop_majority_issue_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_dominant_issue_option(to_pop(primary_slot)), payload(tval[1]).opt_id);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_dominant_ideology(to_nation(primary_slot)), payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_state) {
	return compare_values_eq(tval[0], ws.world.state_instance_get_dominant_ideology(to_state(primary_slot)),
			payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_province) {
	return compare_values_eq(tval[0], ws.world.province_get_dominant_ideology(to_prov(primary_slot)), payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_pop_majority_ideology_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_dominant_ideology(to_pop(primary_slot)), payload(tval[1]).ideo_id);
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_militancy_pop) {
	auto type = ws.world.pop_get_poptype(to_pop(primary_slot));
	auto is_poor = ws.world.pop_type_get_strata(type) == uint8_t(culture::pop_strata::poor);
	return compare_values(tval[0], ve::select(is_poor, ws.world.pop_get_militancy(to_pop(primary_slot)), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_militancy_pop) {
	auto type = ws.world.pop_get_poptype(to_pop(primary_slot));
	auto is_middle = ws.world.pop_type_get_strata(type) == uint8_t(culture::pop_strata::middle);
	return compare_values(tval[0], ve::select(is_middle, ws.world.pop_get_militancy(to_pop(primary_slot)), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_militancy) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_militancy_pop) {
	auto type = ws.world.pop_get_poptype(to_pop(primary_slot));
	auto is_rich = ws.world.pop_type_get_strata(type) == uint8_t(culture::pop_strata::rich);
	return compare_values(tval[0], ve::select(is_rich, ws.world.pop_get_militancy(to_pop(primary_slot)), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_tax_above_poor) {
	return compare_to_true(tval[0],
			ws.world.nation_get_rich_tax(to_nation(primary_slot)) > ws.world.nation_get_poor_tax(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_culture_has_union_tag_pop) {
	auto pop_culture = ws.world.pop_get_culture(to_pop(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pop_culture);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_to_true(tval[0], utag != dcon::national_identity_id());
}
TRIGGER_FUNCTION(tf_culture_has_union_tag_nation) {
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_to_true(tval[0], utag != dcon::national_identity_id());
}
TRIGGER_FUNCTION(tf_this_culture_union_tag) {
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_values_eq(tval[0], utag, payload(tval[1]).tag_id);
}
TRIGGER_FUNCTION(tf_this_culture_union_from) {
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_values_eq(tval[0], utag, ws.world.nation_get_identity_from_identity_holder(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_this_culture_union_this_nation) {
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_values_eq(tval[0], utag, ws.world.nation_get_identity_from_identity_holder(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_this_culture_union_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_values_eq(tval[0], utag, ws.world.nation_get_identity_from_identity_holder(owner));
}
TRIGGER_FUNCTION(tf_this_culture_union_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_values_eq(tval[0], utag, ws.world.nation_get_identity_from_identity_holder(owner));
}
TRIGGER_FUNCTION(tf_this_culture_union_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto pc = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto cg = ws.world.culture_get_group_from_culture_group_membership(pc);
	auto utag = ws.world.culture_group_get_identity_from_cultural_union_of(cg);
	return compare_values_eq(tval[0], utag, ws.world.nation_get_identity_from_identity_holder(owner));
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_nation) {
	auto prim_culture = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto this_culture = ws.world.nation_get_primary_culture(to_nation(this_slot));
	auto pcg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(this_culture);
	return compare_values_eq(tval[0], pcg, tcg);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto prim_culture = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto this_culture = ws.world.nation_get_primary_culture(owner);
	auto pcg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(this_culture);
	return compare_values_eq(tval[0], pcg, tcg);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	auto prim_culture = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto this_culture = ws.world.nation_get_primary_culture(owner);
	auto pcg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(this_culture);
	return compare_values_eq(tval[0], pcg, tcg);
}
TRIGGER_FUNCTION(tf_this_culture_union_this_union_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto prim_culture = ws.world.nation_get_primary_culture(to_nation(primary_slot));
	auto this_culture = ws.world.nation_get_primary_culture(owner);
	auto pcg = ws.world.culture_get_group_from_culture_group_membership(prim_culture);
	auto tcg = ws.world.culture_get_group_from_culture_group_membership(this_culture);
	return compare_values_eq(tval[0], pcg, tcg);
}
TRIGGER_FUNCTION(tf_minorities_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	auto pculture = ws.world.nation_get_primary_culture(to_nation(primary_slot));

	auto accepted_pop = ve::apply(
			[&ws](dcon::nation_id n, dcon::culture_id pc) {
				if(!pc)
					return 0.0f;
				auto accumulated = ws.world.nation_get_demographics(n, demographics::to_key(ws, pc));
				for(auto ac : ws.world.in_culture) {
					if(ws.world.nation_get_accepted_cultures(n, ac)) {
						accumulated += ws.world.nation_get_demographics(n, demographics::to_key(ws, ac));
					}
				}
				return accumulated;
			},
			to_nation(primary_slot), pculture);

	return compare_to_true(tval[0], total_pop != accepted_pop);
}
TRIGGER_FUNCTION(tf_minorities_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto pculture = ws.world.nation_get_primary_culture(owner);

	auto accepted_pop = ve::apply(
			[&ws](dcon::state_instance_id i, dcon::nation_id n, dcon::culture_id pc) {
				if(!pc)
					return 0.0f;
				auto accumulated = ws.world.state_instance_get_demographics(i, demographics::to_key(ws, pc));
				for(auto ac : ws.world.in_culture) {
					if(ws.world.nation_get_accepted_cultures(n, ac)) {
						accumulated += ws.world.state_instance_get_demographics(i, demographics::to_key(ws, ac));
					}
				}
				return accumulated;
			},
			to_state(primary_slot), owner, pculture);

	return compare_to_true(tval[0], total_pop != accepted_pop);
}
TRIGGER_FUNCTION(tf_minorities_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto pculture = ws.world.nation_get_primary_culture(owner);

	auto accepted_pop = ve::apply(
			[&ws](dcon::province_id i, dcon::nation_id n, dcon::culture_id pc) {
				if(!pc)
					return 0.0f;
				auto accumulated = ws.world.province_get_demographics(i, demographics::to_key(ws, pc));
				for(auto ac : ws.world.in_culture) {
					if(ws.world.nation_get_accepted_cultures(n, ac)) {
						accumulated += ws.world.province_get_demographics(i, demographics::to_key(ws, ac));
					}
				}
				return accumulated;
			},
			to_prov(primary_slot), owner, pculture);

	return compare_to_true(tval[0], total_pop != accepted_pop);
}
TRIGGER_FUNCTION(tf_revanchism_nation) {
	return compare_values(tval[0], ws.world.nation_get_revanchism(to_nation(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_revanchism_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	return compare_values(tval[0], ws.world.nation_get_revanchism(owner), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_has_crime) {
	return compare_to_true(tval[0], ws.world.province_get_crime(to_prov(primary_slot)) != dcon::crime_id());
}
TRIGGER_FUNCTION(tf_num_of_substates) {
	return compare_values(tval[0], ws.world.nation_get_substates_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_num_of_vassals_no_substates) {
	return compare_values(tval[0],
			ws.world.nation_get_vassals_count(to_nation(primary_slot)) - ws.world.nation_get_substates_count(to_nation(primary_slot)),
			tval[1]);
}
TRIGGER_FUNCTION(tf_brigades_compare_this) {
	return compare_values(tval[0], ve::select(ws.world.nation_get_active_regiments(to_nation(this_slot)) != 0, ve::to_float(ws.world.nation_get_active_regiments(to_nation(primary_slot))) / ve::to_float(ws.world.nation_get_active_regiments(to_nation(this_slot))), 1'000'000.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_brigades_compare_from) {
	return compare_values(tval[0], ve::select(ws.world.nation_get_active_regiments(to_nation(from_slot)) != 0, ve::to_float(ws.world.nation_get_active_regiments(to_nation(primary_slot))) / ve::to_float(ws.world.nation_get_active_regiments(to_nation(from_slot))), 1'000'000.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_brigades_compare_province_this) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ve::select(ws.world.nation_get_active_regiments(to_nation(this_slot)) != 0, ve::to_float(ws.world.nation_get_active_regiments(owner)) / ve::to_float(ws.world.nation_get_active_regiments(to_nation(this_slot))), 1'000'000.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_brigades_compare_province_from) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	return compare_values(tval[0], ve::select(ws.world.nation_get_active_regiments(to_nation(from_slot)) != 0, ve::to_float(ws.world.nation_get_active_regiments(owner)) / ve::to_float(ws.world.nation_get_active_regiments(to_nation(from_slot))), 1'000'000.0f), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_constructing_cb_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_to_true(tval[0], ws.world.nation_get_constructing_cb_target(to_nation(primary_slot)) == tag_holder);
}
TRIGGER_FUNCTION(tf_constructing_cb_from) {
	return compare_values_eq(tval[0], ws.world.nation_get_constructing_cb_target(to_nation(primary_slot)), to_nation(from_slot));
}
TRIGGER_FUNCTION(tf_constructing_cb_this_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_constructing_cb_target(to_nation(primary_slot)), to_nation(this_slot));
}
TRIGGER_FUNCTION(tf_constructing_cb_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_constructing_cb_target(to_nation(primary_slot)), owner);
}
TRIGGER_FUNCTION(tf_constructing_cb_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_constructing_cb_target(to_nation(primary_slot)), owner);
}
TRIGGER_FUNCTION(tf_constructing_cb_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_constructing_cb_target(to_nation(primary_slot)), owner);
}
TRIGGER_FUNCTION(tf_constructing_cb_discovered) {
	return compare_to_true(tval[0], ws.world.nation_get_constructing_cb_is_discovered(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_constructing_cb_progress) {
	return compare_values(tval[0], ws.world.nation_get_constructing_cb_progress(to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_civilization_progress) {
	return compare_values(tval[0],
			ws.world.nation_get_modifier_values(to_nation(primary_slot), sys::national_mod_offsets::civilization_progress_modifier),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_constructing_cb_type) {
	return compare_values_eq(tval[0], ws.world.nation_get_constructing_cb_type(to_nation(primary_slot)), payload(tval[1]).cb_id);
}
TRIGGER_FUNCTION(tf_is_our_vassal_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(tag_holder)),
			to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_our_vassal_from) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(from_slot))),
			to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_nation) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(this_slot))),
			to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(owner)),
			to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(owner)),
			to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_our_vassal_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(owner)),
			to_nation(primary_slot));
}

TRIGGER_FUNCTION(tf_is_our_vassal_province_tag) {
	auto tag_holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(tag_holder)),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_from) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(from_slot))),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_nation) {
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(to_nation(this_slot))),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(owner)),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(owner)),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_our_vassal_province_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.overlord_get_ruler(ws.world.nation_get_overlord_as_subject(owner)),
			ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)));
}


TRIGGER_FUNCTION(tf_is_substate) {
	return compare_to_true(tval[0], ws.world.nation_get_is_substate(to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_great_wars_enabled) {
	return compare_to_true(tval[0], ws.military_definitions.great_wars_enabled);
}
TRIGGER_FUNCTION(tf_can_nationalize) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n) {
				for(auto c : ws.world.nation_get_unilateral_relationship_as_target(n)) {
					if(c.get_foreign_investment() > 0.0f)
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_part_of_sphere) {
	return compare_to_true(tval[0], ws.world.nation_get_in_sphere_of(to_nation(primary_slot)) != dcon::nation_id());
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[1]).tag_id);
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(holder), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_from) {
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(from_slot)), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(to_nation(this_slot)), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(owner), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(owner), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_is_sphere_leader_of_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_in_sphere_of(owner), to_nation(primary_slot));
}
TRIGGER_FUNCTION(tf_number_of_states) {
	return compare_values(tval[0], ws.world.nation_get_owned_state_count(to_nation(primary_slot)), tval[1]);
}
TRIGGER_FUNCTION(tf_war_score) {
	// unimplemented: which war would this be for?
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_is_releasable_vassal_from) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(from_slot));
	return compare_to_true(tval[0], !ws.world.national_identity_get_is_not_releasable(tag));
}
TRIGGER_FUNCTION(tf_is_releasable_vassal_other) {
	auto tag = ws.world.nation_get_identity_from_identity_holder(to_nation(from_slot));
	return compare_to_true(tval[0], !ws.world.national_identity_get_is_not_releasable(tag));
}
TRIGGER_FUNCTION(tf_has_recent_imigration) {
	return compare_values(tval[0],
			int32_t(tval[1]) +
					ve::apply([&ws](dcon::province_id p) { return ws.world.province_get_last_immigration(p).to_raw_value(); },
							to_prov(primary_slot)),
			ws.current_date.to_raw_value());
}
TRIGGER_FUNCTION(tf_province_control_days) {
	return compare_values(tval[0],
			ws.current_date.to_raw_value() -
					ve::apply([&ws](dcon::province_id p) { return ws.world.province_get_last_control_change(p).to_raw_value(); },
							to_prov(primary_slot)),
			int32_t(tval[1]));
}
TRIGGER_FUNCTION(tf_is_disarmed) {
	return compare_to_true(tval[0], ve::apply([&ws](dcon::nation_id n) {
			auto d = ws.world.nation_get_disarmed_until(n);
			return bool(d) && ws.current_date < d;
		}, to_nation(primary_slot)));
}
TRIGGER_FUNCTION(tf_is_disarmed_pop) {
	return compare_to_true(tval[0], ve::apply([&ws](dcon::nation_id n) {
		auto d = ws.world.nation_get_disarmed_until(n);
		return bool(d) && ws.current_date < d;
		}, nations::owner_of_pop(ws, to_pop(primary_slot))));
}
TRIGGER_FUNCTION(tf_big_producer) {
	// stub: used only rarely in ai chances and would be expensive to test
	return compare_to_true(tval[0], false);
}
TRIGGER_FUNCTION(tf_someone_can_form_union_tag_from) {
	// stub: apparently unused
	return compare_to_true(tval[0], false);
}
TRIGGER_FUNCTION(tf_someone_can_form_union_tag_other) {
	// stub: apparently unused
	return compare_to_true(tval[0], false);
}
TRIGGER_FUNCTION(tf_social_movement_strength) {
	return compare_values(tval[0],
			ve::apply(
					[&ws](dcon::nation_id n) {
						float max_str = 0.0f;
						for(auto m : ws.world.nation_get_movement_within(n)) {
							auto issue = m.get_movement().get_associated_issue_option();
							if(issue) {
								if(culture::issue_type(issue.get_parent_issue().get_issue_type()) == culture::issue_type::social &&
										m.get_movement().get_pop_support() > max_str) {
									max_str = m.get_movement().get_pop_support();
								}
							}
						}
						return max_str;
					},
					to_nation(primary_slot)) *
					ws.defines.movement_support_uh_factor / ws.world.nation_get_non_colonial_population(to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_political_movement_strength) {
	return compare_values(tval[0],
			ve::apply(
					[&ws](dcon::nation_id n) {
						float max_str = 0.0f;
						for(auto m : ws.world.nation_get_movement_within(n)) {
							auto issue = m.get_movement().get_associated_issue_option();
							if(issue) {
								if(culture::issue_type(issue.get_parent_issue().get_issue_type()) == culture::issue_type::political &&
										m.get_movement().get_pop_support() > max_str) {
									max_str = m.get_movement().get_pop_support();
								}
							}
						}
						return max_str;
					},
					to_nation(primary_slot)) *
					ws.defines.movement_support_uh_factor / ws.world.nation_get_non_colonial_population(to_nation(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_can_build_factory_in_capital_state) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_social_movement) {
	return compare_to_true(tval[0], ve::apply(
																			[&ws](dcon::pop_id p) {
																				auto m = fatten(ws.world, ws.world.pop_get_movement_from_pop_movement_membership(p));
																				return m && m.get_associated_issue_option() &&
																							 culture::issue_type(
																									 m.get_associated_issue_option().get_parent_issue().get_issue_type()) ==
																									 culture::issue_type::social;
																			},
																			to_pop(primary_slot)));
}
TRIGGER_FUNCTION(tf_political_movement) {
	return compare_to_true(tval[0], ve::apply(
																			[&ws](dcon::pop_id p) {
																				auto m = fatten(ws.world, ws.world.pop_get_movement_from_pop_movement_membership(p));
																				return m && m.get_associated_issue_option() &&
																							 culture::issue_type(
																									 m.get_associated_issue_option().get_parent_issue().get_issue_type()) ==
																									 culture::issue_type::political;
																			},
																			to_pop(primary_slot)));
}
TRIGGER_FUNCTION(tf_political_movement_from_reb) {
	// returns false because I'm not sure exactly what it is supposed to do (applies to the below as well)
	// it is called in nation scope while modifying chances for a rebel faction to spawn
	// perhaps reducing the chance if there is an "appropriate" movement (because there is almost always some movement)
	// however, the logic from working backwards from rebel factions to movements is not clear to me
	return compare_to_true(tval[0], false);
}
TRIGGER_FUNCTION(tf_social_movement_from_reb) {
	return compare_to_true(tval[0], false);
}

TRIGGER_FUNCTION(tf_has_cultural_sphere) {
	auto prim_culture = ws.world.nation_get_primary_culture(to_nation(this_slot));
	auto culture_group = ws.world.culture_get_group_from_culture_group_membership(prim_culture);

	auto result = ve::apply(
			[&ws](dcon::nation_id n, dcon::culture_group_id g) {
				for(auto s : ws.world.nation_get_gp_relationship_as_great_power(n)) {
					if((s.get_status() & nations::influence::level_mask) == nations::influence::level_in_sphere &&
							s.get_influence_target().get_primary_culture().get_group_from_culture_group_membership() == g) {
						return true;
					}
				}
				return false;
			},
			to_nation(primary_slot), culture_group);

	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_world_wars_enabled) {
	return compare_to_true(tval[0], ws.military_definitions.world_wars_enabled);
}
TRIGGER_FUNCTION(tf_has_pop_culture_pop_this_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)), ws.world.pop_get_culture(to_pop(this_slot)));
}
TRIGGER_FUNCTION(tf_has_pop_culture_state_this_pop) {
	auto culture = ws.world.pop_get_culture(to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::culture_id c) {
				return bool(c) ? ws.world.state_instance_get_demographics(s, demographics::to_key(ws, c)) > 0.0f : false;
			},
			to_state(primary_slot), culture);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_pop_culture_province_this_pop) {
	auto culture = ws.world.pop_get_culture(to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id s, dcon::culture_id c) {
				return bool(c) ? ws.world.province_get_demographics(s, demographics::to_key(ws, c)) > 0.0f : false;
			},
			to_prov(primary_slot), culture);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_pop_culture_nation_this_pop) {
	auto culture = ws.world.pop_get_culture(to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id s, dcon::culture_id c) {
				return bool(c) ? ws.world.nation_get_demographics(s, demographics::to_key(ws, c)) > 0.0f : false;
			},
			to_nation(primary_slot), culture);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_pop_culture_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_culture(to_pop(primary_slot)), payload(tval[1]).cul_id);
}
TRIGGER_FUNCTION(tf_has_pop_culture_state) {
	return compare_to_true(tval[0],
			ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, payload(tval[1]).cul_id)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_culture_province) {
	return compare_to_true(tval[0],
			ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, payload(tval[1]).cul_id)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_culture_nation) {
	return compare_to_true(tval[0],
			ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, payload(tval[1]).cul_id)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_religion_pop_this_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)),
			ws.world.pop_get_religion(to_pop(this_slot)));
}
TRIGGER_FUNCTION(tf_has_pop_religion_state_this_pop) {
	auto rel = ws.world.pop_get_religion(to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::state_instance_id s, dcon::religion_id r) {
				return bool(r) ? ws.world.state_instance_get_demographics(s, demographics::to_key(ws, r)) > 0.0f : false;
			},
			to_state(primary_slot), rel);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_pop_religion_province_this_pop) {
	auto rel = ws.world.pop_get_religion(to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::province_id s, dcon::religion_id r) {
				return bool(r) ? ws.world.province_get_demographics(s, demographics::to_key(ws, r)) > 0.0f : false;
			},
			to_prov(primary_slot), rel);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_pop_religion_nation_this_pop) {
	auto rel = ws.world.pop_get_religion(to_pop(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id s, dcon::religion_id r) {
				return bool(r) ? ws.world.nation_get_demographics(s, demographics::to_key(ws, r)) > 0.0f : false;
			},
			to_nation(primary_slot), rel);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_pop_religion_pop) {
	return compare_values_eq(tval[0], ws.world.pop_get_religion(to_pop(primary_slot)), payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_has_pop_religion_state) {
	return compare_to_true(tval[0],
			ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, payload(tval[1]).rel_id)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_religion_province) {
	return compare_to_true(tval[0],
			ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, payload(tval[1]).rel_id)) > 0.0f);
}
TRIGGER_FUNCTION(tf_has_pop_religion_nation) {
	return compare_to_true(tval[0],
			ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, payload(tval[1]).rel_id)) > 0.0f);
}
TRIGGER_FUNCTION(tf_life_needs) {
	return compare_values(tval[0], ws.world.pop_get_life_needs_satisfaction(to_pop(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_everyday_needs) {
	return compare_values(tval[0], ws.world.pop_get_everyday_needs_satisfaction(to_pop(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_luxury_needs) {
	return compare_values(tval[0], ws.world.pop_get_luxury_needs_satisfaction(to_pop(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_consciousness_pop) {
	return compare_values(tval[0], ws.world.pop_get_consciousness(to_pop(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_consciousness_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::consciousness) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_consciousness_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::consciousness) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_consciousness_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::consciousness) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_literacy_pop) {
	return compare_values(tval[0], ws.world.pop_get_literacy(to_pop(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_literacy_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f, ws.world.province_get_demographics(to_prov(primary_slot), demographics::literacy) / total_pop,
					0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_literacy_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::literacy) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_literacy_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f, ws.world.nation_get_demographics(to_nation(primary_slot), demographics::literacy) / total_pop,
					0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_militancy_pop) {
	return compare_values(tval[0], ws.world.pop_get_militancy(to_pop(primary_slot)), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_militancy_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::militancy) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_militancy_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::militancy) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_militancy_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	return compare_values(tval[0],
			ve::select(total_pop != 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::militancy) / total_pop, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_trade_goods_in_state_state) {
	auto g = payload(tval[1]).com_id;
	auto sdef = ws.world.state_instance_get_definition(to_state(primary_slot));
	auto sowner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto result = ve::apply(
			[&ws, g](dcon::state_definition_id sd, dcon::nation_id o) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(sd)) {
					if(p.get_province().get_nation_from_province_ownership() == o) {
						if(p.get_province().get_rgo() == g)
							return true;
					}
				}
				return false;
			},
			sdef, sowner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_trade_goods_in_state_province) {
	auto g = payload(tval[1]).com_id;
	auto si = ws.world.province_get_state_membership(to_prov(primary_slot));
	auto sdef = ws.world.state_instance_get_definition(si);
	auto sowner = ws.world.state_instance_get_nation_from_state_ownership(si);
	auto result = ve::apply(
			[&ws, g](dcon::state_definition_id sd, dcon::nation_id o) {
				for(auto p : ws.world.state_definition_get_abstract_state_membership(sd)) {
					if(p.get_province().get_nation_from_province_ownership() == o) {
						if(p.get_province().get_rgo() == g)
							return true;
					}
				}
				return false;
			},
			sdef, sowner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_has_flashpoint) {
	return compare_to_true(tval[0],
			ws.world.state_instance_get_flashpoint_tag(to_state(primary_slot)) != dcon::national_identity_id());
}
TRIGGER_FUNCTION(tf_flashpoint_tension) {
	return compare_values(tval[0], ws.world.state_instance_get_flashpoint_tension(to_state(primary_slot)),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_flashpoint_tension_province) {
	return compare_values(tval[0], ws.world.state_instance_get_flashpoint_tension(ws.world.province_get_state_membership(to_prov(primary_slot))),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_crisis_exist) {
	return compare_to_true(tval[0], ws.current_crisis != sys::crisis_type::none);
}
TRIGGER_FUNCTION(tf_is_liberation_crisis) {
	return compare_to_true(tval[0], ws.current_crisis == sys::crisis_type::liberation);
}
TRIGGER_FUNCTION(tf_is_claim_crisis) {
	return compare_to_true(tval[0], ws.current_crisis == sys::crisis_type::claim);
}
TRIGGER_FUNCTION(tf_crisis_temperature) {
	return compare_values(tval[0], ws.crisis_temperature, read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_involved_in_crisis_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id n) {
				auto sz = ws.crisis_participants.size();
				for(uint32_t i = 0; i < sz; ++i) {
					if(!ws.crisis_participants[i].id)
						return false;
					if(ws.crisis_participants[i].id == n)
						return true;
				}
				return false;
			},
			to_nation(primary_slot));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_involved_in_crisis_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id n) {
				auto sz = ws.crisis_participants.size();
				for(uint32_t i = 0; i < sz; ++i) {
					if(!ws.crisis_participants[i].id)
						return false;
					if(ws.crisis_participants[i].id == n)
						return true;
				}
				return false;
			},
			owner);
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_life_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_rich_strata_life_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_everyday_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_everyday_needs) / accumulator,
					0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_everyday_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_everyday_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_rich_strata_everyday_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::rich_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::rich_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::rich_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_rich_strata_luxury_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_rich_strata_luxury_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_life_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_middle_strata_life_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_everyday_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_everyday_needs) / accumulator,
					0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_everyday_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_everyday_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_middle_strata_everyday_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::middle_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::middle_luxury_needs) / accumulator,
					0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::middle_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_middle_strata_luxury_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_middle_strata_luxury_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_life_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_life_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_poor_strata_life_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_everyday_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_everyday_needs) / accumulator,
					0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_everyday_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_everyday_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_poor_strata_everyday_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_nation) {
	auto accumulator = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.nation_get_demographics(to_nation(primary_slot), demographics::poor_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_state) {
	auto accumulator = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::poor_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_province) {
	auto accumulator = ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_total);
	return compare_values(tval[0],
			ve::select(accumulator > 0.0f,
					ws.world.province_get_demographics(to_prov(primary_slot), demographics::poor_luxury_needs) / accumulator, 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_poor_strata_luxury_needs_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_poor_strata_luxury_needs_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_diplomatic_influence_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[2]).tag_id);
	auto result = ve::apply(
			[&ws, holder](dcon::nation_id a) {
				auto gpr = ws.world.get_gp_relationship_by_gp_influence_pair(holder, a);
				return ws.world.gp_relationship_get_influence(gpr);
			},
			to_nation(primary_slot));
	return compare_values(tval[0], result, float(tval[1]));
}
TRIGGER_FUNCTION(tf_diplomatic_influence_this_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto gpr = ws.world.get_gp_relationship_by_gp_influence_pair(b, a);
				return ws.world.gp_relationship_get_influence(gpr);
			},
			to_nation(primary_slot), to_nation(this_slot));
	return compare_values(tval[0], result, float(tval[1]));
}
TRIGGER_FUNCTION(tf_diplomatic_influence_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto gpr = ws.world.get_gp_relationship_by_gp_influence_pair(b, a);
				return ws.world.gp_relationship_get_influence(gpr);
			},
			to_nation(primary_slot), owner);
	return compare_values(tval[0], result, float(tval[1]));
}
TRIGGER_FUNCTION(tf_diplomatic_influence_from_nation) {
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto gpr = ws.world.get_gp_relationship_by_gp_influence_pair(b, a);
				return ws.world.gp_relationship_get_influence(gpr);
			},
			to_nation(primary_slot), to_nation(from_slot));
	return compare_values(tval[0], result, float(tval[1]));
}
TRIGGER_FUNCTION(tf_diplomatic_influence_from_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(from_slot));
	auto result = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto gpr = ws.world.get_gp_relationship_by_gp_influence_pair(b, a);
				return ws.world.gp_relationship_get_influence(gpr);
			},
			to_nation(primary_slot), owner);
	return compare_values(tval[0], result, float(tval[1]));
}
TRIGGER_FUNCTION(tf_pop_unemployment_nation) {
	auto type = payload(tval[3]).popt_id;
	auto pop_size = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, type));
	auto employment = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_employment_key(ws, type));
	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_pop_unemployment_state) {
	auto type = payload(tval[3]).popt_id;
	auto pop_size = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, type));
	auto employment = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_employment_key(ws, type));
	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_pop_unemployment_province) {
	auto type = payload(tval[3]).popt_id;
	auto pop_size = ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, type));
	auto employment = ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_employment_key(ws, type));
	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_pop_unemployment_pop) {
	auto pop_size = ws.world.pop_get_size(to_pop(primary_slot));
	auto employment = ws.world.pop_get_employment(to_pop(primary_slot));
	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_pop_unemployment_nation_this_pop) {
	auto type = ws.world.pop_get_poptype(to_pop(this_slot));

	auto pop_size = ve::apply(
			[&](dcon::nation_id loc, dcon::pop_type_id t) {
				if(t)
					return ws.world.nation_get_demographics(loc, demographics::to_key(ws, t));
				else
					return 0.0f;
			},
			to_nation(primary_slot), type);

	auto employment = ve::apply(
			[&](dcon::nation_id loc, dcon::pop_type_id t) {
				if(t)
					return ws.world.nation_get_demographics(loc, demographics::to_employment_key(ws, t));
				else
					return 0.0f;
			},
			to_nation(primary_slot), type);

	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_pop_unemployment_state_this_pop) {
	auto type = ws.world.pop_get_poptype(to_pop(this_slot));

	auto pop_size = ve::apply(
			[&](dcon::state_instance_id loc, dcon::pop_type_id t) {
				if(t)
					return ws.world.state_instance_get_demographics(loc, demographics::to_key(ws, t));
				else
					return 0.0f;
			},
			to_state(primary_slot), type);

	auto employment = ve::apply(
			[&](dcon::state_instance_id loc, dcon::pop_type_id t) {
				if(t)
					return ws.world.state_instance_get_demographics(loc, demographics::to_employment_key(ws, t));
				else
					return 0.0f;
			},
			to_state(primary_slot), type);

	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_pop_unemployment_province_this_pop) {
	auto type = ws.world.pop_get_poptype(to_pop(this_slot));

	auto pop_size = ve::apply(
			[&](dcon::province_id loc, dcon::pop_type_id t) {
				if(t)
					return ws.world.province_get_demographics(loc, demographics::to_key(ws, t));
				else
					return 0.0f;
			},
			to_prov(primary_slot), type);

	auto employment = ve::apply(
			[&](dcon::province_id loc, dcon::pop_type_id t) {
				if(t)
					return ws.world.province_get_demographics(loc, demographics::to_employment_key(ws, t));
				else
					return 0.0f;
			},
			to_prov(primary_slot), type);

	return compare_values(tval[0], ve::select(pop_size > 0.0f, 1.0f - (employment / pop_size), 0.0f),
			read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_relation_tag) {
	auto holder = ws.world.national_identity_get_nation_from_identity_holder(payload(tval[2]).tag_id);
	auto relation = ve::apply(
			[&ws, holder](dcon::nation_id a) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, holder);
				return ws.world.diplomatic_relation_get_value(rel);
			},
			to_nation(primary_slot));
	return compare_values(tval[0], relation, float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_relation_this_nation) {
	auto relation = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				return ws.world.diplomatic_relation_get_value(rel);
			},
			to_nation(primary_slot), to_nation(this_slot));
	return compare_values(tval[0], relation, float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_relation_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	auto relation = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				return ws.world.diplomatic_relation_get_value(rel);
			},
			to_nation(primary_slot), owner);
	return compare_values(tval[0], relation, float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_relation_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	auto relation = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				return ws.world.diplomatic_relation_get_value(rel);
			},
			to_nation(primary_slot), owner);
	return compare_values(tval[0], relation, float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_relation_from_nation) {
	auto relation = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				return ws.world.diplomatic_relation_get_value(rel);
			},
			to_nation(primary_slot), to_nation(from_slot));
	return compare_values(tval[0], relation, float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_relation_from_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(from_slot));
	auto relation = ve::apply(
			[&ws](dcon::nation_id a, dcon::nation_id b) {
				auto rel = ws.world.get_diplomatic_relation_by_diplomatic_pair(a, b);
				return ws.world.diplomatic_relation_get_value(rel);
			},
			to_nation(primary_slot), owner);
	return compare_values(tval[0], relation, float(payload(tval[1]).signed_value));
}
TRIGGER_FUNCTION(tf_check_variable) {
	auto id = payload(tval[3]).natv_id;
	return compare_values(tval[0], ws.world.nation_get_variables(to_nation(primary_slot), id), read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_upper_house) {
	auto id = payload(tval[3]).ideo_id;
	return compare_values(tval[0], ws.world.nation_get_upper_house(to_nation(primary_slot), id), 100.0f * read_float_from_payload(tval + 1));
}
TRIGGER_FUNCTION(tf_unemployment_by_type_nation) {
	return tf_pop_unemployment_nation<return_type>(tval, ws, primary_slot, int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_unemployment_by_type_state) {
	return tf_pop_unemployment_state<return_type>(tval, ws, primary_slot, int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_unemployment_by_type_province) {
	return tf_pop_unemployment_province<return_type>(tval, ws, primary_slot, int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_unemployment_by_type_pop) {
	auto location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto si = ws.world.province_get_state_membership(location);
	return tf_unemployment_by_type_state<return_type>(tval, ws, to_generic(si), int32_t(), int32_t());
}
TRIGGER_FUNCTION(tf_party_loyalty_nation_province_id) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(payload(tval[1]).prov_id, payload(tval[3]).ideo_id),
			payload(tval[2]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_nation_province_id) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(payload(tval[1]).prov_id, payload(tval[3]).ideo_id),
			payload(tval[2]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_province_province_id) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(payload(tval[1]).prov_id, payload(tval[3]).ideo_id),
			payload(tval[2]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_province_province_id) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(payload(tval[1]).prov_id, payload(tval[3]).ideo_id),
			payload(tval[2]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_nation_from_province) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(to_prov(from_slot), payload(tval[2]).ideo_id),
			payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_generic) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(to_prov(primary_slot), payload(tval[2]).ideo_id),
			payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_nation_scope_province) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(to_prov(primary_slot), payload(tval[2]).ideo_id),
			payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_party_loyalty_from_province_scope_province) {
	return compare_values(tval[0], ws.world.province_get_party_loyalty(to_prov(primary_slot), payload(tval[2]).ideo_id),
			payload(tval[1]).signed_value);
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_no_limit_from_nation) {
	return compare_to_true(tval[0],
			ve::to_float(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::railroad))) +
							ws.world.province_get_modifier_values(to_prov(primary_slot), sys::provincial_mod_offsets::min_build_railroad) <
					ve::to_float(ws.world.nation_get_max_building_level(to_nation(from_slot), uint8_t(economy::province_building_type::railroad))));
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_yes_limit_from_nation) {
	return compare_to_true(tval[0],
			ve::to_float(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::railroad))) +
							ws.world.province_get_modifier_values(to_prov(primary_slot), sys::provincial_mod_offsets::min_build_railroad) <
					ve::to_float(ws.world.nation_get_max_building_level(to_nation(from_slot), uint8_t(economy::province_building_type::railroad))));
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_no_limit_this_nation) {
	return compare_to_true(tval[0],
			ve::to_float(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::railroad))) +
							ws.world.province_get_modifier_values(to_prov(primary_slot), sys::provincial_mod_offsets::min_build_railroad) <
					ve::to_float(ws.world.nation_get_max_building_level(to_nation(this_slot), uint8_t(economy::province_building_type::railroad))));
}
TRIGGER_FUNCTION(tf_can_build_in_province_railroad_yes_limit_this_nation) {
	return compare_to_true(tval[0],
			ve::to_float(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::railroad))) +
							ws.world.province_get_modifier_values(to_prov(primary_slot), sys::provincial_mod_offsets::min_build_railroad) <
					ve::to_float(ws.world.nation_get_max_building_level(to_nation(this_slot), uint8_t(economy::province_building_type::railroad))));
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_no_limit_from_nation) {
	return compare_to_true(tval[0],
			ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::fort)) < ws.world.nation_get_max_building_level(to_nation(from_slot), uint8_t(economy::province_building_type::fort)));
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_yes_limit_from_nation) {
	return compare_to_true(tval[0],
			ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::fort)) < ws.world.nation_get_max_building_level(to_nation(from_slot), uint8_t(economy::province_building_type::fort)));
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_no_limit_this_nation) {
	return compare_to_true(tval[0],
			ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::fort)) < ws.world.nation_get_max_building_level(to_nation(this_slot), uint8_t(economy::province_building_type::fort)));
}
TRIGGER_FUNCTION(tf_can_build_in_province_fort_yes_limit_this_nation) {
	return compare_to_true(tval[0],
			ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::fort)) < ws.world.nation_get_max_building_level(to_nation(this_slot), uint8_t(economy::province_building_type::fort)));
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_no_limit_from_nation) {
	auto result = ws.world.province_get_is_coast(to_prov(primary_slot)) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) <
										ws.world.nation_get_max_building_level(to_nation(from_slot), uint8_t(economy::province_building_type::naval_base))) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) != 0 ||
										ve::apply([&ws](dcon::state_instance_id i) { return !(military::state_has_naval_base(ws, i)); },
												ws.world.province_get_state_membership(to_prov(primary_slot))));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_yes_limit_from_nation) {
	auto result = ws.world.province_get_is_coast(to_prov(primary_slot)) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) <
										ws.world.nation_get_max_building_level(to_nation(from_slot), uint8_t(economy::province_building_type::naval_base))) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) != 0 ||
										ve::apply([&ws](dcon::state_instance_id i) { return !(military::state_has_naval_base(ws, i)); },
												ws.world.province_get_state_membership(to_prov(primary_slot))));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_no_limit_this_nation) {
	auto result = ws.world.province_get_is_coast(to_prov(primary_slot)) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) <
										ws.world.nation_get_max_building_level(to_nation(this_slot), uint8_t(economy::province_building_type::naval_base))) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) != 0 ||
										ve::apply([&ws](dcon::state_instance_id i) { return !(military::state_has_naval_base(ws, i)); },
												ws.world.province_get_state_membership(to_prov(primary_slot))));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_can_build_in_province_naval_base_yes_limit_this_nation) {
	auto result = ws.world.province_get_is_coast(to_prov(primary_slot)) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) <
										ws.world.nation_get_max_building_level(to_nation(this_slot), uint8_t(economy::province_building_type::naval_base))) &&
								(ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::naval_base)) != 0 ||
										ve::apply([&ws](dcon::state_instance_id i) { return !(military::state_has_naval_base(ws, i)); },
												ws.world.province_get_state_membership(to_prov(primary_slot))));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_yes_whole_state_yes_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_yes_whole_state_no_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_no_whole_state_yes_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_railway_in_capital_no_whole_state_no_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_yes_whole_state_yes_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_yes_whole_state_no_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_no_whole_state_yes_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_can_build_fort_in_capital_no_whole_state_no_limit) {
	// stub: virtually unused
	return compare_to_true(tval[0], true);
}
TRIGGER_FUNCTION(tf_work_available_nation) {
	auto type = payload(tval[1]).popt_id;
	auto result = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_employment_key(ws, type)) >=
								ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, type));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_work_available_state) {
	auto type = payload(tval[1]).popt_id;
	auto result = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_employment_key(ws, type)) >=
								ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, type));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_work_available_province) {
	auto type = payload(tval[1]).popt_id;
	auto result = ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_employment_key(ws, type)) >=
								ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, type));
	return compare_to_true(tval[0], result);
}
TRIGGER_FUNCTION(tf_variable_ideology_name_nation) {
	auto id = payload(tval[1]).ideo_id;
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	auto support_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_ideology_name_state) {
	auto id = payload(tval[1]).ideo_id;
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	auto support_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_ideology_name_province) {
	auto id = payload(tval[1]).ideo_id;
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	auto support_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_ideology_name_pop) {
	auto id = payload(tval[1]).ideo_id;
	auto total_pop = ws.world.pop_get_size(to_pop(primary_slot));
	auto support_pop = ws.world.pop_get_demographics(to_pop(primary_slot), pop_demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_issue_name_nation) {
	auto id = payload(tval[1]).opt_id;
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	auto support_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_issue_name_state) {
	auto id = payload(tval[1]).opt_id;
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	auto support_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_issue_name_province) {
	auto id = payload(tval[1]).opt_id;
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	auto support_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_issue_name_pop) {
	auto id = payload(tval[1]).opt_id;
	auto total_pop = ws.world.pop_get_size(to_pop(primary_slot));
	auto support_pop = ws.world.pop_get_demographics(to_pop(primary_slot), pop_demographics::to_key(ws, id));
	return compare_values(tval[0], ve::select(total_pop > 0.0f, support_pop / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_nation) {
	auto option = payload(tval[2]).opt_id;
	auto issue = payload(tval[1]).iss_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_issues(to_nation(primary_slot), issue), option);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto option = payload(tval[2]).opt_id;
	auto issue = payload(tval[1]).iss_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_issues(owner, issue), option);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto option = payload(tval[2]).opt_id;
	auto issue = payload(tval[1]).iss_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_issues(owner, issue), option);
}
TRIGGER_FUNCTION(tf_variable_issue_group_name_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto option = payload(tval[2]).opt_id;
	auto issue = payload(tval[1]).iss_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_issues(owner, issue), option);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_nation) {
	auto option = payload(tval[2]).ropt_id;
	auto issue = payload(tval[1]).ref_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_reforms(to_nation(primary_slot), issue), option);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(primary_slot));
	auto option = payload(tval[2]).ropt_id;
	auto issue = payload(tval[1]).ref_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_reforms(owner, issue), option);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot));
	auto option = payload(tval[2]).ropt_id;
	auto issue = payload(tval[1]).ref_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_reforms(owner, issue), option);
}
TRIGGER_FUNCTION(tf_variable_reform_group_name_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(primary_slot));
	auto option = payload(tval[2]).ropt_id;
	auto issue = payload(tval[1]).ref_id;
	;
	return compare_values_eq(tval[0], ws.world.nation_get_reforms(owner, issue), option);
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_nation) {
	auto total_pop = ws.world.nation_get_demographics(to_nation(primary_slot), demographics::total);
	auto type = demographics::to_key(ws, payload(tval[1]).popt_id);
	auto size = ws.world.nation_get_demographics(to_nation(primary_slot), type);
	return compare_values(tval[0], ve::select(total_pop > 0.0f, size / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_state) {
	auto total_pop = ws.world.state_instance_get_demographics(to_state(primary_slot), demographics::total);
	auto type = demographics::to_key(ws, payload(tval[1]).popt_id);
	auto size = ws.world.state_instance_get_demographics(to_state(primary_slot), type);
	return compare_values(tval[0], ve::select(total_pop > 0.0f, size / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_province) {
	auto total_pop = ws.world.province_get_demographics(to_prov(primary_slot), demographics::total);
	auto type = demographics::to_key(ws, payload(tval[1]).popt_id);
	auto size = ws.world.province_get_demographics(to_prov(primary_slot), type);
	return compare_values(tval[0], ve::select(total_pop > 0.0f, size / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_pop_type_name_pop) {
	auto pop_location = ws.world.pop_get_province_from_pop_location(to_pop(primary_slot));
	auto total_pop = ws.world.province_get_demographics(pop_location, demographics::total);
	auto type = demographics::to_key(ws, payload(tval[1]).popt_id);
	auto size = ws.world.province_get_demographics(pop_location, type);
	return compare_values(tval[0], ve::select(total_pop > 0.0f, size / total_pop, 0.0f), read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_variable_good_name) {
	auto good = payload(tval[1]).com_id;
	auto amount = ws.world.nation_get_stockpiles(to_nation(primary_slot), good);
	return compare_values(tval[0], amount, read_float_from_payload(tval + 2));
}
TRIGGER_FUNCTION(tf_religion_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)), payload(tval[1]).rel_id);
}
TRIGGER_FUNCTION(tf_religion_nation_reb) {
	auto r = ws.world.rebel_faction_get_religion(to_rebel(from_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)), r);
}
TRIGGER_FUNCTION(tf_religion_nation_from_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)),
			ws.world.nation_get_religion(to_nation(from_slot)));
}
TRIGGER_FUNCTION(tf_religion_nation_this_nation) {
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)),
			ws.world.nation_get_religion(to_nation(this_slot)));
}
TRIGGER_FUNCTION(tf_religion_nation_this_state) {
	auto owner = ws.world.state_instance_get_nation_from_state_ownership(to_state(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)), ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_religion_nation_this_province) {
	auto owner = ws.world.province_get_nation_from_province_ownership(to_prov(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)), ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_religion_nation_this_pop) {
	auto owner = nations::owner_of_pop(ws, to_pop(this_slot));
	return compare_values_eq(tval[0], ws.world.nation_get_religion(to_nation(primary_slot)), ws.world.nation_get_religion(owner));
}
TRIGGER_FUNCTION(tf_invention) {
	auto tid = trigger::payload(tval[1]).invt_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_inventions(to_nation(primary_slot), tid));
}
TRIGGER_FUNCTION(tf_invention_province) {
	auto tid = trigger::payload(tval[1]).invt_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_inventions(ws.world.province_get_nation_from_province_ownership(to_prov(primary_slot)), tid));
}
TRIGGER_FUNCTION(tf_invention_pop) {
	auto tid = trigger::payload(tval[1]).invt_id;
	return compare_to_true(tval[0], ws.world.nation_get_active_inventions(nations::owner_of_pop(ws, to_pop(primary_slot)), tid));
}
TRIGGER_FUNCTION(tf_test) {
	auto sid = trigger::payload(tval[1]).str_id;
	auto tid = ws.world.stored_trigger_get_function(sid);
	auto test_result = test_trigger_generic<return_type>(ws.trigger_data.data() + ws.trigger_data_indices[tid.index() + 1], ws, primary_slot, this_slot, from_slot);
	return compare_to_true(tval[0], test_result);
}

TRIGGER_FUNCTION(tf_has_building_bank) {
	return compare_to_true(tval[0], ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::bank)) != 0);
}
TRIGGER_FUNCTION(tf_has_building_university) {
	return compare_to_true(tval[0], ws.world.province_get_building_level(to_prov(primary_slot), uint8_t(economy::province_building_type::university)) != 0);
}

TRIGGER_FUNCTION(tf_party_name) {
	auto ideo = trigger::payload(tval[1]).ideo_id;
	dcon::text_key new_name{ dcon::text_key::value_base_t(trigger::read_int32_t_from_payload(tval + 2)) };

	if(ideo) {
		auto nat = trigger::to_nation(primary_slot);
		auto holder = ws.world.nation_get_identity_from_identity_holder(nat);

		return compare_to_true(tval[0], ve::apply([&ws, ideo, new_name](dcon::national_identity_id h, dcon::nation_id n) {
			auto start = ws.world.national_identity_get_political_party_first(h).id.index();
			auto end = start + ws.world.national_identity_get_political_party_count(h);

			for(int32_t i = start; i < end; i++) {
				auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
				if(politics::political_party_is_active(ws, n, pid) && ws.world.political_party_get_ideology(pid) == ideo) {
					return ws.world.political_party_get_name(pid) == new_name;
				}
			}
			return false;

			}, holder, nat));
		
	} else {
		auto n = trigger::to_nation(primary_slot);
		auto rp = ws.world.nation_get_ruling_party(n);
		return compare_to_true(tval[0], ws.world.political_party_get_name(rp) == new_name);
	}
}

TRIGGER_FUNCTION(tf_party_position) {
	auto ideo = trigger::payload(tval[1]).ideo_id;
	dcon::issue_option_id new_opt = trigger::payload(tval[2]).opt_id;
	auto popt = ws.world.issue_option_get_parent_issue(new_opt);

	if(ideo) {
		auto nat = trigger::to_nation(primary_slot);
		auto holder = ws.world.nation_get_identity_from_identity_holder(nat);

		return compare_to_true(tval[0], ve::apply([&ws, ideo, new_opt, popt](dcon::national_identity_id h, dcon::nation_id n) {
			auto start = ws.world.national_identity_get_political_party_first(h).id.index();
			auto end = start + ws.world.national_identity_get_political_party_count(h);

			for(int32_t i = start; i < end; i++) {
				auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(i));
				if(politics::political_party_is_active(ws, n, pid) && ws.world.political_party_get_ideology(pid) == ideo) {
					return ws.world.political_party_get_party_issues(pid, popt) == new_opt;
				}
			}
			return false;

		}, holder, nat));
	} else {
		auto n = trigger::to_nation(primary_slot);
		auto rp = ws.world.nation_get_ruling_party(n);
		return compare_to_true(tval[0], ws.world.political_party_get_party_issues(rp, popt) == new_opt);
	}
}

template<typename return_type, typename primary_type, typename this_type, typename from_type>
struct trigger_container {
	constexpr static return_type(
			CALLTYPE* trigger_functions[])(uint16_t const*, sys::state&, primary_type, this_type, from_type) = {
			tf_none<return_type, primary_type, this_type, from_type>,
#define TRIGGER_BYTECODE_ELEMENT(code, name, arg) tf_##name <return_type, primary_type, this_type, from_type>,
			TRIGGER_BYTECODE_LIST
#undef TRIGGER_BYTECODE_ELEMENT
			//
			// scopes
			//
			tf_generic_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t generic_scope = 0x0000; // or & and
			tf_x_neighbor_province_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_neighbor_province_scope = 0x0001;
			tf_x_neighbor_country_scope_nation<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_neighbor_country_scope_nation = 0x0002;
			tf_x_neighbor_country_scope_pop<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_neighbor_country_scope_pop = 0x0003;
			tf_x_war_countries_scope_nation<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_war_countries_scope_nation = 0x0004;
			tf_x_war_countries_scope_pop<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_war_countries_scope_pop = 0x0005;
			tf_x_greater_power_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_greater_power_scope = 0x0006;
			tf_x_owned_province_scope_state<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_owned_province_scope_state = 0x0007;
			tf_x_owned_province_scope_nation<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_owned_province_scope_nation = 0x0008;
			tf_x_core_scope_province<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_core_scope_province = 0x0009;
			tf_x_core_scope_nation<return_type, primary_type, this_type, from_type>,	 // constexpr uint16_t x_core_scope_nation = 0x000A;
			tf_x_state_scope<return_type, primary_type, this_type, from_type>,		// constexpr uint16_t x_state_scope = 0x000B;
			tf_x_substate_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_substate_scope = 0x000C;
			tf_x_sphere_member_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_sphere_member_scope = 0x000D;
			tf_x_pop_scope_province<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_pop_scope_province = 0x000E;
			tf_x_pop_scope_state<return_type, primary_type, this_type, from_type>,	// constexpr uint16_t x_pop_scope_state = 0x000F;
			tf_x_pop_scope_nation<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_pop_scope_nation = 0x0010;
			tf_x_provinces_in_variable_region<return_type, primary_type, this_type, from_type>, // constexpr uint16_t x_provinces_in_variable_region = 0x0011; // variable name
			tf_owner_scope_state<return_type, primary_type, this_type, from_type>, // constexpr uint16_t owner_scope_state = 0x0012;
			tf_owner_scope_province<return_type, primary_type, this_type, from_type>,  // constexpr uint16_t owner_scope_province = 0x0013;
			tf_controller_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t controller_scope = 0x0014;
			tf_location_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t location_scope = 0x0015;
			tf_country_scope_state<return_type, primary_type, this_type, from_type>, // constexpr uint16_t country_scope_state = 0x0016;
			tf_country_scope_pop<return_type, primary_type, this_type, from_type>, // constexpr uint16_t country_scope_pop = 0x0017;
			tf_capital_scope<return_type, primary_type, this_type, from_type>,		 // constexpr uint16_t capital_scope = 0x0018;
			tf_this_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t this_scope_pop = 0x0019;
			tf_this_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t this_scope_nation = 0x001A;
			tf_this_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t this_scope_state = 0x001B;
			tf_this_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t this_scope_province = 0x001C;
			tf_from_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t from_scope_pop = 0x001D;
			tf_from_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t from_scope_nation = 0x001E;
			tf_from_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t from_scope_state = 0x001F;
			tf_from_scope<return_type, primary_type, this_type, from_type>,				 // constexpr uint16_t from_scope_province = 0x0020;
			tf_sea_zone_scope<return_type, primary_type, this_type, from_type>,		 // constexpr uint16_t sea_zone_scope = 0x0021;
			tf_cultural_union_scope<return_type, primary_type, this_type, from_type>,  // constexpr uint16_t cultural_union_scope = 0x0022;
			tf_overlord_scope<return_type, primary_type, this_type, from_type>,			// constexpr uint16_t overlord_scope = 0x0023;
			tf_sphere_owner_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t sphere_owner_scope = 0x0024;
			tf_independence_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t independence_scope = 0x0025;
			tf_flashpoint_tag_scope<return_type, primary_type, this_type, from_type>,	 // constexpr uint16_t flashpoint_tag_scope = 0x0026;
			tf_crisis_state_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t crisis_state_scope = 0x0027;
			tf_state_scope_pop<return_type, primary_type, this_type, from_type>,		// constexpr uint16_t state_scope_pop = 0x0028;
			tf_state_scope_province<return_type, primary_type, this_type, from_type>,	 // constexpr uint16_t state_scope_province = 0x0029;
			tf_tag_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t tag_scope = 0x002A; // variable name
			tf_integer_scope<return_type, primary_type, this_type, from_type>, // constexpr uint16_t integer_scope = 0x002B; // variable name
			tf_country_scope_nation<return_type, primary_type, this_type, from_type>, // constexpr uint16_t country_scope_nation = 0x002C;
			tf_country_scope_province<return_type, primary_type, this_type, from_type>, // constexpr uint16_t country_scope_province = 0x002D;
			tf_cultural_union_scope_pop<return_type, primary_type, this_type, from_type>, // constexpr uint16_t cultural_union_scope_pop = 0x002E;
			tf_capital_scope_province<return_type, primary_type, this_type, from_type>, // constexpr uint16_t capital_scope_province = 0x002F;
			tf_capital_scope_pop, //constexpr inline uint16_t capital_scope_pop = first_scope_code + 0x0030;
			tf_x_country_scope, //constexpr inline uint16_t x_country_scope = first_scope_code + 0x0031;
			tf_x_neighbor_province_scope_state, //constexpr inline uint16_t x_neighbor_province_scope_state = first_scope_code + 0x0032;
			tf_x_provinces_in_variable_region_proper, //constexpr inline uint16_t x_provinces_in_variable_region_proper = first_scope_code + 0x0033;
	};
};

template<typename return_type, typename primary_type, typename this_type, typename from_type>
return_type CALLTYPE test_trigger_generic(uint16_t const* tval, sys::state& ws, primary_type primary_slot, this_type this_slot,
		from_type from_slot) {
	return trigger_container<return_type, primary_type, this_type, from_type>::trigger_functions[*tval & trigger::code_mask](tval,
			ws, primary_slot, this_slot, from_slot);
}

#undef CALLTYPE
#undef TRIGGER_FUNCTION

float evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];
	float product = base.factor;
	for(uint32_t i = 0; i < base.segments_count && product != 0; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			if(test_trigger_generic<bool>(state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state, primary,
						 this_slot, from_slot)) {
				product *= seg.factor;
			}
		}
	}
	return product;
}
float evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];
	float sum = base.base;
	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			if(test_trigger_generic<bool>(state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state, primary,
						 this_slot, from_slot)) {
				sum += seg.factor;
			}
		}
	}
	return sum * base.factor;
}

ve::fp_vector evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier, ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];
	ve::fp_vector product = base.factor;
	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			auto res = test_trigger_generic<ve::mask_vector>(
					state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state, primary, this_slot, from_slot);
			product = ve::select(res, product * seg.factor, product);
		}
	}
	return product;
}
ve::fp_vector evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier, ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];
	ve::fp_vector sum = base.base;
	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			auto res = test_trigger_generic<ve::mask_vector>(
					state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state, primary, this_slot, from_slot);
			sum = ve::select(res, sum + seg.factor, sum);
		}
	}
	return sum * base.factor;
}

ve::fp_vector evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier, ve::contiguous_tags<int32_t> primary, ve::contiguous_tags<int32_t> this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];
	ve::fp_vector product = base.factor;
	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			auto res = test_trigger_generic<ve::mask_vector>(
					state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state, primary, this_slot, from_slot);
			product = ve::select(res, product * seg.factor, product);
		}
	}
	return product;
}
ve::fp_vector evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier, ve::contiguous_tags<int32_t> primary, ve::contiguous_tags<int32_t> this_slot, int32_t from_slot) {
	auto base = state.value_modifiers[modifier];
	ve::fp_vector sum = base.base;
	for(uint32_t i = 0; i < base.segments_count; ++i) {
		auto seg = state.value_modifier_segments[base.first_segment_offset + i];
		if(seg.condition) {
			auto res = test_trigger_generic<ve::mask_vector>(
					state.trigger_data.data() + state.trigger_data_indices[seg.condition.index() + 1], state, primary, this_slot, from_slot);
			sum = ve::select(res, sum + seg.factor, sum);
		}
	}
	return sum * base.factor;
}

bool evaluate(sys::state& state, dcon::trigger_key key, int32_t primary, int32_t this_slot, int32_t from_slot) {
	return test_trigger_generic<bool>(state.trigger_data.data() + state.trigger_data_indices[key.index() + 1], state, primary,
			this_slot, from_slot);
}
bool evaluate(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot) {
	return test_trigger_generic<bool>(data, state, primary, this_slot, from_slot);
}

ve::mask_vector evaluate(sys::state& state, dcon::trigger_key key, ve::contiguous_tags<int32_t> primary,
		ve::tagged_vector<int32_t> this_slot, int32_t from_slot) {
	return test_trigger_generic<ve::mask_vector>(state.trigger_data.data() + state.trigger_data_indices[key.index() + 1], state,
			primary, this_slot, from_slot);
}
ve::mask_vector evaluate(sys::state& state, uint16_t const* data, ve::contiguous_tags<int32_t> primary,
		ve::tagged_vector<int32_t> this_slot, int32_t from_slot) {
	return test_trigger_generic<ve::mask_vector>(data, state, primary, this_slot, from_slot);
}

ve::mask_vector evaluate(sys::state& state, dcon::trigger_key key, ve::tagged_vector<int32_t> primary,
		ve::tagged_vector<int32_t> this_slot, int32_t from_slot) {
	return test_trigger_generic<ve::mask_vector>(state.trigger_data.data() + state.trigger_data_indices[key.index() + 1], state,
			primary, this_slot, from_slot);
}
ve::mask_vector evaluate(sys::state& state, uint16_t const* data, ve::tagged_vector<int32_t> primary,
		ve::tagged_vector<int32_t> this_slot, int32_t from_slot) {
	return test_trigger_generic<ve::mask_vector>(data, state, primary, this_slot, from_slot);
}

ve::mask_vector evaluate(sys::state& state, dcon::trigger_key key, ve::contiguous_tags<int32_t> primary,
		ve::contiguous_tags<int32_t> this_slot, int32_t from_slot) {
	return test_trigger_generic<ve::mask_vector>(state.trigger_data.data() + state.trigger_data_indices[key.index() + 1], state,
			primary, this_slot, from_slot);
}
ve::mask_vector evaluate(sys::state& state, uint16_t const* data, ve::contiguous_tags<int32_t> primary,
		ve::contiguous_tags<int32_t> this_slot, int32_t from_slot) {
	return test_trigger_generic<ve::mask_vector>(data, state, primary, this_slot, from_slot);
}

} // namespace trigger
