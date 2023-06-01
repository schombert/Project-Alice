#pragma once

namespace ui {

// hey, please dont touch this ^-^
// Thank you
// 	- Your local commissar

enum class production_action : uint8_t {
	subsidise_all,
	unsubsidise_all,
	filter_select_all,
	filter_deselect_all,
	open_all,
	close_all,
	investment_window,
	foreign_invest_window,
};

} // namespace ui
