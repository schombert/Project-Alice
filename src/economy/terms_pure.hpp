#pragma once


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
	x* y;
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
constexpr value_t ensure_wrapper(float& val) {
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
template <typename right_t>
term::BEq<term::value_t, term::ensure_wrapper_t<right_t>> operator==(term::value_t& Left, right_t Right) {
	return { Left, ensure_wrapper(Right) };
}
template <typename left_t> 
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
template <typename right_t> 
term::BLEq<term::value_t, term::ensure_wrapper_t<right_t>> operator<=(term::value_t& Left, right_t Right) {
	return { Left, term::ensure_wrapper(Right) };
}
template <typename left_t> 
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
term::ite<term::ensure_wrapper_t<condition_t>, term::ensure_wrapper_t<left_t>, term::ensure_wrapper_t<right_t>, out_t> select(condition_t Condition, left_t Left, right_t Right) {
	auto left = term::ensure_wrapper(Left);
	auto right = term::ensure_wrapper(Right);
	return { term::ensure_wrapper(Condition), left,  right };
}
}
