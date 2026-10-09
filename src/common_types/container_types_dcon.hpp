#pragma once
#include "dcon_generated_ids.hpp"
#include "date_interface.hpp"
#include "glm/glm.hpp"
#include "commands_containers.hpp"
#ifdef _WIN64 // WINDOWS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#ifndef WINSOCK2_IMPORTED
#define WINSOCK2_IMPORTED
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#else // NIX
#include <netinet/in.h>
#include <sys/socket.h>
#endif


// THIS FILE IS SUPPOSED TO STORE ONLY CONTAINERS USED IN DCON DIRECTLY


namespace sys {

struct gamerule_option {
	dcon::text_key name;

	dcon::text_key on_select_lua_function;
	dcon::text_key on_deselect_lua_function;


	bool operator==(const gamerule_option& other) const = default;
	bool operator!=(const gamerule_option& other) const = default;
};

struct value_modifier_segment {
	float factor = 0.0f;
	dcon::trigger_key condition;
	uint16_t padding = 0;
};
static_assert(sizeof(value_modifier_segment) ==
	sizeof(value_modifier_segment::factor)
	+ sizeof(value_modifier_segment::condition)
	+ sizeof(value_modifier_segment::padding));

struct value_modifier_description {
	float factor = 0.0f;
	float base = 0.0f;
	uint16_t first_segment_offset = 0;
	uint16_t segments_count = 0;
};
static_assert(sizeof(value_modifier_description) ==
	sizeof(value_modifier_description::factor)
	+ sizeof(value_modifier_description::base)
	+ sizeof(value_modifier_description::first_segment_offset)
	+ sizeof(value_modifier_description::segments_count));

struct event_option {
	dcon::text_key name;
	dcon::value_modifier_key ai_chance;
	dcon::effect_key effect;

	bool operator==(const event_option& other) const = default;
	bool operator!=(const event_option& other) const = default;
};

static_assert(sizeof(event_option) ==
	sizeof(event_option::name)
	+ sizeof(event_option::ai_chance)
	+ sizeof(event_option::effect));


struct commodity_modifier {
	float amount = 0.0f;
	dcon::commodity_id type;
	uint8_t padding[3] = { 0 };
	bool operator==(const commodity_modifier& other) const {
		return other.type == type && other.amount == amount;
	}
	bool operator!=(const commodity_modifier& other) const {
		return !(other == *this);
	}

};
static_assert(sizeof(commodity_modifier) ==
	sizeof(commodity_modifier::amount)
	+ sizeof(commodity_modifier::type)
	+ sizeof(commodity_modifier::padding));

struct unit_variable_stats {
	int32_t build_time = 0;
	int32_t default_organisation = 0;
	float maximum_speed = 0.0f;
	float defence_or_hull = 0.0f;
	float attack_or_gun_power = 0.0f;
	float supply_consumption = 0.0f;
	float support = 0.0f;
	float siege_or_torpedo_attack = 0.0f;
	float reconnaissance_or_fire_range = 0.0f;
	float discipline_or_evasion = 0.0f;
	float maneuver = 0.0f;

