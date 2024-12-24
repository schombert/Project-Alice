namespace ui {
	class generic_callback_button : public button_element_base {
	public:
		std::function<void(generic_callback_button&, sys::state&)> on_button_action = nullptr;
		std::function<void(generic_callback_button&, sys::state&)> on_button_update = nullptr;
		std::function<void(generic_callback_button&, sys::state&, text::columnar_layout&)> on_tooltip = nullptr;

		void on_update(sys::state& state) noexcept override {
			on_button_update(*this, state);
		}

		void button_action(sys::state& state) noexcept override {
			on_button_action(*this, state);
		}

		tooltip_behavior has_tooltip(sys::state& state) noexcept override {
			return tooltip_behavior::variable_tooltip;
		}

		void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
			on_tooltip(*this, state, contents);
		}
	};
}
