#pragma once
#include "gui_element_base.hpp"
#include "system_state.hpp"

namespace alice_ui {
namespace budget_categories {
inline constexpr int32_t diplomatic_income = 0;
inline constexpr int32_t poor_tax = 1;
inline constexpr int32_t middle_tax = 2;
inline constexpr int32_t rich_tax = 3;
inline constexpr int32_t tariffs = 4;
inline constexpr int32_t gold = 5;
inline constexpr int32_t diplomatic_expenses = 6;
inline constexpr int32_t social = 7;
inline constexpr int32_t military = 8;
inline constexpr int32_t education = 9;
inline constexpr int32_t admin = 10;
inline constexpr int32_t domestic_investment = 11;
inline constexpr int32_t overseas_spending = 12;
inline constexpr int32_t subsidies = 13;
inline constexpr int32_t construction = 14;
inline constexpr int32_t army_upkeep = 15;
inline constexpr int32_t navy_upkeep = 16;
inline constexpr int32_t debt_payment = 17;
inline constexpr int32_t category_count = 18;

inline static bool expanded[category_count] = { false };
}

std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
std::unique_ptr<ui::element_base> make_budgetwindow_main(sys::state& state);
}