	void operator+=(unit_variable_stats const& other) {
		build_time += other.build_time;
		default_organisation += other.default_organisation;
		maximum_speed += other.maximum_speed;
		defence_or_hull += other.defence_or_hull;
		attack_or_gun_power += other.attack_or_gun_power;
		supply_consumption += other.supply_consumption;
		support += other.support;
		maneuver += other.maneuver;
		siege_or_torpedo_attack += other.siege_or_torpedo_attack;
		reconnaissance_or_fire_range += other.reconnaissance_or_fire_range;
		discipline_or_evasion += other.discipline_or_evasion;
	}
	void operator-=(unit_variable_stats const& other) {
		build_time -= other.build_time;
		default_organisation -= other.default_organisation;
		maximum_speed -= other.maximum_speed;
		defence_or_hull -= other.defence_or_hull;
		attack_or_gun_power -= other.attack_or_gun_power;
		supply_consumption -= other.supply_consumption;
		support -= other.support;
		maneuver -= other.maneuver;
		siege_or_torpedo_attack -= other.siege_or_torpedo_attack;
		reconnaissance_or_fire_range -= other.reconnaissance_or_fire_range;
		discipline_or_evasion -= other.discipline_or_evasion;
	}
	bool operator==(const unit_variable_stats& other) const = default;
	bool operator!=(const unit_variable_stats& other) const = default;
};
static_assert(sizeof(unit_variable_stats) ==
	sizeof(unit_variable_stats::build_time)
	+ sizeof(unit_variable_stats::default_organisation)
	+ sizeof(unit_variable_stats::maximum_speed)
	+ sizeof(unit_variable_stats::defence_or_hull)
	+ sizeof(unit_variable_stats::attack_or_gun_power)
	+ sizeof(unit_variable_stats::supply_consumption)
	+ sizeof(unit_variable_stats::support)
	+ sizeof(unit_variable_stats::maneuver)
	+ sizeof(unit_variable_stats::siege_or_torpedo_attack)
	+ sizeof(unit_variable_stats::reconnaissance_or_fire_range)
	+ sizeof(unit_variable_stats::discipline_or_evasion));

struct provincial_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 22;

	float values[modifier_definition_size] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	dcon::provincial_modifier_value offsets[modifier_definition_size] = { dcon::provincial_modifier_value{} };
	uint16_t padding = 0;

	bool add_manual_modifier(dcon::provincial_modifier_value modifier, float value) {
		if(value == 0.0f) return true;
		for(uint32_t i = 0; i < modifier_definition_size; i++) {
			if(offsets[i] == dcon::provincial_modifier_value{}) {
				offsets[i] = modifier;
				values[i] = value;
				return true;
			}
		}
		return false;
	}

	bool operator==(const provincial_modifier_definition& other) const {
		return std::memcmp(this->values, other.values, sizeof(values)) == 0 && std::memcmp(this->offsets, other.offsets, sizeof(offsets)) == 0;
	}
	bool operator!=(const provincial_modifier_definition& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(provincial_modifier_definition) ==
	sizeof(provincial_modifier_definition::values)
	+ sizeof(provincial_modifier_definition::offsets)
	+ sizeof(provincial_modifier_definition::padding));


struct national_modifier_definition {
	static constexpr uint32_t modifier_definition_size = 24;

	float values[modifier_definition_size] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	dcon::national_modifier_value offsets[modifier_definition_size] = { dcon::national_modifier_value{} };


	bool add_manual_modifier(dcon::national_modifier_value modifier, float value) {
		if(value == 0.0f) return true;
		for(uint32_t i = 0; i < modifier_definition_size; i++) {
			if(offsets[i] == dcon::national_modifier_value{}) {
				offsets[i] = modifier;
				values[i] = value;
				return true;
			}
		}
		return false;
	}


	bool operator==(const national_modifier_definition& other) const {
		return std::memcmp(this->values, other.values, sizeof(values)) == 0 && std::memcmp(this->offsets, other.offsets, sizeof(offsets)) == 0;
	}
	bool operator!=(const national_modifier_definition& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(national_modifier_definition) ==
	sizeof(national_modifier_definition::values)
	+ sizeof(national_modifier_definition::offsets));

struct unit_modifier : public unit_variable_stats {
	dcon::unit_type_id type;
	uint8_t padding[3] = { 0 };
	bool operator==(const unit_modifier& other) const {
		return dynamic_cast<const unit_variable_stats&>(other) == dynamic_cast<const unit_variable_stats&>(*this) && other.type == type;
	}
	bool operator!=(const unit_modifier& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(unit_modifier) ==
	sizeof(unit_variable_stats)
	+ sizeof(unit_modifier::type)
	+ sizeof(unit_modifier::padding));

struct rebel_org_modifier {
	float amount = 0.0f; //4
	dcon::rebel_type_id type; //1 - no type set = all rebels
	uint8_t padding[2] = { 0, 0 };
	bool operator==(const rebel_org_modifier& other) const {
		return other.amount == amount && other.type == type;
	}
	bool operator!=(const rebel_org_modifier& other) const {
		return !(other == *this);
	}
};
static_assert(sizeof(rebel_org_modifier) ==
	sizeof(rebel_org_modifier::amount)
	+ sizeof(rebel_org_modifier::type)
	+ sizeof(rebel_org_modifier::padding));

struct dated_modifier {
	sys::date expiration;
	dcon::modifier_id mod_id;
	bool operator==(const dated_modifier& other) const = default;
	bool operator!=(const dated_modifier& other) const = default;
};
static_assert(sizeof(dated_modifier) ==
	sizeof(dated_modifier::expiration)
	+ sizeof(dated_modifier::mod_id));





template<size_t _Size>
struct player_value {
	std::array<uint8_t, _Size> data = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	std::string_view to_string_view() const noexcept {
		for(uint32_t i = 0; i < sizeof(data); i++) {
			if(data[i] == ' ' || data[i] == '\0') {
				return std::string_view{ reinterpret_cast<const char*>(&data[0]), uint32_t(i) };
			}
		}
		return std::string_view{ reinterpret_cast<const char*>(&data[0]), sizeof(data) };
	}

