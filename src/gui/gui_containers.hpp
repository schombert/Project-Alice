#pragma once

#include "dcon_generated_ids.hpp"
#include "date_interface.hpp"

namespace ui {
struct save_item {
	native_string file_name;
	uint64_t timestamp = 0;
	sys::date save_date;
	dcon::national_identity_id save_flag;
	dcon::government_type_id as_gov;
	bool is_new_game = false;
	bool checksum_match = false;
	std::string name = "fe_new_game";

	bool is_bookmark() const {
		return file_name.starts_with(NATIVE("bookmark_"));
	}

	bool operator==(save_item const& o) const {
		return save_flag == o.save_flag && as_gov == o.as_gov && save_date == o.save_date && is_new_game == o.is_new_game && file_name == o.file_name && timestamp == o.timestamp;
	}
	bool operator!=(save_item const& o) const {
		return !(*this == o);
	}
};
}
