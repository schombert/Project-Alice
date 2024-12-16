#pragma once
#include "fif.hpp"
#include "politics.hpp"

namespace fif {


inline constexpr sys::year_month_day civil_from_days(int64_t z) noexcept {
	z += 719468;
	const int32_t era = int32_t((z >= 0 ? z : z - 146096) / 146097);
	unsigned const doe = static_cast<unsigned>(z - era * 146097);								// [0, 146096]
	unsigned const yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365; // [0, 399]
	const int32_t y = static_cast<int32_t>(yoe) + era * 400;
	unsigned const doy = doe - (365 * yoe + yoe / 4 - yoe / 100); // [0, 365]
	unsigned const mp = (5 * doy + 2) / 153;											// [0, 11]
	unsigned const d = doy - (153 * mp + 2) / 5 + 1;							// [1, 31]
	unsigned const m = mp < 10 ? mp + 3 : mp - 9;									// [1, 12]
	return sys::year_month_day{ int32_t(y + (m <= 2)), uint16_t(m), uint16_t(d) };
}

inline int32_t f_date_to_year(int64_t v) {
	return civil_from_days(v).year;
}
inline int32_t f_date_to_month(int64_t v) {
	return int32_t(civil_from_days(v).month);
}
inline int32_t* f_date_to_year_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
		return p + 2;
	}

	auto r = f_date_to_year(s.main_data_back(0));
	s.pop_main();
	s.push_back_main(fif::fif_i32, r, nullptr);

	return p + 2;
}
inline int32_t* f_date_to_month_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
		return p + 2;
	}

	auto r = f_date_to_month(s.main_data_back(0));
	s.pop_main();
	s.push_back_main(fif::fif_i32, r, nullptr);

	return p + 2;
}

inline bool f_empty_adjacent_province(sys::state* state, int32_t p_index) {
	dcon::province_id p{ dcon::province_id::value_base_t(p_index) };
	for(auto a : state->world.province_get_province_adjacency(p)) {
		auto other = a.get_connected_provinces(0) == p ? a.get_connected_provinces(1) : a.get_connected_provinces(0);
		if(other.get_nation_from_province_ownership() == dcon::nation_id() && other.id.index() < state->province_definitions.first_sea_province.index())
			return true;
	}
	return false;
}
inline int32_t* f_empty_adjacent_province_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t index = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_empty_adjacent_province(state, index), nullptr);

	return p + 2;
}
inline bool f_empty_adjacent_state(sys::state* state, int32_t p_index) {
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(p_index) };
	auto owner = state->world.state_instance_get_nation_from_state_ownership(s);
	for(auto p : state->world.state_instance_get_definition(s).get_abstract_state_membership()) {
		if(p.get_province().get_nation_from_province_ownership() == owner) {
			for(auto a : p.get_province().get_province_adjacency()) {
				auto other = a.get_connected_provinces(0) == p.get_province() ? a.get_connected_provinces(1) : a.get_connected_provinces(0);
				if(other.get_nation_from_province_ownership() == dcon::nation_id() && other.id.index() < state->province_definitions.first_sea_province.index())
					return true;
			}
		}
	}
	return false;
}
inline int32_t* f_empty_adjacent_state_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t index = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_empty_adjacent_state(state, index), nullptr);

	return p + 2;
}


template<typename F>
class  f_true_accumulator : public F {
private:
	ve::tagged_vector<int32_t> value;
	uint32_t index = 0;
	int32_t accumulated_mask = 0;

public:
	bool result = false;

	f_true_accumulator(F&& f) : F(std::move(f)) { }

