#pragma once

#include "script_constants.hpp"
#include "dcon_generated.hpp"
#include "container_types.hpp"

namespace trigger {

inline int32_t to_generic(dcon::province_id v) {
	return v.index();
}
inline ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::province_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
inline ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::province_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
inline ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::province_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
inline ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::province_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}
inline int32_t to_generic(dcon::nation_id v) {
	return v.index();
}
inline ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::nation_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
inline ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::nation_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
inline ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::nation_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
inline ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::nation_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}
inline int32_t to_generic(dcon::pop_id v) {
	return v.index();
}
inline ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::pop_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
inline ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::pop_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
inline ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::pop_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
inline ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::pop_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}
inline int32_t to_generic(dcon::state_instance_id v) {
	return v.index();
}
inline ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::state_instance_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
inline ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::state_instance_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
inline ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::state_instance_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
inline ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::state_instance_id> v) {
	return ve::partial_contiguous_tags<int32_t>(v.value, v.subcount);
}
inline int32_t to_generic(dcon::rebel_faction_id v) {
	return v.index();
}
inline ve::tagged_vector<int32_t> to_generic(ve::tagged_vector<dcon::rebel_faction_id> v) {
	return ve::tagged_vector<int32_t>(v);
}
inline ve::contiguous_tags<int32_t> to_generic(ve::contiguous_tags<dcon::rebel_faction_id> v) {
	return ve::contiguous_tags<int32_t>(v.value);
}
inline ve::unaligned_contiguous_tags<int32_t> to_generic(ve::unaligned_contiguous_tags<dcon::rebel_faction_id> v) {
	return ve::unaligned_contiguous_tags<int32_t>(v.value);
}
inline ve::partial_contiguous_tags<int32_t> to_generic(ve::partial_contiguous_tags<dcon::rebel_faction_id> v) {
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

inline dcon::pop_id to_pop(int32_t v) {
	return dcon::pop_id(dcon::pop_id::value_base_t(v));
}
inline ve::tagged_vector<dcon::pop_id> to_pop(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::pop_id>(v, std::true_type{});
}
inline ve::contiguous_tags<dcon::pop_id> to_pop(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::pop_id>(v.value);
}
inline ve::unaligned_contiguous_tags<dcon::pop_id> to_pop(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::pop_id>(v.value);
}
inline ve::partial_contiguous_tags<dcon::pop_id> to_pop(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::pop_id>(v.value, v.subcount);
}

inline dcon::state_instance_id to_state(int32_t v) {
	return dcon::state_instance_id(dcon::state_instance_id::value_base_t(v));
}
inline ve::tagged_vector<dcon::state_instance_id> to_state(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::state_instance_id>(v, std::true_type{});
}
inline ve::contiguous_tags<dcon::state_instance_id> to_state(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::state_instance_id>(v.value);
}
inline ve::unaligned_contiguous_tags<dcon::state_instance_id> to_state(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::state_instance_id>(v.value);
}
inline ve::partial_contiguous_tags<dcon::state_instance_id> to_state(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::state_instance_id>(v.value, v.subcount);
}

inline dcon::rebel_faction_id to_rebel(int32_t v) {
	return dcon::rebel_faction_id(dcon::rebel_faction_id::value_base_t(v));
}
inline ve::tagged_vector<dcon::rebel_faction_id> to_rebel(ve::tagged_vector<int32_t> v) {
	return ve::tagged_vector<dcon::rebel_faction_id>(v, std::true_type{});
}
inline ve::contiguous_tags<dcon::rebel_faction_id> to_rebel(ve::contiguous_tags<int32_t> v) {
	return ve::contiguous_tags<dcon::rebel_faction_id>(v.value);
}
inline ve::unaligned_contiguous_tags<dcon::rebel_faction_id> to_rebel(ve::unaligned_contiguous_tags<int32_t> v) {
	return ve::unaligned_contiguous_tags<dcon::rebel_faction_id>(v.value);
}
inline ve::partial_contiguous_tags<dcon::rebel_faction_id> to_rebel(ve::partial_contiguous_tags<int32_t> v) {
	return ve::partial_contiguous_tags<dcon::rebel_faction_id>(v.value, v.subcount);
}

float evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot,
	int32_t from_slot);
float evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot,
	int32_t from_slot);

ve::fp_vector evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier,
	ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::fp_vector evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier,
	ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot);

ve::fp_vector evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier,
	ve::contiguous_tags<int32_t> primary, ve::contiguous_tags<int32_t> this_slot, int32_t from_slot);
ve::fp_vector evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier,
	ve::contiguous_tags<int32_t> primary, ve::contiguous_tags<int32_t> this_slot, int32_t from_slot);

bool evaluate(sys::state& state, dcon::trigger_key key, int32_t primary, int32_t this_slot, int32_t from_slot);
bool evaluate(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot);

ve::mask_vector evaluate(sys::state& state, dcon::trigger_key key, ve::contiguous_tags<int32_t> primary,
	ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::mask_vector evaluate(sys::state& state, uint16_t const* data, ve::contiguous_tags<int32_t> primary,
	ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::mask_vector evaluate(sys::state& state, dcon::trigger_key key, ve::tagged_vector<int32_t> primary,
	ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::mask_vector evaluate(sys::state& state, uint16_t const* data, ve::tagged_vector<int32_t> primary,
	ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::mask_vector evaluate(sys::state& state, dcon::trigger_key key, ve::contiguous_tags<int32_t> primary,
	ve::contiguous_tags<int32_t> this_slot, int32_t from_slot);
ve::mask_vector evaluate(sys::state& state, uint16_t const* data, ve::contiguous_tags<int32_t> primary,
	ve::contiguous_tags<int32_t> this_slot, int32_t from_slot);
} // namespace trigger
