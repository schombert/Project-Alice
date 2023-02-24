#include "nations.hpp"

namespace nations {

void update_national_rankings(sys::state& state) {
	if(!state.national_rankings_out_of_date)
		return;

	state.national_rankings_out_of_date = false;

	// do something to update the rankings
}

}

