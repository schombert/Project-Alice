#pragma once

#include "dcon_generated_ids.hpp"
#include "widgets/table.hpp"
#include "gui_element_types.hpp"

namespace ui {

class trade_flow_window;
class trade_details_window;

class trade_window : public window_element_base {
	trade_flow_window* trade_flow_win = nullptr;
	trade_details_window* details_win = nullptr;
	dcon::commodity_id commodity_id{ 1 };

	table::display<dcon::commodity_id>* table_nation = nullptr;
	table::display<dcon::commodity_id>* table_global = nullptr;
	table::display<dcon::nation_id>* table_trade_good_stats = nullptr;
	table::display<dcon::market_id>* table_trade_good_stats_market = nullptr;

public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

}
