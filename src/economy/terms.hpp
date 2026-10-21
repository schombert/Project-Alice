#pragma once

#include "constants_ui.hpp"
#include "text.hpp"
#include "dcon_generated_ids.hpp"
#include "system_state.hpp"
#include "adaptive_ve.hpp"


template<typename T, typename S>
concept addition_is_defined = requires (T x, S y) {
	x + y;
};
template<typename T, typename S>
concept subtraction_is_defined = requires (T x, S y) {
	x - y;
};
template<typename T, typename S>
concept multiplication_is_defined = requires (T x, S y) {
	x * y;
};
template<typename T, typename S>
concept division_is_defined = requires (T x, S y) {
	x / y;
};
template<typename T, typename S>
concept beq_is_defined = requires (T x, S y) {
	x == y;
};
template<typename T, typename S>
concept bleq_is_defined = requires (T x, S y) {
	x <= y;
};

template<typename T>
concept is_term = requires (T x) {
	x.is_term;
};
template<typename T>
concept is_atom = requires (T x) {
	x.is_atom;
};

namespace sys {
	struct state;
}

namespace text {
	struct layout_box;
	struct layout_base;
}

namespace term {

struct market_id_wrapper {
	dcon::market_id data;
	std::string description;
	static constexpr bool is_term = true;
	static constexpr bool is_atom = true;
};
struct trade_route_id_wrapper {
	dcon::trade_route_id data;
	std::string description;
	static constexpr bool is_term = true;
	static constexpr bool is_atom = true;
};
struct commodity_id_wrapper {
	dcon::commodity_id data;
	std::string description;
	static constexpr bool is_term = true;
	static constexpr bool is_atom = true;
};
struct bool_t {
	bool data;
	std::string description = "term_const";
	static constexpr bool is_term = true;
	static constexpr bool is_atom = true;
};

template <typename left_t, typename right_t>
struct addition {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct multiplication {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct division {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct subtraction {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};

template <typename left_t, typename right_t>
struct addition_result {
	left_t left;
	right_t right;
	float data;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct multiplication_result {
	left_t left;
	right_t right;
	float data;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct division_result {
	left_t left;
	right_t right;
	float data;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct subtraction_result {
	left_t left;
	right_t right;
	float data;
	static constexpr bool is_term = true;
};


template <typename replacement>
inline commodity_id_wrapper compute(sys::state const& state, replacement replace_with, commodity_id_wrapper node) {
	if constexpr(std::is_same_v<commodity_id_wrapper, replacement>) {
		if(node.description.compare(replace_with.description) == 0) {
			return replace_with;
		}
	}
	return node;
}

template <typename replacement>
inline trade_route_id_wrapper compute(sys::state const& state, replacement replace_with, trade_route_id_wrapper node) {
	if constexpr(std::is_same_v<trade_route_id_wrapper, replacement>) {
		if(node.description.compare(replace_with.description) == 0) {
			return replace_with;
		}
	}
	return node;
}

template <typename replacement>
inline market_id_wrapper compute(sys::state const& state, replacement replace_with, market_id_wrapper node) {
	if constexpr(std::is_same_v<market_id_wrapper, replacement>) {
		if(node.description.compare(replace_with.description) == 0) {
			return replace_with;
		}
	}
	return node;
}

template <typename replacement>
inline bool_t compute(sys::state const& state, replacement replace_with, bool_t& node) {
	if constexpr(std::is_same_v<bool_t, replacement>) {
		if(node.description.compare(replace_with.description) == 0) {
			return replace_with;
		}
	}
	return node;
}

template <typename left_t, typename right_t>
struct max {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct max_result {
	left_t left;
	right_t right;
	float data;
	static constexpr bool is_term = true;
};

template <typename left_t, typename right_t>
struct BEq {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct BEq_result {
	left_t left;
	right_t right;
	bool data;
	static constexpr bool is_term = true;
};

template <typename left_t, typename right_t>
struct BLEq {
	left_t left;
	right_t right;
};
template <typename left_t, typename right_t>
struct BLEq_result {
	left_t left;
	right_t right;
	bool data;
	static constexpr bool is_term = true;
};

template <typename condition_t, typename then_t, typename else_t, typename out_type>
struct ite {
	condition_t condition;
	then_t left;
	else_t right;
	static constexpr bool is_term = true;
};
template <typename condition_t, typename then_t, typename else_t, typename out_type>
struct ite_result {
	condition_t condition;
	then_t left;
	else_t right;
	out_type data;
	static constexpr bool is_term = true;
};


template <typename left_t, typename right_t>
struct min {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct min_result {
	left_t left;
	right_t right;
	float data;
	static constexpr bool is_term = true;
};

//template <auto description>
struct value_t {
	float data;
	std::string description;
	text::text_color color;
	bool is_constant = false;
	static constexpr bool is_term = true;
	static constexpr bool is_atom = true;

	constexpr value_t(float in_data) {
		data = in_data;
		description = "term_const";
		color = text::text_color::light_grey;
		is_constant = true;
	}
	constexpr value_t(int in_data) {
		data = float(in_data);
		description = "term_const";
		color = text::text_color::light_grey;
		is_constant = true;
	}
	constexpr value_t(float in_data, std::string desc, text::text_color in_color) {
		data = in_data;
		description = desc;
		color = in_color;
	}
	constexpr value_t(int in_data, std::string desc, text::text_color in_color) {
		data = float(in_data);
		description = desc;
		color = in_color;
	}

	bool operator==(value_t rhs) = delete;
	bool operator<=(value_t rhs) = delete;
	bool operator>=(value_t rhs) = delete;
	bool operator<(value_t rhs) = delete;
	bool operator>(value_t rhs) = delete;
};
struct value_t_result {
	float data;
	std::string description;
	text::text_color color;
	bool is_constant;
	static constexpr bool is_term = true;

	value_t_result(value_t in_value) {
		data = in_value.data;
		description = in_value.description;
		color = in_value.color;
		is_constant = in_value.is_constant;
	}
};

inline addition<value_t, value_t> operator+(const value_t& Left, const value_t& Right) {
	return { Left, Right };
}
inline multiplication <value_t, value_t> operator*(const value_t& Left, const value_t& Right) {
	return { Left, Right };
}
inline division <value_t, value_t> operator/(const value_t& Left, const value_t& Right) {
	return { Left, Right };
}
inline subtraction <value_t, value_t> operator-(const value_t& Left, const value_t& Right) {
	return { Left, Right };
}
template <typename replacement>
inline value_t_result compute(sys::state const& state, replacement replace_with, value_t node) {
	if constexpr(std::is_same_v<value_t_result, replacement>) {
		if(node.description.compare(replace_with.description) == 0) {
			return replace_with;
		}
	}	
	return value_t_result(node);
}

template <typename axis_t>
inline float compute_derivative(sys::state const& state, axis_t axis, value_t_result node) {
	if constexpr(std::is_same_v<value_t_result, axis_t>) {
		if(node.description.compare(axis.description) == 0) {
			return 1.f;
		}
	}
	return 0.f;
}

template <typename left_t, typename right_t>
struct fetch_trade_volume {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct fetch_trade_volume_result {
	left_t left;
	right_t right;
	float data;
};

template <typename left_t, typename right_t>
struct fetch_price {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct fetch_price_result {
	left_t left;
	right_t right;
	float data;
};

template <typename left_t, typename right_t>
struct fetch_expected_probability_to_sell {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct fetch_expected_probability_to_sell_result {
	left_t left;
	right_t right;
	float data;
};

template <typename left_t, typename right_t>
struct fetch_expected_probability_to_buy {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct fetch_expected_probability_to_buy_result {
	left_t left;
	right_t right;
	float data;
};

template <typename left_t, typename right_t>
struct fetch_price_confidence {
	left_t left;
	right_t right;
	static constexpr bool is_term = true;
};
template <typename left_t, typename right_t>
struct fetch_price_confidence_result {
	left_t left;
	right_t right;
	float data;
};

template <typename left_t>
struct trade_house_budget_import_scale {
	left_t left;
	static constexpr bool is_term = true;
};
template <typename left_t>
struct trade_house_budget_import_scale_result {
	left_t left;
	float data;
};

template <typename left_t>
struct naval_transportation_demand_satisfaction {
	left_t left;
	static constexpr bool is_term = true;
};
template <typename left_t>
struct naval_transportation_demand_satisfaction_result {
	left_t left;
	float data;
};

template <typename left_t>
struct land_transportation_demand_satisfaction {
	left_t left;
	static constexpr bool is_term = true;
};
template <typename left_t>
struct land_transportation_demand_satisfaction_result {
	left_t left;
	float data;
};

template <typename left_t>
struct fetch_trade_origin {
	left_t left;
	static constexpr bool is_term = true;
};
template <typename left_t>
struct fetch_trade_origin_result {
	left_t left;
	dcon::market_id data;
};

template <typename left_t>
struct fetch_trade_target {
	left_t left;
	static constexpr bool is_term = true;
};
template <typename left_t>
struct fetch_trade_target_result {
	left_t left;
	dcon::market_id data;
};

template <typename left_t>
struct fetch_trade_owner {
	left_t left;
	static constexpr bool is_term = true;
};
template <typename left_t>
struct fetch_trade_owner_result {
	left_t left;
	dcon::market_id data;
};

template<typename T>
using ensure_wrapper_t = std::conditional_t<
	std::same_as<T, float> || std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, int>,
	value_t,
	T
>;

constexpr value_t ensure_wrapper (float& val) {
	return value_t(val);
}
constexpr value_t ensure_wrapper(int& val) {
	return value_t(val);
}
constexpr bool_t ensure_wrapper(bool& val) {
	return bool_t{ val };
}

template<typename T>
constexpr T ensure_wrapper(T& val) {
	return val;
}

template<typename T>
struct switch_to_result_struct {
	using type = T;
};
template<typename T>
using switch_to_result = std::conditional_t<
	std::is_same_v<T, value_t>,
	value_t_result,
	typename switch_to_result_struct<T>::type
>;

template <typename condition_t, typename then_t, typename else_t, typename out_type>
struct switch_to_result_struct<ite<condition_t, then_t, else_t, out_type>> {
	using type = ite_result<switch_to_result<condition_t>, switch_to_result<then_t>, switch_to_result<else_t>, out_type>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<addition<left_t, right_t>> {
	using type = addition_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<multiplication<left_t, right_t>> {
	using type = multiplication_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<division<left_t, right_t>> {
	using type = division_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<subtraction<left_t, right_t>> {
	using type = subtraction_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<max<left_t, right_t>> {
	using type = max_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<min<left_t, right_t>> {
	using type = min_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<fetch_price<left_t, right_t>> {
	using type = fetch_price_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<fetch_trade_volume<left_t, right_t>> {
	using type = fetch_trade_volume_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<fetch_price_confidence<left_t, right_t>> {
	using type = fetch_price_confidence_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<BEq<left_t, right_t>> {
	using type = BEq_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t, typename right_t>
struct switch_to_result_struct<BLEq<left_t, right_t>> {
	using type = BLEq_result<switch_to_result<left_t>, switch_to_result<right_t>>;
};
template<typename left_t>
struct switch_to_result_struct<fetch_trade_origin<left_t>> {
	using type = fetch_trade_origin_result<switch_to_result<left_t>>;
};
template<typename left_t>
struct switch_to_result_struct<fetch_trade_target<left_t>> {
	using type = fetch_trade_target_result<switch_to_result<left_t>>;
};
template<typename left_t>
struct switch_to_result_struct<fetch_trade_owner<left_t>> {
	using type = fetch_trade_owner_result<switch_to_result<left_t>>;
};
template<typename left_t>
struct switch_to_result_struct<land_transportation_demand_satisfaction<left_t>> {
	using type = land_transportation_demand_satisfaction_result<switch_to_result<left_t>>;
};
template<typename left_t>
struct switch_to_result_struct<naval_transportation_demand_satisfaction<left_t>> {
	using type = naval_transportation_demand_satisfaction_result<switch_to_result<left_t>>;
};
template<typename left_t>
struct switch_to_result_struct<trade_house_budget_import_scale<left_t>> {
	using type = trade_house_budget_import_scale_result<switch_to_result<left_t>>;
};



template <typename left_t, typename right_t> requires (is_term<left_t> || is_term<right_t>)
addition<ensure_wrapper_t<left_t>, ensure_wrapper_t<right_t>> operator+(left_t Left, right_t Right) {
	return { ensure_wrapper(Left), ensure_wrapper(Right) };
}
template <typename left_t, typename right_t>  requires (is_term<left_t> || is_term<right_t>)
multiplication<ensure_wrapper_t<left_t>, ensure_wrapper_t<right_t>> operator*(left_t Left, right_t Right) {
	return { ensure_wrapper(Left), ensure_wrapper(Right) };
}
template <typename left_t, typename right_t>  requires (is_term<left_t> || is_term<right_t>)
division<ensure_wrapper_t<left_t>, ensure_wrapper_t<right_t>> operator/(left_t Left, right_t Right) {
	return { ensure_wrapper(Left), ensure_wrapper(Right) };
}


template <typename right_t> requires (!is_atom<right_t>)
addition<value_t, ensure_wrapper_t<right_t>> operator+(value_t& Left, right_t Right) {
	return { Left, ensure_wrapper(Right) };
}
template <typename right_t> requires (!is_atom<right_t>)
multiplication<value_t, ensure_wrapper_t<right_t>> operator*(value_t& Left, right_t Right) {
	return { Left, ensure_wrapper(Right) };
}
template <typename right_t> requires (!is_atom<right_t>)
division<value_t, ensure_wrapper_t<right_t>> operator/(value_t& Left, right_t Right) {
	return { Left, ensure_wrapper(Right) };
}


template <typename left_t> requires (!is_atom<left_t>)
addition<ensure_wrapper_t<left_t>, value_t> operator+(left_t Left, value_t& Right) {
	return addition<ensure_wrapper_t<left_t>, value_t>{ ensure_wrapper(Left), Right };
}
template <typename left_t> requires (!is_atom<left_t>)
multiplication<ensure_wrapper_t<left_t>, value_t> operator*(left_t Left, value_t& Right) {
	return multiplication<ensure_wrapper_t<left_t>, value_t>{ ensure_wrapper(Left), Right };
}
template <typename left_t> requires (!is_atom<left_t>)
division<ensure_wrapper_t<left_t>, value_t> operator/(left_t Left, value_t& Right) {
	return division<ensure_wrapper_t<left_t>, value_t>{ ensure_wrapper(Left), Right };
}


template <typename left_t, typename right_t> requires (is_term<left_t> || is_term<right_t>)
subtraction<ensure_wrapper_t<left_t>, ensure_wrapper_t<right_t>> operator-(left_t Left, right_t Right) {
	return { ensure_wrapper(Left), ensure_wrapper(Right) };
}
template <typename right_t> requires (!is_atom<right_t>)
subtraction<value_t, ensure_wrapper_t<right_t>> operator-(value_t& Left, right_t Right) {
	return { Left, ensure_wrapper(Right) };
}
template <typename left_t> requires (!is_atom<left_t>)
subtraction<ensure_wrapper_t<left_t>, value_t> operator-(left_t Left, value_t& Right) {
	return subtraction<ensure_wrapper_t<left_t>, value_t>{ ensure_wrapper(Left), Right };
}

}

template <typename left_t, typename right_t> requires (is_term<left_t> || is_term<right_t>)
term::BEq<term::ensure_wrapper_t<left_t>, term::ensure_wrapper_t<right_t>> operator==(left_t Left, right_t Right) {
	return { ensure_wrapper(Left), ensure_wrapper(Right) };
}
template <typename right_t> //requires (!beq_is_defined<right_t, right_t>)
term::BEq<term::value_t, term::ensure_wrapper_t<right_t>> operator==(term::value_t& Left, right_t Right) {
	return { Left, ensure_wrapper(Right) };
}
template <typename left_t> //requires (!beq_is_defined<left_t, left_t>)
term::BEq<term::ensure_wrapper_t<left_t>, term::value_t> operator==(left_t Left, term::value_t& Right) {
	return term::BEq<term::ensure_wrapper_t<left_t>, term::value_t>{ ensure_wrapper(Left), Right };
}
inline term::BEq <term::value_t, term::value_t> operator==(term::value_t& Left, term::value_t& Right) {
	return { Left, Right };
}

template <typename left_t, typename right_t>  requires (is_term<left_t> || is_term<right_t>)
term::BLEq<term::ensure_wrapper_t<left_t>, term::ensure_wrapper_t<right_t>> operator<=(left_t Left, right_t Right) {
	return { term::ensure_wrapper(Left), term::ensure_wrapper(Right) };
}
template <typename right_t> //requires (!bleq_is_defined<right_t, right_t>)
term::BLEq<term::value_t, term::ensure_wrapper_t<right_t>> operator<=(term::value_t& Left, right_t Right) {
	return { Left, term::ensure_wrapper(Right) };
}
template <typename left_t> //requires (!bleq_is_defined<left_t, left_t>)
term::BLEq<term::ensure_wrapper_t<left_t>, term::value_t> operator<=(left_t Left, term::value_t& Right) {
	return term::BEq<term::ensure_wrapper_t<left_t>, term::value_t>{ ensure_wrapper(Left), Right };
}
inline term::BLEq <term::value_t, term::value_t> operator<=(term::value_t& Left, term::value_t& Right) {
	return { Left, Right };
}

namespace adaptive_ve {
template <typename left_t, typename right_t>  requires (is_term<left_t> || is_term<right_t>)
term::max<term::ensure_wrapper_t<left_t>, term::ensure_wrapper_t<right_t>> max(left_t Left, right_t Right) {
	return { term::ensure_wrapper(Left), term::ensure_wrapper(Right) };
}
template <typename right_t>
term::max<term::value_t, term::ensure_wrapper_t<right_t>> max(term::value_t& Left, right_t Right) {
	return { Left, term::ensure_wrapper(Right) };
}
template <typename left_t>
term::max<term::ensure_wrapper_t<left_t>, term::value_t> max(left_t Left, term::value_t& Right) {
	return term::max<term::ensure_wrapper_t<left_t>, term::value_t>{ ensure_wrapper(Left), Right };
}
inline term::max <term::value_t, term::value_t> max(term::value_t& Left, term::value_t& Right) {
	return { Left, Right };
}

template <typename left_t, typename right_t>  requires (is_term<left_t> || is_term<right_t>)
term::min<term::ensure_wrapper_t<left_t>, term::ensure_wrapper_t<right_t>> min(left_t Left, right_t Right) {
	return { term::ensure_wrapper(Left), term::ensure_wrapper(Right) };
}
template <typename right_t>
term::min<term::value_t, term::ensure_wrapper_t<right_t>> min(term::value_t& Left, right_t Right) {
	return { Left, term::ensure_wrapper(Right) };
}
template <typename left_t>
term::min<term::ensure_wrapper_t<left_t>, term::value_t> min(left_t Left, term::value_t& Right) {
	return term::min<term::ensure_wrapper_t<left_t>, term::value_t>{ ensure_wrapper(Left), Right };
}
inline term::min <term::value_t, term::value_t> min(term::value_t& Left, term::value_t& Right) {
	return { Left, Right };
}

template <typename condition_t, typename out_t, typename left_t, typename right_t>  requires ((is_term<left_t> || is_term<right_t>) && (!is_term<out_t>))
term::ite<term::ensure_wrapper_t<condition_t>, term::ensure_wrapper_t<left_t>, term::ensure_wrapper_t<right_t>, out_t> select(condition_t Condition,  left_t Left, right_t Right) {
	auto left = term::ensure_wrapper(Left);
	auto right = term::ensure_wrapper(Right);
	//static_assert(std::is_same<decltype(left.data), decltype(right.data)>::value);
	//static_assert(std::is_same<decltype(left.data), out_t>::value);
	return { term::ensure_wrapper(Condition), left,  right };
}
}

namespace term {

template <typename replacement, typename condition_t, typename then_t, typename else_t, typename out_type>
ite_result<switch_to_result<condition_t>, switch_to_result<then_t>, switch_to_result<else_t>, out_type> compute(sys::state const& state, replacement replace_with,  ite<condition_t, then_t, else_t, out_type>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto condition = compute(state, replace_with, node.condition);
	if(condition.data) {
		return { condition, left, right, left.data };
	} else {
		return { condition, left, right, right.data };
	}
}


template <typename replacement, typename left_t, typename right_t>
BEq_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, BEq<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = left.data == right.data;
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
BLEq_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, BLEq<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = left.data <= right.data;
	return { left, right, result };
}

template <typename replacement, typename left_t, typename right_t>
addition_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, addition<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = left.data + right.data;
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
multiplication_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, multiplication<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = left.data * right.data;
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
division_result<switch_to_result<left_t>, switch_to_result<right_t>>compute(sys::state const& state, replacement replace_with, division<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = left.data / right.data;
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
subtraction_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, subtraction<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = left.data - right.data;
	return { left, right, result };
}
template <typename left_t, typename right_t>
fetch_price_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, fetch_price_result<market_id_wrapper, commodity_id_wrapper> replace_with, fetch_price<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = state.world.market_get_price(left.data, right.data);
	if(left.data == replace_with.left.data && right.data == replace_with.right.data) {
		return { left, right, replace_with.data };
	}
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
fetch_price_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, fetch_price<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = state.world.market_get_price(left.data, right.data);
	return { left, right, result };
}
template <typename replacement, typename left_t>
land_transportation_demand_satisfaction_result<switch_to_result<left_t>> compute(sys::state const& state, replacement replace_with, land_transportation_demand_satisfaction<left_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto result = state.world.market_get_land_transportation_demand_satisfaction(left.data);
	return { left, result };
}
template <typename replacement, typename left_t>
naval_transportation_demand_satisfaction_result<switch_to_result<left_t>> compute(sys::state const& state, replacement replace_with, naval_transportation_demand_satisfaction<left_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto result = state.world.market_get_naval_transportation_demand_satisfaction(left.data);
	return { left, result };
}
template <typename replacement, typename left_t>
trade_house_budget_import_scale_result<switch_to_result<left_t>> compute(sys::state const& state, replacement replace_with, trade_house_budget_import_scale<left_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto result = state.world.market_get_trade_house_budget_import_scale(left.data);
	return { left, result };
}


template <typename replacement, typename left_t, typename right_t>
fetch_price_confidence_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, fetch_price_confidence<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = state.world.market_get_price_confidence(left.data, right.data);
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
fetch_trade_volume_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, fetch_trade_volume<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = state.world.trade_route_get_volume(left.data, right.data);
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
max_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, max<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = adaptive_ve::max(left.data, right.data);
	return { left, right, result };
}
template <typename replacement, typename left_t, typename right_t>
min_result<switch_to_result<left_t>, switch_to_result<right_t>> compute(sys::state const& state, replacement replace_with, min<left_t, right_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto right = compute(state, replace_with, node.right);
	auto result = adaptive_ve::min(left.data, right.data);
	return { left, right, result };
}
template <typename replacement, typename left_t>
fetch_trade_target_result<switch_to_result<left_t>> compute(sys::state const& state, replacement replace_with, fetch_trade_target<left_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto result = state.world.trade_route_get_target(left.data);
	return { left, result };
}
template <typename replacement, typename left_t>
fetch_trade_origin_result<switch_to_result<left_t>> compute(sys::state const& state, replacement replace_with, fetch_trade_origin<left_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto result = state.world.trade_route_get_origin(left.data);
	return { left, result };
}
template <typename replacement, typename left_t>
fetch_trade_owner_result<switch_to_result<left_t>> compute(sys::state const& state, replacement replace_with, fetch_trade_owner<left_t>& node) {
	auto left = compute(state, replace_with, node.left);
	auto result = state.world.trade_route_get_owner(left.data);
	return { left, result };
}


// DERIVATIVE 

template <typename axis_t, typename condition_t, typename then_t, typename else_t, typename out_type>
float compute_derivative(sys::state const& state, axis_t axis, ite_result<condition_t, then_t, else_t, out_type>& node) {
	if(node.condition.data) {
		return compute_derivative(state, axis, node.left);
	} else {
		return compute_derivative(state, axis, node.right);
	}
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, BEq_result<left_t, right_t>& node) {
	static_assert(false);
	return 0.f;
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, BLEq_result<left_t, right_t>& node) {
	static_assert(false);
	return 0.f;
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, addition_result<left_t, right_t>& node) {
	return compute_derivative(state, axis, node.left) + compute_derivative(state, axis, node.right);
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, multiplication_result<left_t, right_t>& node) {
	return compute_derivative(state, axis, node.left) * node.right.data + node.left.data * compute_derivative(state, axis, node.right);
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, division_result<left_t, right_t>& node) {
	return compute_derivative(state, axis, node.left) / node.right.data - node.left.data * compute_derivative(state, axis, node.right) / node.right.data / node.right.data;
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, subtraction_result<left_t, right_t>& node) {
	return compute_derivative(state, axis, node.left) - compute_derivative(state, axis, node.right);
}
template <typename left_t, typename right_t>
float compute_derivative(sys::state const& state, fetch_price_result<market_id_wrapper, commodity_id_wrapper> axis, fetch_price_result<left_t, right_t>& node) {
	if(node.left.data == axis.left.data && node.right.data == axis.right.data) {
		return 1.f;
	} else {
		return 0.f;
	}
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, fetch_price_result<left_t, right_t>& node) {
	return 0.f;
}
template <typename left_t>
float compute_derivative(sys::state const& state, land_transportation_demand_satisfaction_result<market_id_wrapper> axis, land_transportation_demand_satisfaction_result<left_t>& node) {
	if(node.left.data == axis.left.data) {
		return 1.f;
	} else {
		return 0.f;
	}
}
template <typename axis_t, typename left_t>
float compute_derivative(sys::state const& state, axis_t axis, land_transportation_demand_satisfaction_result<left_t>& node) {
	return 0.f;
}
template <typename left_t>
float compute_derivative(sys::state const& state, naval_transportation_demand_satisfaction_result<market_id_wrapper> axis, naval_transportation_demand_satisfaction_result<left_t>& node) {
	if(node.left.data == axis.left.data) {
		return 1.f;
	} else {
		return 0.f;
	}
}
template <typename axis_t, typename left_t>
float compute_derivative(sys::state const& state, axis_t axis, naval_transportation_demand_satisfaction_result<left_t>& node) {
	return 0.f;
}
template <typename left_t>
float compute_derivative(sys::state const& state, trade_house_budget_import_scale_result<market_id_wrapper> axis, naval_transportation_demand_satisfaction_result<left_t>& node) {
	if(node.left.data == axis.left.data) {
		return 1.f;
	} else {
		return 0.f;
	}
}
template <typename axis_t, typename left_t>
float compute_derivative(sys::state const& state, axis_t axis, trade_house_budget_import_scale_result<left_t>& node) {
	return 0.f;
}
template <typename left_t, typename right_t>
float compute_derivative(sys::state const& state, fetch_price_confidence_result<market_id_wrapper, commodity_id_wrapper> axis, fetch_price_confidence_result<left_t, right_t>& node) {
	if(node.left.data == axis.left.data && node.right.data == axis.right.data) {
		return 1.f;
	} else {
		return 0.f;
	}
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, fetch_price_confidence_result<left_t, right_t>& node) {
	return 0.f;
}
template <typename left_t, typename right_t>
float compute_derivative(sys::state const& state, fetch_trade_volume_result<trade_route_id_wrapper, commodity_id_wrapper> axis, fetch_trade_volume_result<left_t, right_t>& node) {
	if(node.left.data == axis.left.data && node.right.data == axis.right.data) {
		return 1.f;
	} else {
		return 0.f;
	}
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, fetch_trade_volume_result<left_t, right_t>& node) {
	return 0.f;
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, max_result<left_t, right_t>& node) {
	if(node.left.data > node.right.data) {
		return compute_derivative(state, axis, node.left);
	} else if(node.right.data > node.left.data) {
		return compute_derivative(state, axis, node.right);
	} else {
		// we can just put whatever here
		return (compute_derivative(state, axis, node.left) + compute_derivative(state, axis, node.right)) / 2.f;
	}
}
template <typename axis_t, typename left_t, typename right_t>
float compute_derivative(sys::state const& state, axis_t axis, min_result<left_t, right_t>& node) {
	if(node.left.data < node.right.data) {
		return compute_derivative(state, axis, node.left);
	} else if(node.right.data < node.left.data) {
		return compute_derivative(state, axis, node.right);
	} else {
		// we can just put whatever here
		return (compute_derivative(state, axis, node.left) + compute_derivative(state, axis, node.right)) / 2.f;
	}
}
template <typename axis_t, typename left_t>
float compute_derivative(sys::state const& state, axis_t axis, fetch_trade_target_result<left_t>& node) {
	return 0.f;
}
template <typename axis_t, typename left_t>
float compute_derivative(sys::state const& state, axis_t axis, fetch_trade_origin_result<left_t>& node) {
	return 0.f;
}
template <typename axis_t, typename left_t>
float compute_derivative(sys::state const& state, axis_t axis, fetch_trade_owner_result<left_t>& node) {
	return 0.f;
}

void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, text::text_color color, std::string description, float value);
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, value_t_result value);

template <typename left_t_computed, typename middle_left_t_computed, typename middle_right_t_computed, typename right_t_computed>
void evaluate_string(
	sys::state& state,
	text::layout_base& dest,
	text::layout_box& box,
	std::string indent,
	bool assoc,
	//addition<addition<left_t, middle_left_t>, addition<middle_right_t, right_t>>& calculation,
	addition_result<addition_result<left_t_computed, middle_left_t_computed>, addition_result<middle_right_t_computed, right_t_computed>>& result
) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, true, result.left);
		evaluate_string(state, dest, box, indent, true, result.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_add") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(result.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", true, result.left);
		evaluate_string(state, dest, box, indent + "|\t\t", true, result.right);
	}
}
template <typename left_t_computed, typename middle_t_computed, typename right_t_computed>
void evaluate_string(
	sys::state& state,
	text::layout_base& dest,
	text::layout_box& box,
	std::string indent,
	bool assoc,
	//addition<addition<left_t, middle_t>, right_t>& calculation,
	addition_result<addition<left_t_computed, middle_t_computed>, right_t_computed>& result
) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, true, result.left);
		evaluate_string(state, dest, box, indent, false, result.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_add") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(result.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", true, result.left);
		evaluate_string(state, dest, box, indent + "|\t\t", false, result.right);
	}
}
template <typename left_t_computed, typename middle_t_computed, typename right_t_computed>
void evaluate_string(
	sys::state& state,
	text::layout_base& dest,
	text::layout_box& box,
	std::string indent,
	bool assoc,
	//addition<left_t, addition<middle_t, right_t>>& calculation,
	addition_result<left_t_computed, addition_result<middle_t_computed, right_t_computed>>& result
) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, false, result.left);
		evaluate_string(state, dest, box, indent, true, result.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_add") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(result.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", false, result.left);
		evaluate_string(state, dest, box, indent + "|\t\t", true, result.right);
	}
}
template <typename left_t_computed, typename right_t_computed>
void evaluate_string(
	sys::state& state,
	text::layout_base& dest,
	text::layout_box& box,
	std::string indent,
	bool assoc,
	//addition<left_t, right_t>& calculation,
	addition_result<left_t_computed, right_t_computed> result
) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, false, result.left);
		evaluate_string(state, dest, box, indent, false, result.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_add") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(result.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", false, result.left);
		evaluate_string(state, dest, box, indent + "|\t\t", false, result.right);
	}
}


template <typename left_t, typename middle_left_t, typename middle_right_t, typename right_t>
void evaluate_string(
	sys::state& state,
	text::layout_base& dest,
	text::layout_box& box,
	std::string indent,
	bool assoc,
	multiplication_result<multiplication_result<left_t, middle_left_t>, multiplication_result<middle_right_t, right_t>>& calculation
) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, true, calculation.left);
		evaluate_string(state, dest, box, indent, true, calculation.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_mul") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", true, calculation.left);
		evaluate_string(state, dest, box, indent + "|\t\t", true, calculation.right);
	}
}
template <typename left_t, typename middle_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, multiplication_result<multiplication_result<left_t, middle_t>, right_t>& calculation) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, true, calculation.left);
		evaluate_string(state, dest, box, indent, false, calculation.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_mul") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", true, calculation.left);
		evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
	}
}
template <typename left_t, typename middle_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, multiplication_result<left_t, multiplication_result<middle_t, right_t>>& calculation) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, false, calculation.left);
		evaluate_string(state, dest, box, indent, true, calculation.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_mul") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
		evaluate_string(state, dest, box, indent + "|\t\t", true, calculation.right);
	}
}
template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, multiplication_result<left_t, right_t>& calculation) {
	if(assoc) {
		evaluate_string(state, dest, box, indent, false, calculation.left);
		evaluate_string(state, dest, box, indent, false, calculation.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_mul") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
		evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
	}
}

template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, division_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_div") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_dividend") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.left.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_divisor") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}


