#pragma once
#include "gui_element_base.hpp"
#include "system_state.hpp"
#include "economy_government.hpp"
#include "graphics/color.hpp"
#include "province_templates.hpp"
#include "rebels.hpp"
#include "gui_population_window.hpp"

namespace alice_ui {

namespace budget_categories {
inline constexpr int32_t diplomatic_income = 0;
inline constexpr int32_t poor_tax = 1;
inline constexpr int32_t middle_tax = 2;
inline constexpr int32_t rich_tax = 3;
inline constexpr int32_t tariffs_import = 4;
inline constexpr int32_t tariffs_export = 5;
inline constexpr int32_t gold = 6;
inline constexpr int32_t diplomatic_expenses = 7;
inline constexpr int32_t social = 8;
inline constexpr int32_t military = 9;
inline constexpr int32_t education = 10;
inline constexpr int32_t admin = 11;
inline constexpr int32_t domestic_investment = 12;
inline constexpr int32_t overseas_spending = 13;
inline constexpr int32_t subsidies = 14;
inline constexpr int32_t construction = 15;
inline constexpr int32_t army_upkeep = 16;
inline constexpr int32_t navy_upkeep = 17;
inline constexpr int32_t debt_payment = 18;
inline constexpr int32_t stockpile = 19;
inline constexpr int32_t category_count = 20;
inline static bool expanded[category_count] = { false };
}

namespace popwindow {
inline static ankerl::unordered_dense::set<int32_t> open_states;
inline static ankerl::unordered_dense::set<int32_t> open_provs;
inline static ankerl::unordered_dense::set<int32_t> excluded_types;
inline static ankerl::unordered_dense::set<int32_t> excluded_cultures;
inline static ankerl::unordered_dense::set<int32_t> excluded_states;
inline static ankerl::unordered_dense::set<int32_t> excluded_religions;
inline static bool show_non_colonial = true;
inline static bool show_colonial = true;
inline static bool show_only_pops = false;
}

bool pop_passes_filter(sys::state& state, dcon::pop_id p);

std::unique_ptr<ui::element_base> make_macrobuilder2_main(sys::state& state);
std::unique_ptr<ui::element_base> make_budgetwindow_main(sys::state& state);
std::unique_ptr<ui::element_base> make_demographicswindow_main(sys::state& state);
}
