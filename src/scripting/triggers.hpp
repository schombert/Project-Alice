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

float evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot, int32_t from_slot);
float evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier, int32_t primary, int32_t this_slot, int32_t from_slot);

ve::fp_vector evaluate_multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier, ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::fp_vector evaluate_additive_modifier(sys::state& state, dcon::value_modifier_key modifier, ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot);

bool evaluate_trigger(sys::state& state, dcon::trigger_key key, int32_t primary, int32_t this_slot, int32_t from_slot);
bool evaluate_trigger(sys::state& state, uint16_t const* data, int32_t primary, int32_t this_slot, int32_t from_slot);

ve::mask_vector evaluate_trigger(sys::state& state, dcon::trigger_key key, ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot);
ve::mask_vector evaluate_trigger(sys::state& state, uint16_t const* data, ve::contiguous_tags<int32_t> primary, ve::tagged_vector<int32_t> this_slot, int32_t from_slot);

}