template <typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, max_result<value_t_result, right_t>& calculation) {
	if(calculation.left.is_constant) {
		text::add_to_layout_box(state, dest, box, indent + "-");
		text::substitution_map m;
		text::add_to_substitution_map(m,
			text::variable_type::x, text::fp_two_places { calculation.left.data }
		);
		text::add_to_substitution_map(m,
			text::variable_type::value, text::fp_two_places{ calculation.data }
		);
		text::localised_format_box(state, dest, box, "term_clamp_low_with", m);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|-", false, calculation.right);
	} else {
		text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_max") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_left") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.left.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
		text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_right") + ": ");
		text::add_to_layout_box(state, dest, box, text::format_float(calculation.right.data), text::text_color::gold);
		text::add_line_break_to_layout_box(state, dest, box);
		evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
	}
}

template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, max_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_max") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_left") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.left.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_right") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}

template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, min_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_min") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_left") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.left.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_right") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}

template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, subtraction_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_sub") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_minuend") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.left.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_subtrahend") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}

template <typename condition_t, typename then_t, typename else_t, typename out_type>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, ite_result<condition_t, then_t, else_t, out_type>& calculation) {
	if(calculation.condition.data) {
		evaluate_string(state, dest, box, indent, assoc, calculation.left);
	} else {
		evaluate_string(state, dest, box, indent, assoc, calculation.right);
	}
}