	inline void add_value(int32_t v) {
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
	inline void flush() {
		if(index != 0 && !result) {
			result = (ve::compress_mask(F::operator()(value)).v & accumulated_mask) != 0;
			index = 0;
		}
	}
};

template<typename F>
inline auto f_make_true_accumulator(F&& f) -> f_true_accumulator<F> {
	return f_true_accumulator<F>(std::forward<F>(f));
}


inline bool f_unowned_core(sys::state* state, int32_t n_index) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(n_index) };
	auto nation_tag = state->world.nation_get_identity_from_identity_holder(n);

	auto acc = f_make_true_accumulator([state, n](ve::tagged_vector<int32_t> v) {
		auto owners = state->world.province_get_nation_from_province_ownership(trigger::to_prov(v));
		return owners != n;
	});

	for(auto p : state->world.national_identity_get_core(nation_tag)) {
		acc.add_value(trigger::to_generic(p.get_province().id));
		if(acc.result)
			return true;
	}

	acc.flush();
	return acc.result;
}
inline int32_t* f_unowned_core_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t index = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_unowned_core(state, index), nullptr);

	return p + 2;
}
inline bool f_core_in_nation(sys::state* state, int32_t n_index, int32_t t_index) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(n_index) };
	dcon::national_identity_id t{ dcon::national_identity_id::value_base_t(t_index) };

	for(auto p : state->world.nation_get_province_ownership(n)) {
		if(state->world.get_core_by_prov_tag_key(p.get_province(), t))
			return true;
	}
	return false;
}
inline int32_t* f_core_in_nation_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0));
	s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_core_in_nation(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_core_in_province(sys::state* state, int32_t p_index, int32_t t_index) {
	dcon::province_id p{ dcon::province_id::value_base_t(p_index) };
	dcon::national_identity_id t{ dcon::national_identity_id::value_base_t(t_index) };

	auto core = state->world.get_core_by_prov_tag_key(p, t);
	return bool(core);
}
inline int32_t* f_core_in_province_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0));
	s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_core_in_province(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_core_in_state(sys::state* state, int32_t s_index, int32_t t_index) {
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(s_index) };
	dcon::national_identity_id t{ dcon::national_identity_id::value_base_t(t_index) };

	auto ab = state->world.state_instance_get_definition(s);
	for(auto p : state->world.state_definition_get_abstract_state_membership(ab)) {
		if(state->world.get_core_by_prov_tag_key(p.get_province(), t))
			return true;
	}
	return false;
}
inline int32_t* f_core_in_state_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0));
	s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_core_in_state(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_core_fully_state(sys::state* state, int32_t s_index, int32_t t_index) {
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(s_index) };
	dcon::national_identity_id t{ dcon::national_identity_id::value_base_t(t_index) };

	auto ab = state->world.state_instance_get_definition(s);
	for(auto p : state->world.state_definition_get_abstract_state_membership(ab)) {
		if(!bool(state->world.get_core_by_prov_tag_key(p.get_province(), t)))
			return false;
	}
	return true;
}
inline int32_t* f_core_fully_state_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0));
	s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_core_fully_state(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_cb_against(sys::state* state, int32_t targ_index, int32_t source_index) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(source_index) };
	dcon::nation_id t{ dcon::nation_id::value_base_t(targ_index) };
	return military::can_use_cb_against(*state, s, t);
}
inline int32_t* f_has_cb_against_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.pop_main();
		s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0));
	s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0));
	s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0));
	s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_cb_against(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_access_with(sys::state* state, int32_t targ_index, int32_t source_index) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(source_index) };
	dcon::nation_id t{ dcon::nation_id::value_base_t(targ_index) };
	if(auto p = state->world.get_unilateral_relationship_by_unilateral_pair(t, s); p) {
		return state->world.unilateral_relationship_get_military_access(p);
	}
	return false;
}
inline int32_t* f_has_access_with_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_access_with(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_any_factory(sys::state* state, int32_t s_index) {
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(s_index) };
	return economy::has_factory(*state, s);
}
inline int32_t* f_has_any_factory_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_any_factory(state, nindex), nullptr);

	return p + 2;
}
inline bool f_has_factory(sys::state* state, int32_t findex, int32_t s_index) {
	dcon::factory_type_id f{ dcon::factory_type_id::value_base_t(findex) };
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(s_index) };
	return economy::has_building(*state, s, f);
}
inline int32_t* f_has_factory_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_factory(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_nmod(sys::state* state, int32_t m_index, int32_t n_index) {
	dcon::modifier_id f{ dcon::modifier_id::value_base_t(m_index) };
	dcon::nation_id s{ dcon::nation_id::value_base_t(n_index) };
	for(auto m : state->world.nation_get_current_modifiers(s)) {
		if(m.mod_id == f)
			return true;
	}
	return false;
}
inline int32_t* f_has_nmod_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_nmod(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_pmod(sys::state* state, int32_t m_index, int32_t p_index) {
	dcon::modifier_id f{ dcon::modifier_id::value_base_t(m_index) };
	dcon::province_id s{ dcon::province_id::value_base_t(p_index) };
	for(auto m : state->world.province_get_current_modifiers(s)) {
		if(m.mod_id == f)
			return true;
	}
	return false;
}
inline int32_t* f_has_pmod_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_pmod(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_in_region(sys::state* state, int32_t rindex, int32_t p_index) {
	dcon::region_id f{ dcon::region_id::value_base_t(rindex) };
	dcon::province_id s{ dcon::province_id::value_base_t(p_index) };
	for(auto m : state->world.province_get_region_membership(s)) {
		if(m.get_region() == f)
			return true;
	}
	return false;
}
inline int32_t* f_in_region_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_in_region(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_owns_region(sys::state* state, int32_t rindex, int32_t n_index) {
	dcon::region_id f{ dcon::region_id::value_base_t(rindex) };
	dcon::nation_id s{ dcon::nation_id::value_base_t(n_index) };
	for(auto m : state->world.region_get_region_membership(f)) {
		if(m.get_province().get_nation_from_province_ownership() != s)
			return false;
	}
	return true;
}
inline int32_t* f_owns_region_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_owns_region(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_owns_state(sys::state* state, int32_t rindex, int32_t n_index) {
	dcon::state_definition_id f{ dcon::state_definition_id::value_base_t(rindex) };
	dcon::nation_id s{ dcon::nation_id::value_base_t(n_index) };
	for(auto m : state->world.state_definition_get_abstract_state_membership(f)) {
		if(m.get_province().get_nation_from_province_ownership() != s)
			return false;
	}
	return true;
}
inline int32_t* f_owns_state_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_owns_state(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_nation_a(sys::state* state, int32_t aindex, int32_t bindex) {
	dcon::nation_id a{ dcon::nation_id::value_base_t(aindex) };
	dcon::nation_id b{ dcon::nation_id::value_base_t(bindex) };
	return bool(state->world.get_nation_adjacency_by_nation_adjacency_pair(a, b));
}
inline int32_t* f_nation_a_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_nation_a(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_units_in_state(sys::state* state, int32_t sindex, int32_t nindex) {
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(sindex) };
	dcon::nation_id n{ dcon::nation_id::value_base_t(nindex) };

	for(auto p : state->world.state_definition_get_abstract_state_membership(state->world.state_instance_get_definition(s))) {
		if(p.get_province().get_state_membership() == s) {
			for(auto a : state->world.province_get_army_location(p.get_province())) {
				if(a.get_army().get_controller_from_army_control() == n)
					return true;
			}
		}
	}
	return false;
}
inline int32_t* f_units_in_state_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_units_in_state(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_units_in_province(sys::state* state, int32_t sindex, int32_t nindex) {
	dcon::province_id s{ dcon::province_id::value_base_t(sindex) };
	dcon::nation_id n{ dcon::nation_id::value_base_t(nindex) };

	for(auto a : state->world.province_get_army_location(s)) {
		if(a.get_army().get_controller_from_army_control() == n)
			return true;
	}
	return false;
}
inline int32_t* f_units_in_province_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_units_in_province(state, nindex, tindex), nullptr);

	return p + 2;
}
inline int32_t f_count_p_units(sys::state* state, int32_t sindex) {
	dcon::province_id s{ dcon::province_id::value_base_t(sindex) };

	int32_t total = 0;
	for(auto a : state->world.province_get_army_location(s)) {
		auto mrange = a.get_army().get_army_membership();
		total += int32_t(mrange.end() - mrange.begin());
	}
	return total;
}
inline int32_t* f_count_p_units_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_i32, f_count_p_units(state, nindex), nullptr);

	return p + 2;
}
inline bool f_are_at_war(sys::state* state, int32_t sindex, int32_t nindex) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(sindex) };
	dcon::nation_id n{ dcon::nation_id::value_base_t(nindex) };
	return military::are_at_war(*state, s, n);
}
inline int32_t* f_are_at_war_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_units_in_province(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_general_in_p(sys::state* state, int32_t sindex) {
	dcon::province_id s{ dcon::province_id::value_base_t(sindex) };
	for(const auto ar : state->world.province_get_army_location(s)) {
		if(state->world.army_leadership_get_general(state->world.army_get_army_leadership(state->world.army_location_get_army(ar)))) {
			return true;
		}
	}
	return false;
}
inline int32_t* f_general_in_p_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_general_in_p(state, nindex), nullptr);

	return p + 2;
}
inline int32_t f_count_ships(sys::state* state, int32_t sindex) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(sindex) };

	int32_t total = 0;
	for(auto a : state->world.nation_get_navy_control(s)) {
		auto mrange = a.get_navy().get_navy_membership();
		total += int32_t(mrange.end() - mrange.begin());
	}
	return total;
}
inline int32_t* f_count_ships_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_i32, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_i32, f_count_ships(state, nindex), nullptr);

	return p + 2;
}
inline bool f_coastal_s(sys::state* state, int32_t sindex) {
	dcon::state_instance_id s{ dcon::state_instance_id::value_base_t(sindex) };
	return province::state_is_coastal(*state, s);
}
inline int32_t* f_coastal_s_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_coastal_s(state, nindex), nullptr);

	return p + 2;
}
inline bool f_state_produces(sys::state* state, int32_t cindex, int32_t sindex) {
	dcon::commodity_id c{ dcon::commodity_id::value_base_t(cindex) };
	dcon::state_instance_id si{ dcon::state_instance_id::value_base_t(sindex) };

	auto o = state->world.state_instance_get_nation_from_state_ownership(si);
	auto d = state->world.state_instance_get_definition(si);
	for(auto p : state->world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			if(p.get_province().get_rgo() == c)
				return true;
			for(auto f : p.get_province().get_factory_location()) {
				if(f.get_factory().get_building_type().get_output() == c)
					return true;
			}
		}
	}

	return false;
}
inline int32_t* f_state_produces_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_state_produces(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_culture_core(sys::state* state, int32_t cindex, int32_t sindex) {
	dcon::culture_id c{ dcon::culture_id::value_base_t(cindex) };
	dcon::province_id p{ dcon::province_id::value_base_t(sindex) };

	for(auto co : state->world.province_get_core(p)) {
		if(co.get_identity().get_primary_culture() == c)
			return true;
	}
	return false;
}
inline int32_t* f_has_culture_core_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_culture_core(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_canal_enabled(sys::state* state, int32_t sindex) {
	return (state->world.province_adjacency_get_type(state->province_definitions.canals[sindex]) & province::border::impassible_bit) == 0;
}
inline int32_t* f_canal_enabled_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_canal_enabled(state, nindex), nullptr);

	return p + 2;
}
inline bool f_truce_with(sys::state* state, int32_t tindex, int32_t sindex) {
	dcon::nation_id target{ dcon::nation_id::value_base_t(tindex) };
	dcon::nation_id source{ dcon::nation_id::value_base_t(sindex) };

	return military::has_truce_with(*state, source, target);
}
inline int32_t* f_truce_with_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_truce_with(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_has_named_leader(sys::state* state, int32_t name, int32_t nindex) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(nindex) };
	dcon::unit_name_id t{ dcon::unit_name_id::value_base_t(name) };

	for(auto l : state->world.nation_get_leader_loyalty(n)) {
		auto lname = l.get_leader().get_name();
		if(state->to_string_view(lname) == state->to_string_view(t))
			return true;
	}
	return false;
}
inline int32_t* f_has_named_leader_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_named_leader(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_can_release(sys::state* state, int32_t tindex, int32_t nindex) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(nindex) };
	dcon::national_identity_id t{ dcon::national_identity_id::value_base_t(tindex) };
	return nations::can_release_as_vassal(*state, n, t);
}
inline int32_t* f_can_release_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_can_release(state, nindex, tindex), nullptr);

	return p + 2;
}
inline bool f_are_allied(sys::state* state, int32_t tindex, int32_t sindex) {
	dcon::nation_id target{ dcon::nation_id::value_base_t(tindex) };
	dcon::nation_id source{ dcon::nation_id::value_base_t(sindex) };

	return nations::are_allied(*state, source, target);
}
inline int32_t* f_are_allied_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_are_allied(state, nindex, tindex), nullptr);
	return p + 2;
}
inline bool f_nation_has_any_factory(sys::state* state, int32_t s_index) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(s_index) };
	for(auto p : state->world.nation_get_province_ownership(s)) {
		auto rng = p.get_province().get_factory_location();
		if(rng.begin() != rng.end())
			return true;
	}
	return false;
}
inline int32_t* f_nation_has_any_factory_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_nation_has_any_factory(state, nindex), nullptr);
	return p + 2;
}
inline bool f_is_bankrupt_debtor_to(sys::state* state, int32_t tindex, int32_t sindex) {
	dcon::nation_id target{ dcon::nation_id::value_base_t(tindex) };
	dcon::nation_id source{ dcon::nation_id::value_base_t(sindex) };

	return economy::is_bankrupt_debtor_to(*state, source, target);
}
inline int32_t* f_is_bankrupt_debtor_to_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_is_bankrupt_debtor_to(state, nindex, tindex), nullptr);
	return p + 2;
}
inline bool f_global_flag_set(sys::state* state, int32_t s_index) {
	dcon::global_flag_id s{ dcon::global_flag_id::value_base_t(s_index) };
	return state->national_definitions.is_global_flag_variable_set(s);
}
inline int32_t* f_global_flag_set_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_global_flag_set(state, nindex), nullptr);
	return p + 2;
}
inline bool f_can_nationalize(sys::state* state, int32_t s_index) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(s_index) };
	for(auto c : state->world.nation_get_unilateral_relationship_as_target(s)) {
		if(c.get_foreign_investment() > 0.0f)
			return true;
	}
	return false;
}
inline int32_t* f_can_nationalize_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_can_nationalize(state, nindex), nullptr);
	return p + 2;
}
inline float f_movement_str(sys::state* state, bool is_social, int32_t s_index) {
	dcon::nation_id s{ dcon::nation_id::value_base_t(s_index) };
	auto itype = is_social ? culture::issue_type::social : culture::issue_type::political;

	float max_str = 0.0f;
	for(auto m : state->world.nation_get_movement_within(s)) {
		auto issue = m.get_movement().get_associated_issue_option();
		if(issue) {
			if(culture::issue_type(issue.get_parent_issue().get_issue_type()) == itype && m.get_movement().get_pop_support() > max_str) {
				max_str = m.get_movement().get_pop_support();
			}
		}
	}
	auto ncpop = state->world.nation_get_non_colonial_population(s);
	return ncpop > 0.0f ? max_str * state->defines.movement_support_uh_factor / ncpop : 0.0f;
}
inline int32_t* f_movement_str_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	bool social = s.main_data_back(0) != 0; s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	auto fres = f_movement_str(state, social, nindex);
	int64_t data = 0;
	memcpy(&data, &fres, 4);
	s.push_back_main(fif::fif_f32, data, nullptr);
	return p + 2;
}
inline float f_influence_on(sys::state* state, int32_t t_index, int32_t n_index) {
	dcon::nation_id gp{ dcon::nation_id::value_base_t(n_index) };
	dcon::nation_id t{ dcon::nation_id::value_base_t(t_index) };

	auto gpr = state->world.get_gp_relationship_by_gp_influence_pair(t, gp);
	return state->world.gp_relationship_get_influence(gpr);
}
inline int32_t* f_influence_on_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t sindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	auto fres = f_influence_on(state, sindex, nindex);
	int64_t data = 0;
	memcpy(&data, &fres, 4);
	s.push_back_main(fif::fif_f32, data, nullptr);
	return p + 2;
}
inline float f_relations(sys::state* state, int32_t t_index, int32_t n_index) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(n_index) };
	dcon::nation_id t{ dcon::nation_id::value_base_t(t_index) };

	auto rel = state->world.get_diplomatic_relation_by_diplomatic_pair(t, n);
	return state->world.diplomatic_relation_get_value(rel);
}
inline int32_t* f_relations_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_f32, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t sindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	auto fres = f_relations(state, sindex, nindex);
	int64_t data = 0;
	memcpy(&data, &fres, 4);
	s.push_back_main(fif::fif_f32, data, nullptr);
	return p + 2;
}
inline bool f_has_union_sphere(sys::state* state, int32_t s_index) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(s_index) };
	auto prim_culture = state->world.nation_get_primary_culture(n);
	auto culture_group = state->world.culture_get_group_from_culture_group_membership(prim_culture);

	for(auto s : state->world.nation_get_gp_relationship_as_great_power(n)) {
		if((s.get_status() & nations::influence::level_mask) == nations::influence::level_in_sphere &&
				s.get_influence_target().get_primary_culture().get_group_from_culture_group_membership() == culture_group) {
			return true;
		}
	}
	return false;
}
inline int32_t* f_has_union_sphere_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_has_union_sphere(state, nindex), nullptr);
	return p + 2;
}
inline bool f_state_rgo(sys::state* state, int32_t cindex, int32_t sindex) {
	dcon::commodity_id c{ dcon::commodity_id::value_base_t(cindex) };
	dcon::state_instance_id si{ dcon::state_instance_id::value_base_t(sindex) };

	auto o = state->world.state_instance_get_nation_from_state_ownership(si);
	auto d = state->world.state_instance_get_definition(si);
	for(auto p : state->world.state_definition_get_abstract_state_membership(d)) {
		if(p.get_province().get_nation_from_province_ownership() == o) {
			if(p.get_province().get_rgo() == c)
				return true;
		}
	}

	return false;
}
inline int32_t* f_state_rgo_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_state_rgo(state, nindex, tindex), nullptr);
	return p + 2;
}
inline bool f_in_crisis(sys::state* state, int32_t nindex) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(nindex) };

	auto sz = state->crisis_participants.size();
	for(uint32_t i = 0; i < sz; ++i) {
		if(!state->crisis_participants[i].id)
			return false;
		if(state->crisis_participants[i].id == n)
			return true;
	}
	return false;
}
inline int32_t* f_in_crisis_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_in_crisis(state, nindex), nullptr);
	return p + 2;
}
inline bool f_can_build_naval_base(sys::state* state, int32_t cindex, int32_t sindex) {
	dcon::province_id p{ dcon::province_id::value_base_t(cindex) };
	dcon::nation_id n{ dcon::nation_id::value_base_t(sindex) };

	return state->world.province_get_is_coast(p)
		&& (state->world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base)) < state->world.nation_get_max_building_level(n, uint8_t(economy::province_building_type::naval_base)))
		&& (state->world.province_get_building_level(p, uint8_t(economy::province_building_type::naval_base)) != 0 || !military::state_has_naval_base(*state, state->world.province_get_state_membership(p)));
}
inline int32_t* f_can_build_naval_base_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t tindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_can_build_naval_base(state, nindex, tindex), nullptr);
	return p + 2;
}
inline void* f_ranked_nations(sys::state* state) {
	return state->nations_by_rank.data();
}
inline int32_t* f_ranked_nations_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main();
		s.push_back_main(fif::fif_opaque_ptr, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int64_t res = (int64_t)(f_ranked_nations(state));

	s.push_back_main(fif::fif_opaque_ptr, res, nullptr);
	return p + 2;
}