	player_value<_Size> from_string_view(std::string_view sv) noexcept {
		size_t length_to_copy = std::min(sv.size(), data.size());
		sv.copy(reinterpret_cast<char*>(data.data()), length_to_copy);
		return *this;
	}

	bool is_equal(const player_value<_Size>& other) const {
		return other.data == data;
	}

	bool operator==(const player_value<_Size>& other) const {
		return is_equal(other);
	}
	bool operator!=(const player_value<_Size>& other) const {
		return !(*this == other);
	}

	std::string to_string() const noexcept {
		return std::string(to_string_view());
	}

	bool empty() noexcept {
		return data[0] == ' ' || data[0] == '\0';
	}

	void append(char c) noexcept {
		for(uint32_t i = 0; i < sizeof(data); i++) {
			if(data[i] == ' ' || data[i] == '\0') {
				data[i] = c;
				return;
			}
		}
	}

	char pop() noexcept {
		for(uint32_t i = 1; i < sizeof(data); i++) {
			if(data[i] == ' ' || data[i] == '\0') {
				auto pop = data[i - 1];
				data[i - 1] = ' ';
				return pop;
			}
		}
		return ' ';
	}
};

using player_name = player_value<24>;
using player_password_salt = player_value<24>;
using player_password_hash = player_value<64>;
using player_password_raw = player_value<24>;

static_assert(sizeof(player_name) == sizeof(player_name::data));
static_assert(sizeof(player_password_salt) == sizeof(player_password_salt::data));
static_assert(sizeof(player_password_hash) == sizeof(player_password_hash::data));
static_assert(sizeof(player_password_raw) == sizeof(player_password_raw::data));





// A fixed-size array wrapper which implements a vector-like interface for keeping track of size.
template<typename data_type, uint32_t capacity>
class fixed_size_vector {
protected:
	uint32_t storage_size = 0;
	std::array<data_type, capacity> _storage{};

	void init() {
		std::memset(this, 0, sizeof(fixed_size_vector)); // Call this in every ctor to make sure that all potential padding is zero'd out
	}

public:

	using iterator = std::array<data_type, capacity>::iterator;
	using const_iterator = std::array<data_type, capacity>::const_iterator;
	using reverse_iterator = std::array<data_type, capacity>::reverse_iterator;
	using const_reverse_iterator = std::array<data_type, capacity>::const_reverse_iterator;

	constexpr fixed_size_vector() {
		init();
	}

	constexpr fixed_size_vector(const std::initializer_list<data_type> initializer) {
		init();
		assert(initializer.size() <= capacity);
		storage_size = initializer.size();
		std::copy(initializer.begin(), initializer.end(), data());
	}
	constexpr fixed_size_vector(uint32_t size) {
		init();
		assert(size <= total_capacity() && size >= 0);
		std::fill_n(_storage.data(), size, data_type{});
		storage_size = size;
	}
	constexpr fixed_size_vector(uint32_t size, const data_type& init_val) {
		init();
		assert(size <= total_capacity() && size >= 0);
		std::fill_n(_storage.data(), size, init_val);
		storage_size = size;
	}

