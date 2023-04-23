#pragma once

#include <cstdint>
namespace sys {

struct state;

struct year_month_day {
	int32_t year;
	uint16_t month;
	uint16_t day;
};

class absolute_time_point {
	int64_t days = 0;
	absolute_time_point(int64_t days) : days(days) { }
public:
	absolute_time_point() noexcept = default;
	absolute_time_point(year_month_day const& date);
	absolute_time_point(absolute_time_point const&) noexcept = default;
	absolute_time_point(absolute_time_point&&) noexcept = default;
	absolute_time_point& operator=(absolute_time_point const&) noexcept = default;
	absolute_time_point& operator=(absolute_time_point&&) noexcept = default;
	constexpr int64_t to_days() const noexcept {
		return days;
	}

	constexpr bool operator==(absolute_time_point v) const noexcept {
		return days == v.days;
	}
	constexpr bool operator!=(absolute_time_point v) const noexcept {
		return days != v.days;
	}
	constexpr bool operator<(absolute_time_point v) const noexcept {
		return days < v.days;
	}
	constexpr bool operator<=(absolute_time_point v) const noexcept {
		return days <= v.days;
	}
	constexpr bool operator>(absolute_time_point v) const noexcept {
		return days > v.days;
	}
	constexpr bool operator>=(absolute_time_point v) const noexcept {
		return days >= v.days;
	}
	absolute_time_point operator+(int32_t v) const noexcept {
		return absolute_time_point(days + v);
	}
	absolute_time_point& operator+=(int32_t v) noexcept {
		days += v;
		return *this;
	}
};

class date {
public:
	using value_base_t = uint16_t;
	using zero_is_null_t = std::true_type;

	uint16_t value = 0;

	constexpr date() noexcept = default;
	explicit constexpr date(uint16_t v) noexcept : value(v + 1) { }
	constexpr date(date const& v) noexcept = default;
	constexpr date(date&& v) noexcept = default;
	date(year_month_day const& v, absolute_time_point base) noexcept;

	date& operator=(date const& v) noexcept = default;
	date& operator=(date&& v) noexcept = default;
	constexpr bool operator==(date v) const noexcept {
		return value == v.value;
	}
	constexpr bool operator!=(date v) const noexcept {
		return value != v.value;
	}
	constexpr bool operator<(date v) const noexcept {
		return value < v.value;
	}
	constexpr bool operator<=(date v) const noexcept {
		return value <= v.value;
	}
	constexpr bool operator>(date v) const noexcept {
		return value > v.value;
	}
	constexpr bool operator>=(date v) const noexcept {
		return value >= v.value;
	}
	explicit constexpr operator bool() const noexcept {
		return value != uint16_t(0);
	}

	date operator+(int32_t v) const noexcept {
		return date{ uint16_t(value + v - 1) };
	}
	date& operator+=(int32_t v) noexcept {
		value = uint16_t(value + v);
		return *this;
	}

	constexpr int32_t to_raw_value() const noexcept {
		return int32_t(value);
	}
	year_month_day to_ymd(absolute_time_point base) const noexcept;
};



bool is_playable_date(date d, absolute_time_point start, absolute_time_point end);
int32_t days_difference(year_month_day start, year_month_day end);

}