inline std::string state_to_owner() {
	return "state_ownership-state nation @ ";
}
inline std::string province_to_owner() {
	return "province_ownership-province nation @ ";
}
inline std::string province_to_controller() {
	return "province_control-province nation @ ";
}
inline std::string pop_to_location() {
	return "pop_location-pop province @ ";
}
inline std::string pop_to_owner() {
	return pop_to_location() + province_to_owner();
}
inline std::string nation_to_tag() {
	return "identity_holder-nation @ identity ";
}
inline std::string tag_to_nation() {
	return "identity_holder-identity nation @ ";
}

inline bool f_party_name(sys::state* state, int32_t s_index, int32_t ideoindex, int32_t nameindex) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(s_index) };
	dcon::ideology_id i{ dcon::ideology_id::value_base_t(ideoindex) };
	dcon::text_key m{ dcon::text_key::value_base_t(nameindex) };

	auto holder = state->world.nation_get_identity_from_identity_holder(n);

	auto start = state->world.national_identity_get_political_party_first(holder).id.index();
	auto end = start + state->world.national_identity_get_political_party_count(holder);

	for(int32_t j = start; j < end; j++) {
		auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(j));
		if(politics::political_party_is_active(*state, n, pid) && state->world.political_party_get_ideology(pid) == i) {
			return state->world.political_party_get_name(pid) == m;
		}
	}

	return false;
}
inline int32_t* f_party_name_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t ideoindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t nameindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_party_name(state, nindex, ideoindex, nameindex), nullptr);
	return p + 2;
}