	constexpr fixed_size_vector(const fixed_size_vector& obj) {
		std::memcpy(this, &obj, sizeof(fixed_size_vector));
	}

	constexpr fixed_size_vector(fixed_size_vector&& obj) {
		std::memcpy(this, &obj, sizeof(fixed_size_vector));
	}

	fixed_size_vector& operator=(fixed_size_vector const& other) noexcept {
		std::memcpy(this, &other, sizeof(fixed_size_vector));
		return *this;
	}
	fixed_size_vector& operator=(fixed_size_vector&& other) noexcept {
		std::memcpy(this, &other, sizeof(fixed_size_vector));
		return *this;
	}
	// Use memcmp in equals operator to check for padding too
	bool operator==(fixed_size_vector const& other) const {
		return std::memcmp(this, &other, sizeof(fixed_size_vector)) == 0;
	}
	bool operator!=(fixed_size_vector const& other) const {
		return !(*this == other);
	}

	constexpr operator std::span<data_type>() { return std::span<data_type>(begin(), end()); };

	constexpr uint32_t total_capacity() const {
		return capacity;
	}


	const data_type* data() const {
		return _storage.data();
	}
	data_type* data() {
		return _storage.data();
	}


	constexpr data_type const& operator[](uint32_t index) const {
		assert(index < size() && index >= 0);
		return _storage[index];
	}
	constexpr data_type& operator[](uint32_t index) {
		assert(index < size() && index >= 0);
		return _storage[index];
	}
	// This will remove the element at the given index by moving it to the end of the collection and then popping it
	constexpr void remove_at(uint32_t index) {
		assert(index < size() && index >= 0);
		std::swap(_storage[index], _storage[size() - 1]);
		pop_back();
	}
	// This will remove the given iterator element by moving it to the end of the collection and then popping it
	constexpr void remove_at(const_iterator iterator) {
		size_t index = iterator - begin();
		remove_at(index);
	}

	constexpr void clear() {
		_storage.fill(data_type{});
		storage_size = 0;
	}

	constexpr auto begin() const {
		return _storage.begin();
	}
	constexpr auto begin() {
		return _storage.begin();
	}
	constexpr auto end() const {
		return const_iterator(data(), size());
	}
	constexpr auto end() {
		return iterator(data(), size());
	}
	constexpr auto rbegin() {
		return reverse_iterator(end());
	}
	constexpr auto rbegin() const {
		return const_reverse_iterator(end());
	}
	constexpr auto rend() const {
		return const_reverse_iterator(begin());
	}
	constexpr auto rend() {
		return reverse_iterator(begin());
	}
	constexpr uint32_t size() const {
		return storage_size;
	}
	constexpr void resize(uint32_t new_size, const data_type& val = data_type{ }) {
		assert(size() <= total_capacity() && new_size >= 0);
		if(new_size < size()) {
			std::fill_n(&_storage[new_size], size() - new_size, data_type{ });
		}
		else {
			std::fill_n(&_storage[size()], new_size - size(), val);
		}
		storage_size = new_size;
	}
	constexpr void pop_back() {
		assert(size() != 0);
		_storage[size() - 1] = data_type{ };
		storage_size--;
	}
	// Returns true if there were enough capacity to add the item, false if not
	constexpr bool push_back(data_type&& v) {
		if(size() != capacity) {
			_storage[size()] = std::move(v);
			storage_size++;
			return true;
		} else {
			return false;
		}
	}
	// Returns true if there were enough capacity to add the item, false if not
	constexpr bool push_back(const data_type& v) {
		if(size() != capacity) {
			_storage[size()] = v;
			storage_size++;
			return true;
		} else {
			return false;
		}
	}

