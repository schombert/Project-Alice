#pragma once

#include "constants_ui.hpp"
#include "text.hpp"
#include "dcon_generated_ids.hpp"
#include "system_state.hpp"
#include "adaptive_ve.hpp"

#include "terms_pure.hpp"

namespace text {
	struct layout_box;
	struct layout_base;
}

namespace term {

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
	text::add_to_layout_box(state, dest, box, text::produce_simple_string(state, "term_derivative") + " " + text::produce_simple_string(state, axis.description) + ": ");
	text::add_to_layout_box(state, dest, box, text::format_float(compute_derivative(state, axis, calculation)), text::text_color::gold);
	text::add_line_break_to_layout_box(state, dest, box);
}

}