inline bool f_party_pos(sys::state* state, int32_t s_index, int32_t ideoindex, int32_t optid) {
	dcon::nation_id n{ dcon::nation_id::value_base_t(s_index) };
	dcon::ideology_id i{ dcon::ideology_id::value_base_t(ideoindex) };
	dcon::issue_option_id m{ dcon::issue_option_id::value_base_t(optid) };

	auto popt = state->world.issue_option_get_parent_issue(m);
	auto holder = state->world.nation_get_identity_from_identity_holder(n);

	auto start = state->world.national_identity_get_political_party_first(holder).id.index();
	auto end = start + state->world.national_identity_get_political_party_count(holder);

	for(int32_t j = start; j < end; j++) {
		auto pid = dcon::political_party_id(dcon::political_party_id::value_base_t(j));
		if(politics::political_party_is_active(*state, n, pid) && state->world.political_party_get_ideology(pid) == i) {
			return state->world.political_party_get_party_issues(pid, popt) == m;
		}
	}
	return false;
}
inline int32_t* f_party_pos_b(fif::state_stack& s, int32_t* p, fif::environment* e) {
	if(fif::typechecking_mode(e->mode)) {
		if(fif::typechecking_failed(e->mode))
			return p + 2;
		s.pop_main(); s.pop_main(); s.pop_main();
		s.push_back_main(fif::fif_bool, 0, nullptr);
		return p + 2;
	}

	sys::state* state = (sys::state*)(s.main_data_back(0)); s.pop_main();
	int32_t nindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t ideoindex = int32_t(s.main_data_back(0)); s.pop_main();
	int32_t nameindex = int32_t(s.main_data_back(0)); s.pop_main();

	s.push_back_main(fif::fif_bool, f_party_pos(state, nindex, ideoindex, nameindex), nullptr);
	return p + 2;
}

