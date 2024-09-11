#include "widgets/table.hpp"

namespace table {

template<class item_type>
void _scrollbar<item_type>::on_value_change(sys::state& state, int32_t v) noexcept {
	static_cast<body<item_type>*>(parent)->update(state);
};

}
