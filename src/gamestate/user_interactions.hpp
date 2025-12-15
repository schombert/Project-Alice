namespace sys {
void on_mouse_wheel(sys::state& state, int32_t x, int32_t y, key_modifiers mod, float amount); // an amount of 1.0 is one "click" of the wheel
void process_dialog_boxes(sys::state& state);
void open_diplomacy_window(sys::state& state, dcon::nation_id target);
}