inline void common_fif_environment(sys::state& state, fif::environment& env) {
	fif::initialize_standard_vocab(env);

	fif::interpreter_stack values{ };

	fif::run_fif_interpreter(env, fif::container_interface(), values);

	values.push_back_main(fif::fif_opaque_ptr, (int64_t)(&(state.world)), nullptr);
	fif::run_fif_interpreter(env, "set-container ", values);
	values.push_back_main(fif::fif_opaque_ptr, (int64_t)(dcon::shared_backing_storage.allocation), nullptr);
	fif::run_fif_interpreter(env, "set-vector-storage ", values);

	fif::run_fif_interpreter(env,
		" ptr(nil) global state-ptr "
		" : set-state state-ptr ! ; "
		" :export set_state ptr(nil) set-state ;  ",
		values);

	values.push_back_main(fif::fif_opaque_ptr, (int64_t)(&state), nullptr);
	fif::run_fif_interpreter(env, "set-state", values);

	//fif::run_fif_interpreter(env, "ptr(nil) global state-ptr state-ptr ! ", values);

	auto nation_id_type = env.dict.types.find("nation_id")->second;
	auto prov_id_type = env.dict.types.find("province_id")->second;

	fif::add_import("current-year", (void *) f_date_to_year, f_date_to_year_b, { fif::fif_i64 }, { fif::fif_i32 }, env);
	fif::add_import("current-month", (void *) f_date_to_month, f_date_to_month_b, { fif::fif_i64 }, { fif::fif_i32 }, env);
	fif::add_import("empty-a-province", (void *) f_empty_adjacent_province, f_empty_adjacent_province_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("empty-a-state", (void *) f_empty_adjacent_state, f_empty_adjacent_state_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("unowned-core", (void *) f_unowned_core, f_unowned_core_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("core-in-nation", (void *) f_core_in_nation, f_core_in_nation_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("core-in-province", (void *) f_core_in_province, f_core_in_province_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("core-in-state", (void *) f_core_in_state, f_core_in_state_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("core-fully-state", (void *) f_core_fully_state, f_core_fully_state_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-cb-against", (void *) f_has_cb_against, f_has_cb_against_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-access-with", (void *) f_has_access_with, f_has_access_with_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-any-factory", (void *) f_has_any_factory, f_has_any_factory_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-factory", (void *) f_has_factory, f_has_factory_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-nmod", (void *) f_has_nmod, f_has_nmod_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-pmod", (void *) f_has_pmod, f_has_pmod_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("in-region", (void *) f_in_region, f_in_region_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("owns-region", (void *) f_owns_region, f_owns_region_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("owns-state", (void *) f_owns_state, f_owns_state_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("nation-a", (void *) f_nation_a, f_nation_a_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("units-in-state", (void *) f_units_in_state, f_units_in_state_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("units-in-province", (void *) f_units_in_province, f_units_in_province_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("count-p-units", (void *) f_count_p_units, f_count_p_units_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_i32 }, env);
	fif::add_import("at-war?", (void *) f_are_at_war, f_are_at_war_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("general-in-province?", (void *) f_general_in_p, f_general_in_p_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("count-ships", (void *) f_count_ships, f_count_ships_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_i32 }, env);
	fif::add_import("coastal-s?", (void *) f_coastal_s, f_coastal_s_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("state-produces?", (void *) f_state_produces, f_state_produces_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-culture-core?", (void *) f_has_culture_core, f_has_culture_core_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("canal-enabled?", (void *) f_canal_enabled, f_canal_enabled_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("truce-with?", (void *) f_truce_with, f_truce_with_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-named-leader?", (void *) f_has_named_leader, f_has_named_leader_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("can-release?", (void *) f_can_release, f_can_release_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("are-allied?", (void *) f_are_allied, f_are_allied_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("nation-has-any-factory", (void *) f_nation_has_any_factory, f_nation_has_any_factory_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("bankrupt-debtor-to?", (void *) f_is_bankrupt_debtor_to, f_is_bankrupt_debtor_to_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("global-flag-set?", (void *) f_global_flag_set, f_global_flag_set_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("can-nationalize?", (void *) f_can_nationalize, f_can_nationalize_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("movement-str", (void *) f_movement_str, f_movement_str_b, { fif::fif_i32, fif::fif_bool, fif::fif_opaque_ptr }, { fif::fif_f32 }, env);
	fif::add_import("has-union-sphere?", (void *) f_has_union_sphere, f_has_union_sphere_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("state-has-rgo?", (void *) f_state_rgo, f_state_rgo_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("in-crisis?", (void *) f_in_crisis, f_in_crisis_b, { fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("influence-on", (void *) f_influence_on, f_influence_on_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_f32 }, env);
	fif::add_import("relations", (void *) f_relations, f_relations_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_f32 }, env);
	fif::add_import("can-build-naval-base?", (void *) f_can_build_naval_base, f_can_build_naval_base_b, { fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("ranked-nation-array", (void *) f_ranked_nations, f_ranked_nations_b, { fif::fif_opaque_ptr }, { fif::fif_opaque_ptr }, env);
	fif::add_import("has-named-party?", (void*)f_party_name, f_party_name_b, { fif::fif_i32, fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);
	fif::add_import("has-positioned-party?", (void*)f_party_pos, f_party_pos_b, { fif::fif_i32, fif::fif_i32, fif::fif_i32, fif::fif_opaque_ptr }, { fif::fif_bool }, env);

	fif::run_fif_interpreter(env,
		" : first-sea-province " + std::to_string(offsetof(sys::state, province_definitions) + offsetof(province::global_provincial_state, first_sea_province)) + " state-ptr @ buf-add ptr-cast ptr(province_id) ; "
		" : start-date " + std::to_string(offsetof(sys::state, start_date)) + " state-ptr @ buf-add ptr-cast ptr(i64) ; "
		" : current-date " + std::to_string(offsetof(sys::state, current_date)) + " state-ptr @ buf-add ptr-cast ptr(u16) ; "
		" : crisis-state " + std::to_string(offsetof(sys::state, crisis_state_instance)) + " state-ptr @ buf-add ptr-cast ptr(state_instance_id) ; "
		" : sea-province? >index first-sea-province @ >index >= ; "
		" :s >owner pop_id s: " + pop_to_owner() + " ; "
		" :s >owner province_id s: " + province_to_owner() + " ; "
		" :s >owner state_instance_id s: " + state_to_owner() + " ; "
		" :s >location pop_id s: " + pop_to_location() + " ; "

		":struct gp-iterator ptr(nil) base i32 count i32 size i32 gp-count ; "
		":s next gp-iterator s: let it it .count " + std::to_string(sizeof(dcon::nation_id)) + " * it .base buf-add ptr-cast ptr(nation_id) @ it .count@ 1 + swap .count! swap ; "
		":s more? gp-iterator s: let it it .count it .size < it .gp-count " + std::to_string(state.defines.great_nations_count) + " < and it swap ; "
		":s inc-gp gp-iterator s: .gp-count@ 1 + swap .gp-count! ; "
		": make-gp-it make gp-iterator state-ptr @ ranked-nation-array swap .base! nation-size swap .size! ; "

		":struct state-prov-iterator vpool-abstract_state_membership-state base i32 count state_instance_id parent ; "
		":s next state-prov-iterator s: let it it .base it .count index @ province it .count@ 1 - swap .count! swap dup state_membership @ it .parent = ; "
		":s more? state-prov-iterator s: .count@ 0 >= ; "
		": make-sp-it  dup definition @ abstract_state_membership-state make state-prov-iterator .base! .parent! .base@ size 1 - swap .count! ; "

		":struct nation-prov-iterator vpool-province_ownership-nation base i32 count ; "
		":s next nation-prov-iterator s: let it it .base it .count index @ province it .count@ 1 - swap .count! swap ; "
		":s more? nation-prov-iterator s: .count@ 0 >= ; "
		": make-n-it  province_ownership-nation make nation-prov-iterator .base! .base@ size 1 - swap .count! ; "

		":struct pcore-iterator vpool-core-province base i32 count ; "
		":s next pcore-iterator s: let it it .base it .count index @ identity @ " + tag_to_nation() + " it .count@ 1 - swap .count! swap ; "
		":s more? pcore-iterator s: .count@ 0 >= ; "
		": make-pc-it  core-province make pcore-iterator .base! .base@ size 1 - swap .count! ; "

		":struct ncore-iterator vpool-core-identity base i32 count ; "
		":s next ncore-iterator s: let it it .base it .count index @ province @ it .count@ 1 - swap .count! swap ; "
		":s more? ncore-iterator s: .count@ 0 >= ; "
		": make-nc-it  core-identity make ncore-iterator .base! .base@ size 1 - swap .count! ; "

		":struct nation-state-iterator vpool-state_ownership-nation base i32 count ; "
		":s next nation-state-iterator s: let it it .base it .count index @ state it .count@ 1 - swap .count! swap ; "
		":s more? nation-state-iterator s: .count@ 0 >= ; "
		": make-ns-it  state_ownership-nation make nation-state-iterator .base! .base@ size 1 - swap .count! ; "

		":struct nation-vassal-iterator vpool-overlord-ruler base i32 count ; "
		":s next nation-vassal-iterator s: let it it .base it .count index @ subject it .count@ 1 - swap .count! swap ; "
		":s more? nation-vassal-iterator s: .count@ 0 >= ; "
		": make-nv-it  overlord-ruler make nation-vassal-iterator .base! .base@ size 1 - swap .count! ; "

		":struct nation-gp-iterator vpool-gp_relationship-great_power base i32 count ; "
		":s next nation-gp-iterator s: let it it .base it .count index @ dup influence_target @ swap status @ >i32 " + std::to_string(nations::influence::level_mask) + " and " + std::to_string(nations::influence::level_in_sphere) + " = >r it .count@ 1 - swap .count! swap r> ; "
		":s more? nation-gp-iterator s: .count@ 0 >= ; "
		": make-ngp-it  gp_relationship-great_power make nation-gp-iterator .base! .base@ size 1 - swap .count! ; "

		":struct prov-pop-iterator vpool-pop_location-province base i32 count ; "
		":s next prov-pop-iterator s: let it it .base it .count index @ pop it .count@ 1 - swap .count! swap ; "
		":s more? prov-pop-iterator s: .count@ 0 >= ; "
		": make-pp-it  pop_location-province make prov-pop-iterator .base! .base@ size 1 - swap .count! ; "

		":struct astate-prov-iterator vpool-abstract_state_membership-state base i32 count ; "
		":s next astate-prov-iterator s: let it it .base it .count index @ province it .count@ 1 - swap .count! swap ; "
		":s more? astate-prov-iterator s: .count@ 0 >= ; "
		": make-asp-it abstract_state_membership-state make astate-prov-iterator .base! .base@ size 1 - swap .count! ; "

		":struct aregion-prov-iterator vpool-region_membership-region base i32 count ; "
		":s next aregion-prov-iterator s: let it it .base it .count index @ province @ it .count@ 1 - swap .count! swap ; "
		":s more? aregion-prov-iterator s: .count@ 0 >= ; "
		": make-arp-it region_membership-region make aregion-prov-iterator .base! .base@ size 1 - swap .count! ; "

		":struct adj-prov-iterator vpool-province_adjacency-connected_provinces base i32 count province_id parent ; "
		":s next adj-prov-iterator s: let it it .base it .count index @ dup >r "
			"connected_provinces .wrapped dup ptr-cast ptr(nil) " + std::to_string(sizeof(dcon::province_id)) + " swap buf-add ptr-cast ptr(province_id) @ swap @ dup it .parent = not select "
			"it .count@ 1 - swap .count! swap "
			"r> type @ " + std::to_string(int32_t(province::border::impassible_bit)) + " >i8 and 0 >i8 = ; "
		":s more? adj-prov-iterator s: .count@ 0 >= ; "
		": make-ap-it  dup province_adjacency-connected_provinces make adj-prov-iterator .base! .parent! .base@ size 1 - swap .count! ; "

		":struct adj-nation-iterator vpool-nation_adjacency-connected_nations base i32 count nation_id parent ; "
		":s next adj-nation-iterator s: let it it .base it .count index @ "
			"connected_nations .wrapped dup ptr-cast ptr(nil) " + std::to_string(sizeof(dcon::nation_id)) + " swap buf-add ptr-cast ptr(nation_id) @ swap @ dup it .parent = not select "
			"it .count@ 1 - swap .count! swap ; "
		":s more? adj-nation-iterator s: .count@ 0 >= ; "
		": make-an-it  dup nation_adjacency-connected_nations make adj-nation-iterator .base! .parent! .base@ size 1 - swap .count! ; "
		,
		values);

	assert(env.mode != fif::fif_mode::error);
}


}

namespace fif_trigger {
std::string multiplicative_modifier(sys::state& state, dcon::value_modifier_key modifier);
std::string additive_modifier(sys::state& state, dcon::value_modifier_key modifier);
std::string evaluate(sys::state& state, dcon::trigger_key key);
}

