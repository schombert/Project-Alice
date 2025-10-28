namespace sys {
struct state;
}
namespace alice_ui {
bool state_is_rtl(sys::state& state);
}
namespace text {
enum class text_color : uint8_t;
}
namespace ogl {
enum class color_modification;
struct color3f;
}
namespace ui {
ogl::color_modification get_color_modification(bool is_under_mouse, bool is_disabled, bool is_interactable);
ogl::color3f get_text_color(sys::state& state, text::text_color text_color);
}