	// Pushes back the item regardless of the capacity. Caller's responsibility to check for appropriate capacity
	constexpr void push_back_unsafe(data_type&& v) {
		assert(size() != capacity);
		_storage[size()] = std::move(v);
		storage_size++;
	}
	// Pushes back the item regardless of the capacity. Caller's responsibility to check for appropriate capacity
	constexpr void push_back_unsafe(const data_type& v) {
		assert(size() != capacity);
		_storage[size()] = v;
		storage_size++;
	}
	constexpr data_type& back() {
		return _storage[size() - 1];
	}
	constexpr data_type const& back() const {
		return _storage[size() - 1];
	}
	constexpr data_type& front() {
		return _storage.front();
	}
	constexpr data_type const& front() const {
		return _storage.front();
	}

};

}


namespace economy {



constexpr uint32_t small_set_size = 6;
constexpr uint32_t normal_set_size = 8;


struct commodity_set {
	static constexpr uint32_t set_size = normal_set_size;

	float commodity_amounts[set_size] = {};
	dcon::commodity_id commodity_type[set_size] = {};

	bool operator==(const commodity_set& other) const {
		return std::memcmp(this->commodity_amounts, other.commodity_amounts, sizeof(commodity_amounts)) == 0 && std::memcmp(this->commodity_type, other.commodity_type, sizeof(commodity_type)) == 0;
	}
	bool operator!=(const commodity_set& other) const {
		return !(other == *this);
	}
	template<typename F>
	void for_each_commodity(const F&& function) const {
		for(uint32_t i = 0; i < set_size; ++i) {
			if(commodity_type[i]) {
				function(commodity_type[i], commodity_amounts[i]);
			} else {
				break;
			}
		}
	}
	template<typename F>
	void for_each_commodity(F&& function) {
		for(uint32_t i = 0; i < set_size; ++i) {
			if(commodity_type[i]) {
				function(commodity_type[i], commodity_amounts[i]);
			} else {
				break;
			}
		}
	}
	template<typename F>
	void for_each_valid_index(F&& function) const {
		for(uint32_t i = 0; i < set_size; ++i) {
			if(commodity_type[i]) {
				function(i);
			} else {
				break;
			}
		}
	}
	// Tries to add a commodity to the first free slot. Returns the index it was added to if sucessful, or -1 if no slot available or if the commodity is already added
	int16_t try_add(dcon::commodity_id cid, float amount) {
		assert(cid);
		for(uint32_t i = 0; i < set_size; ++i) {
			if(!commodity_type[i]) {
				commodity_type[i] = cid;
				commodity_amounts[i] = amount;
				return i;
			} else if(commodity_type[i] == cid) {
				return -1;
			}
		}
		return -1;
	}

	void copy_all_to(commodity_set& dest) const {
		std::memcpy(&dest, this, sizeof(commodity_set));
	}
	void copy_types_to(commodity_set& dest) const {
		std::memcpy(&dest.commodity_type, this->commodity_type, sizeof(commodity_set::commodity_type));
	}
	void copy_amounts_to(commodity_set& dest) const {
		std::memcpy(&dest.commodity_amounts, this->commodity_amounts, sizeof(commodity_set::commodity_amounts));
	}

	void clear_types() {
		std::memset(&commodity_type, 0, sizeof(commodity_type));
	}
	void clear_amounts() {
		std::memset(&commodity_amounts, 0, sizeof(commodity_amounts));
	}
	void clear_all() {
		std::memset(this, 0, sizeof(commodity_set));
	}
	uint32_t size_used() const {
		uint32_t count = 0;
		for(uint32_t i = 0; i < set_size; i++) {
			if(commodity_type[i]) {
				count++;
			} else {
				break;
			}
		}
		return count;
	}

};

struct small_commodity_set {
	static constexpr uint32_t set_size = small_set_size;

	float commodity_amounts[set_size] = {};
	dcon::commodity_id commodity_type[set_size] = {};
	uint16_t padding = { };

