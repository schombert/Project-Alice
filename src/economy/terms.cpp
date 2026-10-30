#include "terms.hpp"

namespace term {
void evaluate_string(sys::state& state, text::layout_base& dest, text::layout_box& box, std::string indent, bool assoc, value_t_result value) {
	evaluate_string(state, dest, box, indent, value.color, value.description, value.data);
};
}
