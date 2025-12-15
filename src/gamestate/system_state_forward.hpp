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
struct color3f;
}
namespace ui {
ogl::color3f get_text_color(sys::state& state, text::text_color text_color);
}