	bool operator==(const small_commodity_set& other) const {
		return std::memcmp(this->commodity_amounts, other.commodity_amounts, sizeof(commodity_amounts)) == 0 && std::memcmp(this->commodity_type, other.commodity_type, sizeof(commodity_type)) == 0;
	}
	bool operator!=(const small_commodity_set& other) const {
		return !(other == *this);
	}
	template<typename F>
	void for_each_commodity(const F&& function) const {
		for(uint32_t i = 0; i < set_size; ++i) {
			if(commodity_type[i]) {
				function(commodity_type[i], commodity_amounts[i]);
			} else {
				break;
			}
		}
	}
	template<typename F>
	void for_each_commodity(F&& function) {
		for(uint32_t i = 0; i < set_size; ++i) {
			if(commodity_type[i]) {
				function(commodity_type[i], commodity_amounts[i]);
			} else {
				break;
			}
		}
	}
	template<typename F>
	void for_each_valid_index(F&& function) const {
		for(uint32_t i = 0; i < set_size; ++i) {
			if(commodity_type[i]) {
				function(i);
			} else {
				break;
			}
		}
	}
	// Tries to add a commodity to the first free slot. Returns the index it was added to if sucessful, or -1 if no slot available or if the commodity is already added
	int16_t try_add(dcon::commodity_id cid, float amount) {
		assert(cid);
		for(uint32_t i = 0; i < set_size; ++i) {
			if(!commodity_type[i]) {
				commodity_type[i] = cid;
				commodity_amounts[i] = amount;
				return i;
			} else if(commodity_type[i] == cid) {
				return -1;
			}
		}
		return -1;
	}

	void copy_all_to(small_commodity_set& dest) const {
		std::memcpy(&dest, this, sizeof(small_commodity_set));
	}
	void copy_types_to(small_commodity_set& dest) const {
		std::memcpy(&dest.commodity_type, this->commodity_type, sizeof(small_commodity_set::commodity_type));
	}
	void copy_amounts_to(small_commodity_set& dest) const {
		std::memcpy(&dest.commodity_amounts, this->commodity_amounts, sizeof(small_commodity_set::commodity_amounts));
	}

	void clear_types() {
		std::memset(&commodity_type, 0, sizeof(commodity_type));
	}
	void clear_amounts() {
		std::memset(&commodity_amounts, 0, sizeof(commodity_amounts));
	}
	void clear_all() {
		std::memset(this, 0, sizeof(small_commodity_set));
	}
	uint32_t size_used() const {
		uint32_t count = 0;
		for(uint32_t i = 0; i < set_size; i++) {
			if(commodity_type[i]) {
				count++;
			} else {
				break;
			}
		}
		return count;
	}

};
static_assert(sizeof(commodity_set) ==
	sizeof(commodity_set::commodity_amounts) +
	+ sizeof(commodity_set::commodity_type));


static_assert(sizeof(small_commodity_set) ==
	sizeof(small_commodity_set::commodity_amounts) +
	+ sizeof(small_commodity_set::commodity_type) +
	sizeof(small_commodity_set::padding)); // take into account two padding bytes


using commodity_amounts = std::array<float, normal_set_size>;
static_assert(sizeof(commodity_amounts) == normal_set_size * sizeof(float));

struct production_type_bonus {
	float amount = 0.0f;
	dcon::trigger_key trigger = dcon::trigger_key{};
	uint8_t padding[2] = { 0 };
};
static_assert(sizeof(production_type_bonus) ==
	sizeof(production_type_bonus::amount)
	+ sizeof(production_type_bonus::trigger)
	+ sizeof(production_type_bonus::padding));

} // namespace economy


namespace military {


struct ship_in_battle {
	static constexpr uint16_t distance_mask = 0x03FF;

	static constexpr uint16_t mode_mask = 0x1C00;
	static constexpr uint16_t mode_seeking = 0x0400;
	static constexpr uint16_t mode_approaching = 0x0800;
	static constexpr uint16_t mode_engaged = 0x0C00;
	static constexpr uint16_t mode_retreating = 0x1000;
	static constexpr uint16_t mode_retreated = 0x1400;
	static constexpr uint16_t mode_sunk = 0x0000;

	static constexpr uint16_t is_attacking = 0x2000;

	static constexpr uint16_t type_mask = 0xC000;
	static constexpr uint16_t type_big = 0x4000;
	static constexpr uint16_t type_small = 0x8000;
	static constexpr uint16_t type_transport = 0x0000;

	dcon::ship_id ship;
	int16_t target_slot = -1;
	uint16_t flags = 0;
	uint16_t ships_targeting_this = 0;
	bool operator == (const ship_in_battle&) const = default;
	bool operator != (const ship_in_battle&) const = default;

