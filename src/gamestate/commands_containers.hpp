#pragma once

#include "events_constants.hpp"

namespace command {
enum class command_type : uint8_t;

// padding due to alignment
struct cmd_header {
	command_type type;
	uint8_t padding = 0;
	dcon::mp_player_id player_id;
	uint32_t payload_size = 0;
	bool operator==(const cmd_header& other) const = default;
	bool operator!=(const cmd_header& other) const = default;

	cmd_header() { };

	cmd_header(cmd_header&& other) = default;
	cmd_header(const cmd_header& other) = default;

	cmd_header& operator=(cmd_header&& other) = default;
	cmd_header& operator=(const cmd_header& other) = default;

};
static_assert(sizeof(command::cmd_header) == sizeof(command::cmd_header::type) + sizeof(command::cmd_header::padding) + sizeof(command::cmd_header::player_id) + sizeof(command::cmd_header::payload_size));

struct command_data {
	cmd_header header{};
	std::vector<uint8_t> payload;
	command_data() {
	};
	command_data(command_type _type) {
		header.type = _type;
	};
	command_data(command_type _type, dcon::mp_player_id _player_id) {
		header.type = _type;
		header.player_id = _player_id;
	};

	command_data(command_data&& other) = default;
	command_data(const command_data& other) = default;

	command_data& operator=(command_data&& other) = default;
	command_data& operator=(const command_data& other) = default;

	// add data to the payload
	template<typename data_type>
	friend command_data& operator << (command_data& msg, data_type& data) {

		static_assert(std::is_standard_layout<data_type>::value, "Data type is too complex");
		size_t curr_size = msg.payload.size();
		msg.payload.resize(msg.payload.size() + sizeof(data_type));

		std::memcpy(msg.payload.data() + curr_size, &data, sizeof(data_type));

		msg.header.payload_size = (uint32_t)msg.payload.size();

		return msg;
	}
	// adds data from pointer to the payload
	template<typename data_type>
	void push_ptr(data_type* ptr, size_t size) {
		size_t curr_size = payload.size();
		payload.resize(payload.size() + sizeof(data_type) * size);

		std::memcpy(payload.data() + curr_size, ptr, sizeof(data_type) * size);

		header.payload_size = (uint32_t)payload.size();
	}
	// adds data from a string to the payload
	void push_string(const std::string& string, size_t size) {
		push_ptr(string.data(), size);
	}
	// adds data from a string_view to the payload
	void push_string_view(std::string_view string_view, size_t size) {
		push_ptr(string_view.data(), size);
	}

	// grab data from the payload
	template<typename data_type>
	friend command_data& operator >> (command_data& msg, data_type& data) {

		static_assert(std::is_standard_layout<data_type>::value, "Data type is too complex");

		size_t i = msg.payload.size() - sizeof(data_type);
		std::memcpy(&data, msg.payload.data() + i, sizeof(data_type));
		msg.payload.resize(i);

		msg.header.payload_size = (uint32_t)msg.payload.size();

		return msg;



	}
	// returns a mutable reference to the payload of the desired type, starting from the start of the vector
	template<typename data_type>
	data_type& get_payload() {
		static_assert(std::is_standard_layout<data_type>::value, "Data type is too complex");
		uint8_t* ptr = payload.data();
		return reinterpret_cast<data_type&>(*ptr);
	}
	// returns a const reference to the payload of the desired type, starting from the start of the vector
	template<typename data_type>
	const data_type& get_payload() const {
		static_assert(std::is_standard_layout<data_type>::value, "Data type is too complex");
		const uint8_t* ptr = payload.data();
		return reinterpret_cast<const data_type&>(*ptr);
	}

	// Checks if the payload of the given type has an additional variable payload of size "expected_size" (in bytes). Returns true if that is the case, false otherwise
	template<typename data_type>
	bool check_variable_size_payload(uint32_t expected_size) const {
		return expected_size == (payload.size() - sizeof(data_type));
	}


	bool operator==(const command_data&) const = default;
	bool operator!=(const command_data&) const = default;

};
static_assert(sizeof(command_data) == sizeof(command_data::header) + sizeof(command_data::payload));

struct pending_human_n_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t primary_slot;
	int32_t from_slot;
	sys::date date;
	dcon::national_event_id e;
	uint8_t opt_choice;
	event::slot_type pt;
	event::slot_type ft;
};
struct pending_human_f_n_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	sys::date date;
	dcon::free_national_event_id e;
	uint8_t opt_choice;
};
struct pending_human_p_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	int32_t from_slot;
	sys::date date;
	dcon::provincial_event_id e;
	dcon::province_id p;
	uint8_t opt_choice;
	event::slot_type ft;
};
struct pending_human_f_p_event_data {
	uint32_t r_lo = 0;
	uint32_t r_hi = 0;
	sys::date date;
	dcon::free_provincial_event_id e;
	dcon::province_id p;
	uint8_t opt_choice;
};

}

namespace event {


}
