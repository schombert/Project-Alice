#pragma once

namespace ui {

enum class production_action : uint8_t {
	subsidise_all,
	unsubsidise_all,
	filter_select_all,
	filter_deselect_all,
	open_all,
	close_all,
	investment_window,
};

}