	uint16_t get_distance() const {
		return flags & distance_mask;
	}
	void set_distance(uint16_t distance) {
		flags &= ~distance_mask;
		flags |= distance_mask & (distance);

	}
};
static_assert(sizeof(ship_in_battle) ==
	sizeof(ship_in_battle::ship)
	+ sizeof(ship_in_battle::ships_targeting_this)
	+ sizeof(ship_in_battle::target_slot)
	+ sizeof(ship_in_battle::flags));

struct mobilization_order {
	sys::date when; //2
	dcon::province_id where; //2
	bool operator==(const mobilization_order& other) const = default;
	bool operator!=(const mobilization_order& other) const = default;
};
static_assert(sizeof(mobilization_order) ==
	sizeof(mobilization_order::where)
	+ sizeof(mobilization_order::when));

struct battle_regiment {
	
	static constexpr uint16_t dig_in_mask = 0x3e0;

	// Crossing works as an enum with only one state being allowed.
	static constexpr uint16_t crossing_mask = 0x0018;
	static constexpr uint16_t crossing_strait = 0x0010;
	static constexpr uint16_t crossing_river = 0x008;
	static constexpr uint16_t crossing_none = 0x0000;

	// unit type works as an enum with only one state being allowed.
	static constexpr uint16_t type_mask = 0x0006;
	static constexpr uint16_t type_infantry = 0x0000;
	static constexpr uint16_t type_cavalry = 0x0002;
	static constexpr uint16_t type_support = 0x0004;

	static constexpr uint16_t is_attacking = 0x0001;

	dcon::regiment_id regiment = dcon::regiment_id{};
	uint16_t flags = 0;

	constexpr battle_regiment() : regiment(dcon::regiment_id{ }), flags(0) {

	}

	constexpr battle_regiment(dcon::regiment_id _regiment, bool attacking, uint16_t type, uint16_t crossing, uint8_t dig_in) : regiment(_regiment) {
		assert(type <= type_mask);
		flags = uint16_t(uint16_t(attacking) | type | crossing);
		flags |= (dig_in << 5) & dig_in_mask;
	}

	constexpr bool get_is_attacking() const {
		return flags & is_attacking;
	}
	constexpr void set_is_attacking(bool attacking) {
		flags &= ~is_attacking;
		flags |= (is_attacking & attacking);
	}

	constexpr uint16_t get_type() const {
		return flags & type_mask;
	}
	constexpr void set_type(uint16_t type) {
		flags &= ~type_mask;
		flags |= (type_mask & type);

	}

	constexpr uint16_t get_crossing() const {
		return flags & crossing_mask;
	}
	constexpr void set_crossing(uint16_t crossing) {
		flags &= ~crossing_mask;
		flags |= (crossing_mask & crossing);
	}

	constexpr uint8_t get_dig_in() const {
		return uint8_t((flags & dig_in_mask) >> 5);
	}
	constexpr void set_dig_in(uint8_t dig_in) {
		flags &= ~dig_in_mask;
		flags |= (dig_in_mask & (dig_in << 5));

	}

	bool operator==(const battle_regiment& other) const = default;
	bool operator!=(const battle_regiment& other) const = default;
};
static_assert(sizeof(battle_regiment) ==
	sizeof(battle_regiment::regiment)
	+ sizeof(battle_regiment::flags));


struct available_cb {
	sys::date expiration; //2
	dcon::nation_id target; //2
	dcon::cb_type_id cb_type; //2
	dcon::state_definition_id target_state;
	bool operator==(const available_cb& other) const = default;
	bool operator!=(const available_cb& other) const = default;
};
static_assert(sizeof(available_cb) ==
	+sizeof(available_cb::target)
	+ sizeof(available_cb::expiration)
	+ sizeof(available_cb::cb_type) +
	sizeof(available_cb::target_state));

}

namespace network {
struct client_handshake_data {
	sys::player_name nickname;
	sys::player_password_raw player_password;
	uint8_t lobby_password[16] = { 0 };
};

enum class client_state : uint8_t {
	normal = 0,
	flushing = 1,
	shutting_down = 2
};
#ifdef _WIN64
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

}