template <typename left_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, naval_transportation_demand_satisfaction_result<left_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_naval_transport_demand_satisfaction") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
}
template <typename left_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, land_transportation_demand_satisfaction_result<left_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_land_transport_demand_satisfaction") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
}
template <typename left_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, trade_house_budget_import_scale_result<left_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_trade_house_budget_import_scale_result") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
}

template <typename left_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_trade_target_result<left_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_trade_target") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_trade_route") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.left.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
}
template <typename left_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_trade_origin_result<left_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_trade_origin") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_trade_route") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.left.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
}
template <typename left_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_trade_owner_result<left_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_trade_owner") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_trade_route") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.left.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);
}


inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, trade_route_id_wrapper& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_trade_route") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
}

inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, commodity_id_wrapper& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_commodity") + ": ");
	text::add_to_layout_box(state, dest, box, text::get_name_as_string(state, calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
}

template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_trade_volume_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_trade_volume") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_trade_route") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.left.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_commodity") + ": ");
	text::add_to_layout_box(state, dest, box, text::get_name_as_string(state, calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}

inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_result<fetch_trade_target_result<trade_route_id_wrapper>, commodity_id_wrapper>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-");
	text::substitution_map m;
	text::add_to_substitution_map(m,
		text::variable_type::x, text::int_wholenum { calculation.left.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::y, state.world.commodity_get_name(calculation.right.data)
	);
	text::add_to_substitution_map(m,
		text::variable_type::val, text::int_wholenum{ calculation.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::value, text::fp_currency{ calculation.data }
	);
	text::localised_format_box(state, dest, box, "term_market_price_target", m);
	text::add_line_break_to_layout_box(state, dest, box);
}
inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_result<fetch_trade_owner_result<trade_route_id_wrapper>, commodity_id_wrapper>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-");
	text::substitution_map m;
	text::add_to_substitution_map(m,
		text::variable_type::x, text::int_wholenum{ calculation.left.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::y, state.world.commodity_get_name(calculation.right.data)
	);
	text::add_to_substitution_map(m,
		text::variable_type::val, text::int_wholenum{ calculation.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::value, text::fp_currency{ calculation.data }
	);
	text::localised_format_box(state, dest, box, "term_market_price_owner", m);
	text::add_line_break_to_layout_box(state, dest, box);
}
inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_result<fetch_trade_origin_result<trade_route_id_wrapper>, commodity_id_wrapper>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-");
	text::substitution_map m;
	text::add_to_substitution_map(m,
		text::variable_type::x, text::int_wholenum{ calculation.left.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::y, state.world.commodity_get_name(calculation.right.data)
	);
	text::add_to_substitution_map(m,
		text::variable_type::val, text::int_wholenum{ calculation.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::value, text::fp_currency{ calculation.data }
	);
	text::localised_format_box(state, dest, box, "term_market_price_origin", m);
	text::add_line_break_to_layout_box(state, dest, box);
}
template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_market_price") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_market") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.left.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_commodity") + ": ");
	text::add_to_layout_box(state, dest, box, text::get_name_as_string(state, calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}


inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_confidence_result<fetch_trade_target_result<trade_route_id_wrapper>, commodity_id_wrapper>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-");
	text::substitution_map m;
	text::add_to_substitution_map(m,
		text::variable_type::x, text::int_wholenum{ calculation.left.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::y, state.world.commodity_get_name(calculation.right.data)
	);
	text::add_to_substitution_map(m,
		text::variable_type::val, text::int_wholenum{ calculation.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::value, text::fp_two_places{ calculation.data }
	);
	text::localised_format_box(state, dest, box, "term_market_price_confidence_target", m);
	text::add_line_break_to_layout_box(state, dest, box);
}
inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_confidence_result<fetch_trade_owner_result<trade_route_id_wrapper>, commodity_id_wrapper>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-");
	text::substitution_map m;
	text::add_to_substitution_map(m,
		text::variable_type::x, text::int_wholenum{ calculation.left.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::y, state.world.commodity_get_name(calculation.right.data)
	);
	text::add_to_substitution_map(m,
		text::variable_type::val, text::int_wholenum{ calculation.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::value, text::fp_two_places{ calculation.data }
	);
	text::localised_format_box(state, dest, box, "term_market_price_confidence_owner", m);
	text::add_line_break_to_layout_box(state, dest, box);
}
inline void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_confidence_result<fetch_trade_origin_result<trade_route_id_wrapper>, commodity_id_wrapper>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-");
	text::substitution_map m;
	text::add_to_substitution_map(m,
		text::variable_type::x, text::int_wholenum{ calculation.left.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::y, state.world.commodity_get_name(calculation.right.data)
	);
	text::add_to_substitution_map(m,
		text::variable_type::val, text::int_wholenum{ calculation.left.data.index() }
	);
	text::add_to_substitution_map(m,
		text::variable_type::value, text::fp_two_places{ calculation.data }
	);
	text::localised_format_box(state, dest, box, "term_market_price_confidence_origin", m);
	text::add_line_break_to_layout_box(state, dest, box);
}
template <typename left_t, typename right_t>
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, fetch_price_confidence_result<left_t, right_t>& calculation) {
	text::add_to_layout_box(state, dest, box, indent + "-" + text::produce_simple_string(state, "term_market_price_confidence") + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(calculation.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_market") + ": ");
	text::add_to_layout_box(state, dest, box, std::to_string(calculation.left.data.index()), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.left);

	text::add_to_layout_box(state, dest, box, indent + "|-" + text::produce_simple_string(state, "term_commodity") + ": ");
	text::add_to_layout_box(state, dest, box, text::get_name_as_string(state, calculation.right.data), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
	evaluate_string(state, dest, box, indent + "|\t\t", false, calculation.right);
}

template <typename T, typename axis_t> requires (is_term<T> && is_term<T>)
void evaluate_derivative(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, T& calculation, axis_t axis) {	
	text::add_to_layout_box(state, dest, box, text::produce_simple_string(state, "derivative") + " " + text::produce_simple_string(state, axis.description) + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(compute_derivative(state, axis, calculation)), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
}

}



